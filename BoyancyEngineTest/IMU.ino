/*  SeaFlight Glider
    NeoM8U GPS


    More information:     https://www.sparkfun.com/products/14686
    Interfacing guide:    https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library

    Todo:


*/
const unsigned int IMU_REFRESH_DELAY = 100;
bool validIMU = false;
BNO080 imu;

void setupIMU() {
  imu.begin();
  imu.enableRotationVector(50); //Send data update every 50ms

  validIMU = false;
}

void loopIMU() {
  /*
  float quatI = myIMU.getQuatI();
  float quatJ = myIMU.getQuatJ();
  float quatK = myIMU.getQuatK();
  float quatReal = myIMU.getQuatReal();
  float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();
  */
}
