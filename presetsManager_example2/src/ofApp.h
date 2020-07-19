#pragma once

#include "ofMain.h"

///
///
//#define MODE_ImGui_EXTERNAL		//MUST be defined at the addon class (or in both places: here too)
///
///

#include "ofxPresetsManager.h"

#define USE_WindowApp
#ifdef USE_WindowApp
#include "ofxWindowApp.h"
#endif
#include "ofxGui.h"

#ifdef MODE_ImGui_EXTERNAL
#include "ofxImGui.h"
#endif

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

#ifdef MODE_ImGui_EXTERNAL
	ofxImGui::Gui gui_ImGui;
	void drawImGui();
	bool bMouseOverGui;//TODO: should have a callback to remove key listeners inside the class..
#endif

    //--

    //presetsManager
    ofxPresetsManager presetsManager;

    //--

    // group

	//group1
    ofParameterGroup params;
    ofParameterGroup paramsNested;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofColor> color;
	ofParameter<int> shapeType;
	ofParameter<int> numShapes;
	ofParameter<int> separation;
    ofParameter<int> shapeSide;
	
	//TODO:
	//group2
	ofParameterGroup params2;
	ofParameter<int> numShapes2;
	ofParameter<int> separation2;
	ofParameter<int> shapeSide2;

	//-

    //// TODO: 
	//alternative listener to be trigged when preset loading/saving is done
    //void Changed_DONE_load(bool &DONE_load);
    //void Changed_DONE_save(bool &DONE_save);
};
