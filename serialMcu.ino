#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "sibop"; //your wi-fi ssid
const char* password = "wifirumah123"; //your wi-fi password
const char* mqtt_server = "52.188.125.251";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE];
int value = 0;

String arrData[2]={""};  //2 sensor

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("topic/azkira/mqtt/temperature", "temperature");
      client.publish("topic/azkira/mqtt/humidity", "humidity");
      // ... and resubscribe
      client.subscribe("topic/azkira/mqtt/temperature");
      client.subscribe("topic/azkira/mqtt/humidity");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() { // run over and over
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  String data = "";
  while (Serial.available()>0) {
    data += char(Serial.read());
    }
  int index = 0;
  for(int i=0; i<data.length(); i++)
  {
    char delimiter = '#';  //pemisah
    if(data[i] != delimiter)
      arrData[index] += data[i];
    else
      index++;
 
  }
  String temp = arrData[0];
  String hum = arrData[1];
  //Serial.println("Temperature: " + temp + " °C ");
  //Serial.println("Humidity: " + hum + " %");

  char outTmp[50];
  char outHum[50];

  temp.toCharArray(outTmp, 50);
  hum.toCharArray(outHum, 50);
  
  //unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, outTmp, value);
    snprintf (msg2, MSG_BUFFER_SIZE, outHum, value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    Serial.println(msg2);
    client.publish("topic/azkira/mqtt/temperature", msg);
    client.publish("topic/azkira/mqtt/humidity", msg2);
  }
  arrData[0]="";
  arrData[1]="";
  delay(15000);
}
