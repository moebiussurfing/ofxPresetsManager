#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(30);

    //-

    //// Gui
    //
    //guiApp.setup();
    //guiVisible = true;

    //--

    // define and group parameters
    params.setName("myGroupParameters");
    params.add(numSquares.set("num squares", 1, 1, 24));
    params.add(separation.set("separation", 5, 1, 100));
    params.add(squareSide.set("square side", 50, 5, 200));

	gui.setup("gui");
	gui.add(params);
	gui.setPosition(400, 50);

    manager.set_pathKit_Folder(pathKit);
    manager.add(params, {'1', '2', '3', '4', '5', '6', '7', '8'});
	
    // NOTE: take care with path folders, that must exist before write inside!

    //-

    // TODO: easy listener temp solution to trig when save/load is done
    // then will load refresh grid sequencer states

    manager.DONE_save.addListener(this, &ofApp::Changed_DONE_save);
    manager.DONE_load.addListener(this, &ofApp::Changed_DONE_load);

    //-

    // STARTUP INIT

//    //this loads selected preset number and gui state
//    manager.load_ControlSettings();

    //-

//    // trick to solve auto load fail because the sorting of xml autoSave after preset selector tag
//    manager.selected_PRE = -1;
//    manager.selected = manager.selected;
//    cout << "selected:" << manager.selected << endl;
    manager.refresh();
    //---

    // STARUP SETTINGS

    manager.set_GUI_Visible(true);
    manager.set_GUI_Position(10, 30);
    manager.set_GUI_Size(100, 300);
    manager.set_CLICKER_Position(10, ofGetHeight() - 120, 40);
    manager.set_CLICKER_Visible(true);

	manager.setAutoLoad(true);
	manager.setAutoSave(true);

    //--
}


//--------------------------------------------------------------
void ofApp::update()
{
    ofSetWindowTitle(ofToString((int) ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::exit()
{
    // TODO: easy listener temp solution to trig when save/load is done
    // then will load refresh grid sequencer states

    manager.DONE_save.removeListener(this, &ofApp::Changed_DONE_save);
    manager.DONE_load.removeListener(this, &ofApp::Changed_DONE_load);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(128);

	gui.draw();

    //-

    // Gui

    //mouseOverGui = false;
    //if (guiVisible)
    //{
    //    mouseOverGui = imGui();
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

    //TODO
    // DEBUG OBJECT LINKED TO GROUPED PARAMETERS

    string str;
    int x = 20;
    int y = 500;
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

    // SCENE DRAW OBJECT LINKED TO GROUPED PARAMETERS

    ofPushStyle();
    ofSetColor(ofColor::white);
    ofNoFill();
    ofPushMatrix();
    ofTranslate(120, 300);
    for (int i = 0; i < numSquares; ++i)
    {
        ofDrawRectangle(0, 0, squareSide, squareSide);
        ofTranslate(separation, separation);
    }
    ofPopMatrix();
    ofPopStyle();

    //-

    manager.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

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

// TODO: easy listener temp solution
//--------------------------------------------------------------
void ofApp::Changed_DONE_save(bool &DONE_save)
{
    ofLogNotice("ofApp") << "Changed_DONE_save: " << ofToString(DONE_save ? "TRUE" : "FALSE");

    // extra stuff when finish loading/saving
	if (manager.DONE_save)
    {
        manager.DONE_save = false;
	}
}

//--------------------------------------------------------------
void ofApp::Changed_DONE_load(bool &DONE_load)
{
    ofLogNotice("ofApp") << "Changed_DONE_load: " << ofToString(DONE_load ? "TRUE" : "FALSE");

    // extra stuff when finish loading/saving
	if (manager.DONE_load)
    {
        manager.DONE_load = false;
    }
}

////--------------------------------------------------------------
//bool ofApp::imGui()
//{
//    static bool Show_Preset_settings = true;
//    auto mainSettings = ofxImGui::Settings();
//    int x, y, w, h, pad;
//    pad = 5;
//    w = 200;
//    h = 400;
//    x = ofGetWidth() - 2 * w - pad;
//    y = 30;
//    mainSettings.windowPos = ofVec2f(x, y);
//    mainSettings.windowSize = ofVec2f(w, h);
//
//    this->guiApp.begin();
//    {
//        if (Show_Preset_settings)
//        {
//            if (ofxImGui::BeginWindow("Preset", mainSettings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
//            {
//                ofxImGui::AddGroup(this->paramsFull, mainSettings);
//
//                //-
//
//                //NOT REQUIRED...
//                //                // NESTED GROUPS
//                ////WORKING
//                //                for (auto parameter : paramsFull)
//                //                {
//                //                    // Group.
//                //                    auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
//                //                    if (parameterGroup)
//                //                    {
//                //                        // Recurse through contents.
//                //                        ofxImGui::AddGroup(*parameterGroup, mainSettings);
//                //                        continue;
//                //                    }
//                //                }
//
//                ofxImGui::EndWindow(mainSettings);
//            }
//        }
//    }
//
//    this->guiApp.end();
//    return mainSettings.mouseOverGui;
//}
