

//CONSIDER using mavlink to be able to 'play back' logs on missioncontrol software
// like this: https://github.com/mehdilauters/MavlinkSerialSdLogger


#define STORAGE_REFRESH_DELAY     1000

const int chipSelect = 1; //SDCARD_SS_PIN;   //todo: remove board specific variables
//const int chipSelect = SDCARD_SS_PIN;   //todo: remove board specific variables

bool validSDCard = false;
long previousMillisStorage = 0;

void setupStorage() {
//  if (!SD.begin(chipSelect)) {
//    validSDCard = false;
//    Serial.println("NOT DETECTED: SD Card");
//  }
//  else {
//    Serial.println("DETECTED: SD Card");
//    validSDCard = true;
//  }
}


void loopStorage() {
  if (validSDCard && validClockRV1805) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisStorage > STORAGE_REFRESH_DELAY) {
      previousMillisStorage = millis();
      String dataString = "";
      dataString += String(currentDate);
      dataString += ",";
      dataString += String(currentTime);
      dataString += ",";
      dataString += String(pumpTestState);
      dataString += ",";
      dataString += String(latestPressureM300_RESERVOIR);
      dataString += ",";
      dataString += String(latestPressureM300_BLADDER);
      dataString += ",";
      dataString += String(reservoirDistanceTOF);

      //File dataFile = SD.open("datalog.txt", FILE_WRITE);
//      if (dataFile) {
//        dataFile.println(dataString);
//        dataFile.close();
//        // print to the serial port too:
//        Serial.println(dataString);
//      }
//      // if the file isn't open, pop up an error:
//      else {
//        Serial.println("SDCARD: error opening datalog.txt");
//      }
    }
  }
}
