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


//relativo al servidor de tiempos NTP
unsigned int localPort = 8888;       // local port to listen for UDP packets
char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


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

void getNTPData() {
  char msgBuffer[20];
  Udp.begin(localPort);
  sendNTPpacket(timeServer);
  delay(1000);
  if (Udp.parsePacket()) {
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears; //unix time

    //hora
    unsigned long hour = (epoch  % 86400L) / 3600;
    unsigned long minute = (epoch  % 3600) / 60;
    unsigned long second = epoch % 60;
    
    delay(10000);
    Ethernet.maintain();

    //lo publicamos.
    if(mqttClient.connect(CLIENT_ID)) {
      mqttClient.publish("test/_hora", dtostrf(hour*100+minute, 6, 2, msgBuffer));
    }
    
  }
}



// send an NTP request to the time server at the given address
void sendNTPpacket(char* address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
