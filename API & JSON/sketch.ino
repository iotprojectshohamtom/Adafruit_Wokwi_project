#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* apiKey = "SCHqV3MJnkhdl82oGW4CIA==gLwiBgKEuW50Yw2K";

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "iotprojectshohamtom"
#define AIO_KEY         "aio_bqyj30K7tKqCNGXFnW8EjeZqAdyQ"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe feed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/JSON_data");

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000); 
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

String extractTime(String json) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, json);
  const char* time = doc["time"];
  return String(time);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  
  display.display();
  delay(2000);
  display.clearDisplay();

  mqtt.subscribe(&feed); 
}

void loop() {
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &feed) {

      String city = (char*)feed.lastread;
      
      HTTPClient http;
  
      String url = "https://api.api-ninjas.com/v1/worldtime?city=" + String(city);
      
      http.begin(url);
      http.addHeader("X-Api-Key", apiKey);
      
      int httpResponseCode = http.GET();
      if (httpResponseCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
  
        String time = extractTime(payload);
        
        StaticJsonDocument<512> doc;
        deserializeJson(doc, payload.c_str());

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 10);
        display.println("Timezone: " + doc["timezone"].as<String>());
        display.setCursor(0, 30);
        display.println("Date: " + doc["date"].as<String>());
        display.setCursor(0, 40);
        display.println("Time: " + doc["hour"].as<String>() + ":" + doc["minute"].as<String>() + ":" + doc["second"].as<String>());
        display.display();
      } else {
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      
      delay(5000);
    }
  }
}
