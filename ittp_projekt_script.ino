/*
 * Dieses Beispiel implementiert einen MQTT-Client mit Hilfe der PubSubClient-Library. Ergänzend zu den online verfügbaren Beispielen
 * werden hier Zugangsdaten für den MQTT-Broker verwendet. Die Beispieldaten funktionieren bei unserem Projekt (kann herausgegeben werden),
 * sofern die WLAN-Verbindung korrekt konfiguriert wurde.
 * 
 * Das Beispiel beinhaltet sowohl MQTT-Subscribe als auch MQTT-Publish.
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// forward declarations
void setup_wifi();
void callback(String topic, byte* message, unsigned int length);
void reconnect();

// WiFi-Zugangsdaten
const char* ssid = "BS-GAST";
const char* passphrase = "bs2020#";

// MQTT-Serverdaten
const char* mqtt_server = "mqtt.devmgr.de";
const int mqtt_port = 1883;
const char* mqtt_user = "prototype";
const char* mqtt_password = "liftoff";
const char* mqtt_client_name = "ESP8266-Client-Delta";  // beliebig wählbar, muss aber pro Client einmalig sein (wegen Mosquitto)

const char* topic_sub = "prototype/liftoff-temp-aussen-1";
char  messageTemp;
int baselineTemp = 0;
int celsius = 0;
int fahrenheit = 0;

WiFiClient wifi;
PubSubClient client(wifi);


/* ---------- main functions ---------- */

/**
 * setup
 */
void setup() {
  // Serial
  Serial.begin(9600);
  Serial.println();
  delay(100);
  Serial.println("Setup...");
  pinMode(A0, INPUT);
  pinMode(2, OUTPUT);

  // WiFi
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Setup abgeschlossen");
}


/**
 * loop
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

    celsius = map(((analogRead(A0) - 20) * 3.04), 0, 1023, -40, 125);
    Serial.print(celsius);
    Serial.print(" C, ");
    Serial.print(messageTemp);
}


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
 * callback
 */
void callback(String topic, byte* message, unsigned int length)
{
  // Callback-Funktion für MQTT-Subscriptions
  
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Message byteweise verarbeiten und auf Konsole ausgeben
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
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
 * reconnect
 */
void reconnect()
{
  // Wiederverbinden mit dem MQTT-Broker
  while (!client.connected())
  {
    Serial.println("MQTT reconnect");
    if (client.connect("ESP8266Client-delta", mqtt_user, mqtt_password))
    {
      Serial.println("connected");
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
