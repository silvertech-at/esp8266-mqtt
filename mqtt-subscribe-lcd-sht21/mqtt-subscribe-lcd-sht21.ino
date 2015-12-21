#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <i2c_sht21.h>

// LCD Settings
LiquidCrystal_I2C lcd(0x3F,20,4);

// LED's
int ledgruen = 13;
int ledrot = 15;

// WLAN-MQTT Einstellungen
const char* ssid = "dd-wrt";
const char* password = "c9l8a7u6s5w4l3a2n1";
const char* mqtt_server = "192.168.1.106";

// SHT21 Settings
i2c_sht21 mySHT21;
#define SHT_address  0x40
float temperatur=0.0;
float humy=0.0;

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
  Wire.begin(2,4);
  mySHT21.init(SHT_address);

  // WLAN Verbinden
  setup_wifi();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Temperatur: ");
  lcd.setCursor(0,1);
  lcd.print("Luftfeuchte: ");

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

  if (String(topic) == "lcd/bus/1"){
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print(msgString);
  }
  else if (String(topic) == "lcd/bus/2"){
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print(msgString);
  }
//  else if (String(topic) == "led/gruen/set" && msgString == "on"){
//    digitalWrite(ledgruen, HIGH);
//    client.publish("led/gruen", "on");
//  }
//  else if (String(topic) == "led/gruen/set" && msgString == "off"){
//    digitalWrite(ledgruen, LOW);
//    client.publish("led/gruen", "off");
//  }
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
      client.subscribe("lcd/bus/1");
      client.subscribe("lcd/bus/2");
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
  
  // Alle 10 Sekunden messen
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    temperatur=mySHT21.readTemp();
    humy=mySHT21.readHumidity();
  
    Serial.print("Temperatur: ");
    Serial.println(temperatur);
  
    lcd.setCursor(12,0);
    lcd.print(temperatur);
    lcd.setCursor(18,0);
    lcd.print("C");
    
    Serial.print("Luftfeuchte: ");
    Serial.println(humy);
  
    lcd.setCursor(13,1);
    lcd.print(humy);
    lcd.setCursor(19,1);
    lcd.print("%");
  }

}
