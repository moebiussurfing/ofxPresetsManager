#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//WindowApp.setDEBUG_Position(0);//top position
	//WindowApp.setSettingsFps(60);
	//WindowApp.setSettingsVsync(true);

	//--

	//local group parameters / preset

	//group1
	params.setName("myGroupParameters");
	params.add(numSquares.set("num squares", 1, 1, 24));
	params.add(separation.set("separation", 5, 1, 100));
	params.add(squareSide.set("square side", 50, 5, 200));

	//-

	////group2
	//params2.setName("circlesParams");
	//params2.add(numCircles.set("num circles", 1, 1, 24));
	//params2.add(circleSeparation.set("circle separation", 5, 1, 100));
	//params2.add(circleSide.set("circles side", 50, 5, 200));

	//---

	//preset presetsManager

	//customize path folders

	//NOTE: take care with path folders, 
	//they must exist before we can write inside!
	//or maybe file settings must be located there to avoid startup crashes!

	presetsManager.set_Path_GlobalFolder("ofxPresetsManager");//main container folder
	presetsManager.set_Path_KitFolder("presets");//kit presets

	//-

	//add target group. this is "the preset" container itself
	//params group and trigger keys
	presetsManager.add(params, { '1', '2', '3', '4', '5', '6', '7', '8' });

	//-

	//TODO:
	//multiple groups
	//presetsManager.add(params2, { 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',' });

	//--

	//apply setup
	presetsManager.setup();

	////callbacks to trig when save/load is done
	//presetsManager.DONE_save.addListener(this, &ofApp::Changed_DONE_save);
	//presetsManager.DONE_load.addListener(this, &ofApp::Changed_DONE_load);

	//--

	//customize gui positions

	//user clicker
	presetsManager.set_CLICKER_Visible(true);
	presetsManager.set_CLICKER_Position(400, ofGetHeight() - 200, 50);//position and boxes sizes

	//ofxGui
	presetsManager.set_GUI_Internal_Visible(true);
	presetsManager.set_GUI_Position(10, 30);
	presetsManager.set_GUI_Size(250, 300);

	//--

	//local gui (to debug params too)
	gui.setup("ofApp");
	gui.add(params);

	//TODO:multiple params groups
	//gui.add(params2);

	gui.setPosition(20, 400);

	//--

	//TODO:
	////browser gui
	//guiApp.setup();
	//guiVisible = true;

	//----

	//startup

	//trick to solve auto load fail because the sorting of xml autoSave after preset selector tag
	presetsManager.refresh();

	//----
}

//--------------------------------------------------------------
void ofApp::update()
{
	//presetsManager
	//simple callback when preset is loaded 
	if (presetsManager.isDoneLoad())
	{
		//ofLogNotice("ofApp") << "DONE PRESET LOADED";
		ofLogNotice("ofApp") << "--------------------------------------------------------------";
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
	//presetsManager.DONE_save.removeListener(this, &ofApp::Changed_DONE_save);
	//presetsManager.DONE_load.removeListener(this, &ofApp::Changed_DONE_load);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(ofColor::darkBlue);

	//-

	//explorer browser

	//Gui
	//mouseOverGui = false;
	//if (guiVisible)
	//{
	//   mouseOverGui = imGui();
	//}
	//if (mouseOverGui)
	//{
	//
	//}
	//else
	//{
	//
	//}

	//-

	//debug object linked to grouped parameters
	string str;
	int x = 20;
	int y = 100;
	int pad = 20;
	int i = 0;
	{
		str = "numSquares: " + ofToString(numSquares);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
		str = "separation: " + ofToString(separation);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
		str = "squareSide: " + ofToString(squareSide);
		ofDrawBitmapStringHighlight(str, x, y + pad * i++);
	}

	//-

	//scene draw object linked to grouped parameters
	ofPushStyle();
	ofSetColor(ofColor::white);
	ofNoFill();
	ofPushMatrix();
	ofTranslate(300, 50);
	for (int i = 0; i < numSquares; ++i)
	{
		ofDrawRectangle(0, 0, squareSide, squareSide);
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
	//randomize selected preset
	if (key == OF_KEY_RETURN)
	{
		int pMax = presetsManager.getNumPresets();//get amount of presets on kit
		int pMin = 1;
		int p = (int)ofRandom(pMin, pMax + 1);//p goes from 1 to (max) 8
		ofLogNotice("ofApp") << "random: " << ofToString(p);
		presetsManager.loadPreset(p);
	}

	//randomize parameters
	if (key == ' ')
	{
		numSquares = ofRandom(numSquares.getMin(), numSquares.getMax());
		separation = ofRandom(separation.getMin(), separation.getMax());
		squareSide = ofRandom(squareSide.getMin(), squareSide.getMax());
	}

	//switch keys control enabled
	if (key == OF_KEY_TAB)
	{
		presetsManager.set_ENABLE_Keys(!presetsManager.isKeysEnabled());
	}
}

//explorer browser
////--------------------------------------------------------------
//bool ofApp::imGui()
//{
//   static bool Show_Preset_settings = true;
//   auto mainSettings = ofxImGui::Settings();
//   int x, y, w, h, pad;
//   pad = 5;
//   w = 200;
//   h = 400;
//   x = ofGetWidth() - 2 * w - pad;
//   y = 30;
//   mainSettings.windowPos = ofVec2f(x, y);
//   mainSettings.windowSize = ofVec2f(w, h);
//
//   this->guiApp.begin();
//   {
//       if (Show_Preset_settings)
//       {
//           if (ofxImGui::BeginWindow("Preset", mainSettings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
//           {
//               ofxImGui::AddGroup(this->paramsFull, mainSettings);
//
//               //-
//
//               //NOT REQUIRED...
//               //               //NESTED GROUPS
//               ////WORKING
//               //               for (auto parameter : paramsFull)
//               //               {
//               //                   //Group.
//               //                   auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
//               //                   if (parameterGroup)
//               //                   {
//               //                       //Recurse through contents.
//               //                       ofxImGui::AddGroup(*parameterGroup, mainSettings);
//               //                       continue;
//               //                   }
//               //               }
//
//               ofxImGui::EndWindow(mainSettings);
//           }
//       }
//   }
//
//   this->guiApp.end();
//   return mainSettings.mouseOverGui;
//}

////callback
////TODO: improve adding isChanged method
////--------------------------------------------------------------
//void ofApp::Changed_DONE_save(bool &DONE_save)
//{
//	ofLogNotice("ofApp") << "Changed_DONE_save: " << ofToString(DONE_save ? "TRUE" : "FALSE");
//	ofLogNotice("ofApp") << "";
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
//	ofLogNotice("ofApp") << "";
//
//	//extra stuff when finish loading/saving
//	if (presetsManager.DONE_load)
//	{
//		presetsManager.DONE_load = false;
//	}
//}
