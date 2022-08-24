/*  SeaFlight Glider
    Bouancy engine pump test state machine + control

    PUMP In means pumping INTO the reservoir                            i.e. DIVING
    PUMP OUT means pumping out of the reservoir (and into the bladder)  i.e. CLIMBING

    Todo:
  Blue Robotics ESC:
  - Stopped  1500 microseconds
  - Max forward 1900 microseconds
  - Max reverse 1100 microseconds

  jeremy:
  180 - forward
  25 - reverse
  95 - neutral
  1 amp = 200ml/minute
  3.5 amps = 25 ml/minute

*/


#define RUN_CYCLES  2                //number of complete cycles (i.e. dives) to go through
#define PRESSURE_UPPER_LIMIT 15   //Pressure Upper Limit
#define PRESSURE_LOWER_LIMIT -5   //Pressure Lower Limit

#define ON  1
#define OFF 0

#define PUMP_INTERVAL_IN_POSTDELAY    5000    //amount of time between cycles.. 
#define PUMP_INTERVAL_OUT_POSTDELAY   5000    //amount of time between cycles.. 
#define PUMP_INTERVAL_IN_PUMP_ON      5000    //amount of time pump IN is ON
#define PUMP_INTERVAL_OUT_PUMP_ON     5000    //amount of time pump OUT is ON 
#define PUMP_INTERVAL_STANDBY         500    //mount of time LED blinks to show it's in standby

#define CYCLE_START_WITH_PUMP_IN 1

#define KEY_START_PUMP_TEST_CYCLE     116     //letter 't' lowercase, kicks off pump cycle
#define KEY_PUMP_IN_TOGGLE            105     //letter 'i' lowercase
#define KEY_PUMP_OUT_TOGGLE           111     //letter 'o' lowercase
#define KEY_SOLENOID_TOGGLE           115     //letter 's' lowercase
#define KEY_NEXT_STATE                110     //letter 'n' lowercase
#define KEY_VT100_DASH_TOGGLE         100     //letter 'd' lowercase
#define KEY_PUMP_EXIT                 120     //letter 'x' lowercase 

#define KEY_FIFTY_PERCENT_BUOYANCY        102     //letter 'f' lowercase
#define KEY_INCREASE_BUOYANCY             43      //character '+'  ...is this okay or are letters preferred, thought a '+' would make more sense
#define KEY_DECREASE_BUOYANCY             45      //character '-'

#define PUMP_PERCENTAGE_JUMP          2  

unsigned long previousMillisPUMP_IN = 0;
unsigned long previousMillisPUMP_OUT = 0;
unsigned long previousMillisPUMP_IN_ON = 0;
unsigned long previousMillisPUMP_OUT_ON = 0;
unsigned long previousMillisPUMP_STANDBY = 0;

int moveToThisReservoirFillPercentage = 0;


bool moveToThisReservoirFillPercentageAction = false;
bool actualPumpOnIn = OFF;
bool actualPumpOnOut = OFF;
bool actualSolenoidOn = OFF;
bool validPumpTest = false;

void setupPumpTest() {
  //changePumpTestState(PUMP_INIT);
  validPumpTest = true;
}

