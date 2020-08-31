#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"

#define USE_WindowApp
#ifdef USE_WindowApp
#include "ofxWindowApp.h"
#endif
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
	void windowResized(int w, int h);

#ifdef USE_WindowApp
	ofxWindowApp WindowApp;
#endif

	ofxPanel gui;
    void drawScene1();

    //presetsManager
    ofxPresetsManager presetsManager;

    // group

	//group1
    ofParameterGroup params;
    ofParameterGroup paramsNested;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;//ofColor not implemented on ImGui yet
	ofParameter<int> shapeType;
	ofParameter<int> numShapes;
	ofParameter<int> separation;
    ofParameter<int> shapeSide;
	
	//TODO:
	ofxPanel gui2;
    void drawScene2();
	//group2
	ofParameterGroup params2;
	ofParameter<int> numShapes2;
	ofParameter<int> separation2;
	ofParameter<int> shapeSide2;
};
