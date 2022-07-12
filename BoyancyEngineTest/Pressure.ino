/*  SeaFlight Glider
    MSP300 Pressure Transducer


    More information:     https://bit.ly/39e2zJF
    Interfacing guide:    https://bit.ly/3c8NAT2

    Todo:
    - error handling - process the status bits returned
    - The MSP300 can return temperature - worth adding as another data point
*/
#include "KellerLD.h"       // https://github.com/bluerobotics/BlueRobotics_KellerLD_Library

#define PRESSURE_M300_BLADDER 0
#define PRESSURE_M300_RESERVOIR 1
#define PRESSURE_SENSOR_REFRESH_RATE 5000 //in milliseconds
#define PRESSURE_SENSOR_STARTUP_DELAY 5000

long latestPressureM300_BLADDER = 0;
long latestPressureM300_RESERVOIR = 0;
unsigned long previousMillisPressureM300 = 0;
unsigned long intervalPressureM300 = 5000;
bool validPressureBLADDER = false;
bool validPressureRESERVOIR = false;
bool validPressureEXTERNAL = false;
bool setupStarted = false;

#define MEM_LEN 2
char databuf[MEM_LEN];


struct PressureSensorsData {
  float pressureBLADDER = 0.0;
  float pressureRESERVOIR = 0.0;
  float pressureEXTERNAL = 0.0;
  float temperatureEXTERNAL = 0.0;
  float depth = 0.0;
  float altitudee = 0.0;
};

//PressureM300 pressureM300_Bladder = { 255 , 0 , 0 , 0};
//PressureM300 pressureM300_Reservoir = { 255 , 0 , 1, 0 };
KellerLD sensorExtPressureKellerLD;
PressureSensorsData latestPressureData;

void setupPressure() {
  Wire1.begin(I2C_MASTER, 0x00, I2C_PINS_37_38, I2C_PULLUP_INT, 400000);
  Wire1.setDefaultTimeout(200000); // 200ms
  //setupStarted = true;
  latestPressureM300_BLADDER = getPressure_M300_BLADDER(false);
  if (validPressureBLADDER) {
    Log.info(module::SENSOR, "DETECTED: M300 Pressure BLADDER Sensor\n");
  }
  else {
    Log.info(module::SENSOR, "NOT DETECTED: M300 Pressure BLADDER Sensor\n");
  }

  latestPressureM300_RESERVOIR = getPressure_M300_RESERVOIR(false);
  if (validPressureRESERVOIR) {
    Log.info(module::SENSOR, "DETECTED: M300 Pressure RESERVOIR Sensor\n");
  }
  else {
    Log.error(module::SENSOR, "NOT DETECTED: M300 Pressure RESERVOIR Sensor\n");
  }

  sensorExtPressureKellerLD.init();
  sensorExtPressureKellerLD.setFluidDensity(1029); // 997 kg/m^3 (freshwater, 1029 for seawater)

  if (sensorExtPressureKellerLD.isInitialized()) {
    Log.info(module::SENSOR, "DETECTED: BAR100 Pressure EXTERNAL Sensor\n");
    validPressureEXTERNAL = true;
  } else {
    Log.error(module::SENSOR, "NOT DETECTED: BAR100 Pressure EXTERNAL Sensor\n");
    validPressureEXTERNAL = true;
  }
}

void loopPressure() {
  if (setupStarted) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisPressureM300 > intervalPressureM300) {
      previousMillisPressureM300 = currentMillis;
    }
  }
  if (validPressureRESERVOIR || validPressureRESERVOIR) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillisPressureM300 > intervalPressureM300) {
      previousMillisPressureM300 = currentMillis;

      if (validPressureBLADDER) {
        getPressure_M300_BLADDER(true);
      }
      if (validPressureRESERVOIR) {
        getPressure_M300_RESERVOIR(true);
      }
      if (validPressureEXTERNAL) {
        getPressure_EXTERNAL(true);
      }
    }
  }
}