void loopPumpTest() {
  unsigned long currentMillis = millis();
  byte b;
  if (validPumpTest) {
    if (engineTestState == ENGINETEST_PUMP_TEST) {
      if (pumpTestState == PUMP_INIT) {
        changePumpTestState(PUMP_STANDBY);
      }
      else if (pumpTestState == PUMP_STANDBY) {

      }
      else if (pumpTestState == PUMP_KEYPRESSWAIT) {
        loopPumpStandbyRespondToKeyPresses();
        if (currentMillis - previousMillisPUMP_STANDBY > PUMP_INTERVAL_STANDBY) {
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
          previousMillisPUMP_STANDBY = millis();
        }
      }
      if (pumpTestState == PUMP_OFF) {  //pump_off is only to turn everything off, then jump to standby
        changePumpTestState(PUMP_STANDBY);
      }
      else if (pumpTestState == PUMP_IN_ON) {
        b = checkSerial();
        if (b == KEY_NEXT_STATE) {                  //pressing 'n' will skip to next state
          changePumpTestState(PUMP_IN_HOLD);
        }
        else if (currentMillis - previousMillisPUMP_IN_ON > PUMP_INTERVAL_IN_PUMP_ON) {
          changePumpTestState(PUMP_IN_HOLD);
        }
        else if (latestPressureM300_BLADDER >= PRESSURE_UPPER_LIMIT ) {
          changePumpTestState(PUMP_IN_HOLD);
        }
        else if (b == KEY_START_PUMP_TEST_CYCLE) { //press 't' again to stop test (toggle)
          changePumpTestState(PUMP_OFF);
        }
        else if (moveToThisReservoirFillPercentageAction == true)
        {
          if (reservoirFillPercentage >= moveToThisReservoirFillPercentage) 
          {
            changePumpTestState(PUMP_OFF);
            moveToThisReservoirFillPercentageAction = false;
          }
        }
      }
      else if (pumpTestState == PUMP_OUT_ON) {
        b = checkSerial();
        if (b == KEY_NEXT_STATE) {
          changePumpTestState(PUMP_OUT_HOLD);
        }
        else if (currentMillis - previousMillisPUMP_OUT_ON > PUMP_INTERVAL_OUT_PUMP_ON) {
          changePumpTestState(PUMP_OUT_HOLD);
        }
        else if (latestPressureM300_BLADDER < PRESSURE_LOWER_LIMIT) {
          changePumpTestState(PUMP_OUT_HOLD);
        }
        else if (b == KEY_START_PUMP_TEST_CYCLE) { //press 't' again to stop test (toggle)
          changePumpTestState(PUMP_OFF);
        }
        else if (moveToThisReservoirFillPercentageAction == true)
        {
          if (reservoirFillPercentage <= moveToThisReservoirFillPercentage) 
          {
            changePumpTestState(PUMP_OFF);
            moveToThisReservoirFillPercentageAction = false;
          }
        }
      }
      else if (pumpTestState == PUMP_IN_HOLD) {
        b = checkSerial();
        if (currentMillis - previousMillisPUMP_IN > PUMP_INTERVAL_IN_POSTDELAY || b == KEY_NEXT_STATE) {
          changePumpTestState(PUMP_OUT_ON);
        }
        else if (b == KEY_START_PUMP_TEST_CYCLE) { //press 't' again to stop test (toggle)
          changePumpTestState(PUMP_OFF);
        }
      }
      else if (pumpTestState == PUMP_OUT_HOLD) {
        b = checkSerial();
        if (currentMillis - previousMillisPUMP_OUT > PUMP_INTERVAL_OUT_POSTDELAY || b == KEY_NEXT_STATE) {
          changePumpTestState(PUMP_IN_ON);
        }
        else if (b == KEY_START_PUMP_TEST_CYCLE) { //press 't' again to stop test (toggle)
          changePumpTestState(PUMP_OFF);
        }
      }
      else if (pumpTestState == PUMP_OFF) {
        digitalWrite(SOLENOID_PIN, LOW);
        //turn pumpoff
      }
    }
  }
}

void changePumpTestState(enum PumpTestState newState) {
  Log.info(module::PUMP, "PUMP STATE CHANGE: from %s to %s\n", pumpTestStateStr[pumpTestState]);
  if (newState == PUMP_INIT) {
    //initControlPump();
  }
  else if (newState == PUMP_KEYPRESSWAIT) {
    out.println("---------------------------------------");
    out.println("PUMP_KEYPRESSWAIT accepting serial commands:");
    out.println("(t) to begin engine cycle test");
    out.println("(s) to toggle solenoid");
    out.println("(i) to toggle pump IN (into reservoir, out of bladder)");
    out.println("(o) to toggle pump OUT (out of reservoir, into bladder)");
    out.println("(x) to EXIT menu");
    out.println("(f) to pump to neutral buoyancy (50%)");
    out.println("(+) to INCREASE the buoyancy by 5%");
    out.println("(-) to DECREASE the buoyancy by 5%");
    out.println("---------------------------------------");
  }
  else if (newState == PUMP_IN_ON) {
    controlSolenoid(ON);
    pumpIn(ON);
    previousMillisPUMP_IN_ON = millis();
    out.println("State: PUMP_IN_ON press (n) to skip to PUMP_IN_HOLD");

  }
  else if (newState == PUMP_OUT_ON) {
    controlSolenoid(OFF);
    pumpOut(ON);
    previousMillisPUMP_OUT_ON = millis();
    out.println("State: PUMP_OUT_ON press (n) to skip to PUMP_OUT_HOLD");
  }
  else if (newState == PUMP_IN_HOLD) {
    controlSolenoid(OFF);
    pumpIn(OFF);
    previousMillisPUMP_IN = millis();       //mark time for timer for how long to be in 'hold' state
    out.print("State: PUMP_IN_HOLD seconds:");
    Serial.println(PUMP_INTERVAL_IN_POSTDELAY);
  }
  else if (newState == PUMP_OUT_HOLD) {
    controlSolenoid(OFF);
    pumpOut(OFF);
    previousMillisPUMP_OUT = millis();      //mark time for timer for how long to be in 'hold' state
    out.print("State: PUMP_IN_HOLD seconds:");
    Serial.println(PUMP_INTERVAL_IN_POSTDELAY);
  }
  else if (newState == PUMP_OFF) {
    controlSolenoid(OFF);
    pumpOut(OFF);
    pumpIn(OFF);
  }
  pumpTestState = newState;

#ifdef ENGINE_DEBUG_PRINT
  out.print("==============NEW PumpTestState = ");
  out.println(pumpTestStateStr[pumpTestState]);
#endif
}

