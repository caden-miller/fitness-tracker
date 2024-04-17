#include <Wire.h>
#include <ArduinoBLE.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;
void updateValues();

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
float beatAvg;

const char * UUID_fitnessTrackerService = "84582cd0-3df0-4e73-9496-29010d7445dd";
const char * UUID_heartRateMeasurementCharacteristic = "84582cd1-3df0-4e73-9496-29010d7445dd"; 

BLEService fitnessTrackerService(UUID_fitnessTrackerService); // 180D is the UUID of the Heart Rate service
BLEFloatCharacteristic chHRAVG(UUID_heartRateMeasurementCharacteristic, BLERead | BLENotify); // 2A37 is the UUID of the Heart Rate Measurement characteristic

void setup() {
  
  Serial.begin(9600);
  Serial.println("Initializing...");

  // Initialize sensor
  Serial.println("Trying to start sensor.");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    while (1) {
      Serial.println("MAX30102 was not found. Please check wiring/power. ");
    }
  }

  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  Serial.println("Trying to start BLE.");
  if (!BLE.begin()) {
    while (1) {
      Serial.println("starting BLE failed!");
    }
  }

  BLE.setLocalName("PeripheralArduino");
  BLE.setAdvertisedService(fitnessTrackerService);
  fitnessTrackerService.addCharacteristic(chHRAVG);
  BLE.addService(fitnessTrackerService);
  chHRAVG.writeValue(0);
  BLE.advertise();
}

void loop() {
  static long preMillis = 0;

  

  // Check if a central device has connected
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    while (central.connected()) 
      {
      // additional placeholder for writing command from central
      // to this device. "myCharacteristic" is a characteristic initialized
      // in write mode (BLEwrite) with his own UUid
      /*
      if (myCharacteristic.written()) 
        {
        command = myCharacteristic.value(); // retrieve value sent from central
        Serial.print(F("commmand value:  "));
        Serial.println(command);
        }
      */
      
      long curMillis = millis();
      if (preMillis>curMillis) preMillis=0; // millis() rollover?
      if (curMillis - preMillis >= 10) // check values every 10mS
        {
        preMillis = curMillis;
        updateValues(); // call function for updating value to send to central
        }
     } // still here while central connected

    // central disconnected:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  } // no central
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
    Serial.print("Mac Address is "); Serial.println(WiFi.macAddress());
}


void updateValues()
{
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  chHRAVG.writeValue(beatsPerMinute);
}
