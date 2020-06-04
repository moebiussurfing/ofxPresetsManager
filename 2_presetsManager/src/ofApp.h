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

#ifdef USE_WindowApp
	ofxWindowApp WindowApp;
#endif

    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
	void windowResized(int w, int h);

	ofxPanel gui;

    //--

    //presetsManager
    ofxPresetsManager presetsManager;

    //--

    // group

	//group1
    ofParameterGroup params;
	ofParameter<int> shapeType;
	ofParameter<int> numShapes;
	ofParameter<int> separation;
    ofParameter<int> shapeSide;

	////group2
	//ofParameterGroup params2;
	//ofParameter<int> numCircles;
	//ofParameter<int> circleSeparation;
	//ofParameter<int> circleSide;

	//-

    //// TODO: 
	//alternative listener to be trigged when preset loading/saving is done
    //void Changed_DONE_load(bool &DONE_load);
    //void Changed_DONE_save(bool &DONE_save);
};
