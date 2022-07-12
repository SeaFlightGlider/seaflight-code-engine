
#define LEAK_SENSE_PIN  35
#define LEAK_SENSOR_REFRESH_RATE 100 //in milliseconds
unsigned long previousMillisLeakDetect = 0;
int intervalLeakDetect = LEAK_SENSOR_REFRESH_RATE;
bool leakDetected = false;

void setupLeakDetect(){
  pinMode(LEAK_SENSE_PIN, INPUT);
}

void loopLeakDetect(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisLeakDetect > intervalLeakDetect) {
    previousMillisLeakDetect = currentMillis;

    if(digitalRead(LEAK_SENSE_PIN) > 0){
      leakDetected = true;
      Log.error(module::POWER,"LEAK DETECTED: BAIL!!!!!!!!!!!!!!! \n"); 
      //set emergency state to inflate bladder
    } 
  }
  
}
