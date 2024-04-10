#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_SSD1306.h>

#define LED_PIN 12
#define WLAN_SSID "Wokwi-GUEST"
#define WLAN_PASS ""
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "iotprojectshohamtom"
#define AIO_KEY         "aio_uBIU10WZjez1pF24ooq35f4Y8cW6"
#define AIO_FEED_1      "/feeds/textnew"
#define AIO_FEED_2      "/feeds/text1"

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish text_display = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME AIO_FEED_1);

Adafruit_MQTT_Subscribe text_form = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME AIO_FEED_2, MQTT_QOS_1);

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected())  
  {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000);
    retries--;
    if (retries == 0) 
    {
      while (1);
    }
  }
  Serial.println("MQTT Connected! We can start NOW");
}

void textformcallcack(char *data, uint16_t len) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Received Message:");
    display.println(data);
    display.display();

}


void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(); Serial.println("WiFi connected");
  text_form.setCallback(textformcallcack);
  mqtt.subscribe(&text_form);
}
 

void loop() {
  int i=0;
  while (1)
  {
    i++;
    MQTT_connect();
    mqtt.processPackets(10000);
   if(! mqtt.ping()) {
    mqtt.disconnect();
   }

    char s[30];
    sprintf(s,"Hello From Wokwi %d", i);
    Serial.println("Sending data...");
    text_display.publish(s);
    delay(10000);
}
}
