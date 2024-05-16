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
//IRSENSOR
#define iRSensor1  9;
#define iRSensor2  8;
//Relay
#define pinRelay1 5;


//LCD
LiquidCrystal_I2C lcd(0x25, 16, 2);

//MILLIS
elapsedMillis setKomponenMillis;
elapsedMillis ultrasonikTangkiMillis;
elapsedMillis ultrasonikGelasMillis;
elapsedMillis GelasMillis;
elapsedMillis LCDOutputMillis;
elapsedMillis thingspeakUploadMillis;

unsigned long setKomponenInterval           = 3000; //3000
unsigned long ultrasonikTangkiInterval      = 500; //2000
unsigned long ultrasonikGelasInterval      = 1000; //2000
unsigned long GelasInterval      = 300;  //2000
unsigned long LCDOutputInterval             = 1000; //10000
unsigned long thingspeakUploadInterval      = 5000; //2000

//WIFI connect
const char* ssid = " ";   // your network SSID (name)
const char* password = "";   // your network password
unsigned long myChannelNumber = 1;
const char* myWriteAPIKey = "";
WiFiClient  client;

//Pin Relay Pompa
//int pinRelay1 = D5;

//Pin Sensor IR
//int iRSensor1 = D9;
//int iRSensor2 = D8;

/*Pin LED
  int pinLEDRed = ;
  int pinLEDGreen = ;
*/

//Variabel nilai ultrasonik
float duration_us, distance_cm;//ultrasonik tangki
float duration_us1, distance_cm1;//ultrasonik gelas

//Variabel lainnya
int batesAirHabis = 20; //tentukan bates tinggi air saat air habis dalam ukuran cm
bool pompaState = false;
bool scanningGelas = true;
int batesGelas; //cm
String ukuranGelas;

void setup() {
  Serial.begin(115200);  //Initialize serial
  WiFi.begin(ssid, password);

  //SETUP LCD
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Memulai");

  //Inisiasi pin ir sensor
  pinMode(iRSensor1, INPUT);
  pinMode(iRSensor2, INPUT);

  //inisiasi pin Ultrasonik tangki
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //Inisiasi pin Ultrasonik gelas
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  //inisiasi pin relay pompa
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);

  //inisiasi pin LED
  //pinMode(pinLEDRed, OUTPUT);
  //pinMode(pinLEDGreen, OUTPUT);

  for (int i = 0; i < 4; i++) {
    lcd.print(".");
    delay(1000);
  }
  Serial.println("Inisiasi Sensor = DONE");
  lcd.setCursor(0, 1);
  lcd.print("Sensor Done");
  delay(1500);
  lcd.clear;

  //Wifi setup
  lcd.setCursor(0, 0);
  lcd.print("Menghubungkan");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi...");
    lcd.print(".");
    delay(1000);
  }
  Serial.println("Connected to WiFi");
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  lcd.setCursor(0, 1);
  lcd.print("Terhubung :)");
  lcd.clear;
  delay(100);

  lcd.setCursor(0, 0);
  lcd.print("Air : ");

}

void loop() {
 
  Tangki();
  delay(20);
  penampilanLayarLCD();
  if (iRSensor1 == 1 && pompaState) {
    pengukuranAirGelas();
    Gelas();
  } else {
    scanningGelas = true;
    digitalWrite(pinRelay1, LOW); //pompa Mati
  }
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
    distance_cm = (duration_us / 2) / 29.1;

    // print the value to Serial Monitor
    Serial.print("distance: ");
    Serial.print(distance_cm);
    Serial.print(" cm \t");


    ultrasonikTangkiMillis = 0;
  }

}


void penampilanLayarLCD() {

  if (LCDOutputMillis >= LCDOutputInterval) {
    //Penampilan Tangki
    if (distance_cm >= batesAirHabis) {
      lcd.setCursor(6, 0);
      lcd.print("HABIS");
      //digitalWrite(ledRed,HIGH);
      pompaState = false;

    } else {
      //Print hasil ke LCD
      lcd.setCursor(6, 0);
      lcd.print(distance_cm);
      lcd.print(" cm");
      pompaState = true;
      //digitalWrite(ledGreen,HIGH);
    }

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

void pengukuranAirGelas() {
  if (ultrasonikGelasMillis >= ultrasonikGelasInterval) {
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);

    // measure duration of pulse from ECHO pin
    duration_us1 = pulseIn(echoPin1, HIGH);

    // calculate the distance
    distance_cm1 = (duration_us1 / 2) / 29.1;

    // print the value to Serial Monitor
    Serial.print("distance: ");
    Serial.print(distance_cm1);
    Serial.print(" cm \t");

    ultrasonikGelasMillis = 0;
  }

}

void Gelas() {

  if (GelasMillis >= GelasInterval) {
    //memastikan gelas
    if (scanningGelas) {
      lcd.setCursor(0, 1);
      lcd.print("Tahan");
      for (int i = 0; i < 4; i++) {
        lcd.print(".");
        delay(1000);
      }

      //Penentuan Ukuran gelas
      if (IRSensor1 == 1) {
        scanningGelas = false;

        //penentuan ukuran Gelas
        if (IRSensor2 == 1) {
          ukuranGelas = "TINGGI";
          batesGelas = 10;
        } else {
          ukuranGelas = "Rendah";
          batesGelas = 5;
        }

        lcd.print("DONE");
      } else {
        lcd.print("GGL");
        delay(1000);
      }
      lcd.setCursor(0, 1);
      lcd.print("               ");
      delay(200);
    }

    //pengisian air
    if (distance_cm1 <= batesGelas) {
      digitalWrite(pinRelay1, LOW); //pompa Mati
      lcd.setCursor(0, 1);
      lcd.print("Selesai :)")
      

    } else {
      digitalWrite(pinRelay1, HIGH);//pompa Nyala
      lcd.setCursor(0, 1);
      lcd.print("Mengisi");
    }
    GelasMillis; = 0;
  }

}
