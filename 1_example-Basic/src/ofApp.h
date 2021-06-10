#pragma once

#include "ofMain.h"

/*

	NOTE ABOUT THIS EXAMPLE

	* To run this example you must go to ofxPresetsManager.h and comment/disable all the surfing engines ->

//	1. SURFING ENGINES
//
//	Tween/Smooth transitions between presets
//	We can enable only one of the two! (Tween or Smooth)
//
//	1.2 TWEEN
#define INCLUDE_ofxSurfingTween
//
//	1.1 SMOOTH
//#define INCLUDE_ofxSurfingSmooth
//
//	1.3 RANDOMIZER
#define INCLUDE_ofxSurfingRandomizer

*/


#include "ofxPresetsManager.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void draw();
	void keyPressed(int key);

	// all the parameters for the scene 
	ofParameterGroup paramsNested1;
	ofParameterGroup paramsNested2;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;
	ofParameter<int> shapeType;
	ofParameter<int> amount;
	ofParameter<int> separation;
	ofParameter<int> size;

	// the main group container
	ofParameterGroup params;

	// presetsManager
	ofxPresetsManager presetsManager;

	void setupScene();
	
	ofTrueTypeFont font;
};
