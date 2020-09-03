#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"

#define USE_WindowApp
#ifdef USE_WindowApp
#include "ofxWindowApp.h"
#endif
#include "ofxSurfingHelpers.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
public:
    void setup();
    void setupParameters();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
	void windowResized(int w, int h);

#ifdef USE_WindowApp
	ofxWindowApp WindowApp;
#endif

    void drawDebug1();
    void drawDebug2();

    //presetsManager
    ofxPresetsManager presetsManager;

	//group1
    ofParameterGroup params1;
    ofParameterGroup paramsNested;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;//ofColor not implemented on ImGui yet
	ofParameter<int> shapeType;
	ofParameter<int> numShapes;
	ofParameter<int> separation;
    ofParameter<int> shapeSide;
    void drawScene1();
	ofxPanel gui1;
	
	//group2
	ofParameterGroup params2;
	ofParameter<int> numShapes2;
	ofParameter<int> separation2;
	ofParameter<int> shapeSide2;
	ofxPanel gui2;
    void drawScene2();
	
	//group3
	ofParameterGroup params3;
	ofxPanel gui3;
	
	//group4
	ofParameterGroup params4;
	ofxPanel gui4;
};
