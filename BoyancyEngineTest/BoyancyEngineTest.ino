
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

//caution:  if you are adding new libraries you need to replace #include <Wire.h> with #include <i2c_t3.h>
//as below..  this is because this i2c library allows us to use all the i2c ports on the teensy..
#include <i2c_t3.h>                         //library: https://github.com/nox771/i2c_t3
#include "TeensySDRotationalModuleLogger.h" //library: https://github.com/embeddedartistry/arduino-logger
//#include <SerLCD.h>                       // Depreciated, no longer using this display module - Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
//#include <SD.h>                             //might be a conflict, if so delete the one 'not used'
#include <SPI.h>
#include "Adafruit_VL6180X.h"               // library: https://github.com/adafruit/Adafruit_VL6180X
#include "SparkFun_RV1805.h"                // library: https://github.com/sparkfun/SparkFun_RV-1805_Arduino_Library/archive/master.zip  https://github.com/sparkfun/SparkFun_RV-1805_Arduino_Library
#include "SFE_HMC6343.h"                    // library: https://github.com/sparkfun/SparkFun_HMC6343_Arduino_Library
#include "SparkFun_BNO080_Arduino_Library.h"// library: https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library

#include <BasicTerm.h>                      // library: https://github.com/nottwo/BasicTerm

#define ENGINE_DEBUG_PRINT 1

#define PRESSURE_RES        0
#define LCD_MUX             4
#define MUX_ADDR            0x70             //7-bit unshifted default I2C Address

#define FIRMWARE_VERSION    0.1

enum module {   //for logging modules..
  SETUP = 0,
  LOOP,
  POWER,
  ENGINE,
  GPS,
  TIME,
  SENSOR,
  STEER,
  PUMP,
  MODULE_COUNT
};

struct GliderTimeStruct {
  uint8_t hund = 42;
  uint8_t sec = 42;
  uint8_t minute = 42;
  uint8_t hour = 4;
  uint8_t date = 1;
  uint8_t month = 4;
  uint8_t year = 1974;
  uint8_t day = 26;
};
static TeensySDRotationalModuleLogger<module::MODULE_COUNT> Log;
static SdFs sd;
static int iterations = 0;

BasicTerm term(&Serial);

class DualPrint : public Print
{
  public:
    DualPrint() : use_Serial(false), use_Serial1(false) {}
    virtual size_t write(uint8_t c) {
      if (use_Serial) {
        Serial.write(c);
      }
      if (use_Serial1) {
        term.position(23, 2);
        term.print(c);
        //Serial1.write(c);

      }
      return 1;
    }
    bool use_Serial, use_Serial1;
} out;

enum GliderState {
  EMERGENCY,
  MISSION,
  STANDBY,
  BENCH,
  POWER_ON
};
static GliderState gliderState;

enum MissionState {
  MISSION_STANDBY,
  MISSION_PREPARE,
  MISSION_SURFACE,
  MISSION_CLIMBING,
  MISSION_DIVING,
  MISSION_BOTTOM,
  MISSION_RECOVERY,
  MISSION_EMERGENCY
};
static MissionState missionState = MISSION_STANDBY;
const char* missionStateStr[] = {"MISSION_STANDBY", "MISSION_PREPARE", "MISSION_SURFACE", "MISSION_CLIMBING", "MISSION_DIVING", "MISSION_BOTTOM", "MISSION_RECOVERY", "MISSION_EMERGENCY"};

enum BenchState {
  INIT,
  DATA,
  TESTING,
  BURN_IN,
  WAITING,
  INIT_DEPLOY
};
enum BenchState benchState;

enum TestingState {
  ENGINE_TEST,
  SENSOR_TEST,
  POWER_TEST,
  COMMS_TEST,
  LEAK_TEST
};
enum TestingState testingState = ENGINE_TEST;
const char* testStateStr[] = {"ENGINE_TEST", "SENSOR_TEST", "POWER_TEST", "COMMS_TEST"};

