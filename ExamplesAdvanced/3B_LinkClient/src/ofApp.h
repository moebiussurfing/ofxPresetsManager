#pragma once

#include "ofMain.h"

#include "ofxRemoteParameters/Client.h"

//#define USE_PRESETS
#ifdef USE_PRESETS
#include "ofxPresetsManager.h"
#endif

#include "ofxImGui.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void draw();
	
	ofxImGui::Gui gui;
	ofTrueTypeFont font;

#ifdef USE_PRESETS
	ofxPresetsManager presetsManager;
#endif

	//--

	// -> Remote

public:
	// Parameters group
	ofParameterGroup params_Remote;
	void setupRemote();
	void drawRemote();
	void buildGui();
	ofxRemoteParameters::Client paramClient;
	ofEventListener modelLoadedEventListener;
};
