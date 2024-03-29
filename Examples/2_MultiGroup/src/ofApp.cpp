#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate(60);
	ofSetWindowPosition(-1920, 25);

	//--

	// Setup your parameters groups 
	// before add to the add-on!
	setupScene();

	//--

	// Presets Manager
	setupPresets();
}

//--------------------------------------------------------------
void ofApp::setupPresets()
{
	// Some Optional API extra tweak controls:
	{
		// 1. Optional before adding groups.

		// 1.1 Disable browsing using arrow keys 
		// to avoid collide with other add-ons
		//presetsManager.setEnableKeysArrowBrowse(disable); 

		// 1.2 Disable auto creation of the GROUP_LINK selector.
		//presetsManager.setEnableGroupLinkSelector(false); 

		// 1.3 Set the amount of presets 
		// for the auto created GROUP_LINK selector (the last one).
		// By default will be created with 9 presets!
		presetsManager.setGroupLinkAmountPresets(3);
	}

	//--

	// 2. Add many independent and parallel groups.

	// Build your presets manager:
	// Add our ofParameterGroup to the preset manager. 
	// Also define which key triggers are associated to each preset. 
	// The amount of keys will be also the amount 
	// of favorites / clickable presets for each group.

	// Group 0
	presetsManager.add(params0, { 'q', 'w', 'e', 'r', 't' , 'y' , 'u' , 'i', 'o' }); // 9

	// Group 1
	presetsManager.add(params1, { 'a', 's', 'd', 'f' });

	// Group 2
	presetsManager.add(params2, { 'z', 'x', 'c' });

	// Group 3
	presetsManager.add(params3, { 'b', 'n', 'm', ',', '.' });

	//--

	// 3. Setup

	// Call after finished adding groups!
	presetsManager.setup(); 

	//--

	// 4. Optional

	//// Customize User-Kit name: 
	//// Then will create this named main settings file: 'bin/data/myKit_01.json'
	//// Must call setup after adding all ofParameterGroup's
	//std::string name = "myKit_01";
	//presetsManager.setup(name);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// Scene draw 
	{
		// Zoom
		ofPushMatrix();
		ofTranslate(-200, -200);
		ofScale(2.5f);

		// Linked to grouped parameters
		if (show0) drawScene0();
		if (show1) drawScene1();
		if (show2) drawScene2();
		if (show3) drawScene3();

		ofPopMatrix();
	}

	//----

	// Gui
	presetsManager.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') 
	{
		presetsManager.setToggleVisibleGui();
	}
}

//--

// Scene

//--------------------------------------------------------------
void ofApp::setupScene()
{
	ofSetCircleResolution(200);

	//--

	// Some useful oF tips with ofParameters:

	// 1. We can exclude some parameters that we don't want to handle into presets
	// e.g.: Exclude all colors
	color0.setSerializable(false);
	color1.setSerializable(false);
	color2.setSerializable(false);
	color3.setSerializable(false);

	//// 2. we can link some parameters to make them change together
	//// note that settled names will be override too
	//size2.makeReferenceTo(size3);//size3 will act as "master"

	//--

	// 0. ofParameterGroup's

	// Group 0
	params0.setName("Group0"); // this is our parent group
	params0.add(show0.set("show0", true));
	params0.add(color0.set("color0", ofColor(255, 0, 0, _alpha), ofColor(0, 0), ofColor(255, 255)));
	params0.add(numObjects0.set("numObjects0", 2, 1, 5));
	params0.add(shapeType0.set("shapeType0", 1, 1, 2));
	params0.add(separation0.set("separation0", 100, 1, 100));
	params0.add(shapeSide0.set("shapeSide0", 50, 5, 200));
	fill0.set("fill0", true);
	/*
	//TODO: BUG: on ofxSurfingImGui 
	// nested groups make fail some folding, 
	// and headers are disappearing...
	// we can use nested groups as ofParameterGroup
	params0_Nested.setName("SubGroup0"); 
	params0_Nested.add(fill0);
	params0_Nested.add(lineWidth0.set("lineWidth0", 1, 0.1, 10));
	params0.add(params0_Nested); // add a group to the parent group
	*/

	// Group 1
	params1.setName("Group1");
	params1.add(show1.set("show1", true));
	params1.add(color1.set("color1", ofColor(0, 255, 0, _alpha), ofColor(0, 0), ofColor(255, 255)));
	params1.add(numObjects1.set("numObjects1", 2, 1, 5));
	params1.add(shapeSide1.set("shapeSide1", 50, 5, 200));
	params1.add(separation1.set("separation1", 100, 1, 100));

	// Group 2
	params2.setName("Group2");
	params2.add(show2.set("show2", true));
	params2.add(color2.set("color2", ofColor(0, 0, 255, _alpha), ofColor(0, 0), ofColor(255, 255)));
	params2.add(numObjects2.set("numObjects2", 2, 1, 5));
	params2.add(size2.set("size2", 50, 5, 200));
	params2.add(fill2.set("fill2", true));

	// Group 3
	params3.setName("Group3");
	params3.add(show3.set("show3", true));
	params3.add(color3.set("color3", ofColor(255, 255, 0, _alpha), ofColor(0, 0), ofColor(255, 255)));
	params3.add(numObjects3.set("numObjects3", 2, 1, 5));
	params3.add(size3.set("size3", 5, 1, 100));
	params3.add(separation3.set("separation3", 50, 5, 100));
}

//--------------------------------------------------------------
void ofApp::drawScene0()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(200, 200);
	ofSetColor(color0.get());
	ofSetLineWidth(lineWidth0);
	if (fill0) ofFill();
	else ofNoFill();
	for (int i = 0; i < numObjects0 + 1; ++i)
	{
		if (shapeType0 == 1) ofDrawRectangle(0, 0, shapeSide0 * i , shapeSide0 * i );
		else if (shapeType0 == 2) ofDrawCircle(0, 0, shapeSide0 * i );
		ofTranslate(separation0, 0);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScene1()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(400, 300 - shapeSide1 / 10);
	ofSetColor(color1.get());
	ofFill();
	for (int i = 0; i < numObjects1; ++i)
	{
		ofDrawCircle(0, 0, shapeSide1);
		ofTranslate(separation1, 0);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScene2()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(400, 300);
	ofSetColor(color2.get());
	if (fill2) ofFill();
	else ofNoFill();
	for (int i = 0; i < numObjects2 + 1; ++i)
	{
		ofDrawCircle(0, 0, (size2 + size2 / i) / 2);
		ofTranslate(size2 * i * 0.25, 0);
	}
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScene3()
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(100, 200);
	ofFill();
	ofSetColor(color3.get());
	for (int i = 0; i < numObjects3; ++i)
	{
		ofDrawRectangle(0, i * 50, size3 * i, size3 * i);
		float _tr = i * (separation3 + 20);
		ofTranslate(_tr, 0);
	}
	ofPopMatrix();
	ofPopStyle();
}