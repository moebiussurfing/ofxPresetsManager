#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setupScene()
{
	// main group 1
	params.setName("paramsGroup");
	
	paramsNested1.setName("styleSubGroup");// another nested group
	paramsNested2.setName("itemSubGroup");// another nested group
	paramsNested1.add(fill.set("fill", true));
	paramsNested1.add(color.set("color", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255)));
	paramsNested1.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	paramsNested2.add(shapeType.set("shapeType", 1, 1, 2));
	paramsNested2.add(size.set("size", 100, 5, 200));
	paramsNested2.add(paramsNested1);
	params.add(amount.set("amount", 10, 1, 24));
	params.add(separation.set("separation", 10, 1, 100));
	params.add(paramsNested2);

	// the group container is ready to be added to presetsManager!
}

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate(60);
	//ofSetVerticalSync(true);

	////window shape
	//int _gap = 28;
	//ofSetWindowPosition(1920, _gap);
	//ofSetWindowShape(1920, 1080 - _gap);

	font.load("assets/fonts/telegrama_render.otf", 11, true, true, true);
	ofSetCircleResolution(200);

	// define our parameters (ofParameterGroup params) for our scene 
	// and add them to our params
	setupScene();

	// add our ofParameterGroup to the presetManager 
	// and define key triggers for each preset. 
	// the amount of keys will be the amount of favourites presets
	presetsManager.add(params, { '0', '1', '2', '3', '4', '5' });

	presetsManager.setup();// must call after adding all the ofParameterGroup(s)
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(64);

	// scene draw object linked to grouped parameters
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(500, 50);
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
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == ' ') presetsManager.load_Next(0, true);
}