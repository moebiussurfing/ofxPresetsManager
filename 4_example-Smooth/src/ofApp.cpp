#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//ofSetFrameRate(60);

	//-

	ofSetCircleResolution(200);

	// define our parameters (ofParameterGroup params) for our scene 
	// and add them to our params
	setupScene();

	// add our ofParameterGroup to the presetManager 
	// and define key triggers for each preset. 
	// the amount of keys will be the amount of favourites presets
	presetsManager.add(params, { '0', '1', '2', '3', '4' });
	presetsManager.add(params2, { 'q', 'w', 'e', 'r' });
	presetsManager.setup(); // must call after adding all the ofParameterGroup(s)
	presetsManager.setSizeBox_PresetClicker(50); // a bit smaller
}

//--------------------------------------------------------------
void ofApp::setupScene()
{
	params.setName("paramsGroup");// main group 
	paramsNested1.setName("styleSubGroup");// another nested group
	paramsNested2.setName("itemSubGroup");// another nested group
	paramsNested1.add(fill.set("fill", true));
	//paramsNested1.add(color.set("color", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255)));//TODO: crash with smoother
	paramsNested1.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	paramsNested2.add(shapeType.set("shapeType", 1, 1, 2));
	paramsNested2.add(size.set("size", 100, 5, 200));
	paramsNested2.add(paramsNested1);
	params.add(amount.set("amount", 10, 1, 24));
	params.add(separation.set("separation", 10, 1, 100));
	params.add(paramsNested2);

	params2.setName("paramsGroup2");// main group 
	params2.add(shapeType2.set("shapeType2", 0, -50, 50));
	params2.add(size2.set("size2", 100, 0, 100));
	params2.add(amount2.set("amount2", 10, 0, 25));

	// group(s) ready to add to presetsManager!
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(64);

	//-

	// get scene parameters

	float _lineWidth;
	int _shapeType;
	int _size;
	int _amount;
	int _separation;

	// A. get smoothed params
	//bool _fill = smoother.get(fill);// not implemented
	_lineWidth = presetsManager.get(lineWidth);
	_shapeType = presetsManager.get(shapeType);
	_size = presetsManager.get(size);
	_amount = presetsManager.get(amount);
	_separation = presetsManager.get(separation);

	//// B. raw params (not smoothed)
	//_lineWidth = (lineWidth);
	//_shapeType = (shapeType);
	//_size = (size);
	//_amount = (amount);
	//_separation = (separation);

	//-

	// scene draw

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
		else if (_shapeType == 2) ofDrawCircle(0, 0, _size);
		ofTranslate(_separation, _separation);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == ' ')
	{
		presetsManager.load_Next(0, true);
	}
	if (key == 'g')
	{
	}
	if (key == OF_KEY_RETURN)
	{
	}
}
