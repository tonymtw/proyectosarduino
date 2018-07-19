#include <SoftwareSerial.h>

#include "Wire.h"  // Incluye la librerÃ­a Wire
#include "RTClib.h" // Incluye la librerÃ­a RTClib
#include <OneWire.h> 


//configuracion de la hora de luzdia_on y apagado de la luz diurna.
int horaini = 1030;
int horafin = 2200;
//configuracion de la hora de luzdia_on y apagado de la luz nocturna.
int horaledini = 2359;
int horaledfin = 1001;
//configuraciÃ³n de la hora de inicio del aireador.
int horaaireini = 2155;
int horaairefin = 0000;

int estadobt=0;

byte estadoLdia = 0;
byte estadoLnoche = 0;
byte estadoBoya = 0;
byte estadoAire = 0;

boolean luzdia_on = false;
boolean luznoche_on = false;
boolean avisoboya_on = false;
boolean aire_on = false;

boolean modomanual = false;

RTC_DS1307 RTC; // Crea el objeto RTC
int Relay = 2;
int luznoche = 4; //pin para encender el led.
int ledagua = 7; //pin para encender el led de aviso de falta de agua
int boya = 8; //pin para leer de la boya
int aireador = 9; //pin para endencider el rele del aire.
int ventilador = 10; //pin para encender el rele de los ventiladores.

//variables para el control de temperatura de la sonda
int DS18S20_Pin = 3; //DS18S20 Signal pin on digital 3 
float MAX_TMP = 29.00; 
byte estadoVentilador = 0;
boolean ventilador_on = false;

OneWire ds(DS18S20_Pin); // on digital pin 2
//bt
SoftwareSerial bt(11,12); 

/**
*Funcion que establece el estado inicial de los ventiladores cuando se hace un reset.
*/
void setestadoInicialVentiladores() {
  float temperature = getTemp();
  Serial.print("Temperatura agua: ");
  Serial.print(temperature);
  Serial.println("C");
  if (temperature > MAX_TMP) {
      Serial.println("Encender Ventiladores");
      encenderVentiladores();
      ventilador_on = true;
  }  else {
    Serial.println("Apagar Ventiladores");
     apagarVentiladores();
     ventilador_on = false;
  }  
}

/**
*Funcion que establece el estado inicial del aireador cuando se hace un reset.
*/
void setestadoInicialAire(int ahora) {
  Serial.println("Comprobando estado incial de aireador");
  Serial.println(ahora,DEC);
  
  if (ahora < horaaireini) {
    apagarAireador();
    aire_on = false;
    estadoAire = 0;
  } else if (ahora >= horaaireini && ahora < horaairefin) {
      encenderAireador();
      aire_on = true;
      estadoAire = 1;
   } else {
     apagarAireador();
     aire_on = false;
     estadoAire = 0;
   }
}

/**
*Funcion que establece el estado inicial de la luz diurna cuando se hace un reset.
*/
void setestadoInicialLuzDia(int ahora) {
  Serial.println("comprobando estado inicial luz dÃ­a");
  Serial.println(ahora,DEC);
   
   if (ahora < horaini) {
     apagarLuzDia();
     luzdia_on = false;
     estadoLdia = 0;
   } else if (ahora >= horaini && ahora < horafin) {
      encenderLuzDia();
      luzdia_on = true;
      estadoLdia = 1;
   } else {
     apagarLuzDia();
     luzdia_on = false;
     estadoLdia = 0;
   }
}

/**
*Funcion que establece el estado inicial de la luz nocturna cuando se hace un reset.
*/
void setestadoInicialLuzNoche(int ahora) {
  Serial.println("comprobando estadoLdia inicial luz noche");
  Serial.println(ahora,DEC);
   
   if (horaledini < horaledfin) {
     //pm
     if (ahora < horaledini) {
       apagarLuzNoche();
       luznoche_on = false;
       estadoLnoche = 0;
     } else if (ahora >= horaledini && ahora < horaledfin) {
        encenderLuzNoche();
        luznoche_on = true;
        estadoLnoche = 1;
     } else {
       apagarLuzNoche();
       luznoche_on = false;
       estadoLnoche = 0;
     }
   } else {
     //am
     if (ahora >= horaledfin && ahora < horaledini) {
        apagarLuzNoche();
       luznoche_on = false;
       estadoLnoche = 0;
     } else {
         encenderLuzNoche();
        luznoche_on = true;
        estadoLnoche = 1;
     } 
   }
}


