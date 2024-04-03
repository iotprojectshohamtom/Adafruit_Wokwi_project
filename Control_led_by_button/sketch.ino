#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define LED_PIN 12
#define WLAN_SSID "Wokwi-GUEST"
#define WLAN_PASS ""
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "iotprojectshohamtom"
#define AIO_KEY         "aio_HvUp69c7y42l28TzugJjQyHL8uDd"
#define AIO_FEED_3      "/feeds/led"



WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe onoffbtn = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME AIO_FEED_3, MQTT_QOS_1);


void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
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
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void onoffcallback(char *data, uint16_t len) {
  Serial.println(data);
  if (strcmp(data, "ON") == 0) {
    Serial.println("Turning Led ON");
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("Turning Led OFF");
    digitalWrite(LED_PIN, LOW);
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(); Serial.println("WiFi connected");
  onoffbtn.setCallback(onoffcallback);
  mqtt.subscribe(&onoffbtn);
}

void loop() {
  MQTT_connect();
  mqtt.processPackets(10000);
   if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}