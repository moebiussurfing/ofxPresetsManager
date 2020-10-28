#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetCircleResolution(200);

	ofSetFrameRate(60);
	ofSetVerticalSync(true);

	//--

	// setup your parameters
	setupParameters();

	//--

	//presetsManager.setEnableKeysArrowBrowse(true);
	//presetsManager.setEnableKeys(true);
	presetsManager.setSizeBox_PresetClicker(50);

	// add our ofParameterGroup to the preset manager 
	// also define wich key triggers are associated to each preset. 
	// the amount of keys will be also the amount of favorites/clickable presets per group

	// group 0
	presetsManager.add(params0, { 'q', 'w', 'e', 'r', 't' });

	// group 1
	presetsManager.add(params1, { 'a', 's', 'd', 'f' });

	// group 2
	presetsManager.add(params2, { 'z', 'x', 'c' });

	// group 3
	presetsManager.add(params3, { 'b', 'n', 'm', ',', '.' });

	//--

	// customize user-kit name: will create this main settings file: 'bin/data/myKit_01.json'
	// must call setup after adding all ofParameterGroup's
	std::string name = "myKit_01";
	presetsManager.setup(name);
}

//--------------------------------------------------------------
void ofApp::setupParameters()
{
	//--

	// some useful OF tips with parameters

	// 1. we can exclude some parameters that we don't want to handle into presets
	// ie: exclude all colors
	color0.setSerializable(false);
	color1.setSerializable(false);
	color2.setSerializable(false);
	color3.setSerializable(false);

	// 2. we can link some parameters to make them change together
	// must call makeRefereceTo before params are setted below
	// note that setted names will be overrided
	//size2.makeReferenceTo(size3);

	//--

	// 0. ofParameterGroup's

	// group0
	params0.setName("paramsGroup0");// this is our parent group
	params0.add(show0.set("show0", true));
	params0.add(color0.set("color0", ofColor(255, 0, 0, _alpha), ofColor(0, 0), ofColor(255, 255)));
	params0.add(numObjects0.set("numObjects0", 2, 1, 3));
	params0.add(shapeType0.set("shapeType0", 1, 1, 2));
	params0.add(separation0.set("separation0", 100, 1, 100));
	params0.add(shapeSide0.set("shapeSide0", 50, 5, 200));
	params0_Nested.setName("styleGroup");// we can use nested groups as ofParameterGroup
	params0_Nested.add(fill0.set("fill0", true));
	params0_Nested.add(lineWidth0.set("lineWidth0", 1, 0.1, 10));
	params0.add(params0_Nested);// add a group to the parent group

	// group1
	params1.setName("paramsGroup1");
	params1.add(show1.set("show1", true));
	params1.add(color1.set("color1", ofColor(0, 255, 0, _alpha), ofColor(0, 0), ofColor(255, 255)));
	params1.add(numObjects1.set("numObjects1", 2, 1, 3));
	params1.add(shapeSide1.set("shapeSide1", 50, 5, 200));
	params1.add(separation1.set("separation1", 100, 1, 100));

	// group2
	params2.setName("paramsGroup2");
	params2.add(show2.set("show2", true));
	params2.add(color2.set("color2", ofColor(0, 0, 255, _alpha), ofColor(0, 0), ofColor(255, 255)));
	params2.add(numObjects2.set("numObjects2", 2, 1, 3));
	params2.add(size2.set("size2", 50, 5, 200));
	params2.add(fill2.set("fill2", true));

	// group3
	params3.setName("paramsGroup3");
	params3.add(show3.set("show3", true));
	params3.add(color3.set("color3", ofColor(255, 255, 0, _alpha), ofColor(0, 0), ofColor(255, 255)));
	params3.add(numObjects3.set("numObjects3", 2, 1, 3));
	params3.add(size3.set("size3", 5, 1, 100));
	params3.add(separation3.set("separation3", 50, 5, 100));

	//--

#ifdef USE_GUI_LOCAL 
	// 1. local guis
	// a local ofApp gui to show/edit our settings/parameters
	// we will see here show how our params/settings changes when using the presets manager
	ofxSurfingHelpers::setThemeDark_ofxGui();

	int x = 5;
	int y = 10;
	int w = 200 + 3;

	gui0.setup("ofApp0");
	gui0.add(params0);
	gui0.setPosition(x, y);

	gui1.setup("ofApp1");
	gui1.add(params1);
	gui1.setPosition(x + 1 * w, y);

	gui2.setup("ofApp2");
	gui2.add(params2);
	gui2.setPosition(x + 2 * w, y);

	gui3.setup("ofApp3");
	gui3.add(params3);
	gui3.setPosition(x + 3 * w, y);
#endif
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(8);

	// scene draw object linked to grouped parameters
	if (show0) drawScene0();
	if (show1) drawScene1();
	if (show2) drawScene2();
	if (show3) drawScene3();

#ifdef USE_GUI_LOCAL 
	// local gui parameters: to show or to edit presets params
	if (bGui) {
		gui0.draw();
		gui1.draw();
		gui2.draw();
		gui3.draw();
	}
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') {
		bGui = !bGui;
		presetsManager.setVisible_GUI(bGui);
	}
	if (key == 'h') {
		presetsManager.setToggleVisible_Help();
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
	presetsManager.exit();// required to store settings. (maybe required before destruct our class params..)
}

//--

// scene

//--------------------------------------------------------------
void ofApp::drawScene0()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(200, 200);
	ofSetColor(color0.get());
	ofSetLineWidth(lineWidth0);
	if (fill0) ofFill();
	else ofNoFill();
	for (int i = 0; i < numObjects0+1; ++i)
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
	ofTranslate(600, 400 - shapeSide1 / 10);
	ofSetColor(color1.get());
	ofFill();
	for (int i = 0; i < numObjects1; ++i)
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
	ofTranslate(600, 500);
	ofSetColor(color2.get());
	if (fill2) ofFill();
	else ofNoFill();
	for (int i = 0; i < numObjects2+1; ++i)
	{
		ofDrawCircle(0, 0, (size2 + size2 / i) / 4);
		ofTranslate(size2 * i * 1.5, 0);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScene3()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(700, 600);
	ofFill();
	ofSetColor(color3.get());
	for (int i = 0; i < numObjects3; ++i)
	{
		ofDrawRectangle(0, 0, size3 * i / 3, size3 * i / 3);
		float _tr = i * (separation3 + 20);
		ofTranslate(_tr, 0);
	}
	ofPopMatrix();
	ofPopStyle();
}