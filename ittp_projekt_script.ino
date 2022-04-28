/*
   Dieses Beispiel implementiert einen MQTT-Client mit Hilfe der PubSubClient-Library. Ergänzend zu den online verfügbaren Beispielen
   werden hier Zugangsdaten für den MQTT-Broker verwendet. Die Beispieldaten funktionieren bei unserem Projekt (kann herausgegeben werden),
   sofern die WLAN-Verbindung korrekt konfiguriert wurde.

   Das Beispiel beinhaltet sowohl MQTT-Subscribe als auch MQTT-Publish.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define Luefter1 D1
// forward declarations
void setup_wifi();
void callback(String topic, byte* message, unsigned int length);
void reconnect();

// WiFi-Zugangsdaten
const char* ssid = "IT-R222";
const char* passphrase = "LIFtOff!";

// MQTT-Serverdaten
const char* mqtt_server = "mqtt.devmgr.de";
const int mqtt_port = 1883;
const char* mqtt_user = "prototype";
const char* mqtt_password = "liftoff";
const char* mqtt_client_name = "ESP8266-Client";  // beliebig wählbar, muss aber pro Client einmalig sein (wegen Mosquitto)

// Topics für Subscription und eigenes Publishing
const char* topic_sub = "prototype/luefter-start-delta-kilian";
const char* topic_pub = "prototype/teamdelte-temp";

const char* startvent = "NO";

// Konstanten
const int pub_interval = 10000;  // Intervall für eigene pub-Nachrichten (in ms)


WiFiClient wifi;
PubSubClient client(wifi);

long now;  // current uptime
long last = 0;  // uptime of last publish

int pub_counter = 0;  // Zählwert als Inhalt für eigene Publish-Nachrichten


/* ---------- main functions ---------- */

/**
   setup
*/
void setup() {
  // Seria
  pinMode(D1, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(9600);

  Serial.println();
  delay(100);

  Serial.println("Setup...");

  // WiFi
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Setup abgeschlossen");
}


/**
   loop
*/
void loop() {
  if (!client.connected())
  {
    // MQTT-Verbindung wieder herstellen
    reconnect();
  }

  // regelmäßigen Nachrichtenempfang gewährleisten, false bei Verbindungsabbruch
  if (!client.loop())
  {
    client.connect(mqtt_client_name, mqtt_user, mqtt_password);
  }

  if(startvent == "YES") {
    digitalWrite(Luefter1, HIGH);
  }else{
    digitalWrite(Luefter1, LOW);
  }
  
  now = millis();
  if (millis() - last > pub_interval)
  {

    last = now;
    pub_counter++;
    const int temp = analogRead(A0);
    client.publish(topic_pub, String(temp).c_str());
    Serial.print("Publish: ");
    Serial.print(topic_pub);
    Serial.print(": ");
    Serial.println(pub_counter);
  }
}


/* ---------- other functions ---------- */

/**
   setup_wifi
*/
void setup_wifi()
{
  delay(10);
  Serial.print("WiFi");
  WiFi.begin(ssid, passphrase);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected");
}


/**
   callback
*/
void callback(String topic, byte* message, unsigned int length)
{
  // Callback-Funktion für MQTT-Subscriptions
  String messageTemp;
  String temp;


  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Message byteweise verarbeiten und auf Konsole ausgeben
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  };

   if (messageTemp == "NO"){
     Serial.print("Lüfter nicht starten");
     startvent = "NO";
    }else{
      Serial.print("Lüfter starten");
      startvent = "YES";
     }
  Serial.println();

  // Behandlung bestimmter Topics
  if (topic == topic_sub)
  {
    if (messageTemp == "on")
    {
      Serial.print(topic_sub);
      Serial.println(" steht auf EIN");
    }
    else if (messageTemp == "off")
    {
      Serial.print(topic_sub);
      Serial.println(" steht auf AUS");
    }
  }
  /* else if (topic == anderes_topic) { ... } */
}


/**
   reconnect
*/
void reconnect()
{
  // Wiederverbinden mit dem MQTT-Broker
  while (!client.connected())
  {
    //Serial.println("MQTT reconnect");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password))
    {
     // Serial.println("connected");
      client.subscribe(topic_sub);
    }
    else
    {
      Serial.print("failed: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}