void getPressure_EXTERNAL(bool logprint)
{
  sensorExtPressureKellerLD.read();
  latestPressureData.pressureEXTERNAL = sensorExtPressureKellerLD.pressure();
  latestPressureData.temperatureEXTERNAL = sensorExtPressureKellerLD.temperature();
  latestPressureData.depth = sensorExtPressureKellerLD.depth();
  latestPressureData.altitudee = sensorExtPressureKellerLD.altitude();  //yes, we are an underwater robot, but you never know..
  if (logprint) {
    Log.info(module::SENSOR, "SENSOR EXTERNAL Pressure=%f temperature=%f depth=%f, altitude=%f\n", latestPressureData.pressureEXTERNAL, latestPressureData.temperatureEXTERNAL, latestPressureData.depth, latestPressureData.altitudee);
  }
}

long getPressure_M300_BLADDER(bool logprint)
{
  int maxPressure = 300;  //M30J1-000105-300PG
  float pressure = 0.0;
  float temperature = 0.0;
  uint16_t rawP;     // pressure data from sensor
  uint16_t rawT;     // temperature data from sensor

  int bytesReturned = Wire.requestFrom(40, 4, true); // Read from Slave (string len unknown, request full buffer)
  if (Wire.getError() || bytesReturned != 4) {
    validPressureBLADDER = false;
  }
  else {
    validPressureBLADDER = true;
    while (Wire.available() == 0);  //consider collecting data inside this while loop  while(Wire.available()) {
    rawP = (uint16_t) Wire.read();    // upper 8 bits
    rawP <<= 8;
    rawP |= (uint16_t) Wire.read();    // lower 8 bits
    rawT = (uint16_t)  Wire.read();    // upper 8 bits
    rawT <<= 8;
    rawT |= (uint16_t) Wire.read();   // lower 8 bits

    byte status = rawP >> 14;   // The status is 0, 1, 2 or 3
    rawP &= 0x3FFF;   // keep 14 bits, remove status bits
    rawT >>= 5;     // the lowest 5 bits are not used
    pressure = ((rawP - 1000.0) / (15000.0 - 1000.0)) * maxPressure;
    temperature = ((rawT - 512.0) / (1075.0 - 512.0)) * 55.0;

    //  Pressure Sensor info:
    //  Manufacturer: TE  Part number: M30J1-000105-300PG
    //  i2c, 17-4PH Stainless Steel, Sleep Mode, i2c add=0X28H, 1/4-18 NPT, psi std=300, bar std=020B, pressure ref = guage
    //  Pmax = 300
    //  Pmin = 0
    // output(decimal counts) = (15000-1000)/(Pmax-Pmin) x (Papplied - Pmin) + 1000

    // float Lmax=100,Lmin=0；//Span 100L，Zero 0L, Span should be defined by the sensor
    // pressure range of customer used. 100 means pressure range of 100L
    // u32 Pvalue,Tvalue,Tspan,Pspan;
    // u16 P1=1000,P2=15000;
    // Pspan=P2-P1;
    // Tdisplay=Tvalue*Tscope/Tspan-50;
    // Pdisplay=Pvalue*(Lmax-Lmin)/Pspan+Lmin;//100L

    //long Pressure = (finalnum - 1000) / ((15000 - 1000) / 300);

    Wire.beginTransmission(40);  //this might not be needed by sending a 'true' in the wire arguments
    Wire.write(0);
    Wire.endTransmission();
    if (logprint) {
      Log.info(module::SENSOR, "SENSOR BLADDER Pressure=%fpsi, temp=%fc, stat=%hhx\n", pressure, temperature, status);
      latestPressureData.pressureBLADDER = pressure;
    }
  }
  return pressure;
}

