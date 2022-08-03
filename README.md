ofxPresetsManager
=============================

# Overview

An **openFrameworks** add-on to handle presets of `ofParameter`'s nested into single or multiple `ofParameterGroup` containers. Also powered with a nice GUI, many Helpers, and a complete user workflow.

**NOTE**  
_If you don't need multiple and independent and linkable groups,_  
_then I recommend using my simpler add-on [ofxSurfingPresets](https://github.com/moebiussurfing/ofxSurfingPresets)._  

## Features

- Easy to integrate.
- Auto-generates a **GUI** to tweak all the parameters.
- GUI based on **Dear ImGui**.
- Clone, Copy, Swap, and **Organize Presets**.
- Handle **Many Groups** independently. 
  - **Browse** multi Groups and Presets by **arrows keys**.
- **BPM Timed Randomizer** Players to switch between presets independently for each group.
- Tweaking for **probabilities** and **Short** or **Long** time **durations**.
- Two **Types of Presets**
  - **Favorite**: Fast clickable boxes/key-trigged presets.  
  - **Standalone**: Archived and named presets that we can load into and from favorites.  
- **Undo** history engine.

</p>
</details>

## Usage

1. Set up your scene and **initialize** the related **ofParameter**'s.
2. Add the parameters to the **ofParameterGroup** container.
3. Add the group to the add-on. 
4. Set **how many** presets you want for the group, and **what keys** to associate as triggers.
5. Done! Just Play with the addon GUI

## Examples

### 1_Basic (only one group)

Illustrates how to handle some `ofParameter`'s bundled into one `ofParameterGroup` used as a container.  

![](Examples/1_Basic/Capture.PNG)  

<details>
  <summary>CODE</summary>
  <p>

**ofApp.h**
```.c++
#include "ofxPresetsManager.h"

ofxPresetsManager presetsManager;

ofParameterGroup params; 
ofParameter<int> shapeType;
ofParameter<int> amount;
```

**ofApp.cpp**
```.c++
ofApp::setup()
{
    // Set the parameters 
    // and add to the ofParameterGroup container. 
    params.setName("sceneParamsGroup");    
    params.add(shapeType.set("shapeType", 1, 1, 2));
    params.add(amount.set("amount", 10, 1, 24));

    // Add the Group to the Preset Manager
    // Set the amount of Presets and each Key Trigger 
    presetsManager.add(params, { 'a', 'b', '0', '1', '2', '3' });
    presetsManager.setup();
}

ofApp::draw()
{
    // Gui
    presetsManager.draw();
}

// Done!
// Nothing more!
```
</p>
</details>

### 2_MultiGroup

Helps to learn how to handle multiple `ofParameterGroup`'s.  

![](Examples/2_MultiGroup/Capture.PNG)

When adding individually multiple `ofParameterGroup` we can handle them independently.  
We can use **arrow keys** to browse the Groups (up/down) and their **Selectors** (left/right).  
Also, we will have an extra **Global Selector** called **GROUP_LINK**, which groups all the other group Selectors indexes:  

<details>
  <summary>CODE</summary>
  <p>

```.cpp
// Group 0
presetsManager.add(params0, { 'q', 'w', 'e', 'r', 't' });
// Group 1
presetsManager.add(params1, { 'a', 's', 'd', 'f' });
// Group 2
presetsManager.add(params2, { 'z', 'x', 'c' });
// Group 3
presetsManager.add(params3, { 'b', 'n', 'm', ',', '.' });

presetsManager.setup();
```
</p>
</details>

<details>
  <summary>ExamplesAdvanced</summary>
  <p>

#### 3A_example-LinkServer and 3B_example-LinkClient:

Both examples run linked together using **ofxRemoteParameters**.  
This allows you to control the presets on the **Client OF_App**.  
The **Server OF_App** draws your scene using the *linked/received* parameters from the **Client OF_App**.  
This is useful because in some scenarios could help to improve the performance or the compiling time reducing dependencies. [OUTDATED VIDEO](http://www.youtube.com/watch?v=kV-t8lIdNRg "VIDEO") 
</p>
</details>

## Dependencies

* [ofxImGui](https://github.com/Daandelange/ofxImGui/tree/jvcleave) / _Fork from  @Daandelange_
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)  
* [ofxRemoteParameters](https://github.com/c-mendoza/ofxRemoteParameters) / _Not required. For the examples 3A-3B_  

## Thanks

_Thanks to all the above add-ons coders!_  

_Special Thanks 1 goes to **Nicola Pisanti** for the original https://github.com/npisanti/ofxGuiPresetSelector  
which inspired this add-on a lot. Some of his code is also used here._

_Special Thanks 2 goes to **@Daandelange**_ for his work to the DEFINITIVE **ofxImGui** repository. Thanks, **Daan**!  

## Tested Systems

- **Windows10** / **VS2017** / **OF ~0.11+**
- Should work on **macOS** too.  

<details>
  <summary>IDEAS / TODO</summary>
  <p>

* A Better performant version without using hard disk files.  

* **Memory Mode**, using a vector of XML/JSON instead of files.

* Improve (silent) de-serialization and ofParameter callbacks / triggering to improve speed.

* Add params tweening or filtering to ease transitions between presets when switching.

* Add a minimal/lite class to play compatible preset files but without any GUI or using a tiny **ofxGui**/**ImGui** version.

* More ideas noted into **ofxPresetsManager.h**.   

* **Feedback**, **Issues**, **PR**'s, and any kind of help are very welcome!
  
  </p>
  </details>

## Author

An add-on by **@moebiusSurfing**  
*( ManuMolina ) 2019-2022*

## License

*MIT License*
