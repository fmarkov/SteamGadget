#include <Wire.h>
#include <RTClib.h>
#include "LedControl.h"

RTC_DS3231 rtc;                     // create rtc for the DS3231 RTC module, address is fixed at 0x68
LedControl lc = LedControl(12,11,10,2);

const unsigned long dotDelay=500;
bool serialOutput = false;
bool showDots = true;
bool militaryTime = true;

int ss = 0;
int mm = 0;
int hh = 0;
int DD = 0;
int dd = 0;
int MM = 0;
int yyyy = 0;
int prevSecond = 0;
float temperature = 0;

int secondsOnes = 0;
int secondsTens = 0;
int minutesOnes = 0;
int minutesTens = 0;
int hoursOnes = 0;
int hoursTens = 0;

void updateRTC()
{

  // ask user to enter new date and time
  const char txt[6][15] = { "year [4-digit]", "month [1~12]", "day [1~31]",
                            "hours [0~23]", "minutes [0~59]", "seconds [0~59]"};
  String str = "";
  long newDate[6];

  while (Serial.available()) {
    Serial.read();  // clear serial buffer
  }

  for (int i = 0; i < 6; i++) {

    Serial.print("Enter ");
    Serial.print(txt[i]);
    Serial.print(": ");

    while (!Serial.available()) {
      ; // wait for user input
    }

    str = Serial.readString();  // read user input
    newDate[i] = str.toInt();   // convert user input to number and save to array

    Serial.println(newDate[i]); // show user input
  }

  // update RTC
  rtc.adjust(DateTime(newDate[0], newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]));
  Serial.println("RTC Updated!");
}

void updateRTCSecs()
{

}

void doEachSecond()
{
  if (ss == prevSecond)
    return;

  serialOutputTime();

  displayDots();

  prevSecond = ss;
}

void serialOutputTime()
{
  if (!serialOutput)
    return;

  Serial.print("TIME: ");
  
  Serial.print(yyyy);
  Serial.print("-");
  Serial.print(MM);
  Serial.print("-");
  Serial.print(dd);
  Serial.print(" ");
  Serial.print(hh);
  Serial.print(":");
  Serial.print(mm);
  Serial.print(":");
  Serial.println(ss);

  Serial.print("secondsTens = ");
  Serial.print(secondsTens);

  Serial.print(" secondsOnes = ");
  Serial.println(secondsOnes);

  Serial.print("minutesTens = ");
  Serial.print(minutesTens);

  Serial.print(" minutesOnes = ");
  Serial.println(minutesOnes);

  Serial.print("TEMP: ");
  Serial.println(temperature);

  Serial.println(" ");

}

void getTime()
{
  DateTime rtcTime = rtc.now();

  ss = rtcTime.second();
  mm = rtcTime.minute();
  
  if (militaryTime)
    hh = rtcTime.hour();
  else
    hh = rtcTime.twelveHour();
  
  DD = rtcTime.dayOfTheWeek();
  dd = rtcTime.day();
  MM = rtcTime.month();
  yyyy = rtcTime.year();
  temperature = rtc.getTemperature();
  
}

void formatTime()
{
  secondsTens = ss / 10;
  secondsOnes = ss % 10;

  minutesTens = mm / 10;
  minutesOnes = mm % 10;

  hoursTens = hh / 10;
  hoursOnes = hh % 10;
}

void segZeroOutputTime()
{
  lc.setDigit(0,0,secondsOnes, false);

  lc.setDigit(0,1,secondsTens, false);

  lc.setDigit(0,3,minutesOnes, false);

  lc.setDigit(0,4,minutesTens, false);
  
  lc.setDigit(0,6,hoursOnes, false);
  
  if (hoursTens != 0)
  {
    lc.setDigit(0,7,hoursTens, false);
  }
  else
  {
    lc.setChar(0,7,' ', false);
  }
  
}

void segOneOutputTime()
{
  lc.setDigit(1,0,secondsOnes, false);

  lc.setDigit(1,1,secondsTens, false);

  lc.setDigit(1,3,minutesOnes, false);

  lc.setDigit(1,4,minutesTens, false);
  
  lc.setDigit(1,6,hoursOnes, false);
  
  if (hoursTens != 0)
  {
    lc.setDigit(1,7,hoursTens, false);
  }
  else
  {
    lc.setChar(1,7,' ', false);
  }
}

void dotsOn()
{
  lc.setChar(0, 5, ' ', true);//dot on
  lc.setChar(0, 2, ' ', true);//dot on

  lc.setChar(1, 5, ' ', true);//dot on
  lc.setChar(1, 2, ' ', true);//dot on
}

void dotsOff()
{
  lc.setChar(0, 5, ' ', false);//dot off
  lc.setChar(0, 2, ' ', false);//dot off

  lc.setChar(1, 5, ' ', false);//dot off
  lc.setChar(1, 2, ' ', false);//dot off
}

void setup() 
{
  Serial.begin(9600); // initialize serial
  
  rtc.begin();

  lc.shutdown(0,false);
  lc.shutdown(1,false);

  /* Set the brightness to a medium values */

  lc.setIntensity(0,4);
  lc.setIntensity(1,4);

  /* and clear the display */

  lc.clearDisplay(0);
  lc.clearDisplay(1);
}

void displayDots()
{
  if (showDots)
  { 
    dotsOn();
    digitalWrite(LED_BUILTIN, LOW);

    delay(dotDelay);
    dotsOff();
    digitalWrite(LED_BUILTIN, HIGH);
  }  
}

void toggleShowDots()
{
  if (showDots)
    showDots = false;
  else
    showDots = true;
}

void toggleSerialOutput()
{
  if (serialOutput)
    serialOutput = false;
  else
    serialOutput = true;
}

void toggleMilitarytime()
{
  if (militaryTime)
    militaryTime = false;
  else
    militaryTime = true;
}

void loop() 
{  
  getTime();

  formatTime();
  
  segZeroOutputTime();

  segOneOutputTime();

  doEachSecond();

  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'u') updateRTC();  // update RTC time
    if (input == 's') updateRTCSecs();
    if (input == 'd') toggleShowDots();
    if (input == 'o') toggleSerialOutput();
    if (input == 't') toggleMilitarytime();
  }
}
