#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"
//#include "ofxSurfingSmooth.h"

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

	//ofxSurfingSmooth smoother;

};
