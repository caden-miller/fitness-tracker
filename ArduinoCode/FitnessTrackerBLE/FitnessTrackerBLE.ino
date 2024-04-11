#include <Wire.h>
#include <SparkFun_MAX3010x_Sensor_Playground.h>
#include <ArduinoBLE.h>

MAX3010x sensor;

BLEService fitnessTrackerService("180D"); // 180D is the UUID of the Heart Rate service
BLEIntCharacteristic heartRateMeasurementChar("2A37", BLERead | BLENotify); // 2A37 is the UUID of the Heart Rate Measurement characteristic

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("FitnessTracker");
  BLE.setAdvertisedService(fitnessTrackerService);
  fitnessTrackerService.addCharacteristic(heartRateMeasurementChar);
  BLE.addService(fitnessTrackerService);
  heartRateMeasurementChar.writeValue(0);
  BLE.advertise();

  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX3010x did not start. Check wiring/power. ");
    while (1);
  }
}

void loop() {
  // Check if a central device has connected
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    while (central.connected()) {
      long irValue = sensor.getIR(); // Read IR data from the sensor
      if (irValue > 0) {
        heartRateMeasurementChar.writeValue(irValue); // Send IR data over BLE
      }
    }
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}