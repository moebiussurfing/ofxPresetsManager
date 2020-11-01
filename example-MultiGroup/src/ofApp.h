#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"

//#define USE_GUI_LOCAL //-> we can even use our personal local gui
#ifdef USE_GUI_LOCAL 
#include "ofxGui.h"
#include "ofxSurfingHelpers.h"
#include "ofxSurfingConstants.h"
#include "ofxSurfing_ofxGui.h"
#endif

class ofApp : public ofBaseApp
{

public:
	void setup();
    void draw();
    void exit();
    void keyPressed(int key);

	void setupParameters();

	// group0
    ofParameterGroup params0;
    ofParameterGroup params0_Nested;
	ofParameter<ofColor> color0;// bc ofColor not implemented on ImGui yet
	ofParameter<bool> show0;
	ofParameter<bool> fill0;
	ofParameter<float> lineWidth0;
	ofParameter<int> shapeType0;
	ofParameter<int> numObjects0;
	ofParameter<int> separation0;
    ofParameter<int> shapeSide0;
	
	// group1
	ofParameterGroup params1;
	ofParameter<ofColor> color1;
	ofParameter<bool> show1;
	ofParameter<int> numObjects1;
	ofParameter<int> separation1;
	ofParameter<int> shapeSide1;
	
	// group2
	ofParameterGroup params2;
	ofParameter<ofColor> color2;
	ofParameter<bool> show2;
	ofParameter<int> numObjects2;
	ofParameter<float> size2;
	ofParameter<bool> fill2;
	
	// group3
	ofParameterGroup params3;	
	ofParameter<ofColor> color3;
	ofParameter<bool> show3;
	ofParameter<int> numObjects3;
	ofParameter<float> size3;
	ofParameter<int> separation3;

    // presetsManager
    ofxPresetsManager presetsManager;

	//-

	// scene
	void drawScene0();
    void drawScene1();
    void drawScene2();
    void drawScene3();
	float _alpha = 255/0.4f;

	//-

	// gui panels
#ifdef USE_GUI_LOCAL 
	ofxPanel gui0;
	ofxPanel gui1;
	ofxPanel gui2;
	ofxPanel gui3;
#endif

	bool bGui = true;
};
