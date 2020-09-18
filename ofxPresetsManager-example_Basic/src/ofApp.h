
//	ofxSphereCam2-exampleBasic
//
//	this is a very simple example to use ofxPresetsManager.
//	we will add one ofParameterGroup but the addon can handle more than one group
//	0. setup (our scene and) the related parameters/settings
//	1. add our parameters to the ofParameterGroup as container
//	2. add the container to the addon object
//	3. done! just play with the addon gui. 
//	the file settings (one for each preset) will be placed into /bin/data

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
