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
	
	void buildGui();
	ofxImGui::Gui gui;
	ofTrueTypeFont font;

	ofParameterGroup params; // parameters group

#ifdef USE_PRESETS
	ofxPresetsManager presetsManager;
#endif

	// Remote
	ofxRemoteParameters::Client paramClient;
	ofEventListener modelLoadedEventListener;
};
