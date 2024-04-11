#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <HTTPClient.h>

//Adafruit IO credentials
#define IO_USERNAME "iotprojectshohamtom"
#define IO_KEY "aio_bqyj30K7tKqCNGXFnW8EjeZqAdyQ"

//Pin for the button
#define buttonPin 33

//WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

//Pin for the DHT sensor
const int DHT_PIN = 15; 

//Create a DHT object
DHT dht(DHT_PIN, DHT22);

//Create a WiFi client
WiFiClient client;

//Create an MQTT client
Adafruit_MQTT_Client mqtt(&client, "io.adafruit.com", 1883, IO_USERNAME, IO_KEY);

//Create an MQTT feed for publishing data
Adafruit_MQTT_Publish Feed = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/temperature");

Adafruit_MQTT_Subscribe Indicator = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/indicator");

//Create a PubSubClient for MQTT
PubSubClient mqttClient(client);

void setup() {
  //Start serial communication
  Serial.begin(115200);

  //Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  //Attempt to connect to Adafruit IO
  int attempts = 0;
  while (!mqtt.connected() && attempts < 10) {
    Serial.println("Connecting to Adafruit IO...");
    mqtt.connect();
    attempts++;
    delay(5000); 
  }

  //Check if connected to Adafruit IO
  if (!mqtt.connected()) {
    Serial.println("Failed to connect to Adafruit IO!");
    return;
  }

  Serial.println("Adafruit IO connected");

  //Initialize the DHT sensor
  dht.begin();

  //Set the button pin as input
  pinMode(buttonPin, INPUT);

  mqtt.subscribe(&Indicator);

}

void loop() {
  //Maintain MQTT connection
  mqttClient.loop();

  //Check if the button is pressed
  if (digitalRead(buttonPin) == HIGH) {
    //Read temperature from DHT sensor
    float temperature = dht.readTemperature();

    //Check if reading is successful
    if (isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    //Convert temperature to a string
    String message = String(temperature);

    //Publish temperature to Adafruit IO feed
    if (Feed.publish(message.c_str())) {
        mqtt.publish(IO_USERNAME "/feeds/indicator", "1");
    }   

    //Delay to avoid multiple readings
    delay(3000); 
    mqtt.publish(IO_USERNAME "/feeds/indicator", "0");

  }
}
