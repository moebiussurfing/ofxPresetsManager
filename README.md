ofxPresetsManager
=============================

# Overview
**ofxPresetsManager** is an **openFrameworks** addon to handle presets of several `ofParameter`'s nested into single or multiple `ofParameterGroup` containers. Also *empowered* with a *nice GUI* and many *helpers* and customizations.

## GIF
![gif](/readme_images/Capture_ofxPresetsManager.gif?raw=true "gif")  

## Features
- Powerful handling of several nested `ofParameter`'s into `ofParameterGroup`'s as containers.
- Easy to integrate into your projects.
- Autogenerates a GUI to tweak all the parameters.
- Gui based on **Dear ImGui** using **ofxImGui**.
- Lot of customizations, tools and helpers:  
  * Clone, copy and **arrenge presets**.
  * Change GUI layout and button sizes, Dark/Light **Themes**. 
  * Play timed **randomizer** to switch between presets (tweaking probabilities).
  * **Randomize** some selected **parameters**.
  * Handle **many Groups** indepently.
  * **Browse** multi groups by arrow **keys**.
  * **Undo** history engine.

## **Two Types of Presets**

* **Favourite Presets**:  
Fast clickable-boxes/key-trigged presets.  

![image](/readme_images/Capture_favourites.PNG?raw=true "image")  
* **Standalone Presets**:  
Archived and named presets that we can load into and from favourites.  

![image](/readme_images/Capture_standalone.PNG?raw=true "image")  

## Working Modes

**Two Modes**: 
* **Edit Mode**:  
Autosaves changes when switching between presets.  
* **Live Mode**:  
Disables autosave and hide some GUI sections to simplify and improve performance.   

**Multi groups**:  

When adding individually several `ofParameterGroup` we can handle them independently.  
We can use arrow keys to browse the groups and their selectors.  
Also, we have an extra selector called `GROUP_LINK` wich groups the other group selectors states:  
GUI selectors for each added group:  

![image](/readme_images/Capture_multigroupSelectors.PNG?raw=true "image")  

## **Two Randomizer Engines**

* **RANDOM SELECTED INDEX**:  
To select a preset index by random and probabilities, customizable tempo, and two types of preset/states durations: normal and short. 

![gif](/readme_images/Capture_randomizer1.gif?raw=true "gif")  

* **RANDOM EDIT**:  
To randomize some enabled parameters from current preset.  

![image](/readme_images/Capture_randomizer2.PNG?raw=true "image")  

## Basic Usage
1. Set up your scene and `initialize` the related ```ofParameter```'s.
2. Add the parameters to the ```ofParameterGroup container```.
3. ```Customize``` some settings if desired.
4. ```Add``` the container ```to the addon``` object. 
5. You can define how many presets you want for each group, and what keys to associate as triggers.
6. ```Done! Just play``` with the addon GUI or the API control methods.  
- Look at the examples and ```ofxPresetsManager.h``` for more learning.  
- The file settings (one file for each preset) will be placed by default into ```/bin/data```.

### ofApp.h
```.cpp
#include "ofxPresetsManager.h"

ofxPresetsManager presetsManager;

ofParameterGroup params;// main params container
ofParameter<int> shapeType;
ofParameter<int> amount;
//...more ofParameter's or nested ofParameterGroup's
```

### ofApp.cpp
```.cpp
ofApp::setup(){
	// define the scene parameters 
	// and add them to the ofParameterGroup container. 
	params.setName("sceneParamsGroup");	
	params.add(shapeType.set("shapeType", 1, 1, 2));
	params.add(amount.set("amount", 10, 1, 24));
	//...more ofParameter's

	// add the ofParameterGroup to the presetManager object
	// and define key triggers for each preset. 
	// the number of keys will be the number of favorites presets for the added group.
	presetsManager.add(params, { 'a', 'b', '0', '1', '2', '3' });
	presetsManager.setup();// must call after adding all the ofParameterGroup(s)
}

// nothing more!!
// but some extra customization or on-runtime commands are available.

ofApp::exit(){
	// only required to save GUI layout 
	// and the current selected preset(s) before destroy.
	presetsManager.exit();
}
```

## Examples Screenshots

* **1_example-Basic**:  
Illustrates how to handle some ```ofParameters``` bundled into one ```ofParameterGroup``` used as a container.  

![image](/readme_images/Capture-example-Basic.PNG?raw=true "image")  

* **2_example-MultiGroup**:  
Helps to learn how to handle multiple `ofParameterGroup`'s.  
```.cpp
// group 0
presetsManager.add(params0, { 'q', 'w', 'e', 'r', 't' });
// group 1
presetsManager.add(params1, { 'a', 's', 'd', 'f' });
// group 2
presetsManager.add(params2, { 'z', 'x', 'c' });

presetsManager.setup();
```
The autogenerated GUI to tweak/debug all parameters looks like:  

![image](/readme_images/Capture_multigroupParams.PNG?raw=true "image")  
  
![image](/readme_images/Capture-example-MultiGroup.PNG?raw=true "image")  

* **3A_example-LinkServer** and **3B_example-LinkClient**:  
Both examples runs together using **ofxRemoteParameters**. This allows to control the presets on the **Client OF_APP**. The **erver OF_APP** draws your scene using the linked/received parameters from the **Clien OF_APP**. This is useful because in some scenarios could help improve the performance or the compiling time and dependencies.
[![ofxPresetsManager](http://img.youtube.com/vi/kV-t8lIdNRg/0.jpg)](http://www.youtube.com/watch?v=kV-t8lIdNRg "VIDEO")

## Dependencies
* [ofxImGui](https://github.com/Daandelange/ofxImGui/tree/jvcleave) [FORK]
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)  
* [ofxScaleDragRect](https://github.com/moebiussurfing/ofxScaleDragRect)
* [ofxRemoteParameters](https://github.com/c-mendoza/ofxRemoteParameters) [For the examples 3A-3B only]

Below addons are bundled. Already included in `OF_ADDON/libs`. Do not require to manually include into `Project Generator`.  
* [ofxUndo](https://github.com/nariakiiwatani/ofxUndo)

*Thanks to the above coders!*  
*Special thanks to **Nicola Pisanti** for the original  
https://github.com/npisanti/ofxGuiPresetSelector  
which inspired this addon a lot. Some of his code is also used here.*
*Special Thanks 2 goes to **@Daandelange** for his work to the definitivew ofxImGui repository. Thanks **Daan**!*

## Tested Systems
- **Windows10** / **VS2017** / **OF ~0.11**
- **macOS. High Sierra** / **Xcode9** & **Xcode10** / **OF ~0.11**

## VERSIONS HISTORY
**v2.0**
* Added examples for **ofxRemoteParameters** from **@c-mendoza**.
* Switched to **ofxImGui** fork from **@Daandelange** allowing multiple ImGui instances, to combine many addons using **ImGui**.

## NOTES
* A Better performant version without using hard disk files. **Memory Mode**, using vector of XML/JSON instead of files.
* Improve deserialization and ofParameter callbacks / triggering to improve speed.
* Add a minimal/lite class to play compatible preset files but without any GUI or using a tiny **ofxGui**/**ImGui** version.
* More ideas noted into **ofxPresetsManager.h**.   
* **Feedback**, **Issues**, **PR**'s and any kind of help are very welcome!

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2019-2021.*

## License
*MIT License.*