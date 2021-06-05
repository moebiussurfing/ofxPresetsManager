#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setupScene()
{
	// group 1 
	params1.setName("paramsGroup1");
	paramsNested11.setName("styleSubGroup");// another nested group
	paramsNested12.setName("itemSubGroup");// another nested group
	paramsNested11.add(fill.set("fill", true));
	//paramsNested11.add(color.set("color", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255)));//TODO: crash with smoother
	paramsNested11.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	paramsNested12.add(shapeType.set("shapeType", 1, 1, 2));
	paramsNested12.add(size.set("size", 100, 5, 200));
	paramsNested12.add(paramsNested11);
	params1.add(amount.set("amount", 10, 1, 24));
	params1.add(separation.set("separation", 10, 1, 100));
	params1.add(paramsNested12);

	// group 2
	params2.setName("paramsGroup2");
	params2.add(shapeType2.set("shapeType2", 0, -50, 50));
	params2.add(size2.set("size2", 100, 0, 100));
	params2.add(amount2.set("amount2", 10, 0, 25));

	// group(s) ready to add to presetsManager!
}

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetCircleResolution(200);

	// define our parameters (ofParameterGroup params) for our scene 
	// and add them to our params
	setupScene();

	// add our ofParameterGroup to the presetManager 
	// and define key triggers for each preset. 
	// the amount of keys will be the amount of favourites presets
	presetsManager.add(params1, { 'q', 'w', 'e', 'r' });
	presetsManager.add(params2, { 'a', 's', 'd' , 'f' });

	presetsManager.setGroupLinkSize(4); // customize GROUP_LINK size
	presetsManager.setup(); // must call after adding all the ofParameterGroup(s)
	presetsManager.setSizeBox_PresetClicker(50); // a bit smaller
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(64);

	//-

	// 1. create aux variables 
	// to store smoothed/tweened parameters

	float _lineWidth;
	int _shapeType;
	int _size;
	int _amount;
	int _separation;

	//-

	// 2. get the params

	_lineWidth = presetsManager.get(lineWidth);
	_shapeType = presetsManager.get(shapeType);
	_size = presetsManager.get(size);
	_amount = presetsManager.get(amount);
	_separation = presetsManager.get(separation);
	//bool _fill = smoother.get(fill);// not implemented

	//-

	// 2. draw the scene

	ofPushStyle();
	ofPushMatrix();
	ofTranslate(500, 50);
	ofSetColor(color.get());
	ofSetLineWidth(_lineWidth);
	if (fill) ofFill();
	else ofNoFill();
	for (int i = 0; i < _amount; ++i)
	{
		if (_shapeType == 1) ofDrawRectangle(0, 0, _size, _size);
		else if (_shapeType == 2 || _shapeType > 2) ofDrawCircle(0, 0, _size);
		//TODO:
		//fix _shapeType >2 rounding ints on tween!
		ofTranslate(_separation, _separation);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == ' ') { presetsManager.load_Next(true); }
	//if (key == ' ') { presetsManager.load_Next(0, true); }
	if (key == 'g') {}
	if (key == OF_KEY_RETURN) {}
}
