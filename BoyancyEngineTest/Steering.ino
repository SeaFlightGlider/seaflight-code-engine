
#define KEY_STEER_TURBINE_1_TOGGLE      49     //letter '1' lowercase, kicks off pump cycle
#define KEY_STEER_TURBINE_2_TOGGLE      50     //letter '2' lowercase
#define KEY_STEER_TURBINE_1_FASTER      113     //letter 'q' lowercase
#define KEY_STEER_TURBINE_1_SLOWER      97     //letter 'a' lowercase
#define KEY_STEER_TURBINE_2_FASTER      119     //letter 'w' lowercase
#define KEY_STEER_TURBINE_2_SLOWER      115     //letter 's' lowercase
#define KEY_STEER_OFF                   111     //letter 'o' lowercase
#define KEY_STEER_QUIT                  120     //letter 'x' lowercase

#define SPEED_SLOW      1

#define TURBINE_1       1
#define TURBINE_2       2
#define TURBINE_1_PIN   6
#define TURBINE_2_PIN   2

bool validSteering = false;
int currentTurbine = TURBINE_1;
byte turbine1pin = TURBINE_1_PIN;
byte turbine2pin = TURBINE_2_PIN;

int currentTurbine1_PWM_ZEROTOTEN = 0;
int currentTurbine2_PWM_ZEROTOTEN = 0;

bool turbine1_ON = false;
bool turbine2_ON = false;

Servo turbine1;
Servo turbine2;

void setupSteering() {
  //changeSteeringTestState(STEER_STANDBY);
  validSteering = true;
}

void loopSteering() {
  if (validSteering) {
    unsigned long currentMillis = millis();
    byte b;
    if (engineTestState == ENGINETEST_STEERING_TEST) {
      if (steeringTestState == STEER_KEYBOARDWAIT) {
        loopSteeringStandbyRespondToKeyPresses();
      }
      else if (steeringTestState == STEER_INIT) {

      }
      else if (steeringTestState == STEER_STANDBY) {

      }
      else if (steeringTestState == STEER_OFF) {

      }
    }
  }
}
void changeSteeringTestState(enum SteeringTestState newState) {
  Log.info(module::STEER, "STEERING STATE CHANGE: from %s to %s; EngineState = %s\n", steeringTestStateStr[steeringTestState], steeringTestStateStr[newState], engineTestStateStr[engineTestState]);
  if (newState == STEER_INIT) {
    initSteering();

  }
  else if (newState == STEER_STANDBY) {

  }
  else if (newState == STEER_KEYBOARDWAIT) {
    out.println("---------------------------------------");
    out.println("STEER_KEYBOARDWAIT accepting serial commands:");
    out.println("(1) to control turbine 1");
    out.println("(2) to control turbine 2");
    out.println("(q) increase speed turbine 1");
    out.println("(a) decrease speed turbine 1");
    out.println("(w) increase speed turbine 2");
    out.println("(s) decrease speed turbine 2");
    out.println("(o) for all OFF");
    out.println("(x) to EXIT menu");
    out.println("---------------------------------------");

  }
  else if (newState == STEER_STEERING) {


  }
  else if (newState == STEER_OFF) {
    turbinesOFF();
  }

  steeringTestState = newState;
}

void initSteering() {
  turbine1.attach(turbine1pin);
  turbine1.writeMicroseconds(1500); // send "stop" signal to ESC.

  turbine2.attach(turbine2pin);
  turbine2.writeMicroseconds(1500); // send "stop" signal to ESC.
}

void turbinesOFF() {
  turbine1.writeMicroseconds(1500); // send "stop" signal to ESC.
  turbine2.writeMicroseconds(1500); // send "stop" signal to ESC.
}

void turbineOFF(int turbineNum) {
  if (turbineNum == TURBINE_1) {
    turbine1.writeMicroseconds(1500); // send "stop" signal to ESC.
  }
  else if (turbineNum == TURBINE_2) {
    turbine2.writeMicroseconds(1500); // send "stop" signal to ESC.
  }
}

//potValue = map(potValue, 0, 1023, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)
//  ESC.write(potValue);    // Send the signal to the ESC
//int signal = 1700; // Set signal value, which should be between 1100 and 1900

void turbinesONspeedOneToTen(int turbineNumber, int turbineSpeedOneToTen) {
  if (turbineNumber == TURBINE_1) {
    currentTurbine1_PWM_ZEROTOTEN = map(turbineSpeedOneToTen, 0, 1100, 0, 1900);
    turbine1.writeMicroseconds(currentTurbine1_PWM_ZEROTOTEN);
  }
  else if (turbineNumber == TURBINE_2) {
    currentTurbine2_PWM_ZEROTOTEN = map(turbineSpeedOneToTen, 0, 1100, 0, 1900);
    turbine2.writeMicroseconds(currentTurbine2_PWM_ZEROTOTEN);
  }
}

void loopSteeringStandbyRespondToKeyPresses() {
  if (Serial.available()) {
    byte b = Serial.read();  // will not be -1
    if (b == KEY_STEER_TURBINE_1_TOGGLE) {
      if (turbine1_ON) {
        turbine1_ON = false;
        turbineOFF(TURBINE_1);
      }
      else {
        turbine1_ON = true;
        currentTurbine1_PWM_ZEROTOTEN = SPEED_SLOW;
        turbinesONspeedOneToTen(TURBINE_1, currentTurbine1_PWM_ZEROTOTEN);
      }
    }
    else if (b == KEY_STEER_TURBINE_2_TOGGLE) {
      if (turbine2_ON) {
        turbine2_ON = false;
        turbineOFF(TURBINE_2);
      }
      else {
        turbine2_ON = true;
        currentTurbine2_PWM_ZEROTOTEN = SPEED_SLOW;
        turbinesONspeedOneToTen(TURBINE_2, currentTurbine2_PWM_ZEROTOTEN);
      }
    }
    else if (b == KEY_STEER_TURBINE_1_FASTER) {
      if (turbine1_ON) {
        if (currentTurbine1_PWM_ZEROTOTEN < 9) {
          currentTurbine1_PWM_ZEROTOTEN++;
          turbinesONspeedOneToTen(TURBINE_1, currentTurbine1_PWM_ZEROTOTEN);
        }
        else {

        }
      }
    }
    else if (b == KEY_STEER_TURBINE_2_FASTER) {
      if (turbine2_ON) {
        if (currentTurbine2_PWM_ZEROTOTEN < 9) {
          currentTurbine2_PWM_ZEROTOTEN++;
          turbinesONspeedOneToTen(TURBINE_1, currentTurbine2_PWM_ZEROTOTEN);
        }
        else {

        }
      }
    }
    else if (b == KEY_STEER_QUIT) {
      Serial.println("KEY_STEER_QUIT pressed...");
      changeSteeringTestState(STEER_OFF);
      changeSteeringTestState(STEER_STANDBY);
      changeEngineTestState(ENGINETEST_KEYPRESSWAIT);
    }
    else if (b == KEY_PUMP_IN_TOGGLE) {
      if (actualPumpOnIn) {
        pumpIn(OFF);
      } else {
        pumpIn(ON);
      }
    }
  }
}
