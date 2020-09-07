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

	//--

	//1. (optional) customize path folders at first
	//presetsManager.setPath_UserKit_Folder("presetsManager");	//main container folder where all other files will be

	//2. add our ofParameterGroup to the preset manager 
	//also define wich key triggers are associated to each preset. 
	//the amount of keys will be also the amount of favorites/clickable presets
	presetsManager.add(params, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

	//3. call setup after adding ofParameterGroup
	presetsManager.setup();

	//4. (optional) customize gui positions
	presetsManager.setVisible_PresetClicker(true);//user panel clicker
	presetsManager.setSizeBox_PresetClicker(80);
	presetsManager.setPosition_PresetClicker(200, ofGetHeight() - 100);

	//--

	//custom callback (B)
	listener_PresetManager_DoneLoad = presetsManager.DONE_load.newListener([this](bool &)
		{
			this->Changed_PresetManager_DoneLoad();
		});

	//--

#ifdef MODE_ImGui_EXTERNAL
	presetsManager.ImGui_FontCustom();
	gui_ImGui.setup();
	presetsManager.ImGui_ThemeMoebiusSurfing();
#endif

	//-------

	//local ofApp gui to show/edit our settings/parameters
	//to show how our params/settings changes when using the presets manager
	gui.setup("ofApp");
	gui.add(params);
	gui.setPosition(50, 300);
}

//--------------------------------------------------------------
void ofApp::update()
{
	//presetsManager.update();

	////easy callback (A)
	//if (presetsManager.isDoneLoad())//just loaded preset it's done
	//{
	//	cout << (__FUNCTION__) << "--------------------------------------------------------------[ EasyCallback -> DoneLoad! ]" << endl;
	//	ofLogWarning(__FUNCTION__) << "--------------------------------------------------------------[ EasyCallback -> DoneLoad! ]";
	//}
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
	str = "";
	str += "shapeType : " + ofToString(shapeType);
	str += "numShapes : " + ofToString(numShapes);
	str += "separation: " + ofToString(separation);
	str += "shapeSide : " + ofToString(shapeSide);
	str += "fill      : " + ofToString(fill);
	str += "color     : " + ofToString(color);
	str += "lineWidth : " + ofToString(lineWidth);

	//-

	//scene draw object linked to grouped parameters
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

	//-

#ifdef MODE_ImGui_EXTERNAL
	drawImGui();
#endif

	//-

	//local gui parameters
	gui.draw();

	//--
}

#ifdef MODE_ImGui_EXTERNAL
//--------------------------------------------------------------
void ofApp::drawImGui()
{
	gui_ImGui.begin();

	//ImGui::ShowDemoWindow();

	if (presetsManager.SHOW_ImGui)
	{
		ofVec2f pos(500, 10);
		auto mainSettings = ofxImGui::Settings();
		mainSettings.windowPos = pos;
		ImGui::SetNextWindowPos(ofVec2f(pos.x, pos.y));

		string _name = "ofxPresetsManager";
		bool _collapse = true;
		if (ofxImGui::BeginWindow(_name, mainSettings, _collapse))
		{
			presetsManager.ImGui_Draw_WindowContent(mainSettings);
		}
		ofxImGui::EndWindow(mainSettings);

		bMouseOverGui = mainSettings.mouseOverGui;
	}

	gui_ImGui.end();
}
#endif

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
}

//--------------------------------------------------------------
void ofApp::exit()
{
	presetsManager.exit();//required to store settings. (maybe required before destruct our class params..)
}