#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"

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
