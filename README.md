# fitness-tracker
Fitness tracker that also detects heart rate and pulse ox anomalies. 

Implemented using an Arduino Nano 33 BLE and MAX30102 heart rate and pulse ox sensor.

## Installation
 - install Cordova using Node.js 
 - ```npm install -g cordova```

## Running
 - connect computer to android phone
 - make sure phone is connected to bluetooth
 - prepare app
 - ```cordova prepare android```
 - build app
 - ```cordova build android```
 - run app
 - ```cordova run android```

## Alternate Running
 - download APK directly via github
 - make sure phone is connected to bluetooth and also is able to install apps from unknown sources
 - located in ```CordovaApp/platforms/android/app/build/outputs/apk/debug/app-debug.apk```
 -

## Plans
 - setup android app
 - get BLE working
 - add heart sensor support
 - send heart sensor data to app
 - add [Edge Impulse](https://edgeimpulse.com/) support for exercise detection
 - send exercise data to app
 - finalize app UI
 - test
 - finalize and polish