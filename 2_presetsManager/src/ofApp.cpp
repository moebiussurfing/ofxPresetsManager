#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//-

#ifdef USE_WindowApp
	WindowApp.setFrameRate(25);
	WindowApp.setSetVerticalSync(false);
	//WindowApp.setDEBUG_Position(0);//top position
#else
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
#endif

	//--

	//local group parameters / preset

	//group1
	params.setName("myGroupParameters");
	params.add(fill.set("fill", false));
	params.add(color.set("color", ofColor(1), ofColor(0, 0), ofColor(1, 1)));
	params.add(lineWidth.set("lineWidth", 1, 0.1, 10));
	params.add(shapeType.set("shape", 1, 1, 2));
	params.add(numShapes.set("num squares", 1, 1, 24));
	params.add(separation.set("separation", 5, 1, 100));
	params.add(shapeSide.set("square side", 50, 5, 200));

	//-

	////group2
	//params2.setName("circlesParams");
	//params2.add(numCircles.set("num circles", 1, 1, 24));
	//params2.add(circleSeparation.set("circle separation", 5, 1, 100));
	//params2.add(circleSide.set("circles side", 50, 5, 200));

	//---

	//preset presetsManager

	//customize path folders

	//NOTE: 
	//take care with your /data path folders, 
	//they must be created before we can write inside!
	//also file settings must (maybe) be located there to avoid startup crashes!

	presetsManager.setPath_GlobalFolder("ofxPresetsManager");//main container folder
	presetsManager.setPath_KitFolder("presets");//from kit/livePanel presets
	presetsManager.setPath_PresetsFolder("archive");//for browser file presets

	//-

	//add (target) parameters group: 
	//this is "the preset" container itself
	//params group and trigger keys associated too.
	presetsManager.add(params, { '1', '2', '3', '4', '5', '6', '7', '8' });

	//-

	//TODO:
	//multiple groups
	//presetsManager.add(params2, { 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',' });

	//--

	//apply setup
	presetsManager.setup();

	//--

	////setup callbacks if required
	////alternative callbacks to trig when save/load is done
	//presetsManager.DONE_save.addListener(this, &ofApp::Changed_DONE_save);
	//presetsManager.DONE_load.addListener(this, &ofApp::Changed_DONE_load);

	//--

	//customize gui positions

	//user clicker
	presetsManager.setVisible_PresetClicker(true);

	//ofxGui show/hide internal control gui
	//this panel have mainly all the addon controls. not all are important to the user.
	//presetsManager.set_GUI_Internal_Visible(true);

	//TODO:
	//browser
	//presetsManager.set_GUI_PositionBrowser(10, 30);
	//presetsManager.set_GUI_SizeBrowser(250, 300);

	//--

	//local ofApp gui 
	//show how we receive presets into the target params group:
	gui.setup("ofApp");
	gui.add(params);

	//TODO:
	//multiple params groups
	//gui.add(params2);

	gui.setPosition(20, 150);

	//-------

	//startup

	//workaround
	//to solve some auto load fails... 
	//because the sorting of xml autoSave after preset selector tag(?)
	presetsManager.refresh();

	//-------
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
void ofApp::windowResized(int w, int h)
{
	int cellw = 50;
	presetsManager.setPosition_PresetClicker(0.5* ofGetWidth() - (9 * cellw*0.5f), ofGetHeight() - 200, cellw);//position and boxes sizes
	//presetsManager.windowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::exit()
{
	presetsManager.exit();

	WindowApp.exit();

	//presetsManager.DONE_save.removeListener(this, &ofApp::Changed_DONE_save);
	//presetsManager.DONE_load.removeListener(this, &ofApp::Changed_DONE_load);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(ofColor::darkBlue);

	//-

	//debug object linked to grouped parameters
	string str;
	int x = 10;
	int y = 20;
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
	//ignore local ofApp keys when mouse is over ImGui window (to textInput)
	if (!presetsManager.isMouseOver())
	{
		//-

		//select one preset by random (1 to 8)
		if (key == OF_KEY_RETURN)
		{
			int pMax = presetsManager.getNumPresets();//get amount of presets on kit
			int pMin = 1;
			int p = (int)ofRandom(pMin, pMax + 1);//p goes from 1 to (max) 8

			ofLogNotice("ofApp") << "select a random preset from the manager kit: " << ofToString(p);
			presetsManager.loadPreset(p);
		}

		//-

		//switch keys control enabled
		if (key == OF_KEY_TAB)
		{
			presetsManager.setToggleEnableKeys();
			//presetsManager.set_ENABLE_Keys(!presetsManager.isKeysEnabled());
		}

		//-

		//randomize (toggle) enabled parameters of current preset 
		if (key == OF_KEY_BACKSPACE)
		{
			presetsManager.randomizePreset();
		}

		//TODO:
		//load preset by code
		//if (key == 'a')
		//{
		//	presetsManager.loadPreset(1);
		//}
		//if (key == 's')
		//{
		//	presetsManager.loadPreset(2);
		//}

		//----

		////randomize parameters of local params (selected preset too if autoSave is enabled)
		//if (key == ' ')
		//{
		//	shapeType = ofRandom(shapeType.getMin(), shapeType.getMax() + 1);
		//	numShapes = ofRandom(numShapes.getMin(), numShapes.getMax() + 1);
		//	separation = ofRandom(separation.getMin(), separation.getMax() + 1);
		//	shapeSide = ofRandom(shapeSide.getMin(), shapeSide.getMax() + 1);
		//}

	}
}

//-

////alternative callback
////--------------------------------------------------------------
//void ofApp::Changed_DONE_save(bool &DONE_save)
//{
//	ofLogNotice("ofApp") << "Changed_DONE_save: " << ofToString(DONE_save ? "TRUE" : "FALSE");
//	ofLogNotice("ofApp") << "--------------------------------------------------------------[DONE_save]";
//
//	//extra stuff when finish loading/saving
//	if (presetsManager.DONE_save)
//	{
//		presetsManager.DONE_save = false;
//	}
//}
//
////--------------------------------------------------------------
//void ofApp::Changed_DONE_load(bool &DONE_load)
//{
//	ofLogNotice("ofApp") << "Changed_DONE_load: " << ofToString(DONE_load ? "TRUE" : "FALSE");
//	ofLogNotice("ofApp") << "--------------------------------------------------------------[DONE_load]";
//
//	//extra stuff when finish loading/saving
//	if (presetsManager.DONE_load)
//	{
//		presetsManager.DONE_load = false;
//	}
//}
