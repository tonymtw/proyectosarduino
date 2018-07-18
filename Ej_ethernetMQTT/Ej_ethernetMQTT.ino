#include <DHT.h>
#include <UIPEthernet.h>
#include "PubSubClient.h"

#define CLIENT_ID       "UnoMQTT"
#define INTERVAL        3000 // 3 sec delay between publishing
#define DHTPIN          3
#define DHTTYPE         DHT11
bool statusKD=HIGH;//living room door
bool statusBD=HIGH;//front door
bool statusGD=HIGH;//garage door
int lichtstatus;
uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
//byte ipfixed[]={192,168,1,66};

//mosquitto address
const char* mqtt_server = "192.168.1.7";

EthernetClient ethClient;
PubSubClient mqttClient;
DHT dht(DHTPIN, DHTTYPE);

long previousMillis;


/*Pin connection Arduino Mega-Enc28j60*/
//Enc28j60   Arduino Mega 2560
//GND       GND
//3.3       3.3V
//SO        Pin50
//SI        Pin51
//SCK       Pin52
//CS        Pin53
/*fin*/


void setup() {
pinMode(4,INPUT_PULLUP);
pinMode(5,INPUT_PULLUP);
pinMode(6,INPUT_PULLUP);
  // setup serial communication
  //Serial.begin(9600);

  //setup ethernet communication using IP fixed.
  //Ethernet.begin(mac,ipfixed);

  
  // setup ethernet communication using DHCP
  if(Ethernet.begin(mac) == 0) {
    for(;;);
  }

  //en algunos sitios dice que para mega se tiene que llamar así pero con la librería EhernetCard, cosa que no funciona con MQTT
  //if (ethClient.begin(sizeof Ethernet::buffer, mac, 53) == 0) {
  //  for(;;);
  //}

  
  // setup mqtt client
  mqttClient.setClient(ethClient);
  mqttClient.setServer(mqtt_server,1883);
  //Serial.println(F("MQTT client configured"));

  // setup DHT sensor
  dht.begin();
  previousMillis = millis();
}

void loop() {
  statusBD=digitalRead(4);
  statusGD=digitalRead(5);
  statusKD=digitalRead(6);
  lichtstatus = analogRead(A0);
  // check interval
  if(millis() - previousMillis > INTERVAL) {
    sendData();
    previousMillis = millis();
  }
  mqttClient.loop();
}

void sendData() {
  char msgBuffer[20];
  float h=dht.readHumidity();
  float t = dht.readTemperature();
  if(mqttClient.connect(CLIENT_ID)) {
   mqttClient.publish("hal/temp", dtostrf(t, 6, 2, msgBuffer));
   mqttClient.publish("hal/humid", dtostrf(h, 6, 2, msgBuffer));
   mqttClient.publish("hal/door", (statusBD == HIGH) ? "OPEN" : "DICHT");
   mqttClient.publish("hal/garage",(statusGD == HIGH) ? "OPEN" : "DICHT");
   mqttClient.publish("hal/kamer",(statusKD == HIGH) ? "OPEN" : "DICHT");
   mqttClient.publish("hal/licht", dtostrf(lichtstatus, 4, 0, msgBuffer));
 //hal=hallway, DICHT=Closed, kamer=room, licht=light
 }
}
