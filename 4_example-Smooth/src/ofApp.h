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
	ofParameterGroup paramsNested1;
	ofParameterGroup paramsNested2;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;
	ofParameter<int> shapeType;
	ofParameter<int> amount;
	ofParameter<int> separation;
	ofParameter<int> size;

	// group container
	ofParameterGroup params;

	// presets manager
	ofxPresetsManager presetsManager;

	void setupScene();
	
	ofxWindowApp windowApp;
};
