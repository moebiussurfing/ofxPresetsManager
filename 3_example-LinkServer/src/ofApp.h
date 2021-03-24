#pragma once

#include "ofMain.h"

#include "ofxRemoteParameters/Server.h"
#include "ofxPresetsManager.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void draw();

	ofTrueTypeFont font;

	ofxRemoteParameters::Server paramServer;

	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;
	ofParameter<int> shapeType;
	ofParameter<int> amount;
	ofParameter<int> separation;
	ofParameter<int> size;
	ofParameter<glm::vec2> position;
	ofParameterGroup paramsNested;

	ofParameterGroup params;// group

	ofxPresetsManager presetsManager;
};
