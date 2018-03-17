// DS3231_Serial_Easy
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my DS3231-library to
// quickly send time and date information over a serial link
//
// To use the hardware I2C (TWI) interface of the Arduino you must connect
// the pins as follows:
//
// Arduino Uno/2009:
// ----------------------
// DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
//
// Arduino Leonardo:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 2 or the dedicated SDA pin
//          SCL pin   -> Arduino Digital 3 or the dedicated SCL pin
//
// Arduino Mega:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL pin
//
// Arduino Due:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA1 (Digital 70) pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL1 (Digital 71) pin
//
// The internal pull-up resistors will be activated when using the
// hardware I2C interfaces.
//
// You can connect the DS3231 to any available pin but if you use any
// other than what is described above the library will fall back to
// a software-based, TWI-like protocol which will require exclusive access
// to the pins used, and you will also have to use appropriate, external
// pull-up resistors on the data and clock signals.
//
#include <Arduino.h>
#include <stdlib.h>
#include "Controller.h"
#include "Gbl.h"
#include "LightCtr.h"

Controller masterCtr;      // handles the remotes
Stream *Gbl::strPtr = &Serial;


Light Red(Gbl::RED_PIN, 0);
Light Green(Gbl::GREEN_PIN, 1);
Light Blue(Gbl::BLUE_PIN, 2);

Light *LightCtr::red = &Red;
Light *LightCtr::green = &Green;
Light *LightCtr::blue = &Blue;

//#define BTUNO
#ifdef BTUNO
	#include "SoftwareSerial.h"
	SoftwareSerial BT = SoftwareSerial(12, 13); // RX,TX
#endif

//#define TM
#ifdef TM
#include <DS3231.h>
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
Time alarm;
#endif

//#define IR
#ifdef IR
#include "IRremote.h"
IRrecv irrecv(Gbl::IREC_PIN);      // from the ir decode library
decode_results Results; // from the ir decode library
#endif


void setup()
{
  // Setup Serial connection
  Serial.begin(115200);
#ifdef BTUNO
  BT.begin(115200);
#endif
#ifdef IR
  irrecv.enableIRIn();
#endif
  // Initialize the rtc object
/*  rtc.begin();

  alarm.hour = 12;
  alarm.min = 13;
  alarm.sec = 0;
  alarm.year = 2018;
  alarm.mon = 1;
  alarm.date = 28;*/

  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(19, 57, 00);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(03, 2, 2018);   // Set the date to January 1st, 2014
  Gbl::strPtr->println(F("we are progammed to recieve"));
  Gbl::freeRam();
}

void loop()
{
	// run the timer
	//masterCtr.timer(millis());

	// run Serial commands
    while (Serial.available()) {
        Gbl::strPtr = &Serial;
        masterCtr.serialReceive();
        Gbl::freeRam();
    }
#ifdef BTUNO
	while (BT.available()) {
        Gbl::strPtr = &BT;
        masterCtr.serialReceive();
        Gbl::freeRam();
    }
#endif

#ifdef IR
    if (irrecv.decode(&Results)) {
		masterCtr.irReceive(Results.value);
		irrecv.resume(); // Receive the next value
	}
#endif

}


/*void timeBits() {
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");

  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());

  // Wait one second before repeating :)

  Serial.print("Temperature: ");
  Serial.print(rtc.getTemp());
  Serial.println(" C");

  // Send Current time
  Serial.print("Current Time.............................: ");
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  Serial.println(rtc.getTimeStr());

  // Send Unixtime
  // ** Note that there may be a one second difference between the current time **
  // ** and current unixtime show if the second changes between the two calls   **
  Serial.print("Current Unixtime.........................: ");
  Serial.println(rtc.getUnixTime(rtc.getTime()));

  // Send Unixtime for 00:00:00 on January 1th 2014
  Serial.print("Alarm set for ");
  Serial.print(rtc.getDOWStr(alarm));
  Serial.print(" ");




  Serial.print(" ");
  Serial.print(rtc.getDateStr(alarm));
  Serial.print(" -- ");
  Serial.println(rtc.getTimeStr(alarm));

  delay (5000);
}*/
