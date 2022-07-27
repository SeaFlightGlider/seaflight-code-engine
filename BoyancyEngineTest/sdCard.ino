//CONSIDER using mavlink to be able to 'play back' logs on missioncontrol software
// like this: https://github.com/mehdilauters/MavlinkSerialSdLogger
// MAKE SURE THAT THE SD CARD LIBRARY IS THE NEWEST ONE, ONLY AVALIABLE WHEN DOWNLOADING IDE, NOT ON GITHUB, LINK BELOW
// https://drive.google.com/drive/folders/1ms9dZfpnZduC7-vpsqOPZyQeZNXBNqy4?usp=sharing
File root;
String userfirstinput;
String usersecondinput;
#define STORAGE_REFRESH_DELAY     1000

const int chipSelect = BUILTIN_SDCARD; //SDCARD_SS_PIN;   //todo: remove board specific variables
//const int chipSelect = SDCARD_SS_PIN;   //todo: remove board specific variables

bool validSDCard = false;
long previousMillisStorage = 0;

void setupStorage() {
  if (!SD.begin(chipSelect)) {
    validSDCard = false;
    Serial.println("NOT DETECTED: SD Card");
  }
  else {
    Serial.println("DETECTED: SD Card");
    validSDCard = true;
  }
  root = SD.open("/");
  Serial.println("---------------------------------------");
  Serial.println("");
  Serial.println("type 'file' to go to files menu");
  Serial.println("");
  Serial.println("---------------------------------------");
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

void loopStorageToSerial() {
  if (Serial.available() > 0) {
    userfirstinput = Serial.readStringUntil('\n');
    Serial.print(userfirstinput);
  }
  if (userfirstinput == "ile") {// <-----This one right here officer
    printDirectory(root, 0);
    Serial.println("");
    Serial.println("Which file would you like to open?");
    for (;;) {
      if (Serial.available() > 0) {
        usersecondinput = Serial.readString();
        char charBuf[50];
        usersecondinput.toCharArray(charBuf, usersecondinput.length());

        root = SD.open(charBuf);
        Serial.println("");

        while (root.available()) {
          Serial.write(root.read());
        }
        root.close();
      }
    }
  }
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry = dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
