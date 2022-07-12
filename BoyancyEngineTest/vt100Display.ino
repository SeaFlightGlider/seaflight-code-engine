
#define VT100_WIDTH   119
#define VT100_HEIGHT  30
#define PRESSURE_X      2
#define PRESSURE_Y      3
#define GPS_X           80
#define GPS_Y           10
#define COMPASS_X       2
#define COMPASS_Y       7
#define STATE_X         80
#define STATE_Y         3

#define UTF8_HEX_UPPER_RIGHT_CORNER             "\xE2\x94\x90"
#define UTF8_HEX_UPPER_LEFT_CORNER              "\xE2\x94\x8C"
#define UTF8_HEX_LOWER_RIGHT_CORNER             "\xE2\x94\x98"
#define UTF8_HEX_LOWER_LEFT_CORNER              "\xE2\x94\x94"
#define UTF8_HEX_HORIZONTAL_LINE                "\xE2\x94\x80"
#define UTF8_HEX_VERTICAL_LINE                  "\xE2\x94\x82"

long intervalVT100DashDisplay = 200;
unsigned long previousMillisVT100DashDisplay = 0;
int refreshAllCounter = 0;
int refreshLimit = 20;
uint32_t now;
uint32_t last;
#define DIGITAL_ROW 3
#define ANALOG_ROW  7
bool firstTime = true;
byte lineFlatLine = 196;          //  E2 94 80
byte lineLeftDown = 218;          //  E2 94 8C
byte lineRightDown = 191;         //  E2 94 90
byte lineUpRight = 192;           //  E2 94 94
byte lineVertical = 179;          //  E2 94 82
byte lineLowerLeftCorner = 192;   //  E2 94 94
byte linelowerRightCorner = 217;  //  E2 94 98

void vt100DashSetup(void) {
  term.init();
  term.cls();
  term.show_cursor(false);
  firstTime = true;
}

void vt100DashLoop(void) {
  if (displayVT100Dash) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisVT100DashDisplay > intervalVT100DashDisplay) {
      previousMillisVT100DashDisplay = currentMillis;
      bool refreshDisplay = false;
      if (refreshAllCounter > refreshLimit || firstTime) {
        refreshDisplay = true;
        refreshAllCounter = 0;
        firstTime = false;
      }
      vt100DashDrawBox(refreshDisplay);
      vt100DashDisplayTopLine(refreshDisplay);
      vt100DashDisplayPressure(refreshDisplay, PRESSURE_X, PRESSURE_Y);
      vt100DashDisplayGPS(refreshDisplay, GPS_X, GPS_Y);
      vt100DashDisplayCompass(refreshDisplay, COMPASS_X, COMPASS_Y);
      vt100DashDisplayStateMachine(refreshDisplay, STATE_X, STATE_Y);
      refreshAllCounter++;
    }
  }
}
//  message = "\x02sRN EIIpAddr\x03";

void vt100DashDrawBox(bool refreshAll) {
  if (refreshAll) {
    term.position(1, 0);
    Serial.print(UTF8_HEX_UPPER_LEFT_CORNER);
    for (int w = 0; w < VT100_WIDTH - 1; w++) {
      Serial.print(UTF8_HEX_HORIZONTAL_LINE);
    }
    Serial.print(UTF8_HEX_UPPER_RIGHT_CORNER);
    for (int y = 2; y < VT100_HEIGHT; y++) {
      term.position(y, 0);
      Serial.print(UTF8_HEX_VERTICAL_LINE);
      term.position(y, VT100_WIDTH);
      Serial.print(UTF8_HEX_VERTICAL_LINE);
    }
    term.position(VT100_HEIGHT, 0);
    Serial.print("\xE2\x94\x94");
    for (int w = 0; w < VT100_WIDTH - 1; w++) {
      Serial.print(UTF8_HEX_HORIZONTAL_LINE);
    }
    Serial.print(UTF8_HEX_LOWER_RIGHT_CORNER);
  }

}

void vt100DashDisplayTopLine(bool refreshAll) {
  if (refreshAll) {
    term.position(0, 0);
    term.print(F("SeaFlight Glider "));
    term.print(FIRMWARE_VERSION);
  }
  term.show_cursor(false);
  term.position(0, 80);
  term.print(currentDate);
  term.print(F(" "));
  term.print(currentTime);
}




/*
  term.position(0, 0);
  term.set_attribute(BT_NORMAL);
  term.set_attribute(BT_BOLD);
  term.print(F("Arduino Status Monitor\t"));

  term.set_attribute(BT_NORMAL);
  term.print(F("Current millis: "));
  term.print(now);
  term.print(F("\tmillis since last loop: "));
  term.set_attribute(BT_UNDERLINE);
  term.print(now - last);
  term.set_attribute(BT_NORMAL);

  term.position(1, 0);
  term.print(F("LED "));
  term.set_color(BT_GREEN, BT_BLACK);
  if (digitalRead(LED) == HIGH) {
    term.set_attribute(BT_BOLD | BT_UNDERLINE);
    term.print(F("ON "));
  } else {
    term.print(F("OFF"));
  }
  term.set_attribute(BT_NORMAL);

  term.position(DIGITAL_ROW, 0);
  term.print(F("Digital Readings"));

  for (i = 0; i <= 13; i++) {
    term.position(DIGITAL_ROW + 1, (i) * 6);
    term.set_attribute(BT_NORMAL);
    term.print("DIN");
    term.print(i);
    term.set_color(BT_RED, BT_BLACK);
    term.position(DIGITAL_ROW + 2, (i) * 6);
    term.print(digitalRead(i));
  }

  term.set_attribute(BT_NORMAL);
  term.position(ANALOG_ROW, 0);
  term.print(F("Analog Readings"));

  for (i = A0; i <= A5; i++) {
    term.position(ANALOG_ROW + 1, (i - A0) * 6);
    term.set_attribute(BT_NORMAL);
    term.print(F("AIN"));
    term.print(i - A0);
    term.set_color(BT_MAGENTA, BT_BLACK);
    term.position(ANALOG_ROW + 2, (i - A0) * 6);
    term.print(analogRead(i));
  }

  }
*/
