#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//-

#ifdef USE_WindowApp
	WindowApp.setFrameRate(60);
	WindowApp.setVerticalSync(true);
#else
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
#endif

	//--

	//0. our app parameters
	params.setName("mySettings");//main (target) group
	params.add(shapeType.set("shape", 1, 1, 2));
	params.add(numShapes.set("num squares", 1, 1, 24));
	params.add(separation.set("separation", 5, 1, 100));
	params.add(shapeSide.set("square side", 50, 5, 200));
	paramsNested.setName("style");//another nested group
	paramsNested.add(fill.set("fill", false));
	paramsNested.add(color.set("color", ofColor(1), ofColor(0, 0), ofColor(1, 1)));
	paramsNested.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	params.add(paramsNested);//main preset settings container

	//--

	//1. (optional) customize path folders at first
	presetsManager.setPath_GlobalFolder("presetsManager");	//main container folder where all other files will be

	//2. add our ofParameterGroup to the preset manager 
	//also define wich key triggers are associated to each preset. 
	//the amount of keys will be also the amount of favorites/clickable presets
	presetsManager.add(params, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

	//3. call setup after adding ofParameterGroup
	presetsManager.setup();

	//4. (optional) customize gui positions
	presetsManager.setVisible_PresetClicker(true);//user panel clicker
	presetsManager.setPosition_PresetClicker(200, 100, 50);

	//-------

	//local ofApp gui to show/edit our settings/parameters
	//to show how our params/settings changes when using the presets manager
	gui.setup("ofApp");
	gui.add(params);
	gui.setPosition(50, 500);
}

//--------------------------------------------------------------
void ofApp::update()
{
	//easy simple callback
	if (presetsManager.isDoneLoad())//just loaded preset it's done
	{
		ofLogNotice("ofApp") << "--------------------------------------------------------------[isDoneLoad]";
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(ofColor::darkBlue);

	//-

	//debug object linked to grouped parameters
	string str;
	
	int x = gui.getPosition().x + 15;
	int y = gui.getPosition().y + gui.getHeight() + 30;
	int pad = 20;
	int i = 0;
	{
		str = "fill      : " + ofToString(fill);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
		str = "lineWidth : " + ofToString(lineWidth);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
		//str = "color     : " + ofToString(color);
		//ofDrawBitmapStringHighlight(str, x, y + pad * i++);
		str = "shapeType : " + ofToString(shapeType);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
		str = "numShapes : " + ofToString(numShapes);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
		str = "separation: " + ofToString(separation);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
		str = "shapeSide : " + ofToString(shapeSide);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
	}

	//-

	//scene draw object linked to grouped parameters
	ofPushStyle();
	ofSetColor(color.get());
	ofSetLineWidth(lineWidth);
	if (fill) ofFill();
	else ofNoFill();
	ofPushMatrix();
	ofTranslate(300, 50);

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

	//-

	//local gui parameters
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	////some of this keys are already controlled by the addon itself. check inside sources.
	////we can ignore local ofApp keys when mouse is over ImGui window (to textInput) to avoid collide
	//if (!presetsManager.isMouseOver())
	//{
	//	//-

	//	if (key == OF_KEY_RETURN)
	//	{
	//		presetsManager.doRandomizeBang();//set a random preset from favorites
	//	}

	//	//-

	//	if (key == OF_KEY_BACKSPACE)
	//	{
	//		presetsManager.doRandomizePreset();//randomize params of current selected preset
	//	}

	//	//-

	//	//switch keys control enabled
	//	if (key == OF_KEY_TAB)
	//	{
	//		presetsManager.setToggleEnableKeys();
	//	}

	//	//-

	//	//load preset by code
	//	//if (key == 'a')
	//	//{
	//	//	presetsManager.loadPreset(1);
	//	//}
	//	//if (key == 's')
	//	//{
	//	//	presetsManager.loadPreset(2);
	//	//}
	//}
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