
#define KEY_ENGINETEST_PUMPTEST             112     //letter 'p' lowercase
#define KEY_ENGINETEST_STEERINGTEST         115     //letter 's' lowercase
#define KEY_ENGINETEST_EXIT                 120     //letter 'x' lowercase 

void SetupEngineTest() {
  //changeEngineTestState(ENGINETEST_INIT);
}

void loopEngineTest() {
  unsigned long currentMillis = millis();
  byte b;
  if (testingState == ENGINE_TEST) {
    if (engineTestState == ENGINETEST_INIT) {
      changeEngineTestState(ENGINETEST_KEYPRESSWAIT);
    }
    if (engineTestState == ENGINETEST_STANDBY) {

    }
    if (engineTestState == ENGINETEST_KEYPRESSWAIT) {
      loopEngineTestStandbyRespondToKeyPresses();
    }
  }
}

void changeEngineTestState(int newState) {
  if (newState == ENGINETEST_INIT) {

  }
  else if (newState == ENGINETEST_STANDBY) {

  }
  else if (newState == ENGINETEST_KEYPRESSWAIT) {
    out.println("---------------------------------------");
    out.println("ENGINETEST_KEYPRESSWAIT accepting serial commands:");
    out.println("(s) steering test");
    out.println("(p) pump test");
    out.println("(x) to EXIT menu");
    out.println("---------------------------------------");
  }

  engineTestState = newState;
}

void loopEngineTestStandbyRespondToKeyPresses() {
  if (Serial.available()) {
    byte b = Serial.read();  // will not be -1
    if (b == KEY_ENGINETEST_PUMPTEST) {
      changeEngineTestState(ENGINETEST_PUMP_TEST);
      changePumpTestState(PUMP_KEYPRESSWAIT);
    }
    else if (b == KEY_ENGINETEST_STEERINGTEST) {
      changeEngineTestState(ENGINETEST_STEERING_TEST);
      changeSteeringTestState(STEER_KEYBOARDWAIT);
    }
  }
}
