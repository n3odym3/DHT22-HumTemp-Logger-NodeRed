#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"
#include "wifi.h"
DHTesp dht;

#define DHTPIN 14 //For WemosD1 mini D5 = GPIO 14
int Delay = 1000 * 60 * 5; //5Min delay

unsigned int prevmillis;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HostName);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(HostName, mqttUser, mqttPassword)) {
      Serial.println("connected");
      //client.subscribe(topic_hum);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void MeasureDHT() {
  String Hstring = String(dht.getHumidity());
  String Tstring = String(dht.getTemperature());
  client.publish(topic_hum, Hstring.c_str());
  Serial.println(Hstring);
  client.publish(topic_temp, Tstring.c_str());
  Serial.println(Tstring);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dht.setup(DHTPIN, DHTesp::DHT22); 
  reconnect();
  MeasureDHT();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if ((millis() - prevmillis) > Delay) {
    MeasureDHT();
    prevmillis = millis();
  }
}
