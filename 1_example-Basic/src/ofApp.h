#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void draw();

	// the main container
	ofParameterGroup params;
	
	// all the parameters for the scene 
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

	// scene
	void setupScene();
	void drawScene();
	
	ofTrueTypeFont font;
};
