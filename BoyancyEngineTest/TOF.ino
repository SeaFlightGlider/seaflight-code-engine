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
#define PRESSURE_SENSOR_REFRESH_RATE 5000 //in milliseconds // change back to 5000

uint8_t reservoirDistanceTOF = 0;
bool validTOF = false;
unsigned long previousMillisTOF = 0;
long intervalTOF = PRESSURE_SENSOR_REFRESH_RATE;

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

      Log.info(module::SENSOR, ", TOF:, Range= %hhu , Lux=%f\n", reservoirDistanceTOF, lux);


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
