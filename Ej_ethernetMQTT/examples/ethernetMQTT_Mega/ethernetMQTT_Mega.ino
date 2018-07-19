/**
 * Programa para lectura de sensor DHT con arduino Meta y envio de mensajes MQTT broker
 * a traves de la ethernet shield ENC28J60. 
 * Tambien válido para modulo HanRun HR911105A
 * Fuente de inspiracion http://npanel.project.free.fr/wp/?p=607
 */
#include <DHT.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>


#define CLIENT_ID       "UnoMQTT"
#define INTERVAL        30000 // 30 sec delay between publishing
#define DHTPIN          3
#define DHTTYPE         DHT11

long previousMillis;

// Poner valores según tu red al shield ethernet
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte ip[]     = { 192, 168, 1, 66 };

//mosquitto address server
const char* mqtt_server = "192.168.1.7";
//byte mqtt_server[] = { 192, 168, 1, 7 };


//Objetos.
EthernetClient ethClient;
//PubSubClient mqttClient(mqtt_server, 1883, callback, ethClient);
PubSubClient mqttClient;
DHT dht(DHTPIN, DHTTYPE);

/*
 * // Callback function
void callback(char* topic, byte* payload, unsigned int length) {
   byte* pl = (byte*)malloc(length);
    memcpy(pl,payload,length);
     // Test du TOPIC
     if (strcmp(topic,"input/set")==0) {
          blink = !blink;
          if(blink) {
              digitalWrite(led, HIGH);
          } else {
              digitalWrite(led, LOW);
          }
          mqttClient.publish("output/system/","set");
      }
     // Test du PAYLOAD
     if (memcmp(pl, "comtest",length)==0) {
       blink = !blink;
       if(blink) {
          digitalWrite(led, HIGH);
        } else {
          digitalWrite(led, LOW);
        }
     }
     if (memcmp(pl, "ping",4)==0) {
        mqttClient.publish("output/system/","ok");
     }
 free(pl);
}
*/

void setup()
{
 //Static ip.
 //Ethernet.begin(mac, ip);
 
 //DHCP IP.
 if(Ethernet.begin(mac) == 0) {
      for(;;);
 }
  //setup mqtt broker
  mqttClient.setClient(ethClient);
  mqttClient.setServer(mqtt_server,1883);

  // setup DHT sensor
  dht.begin();
  previousMillis = millis();
 
}

void loop()
{
  //Chequear intervalo de envio de información.
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
    mqttClient.publish("test/_temperature1", dtostrf(t, 6, 2, msgBuffer));
    mqttClient.publish("test/_humidity1", dtostrf(h, 6, 2, msgBuffer));
  }
}