float getPressure_M300_RESERVOIR(bool logprint)
{
  int maxPressure = 300;  //M30J1-000105-300PG
  float pressure = 0.0;
  float temperature = 0.0;
  uint16_t rawP;     // pressure data from sensor
  uint16_t rawT;     // temperature data from sensor

  int bytesReturned = Wire1.requestFrom(40, 4, true); // Read from Slave (string len unknown, request full buffer)
  if (Wire1.getError() || bytesReturned != 4) {
    validPressureRESERVOIR = false;
  }
  else {
    validPressureRESERVOIR = true;
    while (Wire1.available() == 0);  //consider collecting data inside this while loop  while(Wire.available()) {
    rawP = (uint16_t) Wire1.read();    // upper 8 bits
    rawP <<= 8;
    rawP |= (uint16_t) Wire1.read();    // lower 8 bits
    rawT = (uint16_t)  Wire1.read();    // upper 8 bits
    rawT <<= 8;
    rawT |= (uint16_t) Wire1.read();   // lower 8 bits

    byte status = rawP >> 14;   // The status is 0, 1, 2 or 3
    rawP &= 0x3FFF;   // keep 14 bits, remove status bits
    rawT >>= 5;     // the lowest 5 bits are not used
    pressure = ((rawP - 1000.0) / (15000.0 - 1000.0)) * maxPressure;
    temperature = ((rawT - 512.0) / (1075.0 - 512.0)) * 55.0;
    //    long Pval = Wire1.read();              //MSB
    //    long Pval2 = Wire1.read();             //LSB
    //    int pressureStatus = Pval & 0xC0;     // 0xC0 = 1100 0000
    //    pressureStatus = pressureStatus >> 6;
    //    Pval = Pval & 0x3F;                   //mask out the status bits 0x3F = 0011 1111
    //    long Pvalshift = Pval << 8;
    //    long finalnum = Pvalshift | Pval2;

    //  Pressure Sensor info:
    //  Manufacturer: TE  Part number: M30J1-000105-300PG
    //  i2c, 17-4PH Stainless Steel, Sleep Mode, i2c add=0X28H, 1/4-18 NPT, psi std=300, bar std=020B, pressure ref = guage
    //  Pmax = 300
    //  Pmin = 0
    // output(decimal counts) = (15000-1000)/(Pmax-Pmin) x (Papplied - Pmin) + 1000

    //    float Lmax=100,Lmin=0；//Span 100L，Zero 0L, Span should be defined by the sensor
    //pressure range of customer used. 100 means pressure range of 100L
    //u32 Pvalue,Tvalue,Tspan,Pspan;
    //u16 P1=1000,P2=15000;
    // Pspan=P2-P1;
    // Tdisplay=Tvalue*Tscope/Tspan-50;
    // Pdisplay=Pvalue*(Lmax-Lmin)/Pspan+Lmin;//100L

    //long Pressure = (finalnum - 1000) / ((15000 - 1000) / 300);

    Wire1.beginTransmission(40);  //this might not be needed by sending a 'true' in the wire arguments
    Wire1.write(0);
    Wire1.endTransmission();

    Log.info(module::SENSOR, "SENSOR RESERVOIR Pressure=%fpsi, temp=%fc, stat=%hhx\n", pressure, temperature, status);
    //Log.info(module::SENSOR, "SENSOR RESERVOIR Pressure %ld\n", Pressure);
    latestPressureData.pressureRESERVOIR = pressure;

  }
  return pressure;
}

void vt100DashDisplayPressure(bool refreshAll, int x, int y) {
  if (refreshAll) {
    //term.position(0, 0);
    //term.print(F("SeaFlight Glider "));
    //term.print(FIRMWARE_VERSION);
  }
  term.show_cursor(false);
  //if (validPressureSensors) {
  term.position(y, x + 4);
  term.print(F("Pressure"));
  term.position(y + 1, x);
  term.print(F("Reservoir:"));
  term.position(y + 1, x + 11);
  term.print(latestPressureM300_RESERVOIR);
  term.print(F(" Psi"));
  term.position(y + 2, x);
  term.print(F("Bladder: "));
  term.position(y + 2, x + 11);
  term.print(latestPressureM300_BLADDER);
  term.print(F(" Psi"));
  //}

}

