/* try to read raw data out of a Groove Sound Senser*/

#include "CurieImu.h"
void setup() {
  // put your setup code here, to run once:
      // initialize Serial communication
    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing IMU device...");
    CurieImu.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(CurieImu.testConnection() ? "CurieImu connection successful" : "CurieImu connection failed");

}

void loop() {
  // put your main code here, to run repeatedly:

int soundSensorPin=A0;
int soundReading=0;

soundReading=analogRead(soundSensorPin);
Serial.print("sound level: ");
Serial.println(soundReading);
}
