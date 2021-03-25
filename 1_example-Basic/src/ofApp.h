#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void draw();
	
	// all the parameters for the scene 
	ofParameterGroup paramsNested;
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
