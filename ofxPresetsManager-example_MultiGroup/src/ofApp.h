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

    void drawScene1();
    void drawScene2();
    void drawScene3();

    //presetsManager
    ofxPresetsManager presetsManager;

	//group0
    ofParameterGroup params0;
    ofParameterGroup paramsNested;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;//ofColor not implemented on ImGui yet
	ofParameter<int> shapeType;
	ofParameter<int> numShapes;
	ofParameter<int> separation;
    ofParameter<int> shapeSide;
	
	//group1
	ofParameterGroup params1;
	ofParameter<int> numShapes1;
	ofParameter<int> separation1;
	ofParameter<int> shapeSide1;
	
	//group3
	ofParameterGroup params2;
	ofParameter<ofFloatColor> color2;
	ofParameter<bool> fill2;
	
	//group4
	ofParameterGroup params3;	
	ofParameter<int> numTriangles;
	ofParameter<float> sizeTriangles;
	ofParameter<int> separationTriangles;
	ofParameter<ofFloatColor> color3;
	
	//gui panels
	ofxPanel gui0;
	ofxPanel gui1;
	ofxPanel gui2;
	ofxPanel gui3;
};
