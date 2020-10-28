ofxPresetsManager
=============================

# Overview
**ofxPresetsManager** is an **openFrameworks** addon to handle presets of several ´ofParameter´'s nested into an ´ofParameterGroup´ container. Also empowered with a cute GUI and many helpers and customizations.

## Features
- ´Powerful´ handling of several nested ´ofParameters´ into ´ofParameterGroup´'s as containers.
- ´Easy integrate´ into all your projects.
- You don't need to add extra Gui to control your parameters.
- ´Cute Gui´ based on ´docked ImGui´ windows using ofxImGui.
- ´Two types´ of ´presets´: 
1. ´Favourite presets´: the clickable-boxes/key-trigged presets:  
![image](/readme_images/Capture_favourites.PNG?raw=true "image")  
2. ´Standalone presets´: the archived and named presets:  
![image](/readme_images/Capture_standalone.PNG?raw=true "image")  
- Lot of ´customizations´ and ´helpers´: clone presets, re-arrenge sorting, play randomizer, 
- Two randomizer engines: 
1. To select a preset index by random and probabilities, customizable tempo and two types of preset/states durations: normal and short.  
![image](/readme_images/Capture_randomizer1.PNG?raw=true "image")  
2. To randomize some enabled parameters from current preset.  
![image](/readme_images/Capture_randomizer2.PNG?raw=true "image")  

## Usage
**DOCUMENTATION**  
This is a very simple guide to use `ofxPresetsManager`.  
Simpler use will be to add a single `ofParameterGroup`, but the addon can handle more groups!  
0. Set up your scene and `initialize` the related `parameters`.
1. Add the parameters to the `ofParameterGroup` as `container`.
2. `Customize` some settings if desired.
3. `Add` the container `to the addon` object. 
4. You can define how many preset you want, and what keys to associate as triggers.
5. ´Done! just play´ with the addon Gui or the control API methods.  
- Look the examples.
The file settings (one for each preset) will be placed by default into ´/bin/data´.

## Examples Screenshots
´example-Basic´: handles one ´ofParameterGroup´.  
![image](/readme_images/Capture-example-Basic.PNG?raw=true "image")  
´example-MultiGroup´: handles multiple ´ofParameterGroup´'s.  
![image](/readme_images/Capture-example-MultiGroup.PNG?raw=true "image")  

### ofApp.h
```.cpp
#include "ofxPresetsManager.h"

ofxPresetsManager presetsManager;
ofParameterGroup params;
ofParameter<int> shapeType;
ofParameter<int> amount;
//...more parameters
```

### ofApp.cpp
```.cpp
ofApp::setup(){
	// define our parameters (ofParameterGroup params) for our scene and add them to our params
	params.setName("sceneParamsGroup");// preset settings main container	
	params.add(shapeType.set("shapeType", 1, 1, 2));
	params.add(amount.set("amount", 10, 1, 24));
	//...more parameters

	// add our ofParameterGroup to the presetManager 
	// and define key triggers for each preset. the amount of keys will be the amount of favourites presets
	presetsManager.add(params, { 'a', 'b', '0', '1', '2', '3' });
	presetsManager.setup();// must call after adding all the ofParameterGroup(s)
}
```

## Dependencies
Already included in ´OF_ADDON/libs´. Do not require to manually include into ´Project Generator´.
- ofxImGui
- ofxScaleDragRect
- ofxUndo

## Tested systems
- **Windows10** / **VS2017** / **OF ~0.11**
- **macOS High Sierra** / **Xcode 9/10** / **OF ~0.11**

### TODO/IDEAS
* 

### Notes
*

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License.*