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

	//0. setup your parameters
	setupParameters();

	//--

	//1. (optional) customize path folders at first
	//presetsManager.setPath_UserKit_Folder("myAddon");
	//main container folder where all other files will be

	//--

	//2. add our ofParameterGroup to the preset manager 
	//also define wich key triggers are associated to each preset. 
	//the amount of keys will be also the amount of favorites/clickable presets per group

	//group 0
	presetsManager.add(params0, { 'q', 'w', 'e', 'r', 't' });

	//group 1
	presetsManager.add(params1, { 'a', 's', 'd', 'f' });

	//group 2
	presetsManager.add(params2, { 'z', 'x', 'c' });

	//group 3
	presetsManager.add(params3, { 'b', 'n', 'm', ',', '.' });

	//--

	//3. call setup after adding ofParameterGroup
	std::string name = "myKit_01";
	presetsManager.setup(name);

	//4. show clicker
	presetsManager.setVisible_PresetClicker(true);//user panel clicker

}

//--------------------------------------------------------------
void ofApp::setupParameters()
{
	//0. ofParameterGroup's

	//group0
	params0.setName("paramsGroup0");
	params0.add(color0.set("color0", ofColor(ofColor::red, 0.8), ofColor(0, 0), ofColor(255, 255)));
	params0.add(numShapes0.set("numShapes0", 5, 1, 5));
	params0.add(shapeType0.set("shapeType0", 1, 1, 2));
	params0.add(separation0.set("separation0", 100, 1, 100));
	params0.add(shapeSide0.set("shapeSide0", 50, 5, 200));
	params0_Nested.setName("styleGroup");//another nested group
	params0_Nested.add(fill0.set("fill0", true));
	params0_Nested.add(lineWidth0.set("lineWidth0", 1, 0.1, 10));
	params0.add(params0_Nested);//main preset settings container

	//group1
	params1.setName("paramsGroup1");
	params1.add(color1.set("color1", ofColor(ofColor::green, 0.8), ofColor(0, 0), ofColor(255, 255)));
	params1.add(numShapes1.set("numShapes1", 5, 1, 5));
	params1.add(shapeSide1.set("shapeSide1", 50, 5, 200));
	params1.add(separation1.set("separation1", 100, 1, 100));

	//group2
	params2.setName("paramsGroup2");
	params2.add(color2.set("color2", ofColor(ofColor::yellow, 0.8), ofColor(0, 0), ofColor(255, 255)));
	params2.add(size2.set("size2", 50, 5, 200));
	params2.add(fill2.set("fill2", true));

	//group3
	params3.setName("paramsGroup3");
	params3.add(color3.set("color3", ofColor(ofColor::blueViolet, 0.8), ofColor(0, 0), ofColor(255, 255)));
	params3.add(numTriangles3.set("numTriangles3", 5, 1, 5));
	params3.add(sizeTriangles3.set("sizeTriangles3", 5, 1, 200));
	params3.add(separationTriangles3.set("separationTriangles3", 50, 5, 100));

	//--

	//1. guis

	//local ofApp gui to show/edit our settings/parameters
	//to show how our params/settings changes when using the presets manager
	gui0.setup("ofApp0");
	gui0.add(params0);
	gui0.setPosition(5, 10);

	gui1.setup("ofApp1");
	gui1.add(params1);
	gui1.setPosition(5 + 1 * (5 + 200), 10);

	gui2.setup("ofApp2");
	gui2.add(params2);
	gui2.setPosition(5 + 2 * (5 + 200), 10);

	gui3.setup("ofApp3");
	gui3.add(params3);
	gui3.setPosition(5 + 3 * (5 + 200), 10);
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
	drawScene0();
	drawScene1();
	drawScene2();
	drawScene3();

	//local gui parameters
	gui0.draw();
	gui1.draw();
	gui2.draw();
	gui3.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	//customize layout
	//bottom and centered
	presetsManager.setSizeBox_PresetClicker(75);
	int _w = presetsManager.getPresetClicker_Width();
	presetsManager.setPosition_PresetClicker(ofGetWidth() / 2.f - _w / 2.f, ofGetHeight() - presetsManager.getPresetClicker_Height() - 20);
}

//--------------------------------------------------------------
void ofApp::exit()
{
	presetsManager.exit();//required to store settings. (maybe required before destruct our class params..)
}

//--------------------------------------------------------------
void ofApp::drawScene0()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(100, 700);
	ofSetColor(color0.get());
	ofSetLineWidth(lineWidth0);
	if (fill0) ofFill();
	else ofNoFill();
	for (int i = 0; i < numShapes0; ++i)
	{
		if (shapeType0 == 1) ofDrawRectangle(0, 0, shapeSide0 * i / 2, shapeSide0 * i / 2);
		else if (shapeType0 == 2) ofDrawCircle(0, 0, shapeSide0 * i / 2);
		ofTranslate(separation0, 0);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScene1()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(350, 700 - shapeSide1 / 10);
	ofSetColor(color1.get());
	ofFill();
	for (int i = 0; i < numShapes1; ++i)
	{
		ofDrawCircle(0, 0, shapeSide1);
		ofTranslate(separation1 * 3, 0);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScene2()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(100, 700);
	ofSetColor(color2.get());
	if (fill2) ofFill();
	else ofNoFill();
	for (int i = 0; i < 5; ++i)
	{
		ofDrawCircle(0, 0, (size2 + size2 / i) / 4);
		ofTranslate(size2 * i * 1.5, -size2 / 10 - i * 25);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScene3()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(400, 700);
	//ofSetLineWidth(2);
	ofFill();
	ofSetColor(color3.get());
	for (int i = 0; i < numTriangles3; ++i)
	{
		//ofDrawLine(0, 0, 0, sizeTriangles3);
		//ofDrawLine(0, sizeTriangles3, sizeTriangles3 / 2, sizeTriangles3 / 2);
		//ofDrawLine(sizeTriangles3 / 2, sizeTriangles3 / 2, 0, 0);
		ofDrawRectangle(0, 0, sizeTriangles3 * i / 2, sizeTriangles3 * i / 2);

		float _tr = i * (separationTriangles3 + 20);
		ofTranslate(_tr, -_tr);
	}
	ofPopMatrix();
	ofPopStyle();
}