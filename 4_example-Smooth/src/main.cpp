#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1920, 1080, OF_WINDOW);
	//TIME_SAMPLE_SET_FRAMERATE(60);
	ofRunApp(new ofApp());

}
