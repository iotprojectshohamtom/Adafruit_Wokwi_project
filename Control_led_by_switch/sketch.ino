#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define LED_PIN 12
/************************* WiFi Access Point ***************************/
#define WLAN_SSID "Wokwi-GUEST"
#define WLAN_PASS ""
/************************* Adafruit.io Setup ***************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "iotprojectshohamtom"
#define AIO_KEY         "aio_tJym408WHNghnTMiQtlDD6vzffSs"
#define AIO_FEED_1      ""
#define AIO_FEED_3      "/feeds/led"



// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Publish slider = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME AIO_FEED_1);

// Setup a feed called 'onoff' for subscribing to changes to the ON/OFF button
Adafruit_MQTT_Subscribe onoffbtn = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME AIO_FEED_3, MQTT_QOS_1);

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000);  // wait 10 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void onoffcallback(char *data, uint16_t len) {
  Serial.print("Hey we're in an onoff callback, the button value is: ");
  Serial.println(data);
  if (strcmp(data, "ON") == 0) {
    Serial.println("Turning AC ON");
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("Turning AC OFF");
    digitalWrite(LED_PIN, LOW);
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // connecting to WIFI
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(); Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  Serial.println("Hello, ESP32!");

  onoffbtn.setCallback(onoffcallback);
  
  // Setup MQTT subscription for time feed.
  mqtt.subscribe(&onoffbtn);
}

void loop() {
     // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(10000);
   if(! mqtt.ping()) {
    mqtt.disconnect();
  }

  int num = random(0,101);
  Serial.print("num = ");
  Serial.println(num);
  slider.publish(num);
}