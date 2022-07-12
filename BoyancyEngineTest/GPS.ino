/*  SeaFlight Glider
    NeoM8U GPS


    More information:     https://www.sparkfun.com/products/16329
    Interfacing guide:    https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library

    Todo:


*/
#define defaultMaxWait 250
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_u-blox_GNSS

const unsigned int GPS_8MU_REFRESH_DELAY = 5000;
bool validGPS = false;
SFE_UBLOX_GNSS myGNSS;
long previousMillisGPSNeoM8Urefresh = 0;
bool verifyTime = true;
bool syncTimeToRTC = true;
long gps_hour = 0;
long gps_minute = 0;

    
struct gpsStruct {
  int32_t alt;
    int32_t lat;
    int32_t lng;
    
  long latitude;
  long longitude;
  long altitude;
  byte SIV;
};

GliderTimeStruct gpsTime;

void setupGPS() {

  if (myGNSS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Log.error(module::GPS, "NOT DETECTED: GPS M8Q chip\n");
    validGPS = false;
  }
  else {
    Log.info(module::GPS, "DETECTED: GPS M8Q chip\n");
    validGPS = true;
    //    //Disable or enable various NMEA sentences over the I2C interface
    //    myGNSS.setI2COutput(COM_TYPE_NMEA | COM_TYPE_UBX); // Turn on both UBX and NMEA sentences on I2C. (Turn off RTCM and SPARTN)
    //    myGNSS.enableNMEAMessage(UBX_NMEA_GLL, COM_PORT_I2C); // Several of these are on by default on ublox board so let's disable them
    //    myGNSS.enableNMEAMessage(UBX_NMEA_GSA, COM_PORT_I2C);
    //    myGNSS.enableNMEAMessage(UBX_NMEA_GSV, COM_PORT_I2C);
    //    myGNSS.enableNMEAMessage(UBX_NMEA_RMC, COM_PORT_I2C);
    //    myGNSS.enableNMEAMessage(UBX_NMEA_VTG, COM_PORT_I2C);
    //    myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_I2C); // Leave only GGA enabled at current navigation rate
    //
    //    // Set the Main Talker ID to "GP". The NMEA GGA messages will be GPGGA instead of GNGGA
    //    myGNSS.setMainTalkerID(SFE_UBLOX_MAIN_TALKER_ID_GP);
    //    myGNSS.setMainTalkerID(SFE_UBLOX_MAIN_TALKER_ID_DEFAULT); // Uncomment this line to restore the default main talker ID

    //myGNSS.setHighPrecisionMode(true); // Enable High Precision Mode - include extra decimal places in the GGA messages

    //myGNSS.saveConfiguration(VAL_CFG_SUBSEC_IOPORT | VAL_CFG_SUBSEC_MSGCONF); //Optional: Save only the ioPort and message settings to NVM

    myGNSS.setI2COutput(COM_TYPE_UBX | COM_TYPE_NMEA); //Set the I2C port to output both NMEA and UBX messages
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

    //This will pipe all NMEA sentences to the serial port so we can see them
    myGNSS.setNMEAOutputPort(Serial);
  }
}

void loopGPS() {
  if (validGPS) {
    //if(gliderState == 
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisGPSNeoM8Urefresh > GPS_8MU_REFRESH_DELAY) {
      previousMillisGPSNeoM8Urefresh = millis();
      uint8_t result = myGNSS.checkUblox(); //See if new data is available. Process bytes as they come in.
      byte fixType = myGNSS.getFixType(); // Get the fix type
      
      if(fixType > 1 && verifyTime){
        verifyTime = false;
        GliderTimeStruct currentTime;
        currentTime.day = myGNSS.getDay();
        currentTime.month = myGNSS.getMonth();
        currentTime.year = myGNSS.getYear();
        currentTime.hour = myGNSS.getHour();
        currentTime.minute = myGNSS.getMinute();
        currentTime.sec = myGNSS.getSecond();
        
        compareTime(currentTime, "GPS");
      }
      if(fixType > 1 && syncTimeToRTC){
        syncTimeToRTC = false;
        GliderTimeStruct currentTime;
        currentTime.day = myGNSS.getDay();
        currentTime.month = myGNSS.getMonth();
        currentTime.year = myGNSS.getYear();
        currentTime.hour = myGNSS.getHour();
        currentTime.minute = myGNSS.getMinute();
        currentTime.sec = myGNSS.getSecond();
        
        setClockRV1805(currentTime, "GPS");
      }
      if (fixType == 0)
        Serial.print(F("GPS (No fix)"));
      else if (fixType == 1) {
        Serial.print(F("GPS (Dead reckoning)"));

      }
      else if (fixType == 2)
        Serial.print(F("GPS (2D)"));
      else if (fixType == 3) {
        Serial.print(F("GPS (3D)"));
        
      }
      else if (fixType == 4)
        Serial.print(F("GPS (GNSS + Dead reckoning)"));

      //      float latitude = myGNSS.getLatitude();
      //      long longitude = myGNSS.getLongitude();
      //      Serial.print(F(" Long: "));
      //      Serial.print(longitude);
      //      Serial.print(F(" (degrees * 10^-7)"));
      //
      //      long altitude = myGNSS.getAltitude();
      //      Serial.print(F(" Alt: "));
      //      Serial.print(altitude);
      //      Serial.print(F(" (mm)"));
      //
      //      long SIV = myGNSS.getSIV();
      //      Serial.print(F(" SIV: "));
      //      Serial.println(SIV);
      //
      //      Serial.print(myGNSS.getYear());
      //      Serial.print("-");
      //      Serial.print(myGNSS.getMonth());
      //      Serial.print("-");
      //      Serial.print(myGNSS.getDay());
      //      Serial.print(" ");
      //      Serial.print(myGNSS.getHour());
      //      Serial.print(":");
      //      Serial.print(myGNSS.getMinute());
      //      Serial.print(":");
      //      Serial.print(myGNSS.getSecond());
      //
      //      Serial.print("  Time is ");
      //      if (myGNSS.getTimeValid() == false)
      //      {
      //        Serial.print("not ");
      //      }
      //      Serial.print("valid  Date is ");
      //      if (myGNSS.getDateValid() == false)
      //      {
      //        Serial.print("not ");
      //      }
      //      Serial.print("valid");
      //
      //      Serial.println();


    }
  }
}

void vt100DashDisplayGPS(bool refreshAll, int x, int y) {
  if (validGPS) {

    // if (rtc.setTime(0, myGNSS.getSecond(), myGNSS.getMinute(), myGNSS.getHour(), date, myGNSS.getMonth(), myGNSS.getYear(), myGNSS.getDay()) == false) {
    //   Serial.println("Something went wrong setting the time");
    // }
  }
}
