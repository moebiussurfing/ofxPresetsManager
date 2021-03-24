#pragma once

#include "ofMain.h"

#include "ofxRemoteParameters/Client.h"

#include "ofxPresetsManager.h"
#include "ofxImGui.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void draw();
	
	void buildGui();
	ofxImGui::Gui gui;
	ofTrueTypeFont font;

	ofxRemoteParameters::Client paramClient;
	ofEventListener modelLoadedEventListener;

	ofParameterGroup params;// group

	ofxPresetsManager presetsManager;
};
