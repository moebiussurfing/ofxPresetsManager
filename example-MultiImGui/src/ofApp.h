#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"
#include "ofxImGui.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void draw();
	void exit();

	// scene
	void setupScene();
	void drawScene();

	// preset params
	ofParameterGroup params;
	ofParameterGroup paramsNested;
	ofParameter<bool> fill;
	ofParameter<float> lineWidth;
	ofParameter<ofFloatColor> color;
	ofParameter<int> shapeType;
	ofParameter<int> amount;
	ofParameter<int> separation;
	ofParameter<int> size;

	// presetsManager
	ofxPresetsManager presetsManager;

	ofxImGui::Gui gui;
	float v1;
	float v2;

	void drawImGui() {
		// [...] some code
		gui.begin(); //   <-- First call = Normal behaviour
		ImGui::SliderFloat("slider1", &v1, 0.f, 10.f); //....
		ImGui::SliderFloat("slider2", &v2, 0.f, 10.f); //....
		gui.end(); //     <-- Does nothing with chaining on
		// [...] more code
		{ // scope = called plugin code that you can't change
			gui.begin(); // <-- Does nothing with chaining on
			ImGui::SliderFloat("slider1", &v1, 0.f, 10.f); //....
			ImGui::SliderFloat("slider2", &v2, 0.f, 10.f); //....
			gui.end(); //   <-- Does nothing with chaining on
		}
		gui.draw(); //    <-- Also calls gui.end()
	}
};