void controlSolenoid(int turnON) {
  if (turnON) {
    digitalWrite(SOLENOID_PIN, HIGH);
    actualSolenoidOn = ON;
#ifdef ENGINE_DEBUG_PRINT
    out.println("----Solenoid ON----");
#endif
  }
  else {
    digitalWrite(SOLENOID_PIN, LOW);
    actualSolenoidOn = OFF;
#ifdef ENGINE_DEBUG_PRINT
    out.println("----Solenoid OFF----");
#endif
  }
}

void pumpIn(bool turnON) {
  if (!actualPumpOnOut) {
    if (turnON) {
#ifdef ENGINE_DEBUG_PRINT
      Serial.println("----PumpIN ON----");
#endif
      //turn pump on
      pumpMotorSpin(DIRECTION_IN, PWM_SLOW);
      actualPumpOnIn = ON;

    }
    else {
      //turn pump off
      pump_off();
      actualPumpOnIn = OFF;
#ifdef ENGINE_DEBUG_PRINT
      out.println("----PumpIN OFF----");
#endif
    }
  }
  else {
#ifdef ENGINE_DEBUG_PRINT
    out.println("----ERROR: pump is already ON OUT");
#endif
  }
}
void pumpOut(bool turnON) {
  if (!actualPumpOnIn) {
    if (turnON) {
#ifdef ENGINE_DEBUG_PRINT
      out.println("----PumpOUT ON----");
#endif
      //turn pump on
      pumpMotorSpin(DIRECTION_OUT, PWM_SLOW);
      actualPumpOnOut = ON;
    }
    else {
      //turn pump off
      actualPumpOnOut = OFF;
      pump_off();
#ifdef ENGINE_DEBUG_PRINT
      out.println("----PumpOUT OFF----");
#endif
    }
  }
  else {
#ifdef ENGINE_DEBUG_PRINT
    out.println("----ERROR: pump is already ON IN");
#endif
  }
}

void pumpTestProcessInput(byte incomingByte) {
  if (incomingByte == 49) {
    if (pumpTestState == PUMP_STANDBY) {
      changePumpTestState(PUMP_IN_ON);
    }
    else if (pumpTestState == PUMP_STANDBY) {
      //changePumpTestState(PUMP_IN_OFF);
    }

  }
}

