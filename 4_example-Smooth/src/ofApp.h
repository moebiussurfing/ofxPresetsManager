#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"
#include "ofxWindowApp.h"
//#include "ofxSurfingRandomizer.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void draw();
	void keyPressed(int key);

	// scene parameters 

	// main group 1
	ofParameterGroup params;
	ofParameterGroup paramsNested1;
	ofParameterGroup paramsNested2;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;
	ofParameter<int> shapeType;
	ofParameter<int> amount;
	ofParameter<int> separation;
	ofParameter<int> size;

	// main group 2
	ofParameterGroup params2;
	ofParameter<int> shapeType2;
	ofParameter<int> amount2;
	ofParameter<int> size2;
	
	// presets manager
	ofxPresetsManager presetsManager;

	void setupScene();
	
	ofxWindowApp windowApp;

	//ofxSurfingRandomizer randomizer;
	//ofParameterGroup params_Randomizator;
};
