#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#define buzzer 13
LiquidCrystal_I2C lcd(0x27,20,4);
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
 SoftwareSerial mySerial(14,12);
#else
#define mySerial Serial1
#endif
//...................Db............>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define WIFI_SSID "D"
#define WIFI_PASSWORD "DRC@a1b2c3"
#define API_KEY "AIzaSyA9PEPm6-ouv35-5SwoPjXUmsK-V5lT9G4"
#define DATABASE_URL "hall-management-7edf6-default-rtdb.firebaseio.com"
#define USER_EMAIL "testmail@testmail.com"
#define USER_PASSWORD "1234567890"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
//..............end DB..............<

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

void setup()
{
  pinMode(buzzer, OUTPUT);
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello Mr. DUETian!");
  lcd.setCursor(2,1);
  lcd.print("....!");
  
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.setCursor(0,2);
    lcd.print("Sensor Ok....!");
  } 
  else {
    Serial.println("Did not find fingerprint sensor :(");
    while (!finger.verifyPassword()) { delay(250); lcd.setCursor(2,2);
      lcd.print("Searching Sensor...!");}
    }
  lcd.clear();
  EEPROM.begin(1);
  //.....................DB.............................
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  lcd.setCursor(0,2);
  lcd.print("Connecting network..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL; 
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  lcd.setCursor(0,2);
  lcd.print("Wifi Connected...!");
  delay(1000);
  lcd.clear();
  //.........................DB........................
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
  
}
uint8_t enrolledID;
void buz(int t){ digitalWrite(buzzer,1); delay(t);digitalWrite(buzzer,0);}
String s1,s2;
void loop()                     // run over and over again
{
  enrolledID=0;
  lcd.setCursor(0,1);
  lcd.print("Ready to enroll...");
  delay(150);
  enrolledID=getFingerprintID();
  while(finger.getImage() == FINGERPRINT_NOFINGER)
  {delay(200);}

  if (enrolledID==0)
    {
      lcd.clear();
    }
  else if (enrolledID==1 || enrolledID==2)
    {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Admin enroll...");
      buz(300);
      delay(100);
      buz(300);
      delay(100);
      buz(300);
      delay(100);
      buz(300);
      delay(100);
        id=EEPROM.read(0);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("press new std finger");
        delay(2000);
        while (!  getFingerprintEnroll() );

    }
  else if(enrolledID>2 && enrolledID<99)
  {
    
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("...ID Found...");
    lcd.setCursor(0,2);
    lcd.print("ID# ");
    lcd.setCursor(4,2);
    lcd.print(enrolledID);
//    String a="test";
    if (Firebase.ready() )
    {
      int temp,tmp2;
      
      switch(enrolledID){
        case 3:
          s2= Firebase.RTDB.getString(&fbdo, "/daily/3") ? String(fbdo.stringData()).c_str() : fbdo.errorReason().c_str();
          Serial.println(s2);
          temp=s2[0] - '0';
          if(temp==0){
            
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("Meal Empty!");
            buz(900);
            break;
          }
          else {
            temp--;
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("Go get your Meal!");
            buz(250);
            delay(100);
            buz(250);
            delay(100);
          }
          s1= temp;s1=s1 + s2[1] + s2[2];
          Serial.println(s1);
          Serial.printf("Set int... %s\n", Firebase.RTDB.setString(&fbdo, "/daily/3", s1) ? "ok" : fbdo.errorReason().c_str());
          break;
        case 4:
          Serial.printf("Set int... %s\n", Firebase.RTDB.setString(&fbdo, "/test/4", String(enrolledID)) ? "ok" : fbdo.errorReason().c_str());
          break;
        default:
          Serial.printf("Set int... %s\n", Firebase.RTDB.setString(&fbdo, "/test/default", String(enrolledID)) ? "ok" : fbdo.errorReason().c_str());
        
      }
    }
    
    delay(2000);
    lcd.clear();
  }
  else
  {
    
  }
  // delay(500);
  lcd.clear();
  // id = readnumber();
  // if (id == 0) {
  //    return;
  // }

  // while (!  getFingerprintEnroll() );
}


uint8_t getFingerprintID() {
  delay(100);
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error-1");
      p = finger.getImage();
      if (p==FINGERPRINT_PACKETRECIEVEERR)
        return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("error 250: No finger");
      return 100;
  }

  // OK success!
  delay(100);
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error-2");
      p = finger.getImage();
      p = finger.image2Tz();
      if (p==FINGERPRINT_PACKETRECIEVEERR)
        return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error 251");
      delay(100);
      return 251;
  }

  // OK converted!
  delay(100);
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error-3");
    return 252;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Did not find a match");
//    buz(100);
    delay(100);
    // lcd.clear();
    return 252;
  } else {
    Serial.println("Unknown error 252");
    return 252;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      delay(50);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return 251;
  }

  Serial.println("Remove finger");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(0,2);
  lcd.print("Same finger again plz");
  delay(1000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } 
  else {
    Serial.println("Unknown error");
    // lcd.clear();
    lcd.setCursor(0,3);
    lcd.print("Error to capture");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("....ok....");
    lcd.setCursor(1,1);
    lcd.print("ID# ");
    lcd.setCursor(6,1);
    lcd.print(id);
    lcd.setCursor(0,2);
    lcd.print("....done....");
    EEPROM.write(0, id+1);
    EEPROM.commit();
    delay(1500);
    lcd.clear();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
