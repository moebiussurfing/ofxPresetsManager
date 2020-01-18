#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate(60);

	//-

	////Gui
	//guiApp.setup();
	//guiVisible = true;

	//--

	//local group parameters / preset
	
	//group1
	params.setName("myGroupParameters");
	params.add(numSquares.set("num squares", 1, 1, 24));
	params.add(separation.set("separation", 5, 1, 100));
	params.add(squareSide.set("square side", 50, 5, 200));

	////group2
	//params2.setName("circlesParams");
	//params2.add(numCircles.set("num circles", 1, 1, 24));
	//params2.add(circleSeparation.set("circle separation", 5, 1, 100));
	//params2.add(circleSide.set("circles side", 50, 5, 200));
	
	//---

	//preset manager

	//some settings
	//manager.setAutoLoad(true);
	//manager.setAutoSave(true);

	//customize path folders
	//NOTE: take care with path folders, they must exist before we can write inside!
	//manager.set_GlobalFolder("ofxPresetsManager/");//TODO:
	//manager.set_pathKit_Folder(pathKit);

	//add target group. this is "the preset" container itself
	//manager.add(params, { '1', '2', '3', '4', '5', '6', '7', '8' });
	//manager.add(params, { 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',' });

	manager.set_pathKit_Folder("assets/groups/kit");
	
	manager.add(params, { '1', '2', '3', '4', '5', '6', '7', '8' });
	//manager.add(params2, { 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',' });

	//--

	//apply setup
	manager.setup();

	//callbacks to trig when save/load is done
	manager.DONE_save.addListener(this, &ofApp::Changed_DONE_save);
	manager.DONE_load.addListener(this, &ofApp::Changed_DONE_load);

   //trick to solve auto load fail because the sorting of xml autoSave after preset selector tag
	manager.refresh();

	//--

	//user clicker
	manager.set_CLICKER_Position(400, ofGetHeight() - 200, 50);//position and boxes sizes
	manager.set_CLICKER_Visible(true);

	//imGui
	manager.set_GUI_Visible(true);
	manager.set_GUI_Position(10, 30);
	manager.set_GUI_Size(250, 300);

	//--

	//local gui (to debug params too)
	gui.setup("ofApp::gui");
	gui.add(params);
	//gui.add(params2);
	gui.setPosition(20, 100);
}

//--------------------------------------------------------------
void ofApp::update()
{
	string str;
	str = ofToString((int)ofGetFrameRate()) + " FPS";
	ofSetWindowTitle(str);

	//-

	//preset manager
	manager.update();

	//simple callback when preset is loaded 
	if (manager.isDoneLoad())
	{
		ofLogNotice("ofApp") << "+ MANAGER DONE LOAD";
		ofLogNotice("ofApp") << endl;
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
	manager.DONE_save.removeListener(this, &ofApp::Changed_DONE_save);
	manager.DONE_load.removeListener(this, &ofApp::Changed_DONE_load);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(ofColor::darkBlue);

	//-

	//browser
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
	int y = 20;
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

	//addon gui
	manager.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	//randomize
	if (key == ' ')
	{
		numSquares = ofRandom(numSquares.getMin(), numSquares.getMax());
		separation = ofRandom(separation.getMin(), separation.getMax());
		squareSide = ofRandom(squareSide.getMin(), squareSide.getMax());
	}
	//switch keys control enabled
	else if (key == OF_KEY_TAB)
	{
		manager.set_ENABLE_Keys(!manager.isKeysEnabled());
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

//callback
//--------------------------------------------------------------
void ofApp::Changed_DONE_save(bool &DONE_save)
{
	ofLogNotice("ofApp") << "Changed_DONE_save: " << ofToString(DONE_save ? "TRUE" : "FALSE");
	ofLogNotice("ofApp") << endl;

	//extra stuff when finish loading/saving
	if (manager.DONE_save)
	{
		manager.DONE_save = false;
	}
}

//--------------------------------------------------------------
void ofApp::Changed_DONE_load(bool &DONE_load)
{
	ofLogNotice("ofApp") << "Changed_DONE_load: " << ofToString(DONE_load ? "TRUE" : "FALSE");
	ofLogNotice("ofApp") << endl;

	//extra stuff when finish loading/saving
	if (manager.DONE_load)
	{
		manager.DONE_load = false;
	}
}

//browser
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
