/*  SeaFlight Glider
    20x4 LCD Display

    Product Information:    
    More information:     https://www.sparkfun.com/products/16398
    Interfacing guide:    https://github.com/sparkfun/SparkFun_HMC6343_Arduino_Library

    Todo:
    
    
*/

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

#define LCD_REFRESH_DELAY     10

long previousMillisLCDRefresh = 0;
int displayCNT = 0;

void setupLCDDisplay() {
  display.begin(0x3C, true); // Address 0x3C default
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();

  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.print("Connecting to SSID\n'adafruit':");
  display.print("connected!");
  display.println("IP: 10.0.1.23");
  display.println("Sending val #0");
  display.display(); // actually display all of the above
  /*
  lcd.begin(Wire); //Set up the LCD for I2C communication

  lcd.setBacklight(200, 255, 255); //Set backlight to bright white
  lcd.setContrast(3); //Set contrast. Lower to 0 for higher contrast.

  lcd.clear(); //Clear the display - this moves the cursor to home position as well
  lcd.print("SeaFlightGlider 0.01");
  //disableMuxPort(LCD_MUX);
  */
  display.display();
  delay(1000);
}

void loopLCDDisplay() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisLCDRefresh > LCD_REFRESH_DELAY) {
    previousMillisLCDRefresh = millis();
    if (displayCNT++ > 10) {
      lcdWritePressureReservoir();
      lcdWritePressureBladder();
      lcdWritePumpTestState();
      lcdWriteTOF();
      lcdWriteInput();
      displayCNT = 0;
    }
    lcdWriteTimeDate();
  }
}

void lcdWriteTimeDate() {
  //lcd.setCursor(0, 0);
  //lcd.print("                 ");
  lcd.setCursor(0, 0);
  lcd.print(currentDate);
  lcd.print(currentTime);
}
void lcdWritePressureReservoir() {
  
  lcd.setCursor(0, 4);
  lcd.print("      ");
  lcd.setCursor(0, 4);
  lcd.print(latestPressureM300_RESERVOIR);
}

void lcdWritePressureBladder() {
 
  lcd.setCursor(6, 3);
  lcd.print("   ");
  lcd.setCursor(6, 3);
  lcd.print(latestPressureM300_BLADDER);
}

void lcdWriteInput() {
  if (stopButtonON) {
    lcd.setCursor(18, 4);
    lcd.print("S");
  }
  else {
    lcd.setCursor(18, 4);
    lcd.print(" ");
  }
}
void lcdWriteTOF() {
  lcd.setCursor(10, 3);
  lcd.print("         ");
  lcd.setCursor(10, 3);
  if (reservoirDistanceTOF == 255) {
    lcd.print("TOF=inf");
  } else {
    lcd.print("TOF=");
    lcd.print(reservoirDistanceTOF);
    lcd.print("cm");
  }
}

void lcdWritePumpTestState() {
  //enableMuxPort(LCD_MUX);
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 2);
  lcd.print("State=");
  //lcd.setCursor(7, 2);
  lcd.print(pumpTestStateStr[pumpTestState]);
  //lcd.print("     ");
  //disableMuxPort(LCD_MUX);
}
