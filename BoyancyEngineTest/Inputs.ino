#include <SparkFun_Qwiic_Button.h>          // library: https://github.com/sparkfun/SparkFun_Qwiic_Button_Arduino_Library
QwiicButton redStopButton;

bool stopButtonON = false;
bool validButtonsFound = false;

void setupInputs() {
  if (redStopButton.begin() == false) {
    Serial.println("Red Button did not acknowledge! Freezing.");
    //while (1);
  } else {
    validButtonsFound = true;
  }
}

void loopInputs() {
  if (validButtonsFound) {
    if (redStopButton.isPressed() == true) {
      stopButtonON = true;
      Serial.println("redStopButton PRESSED");
      changePumpTestState(PUMP_OFF);
    }
    else {
      stopButtonON = false;
    }
  }
}
