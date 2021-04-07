ofxPresetsManager
=============================

# Overview

An **openFrameworks** addon to handle presets of several `ofParameter`'s nested into single or multiple `ofParameterGroup` containers.  

Also *empowered* with a *nice GUI* and many *helpers* and customizations.

## Screenshot

![image](/readme_images/Capture_v2.JPG?raw=true "image")  

## Features

- Handling of several nested `ofParameter`'s into `ofParameterGroup`'s as containers.
- Easy to integrate into your projects.
- Autogenerates a GUI to tweak all the parameters.
- GUI based on **Dear ImGui** using a fork of **ofxImGui**.
- Lot of customizations, **tools, and helpers**:  
  * Clone, Copy, and **Organize presets**.
  * Change GUI layout, customize docking windows, button sizes and Dark/Light **Themes**. 
  * Play BPM Timed **Randomizer** to switch between presets (tweaking probabilities) independent for each group.
  * **Randomize** some selected **parameters** for a preset.
  * Handle **many Groups** independently. **Browse** multi groups by arrow **keys**.
  * **Undo** history engine.

## **Multi groups**  

When adding individually multiple `ofParameterGroup` we can handle them independently.  
We can use arrow keys to browse the groups and their selectors.  
Also, we will have an extra **Global Selector** called **GROUP_LINK**, which groups all the other group selectors states:  

![image](/readme_images/Capture_multigroupClicker.JPG?raw=true "image")  

## **Two Types of Presets**

1. **Favorite Presets**:  
Fast clickable-boxes/key-trigged presets.  

2. **Standalone Presets**:  
Archived and named presets that we can load into and from favorites.  

## Two Working Modes

1. **Edit Mode**:  
Auto-saves parameters changes when switching between presets.  
Used to allow fast edititon of the parameters.  

2. **Live Mode**:  
Disables auto-save and hides some GUI sections to simplify and improve performance.  
Used when just playing the scenes. (We can save manually.)

## **Two Randomizer Engines**

1. **RANDOM SELECTED INDEX**:  
To select a preset index by random and probabilities, customizable tempo, and two types of preset/states durations: normal and short. 

2. **RANDOM EDIT**:  
To randomize some enabled parameters from the current preset.  

## Usage

1. Set up your scene and `initialize` the related ```ofParameter```'s.
2. ```Add``` the parameters ```to``` the ```ofParameterGroup container```.
3. ```Add``` the group ```to the addon```. 
4. Set ```how many presets``` you want for the group, ```and what keys``` to associate as triggers.
5. ```Done!``` Just Play with the addon GUI (or using the API control methods by code).  

Look at the examples and ```ofxPresetsManager.h``` for more learning.  
The file settings (one file for each preset) will be placed by default into ```/bin/data```.  

**ofApp.h**
```.cpp
#include "ofxPresetsManager.h"

ofxPresetsManager presetsManager;

ofParameterGroup params;// group params container
ofParameter<int> shapeType;
ofParameter<int> amount;
...
```

**ofApp.cpp**
```.cpp
ofApp::setup()
{
	// 1. set the scene parameters 
	// 2. add them to the ofParameterGroup container. 
	params.setName("sceneParamsGroup");	
	params.add(shapeType.set("shapeType", 1, 1, 2));
	params.add(amount.set("amount", 10, 1, 24));
	...

	// 3. add the group to preset manager
	// 4. set the amount of presets and each key triggers 
	presetsManager.add(params, { 'a', 'b', '0', '1', '2', '3' });
	presetsManager.setup();
}

// 5. Done!
// nothing more!
```

## About the examples

* **1_example-Basic**:  
Illustrates how to handle some ```ofParameter```'s bundled into one ```ofParameterGroup``` used as a container.  

* **2_example-MultiGroup**:  
Helps to learn how to handle multiple `ofParameterGroup`'s.  
```.cpp
// group 0
presetsManager.add(params0, { 'q', 'w', 'e', 'r', 't' });
// group 1
presetsManager.add(params1, { 'a', 's', 'd', 'f' });
// group 2
presetsManager.add(params2, { 'z', 'x', 'c' });
// group 3
presetsManager.add(params3, { 'b', 'n', 'm', ',', '.' });

presetsManager.setup();
```

![image](/readme_images/Capture_multigroup.JPG?raw=true "image")  

* **3A_example-LinkServer** and **3B_example-LinkClient**:  
Both examples runs linked together using **ofxRemoteParameters**.  
This allows you to control the presets on the **Client OF_App**.  
The **Server OF_App** draws your scene using the *linked/received* parameters from the **Client OF_App**.  
This is useful because in some scenarios could help to improve the performance or the compiling time reducing dependencies.  
[VIDEO](http://www.youtube.com/watch?v=kV-t8lIdNRg "VIDEO") 

## Dependencies
* [ofxImGui](https://github.com/Daandelange/ofxImGui/tree/jvcleave) [ Fork ]
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)  
* [ofxScaleDragRect](https://github.com/moebiussurfing/ofxScaleDragRect)
* [ofxRemoteParameters](https://github.com/c-mendoza/ofxRemoteParameters) [ Only for the examples 3A-3B ]  

Below add-ons are bundled. Already included in **OF_ADDON/libs**.  
Do not require to manually include in **Project Generator**:  
* [ofxUndo](https://github.com/nariakiiwatani/ofxUndo)

## Thanks
*Thanks to all the above addons coders!*  
*Special Thanks to **Nicola Pisanti** for the original https://github.com/npisanti/ofxGuiPresetSelector  
which inspired this addon a lot. Some of his code is also used here.*  
*Special Thanks 2 goes to **@Daandelange** for his work to the DEFINITIVE **ofxImGui** repository. Thanks **Daan**!*

## Tested Systems
- **Windows10** / **VS2017** / **OF ~0.11**
- **macOS. High Sierra** / **Xcode9** & **Xcode10** / **OF ~0.11**

## History of Changes
**v2.0**
* Added examples for **ofxRemoteParameters** from **@c-mendoza**.
* Switched to **ofxImGui** fork from **@Daandelange** allowing multiple ImGui instances, to combine many addons using **ImGui**.

## Notes / TODO
* A Better performant version without using hard disk files. **Memory Mode**, using a vector of XML/JSON instead of files.
* Improve (silent) de-serialization and ofParameter callbacks / triggering to improve speed.
* Add params tweening or filtering to ease transitions between presets when switching.
* Add a minimal/lite class to play compatible preset files but without any GUI or using a tiny **ofxGui**/**ImGui** version.
* More ideas noted into **ofxPresetsManager.h**.   
* **Feedback**, **Issues**, **PR**'s, and any kind of help are very welcome!

## Author
An addon by **@moebiusSurfing**  
*( ManuMolina ) 2019-2021*

## License
*MIT License*
