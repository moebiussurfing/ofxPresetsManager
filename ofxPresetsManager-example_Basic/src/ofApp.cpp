#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate(60);
	ofSetVerticalSync(true);

	// define our parameters (ofParameterGroup params) and our scene
	setupScene();

	// add our ofParameterGroup to the presetManager 
	// and define key triggers for each preset. the amount of keys will be the amount of favourites presets
	presetsManager.add(params, { '0', '1', '2', '3', '4', '5' });
	presetsManager.setup();//must call after adding all the ofParameterGroup(s)
}

//--------------------------------------------------------------
void ofApp::setupScene()
{
	// preset parameters
	params.setName("sceneParamsGroup");// main group
	params.add(shapeType.set("shapeType", 1, 1, 2));
	params.add(amount.set("amount", 10, 1, 24));
	params.add(separation.set("separation", 10, 1, 100));
	params.add(size.set("size", 100, 5, 200));
	paramsNested.setName("styleGroup");// another nested group
	paramsNested.add(fill.set("fill", true));
	paramsNested.add(color.set("color", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255)));
	paramsNested.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	params.add(paramsNested);// preset main settings container	
}

//--------------------------------------------------------------
void ofApp::draw()
{
	drawScene();
}

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

	// debug object linked to grouped parameters
	int x = 15;
	int y = 30;
	string str = "";
	str += "shapeType : " + ofToString(shapeType); str += "\n";
	str += "amount    : " + ofToString(amount); str += "\n";
	str += "separation: " + ofToString(separation); str += "\n";
	str += "size      : " + ofToString(size); str += "\n";
	str += "fill      : " + ofToString(fill); str += "\n";
	str += "color     : " + ofToString(color); str += "\n";
	str += "lineWidth : " + ofToString(lineWidth);
	ofDrawBitmapStringHighlight(str, x, y);
}

//--------------------------------------------------------------
void ofApp::exit()
{
	presetsManager.exit();// call required to store current selected preset settings on exit. 
}