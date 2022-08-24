/*  SeaFlight Glider
    Time Of Flight VL6180 Range Finder
    Mounted to measure location of pressure reservoir


    More information:     https://www.sparkfun.com/products/12785
    Library:              https://github.com/sparkfun/SparkFun_ToF_Range_Finder-VL6180_Arduino_Library/tree/V_1.1.0

    Todo:
    - add SD Card Interface
    - add time since last update
    - add clock sync
    -
*/

#define PRESSURE_SENSOR_REFRESH_RATE 200 //in milliseconds // change back to 5000
#define TOFmin 160 
#define TOFmax 20

int reservoirDistanceTOF = 0;
unsigned long previousReservoirDistanceTOF = 0;
int reservoirFillPercentage = 0;
bool validTOF = false;
unsigned long previousMillisTOF = 0;
long intervalTOF = PRESSURE_SENSOR_REFRESH_RATE;

int pumpInRotations = 0;
int pumpOutRotations = 0;



Adafruit_VL6180X vl = Adafruit_VL6180X();

void setupTOF() {
  if (!vl.begin()) {
    validTOF = false;
    Log.error(module::SENSOR, "NOT DETECTED: VL6180 Time Of Flight reservoir Sensor\n");
  } else {
    Log.info(module::SENSOR, "DETECTED: VL6180 Time Of Flight reservoir Sensor\n");
    validTOF = true;
  }
}

void loopTOF() {
  if (validTOF) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisTOF > intervalTOF) {
      previousMillisTOF = currentMillis;

      float lux = vl.readLux(VL6180X_ALS_GAIN_5);
      uint8_t range = vl.readRange();
      uint8_t status = vl.readRangeStatus();
      reservoirDistanceTOF = range;
      reservoirFillPercentage = TOFtoPercentage();

      Log.info(module::SENSOR, "TOF: Range= %hhu Lux=%f\n", reservoirDistanceTOF, lux);
      Log.info(module::SENSOR, "TOF: Range= %hhu", reservoirDistanceTOF);
      Log.info(module::SENSOR, "TOF: Percentage= %hhu\n", reservoirFillPercentage);


      

      if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5) && SENSOR_DEBUG_PRINT) {
        Log.error(module::SENSOR, "TOF: VL6180X_ERROR_SYSERR_1\n");
      }
      else if (status == VL6180X_ERROR_ECEFAIL && SENSOR_DEBUG_PRINT) {
        Log.error(module::SENSOR, "TOF: VL6180X ECE failure\n");
      }
      else if (status == VL6180X_ERROR_NOCONVERGE && SENSOR_DEBUG_PRINT) {
        Log.error(module::SENSOR, "TOF: VL6180X No convergence\n");
      }
      else if (status == VL6180X_ERROR_RANGEIGNORE && SENSOR_DEBUG_PRINT) {
        Log.error(module::SENSOR, "TOF: VL6180X Ignoring range\n");
      }
      else if (status == VL6180X_ERROR_SNR && SENSOR_DEBUG_PRINT) {
        Log.error(module::SENSOR, "TOF: VL6180X Signal/Noise error\n");
      }
      else if (status == VL6180X_ERROR_RAWOFLOW && SENSOR_DEBUG_PRINT) {
        Log.error(module::SENSOR, "TOF: VL6180X Raw reading overflow\n");
      }
      else if (status == VL6180X_ERROR_RANGEUFLOW && SENSOR_DEBUG_PRINT) {
        Log.error(module::SENSOR, "TOF: VL6180X Range reading underflow\n");
      }
      else if (status == VL6180X_ERROR_RANGEOFLOW && SENSOR_DEBUG_PRINT) {
        Log.error(module::SENSOR, "TOF: VL6180X Range reading overflow\n");
      }
    }
  }
}

// converts reservoir fill value to a percentage based on how full it is
int TOFtoPercentage() //maybe not a global variable
{
  if (reservoirDistanceTOF < TOFmin || reservoirDistanceTOF > TOFmax) //validates that value is in range
  {
    int percent; 
    if(reservoirDistanceTOF == 20) // edge case
    {
      return 100;
    }
    else if(reservoirDistanceTOF == 160) // edge case
    {
      return 0;
    }
    else
    {
      percent = (((reservoirDistanceTOF - TOFmin) * 100) / (TOFmin - TOFmax)); // formula to find % (reversed)
      percent = abs(percent);
      return percent;
    }
    
  }
  else
  {
    Log.error(module::SENSOR, "TOF: Out of range\n");
  }
}

void validatePumpStates() 
{
  //look for change in state
  previousReservoirDistanceTOF = reservoirDistanceTOF; //make global, future array

  //how much fluid and volume... find volume and solve for height. Compare height to TOF sensor. 


  if (pumpTestState == PUMP_OFF) 
  {
    if((reservoirDistanceTOF - previousReservoirDistanceTOF) < 3)
    {
      Log.error(module::SENSOR, "TOF: Pump leak failure, oil leaving reservoir while pump is OFF\n"); // leak due to piston distance growing from sensor
    }
    else if((reservoirDistanceTOF - previousReservoirDistanceTOF) < -3)
    {
      Log.error(module::SENSOR, "TOF: Pump failure, oil going to reservoir while pump is OFF\n"); // pump is pumping too much, piston distance decreasing from sensor
    }
    else
    {
      break;
    }
  }

  if (pumpTestState == PUMP_IN_ON)
  { 
    if((reservoirDistanceTOF - previousReservoirDistanceTOF) < 5)   // pumping more into reservoir, should not be increasing. If positive we have problem.
    {
      Log.error(module::SENSOR, "TOF: Pump failure, increasing when should be decreasing\n");
    }
    else if(pumpInRotations < /*correct amount of rotations*/) //pump speed required to ensure pump is functioning as intended.
    {
      Log.error(module::SENSOR, "TOF: Pump failure, not pumping in correctly (too slow)\n"); //?
    } 
    else if (pumpInRotations > /*correct amount of rotations*/)
    {
      Log.error(module::SENSOR, "TOF: Pump failure, not pumping in correctly (too fast)\n");//?
    }
    else
    {
      break;
    }
  }

  if (pumpTestState == PUMP_OUT_ON)
  {
    if((reservoirDistanceTOF - previousReservoirDistanceTOF) < -5)   // pumping out of reservoir, should not be decreaing. If negative we have problem.
    {
      Log.error(module::SENSOR, "TOF: Pump failure, decreasing when should be increasing\n");
    }
    else if(pumpOutRotations < /*correct amount of rotations*/) //pump speed required to ensure pump is functioning as intended.
    {
      Log.error(module::SENSOR, "TOF: Pump failure, not pumping out correctly (too slow)\n"); //?
    } 
    else if(pumpOutRotations > /*correct amount of rotations*/)
    {
      Log.error(module::SENSOR, "TOF: Pump failure, not pumping out correctly (too fast)\n"); //?
    }
    else
    {
      break;
    }
  }

}


