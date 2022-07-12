/*  SeaFlight Glider
    RV1805 Real Time Clock


    More information:     https://www.sparkfun.com/products/14558
    Interfacing guide:    https://github.com/sparkfun/Qwiic_RTC_Module

    Todo:
    - Be able to set time manually
    - Sync time to GPS
    - go through this example: https://www.pjrc.com/teensy/td_libs_Time.html
*/
const unsigned int CLOCK_RV1805_REFRESH_DELAY = 20;
bool validClockRV1805 = false;
long previousMillisClockRV1805 = 0;

//rtc.enableTrickleCharge(DIODE_0_3V, ROUT_3K);


void setupTime() {
  if (rtc.begin() == false) {
    validClockRV1805 = false;
    Log.error(module::TIME, "NOT DETECTED: Clock RV1805 Sensor\n");
  }
  else {
    Log.info(module::TIME, "DETECTED: Clock RV1805 Sensor\n");
    validClockRV1805 = true;
    rtc.set24Hour();
  }
}

void loopTime() {
  if (validClockRV1805) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisClockRV1805 > CLOCK_RV1805_REFRESH_DELAY) {
      previousMillisClockRV1805 = millis();
      if (rtc.updateTime() == false) //Updates the time variables from RTC
      {
        Log.error(module::TIME, "ERROR: Clock RV1805 failed to update\n");
      }
      else {
        currentDate = rtc.stringDateUSA(); //Get the current date in mm/dd/yyyy format (we're weird)
        //String currentDate = rtc.stringDate()); //Get the current date in dd/mm/yyyy format
        currentTime = rtc.stringTime(); //Get the time

      }
    }
  }
}

bool setClockRV1805(GliderTimeStruct newTime, String timesource) {
  if (rtc.setTime(newTime.hund, newTime.sec, newTime.minute, newTime.hour, newTime.date, newTime.month, newTime.year, newTime.day) == false) {
    Log.error(module::TIME, "ERROR: Clock RV1805 failed to update from %s\n", timesource);
    return 1;
  }
  Log.info(module::TIME, "TIME: RTC syncd from %s : sec=%i, min=%i, hour=%i, day=%i, month=%i, year=%i\n", timesource, newTime.sec, newTime.minute, newTime.hour, newTime.date, newTime.month, newTime.day);

  return 0;
}

void compareTime(GliderTimeStruct newTime, String timesource) {
  //uint8_t diffHund = newTime.hund - rtc.getHundredths();
  uint8_t diffSec = newTime.sec - rtc.getSeconds();
  uint8_t diffMin = newTime.minute - rtc.getMinutes();
  uint8_t diffHour = newTime.hour - rtc.getHours();
  uint8_t diffDay = newTime.day - rtc.getDate();
  uint8_t diffMonth = newTime.month - rtc.getMonth();
  uint8_t diffYear = newTime.year - rtc.getYear();
  Log.info(module::TIME, "COMPARE TIME with %s diff: sec=%i, min=%i, hour=%i, day=%i, month=%i, year=%i\n", timesource, diffSec, diffMin, diffHour, diffDay, diffMonth, diffYear);

}
