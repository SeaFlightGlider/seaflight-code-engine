/*
  SerLCD Library - Show the firmware version
  Gaston Williams - August 29, 2018

  This sketch prints the device's firmware version to the screen.

  The circuit:
   SparkFun RGB OpenLCD Serial display connected through
   a Sparkfun Qwiic adpater to an Ardruino with a
   Qwiic shield or a Sparkfun Blackboard with Qwiic built in.

  The Qwiic adapter should be attached to the display as follows:
  Display	/ Qwiic Cable Color
 	GND	/	Black
 	RAW	/	Red
 	SDA	/	Blue
 	SCL	/	Yellow

  Note: If you connect directly to a 5V Arduino instead, you *MUST* use
  a level-shifter to convert the i2c voltage levels down to 3.3V for the display.

  This code is based on the LiquidCrystal code originally by David A. Mellis
  and the OpenLCD code by Nathan Seidle at SparkFun.

  License: This example code is in the public domain.
*/

//#include <Wire.h>
#include <i2c_t3.h>                         //library: https://github.com/nox771/i2c_t3


#include <SerLCD.h> //Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
SerLCD lcd; // Initialize the library with default I2C address 0x72

void setup() {
  Wire.begin();

  lcd.begin(Wire); //Set up the LCD for I2C
}

void loop() {
  lcd.command(','); //Send the comma to display the firmware version
  //Firmware will be displayed for 500ms so keep re-printing it
  delay(500);
}

