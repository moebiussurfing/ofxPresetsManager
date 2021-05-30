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

	ofParameterGroup params;// group container
	ofParameterGroup paramsNested1;
	ofParameterGroup paramsNested2;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;
	ofParameter<int> shapeType;
	ofParameter<int> amount;
	ofParameter<int> separation;
	ofParameter<int> size;

	ofParameterGroup params2;// group container
	ofParameter<int> shapeType2;
	ofParameter<int> amount2;
	ofParameter<int> size2;
	
	// presets manager
	ofxPresetsManager presetsManager;

	void setupScene();
	
	ofxWindowApp windowApp;
};
