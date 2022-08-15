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
  display.print("SeaFlight Glider V0.1 Starting..");
  display.display(); // actually display all of the above
  /*
  display.begin(Wire); //Set up the LCD for I2C communication

  display.setBacklight(200, 255, 255); //Set backlight to bright white
  display.setContrast(3); //Set contrast. Lower to 0 for higher contrast.

  display.clear(); //Clear the display - this moves the cursor to home position as well
  display.print("SeaFlightGlider 0.01");
  //disableMuxPort(LCD_MUX);
  */
  delay(1000);
  display.clearDisplay();
  display.display();
}

void serialDisplay(int serialDelay){
  display.clearDisplay();
  display.setCursor(0,0);
  if (Serial){
    display.print("serial connected ");
  } else{
    display.print("looking for serial connection ");
  }
  display.print(serialDelay);
  display.display();
}

void loopLCDDisplay() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisLCDRefresh > LCD_REFRESH_DELAY) {
    previousMillisLCDRefresh = millis();
    if (displayCNT++ > 1) {
      display.clearDisplay();
      displayWritePressureReservoir();
      displayWritePressureBladder();
      displayWritePumpTestState();
      displayWriteTOF();
      displaySolenoidState();
      displayCNT = 0;
    }
    //displayWriteTimeDate();
    display.display();
  }
}

void displayWritePressureReservoir() {
  if (validPressureBLADDER){
    display.setCursor(0, 20);
    display.print("reservoir P=");
    display.print(latestPressureData.pressureRESERVOIR);
  }
}

void displayWritePressureBladder() {
  if (validPressureBLADDER) {
    display.setCursor(0, 10);
    display.print("Bladder P=");
    display.print(latestPressureData.pressureBLADDER);
  }
}

void displayWriteTOF() {
  display.setCursor(0, 0);
  if (reservoirDistanceTOF == 255) {
    display.print("TOF=inf");
  } else {
    display.print("TOF=");
    display.print(reservoirDistanceTOF);
    display.print("mm");
  }
}

void displayWritePumpTestState() {
  display.setCursor(0, 30);
  display.print(pumpTestStateStr[pumpTestState]);
}

void displaySolenoidState() {
  display.setCursor(0, 40);
  if(actualSolenoidOn) display.print("solenoid on");
  display.setCursor(0, 50);
  if(actualPumpOnOut) display.print("pump out");
  if(actualPumpOnIn) display.print("pump In");
}


