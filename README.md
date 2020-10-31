ofxPresetsManager
=============================

# Overview
**ofxPresetsManager** is an **openFrameworks** addon to handle presets of several `ofParameter`'s nested into an `ofParameterGroup` container. Also *empowered* with a *nice GUI* and many *helpers* and customizations.

## Features
- `Powerful` handling of several nested `ofParameters` into `ofParameterGroup`'s as containers.
- `Easy to integrate` into your projects.
- `Autogenerates a GUI` to tweak all the parameters.
- `Cute Gui` based on `docked ImGui` windows using ofxImGui.
- Lot of `customizations` and `helpers`: clone presets, change GUI layout and button sizes, dark/light themes, re-arrange/sorting presets, play timed randomizer to switching between presets, randomize some parameters, browse multigroups by arrow keys...etc.  

**Two Types of Presets**:  
1. **Favourite Presets**: Fast clickable-boxes/key-trigged presets.  

![image](/readme_images/Capture_favourites.PNG?raw=true "image")  
2. **Standalone Presets**: Archived and named presets that we can load into and from favourites.  

![image](/readme_images/Capture_standalone.PNG?raw=true "image")  


**Two Modes**: 
1. **Edit Mode**: Autosaves changes when switching between presets.  
2. **Live Mode**: Disables autosave and hide some GUI sections to simplify and improve performance.   

**Multigroups**:  

When adding individually several `ofParameterGroup` we can handle them independtly.  
We can use arrow keys to browse the groups and their selectors.  
Also we have an extra selector called `GROUP_LINK` wich groups the other group selectors states:  
GUI selectors for each added group:  

![image](/readme_images/Capture_multigroupSelectors.PNG?raw=true "image")  

**Two Randomizer Engines**: 
1. **RANDOM SELECTED INDEX**: To select a preset index by random and probabilities, customizable tempo, and two types of preset/states durations: normal and short.  

![image](/readme_images/Capture_randomizer1.PNG?raw=true "image")  
2. **RANDOM EDIT**: To randomize some enabled parameters from current preset.  

![image](/readme_images/Capture_randomizer2.PNG?raw=true "image")  

## Basic Usage
1. Set up your scene and `initialize` the related `ofParameter`'s.
2. Add the parameters to the `ofParameterGroup` `container`.
3. `Customize` some settings if desired.
4. `Add` the container `to the addon` object. 
5. You can define how many presets you want for each group, and what keys to associate as triggers.
6. `Done! Just play` with the addon GUI or the API control methods.  
- Look at the examples and `ofxPresetsManager.h` for more learning.  
- The file settings (one for each preset) will be placed by default into `/bin/data`.

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
	// the amount of keys will be the amount of favourites presets for the added group.
	presetsManager.add(params, { 'a', 'b', '0', '1', '2', '3' });
	presetsManager.setup();// must call after adding all the ofParameterGroup(s)
}

// nothing more!
// but some optional customization is allowed.
```

### Other API useful methods
```.cpp
	presetsManager.setSizeBox_PresetClicker(45);
	presetsManager.setVisible_PresetClicker(false);
	presetsManager.toggleVisible_PresetClicker();

	presetsManager.setToggleEnableKeys();
	presetsManager.setEnableKeys(false);
	
	// disable the Group_Link auto creation by default
	presetsManager.setEnableGroupLinkSelector(false);
	
	//presetsManager.setEnableGroupLinkSelector(true);
	//presetsManager.setGroupLinkSize(2);// define how many presets we want for Group_Link selector

	// save current preset from an added group
	presetsManager.saveCurrentPreset(params.getName());// by name
	presetsManager.saveCurrentPreset(0);// by index

	// B. callback
	// callbacks for all selectors to know when a group selector changed
	for (int i = 0; i < presetsManager.PRESETS_Selected_Index.size(); i++)
	{
		ofAddListener(presetsManager.getSelectorsGroup().parameterChangedE(), this, &ofApp::Changed_PresetsManagerSelectors);
	}
	//--------------------------------------------------------------
	void ofApp::Changed_PresetsManagerSelectors(ofAbstractParameter &e)
	{
		string name = e.getName();
		ofLogNotice(__FUNCTION__) << "THE SELECTOR FOR GROUP WITH NAME " << name << " CHANGED TO PRESET INDEX: " << e;
		if (name == params.getName()) {}
	}

	// A. an easier callback that can not know wich group selector changed
	if (presetsManager.isDoneLoad())
	{
		ofLogNotice(__FUNCTION__) << "DONE LOAD PRESET. SOME GROUP HAVE CURRENTLY LOADED/CHANGED TO A PRESET INDEX";
	}

	// customize path to allow using more session presets
	presetsManager.setPath_UserKit_Folder(path);
```

## Examples Screenshots
* `example-Basic`:  
Illustrates how to handle one `ofParameterGroup`.  

![image](/readme_images/Capture-example-Basic.PNG?raw=true "image")  

* `example-MultiGroup`:  
Learn how to handle multiple `ofParameterGroup`'s.  
```.cpp
// group 0
presetsManager.add(params0, { 'q', 'w', 'e', 'r', 't' });
// group 1
presetsManager.add(params1, { 'a', 's', 'd', 'f' });
// group 2
presetsManager.add(params2, { 'z', 'x', 'c' });

presetsManager.setup();
```
Autogenerated GUI to tweak/debug all parameters:  

![image](/readme_images/Capture_multigroupParams.PNG?raw=true "image")  
  
![image](/readme_images/Capture-example-MultiGroup.PNG?raw=true "image")  

## Dependencies
All are bundled. Already included in `OF_ADDON/libs`. Do not require to manually include into `Project Generator`.  
- **ofxImGui** fork from https://github.com/MacFurax/ofxImGui
- **ofxScaleDragRect** from https://github.com/roymacdonald/ofxScaleDragRect
- **ofxUndo** from https://github.com/nariakiiwatani/ofxUndo  

*Thanks to the above coders!*  
*Special thanks to **Nicola Pisanti** for the original https://github.com/npisanti/ofxGuiPresetSelector,  
that inspired this addon a lot.*

## Tested Systems
- **Windows10** / **VS2017** / **OF ~0.11**
- **macOS. High Sierra** / **Xcode9** & **Xcode10** / **OF ~0.11**

## NOTES
* Should improve **ofxImGui** contexts because we can't use more than one instance yet.  
* Other ideas noted into `ofxPresetsManager.h`.   
* Undo workflow not operative now.
* Add minimal/lite class compatible but without any GUI or using a tiny `ofxGui` version.
* Better performant version without using hard disk files. Memory vector of xml/json instead.
* **Feedback**, **issues**, and **PR**'s are very welcome!

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License.*