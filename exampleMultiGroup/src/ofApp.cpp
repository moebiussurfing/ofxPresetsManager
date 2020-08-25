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

	//--

	//0. our app parameters
	params.setName("paramsGroup");//main (target) group
	params.add(shapeType.set("shapeType", 1, 1, 2));
	params.add(numShapes.set("numShapes", 1, 1, 24));
	params.add(separation.set("separation", 5, 1, 100));
	params.add(shapeSide.set("shapeSide", 50, 5, 200));
	paramsNested.setName("styleGroup");//another nested group
	paramsNested.add(fill.set("fill", false));
	paramsNested.add(color.set("color", ofColor(0, 255), ofColor(0, 0), ofColor(255, 255)));
	paramsNested.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	params.add(paramsNested);//main preset settings container

	////TODO:
	////group2
	//params2.setName("paramsGroup2");
	//params2.add(shapeSide2.set("shapeSide2", 50, 5, 200));
	//params2.add(numShapes2.set("numShapes2", 10, 1, 24));
	//params2.add(separation2.set("separation2", 15, 1, 100));

	//--

	//1. (optional) customize path folders at first
	presetsManager.setPath_GlobalFolder("presetsManager");	//main container folder where all other files will be

	//2. add our ofParameterGroup to the preset manager 
	//also define wich key triggers are associated to each preset. 
	//the amount of keys will be also the amount of favorites/clickable presets
	presetsManager.add(params, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

	//TODO:
	//group2
	//presetsManager.add(params2, { 'a', 's', 'd', 'e' });

	//3. call setup after adding ofParameterGroup
	presetsManager.setup();

	//4. (optional) customize gui positions
	presetsManager.setVisible_PresetClicker(true);//user panel clicker
	presetsManager.setPosition_PresetClicker(200, 100, 50);
	
	//-------

	//local ofApp gui to show/edit our settings/parameters
	//to show how our params/settings changes when using the presets manager
	gui.setup("ofApp1");
	gui.add(params);
	gui.setPosition(50, 300);

	//gui2.setup("ofApp2");
	//gui2.add(params2);
	//gui2.setPosition(700, 300);
}

//--------------------------------------------------------------
void ofApp::update()
{
}

//--------------------------------------------------------------
void ofApp::drawScene1()
{
	//debug object linked to grouped parameters
	string str;
	int x = gui.getPosition().x + 15;
	int y = gui.getPosition().y + gui.getHeight() + 30;
	str = "";
	str += "shapeType : " + ofToString(shapeType);
	str += "numShapes : " + ofToString(numShapes);
	str += "separation: " + ofToString(separation);
	str += "shapeSide : " + ofToString(shapeSide);
	str += "fill      : " + ofToString(fill);
	str += "color     : " + ofToString(color);
	str += "lineWidth : " + ofToString(lineWidth);

	ofPushStyle();
	ofPushMatrix();

	ofTranslate(300, 50);
	ofSetColor(color.get());
	ofSetLineWidth(lineWidth);
	if (fill) ofFill();
	else ofNoFill();

	for (int i = 0; i < numShapes; ++i)
	{
		if (shapeType == 1)
			ofDrawRectangle(0, 0, shapeSide, shapeSide);
		else if (shapeType == 2)
			ofDrawCircle(0, 0, shapeSide);
		ofTranslate(separation, separation);
	}

	ofPopMatrix();
	ofPopStyle();
}

////--------------------------------------------------------------
//void ofApp::drawScene2()
//{
//	//debug object linked to grouped parameters
//	string str;
//	int x = gui2.getPosition().x + 15;
//	int y = gui2.getPosition().y + gui2.getHeight() + 30;
//	str = "";
//	str += "numShapes2 : " + ofToString(numShapes2);
//	str += "separation2: " + ofToString(separation2);
//	str += "shapeSide2 : " + ofToString(shapeSide2);
//
//	ofPushStyle();
//	ofPushMatrix();
//
//	ofTranslate(x + 200, 50);
//	ofSetColor(0);
//	ofFill();
//
//	for (int i = 0; i < numShapes2; ++i)
//	{
//		ofDrawCircle(0, 0, shapeSide2);
//		ofTranslate(-separation2, separation2);
//	}
//
//	ofPopMatrix();
//	ofPopStyle();
//}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(ofColor::darkBlue);

	//scene draw object linked to grouped parameters
	drawScene1();
	//drawScene2();

	//local gui parameters
	gui.draw();
	//gui2.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	//optional adapt to layout
	int cellw = 50;
	presetsManager.setPosition_PresetClicker(0.5* ofGetWidth() - (9 * cellw*0.5f), ofGetHeight() - 200, cellw);//position and boxes sizes
}

//--------------------------------------------------------------
void ofApp::exit()
{
	presetsManager.exit();//required to store settings. (maybe required before destruct our class params..)

	WindowApp.exit();
}