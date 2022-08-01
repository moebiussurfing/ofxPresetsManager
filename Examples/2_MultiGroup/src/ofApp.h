#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"

class ofApp : public ofBaseApp
{
public:

	void setup();
    void draw();
    void keyPressed(int key);

	//--

	// Presets Manager
	ofxPresetsManager presetsManager;

	void setupPresets();
	bool bGui = true;

	//--

	// Scene

	// ofParameters
	 
	// group0
	ofParameter<bool> show0;
    ofParameterGroup params0;
    ofParameterGroup params0_Nested;
	ofParameter<ofColor> color0;
	ofParameter<bool> fill0;
	ofParameter<float> lineWidth0;
	ofParameter<int> shapeType0;
	ofParameter<int> numObjects0;
	ofParameter<int> separation0;
    ofParameter<int> shapeSide0;
	
	// group1
	ofParameter<bool> show1;
	ofParameterGroup params1;
	ofParameter<ofColor> color1;
	ofParameter<int> numObjects1;
	ofParameter<int> separation1;
	ofParameter<int> shapeSide1;
	
	// group2
	ofParameter<bool> show2;
	ofParameterGroup params2;
	ofParameter<ofColor> color2;
	ofParameter<int> numObjects2;
	ofParameter<float> size2;
	ofParameter<bool> fill2;
	
	// group3
	ofParameter<bool> show3;
	ofParameterGroup params3;	
	ofParameter<ofColor> color3;
	ofParameter<int> numObjects3;
	ofParameter<float> size3;
	ofParameter<int> separation3;

	//--
	
	void setupScene();

	void drawScene0();
    void drawScene1();
    void drawScene2();
    void drawScene3();
	float _alpha = 255/0.4f;

};
