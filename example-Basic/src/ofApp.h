
//	ofxSphereCam2-exampleBasic

#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"
#include "ofxSurfingConstants.h" // -> defines (modes) are here "to share between addons" in one place

class ofApp : public ofBaseApp
{
public:
	void setup();
	void draw();
	void exit();

	// scene
	void setupScene();
	void drawScene();

	// preset params
	ofParameterGroup params;
	ofParameterGroup paramsNested;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;
	ofParameter<int> shapeType;
	ofParameter<int> amount;
	ofParameter<int> separation;
	ofParameter<int> size;

	// presetsManager
	ofxPresetsManager presetsManager;
};
