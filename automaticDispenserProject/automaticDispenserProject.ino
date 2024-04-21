#include <ESP8266WiFi.h>
#include <elapsedMillis.h>
#include <ThingSpeak.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Sensor Ultrasonik
#define TRIGGER_PIN   11 // Arduino pin tied to trigger pin on the ultrasonic1 sensor.
#define ECHO_PIN      12 // Arduino pin tied to echo pin on the ultrasonic1 sensor.
#define TRIGGER_PIN1  6  // Arduino pin tied to trigger pin on the ultrasonic2 sensor.
#define ECHO_PIN1     7  // Arduino pin tied to echo pin on the ultrasonic2 sensor.

//LCD
LiquidCrystal_I2C lcd(0x25, 16, 2);

//MILLIS
elapsedMillis setKomponenMillis;
elapsedMillis relayTangkiMillis;
elapsedMillis reservoirRelayOutputMillis;
elapsedMillis ultrasonikTangkiMillis;
elapsedMillis ultrasonikGelasMillis;
elapsedMillis LCDOutputMillis;
elapsedMillis thingspeakUploadMillis;

unsigned long setKomponenInterval           = 3000; //3000
unsigned long relayTangkiInterval           = 1000; //2000
unsigned long reservoirRelayOutputInterval  = 2000; //2000
unsigned long ultrasonikTangkiInterval      = 1000; //2000
unsigned long ultrasonikGelasInterval       = 500;  //2000
unsigned long LCDOutputInterval             = 1000; //10000
unsigned long thingspeakUploadInterval      = 5000; //2000

//WIFI connect
const char* ssid = " ";   // your network SSID (name)
const char* password = "";   // your network password
unsigned long myChannelNumber = ;
const char* myWriteAPIKey = "";
WiFiClient  client;


//Pin Relay Pompa
int pinRelay1 = ;
int pinRelay2 = D5;

//Pin Sensor IR
int piniRSensor ;

//Variabel nilai ultrasoni
float duration_us, distance_cm;//ultrasonik tangki
float duration_us1, distance_cm1;//ultrasonik gelas

void setup() {
  Serial.begin(115200);  //Initialize serial
  WiFi.begin(ssid, password);

  //Inisiasi pin ir sensor
  pinMode(iRSensor, INPUT);
  //inisiasi pin Ultrasonik tangki
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //Inisiasi pin Ultrasonik gelas
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  //inisiasi pin relay pompa
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);
  delay(2000);
  Serial.println("Inisiasi Sensor = DONE");

  //SETUP LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("UKURAN AIR: ");
  delay(1000);
  Serial.println("LCD = DONE");

  //Wifi setup
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  //
  pendeteksianGelas();
  Tangki();
  delay(20);
  penampilanLayarLCD();
  pengirimanDatatoThingspeak();

}


void Tangki() {
  if (ultrasonikTangkiMillis >= ultrasonikTangkiInterval) {
    // generate 10-microsecond pulse to TRIG pin
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // measure duration of pulse from ECHO pin
    duration_us = pulseIn(echoPin, HIGH);

    // calculate the distance
    distance_cm = (duration / 2) / 29.1;

    pengisianAirTangki();

    // print the value to Serial Monitor
    Serial.print("distance: ");
    Serial.print(distance_cm);
    Serial.print(" cm \t");

    ultrasonikTangkiMillis = 0;
  }

}

void pengisianAirTangki() {
  if (relayTangkiMillis >= relayTangkiInterval) {
    //Air dalam tangki dispenser kurang
    if (distance_cm >= 20) {
      digitalWrite(pinRelay1, HIGH);
    } else if (distance_cm <= 2) {
      digitalWrite(pinRelay1, LOW);
    }

    relayTagkiMillis = 0;
  }

}

void penampilanLayarLCD() {

  if(LCDOutputMillis >= LCDOutputInterval){
    lcd.setCursor(3, 0);
    lcd.print(t);
    lcd.print(char(223));
    lcd.print("C");
    lcd.setCursor(13, 0);
    lcd.print(h);
    lcd.print("%");
    lcd.setCursor(9, 1);
    lcd.print(nilaimapping);
    lcd.print("%");

    
    LCDOutputMillis = 0;
  }

}

void pengirimanDatatoThingspeak() {
  if (thingspeakUpload >= thingspeakUploadInterval) {
    // Write to ThingSpeak. There are up  to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeField(myChannelNumber, 1, distance_cm, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    thingspeakUpload = 0;
  }


}

void pendeteksianGelas() {

}