void loopPumpStandbyRespondToKeyPresses() {
  if (Serial.available()) {
    byte b = Serial.read();  // will not be -1

    if (b == KEY_START_PUMP_TEST_CYCLE) {
      changePumpTestState(PUMP_IN_ON);
    }
    else if (b == KEY_FIFTY_PERCENT_BUOYANCY)  
    {
      pumpToFiftyPercentReservoir(reservoirFillPercentage);

    }
    // if '+' key is pressed, reservoir will fill 5 more.
    else if (b == KEY_INCREASE_BUOYANCY)
    {
      increaseBuoyancy(reservoirFillPercentage, 5);

    }

    else if (b == KEY_DECREASE_BUOYANCY)
    {
      decreaseBuoyancy(reservoirFillPercentage, 5);
    }

    else if (b == KEY_PUMP_IN_TOGGLE) {
      if (actualPumpOnIn) {
        pumpIn(OFF);
      } else {
        pumpIn(ON);
      }
#ifdef ENGINE_DEBUG_PRINT
      out.print("KEY_PUMP_IN_TOGGLE pumpval= ");
      out.println(actualPumpOnIn);
#endif
    }
    else if (b == KEY_PUMP_OUT_TOGGLE) {
      if (actualPumpOnOut) {
        pumpOut(OFF);
      } else {
        pumpOut(ON);
      }
#ifdef ENGINE_DEBUG_PRINT
      out.print("KEY_PUMP_OUT_TOGGLE pumpval= ");
      out.println(actualPumpOnOut);
#endif
    }
    else if (b == KEY_SOLENOID_TOGGLE) {
      if (actualSolenoidOn) {
        controlSolenoid(OFF);
      }
      else {
        controlSolenoid(ON);
      }
#ifdef ENGINE_DEBUG_PRINT
      out.print("KEY_SOLENOID_TOGGLE pumpval= ");
      out.println(actualSolenoidOn);
#endif
    }
    else if (b == KEY_PUMP_EXIT) {
      changePumpTestState(PUMP_OFF);
      changePumpTestState(PUMP_STANDBY);
      changeEngineTestState(ENGINETEST_KEYPRESSWAIT);
    }
    else if (b == KEY_VT100_DASH_TOGGLE) {
      Serial.end();
      if (displayVT100Dash) {
        displayVT100Dash = false;
      }
      else {
        displayVT100Dash = true;
      }
#ifdef ENGINE_DEBUG_PRINT
      out.print("KEY_VT100_DASH_TOGGLE displayVT100Dash= ");
      out.println(displayVT100Dash);
#endif
    }
  }
  else {
    //Serial.println("loopPumpStandbyRespondToKeyPresses serial NOT available");
  }
}

byte checkSerial() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    byte incomingByte = Serial.read();

    // say what you got:
    //Serial.print("I received: ");
    //Serial.println(incomingByte, DEC);
    //pumpTestProcessInput(incomingByte);
    return incomingByte;
  }
  return -1;
}

void vt100DashDisplayStateMachine(bool refreshAll, int x, int y) {
  if (refreshAll) {

  }
  // pumpTestStateStr[pumpTestState]
  term.position(y, x);
  term.print(F("State Machine"));
  int y_off = 1;
  term.position(y + y_off, x);
  term.print(F("Pump Test State:"));
  term.print(pumpTestStateStr[pumpTestState]);
}

void pumpToFiftyPercentReservoir(int reservoirFillPercentage)
{  
  if (reservoirFillPercentage > 50)
  {
      
    if(reservoirFillPercentage != 50)  // function that needs to loop and check itself, while loop?? 
    {
      moveToThisReservoirFillPercentageAction = true;
      moveToThisReservoirFillPercentage = 50;
      changePumpTestState(PUMP_OUT_ON);
    }
        
        
  }
  else if (reservoirFillPercentage < 50)
  {    
    moveToThisReservoirFillPercentage = 50;
    moveToThisReservoirFillPercentageAction = true;
    changePumpTestState(PUMP_IN_ON);
  }
    else if (reservoirFillPercentage == 50)
    {
      Log.error(module::SENSOR, "TOF: Buoyancy already at 50%\n");
    }
}

void increaseBuoyancy(int reservoirFillPercentage, int desiredFillPercentageOffset)
{
  int error = 5; 
  moveToThisReservoirFillPercentage = reservoirFillPercentage + desiredFillPercentageOffset; 

  if(moveToThisReservoirFillPercentage < (100 - error - desiredFillPercentageOffset) && moveToThisReservoirFillPercentage > (0 + error + desiredFillPercentageOffset))
  {
    moveToThisReservoirFillPercentageAction = true; 
    changePumpTestState(PUMP_OUT_ON);
  }
  else
  {
    Log.error(module::SENSOR, "TOF: Failed to increase buoyancy\n");
  }

}

void decreaseBuoyancy(int reservoirFillPercentage, int desiredFillPercentageOffset)
{
  int error = 5;
  moveToThisReservoirFillPercentage = reservoirFillPercentage - desiredFillPercentageOffset; 

  if(moveToThisReservoirFillPercentage < (100 - error - desiredFillPercentageOffset) && moveToThisReservoirFillPercentage > (0 + error + desiredFillPercentageOffset))
  {
    moveToThisReservoirFillPercentageAction = true;
    changePumpTestState(PUMP_OUT_ON);
  }
  else
  {
    Log.error(module::SENSOR, "TOF: Failed to decrease buoyancy.\n");
  }

}