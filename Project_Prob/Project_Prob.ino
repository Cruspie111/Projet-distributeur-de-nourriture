#include <Wire.h>   //stepPerRevolution =2048  8 MIPS
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd( 0x26,16, 2);

#define bp1  3
#define bp2  4
#define bp3  5 
#define bp4  6
#define poto  A0

#define motor0  9
#define motor1  10
#define motor2  11
#define motor3  12
char heure[10];
float portionCal;
int portion, portionPre, nourriturePre = 500, feedDay, feedEve, valCap1, valCap2, var, cap1, cap2, time1;
int state = 0, run1 = 0;
int stepMotor = 0;
int statePlus = 0;
int stateMinus = 0;
int statePlusPrevious = 0;
int stateMinusPrevious = 0;
int value = 0;
int valuePrevious = 0;
int reee = 0;
int reeePrevious = 0;
int fedDay = 0;
int fedEve = 0;

void setup()
{
  pinMode(bp1, INPUT_PULLUP);
  pinMode(bp2, INPUT_PULLUP);
  pinMode(bp3, INPUT_PULLUP);
  pinMode(bp4, INPUT_PULLUP);
  pinMode(motor0, OUTPUT);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(motor3, OUTPUT);
  Serial.begin(9600);
  //DS1307 setting
  // Attente de la connection serie avec l'Arduino
  while (!Serial);

  // Lance le communication I2C avec le module RTC et
  // attend que la connection soit operationelle
  while (! rtc.begin()) {
    Serial.println("Attente du module RTC...");
    delay(1000);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("Horloge du module RTC mise a jour");

  //lcd setting
  lcd.begin (16, 2); // initialize the lcd
  lcd.backlight();//To Power ON the back light
  lcd.init();
  lcd.clear();
}

void loop()
{
  cap1 = analogRead(A1);
  cap2 = analogRead(A2);
  DateTime now = rtc.now();
  statePlus = digitalRead(bp1);
  stateMinus = digitalRead(bp2);
  value = digitalRead(bp3);
  if (statePlus == HIGH && statePlusPrevious == LOW)
    state++;
  if (stateMinus == HIGH && stateMinusPrevious == LOW)
    state--;
  if (value == HIGH && valuePrevious == LOW && reeePrevious == 0)
  {
    reee = 1;
    reeePrevious = reee;
    valuePrevious = value;
  }
  if (value == HIGH && valuePrevious == LOW && reeePrevious == 1)
  {
    reee = 0;
    reeePrevious = reee;
    valuePrevious = value;
  }
  if (state > 3) state = 3;
  if (state < 0)  state = 0;
  statePlusPrevious = statePlus;
  stateMinusPrevious = stateMinus;
  valuePrevious = value;

  if (reee == 0)
  {
    switch (state)
    {
      case 0:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bienvenue");
        lcd.setCursor(11, 1);
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        lcd.print(now.minute(), DEC);
        delay(300);
        break;
      case 1:
        lcd.clear();
        portionPre = analogRead(A0);
        portionPre = map(portionPre, 0, 1023, 0, 20);
        portion=portionPre%10;
        portionCal=(portionPre/10)+(portion/10.0);
        lcd.setCursor(0, 0);
        lcd.print("Portion Value ");
        lcd.setCursor(0, 1);
        lcd.print(portionCal);
        lcd.setCursor(11, 1);
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        lcd.print(now.minute(), DEC);
        delay(300);
        break;

      case 2:
        lcd.clear();
        feedDay = analogRead(A0);
        feedDay = map(feedDay, 0, 1023, 0, 14);
        lcd.setCursor(0, 0);
        lcd.print("Feed time Day");
        lcd.setCursor(2, 1);
        lcd.print("H");
        lcd.setCursor(0, 1);
        lcd.print(feedDay);
        lcd.setCursor(11, 1);
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        lcd.print(now.minute(), DEC);
        delay(300);
        break;

      case 3:
        lcd.clear();
        feedEve = analogRead(A0);
        feedEve = map(feedEve, 0, 1023, 15, 23);
        lcd.setCursor(0, 0);
        lcd.print("Feed time Eve");
        lcd.setCursor(2, 1);
        lcd.print("H");
        lcd.setCursor(0, 1);
        lcd.print(feedEve);
        lcd.setCursor(11, 1);
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        lcd.print(now.minute(), DEC);
        delay(300);
        break;
      case 4:
        break;
    }
  }

  if (reee == 1)
  {
    switch (run1)
    {
      case 0:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Running");
        lcd.setCursor(11, 1);
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        lcd.print(now.minute(), DEC);
        if (feedDay == now.hour() && fedDay == 0)
        {
          fedEve = 0;
          fedDay = 1;
          run1 = 1;
        }
        else if (feedEve == now.hour() && fedEve == 0)
        {
          fedEve = 1;
          fedDay = 0;
          run1 = 1;
        }
        delay(1000);
        break;

      case 1:
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Cap1  Cap2");
        lcd.setCursor(3, 1);
        lcd.print(cap1);
        lcd.setCursor(9, 1);
        lcd.print(cap2);
        if ( (cap1 > nourriturePre) || (cap2 > nourriturePre)) run1 = 2;
        else if ( (cap1 < nourriturePre) || (cap2 < nourriturePre)) run1 = 3;
        break;

      case 2:
        for (int a = 0; a < 256; a++)
        { motor(false);
          delay(5);
        }
        delay(portionCal*600.0);
        for (int a = 0; a < 256; a++)
        { motor(true);
          delay(5);
          digitalWrite(motor0, LOW);
          digitalWrite(motor1, LOW);
          digitalWrite(motor2, LOW);
          digitalWrite(motor3, LOW);
          run1 = 0;
        }

        break;

      case 3:
        for (int a = 0; a < 256; a++)
        { motor(false);
          delay(5);
        }
        delay(portionCal*600.0);
        for (int a = 0; a < 256; a++)
        {
          motor(true);
          delay(5);
          digitalWrite(motor0, LOW);
          digitalWrite(motor1, LOW);
          digitalWrite(motor2, LOW);
          digitalWrite(motor3, LOW);
          run1 = 0;
        }
        break;
    }
  }
}
void motor(bool rot)
{
  if (rot)
  {
    switch (stepMotor)
    {
      case 0:
        digitalWrite(motor0, HIGH);
        digitalWrite(motor1, HIGH);
        digitalWrite(motor2, LOW);
        digitalWrite(motor3, LOW);
        break;
      case 1:
        digitalWrite(motor0, LOW);
        digitalWrite(motor1, HIGH);
        digitalWrite(motor2, HIGH);
        digitalWrite(motor3, LOW);
        break;
      case 2:
        digitalWrite(motor0, LOW);
        digitalWrite(motor1, LOW);
        digitalWrite(motor2, HIGH);
        digitalWrite(motor3, HIGH);
        break;
      case 3:
        digitalWrite(motor0, HIGH);
        digitalWrite(motor1, LOW);
        digitalWrite(motor2, LOW);
        digitalWrite(motor3, HIGH);
        break;
    }
  }
  else
  {
    switch (stepMotor)
    {
      case 0:
        digitalWrite(motor0, HIGH);
        digitalWrite(motor1, LOW);
        digitalWrite(motor2, LOW);
        digitalWrite(motor3, HIGH);
        break;
      case 1:
        digitalWrite(motor0, LOW);
        digitalWrite(motor1, LOW);
        digitalWrite(motor2, HIGH);
        digitalWrite(motor3, HIGH);
        break;
      case 2:
        digitalWrite(motor0, LOW);
        digitalWrite(motor1, HIGH);
        digitalWrite(motor2, HIGH);
        digitalWrite(motor3, LOW);
        break;
      case 3:
        digitalWrite(motor0, HIGH);
        digitalWrite(motor1, HIGH);
        digitalWrite(motor2, LOW);
        digitalWrite(motor3, LOW);
        break;
    }
  }
  stepMotor++;
  if (stepMotor > 3)
    stepMotor = 0;
}
