#include <SPI.h>
#include <RFID.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

const byte PIN_LED_R = 3;
const byte PIN_LED_G = 4;
const byte PIN_LED_B = 5;

char DIO_Buzzer = 2;

RFID monModuleRFID(10, 9);

int UID[5];
int UID_Carte_Connu[5] = {176, 163, 255, 167, 75};

int sensorAnalogPin = A0;
int sensorDigitalPin = 7;
int analogValue = 0;
int digitalValue;

void displayColor(byte r, byte g, byte b)
{
  analogWrite(PIN_LED_R, r);
  analogWrite(PIN_LED_G, g);
  analogWrite(PIN_LED_B, b);
}

void setup()
{
  Wire.begin();
  Serial.begin(38400);

  displayColor(255, 255, 255);
  tone(DIO_Buzzer, 500, 100);
  Serial.println(" ");
  Serial.println("Initialisation du programme...");
  Serial.println(" ");
  delay(500);
  displayColor(0, 0, 0);

  Serial.println("Initialisation de l'acceléromètre...");
  Serial.println(" ");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connecté" : "La connexion a échoué");

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  displayColor(0, 0, 0);

  SPI.begin();
  monModuleRFID.init();

  pinMode(sensorDigitalPin, INPUT);
}

void loop()
{

  /* -------------------------------------------------- Accéléromètre + LED + Buzzer -------------------------------------------------- */

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  ax = map(ax, -18000, 18000, 0, 180);
  ay = map(ay, -18000, 18000, 0, 180);
  az = map(az, -18000, 18000, 0, 180);
  gx = map(gx, -18000, 18000, 0, 180);
  gy = map(gy, -18000, 18000, 0, 180);
  gz = map(gz, -18000, 18000, 0, 180);

  int vitesse;

  vitesse = az / 2;

  delay(500);

  Serial.print("Axe X-Y-Z : \t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t\t");
  Serial.print("Gyro X-Y-Z : \t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.print(gz); Serial.print("\t\t");
  Serial.print("Vitesse : \t");
  Serial.print(vitesse); Serial.print(" m/s \t\t");

  /*if (ax > 50) {
    displayColor(255, 0, 0);
    tone(DIO_Buzzer, 500, 50);
    } else {
    displayColor(0, 0, 0);
    noTone(DIO_Buzzer);
    }

    if (ax > 80) {
    displayColor(0, 255, 0);
    noTone(DIO_Buzzer);
    tone(DIO_Buzzer, 500, 50);
    } else {
    displayColor(0, 0, 0);
    noTone(DIO_Buzzer);
    }

    if (ax < 110) {
    displayColor(0, 0, 255);
    tone(DIO_Buzzer, 500);
    } else {
    displayColor(0, 0, 0);
    noTone(DIO_Buzzer);
    }*/

  /*if (ax > 150) {
    displayColor(0, 0, 255);
    noTone(DIO_Buzzer);
    tone(DIO_Buzzer, 500, 100);
    } else {
    displayColor(0, 255, 0);
    noTone(DIO_Buzzer);
    }

    if (ay > 150) {
    displayColor(255, 0, 0);
    noTone(DIO_Buzzer);
    tone(DIO_Buzzer, 500, 400);
    } else {
    }*/

  if ((az > 50) && (az < 100)) {
    displayColor(0, 255, 255);
    delay(200);
    noTone(DIO_Buzzer);
    tone(DIO_Buzzer, 500, 100);
  } else {
    displayColor(0, 0, 0);
    noTone(DIO_Buzzer);
  }

  if ((az > 100) && (az < 150)) {
    displayColor(255, 165, 0);
    delay(200);
    noTone(DIO_Buzzer);
    tone(DIO_Buzzer, 500, 400);
  } else {
    displayColor(0, 0, 0);
    noTone(DIO_Buzzer);
  }

  if (az > 150) {
    Serial.println("Trop haut ! \t");
    displayColor(0, 0, 255);
    delay(200);
    noTone(DIO_Buzzer);
    tone(DIO_Buzzer, 500, 800);
  } else {
    displayColor(0, 0, 0);
    noTone(DIO_Buzzer);
  }

  /* -------------------------------------------------- Sound Sensor -------------------------------------------------- */

  analogValue = analogRead(sensorAnalogPin);
  digitalValue = digitalRead(sensorDigitalPin);
  Serial.print("Son : \t");
  Serial.println(analogValue);

  if (digitalValue == HIGH) {
    Serial.println("Trop fort ! \t");
  }

  /* -------------------------------------------------- Module RFID -------------------------------------------------- */

  if (monModuleRFID.isCard()) {
    if (monModuleRFID.readCardSerial()) {
      Serial.println(" ");
      Serial.print("L'UID de votre carte est : ");
      for (int i = 0; i <= 4; i++)
      {
        UID[i] = monModuleRFID.serNum[i];
        Serial.print(UID[i], DEC);
        Serial.print(".");
      }
      Serial.println(" ");
    }
    if (UID[0] == UID_Carte_Connu[0] && UID[1] == UID_Carte_Connu[1] && UID[2] == UID_Carte_Connu[2] && UID[3] == UID_Carte_Connu[3] && UID[4] == UID_Carte_Connu[4]) {
      displayColor(0, 255, 0);
      tone(DIO_Buzzer, 500, 200);
      delay(1000);
      displayColor(0, 0, 0);
      noTone(DIO_Buzzer);
      delay(1000);
      Serial.println(" ");
      Serial.println("Accès autorisé");
      Serial.println(" ");
      delay(1000);
      Serial.println("Arrêt complet du programme");
      Serial.println(" ");
      delay(300);
      while (1);
    } else {
      displayColor(255, 0, 255);
      noTone(DIO_Buzzer);
      tone(DIO_Buzzer, 400);
      delay(500);

      noTone(DIO_Buzzer);
      tone(DIO_Buzzer, 400);
      Serial.println(" ");
      Serial.println("Accès refusé");
      Serial.println(" ");
      delay(750);

      noTone(DIO_Buzzer);
      tone(DIO_Buzzer, 400);
      Serial.println("Autodestruction du programme dans...");
      Serial.println(" ");
      delay(1000);

      noTone(DIO_Buzzer);
      tone(DIO_Buzzer, 400);
      Serial.println("3...");
      Serial.println(" ");
      delay(1000);

      noTone(DIO_Buzzer);
      tone(DIO_Buzzer, 400);
      Serial.println("2...");
      Serial.println(" ");
      delay(1000);

      noTone(DIO_Buzzer);
      tone(DIO_Buzzer, 400);
      Serial.println("1...");
      Serial.println(" ");
      delay(1000);

      noTone(DIO_Buzzer);
      tone(DIO_Buzzer, 500);
      Serial.println("BOOM !!!");
      Serial.println(" ");
      delay(500);
    }
    monModuleRFID.halt();
  }
  delay(1);

}

/*//bleu
    displayColor(255, 0, 0);

    //vert
    displayColor(0, 255, 0);

    //rouge
    displayColor(0, 0, 255);

    //rose
    displayColor(255, 0, 255);

    //jaune
    displayColor(0, 255, 255);

    //bleu clair
    displayColor(255, 255, 0);

    //blanc
    displayColor(255, 255, 255);*/
