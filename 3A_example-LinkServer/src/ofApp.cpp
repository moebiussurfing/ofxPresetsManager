#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowTitle("OF SERVER ofxRemoteParameters");
	ofSetWindowPosition(1920 / 2, 25);

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(60);
	ofSetCircleResolution(100);
	font.load("assets/fonts/telegrama_render.otf", 11);

	params.setName("paramsGroup");
	params.add(shapeType.set("shapeType", 1, 1, 2));
	params.add(amount.set("amount", 10, 1, 24));
	params.add(separation.set("separation", 10, 1, 100));
	params.add(size.set("size", 100, 5, 200));
	params.add(position.set("position", { 200, 200 }, { 0, 0 }, { ofGetWidth(),ofGetHeight() }));
	paramsNested.setName("styleGroup");// another nested group
	paramsNested.add(fill.set("fill", true));
	paramsNested.add(color.set("color", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255)));
	paramsNested.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	params.add(paramsNested);

	paramServer.setup(params);

	gui.setup();
	ImGui::StyleColorsLight();
	//ImGui::StyleColorsDark();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	/* scene */

	ofBackground(ofColor::darkBlue);

	// scene draw object linked to grouped parameters
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(position.get().x, position.get().y);
	ofSetColor(color.get());
	ofSetLineWidth(lineWidth);
	if (fill) ofFill();
	else ofNoFill();
	for (int i = 0; i < amount; ++i)
	{
		if (shapeType == 1) ofDrawRectangle(0, 0, size, size);
		else if (shapeType == 2) ofDrawCircle(0, 0, size);
		ofTranslate(separation, separation);
	}
	ofPopMatrix();
	ofPopStyle();

	//-

	/* gui */

	auto mainSettings = ofxImGui::Settings();
	gui.begin();
	ofxImGui::AddGroup(params, mainSettings);
	gui.end();
}
