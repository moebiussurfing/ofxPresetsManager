#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"

#include "ofxGui.h"

#include "ofxSurfingHelpers.h"	// optional: to use setThemeDark_ofxGui only
#define USE_WindowApp			// optional: to handle app window only

#ifdef USE_WindowApp
#include "ofxWindowApp.h"
#endif

class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
	void windowResized(int w, int h);

	// group0
    ofParameterGroup params0;
    ofParameterGroup params0_Nested;
	ofParameter<ofFloatColor> color0;// bc ofColor not implemented on ImGui yet
	ofParameter<bool> show0;
	ofParameter<bool> fill0;
	ofParameter<float> lineWidth0;
	ofParameter<int> shapeType0;
	ofParameter<int> numShapes0;
	ofParameter<int> separation0;
    ofParameter<int> shapeSide0;
	
	// group1
	ofParameterGroup params1;
	ofParameter<ofFloatColor> color1;
	ofParameter<bool> show1;
	ofParameter<int> numShapes1;
	ofParameter<int> separation1;
	ofParameter<int> shapeSide1;
	
	// group2
	ofParameterGroup params2;
	ofParameter<ofFloatColor> color2;
	ofParameter<bool> show2;
	ofParameter<float> size2;
	ofParameter<bool> fill2;
	
	// group3
	ofParameterGroup params3;	
	ofParameter<ofFloatColor> color3;
	ofParameter<bool> show3;
	ofParameter<int> numObjects3;
	ofParameter<float> sizeObjects3;
	ofParameter<int> separationObjects3;

    void setupParameters();

    // presetsManager
    ofxPresetsManager presetsManager;

	// scene
	void drawScene0();
    void drawScene1();
    void drawScene2();
    void drawScene3();
	
	// gui panels
	ofxPanel gui0;
	ofxPanel gui1;
	ofxPanel gui2;
	ofxPanel gui3;
	bool bGui = true;

#ifdef USE_WindowApp
	ofxWindowApp WindowApp;
#endif
};
