#pragma once

#include "ofMain.h"

#include "ofxPresetsManager.h"
//#include "ofxImGui.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{

public:

	ofxPanel gui;

    //ofxImGui::Gui guiApp;
    //bool imGui();
    //bool guiVisible;
    //bool mouseOverGui;

    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    //--

    // PRESET MANAGER

    ofxPresetsManager manager;
    string pathKit = "assets/groups/kit_1";

    //--

    // group

    ofParameterGroup params;
    ofParameter<int> numSquares;
    ofParameter<int> separation;
    ofParameter<int> squareSide;
    ofParameterGroup paramsSub;
    ofParameter<int> numSquaresSub;
    ofParameter<int> separationSub;
    ofParameter<int> squareSideSub;
    ofParameterGroup params2;
    ofParameter<bool> myBool;
    ofParameter<float> myFloat;
    ofParameter<float> myFloat2;
    ofParameterGroup paramsFull;

	//-

    // TODO: easy listener temp solution..
    void Changed_DONE_load(bool &DONE_load);
    void Changed_DONE_save(bool &DONE_save);
};