void setestadoInicialBoya() {
  int lectura = digitalRead(boya);
  if (lectura == 0) {
     digitalWrite(ledagua, LOW);
    avisoboya_on = false;    
    estadoBoya = 0;
  } else {
    digitalWrite(ledagua, HIGH);
    avisoboya_on = true;    
    estadoBoya = 1;
   
  }
  
}

void setestadoInicial() {
  DateTime now = RTC.now();
  int hora = now.hour();
  int minutos = now.minute();
  int ahora = hora*100 + minutos;

   setestadoInicialLuzDia(ahora);
   setestadoInicialLuzNoche(ahora);
   setestadoInicialAire(ahora);
   setestadoInicialBoya();
   setestadoInicialVentiladores();
 }


void estadoLdia0 () {
    Serial.println("estadoLdia 0");
   DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC

   int hora = now.hour();
   int minutos = now.minute();
   
   int ahora = hora*100 + minutos;
   Serial.println(ahora);
   
   if (ahora == horaini) {
      encenderLuzDia();
      estadoLdia = 1;
      luzdia_on = true;
   }   
   return;
}

void estadoLdia1 () {
  Serial.println("estadoLdia 1");
   DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
     int hora = now.hour();
   int minutos = now.minute();
   
    int ahora = hora*100 + minutos;
   Serial.println(ahora);
   
   if (ahora == horafin) {
      apagarLuzDia();
      estadoLdia = 0;
      luzdia_on = false;
   }  
  return;
}



void estadoAire0() {
    Serial.println("estadoAire 0");
   DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC

   int hora = now.hour();
   int minutos = now.minute();
   
   int ahora = hora*100 + minutos;
   Serial.println(ahora);
   
   if (ahora == horaaireini) {
      encenderAireador();
      estadoAire = 1;
      aire_on = true;
   }   
   return;
}
void estadoAire1 () {
  Serial.println("estadoAire 1");
   DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
     int hora = now.hour();
   int minutos = now.minute();
   
    int ahora = hora*100 + minutos;
   Serial.println(ahora);
   
   if (ahora == horaairefin) {
      apagarAireador();
      estadoAire = 0;
      aire_on = false;
   }  
  return;
}


void estadoLnoche0 () {
    Serial.println("estadoLnoche 0");
   DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC

   int hora = now.hour();
   int minutos = now.minute();
   
   int ahora = hora*100 + minutos;
   Serial.println(ahora);
   
   if (ahora == horaledini) {
      encenderLuzNoche();
      estadoLnoche = 1;
      luznoche_on = true;
   }   
   return;
}

void estadoLnoche1 () {
  Serial.println("estadoLnoche 1");
   DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
     int hora = now.hour();
   int minutos = now.minute();
   
    int ahora = hora*100 + minutos;
   Serial.println(ahora);
   
   if (ahora == horaledfin) {
      apagarLuzNoche();
      estadoLnoche = 0;
      luznoche_on = false;
   }  
  return;
}


void estadoBoya1() {
  int lectura = digitalRead(boya);
  Serial.println("estado boya: " + lectura);
  if (lectura == 0) {
    digitalWrite(ledagua, LOW);
    avisoboya_on = false;    
    estadoBoya = 0;
  }
  //control por si la boya falla 
  if (!lectura) {
    Serial.println("ERROR en lectura de BOYA nivel");
    digitalWrite(ledagua, LOW);
    avisoboya_on = false;    
    estadoBoya = 0;
  }
}

void estadoBoya0() {
  int lectura = digitalRead(boya);
   Serial.println("estado boya: " + lectura);
  if (lectura == 1) {
    digitalWrite(ledagua, HIGH);
    avisoboya_on = true;    
    estadoBoya = 1;
  }
  //control por si la boya falla 
    if (!lectura) {
    Serial.println("ERROR en lectura de BOYA nivel");
    digitalWrite(ledagua, LOW);
    avisoboya_on = false;    
    estadoBoya = 0;
  } 
}

void estadoVentiladores0() {
  float temperature = getTemp();
  Serial.print("Temperatura agua: ");
  Serial.print(temperature);
  Serial.println("C");
  if (temperature > MAX_TMP && ventilador_on == false) {
        Serial.println("Encender Ventiladores");
        encenderVentiladores();
        ventilador_on = true;
        estadoVentilador = 1;      
  } 
}

void estadoVentiladores1() {
  float temperature = getTemp();
  Serial.print("Temperatura agua: ");
  Serial.print(temperature);
  Serial.println("C");
  if (temperature <= MAX_TMP && ventilador_on) {
        Serial.println("Encender Ventiladores");
        apagarVentiladores();
        ventilador_on = false;
        estadoVentilador = 0;      
  } 
}



void encenderLuzDia() {
  digitalWrite(Relay, HIGH);   //Realy Close
}

