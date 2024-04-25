# pulseduino
Pulse Ox that sends data to an Android app, using an Arduino, MAX30102, and BLE standard. 

## Dependencies
 - Node.js
 - Android SDK and IDE
 - Arduino IDE 
 - Expo Go on Android phone (may have permission issues with newer devices)
 - Android App Developer Permissions (secret setting)

## Installation
 - download from github using either of the following
 - HTTPS
 - ```git clone https://github.com/caden-miller/pulseduino.git```
 - SSH
 - ```git clone git@github.com:caden-miller/pulseduino.git```
 - install project using NPM 
 - ```cd pulseduino``` 
 - ```npm install```
 - setup eas 
 - ```npx npm install eas-cli```
 - ```npx expo install expo-dev-client```

## Running 
 - run using expo
 - ```npm run android```
 - then use the CLI to navigate to an android and scan the QR code
 - install on Android
 - upload Arduino code to your Arduino Nano 33 BLE
 - use serial monitor to debug any connection issues 
 - turn on BLE 
 - accept permissions
 - connect Arduino
 - enjoy heart rate reading!

## Plans
 - verify the graph displays properly
 - implement database for more data storage
 - implement different user profiles
 - alert for inconsistent or unreasonable heart rate readings 
 - alerts and corresponding information about unhealthy heart rates
 - add support for SpO2
 - add exercise support?