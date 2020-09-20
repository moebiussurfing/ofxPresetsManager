#pragma once

#include "ofMain.h"

//#define MODE_ImGui_EXTERNAL		//MUST be defined at the addon class (or in both places: here too)

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
    ofParameterGroup params;
    ofParameterGroup paramsNested;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;//ofColor not implemented on ImGui yet
	ofParameter<int> shapeType;
	ofParameter<int> numShapes;
	ofParameter<int> separation;
    ofParameter<int> shapeSide;

	//--

	//custom callback (B)
	ofEventListener listener_PresetManager_DoneLoad;
	//--------------------------------------------------------------
	void Changed_PresetManager_DoneLoad() {
		//ofLogWarning(__FUNCTION__) << "[ CustomCallback -> DoneLoad! ]";
		//cout << (__FUNCTION__) << "[ CustomCallback -> DoneLoad! ]" << endl;
	}
};