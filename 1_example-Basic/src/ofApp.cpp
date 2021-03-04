#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setupScene()
{
	params.setName("sceneParamsGroup");
	params.add(shapeType.set("shapeType", 1, 1, 2));
	params.add(amount.set("amount", 10, 1, 24));
	params.add(separation.set("separation", 10, 1, 100));
	params.add(size.set("size", 100, 5, 200));
	paramsNested.setName("styleGroup");// another nested group
	paramsNested.add(fill.set("fill", true));
	paramsNested.add(color.set("color", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255)));
	paramsNested.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	params.add(paramsNested);

	// the group container is ready to be added to presetsManager!
}

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	font.load("assets/fonts/telegrama_render.otf", 11, true, true, true);

	// define our parameters (ofParameterGroup params) for our scene 
	// and add them to our params
	setupScene();

	// add our ofParameterGroup to the presetManager 
	// and define key triggers for each preset. 
	// the amount of keys will be the amount of favourites presets
	presetsManager.add(params, { '0', '1', '2', '3', '4', '5' });
	presetsManager.setup();// must call after adding all the ofParameterGroup(s)
}


/* Scene */

//--------------------------------------------------------------
void ofApp::drawScene()
{
	ofBackground(ofColor::darkBlue);

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

	// show help and debug preset
	std::string str = "HELP\n";
	str += "H            : Show Help\n";
	str += "0,1,2,3,4,5  : Presets Keys\n";
	str += "\n";

	str += "DEBUG SCENE\n";
	str += "shapeType    : " + ofToString(shapeType) + "\n";
	str += "amount       : " + ofToString(amount) + "\n";
	str += "separation   : " + ofToString(separation) + "\n";
	str += "size         : " + ofToString(size) + "\n";
	str += "fill         : " + ofToString(fill) + "\n";
	std::string sc = 
		ofToString(color.get().r, 1) + ", " +
		ofToString(color.get().g, 1) + ", " +
		ofToString(color.get().b, 1) + ", " +
		ofToString(color.get().a, 1);
	str += "color        : " + sc + "\n";
	str += "lineWidth    : " + ofToString(lineWidth);

	//ofDrawBitmapStringHighlight(str, 35, 50);
	ofxSurfingHelpers::drawTextBoxed(font, str, 35, 50);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	drawScene();

	// nothing more!
}