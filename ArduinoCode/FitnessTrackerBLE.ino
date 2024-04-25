#include <Wire.h>
#include <ArduinoBLE.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;


void updateValues();
long lastBeat = 0; // time at which the last beat occurred
float beatsPerMinute;


// random UUID's for the service and characteristics 
const char * UUID_pulseduinoService = "84582cd0-3df0-4e73-9496-29010d7445dd";
const char * UUID_heartRateCharacteristic = "84582cd1-3df0-4e73-9496-29010d7445dd"; 
const char * UUID_interruptCharacteristic = "84582cd2-3df0-4e73-9496-29010d7445dd"; 

// create the service and characteristics
BLEService pulseduinoService(UUID_pulseduinoService); // 180D is the UUID of the Heart Rate service
BLEFloatCharacteristic chHeartRate(UUID_heartRateCharacteristic, BLERead | BLENotify); // 2A37 is the UUID of the Heart Rate Measurement characteristic
BLEByteCharacteristic chInterrupt(UUID_interruptCharacteristic, BLERead | BLENotify);


volatile bool interruptFlag = false;

const int interruptPin = 3; // D3 pin

void handleInterrupt()
{
  Serial.println("Interrupt detected");
  chInterrupt.writeValue(1); // Notify the central device of the interrupt
  Serial.println("Notified central device");
  interruptFlag = false;
  interruptFlag = true;
}

void setup() {
  
  Serial.begin(9600);
  Serial.println("Initializing...");

  // initialize sensor
  Serial.println("Trying to start sensor.");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    while (1) {
      Serial.println("MAX30102 was not found. Please check wiring/power. ");
    }
  }

  // initialize interrupt pin
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);

  // setup MAX30102 stuff
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  // setup BLE stuff
  Serial.println("Trying to start BLE.");
  BLE.begin();
  BLE.setLocalName("Arduino");
  BLE.setAdvertisedService(pulseduinoService);
  pulseduinoService.addCharacteristic(chHeartRate);
  pulseduinoService.addCharacteristic(chInterrupt);
  BLE.addService(pulseduinoService);
  chInterrupt.writeValue(0);
  chHeartRate.writeValue(0);
  BLE.advertise();
}

void loop() {
  static long preMillis = 0;
  long currMillis;
  // check if a central device has connected
  BLEDevice central = BLE.central();
  if (central) 
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // while connected, update heartrate and check for 
    while (central.connected()) 
    { 
      curMillis = millis();
      if (preMillis > curMillis) 
      {
        preMillis = 0; // millis() rollover?
      }
      if (curMillis - preMillis >= 10) // check values every 10mS
      {
        preMillis = curMillis;
        updateValues(); // call function for updating value to send to central
      }
    } // still here while central connected

    // central disconnected:
    Serial.println("Disconnected from central: ");
  } // no central
    Serial.println("Disconnected from central: ");
}


void updateValues()
{
  long irValue = particleSensor.getIR();
  int int_beatsPerMinute;
  if (checkForBeat(irValue) == true) {
    // we sensed a beat!
    // use time to update the average of the BPM
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    int_beatsPerMinute = (int)beatsPerMinute;
  }

  // send heart rate data to central device 
  Serial.println("Passing central device HR = " + String(int_beatsPerMinute));
  chHeartRate.writeValue(int_beatsPerMinute);
}
