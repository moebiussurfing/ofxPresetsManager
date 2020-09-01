#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
#ifdef USE_WindowApp
	WindowApp.setFrameRate(60);
	WindowApp.setVerticalSync(true);
#else
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
#endif
	ofxSurfingHelpers::setTheme_ofxGui();

	//--

	//0. parameters
	setupParameters();

	//--

	//1. (optional) customize path folders at first
	presetsManager.setPath_UserKit_Folder("presetsManager");
	//main container folder where all other files will be

	//--

	//2. add our ofParameterGroup to the preset manager 
	//also define wich key triggers are associated to each preset. 
	//the amount of keys will be also the amount of favorites/clickable presets

	//group1
	presetsManager.add(params1, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

	//group2
	presetsManager.add(params2, { 'q', 'w', 'e', 'r', 't' });

	//group3
	presetsManager.add(params3, { 'a', 's', 'd', 'f' });

	//group4
	presetsManager.add(params4, { 'z', 'x', 'c', 'v', 'b' });

	//--

	//3. call setup after adding ofParameterGroup
	presetsManager.setup();

	//4. (optional) customize gui positions
	presetsManager.setVisible_PresetClicker(true);//user panel clicker
}

//--------------------------------------------------------------
void ofApp::setupParameters()
{
	//0. ofParameterGroups

	//group1
	params1.setName("paramsGroup1");//main (target) group
	params1.add(shapeType.set("shapeType", 1, 1, 2));
	params1.add(numShapes.set("numShapes", 1, 1, 24));
	params1.add(separation.set("separation", 5, 1, 100));
	params1.add(shapeSide.set("shapeSide", 50, 5, 200));
	paramsNested.setName("styleGroup");//another nested group
	paramsNested.add(fill.set("fill", false));
	paramsNested.add(color.set("color", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255)));
	paramsNested.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	params1.add(paramsNested);//main preset settings container

	//group2
	params2.setName("paramsGroup2");
	params2.add(shapeSide2.set("shapeSide2", 50, 5, 200));
	params2.add(numShapes2.set("numShapes2", 10, 1, 24));
	params2.add(separation2.set("separation2", 15, 1, 100));

	//group3
	params3.setName("paramsGroup3");
	params3.add(color);
	params3.add(fill);

	//group4
	params4.setName("paramsGroup4");
	params4.add(numShapes);
	params4.add(separation);
	params4.add(shapeType);
	params4.add(fill);

	//--

	//1. guis

	//local ofApp gui to show/edit our settings/parameters
	//to show how our params/settings changes when using the presets manager
	gui1.setup("ofApp1");
	gui1.add(params1);
	gui1.setPosition(5, 10);

	gui2.setup("ofApp2");
	gui2.add(params2);
	gui2.setPosition(5 + 1 * (5 + 200), 10);

	gui3.setup("ofApp3");
	gui3.add(params3);
	gui3.setPosition(5 + 2 * (5 + 200), 10);

	gui4.setup("ofApp4");
	gui4.add(params4);
	gui4.setPosition(5 + 3 * (5 + 200), 10);
}

//--------------------------------------------------------------
void ofApp::update()
{
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(ofColor::darkBlue);

	//scene draw object linked to grouped parameters
	drawScene1();
	drawScene2();

	//drawDebug1();
	//drawDebug2();
	
	//local gui parameters
	gui1.draw();
	gui2.draw();
	gui3.draw();
	gui4.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	//optional adapt to layout
	int _cellSize = 50;
	presetsManager.setPosition_PresetClicker(0.5* ofGetWidth() - (9 * _cellSize*0.5f), ofGetHeight() - 5 * _cellSize, _cellSize);//position and boxes sizes
}

//--------------------------------------------------------------
void ofApp::exit()
{
	presetsManager.exit();//required to store settings. (maybe required before destruct our class params..)

	WindowApp.exit();
}

//--------------------------------------------------------------
void ofApp::drawScene1()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(300, 500);
	ofSetColor(color.get());
	ofSetLineWidth(lineWidth);
	if (fill) ofFill();
	else ofNoFill();
	for (int i = 0; i < numShapes; ++i)
	{
		if (shapeType == 1) ofDrawRectangle(0, 0, shapeSide, shapeSide);
		else if (shapeType == 2) ofDrawCircle(0, 0, shapeSide);
		ofTranslate(separation, separation);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawDebug1()
{
	ofPushStyle();
	string str;
	int x = gui1.getPosition().x + 30;
	int y = gui1.getPosition().y + gui1.getHeight() + 30;
	str = "";
	str += "shapeType : " + ofToString(shapeType); str += "\n";
	str += "numShapes : " + ofToString(numShapes); str += "\n";
	str += "separation: " + ofToString(separation); str += "\n";
	str += "shapeSide : " + ofToString(shapeSide); str += "\n";
	str += "fill      : " + ofToString(fill); str += "\n";
	str += "color     : " + ofToString(color); str += "\n";
	str += "lineWidth : " + ofToString(lineWidth);
	ofDrawBitmapStringHighlight(str, gui1.getShape().x + 5, gui1.getShape().y + gui1.getHeight() + 30);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScene2()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(700, 500);
	ofSetColor(0);
	ofFill();
	for (int i = 0; i < numShapes2; ++i)
	{
		ofDrawCircle(0, 0, shapeSide2);
		ofTranslate(-separation2, separation2);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawDebug2()
{
	ofPushStyle();
	string str;
	int x = gui2.getPosition().x + 30;
	int y = gui2.getPosition().y + gui2.getHeight() + 30;
	str = "";
	str += "numShapes2 : " + ofToString(numShapes2); str += "\n";
	str += "separation2: " + ofToString(separation2); str += "\n";
	str += "shapeSide2 : " + ofToString(shapeSide2);
	ofDrawBitmapStringHighlight(str, gui2.getShape().x + 5, gui2.getShape().y + gui2.getHeight() + 30);
	ofPopStyle();
}