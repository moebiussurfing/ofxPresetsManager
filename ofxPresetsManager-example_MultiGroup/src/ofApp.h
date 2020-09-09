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

    void drawScene0();
    void drawScene1();
    void drawScene2();
    void drawScene3();

    //presetsManager
    ofxPresetsManager presetsManager;

	//group0
    ofParameterGroup params0;
    ofParameterGroup params0_Nested;
	ofParameter<bool> fill0;
	ofParameter<float> lineWidth0;
	ofParameter<ofFloatColor> color0;//bc ofColor not implemented on ImGui yet
	ofParameter<int> shapeType0;
	ofParameter<int> numShapes0;
	ofParameter<int> separation0;
    ofParameter<int> shapeSide0;
	
	//group1
	ofParameterGroup params1;
	ofParameter<ofFloatColor> color1;
	ofParameter<int> numShapes1;
	ofParameter<int> separation1;
	ofParameter<int> shapeSide1;
	
	//group2
	ofParameterGroup params2;
	ofParameter<ofFloatColor> color2;
	ofParameter<float> size2;
	ofParameter<bool> fill2;
	
	//group3
	ofParameterGroup params3;	
	ofParameter<int> numTriangles3;
	ofParameter<float> sizeTriangles3;
	ofParameter<int> separationTriangles3;
	ofParameter<ofFloatColor> color3;
	
	//gui panels
	ofxPanel gui0;
	ofxPanel gui1;
	ofxPanel gui2;
	ofxPanel gui3;
};
