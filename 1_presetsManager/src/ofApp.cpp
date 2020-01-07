#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(30);
    //ofSetWindowTitle("PRESET MANAGER");

    //-

    //// Gui
    //
    //guiApp.setup();
    //guiVisible = true;

    //--

#ifdef USE_OF_PARAMETER_GROUP

    // define and group parameters
    paramsSub.setName("sub");
    paramsSub.add(numSquaresSub.set("num squaresSub", 1, 1, 24));
    paramsSub.add(separationSub.set("separationSub", 5, 1, 100));
    paramsSub.add(squareSideSub.set("square sideSub", 50, 5, 200));
    params.setName("myGroupParameters");
    params.add(numSquares.set("num squares", 1, 1, 24));
    params.add(separation.set("separation", 5, 1, 100));
    params.add(squareSide.set("square side", 50, 5, 200));
    params2.setName("params2");
    params2.add(myBool.set("BOOL", true));
    params2.add(myFloat.set("FLOAT1", 5, 1, 100));
    params2.add(myFloat2.set("FLOAT2", 50, 5, 200));
    params.add(paramsSub);
    params2.add(params);

    paramsFull.setName("myNested_group");
    paramsFull.add(paramsSub);
    paramsFull.add(params);
    //paramsFull.add(paramsSub);
    //paramsFull.add(params);
    //paramsFull.add(myFloat2);
    paramsFull.add(params2);

#endif

    //-

    // A. ofParameterGroup params
#ifdef USE_OF_PARAMETER_GROUP

    manager.set_pathKit_Folder(pathKit);
    manager.add(paramsFull, {'1', '2', '3', '4', '5', '6', '7', '8'});

#endif

    //-

    // B. custom DATA
#ifdef USE_CUSTOM_DATAGRID

    manager.set_pathKit_Folder("assets/patterns/kit_1");
    myDataGrid.setName("stepSequencer");
    manager.add( myDataGrid, { '1', '2', '3', '4', '5', '6', '7', '8'} );

#endif

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
    manager.set_CLICKER_Visible(false);

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
    //-

    // TODO: easy listener temp solution to trig when save/load is done
    // then will load refresh grid sequencer states

    manager.DONE_save.removeListener(this, &ofApp::Changed_DONE_save);
    manager.DONE_load.removeListener(this, &ofApp::Changed_DONE_load);

    //-
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(128);

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

        str = "numSquaresSub: " + ofToString(numSquaresSub);
        ofDrawBitmapStringHighlight(str, x, y + pad * i++);

        str = "separationSub: " + ofToString(separationSub);
        ofDrawBitmapStringHighlight(str, x, y + pad * i++);

        str = "squareSideSub: " + ofToString(squareSideSub);
        ofDrawBitmapStringHighlight(str, x, y + pad * i++);

        str = "myBool: " + ofToString(myBool);
        ofDrawBitmapStringHighlight(str, x, y + pad * i++);

        str = "myFloat: " + ofToString(myFloat);
        ofDrawBitmapStringHighlight(str, x, y + pad * i++);

        str = "myFloat2: " + ofToString(myFloat2);
        ofDrawBitmapStringHighlight(str, x, y + pad * i++);
    }

    //-

    // SCENE DRAW OBJECT LINKED TO GROUPED PARAMETERS

#ifdef USE_OF_PARAMETER_GROUP

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

#endif

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

        // GRID_getFrom_Sequencer();// get sequencer state before saving in preset manager
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

        //-

        //        // 1. put local grid to sequencer grid:
        //
        ////        ofLogNotice("ofxSEQ") << "ofxSequencer size rows:" << sequencer.grid.size();
        //        for(int i=0 ; i < sequencer.grid.size() ; i++)
        //        {
        ////            ofLogNotice("ofxSEQ") << "ofxSequencer size cols:" << sequencer.grid[i].size() ;
        //            for(int j=0 ; j < sequencer.grid[i].size() ; j++)
        //            {
        //                sequencer.grid[i][j] = (bool) myDataGrid.grid[i][j];
        //            }
        //        }
        //
        //        //-
        //
        //        // 2. refresh from sequencer grid to draw params
        //
        //        sequencer.set_SequencerFromGrid();

        //-
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
