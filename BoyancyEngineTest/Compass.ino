/*  SeaFlight Glider
    Compass HMC6343


    More information:     https://www.sparkfun.com/products/12916
    Interfacing guide:    https://github.com/sparkfun/SparkFun_HMC6343_Arduino_Library

    Todo:
    - add SD Card Interface
    - add time since last update
    - add clock sync
    - 
*/


#define UTF8_HEX_DEGREE_SYMBOL                  "\xC2\xB0"
const unsigned int COMPASS_REFRESH_DELAY = 2000;

SFE_HMC6343 compass; // Declare the sensor object
bool validCompass = false;
long previousMillisCompass = 0;

void setupCompass() {
  if (!compass.init())
  {
    validCompass = false;
    Log.error(module::SENSOR, "NOT DETECTED: Compass HMC6343 chip\n");
  }
  else {
    Log.info(module::SENSOR, "DETECTED: Compass HMC6343 chip\n");
    validCompass = true;
  }
}

void loopCompass() {
  if (validCompass) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisCompass > COMPASS_REFRESH_DELAY) {
      previousMillisCompass = millis();
      // Read, calculate, and print the heading, pitch, and roll from the sensor
      compass.readHeading();
      compass.readMag();
      compass.readTilt();
      // Read, calculate, and print the acceleration on the x, y, and z axis of the sensor
      compass.readAccel();
      Log.info(module::SENSOR, "DETECTED: Compass HMC6343 chip\n");

    }
  }
}

void vt100DashDisplayCompass(bool refreshAll, int x, int y) {
  if (validCompass) {
    if (refreshAll) {

    }

    term.position(y, x);
    term.print(F("Compass"));
    int y_off = 1;
    term.position(y+y_off, x);
    term.print(F("Heading:"));
    term.print((float) compass.heading/10.0);
    Serial.print(UTF8_HEX_DEGREE_SYMBOL);
    
    y_off++;
    x = x+2;
    term.position(y+y_off, x);
    term.print(F("Magnetometer:"));
    term.position(y+y_off, x + 14);
    term.print(F("x="));
    term.print(compass.magX);
    term.position(y+y_off, x + 24);
    term.print(F("y="));
    term.print(compass.magY);
    term.position(y+y_off, x + 34);
    term.print(F("z="));
    term.print(compass.magZ);

    y_off++;
    term.position(y+y_off, x);
    term.print(F("Raw Tilt Data:"));
    term.position(y+y_off, x + 14);
    term.print(F("pitch="));
    term.print(compass.pitch);
    term.position(y+y_off, x + 27);
    term.print(F("roll="));
    term.print(compass.roll);
    term.position(y+y_off, x + 40);
    term.print(F("temp="));
    term.print(compass.temperature);

    y_off++;
    term.position(y+y_off, x);
    term.print(F("Acceleration:"));
    term.position(y+y_off, x + 14);
    term.print(F("x="));
    term.print((float) compass.accelX/1024.0);
    term.print(F("g")); 
    term.position(y+y_off, x + 24);
    term.print(F("y="));
    term.print((float) compass.accelY/1024.0);
    term.print(F("g")); 
    term.position(y+y_off, x + 34);
    term.print(F("z="));
    term.print((float) compass.accelZ/1024.0);
    term.print(F("g")); 
  }
}

//   compass.enterStandby();
//   compass.exitStandby();
//    compass.enterSleep();
//    compass.exitSleep();
//    compass.setOrientation(LEVEL);

//    byte opmode1 = compass.readOPMode1();
//    Serial.print("OP Mode 1: ");
//    Serial.println(opmode1,BIN);

//    int sn = compass.readEEPROM(SN_LSB);
//    delay(10);
//    sn |= (compass.readEEPROM(SN_MSB) << 8);
