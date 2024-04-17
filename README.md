# fitness-tracker
Fitness tracker that also detects heart rate and pulse ox anomalies. 

Implemented using an Arduino Nano 33 BLE and MAX30102 heart rate and pulse ox sensor.

## Installation
 - install project using Node.js
 - ```cd fitness-tracker-app``` 
 - ```npm install```

## Running 
 - setup eas 
 - ```npx npm install eas-cli```
 - ```npx expo install expo-dev-client```
 - run using eas
 - ```eas build --profile development --platform android```
 - scan QR code once built
 - install on Android
 - scan for devices
 - accept permissions
 - get heart rate

## Plans
 - verify app can connect to Arduino
 - verify BLE is transmitting HR data
 
 - finalize 3D model
 - verify heart rate data
 - 
 - make battery work 
 - finalize app UI
 - test
 - finalize and polish