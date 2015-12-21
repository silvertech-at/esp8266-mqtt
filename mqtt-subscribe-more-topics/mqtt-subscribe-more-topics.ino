#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// LED's
int ledgruen = 13;
int ledrot = 15;

// WLAN-MQTT Einstellungen
const char* ssid = "wifissid";
const char* password = "wifipassword";
const char* mqtt_server = "mqtt-broker.home.local";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  
  // LED auf OUTPUT setzen
  pinMode(ledgruen, OUTPUT);
  pinMode(ledrot, OUTPUT);
  digitalWrite(ledrot, LOW);
  digitalWrite(ledgruen, LOW);
  Serial.begin(115200);

  // WLAN Verbinden
  setup_wifi();

  // MQTT starten
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Warten auf WLAN Verbindung
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // WLAN verbunden - Infos ausgeben
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT Subscribe Funktion
void callback(char* topic, byte* payload, unsigned int length) {

  // Hilfsvariablen um empfangene Daten als String zu behandeln
  int i = 0;
  char message_buff[100];

  // Epfangene Nachtricht ausgeben
  Serial.println("Message arrived: topic: " + String(topic));
  Serial.println("Length: " + String(length,DEC));

  // Nachricht kopieren
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
    }

  // Nachricht mit einen Bit abschließen und zu String umwandeln und ausgeben
  message_buff[i] = '\0';
  String msgString = String(message_buff);
  Serial.println("Payload: " + msgString);
  Serial.println("Topic: ");
  Serial.print(topic);

  if (String(topic) == "led/rot/set" && msgString == "on"){
    digitalWrite(ledrot, HIGH);
    client.publish("led/rot", "on");   
  }
  else if (String(topic) == "led/rot/set" && msgString == "off"){
    digitalWrite(ledrot, LOW);
    client.publish("led/rot", "off");
  }
  else if (String(topic) == "led/gruen/set" && msgString == "on"){
    digitalWrite(ledgruen, HIGH);
    client.publish("led/gruen", "on");
  }
  else if (String(topic) == "led/gruen/set" && msgString == "off"){
    digitalWrite(ledgruen, LOW);
    client.publish("led/gruen", "off");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Juhuu, verbunden
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      
      // in OutTopic Nachricht ausgeben
      client.publish("outTopic", "hello world");
      
      // subscribe auf folgende Topics
      client.subscribe("led/rot/set");
      client.subscribe("led/gruen/set");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // 5 Sekunden warten bis zu einen erneuten Verbinden
      delay(5000);
    }
  }
}
void loop() {

  // MQTT verbinden
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  // Alle 20 Sekunden eine Nachricht ausgeben
//  long now = millis();
//  if (now - lastMsg > 20000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, 75, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish("outTopic", msg);
//  }
}
