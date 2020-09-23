ofxPresetsManager
=============================

# Overview
**ofxPresetsManager** is an **openFrameworks** addon template for MYSELF.

## Screenshot
![image](/readme_images/Capture1.PNG?raw=true "image")
![image](/readme_images/Capture2.PNG?raw=true "image")

## Features
- Customize gui.
- Basic parameters and app modes.
- Store/Recall settings.
- 


## Usage
 
### ofApp.h
```.cpp
#include "ofxPresetsManager.h"
ofxPresetsManager myAddon;
```

### ofApp.cpp
```.cpp
ofApp::setup(){
	ofxPresetsManager.setup();
}

ofApp::update(){
	ofxPresetsManager.update();
}

ofApp::draw(){
	ofxPresetsManager.draw();
	ofxPresetsManager.drawGui();
}
```

## Dependencies
- 

## Tested systems
- **Windows10** / **VS2017** / **OF 0.11**

### TODO/IDEAS
* 

### Notes
*

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License.*