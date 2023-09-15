// UNO I2c with 16 channel PCA9685 Servo driver and 7 servo

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver myServo = Adafruit_PWMServoDriver();

int max6 = 445, min6 = 200, min5 = 50, max5 = 530, pos5, min = 0, max = 400;

void setup() {
  Serial.begin(9600) ;
  myServo.begin();
  myServo.setPWMFreq(50);

  pos5 = min5;

  myServo.setPWM(0, 0, min);
  myServo.setPWM(1, 0, min);
  myServo.setPWM(2, 0, min);
  myServo.setPWM(3, 0, min);
  myServo.setPWM(4, 0, min);
  myServo.setPWM(5, 0, min5);
  myServo.setPWM(6, 0, min6);

}

int grabs = 0;
void grab()
{
  if (grabs == 0)
  {
    for (int i = min6; i <= max6; i++) {
      myServo.setPWM(6, 0, i);
      delay(10);
      Serial.println("Grabing at: " + String(i));
    }
    grabs = 1;
  }
  else
  {
    for (int i = max6; i >= min6 ; i--) {
      myServo.setPWM(6, 0, i);
      delay(10);
      Serial.println(i);
    }
    grabs = 0;
  }

}

void motor6(int n)
{
  n = map(n, 0, 100, min5, max5);

  if (n > pos5)
  {
    for (int i = pos5; i != n; i++) {
      myServo.setPWM(5, 0, i);
      delay(10);
      Serial.println("motor6 at: " + String(i));
    }
    pos5 = n;
  }
  else if ( n < pos5) // 100 500
  {
    for (int i = pos5; i != n ; i--) {
      myServo.setPWM(5, 0, i);
      delay(10);
      Serial.println("motor6 at: " + String(i));
    }
    pos5 = n;
  }
}

void loop() {
  //    Serial.begin(9600) ;
  //    if(Serial.available() >0 )
  //    {
  //      int t=99;
  //      t= Serial.parseInt();
  //      Serial.println("Rec: " + String(t) );
  //      if (t==2)
  //        grab();
  //      Serial.end();
  //      delay(100);
  //     }
  //  for (int i = min5; i <= max5; i++) {
  //    myServo.setPWM(5, 0, i);
  //    delay(10);
  //    Serial.println("5 at: " + String(i));
  //  }
  //
  //  for (int i = max5; i >= min5 ; i--) {
  //    myServo.setPWM(5, 0, i);
  //    delay(10);
  //    Serial.println(i);
  //  }

//  motor6(100);

}