enum EngineTestState {
  ENGINETEST_INIT,
  ENGINETEST_STANDBY,
  ENGINETEST_KEYPRESSWAIT,
  ENGINETEST_DIVING,
  ENGINETEST_CLIMBING,
  ENGINETEST_STEERING_TEST,
  ENGINETEST_PUMP_TEST
};
enum EngineTestState engineTestState = ENGINETEST_STANDBY;
const char* engineTestStateStr[] = {"ENGINETEST_INIT", "ENGINETEST_STANDBY", "ENGINETEST_KEYPRESSWAIT", "ENGINETEST_DIVING", "ENGINETEST_CLIMBING", "ENGINETEST_STEERING_TEST", "ENGINETEST_PUMP_TEST"};

enum PumpTestState {
  PUMP_INIT,
  PUMP_STANDBY,
  PUMP_KEYPRESSWAIT,
  PUMP_IN_ON,
  PUMP_OUT_ON,
  PUMP_OUT_HOLD,    //Simulated climbing time
  PUMP_IN_HOLD,     //Simulated Diving time
  PUMP_OFF,
  PUMP_HOLD_FOR
};
enum PumpTestState pumpTestState = PUMP_STANDBY;
const char* pumpTestStateStr[] = {"PUMP_INIT", "PUMP_STANDBY", "PUMP_KEYPRESSWAIT", "PUMP_IN_ON", "PUMP_OUT_ON", "PUMP_OUT_HOLD", "PUMP_IN_HOLD", "PUMP_OFF", "PUMP_HOLD_FOR"};

enum SteeringTestState {
  STEER_INIT,
  STEER_STANDBY,
  STEER_STEERING,
  STEER_KEYBOARDWAIT,
  STEER_OFF
};
enum SteeringTestState steeringTestState;
const char* steeringTestStateStr[] = {"STEER_INIT", "STEER_STANDBY", "STEER_STEERING", "STEER_KEYBOARDWAIT", "STEER_OFF"};

bool displayVT100Dash = true;
String currentDate = "";
String currentTime = "";
long Pressure;

int Pset = 75;   //Pressure Upper Limit
int Pmin = 10;   //Pressure Lower Limit

// SerLCD lcd; // Initialize the library with default I2C address 0x72
RV1805 rtc;

bool usingVT100 = true;

bool SENSOR_DEBUG_PRINT = true;  //To see real time readouts on the Serial console set this to TRUE

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  Serial.begin(115200);

  setupLCDDisplay();

  int serialDelay = 0;

  while (serialDelay < 8) {
    if (!Serial) { 
    // wait for serial port to connect. Needed for native USB port only
    serialDisplay(serialDelay);
    delay(100);
    serialDelay++;
    } else{
      break;
    }
  }

  delay(2000);
  out.use_Serial = true;
  sd.begin(SdioConfig(FIFO_SDIO));
  Log.begin(sd);

  Log.level(module::SETUP, log_level_e::debug);
  Log.level(module::LOOP, log_level_e::info);
  Log.level(module::POWER, log_level_e::info);
  Log.level(module::GPS, log_level_e::info);
  Log.level(module::TIME, log_level_e::info);
  Log.level(module::SENSOR, log_level_e::info);
  Log.level(module::STEER, log_level_e::info);

  Log.echo(true); // log calls will be printed over Serial
  Log.debug(module::SETUP, "SeaFlight Glider V0.1 Starting..\n");

  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_INT, 200000);
  //Wire.setDefaultTimeout(200000); // 200ms


  gliderState = BENCH;
  benchState = TESTING;
  testingState = ENGINE_TEST;
  engineTestState = ENGINETEST_STANDBY;
  pumpTestState = PUMP_STANDBY;
  steeringTestState = STEER_STANDBY;
  
  setupPower();
  //setupGPS();
  //setupTime();
  setupPressure();
  SetupEngineTest();
  setupPumpTest();setupControlPump();
  setupTOF();
  //setupLeakDetect();
  //setupSteering();
  //setupInputs();
  //setupStorage();
  //setupIMU();
  //setupCompass();
  //vt100DashSetup();

   changeEngineTestState(ENGINETEST_KEYPRESSWAIT);

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void loop() {
  //loopTime();
  loopPower();
  loopGPS();
  loopPressure();
  loopTOF();
  loopEngineTest();
  loopPumpTest();
  //loopSteering();

  //loopInputs();
  loopLCDDisplay();
  //loopLeakDetect();
  //loopIMU();
  //loopCompass();
  //vt100DashLoop();
  //loopStorage();
}
