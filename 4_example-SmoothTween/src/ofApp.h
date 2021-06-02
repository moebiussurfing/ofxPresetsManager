#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"
#include "ofxWindowApp.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void draw();
	void keyPressed(int key);

	// scene parameters 

	// main group 1
	ofParameterGroup params1;
	ofParameterGroup paramsNested11;
	ofParameterGroup paramsNested12;
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
};