//from https://forum.arduino.cc/t/interfacing-with-a-m3200-series-i2c-pressure-sensor/670139/5
//int M3200address = ?   // 0x28, 0x36 or 0x46, depending on the sensor.
//float maxPressure = ?   // pressure in PSI for this sensor, 100, 250, 500, ... 10k.
//
//int n = Wire.requestFrom( M3200address, 4);   // request 4 bytes
//if( n == 4)
//{
// uint16_t rawP;     // pressure data from sensor
// uint16_t rawT;     // temperature data from sensor
// rawP = (uint16_t) Wire.read();    // upper 8 bits
// rawP <<= 8;
// rawP |= (uint16_t) Wire.read();    // lower 8 bits
// rawT = (uint16_t)  Wire.read();    // upper 8 bits
// rawT <<= 8;
// rawT |= (uint16_t) Wire.read();   // lower 8 bits
//
// byte status = rawP >> 14;   // The status is 0, 1, 2 or 3
// rawP &= 0x3FFF;   // keep 14 bits, remove status bits
//
// rawT >>= 5;     // the lowest 5 bits are not used
//
// // The math could be done with integers, but I choose float for now
// float pressure = ((rawP - 1000.0) / (15000.0 - 1000.0)) * maxPressure;
// float temperature = ((rawT - 512.0) / (1075.0 - 512.0)) * 55.0;
//
// Serial.print( "Status = ");
// Serial.print( status);
// Serial.print( ", Pressure = ");
// Serial.print( pressure);
// Serial.print( " psi, Temperature = ");
// Serial.print( temperature);
// Serial.print( " *C");
// Serial.println();
//}
//else
//{
// Serial.println( "Sensor not found");
//}



// ORIGINAL = FLAKY
//int getPressure()
//{
//  //start the communication with IC with the address xx
//  Wire.beginTransmission(40);
//  //send a bit and ask for register zero
//  Wire.write(0);
//  //end transmission
//  Wire.endTransmission();
//  //request 2 byte from address xx
//  Wire.requestFrom(40, 2);
//  //wait for response
//  while (Wire.available() == 0);
//  int Pval = Wire.read();
//  int Pvalshift = Pval << 8;
//  int Pval2 = Wire.read();
//  int Pval3 = Wire.read();
//  long finalnum = Pvalshift | Pval2;
//  long Pressure = (finalnum - 1000) / ((15000 - 1000) / 300);
//  //Serial.print("getPressure | ");
//  //Serial.println(Pressure);
//  return Pressure;
//}



//working... but could be better, needs temp + proper status interpretation:
//long getPressure_M300_RESERVOIR()
//{
//  Wire1.requestFrom(40, 2); // Read from Slave (string len unknown, request full buffer)
//  if (Wire1.getError()) {
//    validPressureRESERVOIR = false;
//  }
//  else {
//    validPressureRESERVOIR = true;
//    while (Wire1.available() == 0);  //consider collecting data inside this while loop  while(Wire.available()) {
//    long Pval = Wire1.read();              //MSB
//    long Pval2 = Wire1.read();             //LSB
//    int pressureStatus = Pval & 0xC0;     // 0xC0 = 1100 0000
//    pressureStatus = pressureStatus >> 6;
//    Pval = Pval & 0x3F;                   //mask out the status bits 0x3F = 0011 1111
//    //int Pval3 = Wire.read();
//    long Pvalshift = Pval << 8;
//    long finalnum = Pvalshift | Pval2;
//
//    //  Pressure Sensor info:
//    //  Manufacturer: TE  Part number: M30J1-000105-300PG
//    //  i2c, 17-4PH Stainless Steel, Sleep Mode, i2c add=0X28H, 1/4-18 NPT, psi std=300, bar std=020B, pressure ref = guage
//    //  Pmax = 300
//    //  Pmin = 0
//    // output(decimal counts) = (15000-1000)/(Pmax-Pmin) x (Papplied - Pmin) + 1000
//
//    //    float Lmax=100,Lmin=0；//Span 100L，Zero 0L, Span should be defined by the sensor
//    //pressure range of customer used. 100 means pressure range of 100L
//    //u32 Pvalue,Tvalue,Tspan,Pspan;
//    //u16 P1=1000,P2=15000;
//    // Pspan=P2-P1;
//    // Tdisplay=Tvalue*Tscope/Tspan-50;
//    // Pdisplay=Pvalue*(Lmax-Lmin)/Pspan+Lmin;//100L
//
//    long Pressure = (finalnum - 1000) / ((15000 - 1000) / 300);
//
//    Wire1.beginTransmission(40);
//    Wire1.write(0);
//    Wire1.endTransmission();
//
//    Log.info(module::SENSOR, "SENSOR BLADDER Pressure %ld finalnum=%ld p1=%ld, p2=%ld stat=%i\n", Pressure, finalnum, Pval, Pval2, pressureStatus);
//    //Log.info(module::SENSOR, "SENSOR RESERVOIR Pressure %ld\n", Pressure);
//  }
//  return Pressure;
//}