void apagarLuzDia() {
   digitalWrite(Relay, LOW);    //Relay Open
}


void encenderLuzNoche() {
  digitalWrite(luznoche, HIGH);   //Realy Close
}

void apagarLuzNoche() {
   digitalWrite(luznoche, LOW);    //Relay Open
}

void encenderAireador() {
  digitalWrite(aireador, HIGH);   //Realy Close
}

void apagarAireador() {
   digitalWrite(aireador, LOW);    //Relay Open
}


void encenderVentiladores() {
  digitalWrite(ventilador, HIGH);   //Realy Close
}

void apagarVentiladores() {
   digitalWrite(ventilador, LOW);    //Relay Open
}



void setup () {
  
  
  
  pinMode(Relay, OUTPUT);     //define PIN0 port as output for rele
  pinMode(luznoche, OUTPUT);  //define PIN3 port as output for blue led.
  
  pinMode(boya, INPUT); //DEFINE pin 8 port as input for boya
 // pinMode(DS18S20_Pin,INPUT); //ES no se si hay que definirlo.????????????????????
  pinMode(ledagua, OUTPUT); //Definepin 7 as output for led red.
  pinMode(aireador,OUTPUT ); //Define el pin 9 as output for aireador
  pinMode(ventilador,OUTPUT); //Define el pin n as output for ventilador.
  
  digitalWrite(Relay, LOW);    //Relay Open
  digitalWrite(luznoche, LOW); //led apagado.
  digitalWrite(ledagua, LOW);
  digitalWrite(aireador, LOW);//aireador apagado.
  digitalWrite(ventilador, LOW); //ventiladores apagados
  
  Serial.begin(9600); // Establece la velocidad de datos del puerto serie
  
  Wire.begin(); // Establece la velocidad de datos del bus I2C
  RTC.begin(); // Establece la velocidad de datos del RTC
  if (! RTC.isrunning()) {
  //RTC.adjust(DateTime(__DATE__, __TIME__)); //ajusta el RTC con la fecha y hora de la compilacion.
  }

  //bt
   bt.begin(9600);
   
  setestadoInicial();
}





float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius
 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
     //no more sensors on chain, reset search
     ds.reset_search();
     return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
     Serial.println("CRC is not valid!");
     return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
     Serial.print("Device is not recognized");
     return -1000;
 }
 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);    
 ds.write(0xBE); // Read Scratchpad

 for (int i = 0; i < 9; i++) { // we need 9 bytes
   data[i] = ds.read();
 }
 
 ds.reset_search();
 byte MSB = data[1];
 byte LSB = data[0];
 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 return TemperatureSum;
 }





void loop () {

//proceso normal
    modomanual = false;
  
   Serial.println(estadoLdia);
   //comprobamos luz dÃ­a
 if (estadoLdia == 0) {
   Serial.println("Dentro de estadoLdia 0");
   estadoLdia0();
 } else if (estadoLdia == 1) {
   Serial.println("Dentro de estadoLdia 1");
    estadoLdia1();
 }
 //comprobamos luz noche
 if (estadoLnoche == 0) {
   Serial.println("Dentro de estadoLdia 0");
   estadoLnoche0();
 } else if (estadoLnoche == 1) {
   Serial.println("Dentro de estadoLdia 1");
    estadoLnoche1();
 }
 //comprobamos aireador
 if (estadoAire == 0) {
   Serial.println("Dentro de estadoLdia 0");
   estadoAire0();
 } else if (estadoAire == 1) {
   Serial.println("Dentro de estadoLdia 1");
    estadoAire1();
 }
 //comprobamos boya de nivel.
 if (estadoBoya == 1) {
   estadoBoya1();
 } else if (estadoBoya == 0) {
   estadoBoya0();
 }
 
 //comprobamos temperatura.
 if (estadoVentilador == 1) {
   estadoVentiladores1();
 } else if (estadoVentilador == 0) {
   estadoVentiladores0();
 }
 


//El recibir o no cosas del bt marca el flujo
 if(bt.available()>0){
    modomanual = true;
    estadobt = bt.read();
     if (estadobt =='1'){
      encenderLuzDia();
     }
     if (estadobt =='2'){
      apagarLuzDia();
     }
     if (estadobt =='3'){
      encenderAireador();
     }
     if (estadobt =='4'){
      apagarAireador();
     }
     if (estadobt =='5'){
      encenderLuzNoche();
     }
     if (estadobt =='6'){
      apagarLuzNoche();
     }
     //delay(10000);
 }
  delay(1000); // La informaciÃ³n se actualiza cada 1 seg.

}

