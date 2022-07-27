/*  SeaFlight Glider
    Power current + voltage monitoring

    INA260
    More information:     https://www.adafruit.com/product/4226
    Interfacing guide:    https://github.com/adafruit/Adafruit_INA260

    Todo:
      - create 'sampling' modes where

*/
#include <Adafruit_INA260.h>

#define POWER_SENSOR_REFRESH_RATE 10000 //in milliseconds

#define PWR_ADDRESS_5V        0x41
#define PWR_ADDRESS_3V        0x40
#define PWR_ADDRESS_ENGINE    0x44
#define PWR_ADDRESS_PUMP      0x45
#define PWR_ADDRESS_SOLENOID  0x43
#define PWR_ADDRESS_TURBINE   0x45

#define TOTAL_CURRENT_SENSORS 6

unsigned long previousMillisPressurePower = 0;
unsigned long intervalPowerSensors = POWER_SENSOR_REFRESH_RATE;
//Sets PowerMeasurment as structure
struct PowerMeasurement {
  int timee = 0;
  char address = 0x00;
  bool valid = false;
  float current = 0.0;
  float busVoltage = 0.0;
  float power = 0.0;
  Adafruit_INA260 pwrMon;
  const char* busName;
};
//
PowerMeasurement pwrMons[TOTAL_CURRENT_SENSORS] = {
  {millis(), PWR_ADDRESS_3V, false, 0, 0.0, 0.0, Adafruit_INA260(), "3V"},
  {millis(), PWR_ADDRESS_5V, false, 0, 0.0, 0.0, Adafruit_INA260(), "5V"},
  {millis(), PWR_ADDRESS_ENGINE, false, 0, 0.0, 0.0, Adafruit_INA260(), "Engine"},
  {millis(), PWR_ADDRESS_PUMP, false, 0, 0.0, 0.0, Adafruit_INA260(), "Pump"},
  {millis(), PWR_ADDRESS_SOLENOID, false, 0, 0.0, 0.0, Adafruit_INA260(), "Turbine"},
  {millis(), PWR_ADDRESS_TURBINE, false, 0, 0.0, 0.0, Adafruit_INA260(), "Solenoid"}
};

void setupPower() {
  for (int x = 0; x < TOTAL_CURRENT_SENSORS; x++) {
    if (pwrMons[x].pwrMon.begin(pwrMons[x].address)) {
      pwrMons[x].valid = true;
      Log.info(module::POWER,"DETECTED: INA260 chip %s \n", pwrMons[x].busName);   
    }
    else {
      pwrMons[x].valid = false;
      Log.error(module::POWER,"NOT DETECTED: INA260 chip %s \n", pwrMons[x].busName); 
    }
  }

}

void loopPower() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisPressurePower > intervalPowerSensors) {
    previousMillisPressurePower = currentMillis;
    for (int x = 0; x < TOTAL_CURRENT_SENSORS; x++) {
      if (pwrMons[x].valid == true) {
        pwrMons[x].current = pwrMons[x].pwrMon.readCurrent();
        pwrMons[x].busVoltage = pwrMons[x].pwrMon.readBusVoltage();
        pwrMons[x].power = pwrMons[x].pwrMon.readPower();
        //Dont even need to fix V because I just need to add time stamp then format it.
        if(timecode ==0){
          Log.info(module::SETUP,", Time:    , POWER:   , Voltage:    , Current:    , Power:    \n");
          timecode = 1;
        }
        else{
          Log.info(module::LOOP,", %u   , %s    , %F    , %F    , %F     \n", millis(), pwrMons[x].busName, pwrMons[x].busVoltage, pwrMons[x].current, pwrMons[x].power);
        }
        
      }
      else {

      }
    }
  }
}
