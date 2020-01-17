#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1440, 900, OF_WINDOW);
	TIME_SAMPLE_SET_FRAMERATE(60);
	ofRunApp(new ofApp());

}
