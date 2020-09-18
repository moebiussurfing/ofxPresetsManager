#include "ofxPresetsManager.h"


//--------------------------------------------------------------
ofxPresetsManager::ofxPresetsManager()
{
	// layout
	setSizeBox_PresetClicker(80);
	setPosition_PresetClicker(200, ofGetHeight() - getPresetClicker_Height() - 100);

	//-

#ifdef DEBUG_randomTest
	ofSetLogLevel("ofxPresetsManager", OF_LOG_VERBOSE);
#endif

	//-

#ifndef USE_JSON
	fileExtension = ".xml";
#endif

#ifdef USE_JSON
	fileExtension = ".json";
#endif

	//-

	DISABLE_CALLBACKS = true;

	//-

	//subscribed to auto run update and draw without required 'manual calls'
	ofAddListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofAddListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	//-

	//mainly to measure performance when using hd files vs faster memory vectors
#ifdef INCLUDE_PERFORMANCE_MEASURES
	//specify a target framerate
	//TIME_SAMPLE_SET_FRAMERATE(fps);
	//TIME_SAMPLE_ENABLE();
	TIME_SAMPLE_SET_AVERAGE_RATE(0.1);
	TIME_SAMPLE_SET_DRAW_LOCATION(TIME_SAMPLE_DRAW_LOC_BOTTOM_LEFT);
	TIME_SAMPLE_SET_PRECISION(4);
	TIME_SAMPLE_GET_INSTANCE()->setEnabled(true);
	TIME_SAMPLE_DISABLE();
#endif

	//--

	// default settings paths

	displayNameUserKit = "ofxPresetsManager";

	//-

	filenameMainSettings = "ofxPresetsManagerSettings" + fileExtension;// default user preset name

	// top parent folder
	// root

	// default User-Kit folder (main root container!)
	path_UserKit_Folder = "ofxPresetsManager";

	// default kit folder for live/favorites presets
	path_PresetsFavourites = "presets";
	// big browser
	path_PresetsStandalone = "archive";
	// app settings
	path_ControlSettings = "appSettings";

	// randomizer settings for each kit
	filename_Randomizers = "settingsRandomizer.xml";

	// global app session settings
	filename_ControlSettings = "settingsControl.xml";

	// to add to all presets filenames
	filenamesPrefix = "_";//"_preset_";

	//// root folder
	//path_Root = "myAddon";

	//--

	bKeys = false;
	keysNotActivated = true;
	lastMouseButtonState = false;
	modeKeySave = OF_KEY_CONTROL;
	bKeySave = false;
	modKeySwap = OF_KEY_ALT;
	bKeySwap = false;

	//--

	// this multidimension is for multiple gui/groups (feature not implemented!)
	groups.reserve(NUM_MAX_GROUPS);
	keys.reserve(NUM_MAX_GROUPS);
	PRESETS_Selected_Index.reserve(NUM_MAX_GROUPS);

	//--

	// callbacks to know when preset loading/save is done

	DONE_load.set("DONE LOAD", false);//callback to know (in ofApp) that preset LOAD is done
	DONE_save.set("DONE SAVE", false);//callback to know (in ofApp) that preset SAVE is done

	//easy callback: we can use too the easy isDoneLoad() to check in ofApp update() as kind of callback.

	//--

	// control parameters

	PRESET_Selected_IndexMain.set("PRESET", 0, 0, mainGroupAmtPresetsFav - 1);
	SHOW_ImGui.set("SHOW ImGui", false);
	SHOW_ImGui_PresetsParams.set("SHOW PARAMETERS", false);
	SHOW_ImGui_Selectors.set("SHOW SELECTORS", false);
	SHOW_Help.set("SHOW HELP", false);
	MODE_Browser_NewPreset.set("NEW!", false);
	displayNamePreset = "NO_NAME_PRESET";//browser loaded preset name
	bSave.set("SAVE", false);
	//bLoad.set("LOAD", false);
	MODE_MemoryLive.set("MODE MEMORY", false);
	loadToMemory.set("LOAD TO MEMORY", false);
	saveFromMemory.set("SAVE FROM MEMORY", false);
	autoLoad.set("AUTO LOAD", true);
	autoSave.set("AUTO SAVE", true);
	bAutosaveTimer.set("TIMER AUTO SAVE", false);
	SHOW_Gui_AdvancedControl.set("SHOW ADVANCED", false);
	SHOW_ClickPanel.set("SHOW CLICK PANEL", true);
	bCloneRight.set("CLONE >", false);
	bCloneAll.set("CLONE ALL", false);
	ENABLE_Keys.set("ENABLE KEYS", true);

	//-

	// layout
	Gui_Internal_Position.set("GUI INTERNAL POSITION",
		glm::vec2(ofGetWidth() * 0.5, ofGetHeight()* 0.5),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);

	ImGui_Position.set("GUI ImGui POSITION",
		glm::vec2(ofGetWidth() * 0.5, 10),//top centered
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);
	ImGui_Size.set("GUI ImGui SIZE",
		glm::vec2(300, 600),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);

	//-

	// exclude from xml settings
	bSave.setSerializable(false);
	bCloneRight.setSerializable(false);
	bCloneAll.setSerializable(false);
	loadToMemory.setSerializable(false);
	saveFromMemory.setSerializable(false);
	params_HelperTools.setSerializable(false);// only reset toggles inside
	//bLoad.setSerializable(false);
	//SHOW_Gui_AdvancedControl.setSerializable(false);

	//--

	// params groups

	params_Options.setName("OPTIONS");
	params_Options.add(MODE_MemoryLive);
	params_Options.add(loadToMemory);
	params_Options.add(saveFromMemory);
	params_Options.add(autoLoad);
	params_Options.add(autoSave);
	params_Options.add(bAutosaveTimer);

	params_Gui.setName("GUI");
	params_Gui.add(SHOW_Gui_AdvancedControl);
	params_Gui.add(SHOW_ClickPanel);
	params_Gui.add(SHOW_ImGui);
	params_Gui.add(SHOW_ImGui_PresetsParams);
	params_Gui.add(ENABLE_Keys);

	// layout
	params_Gui.add(ImGui_Position);
	params_Gui.add(ImGui_Size);
	params_Gui.add(Gui_Internal_Position);

	params_HelperTools.setName("HELPER TOOLS");
	params_HelperTools.add(bCloneRight);
	params_HelperTools.add(bCloneAll);

	//params_HelperTools.add(bRandomizeEditorPopulateFavs);
	//#ifdef INCLUDE_RANDOMIZER
	//	params_HelperTools.add(params_Randomizer);
	//	//TODO:
	//	//there's a problem here bc is dependent of the group params content..
	//	params_HelperTools.add(params_Editor_Toggles);
	//#endif

	//-

	// main selector
	//mainSelector.set("Main selector", 0, 0, 1);
	//params_PRESETS_Selected.add(mainSelector);

	//--

	//// user-kit
	//params_UserKitSettings.setName("USER-KIT");
	////params_UserKitSettings.add(params_PRESETS_Selected);//includes all selectors
	//params_UserKitSettings.add(PRESETS_Selected_Index[groups.size() - 1].get());//includes group selector only!
	////params_UserKitSettings.add(PRESET_Selected_IndexMain);

	//// custom path
	//params_Custom.setName("CUSTOM PATH");
	//params_Custom.add(bPathDirCustom);
	//params_Custom.add(pathDirCustom);
	//params_UserKitSettings.add(params_Custom);

	//ofAddListener(params_UserKitSettings.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_UserKit);

	//--

	// all nested params for callbacks and storage settings
	params_Control.setName("ofxPresetsManager");
	params_Control.add(bSave);
	//params_Control.add(bLoad);
	params_Control.add(params_HelperTools);

	ofAddListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);

	//--

	// gui font
	string str;
	str = "overpass-mono-bold.otf";
	myTTF = "assets/fonts/" + str;//assets folder
	sizeTTF = 10;
	bool bLoaded = myFont.load(myTTF, sizeTTF, true, true);
	if (!bLoaded) bLoaded = myFont.load(OF_TTF_SANS, sizeTTF, true, true);

	//--

	// custom path:
	bPathDirCustom.set("MODE CUSTOM PATH", false);
	pathDirCustom.set("Path", "DEFAULT_DATA_PATH");

	// randomizer settings
	params_RandomizerSettings.add(params_Randomizer);
	params_RandomizerSettings.add(params_Editor);
}

//----

// setup methods

//--------------------------------------------------------------
void ofxPresetsManager::setup()
{
	ofLogNotice(__FUNCTION__);
	//filenameMainSettings will use default name "ofxPresetsManagerSettings.xml"
	setup(bAllowGroupSelector);// split folders by group name
	//setup(true);// split folders by group name
}

//--------------------------------------------------------------
void ofxPresetsManager::setup(std::string name)/// must be called after adding params group
{
	ofLogNotice(__FUNCTION__) << name;
	filenameMainSettings = name + fileExtension;
	setup(bAllowGroupSelector);
	//setup(true);
}

//--------------------------------------------------------------
void ofxPresetsManager::setup(std::string name, bool _buildGroupSelector)/// must be called after adding params group
{
	ofLogNotice(__FUNCTION__) << name << " " << _buildGroupSelector;
	filenameMainSettings = name + fileExtension;
	setup(_buildGroupSelector);
}

//--------------------------------------------------------------
void ofxPresetsManager::setup(bool _buildGroupSelector)
{
	ofLogNotice(__FUNCTION__) << _buildGroupSelector;

	DISABLE_CALLBACKS = true;

	//--

	// create main GROUP selector only when we have added more than one group. 
	// if not, we don't have nothing to group!
	if (PRESETS_Selected_Index.size() > 1 && groups.size() > 1)
		bBuildGroupSelector = _buildGroupSelector;
	else
	{
		bBuildGroupSelector = false;
		SHOW_ImGui_Selectors = false;
	}

	//--

	// group selector

	if (bBuildGroupSelector)
	{
		// will combine all group
		//#define NUM_MAIN_SELECTOR_PRESETS 8
		//mainSelector.setMax(NUM_MAIN_SELECTOR_PRESETS);

		ofParameterGroup params_GroupMainSelector{ "GROUP" };
		for (int i = 0; i < PRESETS_Selected_Index.size(); i++)
		{
			params_GroupMainSelector.add(PRESETS_Selected_Index[i]);
		}

		// TODO: should allow customize keys..
		//create the extra main selector
		add(params_GroupMainSelector, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

		//--

		// TODO:
		// store the main slector only!
		int _last = groups.size() - 1;
		//int _last = PRESETS_Selected_Index.size() - 1;
		GROUP_Selected_Index.setMax(groupsSizes[_last] - 1);
		//GROUP_Selected_Index.setMax(_last);
		//GROUP_Selected_Index = PRESETS_Selected_Index[_last];
		GROUP_Selected_Index.makeReferenceTo(PRESETS_Selected_Index[_last]);

		//excludes all selectors except the main one. the other will be saved as preset
		params_PRESETS_Selected.setSerializable(false);

	}

	//--

	PRESETS_Selected_Index_PRE.resize(PRESETS_Selected_Index.size());
	for (int i = 0; i < PRESETS_Selected_Index_PRE.size(); i++)
	{
		PRESETS_Selected_Index_PRE[i] = -1;
	}

	//--

	//callback
	ofAddListener(params_Randomizer.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);

	//----

	//ImGui

#ifndef MODE_ImGui_EXTERNAL
	ImGui_Setup();
#endif

#ifdef INCLUDE_ofxUndoSimple
	undoStringParams = groups[0].toString();//TODO: main group only
#endif

	//--

	bSplitGroupFolders.setSerializable(false);//force to this mode. do not store

	// some params that we want to store
	params_Options.add(MODE_Editor);
	params_Options.add(bSplitGroupFolders);
	params_Control.add(params_Options);

	params_Gui.add(bThemDark);
	params_Gui.add(SHOW_ImGui_Selectors);
	params_Gui.add(SHOW_ImGui_PresetsParams);
	params_Gui.add(SHOW_Help);
	params_Control.add(params_Gui);

	//--

	// user-kit
	params_UserKitSettings.setName("USER-KIT");
	params_UserKitSettings.add(params_PRESETS_Selected);//includes all selectors

	// main group selector
	int _last = groups.size() - 1;
	GROUP_Selected_Index.set("GROUP SELECTOR", 0, 0, groupsSizes[_last] - 1);
	params_UserKitSettings.add(GROUP_Selected_Index);

	//--

	//TODO:
	//int _last = (groups.size() - 1);
	//int _last = (PRESETS_Selected_Index.size() - 1);
	////params_UserKitSettings.add(PRESETS_Selected_Index[_last]);//includes group selector only!
	////params_UserKitSettings.add(PRESET_Selected_IndexMain);
	//PRESETS_Selected_Index[_last].setSerializable(true);
	////exclude saving all slectors except last one, that will be enalbed at setup
	//PRESETS_Selected_Index[_last].setMax(groupsSizes[_last]-1);

	//----

	// custom path
	params_Custom.setName("CUSTOM PATH");
	params_Custom.add(bPathDirCustom);
	params_Custom.add(pathDirCustom);
	params_UserKitSettings.add(params_Custom);

	ofAddListener(params_UserKitSettings.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_UserKit);

	//-------

	startup();
}

//--------------------------------------------------------------
void ofxPresetsManager::startup()
{
	ofLogNotice(__FUNCTION__);

	//DISABLE_CALLBACKS = false;//enable callbacks after setup

	//--

	// load all app session settings & randomizers (not the related presets)
	load_ControlSettings();// here bPathDirCustom is loaded (but if files are present, not in first runtime)

	DISABLE_CALLBACKS = false;// enable callbacks after setup

	//--

	if (bPathDirCustom.get())
	{
		if (pathDirCustom.get() != "DEFAULT_DATA_PATH") buildCustomUserKit();
		else bPathDirCustom = false;// force false
	}
	if (!bPathDirCustom.get())
	{
		buildDefaultUserKit();
	}

	//--

	// timer auto save
	timerLast_Autosave = ofGetElapsedTimeMillis();

	setVisible_GUI_Internal(false);
	windowResized(ofGetWidth(), ofGetHeight());

	//--

	//// TODO: 
	//// memory mode
	//load_AllKit_ToMemory();

	//--

	//// TODO:
	// workflow
	// check if presets folders is empty. then populate all elements if not
	//doCheckPresetsFolderIsEmpty();

	//--

	// TODO:
	// create data folders if they are not presets: 
	// when you create a new project or added the addon to your existing project
	// and no /data files are present
	CheckAllFolders();

	//--

	// group selector

	// TODO: startup crash when first clicking main group...
	//// refresh already loaded user global main selector
	//if (bBuildGroupSelector)
	//{
	//	int _last = groups.size() - 1;
	//	cout << "PRESETS_Selected_Index[_last] : " << PRESETS_Selected_Index[_last] << endl;
	//	PRESETS_Selected_Index[_last] = groupsSizes[_last];
	//	cout << "PRESETS_Selected_Index[_last] : " << PRESETS_Selected_Index[_last] << endl;
	//	PRESETS_Selected_Index[_last] = GROUP_Selected_Index.get();
	//	cout << "PRESETS_Selected_Index[_last] : " << PRESETS_Selected_Index[_last] << endl;
	//}

	//--
}

//--------------------------------------------------------------
void ofxPresetsManager::update(ofEventArgs & args)
{
	if (!DISABLE_CALLBACKS)
	{
		//-

		// randomizer timer mode latch
		if (bIsDoneLoad &&
			MODE_LatchTrig &&
			!PLAY_RandomizeTimer) {

			randomizerTimer = ofGetElapsedTimeMillis();

			if (PRESET_Selected_IndexMain != 0)
			{
				bLatchRun = true;
			}
			else
			{
				bLatchRun = false;
			}
		}

		//-

		// randomize timer
		if (PLAY_RandomizeTimer || MODE_LatchTrig)//?
		{
			uint32_t _time = ofGetElapsedTimeMillis();
			timerRandomizer = _time - randomizerTimer;

			if (PRESET_Selected_IndexMain < presetsRandomModeShort.size()) {// avoid out of range

				// A. long mode
				if (presetsRandomModeShort[PRESET_Selected_IndexMain] == false)// get if it's marked as shor or long by default (false)
				{
					if (timerRandomizer >= randomizeDuration)
					{
						if (MODE_LatchTrig) {
							if (bLatchRun) {
								loadPreset(0);
							}
						}
						else {
							bRandomizeIndex = true;// TODO: can be improved calling directly the method! bc this flag will be readed on update()..
						}
					}
				}

				// B. short mode
				else {
					if (timerRandomizer >= randomizeDurationShort)
					{
						if (MODE_LatchTrig) {
							if (bLatchRun) {
								loadPreset(0);
							}
						}
						else {
							bRandomizeIndex = true;
						}
					}
				}
			}
		}

		//--

		// 1.0.2 draw progress bar for the randomizer timer
		//// long mode
		//if (presetsRandomModeShort[PRESET_Selected_IndexMain - 1] == false) _prog = timerRandomizer / (float)randomizeDuration;
		//// short mode
		//else _prog = timerRandomizer / (float)randomizeDurationShort;
		// bar relative only to long
		if (PLAY_RandomizeTimer) {
			randomizerProgress = 100 * timerRandomizer / (float)randomizeDuration;
		}
		else if (MODE_LatchTrig) {
			if (bLatchRun) {
				randomizerProgress = 100 * timerRandomizer / (float)randomizeDuration;
			}
			else {
				randomizerProgress = 0;
			}
		}
		else {
			randomizerProgress = 0;
		}
		_prog = randomizerProgress / 100.f;

		//--

		// TODO:
		//_totalDicesStr = "/ " + ofToString(randomizedDice.getMax());

		//-

		//plotters
#ifdef INCLUDE_PERFORMANCE_MEASURES 
		TS_START("LOAD 1");
		TS_STOP("LOAD 1");
		TS_START("LOAD 2");
		TS_STOP("LOAD 2");
		TS_START("LOAD MEM");
		TS_STOP("LOAD MEM");
#endif

		//--

		// TODO: this disables the easycallback feature..
		if (isDoneLoad())
		{
			ofLogNotice(__FUNCTION__) << groups[0].getName() << " PRESET " << PRESET_Selected_IndexMain << " LOADED.";
			ofLogVerbose(__FUNCTION__) << groups[0].getName() << " -------------------------------------------------------------";
			//ofLogNotice() << endl;
		}

		//--

		// TODO:
		////autosave
		////&& autoLoad? 
		//if (!MODE_Browser_NewPreset && autoSave && bAutosaveTimer && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
		//{
		//	ofLogNotice(__FUNCTION__) << "[AUTOSAVE]";

		//	//app settings
		//	save_ControlSettings();

		//	//save current preset
		//	doSave(PRESET_Selected_IndexMain);

		//	if (!MODE_MemoryLive)
		//	{
		//		//MODE A: from hd file
		//		//not required because the files are already there
		//	}
		//	else
		//	{
		//		//MODE B: direct from memory
		//		save_AllKit_FromMemory();
		//	}

		//	//-

		//	//auto save timer
		//	timerLast_Autosave = ofGetElapsedTimeMillis();
		//}
	}
}

//---------------------------------------------------------------------
void ofxPresetsManager::draw(ofEventArgs & args)
{
	//----

	//user clicker boxes preset selector 
	//(from live kit/favorites)
	if (SHOW_ClickPanel)
	{
		drawPresetClicker();
	}

	//----

	//gui

#ifndef MODE_ImGui_EXTERNAL
	//draw ImGui
	if (SHOW_ImGui)
	{
		ImGui_Draw_WindowBegin();

		//draw content
		bImGui_mouseOver = ImGui_Draw_Window();

		ImGui_Draw_WindowEnd();
	}
#else
	bImGui_mouseOver = false;
#endif

}

//--------------------------------------------------------------
void ofxPresetsManager::windowResized(int w, int h)
{
	ofLogNotice(__FUNCTION__) << w << "," << h;
}

//--------------------------------------------------------------
void ofxPresetsManager::drawPresetClicker()
{
	// user trigger boxes clickable selector and save button
	// draws some minimalistic graphics to monitor the active preset
	// when graphics are drawn you can also click on them for saving/loading

	//-

	ofColor _colorText;// lines and text color
	ofColor _colorButton;// bg selected button
	ofColor _colorBg;// background color

	float _round = 3.0f;
	float _pad = 3.0f;
	string _label;

	//-

	if (!lastMouseButtonState && ofGetMousePressed())
	{
		mousePressed(ofGetMouseX(), ofGetMouseY());
	}
	lastMouseButtonState = ofGetMousePressed();

	//-

	ofPushStyle();
	ofPushMatrix();
	ofTranslate(clicker_Pos);

	if (bThemDark) {// dark theme
		_colorText = ofColor(0, 255);
		_colorButton = ofColor(8, 100);
		_colorBg = ofColor(200, 50);
	}
	else {// light theme
		_colorText = ofColor(255, 200);
		_colorButton = ofColor(0, 150);
		_colorBg = ofColor(0, 128);
	}

	//----

	// draw keys buttons for each row/group

	for (size_t i = 0; i < keys.size(); ++i)
	{
		// 0. bg box of all boxes background
		ofFill();
		ofSetColor(_colorBg);
		int _extraButs;
		_extraButs = (i == groups.size() - 1 ? 2 : 1);// only main group has gui toggle button
		ofDrawRectangle(0, i * cellSize, cellSize * (keys[i].size() + _extraButs), cellSize);
		// amount group presets + 2 save. + gui if it's main group row

		//-

		// 1. draw each preset box button
		ofSetColor(_colorText);
		ofNoFill();

		size_t k = 0;// iterate keys
		for (; k < keys[i].size(); ++k)
		{
			// 1.1 outbox border container
			ofDrawRectangle(cellSize * k, cellSize * i, cellSize, cellSize);

			//--

			// 2. inner box. double mark current selected preset
			if (PRESETS_Selected_Index[i] == k)//it is selected
			{
				ofPushStyle();

				// filled
				ofSetColor(_colorButton);
				ofFill();
				ofDrawRectRounded(
					cellSize * k + _pad, cellSize * i + _pad,
					cellSize - 2 * _pad, cellSize - 2 * _pad,
					_round);
				ofNoFill();

				// border only
				ofSetColor(_colorText);
				ofNoFill();
				ofDrawRectRounded(
					cellSize * k + _pad, cellSize * i + _pad,
					cellSize - 2 * _pad, cellSize - 2 * _pad,
					_round);

				ofPopStyle();
			}

			//--

			// 1.2 label boxes
			if (!myFont.isLoaded())// without ttf font
			{
				ofDrawBitmapString(ofToString((char)keys[i][k]),
					cellSize*k + 8,
					cellSize*i + 18);
			}
			else// custom font 
			{
				myFont.drawString(ofToString((char)keys[i][k]),
					cellSize * k + (0.5 * cellSize - 0.25 * sizeTTF),
					cellSize * i + (0.5 * cellSize + 0.5 * sizeTTF));
			}
		}

		// here, it's last k = keys[i].size()

		int ySave = cellSize * i + (0.5 * cellSize + 0.5 * sizeTTF);

		//--

		//if (!autoSave)
		{
			// 3.1 save button

			ofDrawRectangle(cellSize * k, cellSize * i, cellSize, cellSize);

			//--

			// 3.2 save label

			_label = "SAVE";
			if (!myFont.isLoaded())// without ttf font
			{
				ofDrawBitmapString(_label,
					cellSize*k + 8,
					cellSize*i + 18);
			}
			else// custom font 
			{
				float wx = 0.5f * myFont.getStringBoundingBox(_label, 0, 0).width;
				myFont.drawString(_label,
					cellSize * k + 0.5 * cellSize - wx,
					ySave);
			}
		}

		//--

		k++;

		//--

		// 5. gui toggle button box

		int _i;
		if (bBuildGroupSelector) _i = groups.size() - 1;
		else if (groups.size() > 1) _i = 1;
		else _i = 0;

		if (i == _i)
		{
			ofDrawRectangle(cellSize * k, cellSize * i, cellSize, cellSize);

			//--

			// 6. show gui label button

			_label = "GUI";

			// mark if gui visible
			if (SHOW_ImGui)
			{
				// filled
				ofSetColor(_colorButton);
				ofFill();
				ofDrawRectRounded(cellSize * k + _pad, cellSize * i + _pad,
					cellSize - 2 * _pad, cellSize - 2 * _pad,
					_round);

				// border only
				ofNoFill();
				ofSetColor(_colorText);
				ofDrawRectRounded(cellSize * k + _pad, cellSize * i + _pad,
					cellSize - 2 * _pad, cellSize - 2 * _pad,
					_round);
			}

			// label
			if (!myFont.isLoaded())// without ttf font
			{
				ofDrawBitmapString(_label, cellSize * k + 8, cellSize * i + 18);
			}
			else// custom font 
			{
				ofSetColor(_colorText);
				float wx = 0.5f * myFont.getStringBoundingBox(_label, 0, 0).width;
				myFont.drawString(_label,
					cellSize * k + 0.5 * cellSize - wx,
					ySave);
			}
		}

		//--

		// 7. label paramGroup name to the left of clicker

		if (SHOW_GroupName)
		{
			string info = groups[i].getName();

			// double font to improve different background colors
			int gap = 1;

			float strW = 50;
			if (myFont.isLoaded()) {
				strW = myFont.getStringBoundingBox(info, 0, 0).width;
			}
			else {// not loaded font
				strW = 100;
				ySave -= 10;
			}
			int xG = -strW - 20;
			ySave = ySave - 2;// little up

			if (bThemDark) ofSetColor(_colorBg);// shadow
			else ofSetColor(_colorButton);// shadow
			if (myFont.isLoaded()) myFont.drawString(info, xG + gap, ySave + gap);
			else ofDrawBitmapString(info, xG + gap, ySave + gap);

			ofSetColor(_colorText);
			if (myFont.isLoaded()) myFont.drawString(info, xG, ySave);
			else ofDrawBitmapString(info, xG, ySave);
		}

		//--

		// 8. help info text: 
		{
			bool bLateralPosition = false;// false = on top
			bool bLeftPosition = true;

			debugClicker = SHOW_Help.get();
			if (debugClicker && ENABLE_Keys && (i == 0))
			{
				string info = helpInfo;

				int x = 0;
				int y = 0;
				int pad = 13;

				// A. vertical position below boxes
				if (!bLateralPosition)
				{
					float hh = ofxSurfingHelpers::getHeightBBtextBoxed(myFont, info);
					x = pad;
					y -= hh + pad;
				}

				// B. lateral position right to the boxes
				else
				{
					if (!bLeftPosition)
					{
						x = cellSize * i + pad;
						y = ySave - sizeTTF;


					}
					else {
						float strW;
						if (myFont.isLoaded())
						{
							strW = myFont.getStringBoundingBox(info, 0, 0).getWidth();
							strW += myFont.getStringBoundingBox(groups[0].getName(), 0, 0).getWidth();
							strW += 50;
						}
						x = -strW;
						y = ySave;
					}
				}

				ofxSurfingHelpers::drawTextBoxed(myFont, info, x, y, _colorText, _colorBg, true, _colorButton);
			}
		}
	}

	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
ofxPresetsManager::~ofxPresetsManager()
{
	//exit();//TODO: not sure if can avoid call manually exit(), bc here groups could be externally destroyed..
	//so we would prefer to call presetsManager.exit() manually on the first place sorting.
}

//---

#pragma mark - ENGINE

//--------------------------------------------------------------
string ofxPresetsManager::getPresetPath(string _gName, int _presetIndex)
{
	string _pathFolder;
	string _pathFilename;
	string _path;

	_pathFolder = path_UserKit_Folder + "/" + path_PresetsFavourites + "/";

	//append group name to subfolder files by each parameter group
	if (bSplitGroupFolders) _pathFolder += _gName + "/";

	_pathFilename = _gName + filenamesPrefix + ofToString(_presetIndex);
	_path = _pathFolder + _pathFilename;
	_path += fileExtension;

	ofLogVerbose(__FUNCTION__) << "group name: " << _gName << " path: " << _path;

	return _path;
}

//--------------------------------------------------------------
string ofxPresetsManager::getGroupPath(string _gName)
{
	string _pathFolder;

	_pathFolder = path_UserKit_Folder + "/" + path_PresetsFavourites + "/";

	//append group name to subfolder files by each parameter group
	if (bSplitGroupFolders) _pathFolder += _gName + "/";

	ofLogVerbose(__FUNCTION__) << "group name: " << _gName << " path: " << _pathFolder;

	return _pathFolder;
}

//--------------------------------------------------------------
string ofxPresetsManager::getGroupPath(int _index)
{
	string _pathFolder;
	string _gName;
	_gName = getGroupPath(groups[_index].getName());
	if (_index < groups.size()) _pathFolder = _gName;
	else return "UNKNOWN";

	////append group name to subfolder files by each parameter group
	//if (bSplitGroupFolders) _pathFolder += _gName + "/";

	ofLogVerbose(__FUNCTION__) << "group path: " << _pathFolder;

	return _pathFolder;
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup _params, int _amt_presets)//main adder
{
	//main adder of a ofParameterGroup for preset management

	ofLogNotice(__FUNCTION__) << "Added group " << _params.getName() << " with " << _amt_presets << " presets";
	int _size = groups.size();

	groups.push_back(_params);//each enqueued group of params handles all presets of each added ofParameterGroup
	groupsSizes.push_back(_amt_presets);

	//preset selectors
	ofParameter<int> p{ groups[_size].getName(), 0, 0,  _amt_presets - 1 };
	//p.setSerializable(false);//exclude saving all slectors except last one, that will be enalbed at setup
	PRESETS_Selected_Index.push_back(p);
	params_PRESETS_Selected.add(PRESETS_Selected_Index[_size]);

	//-

	//TODO:

	//A. first and main group 0 
	//has special control helpers, panel, randomizers and features

	if (_size == 0)
	{
		//update control gui panel params
		mainGroupAmtPresetsFav = _amt_presets;
		PRESET_Selected_IndexMain.setMax(mainGroupAmtPresetsFav - 1);

		mainGroupPresetsXmls.resize(mainGroupAmtPresetsFav);

		//-

		//used for path folder and xml presets file names
		ofLogNotice(__FUNCTION__) << "group 0 : " << groups[0].getName();

		//-

		//memory mode
		load_AllKit_ToMemory();
	}

	////B. extra groups
	////else
	//{
	//	//ofParameter<int> p{ groups[_size].getName(), 0, 0,  groupsSizes[_size] - 1};
	//	//PRESETS_Selected_Index.push_back(p);
	//	//ofLogNotice(__FUNCTION__) << "group " << _size << " : " << groups[_size].getName();
	//}
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup _params, initializer_list<int> _keysList)
{
	//adds a ofParameterGroup for preset management with the list of trigger keys

	add(_params, _keysList.size());//re add with known size affter counted passed key triggers

	keys.resize(groups.size());
	int i = groups.size() - 1;
	keys[i].reserve(_keysList.size());

	for (const int &key : _keysList)
	{
		keys[i].push_back(key);
	}

	if (keysNotActivated) addKeysListeners();
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::save(int presetIndex, int guiIndex)
{
	ofLogNotice(__FUNCTION__) << "group: " << guiIndex << " preset: " << presetIndex;

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		//MODE A. it's important if this line is before or after ofSerialize
		DONE_save = true;

		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);

		bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);

		if (b) ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " " << guiIndex << " at " << _path;
		else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " at " << _path;
	}

	//--

	////check clamp limiters
	//if ((guiIndex >= 0) && (guiIndex < (int)groups.size()))
	//{
	//	//A. main group
	//	if ((guiIndex == 0) &&
	//		(presetIndex >= 0) && (presetIndex < mainGroupPresetsXmls.size()))
	//	{
	//		ofLogVerbose(__FUNCTION__) << "DONE_save (by index)";

	//		//MODE A. it's important if this line is before or after ofSerialize
	//		DONE_save = true;

	//		//MODE A: from hd file
	//		if (!MODE_MemoryLive)
	//		{
	//			ofLogNotice(__FUNCTION__) << "FILE MODE";
	//			TS_START("SAVE FILE 1");
	//			std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
	//			bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
	//			if (b) ofLogNotice(__FUNCTION__) << " " << groups[guiIndex].getName() << " at " << _path;
	//			else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " at " << _path;
	//			TS_STOP("SAVE FILE 1");
	//		}

	//		//MODE B: direct from memory
	//		else
	//		{
	//			ofLogNotice(__FUNCTION__) << "MEMORY MODE";
	//			TS_START("SAVE MEM 1");
	//			if ((presetIndex >= 0) && (presetIndex < mainGroupPresetsXmls.size()))
	//			{
	//				ofSerialize(mainGroupPresetsXmls[presetIndex], groups[guiIndex]);
	//			}
	//			else
	//			{
	//				ofLogError(__FUNCTION__) << "mainGroupPresetsXmls OUT OF RANGE";
	//			}
	//			TS_STOP("SAVE MEM 1");
	//		}

	//		////MODE B. it's important if this line is before or after ofSerialize
	//		//DONE_save = true;
	//	}

	//	//TODO:
	//	//B. extra groups guiIndex != 0
	//	else if ((presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	//	{
	//		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
	//		bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
	//		if (b) ofLogNotice(__FUNCTION__) << >" << groups[guiIndex].getName() << " : " << guiIndex << " at " << _path;
	//		else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " at " << _path;
	//	}
	//}

	//--

	else
	{
		ofLogError(__FUNCTION__) << "Group or Preset out of ranges! preset: " << ofToString(presetIndex) << " group: " << ofToString(guiIndex);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::save(int presetIndex, string gName)
{
	ofLogNotice(__FUNCTION__) << "group: " << gName << " preset: " << presetIndex;

	int guiIndex = getGuiIndex(gName);

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		DONE_save = true;
		{
			TS_START("SAVE FILE 2");//for TimeMeasurements only
			std::string _path = getPresetPath(gName, presetIndex);
			bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
			if (b) ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " " << guiIndex << " at " << _path;
			else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " at " << _path;
			TS_STOP("SAVE FILE 2");//for TimeMeasurements only
		}
	}

	//--

	////check clamp limiters
	//if ((guiIndex >= 0) && (guiIndex < (int)groups.size()))
	//{
	//	//A. main group
	//	if ((guiIndex == 0) &&
	//		(presetIndex >= 0) && (presetIndex < mainGroupPresetsXmls.size()))
	//	{
	//		ofLogVerbose(__FUNCTION__) << "DONE_save (by name)";

	//		//MODE A. it's important if this line is before or after ofSerialize
	//		DONE_save = true;

	//		//MODE A: from hd file
	//		if (!MODE_MemoryLive)
	//		{
	//			TS_START("SAVE FILE 2");//for TimeMeasurements only
	//			std::string _path = getPresetPath(gName, presetIndex);
	//			bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
	//			if (b) ofLogNotice(__FUNCTION__) << " " << groups[guiIndex].getName() << " : " << guiIndex << " at " << _path;
	//			else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " at " << _path;
	//			TS_STOP("SAVE FILE 2");//for TimeMeasurements only
	//		}

	//		//MODE B: direct from memory
	//		else
	//		{
	//			TS_START("SAVE MEM 2");//for TimeMeasurements only

	//			if (presetIndex < mainGroupPresetsXmls.size()) {
	//				ofSerialize(mainGroupPresetsXmls[presetIndex], groups[guiIndex]);
	//			}
	//			else {
	//				ofLogError(__FUNCTION__) << "mainGroupPresetsXmls OUT OF RANGE";
	//			}

	//			TS_STOP("SAVE MEM 2");//for TimeMeasurements only
	//		}

	//		////MODE B. it's important if this line is before or after ofSerialize
	//		//DONE_save = true;
	//	}

	//	//TODO:
	//	//B. extra groups guiIndex != 0
	//	else if ((presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	//	{
	//		ofSerialize(mainGroupPresetsXmls[presetIndex], groups[guiIndex]);
	//		ofLogNotice(__FUNCTION__) << "Serialized: " << groups[guiIndex].getName();
	//	}
	//}

	//--

	else
	{
		ofLogError(__FUNCTION__) << "Group out of range. preset: " << ofToString(presetIndex) << " group: " << ofToString(guiIndex);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load(int presetIndex, int guiIndex)
{
	ofLogNotice(__FUNCTION__) << "group: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex;

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		//TODO:
		//avoid startup creators...
		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
		bool b = ofxSurfingHelpers::loadGroup(groups[guiIndex], _path);
		if (!b) {
			ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " : " << guiIndex << " at " << _path;

			//file not found so create one file instead
			save(presetIndex, guiIndex);
		}

		//TODO: ? already marked?
		//mark selected
		PRESETS_Selected_Index[guiIndex] = presetIndex;
	}

	//--

	////check groups range clamp limiters
	//if ((guiIndex >= 0) && (guiIndex < (int)groups.size()))
	//{
	//	//A. main group
	//	if ((guiIndex == 0) &&
	//		(presetIndex >= 0) && (presetIndex < mainGroupPresetsXmls.size()))
	//	{
	//		if (!MODE_MemoryLive)
	//		{
	//			//MODE A: from hd file
	//			TS_START("LOAD FILE 1");//for TimeMeasurements only
	//			std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
	//			bool b = ofxSurfingHelpers::loadGroup(groups[guiIndex], _path);
	//			if (b) ofLogNotice(__FUNCTION__) << "Load: " << groups[guiIndex].getName() << " at " << _path;
	//			else ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " at " << _path;
	//			TS_STOP("LOAD FILE 1");//for TimeMeasurements only
	//		}
	//		else
	//		{
	//			//MODE B: direct from memory
	//			TS_START("LOAD MEM 1");//for TimeMeasurements only
	//			//using xml array
	//			if (presetIndex < mainGroupPresetsXmls.size())
	//			{
	//				ofDeserialize(mainGroupPresetsXmls[presetIndex], groups[guiIndex]);
	//			}
	//			else
	//			{
	//				ofLogError(__FUNCTION__) << "mainGroupPresetsXmls OUT OF RANGE";
	//			}
	//			TS_STOP("LOAD MEM 1");//for TimeMeasurements only
	//		}

	//		//-

	//		//mark selected
	//		PRESETS_Selected_Index[guiIndex] = presetIndex;

	//		//-

	//		//callback
	//		//MODE A. it's important if this line is before or after ofSerialize
	//		ofLogVerbose(__FUNCTION__) << "DONE_load (by index)";
	//		DONE_load = true;

	//		//simple callback
	//		bIsDoneLoad = true;
	//	}

	//	//TODO:
	//	//B. extra groups guiIndex != 0
	//	else if ((presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	//	{
	//		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
	//		bool b = ofxSurfingHelpers::loadGroup(groups[guiIndex], _path);
	//		if (!b) {
	//			ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " : " << guiIndex << " at " << _path;

	//			//file not found so create one file instead
	//			save(presetIndex, guiIndex);
	//		}

	//		//mark selected
	//		PRESETS_Selected_Index[guiIndex] = presetIndex;
	//	}
	//}

	//--

	else
	{
		ofLogError(__FUNCTION__) << "Group or Preset out of ranges! group: " << guiIndex << " preset: " << presetIndex;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load(int presetIndex, string gName)
{
	ofLogNotice(__FUNCTION__) << "name: " << gName << " preset: " << presetIndex;

	int guiIndex = getGuiIndex(gName);

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		// TODO:
		// avoid startup creators...
		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
		bool b = ofxSurfingHelpers::loadGroup(groups[guiIndex], _path);
		if (!b) {
			ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " : " << guiIndex << " at " << _path;

			//file not found so create one file instead
			save(presetIndex, guiIndex);
		}

		// mark selected
		PRESETS_Selected_Index[guiIndex] = presetIndex;
	}

	//--

	////check clamp limiters
	//if ((guiIndex >= 0) && (guiIndex < (int)groups.size()))
	//{
	//	//A. main group
	//	if ((guiIndex == 0) &&
	//		(presetIndex >= 0) && (presetIndex < mainGroupPresetsXmls.size()))
	//	{
	//		if (!MODE_MemoryLive)
	//		{
	//			//MODE A: from hd file
	//			TS_START("LOAD FILE 2");//for TimeMeasurements only
	//			std::string strPath = getPresetPath(gName, presetIndex);
	//			ofXml settings;
	//			settings.load(strPath);
	//			ofDeserialize(settings, groups[guiIndex]);
	//			TS_STOP("LOAD FILE 2");//for TimeMeasurements only
	//		}
	//		else
	//		{
	//			//MODE B: direct from memory
	//			TS_START("LOAD MEM 2""LOAD MEM 2");//for TimeMeasurements only
	//			if (presetIndex < mainGroupPresetsXmls.size()) {
	//				ofDeserialize(mainGroupPresetsXmls[presetIndex], groups[guiIndex]);
	//			}
	//			else {
	//				ofLogError(__FUNCTION__) << "mainGroupPresetsXmls OUT OF RANGE LOAD";
	//			}
	//			TS_STOP("LOAD MEM 2");//for TimeMeasurements only
	//		}

	//		//-

	//		//mark selected
	//		PRESETS_Selected_Index[guiIndex] = presetIndex;

	//		//-

	//		//callback
	//		ofLogVerbose(__FUNCTION__) << "DONE_load (by name)";
	//		DONE_load = true;

	//		//simple callback
	//		bIsDoneLoad = true;
	//	}

	//	//TODO:
	//	//B. extra groups guiIndex != 0
	//	else if ((presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	//	{
	//		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
	//		bool b = ofxSurfingHelpers::loadGroup(groups[guiIndex], _path);
	//		if (!b) {
	//			ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " : " << guiIndex << " at " << _path;

	//			//file not found so create one file instead
	//			save(presetIndex, guiIndex);
	//		}

	//		//mark selected
	//		PRESETS_Selected_Index[guiIndex] = presetIndex;
	//		//TODO: take car bc this will re-trig the callback too (if load it's "called from the callback")
	//	}
	//}

	//--

	else
	{
		ofLogError(__FUNCTION__) << "Group out of range. group: " << guiIndex << " preset: " << presetIndex;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::loadPreset(int p)
{
	if (!DISABLE_CALLBACKS)
	{
		ofLogNotice(__FUNCTION__) << ofToString(p);

		//TODO:
		//mode latch
		if (MODE_LatchTrig && bLatchRun)
		{
			if (p != 0)
			{
				bLatchRun = true;
			}
			else if (p == 0)
			{
				bLatchRun = false;
			}

			//randomizerTimer = ofGetElapsedTimeMillis();
		}

		if (PRESET_Selected_IndexMain >= 0 && PRESET_Selected_IndexMain <= mainGroupAmtPresetsFav - 1)
		{
			PRESET_Selected_IndexMain = p;
			//PRESET_Selected_IndexMain_PRE = PRESET_Selected_IndexMain;//TODO:
		}
		else
		{
			ofLogError(__FUNCTION__) << "OUT OF RANGE! CLAMP PRESET TO 0";

			//workaround
			//clamp
			PRESET_Selected_IndexMain = 0;//set to first as default presets when out of range
		}
	}
}


//--------------------------------------------------------------
void ofxPresetsManager::savePreset(int presetIndex, int groupIndex)
{
	//if (!DISABLE_CALLBACKS)
	{
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex << " preset: " << presetIndex;

		save(presetIndex, groupIndex);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::loadPreset(int presetIndex, int groupIndex)
{
	//if (!DISABLE_CALLBACKS)
	{
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex << " preset: " << presetIndex;

		load(presetIndex, groupIndex);
	}
}

//--

//engine helpers
//--------------------------------------------------------------
int ofxPresetsManager::getPresetIndex(int guiIndex) const
{
	if (guiIndex >= 0 && guiIndex < (int)groups.size())
	{
		return PRESETS_Selected_Index[guiIndex];
	}
	else
	{
		return -1;
	}
}

//--------------------------------------------------------------
int ofxPresetsManager::getPresetIndex(string gName) const
{
	int guiIndex = getGuiIndex(gName);

	if (guiIndex >= 0 && guiIndex < (int)groups.size())
	{
		return PRESETS_Selected_Index[guiIndex];
	}
	else
	{
		return -1;
	}
}

//--------------------------------------------------------------
int ofxPresetsManager::getGuiIndex(string name) const
{
	for (size_t i = 0; i < (int)groups.size(); ++i)
	{
		if (groups[i].getName() == name)
		{
			return i;
		}
	}
	return -1;
}

//--------------------------------------------------------------
void ofxPresetsManager::setModeKeySave(int key)
{
	modeKeySave = key;
}

//--------------------------------------------------------------
void ofxPresetsManager::setModeKeySwap(int key)
{
	modKeySwap = key;
}

#pragma mark - OF_LISTENERS

//----------------------------------------------------------------
void ofxPresetsManager::keyPressed(ofKeyEventArgs &eventArgs)
{
	const int &key = eventArgs.key;

	if (key == 'K' && !bImGui_mouseOver)//restore keys control
	{
		ENABLE_Keys = !ENABLE_Keys;
	}

	if (bKeys && ENABLE_Keys && !bImGui_mouseOver)//disable keys when mouse over gui
	{

		//-

		//mode key for saving with mouse or trigger keys
		if (key == modeKeySave)
		{
			bKeySave = true;
			ofLogVerbose(__FUNCTION__) << "modeKeySave TRUE" << endl;
			return;
		}

		//mode key for swap with mouse or trigger keys
		else if (key == modKeySwap)
		{
			bKeySwap = true;
			ofLogVerbose(__FUNCTION__) << "modKeySwap TRUE" << endl;
			return;
		}

		//-

		//TODO: 
		if (false) {}

		//controlled from outside
		//hide/show control gui
		else if (key == 'G')
		{
			SHOW_Gui_AdvancedControl = !SHOW_Gui_AdvancedControl;
			setVisible_GUI_Internal(SHOW_Gui_AdvancedControl);
			setVisible_GUI_ImGui(SHOW_Gui_AdvancedControl);
			setVisible_PresetClicker(SHOW_Gui_AdvancedControl);
		}
		else if (key == 'H')
		{
			SHOW_Help = !SHOW_Help;
		}
		else if (key == 'E')
		{
			MODE_Editor = !MODE_Editor;
		}

		//if (key == 'g')
		//{
		//	setVisible_PresetClicker(!isVisible_PresetClicker());
		//}

		//----

		//main group only

#ifdef INCLUDE_RANDOMIZER
		bool bEnableKeyRandomizers = false;
		if (bEnableKeyRandomizers) {
			//timer to randomize and choice a random preset from the kit
			if (key == 'R')
			{
				setTogglePlayRandomizerPreset();
			}
			else if (key == 'r')
			{
				doRandomizeWichSelectedPreset();
			}

			else if (key == 'E')
			{
				doRandomizeEditor();
			}
		}
#ifdef INCLUDE_ofxUndoSimple
		if (key == 'A')// previous
		{
			ofLogNotice(__FUNCTION__) << "UNDO <-";
			undoStringParams.undo();
			undoRefreshParams();
		}
		else if (key == 'D')// next
		{
			ofLogNotice(__FUNCTION__) << "REDO ->";
			undoStringParams.redo();
			undoRefreshParams();
		}
		else if (key == 'Q')// clear history
		{
			ofLogNotice(__FUNCTION__) << "UNDO CLEAR";
			undoStringParams.clear();
		}
#endif

#endif

		//----

		//navigate kit/favorites presets
		if (key == OF_KEY_RIGHT && ENABLE_KeysArrowBrowse)
		{
			////browse presets
			//if (MODE_Browser_NewPreset)
			//{
			//	currentFile++;
			//	if (currentFile > files.size() - 1) currentFile = files.size() - 1;
			//	displayNamePreset = fileNames[currentFile];
			//	ofLogNotice(__FUNCTION__) << "[>] LOAD" << endl;
			//	ofLogNotice(__FUNCTION__) << "Preset Name: " << displayNamePreset;
			//	doLoadMainGroupPreset(displayNamePreset);
			//}
			////browse favorites
			//else
			{
				//int i = PRESET_Selected_IndexMain;
				//i++;
				//if (i > mainGroupPresetsXmls.size() - 1) i = mainGroupPresetsXmls.size() - 1;
				//PRESET_Selected_IndexMain = i;

				//TODO: crashes
				//TODO: must make a selectors browser engine

				DISABLE_CALLBACKS = true;
				int i;
				int _lastGroup = groups.size() - 1;//last group (main selector) index
				if (_lastGroup < PRESETS_Selected_Index.size())
					i = PRESETS_Selected_Index[_lastGroup].get();//get selected
				int _lastIndex = groupsSizes[_lastGroup] - 1;
				i++;
				if (i > _lastIndex) i = _lastIndex;
				DISABLE_CALLBACKS = false;

				if (_lastGroup < PRESETS_Selected_Index.size())
					PRESETS_Selected_Index[_lastGroup] = i;
				else cout << "ERROR:" << _lastGroup << endl;
			}
		}

		else if (key == OF_KEY_LEFT && ENABLE_KeysArrowBrowse)
		{
			////browse presets
			//if (MODE_Browser_NewPreset)
			//{
			//	currentFile--;
			//	if (currentFile < 0) currentFile = 0;
			//	displayNamePreset = fileNames[currentFile];
			//	ofLogNotice(__FUNCTION__) << "[<] LOAD" << endl;
			//	ofLogNotice(__FUNCTION__) << "Preset Name: " << displayNamePreset;
			//	doLoadMainGroupPreset(displayNamePreset);
			//}
			////browse favorites
			//else
			{
				//int i = PRESET_Selected_IndexMain;
				//i--;
				//if (i < 0) i = 0;
				//PRESET_Selected_IndexMain = i;

				//DISABLE_CALLBACKS = true;
				//int _lastGroup = groups.size() - 1;//last group (main selector) index
				//int i = PRESETS_Selected_Index[_lastGroup];//get selected
				//i--;
				//if (i < 0) i = 0;
				//DISABLE_CALLBACKS = false;

				//PRESETS_Selected_Index[_lastGroup] = i;


				DISABLE_CALLBACKS = true;
				int i;
				int _lastGroup = groups.size() - 1;//last group (main selector) index
				if (_lastGroup < PRESETS_Selected_Index.size())
					i = PRESETS_Selected_Index[_lastGroup].get();//get selected
				int _lastIndex = groupsSizes[_lastGroup] - 1;
				i--;
				if (i < 0) i = 0;
				DISABLE_CALLBACKS = false;

				if (_lastGroup < PRESETS_Selected_Index.size())
					PRESETS_Selected_Index[_lastGroup] = i;
				else cout << "ERROR:" << _lastGroup << endl;
			}
		}

		//----

		//presets selectors

		for (int g = 0; g < keys.size(); g++)
		{
			for (int k = 0; k < keys[g].size(); k++)//performs all registered keys: one for each [8] preset
			{
				if (key == keys[g][k])
				{
					ofLogNotice(__FUNCTION__) << "key: " << (char)key;

					if (bKeySave)
					{
						save(k, g);
					}
					else
					{
						ofLogNotice(__FUNCTION__) << groups[g].getName() << " group: " << g << " preset: " << k;

						if (g < PRESETS_Selected_Index.size()) PRESETS_Selected_Index[g] = k;
					}

					return;
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::keyReleased(ofKeyEventArgs &eventArgs)
{
	//mod keys
	if (eventArgs.key == modeKeySave && ENABLE_Keys)
	{
		bKeySave = false;
		ofLogVerbose(__FUNCTION__) << "modeKeySave FALSE" << endl;
	}
	else if (eventArgs.key == modKeySwap && ENABLE_Keys)
	{
		bKeySwap = false;
		ofLogVerbose(__FUNCTION__) << "modKeySwap FALSE" << endl;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::addKeysListeners()
{
	ofAddListener(ofEvents().keyPressed, this, &ofxPresetsManager::keyPressed);
	ofAddListener(ofEvents().keyReleased, this, &ofxPresetsManager::keyReleased);
	keysNotActivated = false;
	bKeys = true;
}

//--------------------------------------------------------------
void ofxPresetsManager::removeKeysListeners()
{
	ofRemoveListener(ofEvents().keyPressed, this, &ofxPresetsManager::keyPressed);
	ofRemoveListener(ofEvents().keyReleased, this, &ofxPresetsManager::keyReleased);
}

//-----------------------------------------------------
void ofxPresetsManager::mousePressed(int x, int y)
{
	x = x - clicker_Pos.x;
	y = y - clicker_Pos.y;

	int xIndex = x / cellSize;
	int yIndex = y / cellSize;

	//Index -1 for out (left) of boxes
	xIndex = (x > 0) ? xIndex : -1;
	yIndex = (y > 0) ? yIndex : -1;

	//-

	//1. presets buttons & save button

	//click is iniside allowed presets/groups
	if ((yIndex >= 0) && (yIndex < (int)groups.size()))//valid group
	{
		//avoid outer panel logs. only into the group row levels
		if ((xIndex != -1) && (yIndex != -1) &&//valid preset. to the left of panels
			(xIndex >= 0) && (xIndex < groupsSizes[yIndex]))//valid group. to the right of panels

			ofLogNotice(__FUNCTION__) << groups[yIndex].getName() << " group: " << yIndex << " preset: " << xIndex;

		//-

		if ((xIndex >= 0) && (xIndex < groupsSizes[yIndex]))
		{
			//1. mod save controlled by modeKeySave
			if (bKeySave)
			{
				ofLogNotice(__FUNCTION__) << "SAVE";

				save(xIndex, yIndex);

				//will auto load and set the already clicked preset button
				if (yIndex < PRESETS_Selected_Index.size()) PRESETS_Selected_Index[yIndex] = xIndex;
			}

			//-

			//2. mod swap controlled by modKeySwap
			else if (bKeySwap)
			{
				ofLogNotice(__FUNCTION__) << "SWAP";

				if (yIndex < PRESETS_Selected_Index.size()) doSwap(yIndex, PRESETS_Selected_Index[yIndex], xIndex);
			}

			//-

			//3. no mod keys: normal load (not any key modifier pressed)
			else
			{
				ofLogNotice(__FUNCTION__) << "LOAD";

				if (yIndex < PRESETS_Selected_Index.size()) PRESETS_Selected_Index[yIndex] = xIndex;
			}
		}

		//-

		//2. last button (save button)
		else if (xIndex == groupsSizes[yIndex])
		{
			//TODO: hide save buttons if autoSave 
			//if (!autoSave)
			{
				ofLogNotice(__FUNCTION__) << "saveButton group:" << yIndex;

				save(PRESETS_Selected_Index[yIndex], yIndex);
			}
		}
	}

	//-

	//3. toggle show gui (on main group only)
	//TODO: hide save button on autosave mode...
	//int _offset = (autoSave ? 0 : 1);
	int _offset = 1;

	int _i;
	//if (bBuildGroupSelector) _i = groups.size() - 1;
	//else _i = 0;
	if (bBuildGroupSelector) _i = groups.size() - 1;
	//else _i = 1;
	else if (groups.size() > 1) _i = 1;
	else _i = 0;

	if ((yIndex == groups.size() - 1) && (xIndex == groupsSizes[_i] + _offset))
	{
		SHOW_ImGui = !SHOW_ImGui;

		ofLogNotice(__FUNCTION__) << "SHOW_ImGui : " << (SHOW_ImGui ? "TRUE" : "FALSE");
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doCloneRight(int pIndex)
{
	ofLogNotice(__FUNCTION__) << "from preset: " << pIndex;

	CheckAllFolders();

	for (int i = pIndex + 1; i < mainGroupAmtPresetsFav; i++)
	{
		save(i, 0);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doPopulateFavs()
{
	ofLogNotice(__FUNCTION__);

	CheckAllFolders();//? required

	for (int i = 0; i < mainGroupAmtPresetsFav; i++)
	{
		doRandomizeEditor();
		save(i, 0);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doSwap(int groupIndex, int fromIndex, int toIndex)
{
	ofLogNotice(__FUNCTION__) << "group:" << groupIndex << " : " << fromIndex << "->" << toIndex;

	string srcName = getPresetPath(groups[groupIndex].getName(), fromIndex);
	string dstName = getPresetPath(groups[groupIndex].getName(), toIndex);

	ofLogNotice(__FUNCTION__) << "Source: " << fromIndex;
	ofLogNotice(__FUNCTION__) << "Dest  : " << toIndex;
	ofLogNotice(__FUNCTION__) << "Source: " << srcName;
	ofLogNotice(__FUNCTION__) << "Dest  : " << dstName;

	//1. save source preset (from memory) to temp file
	string _pathSrc = "tempSrc.xml";
	ofXml _settingsSrc;
	ofSerialize(_settingsSrc, groups[groupIndex]);
	_settingsSrc.save(_pathSrc);

	//2. load destination "from kit" to memory
	std::string _pathDest = getPresetPath(groups[groupIndex].getName(), toIndex);
	ofXml _settingsDest;
	_settingsDest.load(_pathDest);
	ofDeserialize(_settingsDest, groups[groupIndex]);

	//3. save destination preset (from memory) to temp file
	ofXml _settingsDst2;
	ofSerialize(_settingsDst2, groups[groupIndex]);

	//4. using files
	//save source (from dest)
	_settingsDst2.save(srcName);
	_settingsSrc.save(dstName);

	//5. delete temp file
	ofFile _file;
	_file.removeFile(_pathSrc);

	//workflow
	//6. auto load source (the same preset was selected befor swap clicked!)
	PRESETS_Selected_Index[groupIndex] = toIndex;
}

//--------------------------------------------------------------
void ofxPresetsManager::doCloneAll()
{
	ofLogNotice(__FUNCTION__);

	CheckAllFolders();

	//auto save current preset
	if (autoSave)
	{
		ofLogVerbose(__FUNCTION__) << "autosave preset: " << PRESET_Selected_IndexMain.get();
		//doSave(PRESET_Selected_IndexMain);
		save(PRESET_Selected_IndexMain, 0);
	}

	//clone all
	for (int i = 0; i < mainGroupAmtPresetsFav; i++)
	{
		save(i, 0);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::setToggleKeysControl(bool active)
{
	bKeys = active;
}

#pragma mark - CALLBACKS

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Params_Editor(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		ofLogNotice(__FUNCTION__) << groups[0].getName() << " " << name << " : " << e;

		//-

		if (name == bRandomizeEditor.getName() && bRandomizeEditor)//trig randomize
		{
			bRandomizeEditor = false;

			doRandomizeEditor();
		}
		else if (name == bRandomizeEditorAll.getName() && bRandomizeEditorAll)//all
		{
			bRandomizeEditorAll = false;

			for (auto &p : editorPresets) {
				p.set(true);
			}
		}
		else if (name == bRandomizeEditorNone.getName() && bRandomizeEditorNone)//none
		{
			bRandomizeEditorNone = false;

			for (auto &p : editorPresets) {
				p.set(false);
			}
		}
		else if (name == bRandomizeEditorPopulateFavs.getName() && bRandomizeEditorPopulateFavs)//populate random all favs
		{
			bRandomizeEditorPopulateFavs = false;
			doPopulateFavs();
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Params_UserKit(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		if ((name != "exclude") //&&
			//(name != ImGui_Position.getName()) &&
			)
		{
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		if (false) {}

		//--

		//TODO: not working
		////custom path
		//else if (name == bPathDirCustom.getName())
		//{
		//	ofLogNotice(__FUNCTION__) << "bPathDirCustom: " << (bPathDirCustom.get() ? "TRUE" : "FALSE");

		//	//--

		//	if (bPathDirCustom.get())
		//	{
		//		if (pathDirCustom.get() != "DEFAULT_DATA_PATH") buildCustomUserKit();
		//		else bPathDirCustom = false;//force false
		//	}
		//	if (!bPathDirCustom.get())
		//	{
		//		buildDefaultUserKit();
		//	}
		//}

		//--

		//ALL GROUPS PRESETS 

		//presets selectors
		if (!DISABLE_CALLBACKS_SELECTORS)
		{
			//exclude group 0
			for (int g = 0; g < groups.size(); g++)//iterate each group
			{
				for (int p = 0; p < groupsSizes[g]; p++)//iterate each preset on each group
				{
					if (name == PRESETS_Selected_Index[g].getName() && PRESETS_Selected_Index[g].get() == p)
					{
						//some preset of any group changed

						ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " group: " << g << " preset: " << p;

						//-

						//if (g == 0)
						//{
						//	PRESETS_Selected_Index[p] = 
						//}

						//-

						//1. main group selected preset NOT CHANGED

						if (PRESETS_Selected_Index[g] == PRESETS_Selected_Index_PRE[g])
						{
							ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " PRESET " << p << " [NOT CHANGED]";

							//browser
							//if (MODE_Browser_NewPreset)
							//{
							//	if (autoLoad)
							//	{
							//		load(PRESET_Selected_IndexMain, g);
							//	}
							//}
							//bMustTrig = true;
						}

						//--

						//2. main group selected preset CHANGED

						else if (PRESETS_Selected_Index[g] != PRESETS_Selected_Index_PRE[g])
						{
							ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " PRESET " << p << " [CHANGED]";

							//-

							//workflow
							//browser mode bypasses autosave
							//if (autoSave && !MODE_Browser_NewPreset)
							if (autoSave)
							{
								save(PRESETS_Selected_Index_PRE[g], g);
							}

							//-

							//remember this PRE state to know if changed or not on the next time..
							PRESETS_Selected_Index_PRE[g] = PRESETS_Selected_Index[g];

							//-

							if (autoLoad) load(PRESETS_Selected_Index[g], g);
						}
					}
				}
			}

			//--

			if (name == "PRESET")//TODO: aux for main group
			{
				PRESETS_Selected_Index[0] = PRESET_Selected_IndexMain;
			}

			//--

		//------

		////PRESET MAIN

		////1. selected main preset

		////TODO: 
		////should be nice to add toggle to auto retrig or not behavior 

		//else if (name == "PRESET")
		//{
		//	ofLogNotice(__FUNCTION__) << "--------------------------------------------------------------";

		//	//1. main group selected preset NOT CHANGED

		//	if (PRESET_Selected_IndexMain == PRESET_Selected_IndexMain_PRE)
		//	{
		//		ofLogNotice(__FUNCTION__) << groups[0].getName() << " PRESET " << e << " [NOT Changed]";

		//		//browser
		//		if (MODE_Browser_NewPreset)
		//		{
		//			if (autoLoad)
		//			{
		//				load(PRESET_Selected_IndexMain, 0);
		//			}
		//		}
		//		bMustTrig = true;
		//	}

		//	//--

		//	//2. main group selected preset CHANGED

		//	else if (PRESET_Selected_IndexMain != PRESET_Selected_IndexMain_PRE)
		//	{
		//		ofLogNotice(__FUNCTION__) << groups[0].getName() << " PRESET " << PRESET_Selected_IndexMain;

		//		//-

		//		//workflow
		//		//browser mode bypasses autosave
		//		if (autoSave && !MODE_Browser_NewPreset)
		//		{
		//			save(PRESET_Selected_IndexMain_PRE, 0);
		//		}

		//		//-

		//		//remember this PRE state to know if changed or not on the next time..
		//		PRESET_Selected_IndexMain_PRE = PRESET_Selected_IndexMain;

		//		//-

		//		if (autoLoad) load(PRESET_Selected_IndexMain, 0);
		//	}
		//}


		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Params_Control(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		if ((name != "exclude") &&
			(name != ImGui_Position.getName()) &&
			(name != ImGui_Size.getName())// &&
			//(name != "DICE") &&
			//(name != "PRESET")
			)
		{
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		if (false) {}

		//-

		//mode edit
		else if (name == MODE_Editor.getName())
		{
			ofLogNotice(__FUNCTION__) << "MODE EDITOR: " << (MODE_Editor.get() ? "TRUE" : "FALSE");

			autoSave = MODE_Editor.get();

			//--

			//TODO:
			//refresh..
			buildHelpInfo();
		}

		//--

		//save load
		else if (name == "SAVE" && bSave)
		{
			ofLogNotice(__FUNCTION__) << "SAVE: " << e;
			bSave = false;
			//doSave(PRESET_Selected_IndexMain);
			save(PRESET_Selected_IndexMain, 0);
		}
		//else if (name == "LOAD" && bLoad)
		//{
		//	ofLogNotice(__FUNCTION__) << "LOAD: " << e;
		//	bLoad = false;
		//	doLoad(PRESET_Selected_IndexMain);
		//}

		//-

		//helper tools
		else if (name == "CLONE >" && bCloneRight)
		{
			ofLogNotice(__FUNCTION__) << "CLONE >: " << e;
			bCloneRight = false;
			doCloneRight(PRESET_Selected_IndexMain);
		}
		else if (name == "CLONE ALL" && bCloneAll)
		{
			ofLogNotice(__FUNCTION__) << "CLONE ALL: " << e;
			bCloneAll = false;
			doCloneAll();
		}

		//--

		////clicker
		//else if (name == "SHOW CLICK PANEL" && !SHOW_ClickPanel.get())
		//{
		//	SHOW_ImGui = false;//workflow
		//}
		//randomizer
		else if (name == "RANDOMIZE INDEX" && bRandomizeIndex)
		{
			ofLogNotice(__FUNCTION__) << "RANDOMIZE !";
			bRandomizeIndex = false;
			doRandomizeWichSelectedPreset();
		}
		//play randomizer
		else if (name == PLAY_RandomizeTimer.getName())
		{
			ofLogNotice(__FUNCTION__) << "MODE TIMER: " << e;
			if (PLAY_RandomizeTimer) {
				MODE_LatchTrig = false;
			}
		}
		//latch
		else if (name == "MODE LATCH")
		{
			ofLogNotice(__FUNCTION__) << "MODE LATCH: " << e;
			if (MODE_LatchTrig) {
				PLAY_RandomizeTimer = false;
			}
		}
		//else if (name == "SPEED FACTOR")
		//{
		//	ofLogNotice(__FUNCTION__) << "SPEED FACTOR: " << e;
		//	//randomizeDuration = randomize_MAX_DURATION * (1.f - randomizeSpeedF);
		//	randomizeDuration.setMax(randomizeSpeedF * randomize_MAX_DURATION);
		//	randomizeDurationShort.setMax(randomizeSpeedF * randomize_MAX_DURATION_SHORT);
		//}
		else if (name == randomizeDuration.getName())
		{
			ofLogNotice(__FUNCTION__) << "DURATION: " << e;

			randomizeDurationBpm = 60000.f / randomizeDuration;

			//randomizeSpeedF = -((float)randomizeDuration / (float)randomize_MAX_DURATION) + 1.f;
			////randomizeSpeedF = 1 + (randomizeDuration / (float)randomize_MAX_DURATION);
		}
		else if (name == randomizeDurationBpm.getName())
		{
			ofLogNotice(__FUNCTION__) << "BPM: " << e;

			//60,000 ms (1 minute) / Tempo (BPM) = Delay Time in ms for quarter-note beats
			randomizeDuration = 60000.f / randomizeDurationBpm;
			randomizeDurationShort = randomizeDuration / 4.f;
		}
#ifdef DEBUG_randomTest
		else if (name == "DICE")//when debug enabled: set dice by user to test
		{
			ofLogNotice(__FUNCTION__) << "DICE: " << e;
			doRandomizeWichSelectedPreset();
		}
#endif
		else if (name == "RESET DICES" && bResetDices)
		{
			ofLogNotice(__FUNCTION__) << "RESET DICES: " << e;
			doResetDices();
			bResetDices = false;
		}

		//--

		else if (name == "LOAD TO MEMORY" && loadToMemory)
		{
			ofLogNotice(__FUNCTION__) << "loadToMemory:" << e;
			loadToMemory = false;
			load_AllKit_ToMemory();
		}
		else if (name == "SAVE FROM MEMORY" && saveFromMemory)
		{
			ofLogNotice(__FUNCTION__) << "saveFromMemory:" << e;
			saveFromMemory = false;
			save_AllKit_FromMemory();
		}
		else if (name == "MODE MEMORY")
		{
			ofLogNotice(__FUNCTION__) << "MODE MEMORY: " << e;

			if (MODE_MemoryLive)
			{
				//reload all xml preset files to memory
				load_AllKit_ToMemory();
			}
			else
			{
				//save all xml preset files to disk from memory
				save_AllKit_FromMemory();
			}
		}

		//--

		//else if (name == "ENABLE KEYS")
		//{
		//}

		//--

		//		else if (name == "GUI ImGui POSITION")
		//		{
		//			ofLogVerbose(__FUNCTION__) << "GUI BROWSER POSITION: " << e;
		//
		//			////clamp inside window
		//			//float x, y;
		//			//x = ofClamp(ImGui_Position.get().x, 0, ofGetWidth() - 200);
		//			//y = ofClamp(ImGui_Position.get().y, 0, ofGetHeight() - 20);
		//			//ImGui_Position = glm::vec2(x, y);
		//		}
		//		else if (name == "GUI ImGui SIZE")
		//		{
		//			ofLogVerbose(__FUNCTION__) << "GUI BROWSER SIZE: " << e;
		//
		//			////clamp inside window
		//			//float x, y;
		//			//x = ofClamp(ImGui_Position.get().x, 0, ofGetWidth() - 200);
		//			//y = ofClamp(ImGui_Position.get().y, 0, ofGetHeight() - 20);
		//			//ImGui_Position = glm::vec2(x, y);
		//		}
		//--

		//--

		//all other widgets/params
		else
		{
			//check if changed prob sliders
			{
				bool doDices = false;
				for (int i = 0; i < presetsRandomFactor.size(); i++)
				{
					if (name == "PROB " + ofToString(i)) {
						doDices = true;//TODO: would be faster making return on first 'true'
					}
				}
				if (doDices)
				{
					//sum total dices/all probs
					dicesTotalAmount = 0;
					for (auto &p : presetsRandomFactor) {
						dicesTotalAmount += p.get();
					}
					randomizedDice.setMax(dicesTotalAmount - 1);

					ofLogNotice(__FUNCTION__) << "dicesTotalAmount: " << dicesTotalAmount;
				}
			}
		}
	}
}
#pragma mark - SETTINGS

//--------------------------------------------------------------
void ofxPresetsManager::load_ControlSettings()
{
	//--

	//control settings

	string path = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_ControlSettings;

	bool b = ofxSurfingHelpers::loadGroup(params_Control, path);

	ofLogNotice(__FUNCTION__) << "Loaded " << path << " " << (b ? "DONE" : "FAILED");
	if (!b)
	{
		ofLogError(__FUNCTION__) << "FILE '" << path << "' NOT FOUND!";
	}

	//--

	//ofLogNotice(__FUNCTION__) << "PRESET " << PRESET_Selected_IndexMain;

	//for (int i = 0; i < PRESETS_Selected_Index.size(); i++)
	//{
	//	ofLogNotice(__FUNCTION__) << "Selector Preset " << i << " : " << PRESETS_Selected_Index[i];
	//}

	//--

	//load randomizers settings

	string path2 = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_Randomizers;

	bool b2 = ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path2);

	ofLogNotice(__FUNCTION__) << "Loaded " << path2 << " " << (b2 ? "DONE" : "FAILED");
	if (!b2)
	{
		ofLogError(__FUNCTION__) << "FILE '" << path << "' NOT FOUND!";
	}

	//--

	//user settings

	//TODO:
	//testing to enable here to avoid create empty xml settings on global root?
	DISABLE_CALLBACKS = false;

	string path3 = filenameMainSettings;

	b = ofxSurfingHelpers::loadGroup(params_UserKitSettings, path3);

	if (!b) ofLogError(__FUNCTION__) << "CANT LOAD FILE '" << path3 << "'!";
	else ofLogNotice(__FUNCTION__) << "LOADED " << path3;

	//--

	//TODO: update selectors
	for (int i = 0; i < PRESETS_Selected_Index.size(); i++)
	{
		if (i < PRESETS_Selected_Index_PRE.size())
			PRESETS_Selected_Index_PRE[i] = PRESETS_Selected_Index[i];
		else
			ofLogError(__FUNCTION__) << "Out of Range: PRESETS_Selected_Index_PRE '" << i << "'!";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::save_ControlSettings()
{
#ifndef DEBUG_BLOCK_SAVE_SETTINGS

	DISABLE_CALLBACKS = true;

	//---

	try//TODO: test to avoid crashes..?
	{
		//save randomizers settings
		ofLogVerbose(__FUNCTION__) << endl << params_Control.toString() << endl;

		string path = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_ControlSettings;
		bool b;

		b = ofxSurfingHelpers::saveGroup(params_Control, path);
		if (!b) ofLogError(__FUNCTION__) << "CANT SAVE FILE '" << path << "'!";
		else ofLogNotice(__FUNCTION__) << "SAVED " << path;

		//-

		//save randomizers settings
		string path2 = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_Randomizers;

		b = ofxSurfingHelpers::saveGroup(params_RandomizerSettings, path2);
		if (!b) ofLogError(__FUNCTION__) << "CANT SAVE FILE '" << path2 << "'!";
		else ofLogNotice(__FUNCTION__) << "SAVED " << path2;

		//--

		//user settings
		string path3 = filenameMainSettings;

		b = ofxSurfingHelpers::saveGroup(params_UserKitSettings, path3);
		if (!b) ofLogError(__FUNCTION__) << "CANT SAVE FILE '" << path3 << "'!";
		else ofLogNotice(__FUNCTION__) << "SAVED " << path3;

		//--
	}
	catch (int n)
	{
		ofLogError(__FUNCTION__) << "CATCH ERROR " << n << endl;
		throw;
	}

	//---

	DISABLE_CALLBACKS = false;
#else
	ofLogNotice(__FUNCTION__) << "[DEBUG] BLOCKED save_ControlSettings()";
#endif
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::setPath_UserKit_Folder(string folder)
{
	ofLogNotice(__FUNCTION__) << folder;

	path_UserKit_Folder = folder;

	CheckFolder(folder);

	//create the folders of each group into main presets folder 
	string _path;
	if (bSplitGroupFolders.get())
	{
		for (int i = 0; i < groups.size(); i++)
		{
			_path = path_UserKit_Folder + "/" + path_PresetsFavourites;//current kit-presets presets folder
			_path += "/" + groups[i].getName();//append group name

			CheckFolder(_path);
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::setPath_PresetsFavourites(string folder)
{
	ofLogNotice(__FUNCTION__) << folder;
	path_PresetsFavourites = folder;
	CheckFolder(path_UserKit_Folder + "/" + path_PresetsFavourites);
}

//--------------------------------------------------------------
void ofxPresetsManager::setPath_PresetsStandalone(string folder)
{
	ofLogNotice(__FUNCTION__) << folder;
	path_PresetsStandalone = folder;
	CheckFolder(path_UserKit_Folder + "/" + path_PresetsStandalone);
}

//--

//memory mode 
//(loaded from data vector instead of hd files)
//--------------------------------------------------------------
void ofxPresetsManager::save_AllKit_FromMemory()
{
	ofLogVerbose(__FUNCTION__);

	for (int i = 0; i < mainGroupPresetsXmls.size(); i++)
	{
		string strFolder;
		string strFile;
		string strPath;

		strFolder = path_UserKit_Folder + "/" + path_PresetsFavourites + "/";
		strFile = groups[0].getName() + filenamesPrefix + ofToString(i);
		strPath = strFolder + strFile;
		strPath += fileExtension;

		if (i < mainGroupPresetsXmls.size()) {
			mainGroupPresetsXmls[i].save(strPath);
		}
		else {
			ofLogError(__FUNCTION__) << "mainGroupPresetsXmls OUT OF RANGE";
		}

	}

	//debug params
	if (true)
	{
		for (int i = 0; i < mainGroupPresetsXmls.size(); i++)
		{
			ofLogNotice(__FUNCTION__) << "mainGroupPresetsXmls[" << i << "] " << ofToString(mainGroupPresetsXmls[i].toString());
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load_AllKit_ToMemory()
{
	ofLogNotice(__FUNCTION__);

	////TODO:
	//groupsMem.clear();
	//groupsMem.reserve(NUM_OF_PRESETS);
	//groupsMem.resize(NUM_OF_PRESETS);

	for (int i = 0; i < mainGroupPresetsXmls.size(); i++)
	{
		//TODO:
		//PROBLEM:
		//this is not working because all the groups of the vector are shallow copies,
		//so all are changing together
		//pre-create params inside the group not avoid emptyness
		//ofParameterGroup g = groups[0];
		//groupsMem[i] = g;

		//-

		string pathFolder;
		string pathFilename;
		string pathComplete;
		pathFolder = path_UserKit_Folder + "/" + path_PresetsFavourites + "/";
		pathFilename = groups[0].getName() + filenamesPrefix + ofToString(i);
		pathFilename += fileExtension;

		pathComplete = pathFolder + pathFilename;


		//load xml file
		ofXml settings;
		bool bLoaded = settings.load(pathComplete);

		//debug
		if (false)
		{
			ofLogNotice(__FUNCTION__) << "[" << i << "]";
			ofLogNotice(__FUNCTION__) << "File: " << pathComplete
				<< "\n" << ofToString(settings.toString());
		}

		//-

		////TODO:
		//ofDeserialize(settings, groupsMem[i]);
		//ofLogNotice(__FUNCTION__) << "ofParameterGroup: " << i << "\n" << ofToString(groupsMem[i]);

		if (bLoaded)
		{
			if (i < mainGroupPresetsXmls.size()) {
				mainGroupPresetsXmls[i] = settings;
			}
			else {
				ofLogError(__FUNCTION__) << "mainGroupPresetsXmls OUT OF RANGE";
			}
		}
	}

	ofLogNotice(__FUNCTION__) << "-------------------------------------------------------------------------------------------------------";

	//debug params
	if (false)
	{
		for (int i = 0; i < mainGroupPresetsXmls.size(); i++)
		{
			ofLogNotice(__FUNCTION__) << "mainGroupPresetsXmls[" << i << "] " << ofToString(mainGroupPresetsXmls[i].toString());
		}
	}
}

////--------------------------------------------------------------
//void ofxPresetsManager::addGroup_TARGET(ofParameterGroup &g)
//{
//	ofLogNotice(__FUNCTION__) << "addGroup_TARGET:" << "" << endl;
//
//	group_TARGET = g;
//}

//--------------------------------------------------------------
void ofxPresetsManager::exit()
{
	ofLogVerbose(__FUNCTION__);

	DISABLE_CALLBACKS = true;

	//destroy callbacks
	removeKeysListeners();
	ofRemoveListener(params_UserKitSettings.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_UserKit);
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);
	ofRemoveListener(params_Randomizer.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);
	ofRemoveListener(params_Editor.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Editor);

	//TODO: required?
	ofRemoveListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofRemoveListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	//--

	//main group

	////autosave preset
	//if (autoSave)
	//{
	//	//doSave(PRESET_Selected_IndexMain);
	//	save(PRESET_Selected_IndexMain, 0);
	//}

	//-

	//TODO:
	//MODE B: direct from memory
	if (MODE_MemoryLive && autoSave)
	{
		save_AllKit_FromMemory();//update of files is required bc presets are on memmory only
	}

	//--

	//autosave all group presets
	if (autoSave)
	{
		//seve selected presets
		for (int g = 0; g < groups.size(); g++)//iterate each group
		{
			save(PRESETS_Selected_Index[g], g);
		}
	}

	////populate
	//for (int g = 0; g < groups.size(); g++)//iterate each group
	//{
	//	for (int p = 0; p < groupsSizes[g]; p++)//save each preset on each group
	//	{
	//		save(p, g);
	//	}
	//}

	//--

	//app settings
	save_ControlSettings();

	//--

	//TODO: debug crash
	//gui_ImGui.exit();
}

//--

#pragma mark - IM_GUI

#ifndef MODE_ImGui_EXTERNAL
//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Setup()
{
	ofLogNotice(__FUNCTION__);

	//--

	//font customize
#ifndef MODE_ImGui_EXTERNAL
#ifdef INCLUDE_IMGUI_CUSTOM_FONT
	ofxSurfingHelpers::ImGui_FontCustom();
#endif
#endif

	//--

	gui_ImGui.setup();

	//--

	//theme
#ifndef MODE_ImGui_EXTERNAL
	ofxSurfingHelpers::ImGui_ThemeModernDark();
	//ofxSurfingHelpers::ImGui_ThemeMoebiusSurfing();

	//--

	//mouse over
	ImGui::GetIO().MouseDrawCursor = false;
	bImGui_mouseOver.set("mouseOverGui", false);
	//ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
#endif

	//--
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_WindowBegin()
{
	//mouse over gui system
	bMouseOver_Changed = false;
	bImGui_mouseOver = false;

	//-

#ifndef USE_ofxImGuiSimple
	gui_ImGui.begin();
#else
	gui_ImGui.begin();
	ImGui::ShowDemoWindow();
	//TEST: params helpers..
	ofxImGui::AddParameter(PRESET_Selected_IndexMain);
#endif

	//-
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_WindowEnd()
{
	if (bImGui_mouseOver != bImGui_mouseOver_PRE)
	{
		bImGui_mouseOver_PRE = bImGui_mouseOver;
		bMouseOver_Changed = true;
	}

	if (bMouseOver_Changed)
	{
		ofLogVerbose(__FUNCTION__) << "bImGui_mouseOver: " << (bImGui_mouseOver ? "INSIDE" : "OUTSIDE");
	}

	//--

#ifndef USE_ofxImGuiSimple
	gui_ImGui.end();
#else
	gui_ImGui.end();
#endif
}

//--------------------------------------------------------------
bool ofxPresetsManager::ImGui_Draw_Window()
{
	auto mainSettings = ofxImGui::Settings();
	ofVec2f _pos;
	ofVec2f _size;

	_pos.set(ImGui_Position.get().x, ImGui_Position.get().y);
	_size.set(ImGui_Size.get().x, ImGui_Size.get().y);

	mainSettings.windowPos = _pos;//required
	mainSettings.windowSize = _size;
	auto _mode = ImGuiCond_FirstUseEver;//ImGuiCond_Always;
	ImGui::SetNextWindowPos(ofVec2f(_pos.x, _pos.y), _mode);
	ImGui::SetNextWindowSize(ofVec2f(_size.x, _size.y), _mode);

	//--

	//User-Kit name
	string _name = "ofxPresetsManager : " + displayNameUserKit;
	//string _name = "ofxPresetsManager : " + groups[0].getName();
	bool _collapse = false;

	//--

	//set window properties
	//bool show = true;
	static bool no_titlebar = false;
	static bool no_border = true;
	static bool no_resize = true;
	static bool no_move = true;
	static bool no_scrollbar = false;
	static bool no_collapse = true;
	static bool no_close = true;
	static bool no_menu = true;
	static bool no_settings = true;
	static float bg_alpha = -0.01f; // <0: default

	// Demonstrate the various window flags. 
	// Typically you would just use the default.
	ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoBackground;//make background transparent
	//if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
	//if (!no_border)   window_flags |= ImGuiWindowFlags_NoDecoration;
	//if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
	//if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
	//if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
	//if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
	//if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
	//if (no_settings)	window_flags |= ImGuiWindowFlags_NoSavedSettings;

	//----

	//A window: main (1st) group
	_collapse = true;
	if (ofxImGui::BeginWindow(_name, mainSettings, window_flags, &_collapse))
	{
		ImGui_Draw_WindowContent(mainSettings);

		//get window position/size: must be inside begin/end
		_pos = ImGui::GetWindowPos();
		_size = ImGui::GetWindowSize();

		ImGui_Position = glm::vec2(_pos.x, _pos.y);
		ImGui_Size = glm::vec2(_size.x, _size.y);
	}
	ofxImGui::EndWindow(mainSettings);

	//----

	//B window
	//window for all group parameters contained into presetsManager
	//all parameters in all added groups

	if (SHOW_ImGui_PresetsParams) {

		_pos = _pos - ofVec2f(410, 0);
		_size = ofVec2f(400, 600);

		mainSettings.windowPos = _pos;
		mainSettings.windowSize = _size;
		_mode = ImGuiCond_FirstUseEver;
		ImGui::SetNextWindowPos(ofVec2f(_pos.x, _pos.y), _mode);
		ImGui::SetNextWindowSize(ofVec2f(_size.x, _size.y), _mode);

		_name = "ofxPresetsManager";
		if (ofxImGui::BeginWindow(_name, mainSettings, window_flags, &_collapse))
		{
			ImGui_Draw_PresetPreview(mainSettings);
		}
		ofxImGui::EndWindow(mainSettings);
	}

	//----

	//C window
	//each group selectors 
	//window for each group selector

	if (SHOW_ImGui_Selectors) {

		_pos = _pos - ofVec2f(310, 0);
		_size = ofVec2f(300, 300);

		mainSettings.windowPos = _pos;
		mainSettings.windowSize = _size;
		_mode = ImGuiCond_FirstUseEver;
		ImGui::SetNextWindowPos(ofVec2f(_pos.x, _pos.y), _mode);
		ImGui::SetNextWindowSize(ofVec2f(_size.x, _size.y), _mode);

		_name = "SELECTORS";
		if (ofxImGui::BeginWindow(_name, mainSettings, window_flags, &_collapse))
		{
			ImGui_Draw_Selectors(mainSettings);
		}
		ofxImGui::EndWindow(mainSettings);
	}

	//----

	return mainSettings.mouseOverGui;
}

#endif

//ImGui pure content
//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_PresetPreview(ofxImGui::Settings &settings)
{
	if (ofxImGui::BeginTree("PRESETS PARAMETERS", settings))
	{
		for (int i = 0; i < groups.size(); i++) {
			ofxImGui::AddGroup(groups[i], settings);
		}

		ofxImGui::EndTree(settings);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Selectors(ofxImGui::Settings &settings)
{
	//0.1 sliders preset selectors
	//for (int i = 0; i < PRESETS_Selected_Index.size(); i++)
	//{
	//	ofxImGui::AddParameter(PRESETS_Selected_Index[i]);
	//}

	ofxImGui::AddGroup(params_PRESETS_Selected, settings);
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_MainPanel(ofxImGui::Settings &settings)
{
	if (ofxImGui::BeginTree("MAIN CONTROL", settings))
	{
		//---

		////0.label
		//ImGui::Text("PRESET");

		////User-Kit name
		//string str = "User-Kit: " + displayNameUserKit;
		//ImGui::Text(str.c_str());

		//---

		//ofxSurfingHelpers::AddBigToggle(MODE_Editor, 30);//TODO: repair
		ofxImGui::AddParameter(MODE_Editor);

		//ImGui::SameLine();

		ofxImGui::AddParameter(SHOW_ImGui_PresetsParams);
		if (bBuildGroupSelector) ofxImGui::AddParameter(SHOW_ImGui_Selectors);
		ofxImGui::AddParameter(SHOW_ClickPanel);

		//---

		////1. preset selector slider

		//auto parameter = PRESET_Selected_IndexMain;
		//auto tmpRef = PRESET_Selected_IndexMain.get();
		//auto name = ofxImGui::GetUniqueName(PRESET_Selected_IndexMain);
		//if (ImGui::SliderInt(ofxImGui::GetUniqueName(parameter), (int *)&tmpRef, parameter.getMin(), parameter.getMax()))
		//{
		//	parameter.set(tmpRef);
		//}

		//ofxImGui::AddParameter(PRESET_Selected_IndexMain);//main preset selector

		//--

		////big toggle button
		//ofxSurfingHelpers::AddBigToggle(PLAY_RandomizeTimer, 30);

		//--

		//ImGui::TreePop();
		ofxImGui::EndTree(settings);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Basic(ofxImGui::Settings &settings)
{
	//----

	//if (ImGui::TreeNode("BASIC"))
	if (ofxImGui::BeginTree("BASIC", settings))
	{
		//-

		if (ImGui::TreeNode("HELPERS"))
		{
			// main helpers
			if (ImGui::Button("CLONE ALL"))
			{
				bCloneAll = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("CLONE >"))
			{
				bCloneRight = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("POPULATE!"))
			{
				// populate all favs
				doPopulateFavs();
				// create browser files too
				doGetFavsToFilesBrowser();
			}

			//-

			ofxImGui::AddParameter(autoSave); ImGui::SameLine();
			//ofxImGui::AddParameter(autoLoad);//ImGui::SameLine();
			ofxImGui::AddParameter(MODE_MemoryLive);

			//-

			ofxImGui::AddParameter(SHOW_Help); ImGui::SameLine();
			ofxImGui::AddParameter(bThemDark);

			ImGui::TreePop();
		}

		//--

		// 2. panels toggles

		if (ImGui::TreeNode("EXTRA")) {
			ofxImGui::AddParameter(SHOW_ImGui);
			ofxImGui::AddParameter(SHOW_Gui_AdvancedControl);

			//-

			// 2. advanced

			if (SHOW_Gui_AdvancedControl)
			{
				// show ALL the addon params! mainly to debug..
				ofxImGui::AddGroup(params_Control, settings);
			}

			ImGui::TreePop();
		}

		//--

		if (MODE_Editor)
		{
			if (ImGui::TreeNode("USER-KIT")) {

				// User-Kit name
				string str;
				str = "User-Kit:";
				ImGui::Text(str.c_str());
				str = displayNameUserKit;
				ImGui::Text(str.c_str());

				// button to Open File Dialog as folder
				if (ImGui::Button("Set Custom folder")) {

					ofFileDialogResult openFileResult = ofSystemLoadDialog("Select User-Kit folder", true, ofToDataPath(path_UserKit_Folder, true));

					// Check if the user opened a file
					if (openFileResult.bSuccess) {

						ofLogNotice(__FUNCTION__) << ("User selected a folder");

						//We have a file, check it and process it
						doFileDialogProcessSelection(openFileResult);
					}
					else {
						ofLogNotice(__FUNCTION__) << ("User hit cancel");
					}
				}

				// monitor custom state
				//ofxImGui::AddParameter(bPathDirCustom);

				ImGui::TreePop();
			}
		}

		//-

		ofxImGui::EndTree(settings);
	}
	//ImGui::TreePop();
}

//--------------------------------------------------------------
void ofxPresetsManager::buildHelpInfo() {
	// build help info
	helpInfo = "";
	helpInfo += "USER-KIT: " + displayNameUserKit + "\n";
	helpInfo += "PATHS: ";
	helpInfo += getGroupsPaths();
	helpInfo += "\n";

	// TODO:
	//bool bKeysinfo = false;
	//if (!bKeysinfo)
	{
		helpInfo += "MOUSE&KEYS LOAD\n";
		helpInfo += "CTRL SAVE/COPY\n";
		helpInfo += "ALT  SWAP";
	}
	//else
	//{
	//	//keys[i][k]
	//	helpInfo += "[keys " + ofToString((char)keys[0][0]) + "|";
	//	helpInfo += ofToString((char)keys[0][keys[0].size() - 1]) + "]";
	//}
}

//--------------------------------------------------------------
void ofxPresetsManager::buildDefaultUserKit() {
	setPath_UserKit_Folder(path_UserKit_Folder);// overwrites the path name but also creates folders if required

	//displayNameUserKit = path_UserKit_Folder;

	//--

	// browser for standalone presets
	buildStandalonePresets();

	//--

	// reduce path to name only (to display purposes only)
	string str = path_UserKit_Folder;
	string temp = R"(\)";//use '\' as splitter...should use '/' too bc Windows/macOS compatibility..
	auto ss = ofSplitString(path_UserKit_Folder, temp);
	displayNameUserKit = ss[ss.size() - 1];

	//--

	// build help info
	buildHelpInfo();

	//--

	// radomizers
	buildRandomizers();
}

// TODO:
//--------------------------------------------------------------
void ofxPresetsManager::buildCustomUserKit() {

	setPath_UserKit_Folder(pathDirCustom);//creates subfolders for each group

	//-

	//reduce to name only
	string str = path_UserKit_Folder;
	std::string temp = R"(\)";//use '\' as splitter...should use '/' too bc Windows/macOS compatibility..
	auto ss = ofSplitString(path_UserKit_Folder, temp);
	displayNameUserKit = ss[ss.size() - 1];//get last word and use as name

	//-

	//TODO:
	//load standalone presets for browser gui
	buildStandalonePresets();

	//-

	//load randomizers settings
	string path2;
	bool b2;
	path2 = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_Randomizers;
	b2 = ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path2);
	ofLogNotice(__FUNCTION__) << "Loaded " << path2 << " " << (b2 ? "DONE" : "FAILED");	//--

	//build help info
	buildHelpInfo();

	//--

	//radomizers
	buildRandomizers();
}

//--------------------------------------------------------------
void ofxPresetsManager::doFileDialogProcessSelection(ofFileDialogResult openFileResult) {

	ofLogNotice(__FUNCTION__) << ("getName(): " + openFileResult.getName());
	ofLogNotice(__FUNCTION__) << ("getPath(): " + openFileResult.getPath());

	//--

	//autosave all group presets
	if (autoSave)
	{
		//seve selected presets
		for (int g = 0; g < groups.size(); g++)//iterate each group
		{
			save(PRESETS_Selected_Index[g], g);
		}
	}

	//--

	//save randomizers settings
	string path2;
	bool b2;
	path2 = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_Randomizers;

	b2 = ofxSurfingHelpers::saveGroup(params_RandomizerSettings, path2);

	ofLogNotice(__FUNCTION__) << "Saved " << path2 << " " << (b2 ? "DONE" : "FAILED");

	//--

	DISABLE_CALLBACKS = true;//TODO:
	bPathDirCustom = true;
	DISABLE_CALLBACKS = false;

	pathDirCustom = openFileResult.getPath();

	//--

	buildCustomUserKit();

	//--

	//autoload all group presets
	if (autoLoad)
	{
		//must disable autosave to avoid overwrite between user-kits
		bool bPre = autoSave;
		autoSave = false;

		//set selected first preset o last/main group
		int _last = groups.size() - 1;
		PRESETS_Selected_Index[_last] = 0;
		load(PRESETS_Selected_Index[_last], _last);
		autoSave = bPre;

		////load all group selected presets
		//for (int g = 0; g < groups.size(); g++)//iterate each group
		//{
		//	load(PRESETS_Selected_Index[g], g);
		//}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Browser(ofxImGui::Settings &settings)
{
	if (ofxImGui::BeginTree("FILES BROWSER", settings))
	{
		int _numfiles = fileNames.size();

		//--

		if (ImGui::TreeNode("STANDALONE PRESETS"))
		{
			//new button
			ofxImGui::AddParameter(MODE_Browser_NewPreset);

			//-

			//0. send/save current browsed (from "/archive/") preset to current presets on favorites

			//get/copy all favs presets from favs and send/save to browser folder ("archive")

			if (ImGui::Button("FROM FAVS"))
			{
				ofLogNotice(__FUNCTION__) << "FROM FAVS";
				doGetFavsToFilesBrowser();
			}

			//-

			ImGui::SameLine();
			if (ImGui::Button("TO FAVS"))
			{
				ofLogNotice(__FUNCTION__) << "TO FAVS: SAVE BROWSED PRESET: " << displayNamePreset;
				ofLogNotice(__FUNCTION__) << "TO FAVORITES PRESET: [" << PRESET_Selected_IndexMain << "]";

				if (MODE_Browser_NewPreset)
				{
					save(PRESET_Selected_IndexMain, 0);
				}
			}

			//---

			//label for User-Kit folder
			string str = "";
			str += path_UserKit_Folder;
			ImGui::Text(str.c_str());

			//-

			//blink error when no files detected on folder

			if (_numfiles == 0)
			{
				int n = 30;
				float a = ofMap(ofGetFrameNum() % n, 0, n, 0.f, 1.f);
				ImGui::PushID(1);
				ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.5, 0.0f, 0.5f, a));
				ImGui::Text("DIR OR FILES NOT FOUND!");
				string browser_path = path_UserKit_Folder + "/" + path_PresetsStandalone;
				const char *array = browser_path.c_str();
				ImGui::Text(array);
				ImGui::PopStyleColor(1);
				ImGui::PopID();
			}

			else if (_numfiles > 0)
			{
				//1. arrow buttons

				static int counter = currentFile;
				float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
				ImGui::PushButtonRepeat(true);

				//--

				//1.1 prev file

				if (ImGui::ArrowButton("##left", ImGuiDir_Left))
				{
					if (counter > 0)
					{
						counter--;
						currentFile = counter;
						if (currentFile < files.size())
						{
							displayNamePreset = fileNames[currentFile];
							ofLogNotice(__FUNCTION__) << "ARROW: displayNamePreset: [" + ofToString(currentFile) + "] " << displayNamePreset;
							//if (MODE_Browser_NewPreset)
							ofLogNotice(__FUNCTION__) << "LOAD" << endl;
							ofLogNotice(__FUNCTION__) << "PRESET NAME: " << displayNamePreset;
							doLoadMainGroupPreset(displayNamePreset);
						}
					}
				}

				//--

				//1.2 next file

				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##right", ImGuiDir_Right))
				{
					if (counter < files.size() - 1)
					{
						counter++;
						currentFile = counter;
						if (currentFile < files.size())
						{
							displayNamePreset = fileNames[currentFile];
							ofLogNotice(__FUNCTION__) << "ARROW: displayNamePreset: [" + ofToString(currentFile) + "] " << displayNamePreset;

							//if (MODE_Browser_NewPreset)
							ofLogNotice(__FUNCTION__) << "LOAD PRESET NAME: " << displayNamePreset;
							doLoadMainGroupPreset(displayNamePreset);
						}
					}
				}
				ImGui::PopButtonRepeat();

				//--

				//1.3 text preview current preset index to total amount.

				ImGui::SameLine();
				ImGui::Text("%d/%d", currentFile, _numfiles - 1);
			}

			//--

			//3. scrollable filenames list

			if (!fileNames.empty())
			{
				ImGui::SetNextItemWidth(140);

				int currentFileIndex = currentFile;
				if (ofxImGui::VectorCombo(" ", &currentFileIndex, fileNames))
				{
					ofLogNotice(__FUNCTION__) << "Preset Index: " << ofToString(currentFileIndex);
					if (currentFileIndex < fileNames.size())
					{
						currentFile = currentFileIndex;
						displayNamePreset = fileNames[currentFile];

						ofLogNotice(__FUNCTION__) << "LOAD Preset Name: " << displayNamePreset;
						doLoadMainGroupPreset(displayNamePreset);
					}
				}
			}

			//--

			//4.2 update

			if (ImGui::Button("UPDATE"))
			{
				//displayNamePreset = textInput_temp;
				//ofLogNotice(__FUNCTION__) << "UPDATE PRESET NAME: " << displayNamePreset << endl;

				//0. get filename of selected
				string _currName = files[currentFile].getBaseName();
				ofLogNotice(__FUNCTION__) << "UPDATE Preset Name: " << _currName;

				//1. delete old file
				files[currentFile].remove();

				//2. save "ovewrite"
				doStandalonePresetSave(_currName);

				//workflow
				//3. refresh files
				doStandaloneRefreshPresets();

				//4. reselect last save preset (bc directory sorting changes)
				ofLogNotice(__FUNCTION__) << "Reload last updated preset:";
				int iNew = -1;//search index for filename
				for (size_t i = 0; i < files.size(); i++)
				{
					string n = files[i].getBaseName();
					if (n == _currName)
					{
						iNew = i;
					}
				}
				if (iNew != -1)
				{
					ofLogNotice(__FUNCTION__) << "Index [" << iNew << "] " << files[iNew].getBaseName();
					currentFile = iNew;
					displayNamePreset = fileNames[currentFile];
					doLoadMainGroupPreset(displayNamePreset);
				}
				else
				{
					ofLogError(__FUNCTION__) << "Not found! Bad Index [" << iNew << "]";
				}
			}

			//-

			//4.3 reload

			ImGui::SameLine();
			if (ImGui::Button("RELOAD"))
			{
				ofLogNotice(__FUNCTION__) << "RELOAD Preset Name: " << displayNamePreset;
				doLoadMainGroupPreset(displayNamePreset);
			}

			//-

			//4.4 delete

			ImGui::SameLine();
			if (ImGui::Button("DELETE"))//current preset
			{
				ofLogNotice(__FUNCTION__) << "DELETE Preset Name: " << displayNamePreset;
				ofLogNotice(__FUNCTION__) << "file: " << files[currentFile].getAbsolutePath();

				//1. delete file
				files[currentFile].remove();

				//workflow
				//2. refresh files
				bool b = doStandaloneRefreshPresets();
				if (b)
				{
					currentFile = 0;
					displayNamePreset = fileNames[currentFile];
					doLoadMainGroupPreset(displayNamePreset);
				}
				else
				{
					ofLogError(__FUNCTION__) << "Error listing directory!";
				}
			}

			//-

			//4.5 clear. delete all!

			ImGui::SameLine();
			if (ImGui::Button("CLEAR"))//delete all files
			{
				ofLogNotice(__FUNCTION__) << "CLEAR Presets folder: " << path_UserKit_Folder + "/" + path_PresetsStandalone;

				for (int i = 0; i < files.size(); i++) {
					ofLogWarning(__FUNCTION__) << "DELETE file: " << files[i].getAbsolutePath();

					//1. delete file
					bool b = files[i].remove();
					if (!b) ofLogError(__FUNCTION__) << "Can not DELETE file: " << files[i].getAbsolutePath();
				}

				//workflow
				//2. refresh files
				bool b = doStandaloneRefreshPresets();
				if (b)
				{
					currentFile = 0;
					displayNamePreset = fileNames[currentFile];
					doLoadMainGroupPreset(displayNamePreset);
				}
				else
				{
					ofLogError(__FUNCTION__) << "Error listing directory!";
				}
			}

			//----

			//mode new preset enabled

			//5. second panel

			if (MODE_Browser_NewPreset)
			{
				//5.1 new preset name

				//ImGui::Text("NEW PRESET:");

				//-

				//5.2 user input text

				//loaded string into char array
				char tab[32];
				strncpy(tab, textInput_New.c_str(), sizeof(tab));
				tab[sizeof(tab) - 1] = 0;

				if (ImGui::InputText("", tab, IM_ARRAYSIZE(tab)))
				{
					ofLogNotice(__FUNCTION__) << "InputText [tab]:" << ofToString(tab) << endl;
					textInput_New = ofToString(tab);
					ofLogNotice(__FUNCTION__) << "textInput_New:" << textInput_New << endl;

					//bBlink = true;//not workind. we like to blink when mouse_on_text_input
				}

				//--

				//5.3 save new

				//workflow
				//TODO:
				//blink when it's editing a new preset..
				bool bBlink;
				bBlink = true;
				if (bBlink)
				{
					//ImGui::PushID(1);
					int n = 30;
					float a = ofMap(ofGetFrameNum() % n, 0, n, 0.2f, 0.5f);
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5, 0.0f, 0.5f, a));
				}

				if (ImGui::Button("SAVE"))
				{
					ofLogNotice(__FUNCTION__) << "textInput_New: " << textInput_New << endl;

					//1. save
					doStandalonePresetSave(textInput_New);

					//workflow
					//2. disable new preset mode
					MODE_Browser_NewPreset = false;

					//3. refresh files
					doStandaloneRefreshPresets();

					//4. reselect last save preset (bc directory sorting changes)
					ofLogNotice(__FUNCTION__) << "Reload last saved preset:";
					int iNew = -1;
					for (size_t i = 0; i < files.size(); i++)
					{
						string n = files[i].getBaseName();
						if (n == textInput_New)
						{
							iNew = i;
						}
					}
					ofLogNotice(__FUNCTION__) << "Index [" << iNew << "] " << files[iNew].getBaseName();
					currentFile = iNew;
					doLoadMainGroupPreset(textInput_New);
				}

				if (bBlink)
				{
					ImGui::PopStyleColor(1);
				}
			}
			ofxImGui::EndTree(settings);
		}

		ImGui::TreePop();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_WindowContent(ofxImGui::Settings &settings)
{
	//0. tittle
	//ImGui::Text("PRESETS MANAGER");
	//ImGui::NewLine();

	//main panel
	ImGui_Draw_MainPanel(settings);

	//1. basic controls
	if (MODE_Editor) ImGui_Draw_Basic(settings);

	////2. preset params preview
	//ImGui_Draw_PresetPreview(settings);

	//3. advanced params
	ImGui_Draw_GroupRandomizers(settings);

	//4. browser params
	if (MODE_Editor) {
		ImGui_Draw_Browser(settings);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_GroupRandomizers(ofxImGui::Settings &settings)
{
	//1. randomizers

	//if (ImGui::TreeNode("RANDOMIZERS"))
	if (ofxImGui::BeginTree("GROUP RANDOMIZERS", settings))
	{
		//---

		// preset selector

		// main group
		if (bBuildGroupSelector) ofxImGui::AddParameter(PRESETS_Selected_Index[groups.size() - 1]);

		//--

		//group 0 

		string str = groups[0].getName();
		//string str = "User-Kit: " + displayNameUserKit;
		ImGui::Text(str.c_str());

		//--

		//preset clicker matrix buttons
		//if (ImGui::TreeNode("Test"))
		{

			//toggle button matrix
			ImVec2 button_sz(40, 40);
			//Manually wrapping
			//(we should eventually provide this as an automatic layout feature, but for now you can do it manually)
			//ImGui::Text("PRESET SELECTOR:");
			ImGuiStyle& style = ImGui::GetStyle();
			int _amtButtons = mainGroupAmtPresetsFav;
			float _windowVisible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			for (int n = 0; n < _amtButtons; n++)
			{
				ImGui::PushID(n);
				string name = ofToString((char)(keys[0][n]));
				//customize colors
				{
					if (PRESET_Selected_IndexMain.get() == n)//when selected
					{
						const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonHovered];//changes the color
						ImGui::PushStyleColor(ImGuiCol_Button, colorActive);
					}
					else {
						const ImVec4 colorButton = style.Colors[ImGuiCol_Button];//do not changes the color
						ImGui::PushStyleColor(ImGuiCol_Button, colorButton);
					}
					//draw button
					if (ImGui::Button(name.c_str(), button_sz))
					{
						loadPreset(n);//trig load preset
					}
					//customize colors
					ImGui::PopStyleColor();
				}
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
				if (n + 1 < _amtButtons && next_button_x2 < _windowVisible_x2) ImGui::SameLine();
				ImGui::PopID();
			}
		}
		//ImGui::TreePop();

		//ImGui::Spacing(0, 10.0f);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//--

		//1. presets randomizers

		ImGuiStyle *style = &ImGui::GetStyle();

		//--

		//1.0.1 play randomizer index
		ofxSurfingHelpers::AddBigToggle(PLAY_RandomizeTimer, 30);

		//-

		//1.0.1B bpm slider
		auto parameter = randomizeDurationBpm;
		auto tmpRef = randomizeDurationBpm.get();
		auto name = ofxImGui::GetUniqueName(randomizeDurationBpm);
		if (ImGui::SliderFloat(ofxImGui::GetUniqueName(parameter), (float *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
		}

		//--

		////1.0.2 draw progress bar for the randomizer timer

		ImGui::PushID("prog");
		const ImVec4 color = style->Colors[ImGuiCol_ButtonHovered];//we can force change this color on theme... only used here
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
		//ImGui::ProgressBar(randomizerProgress.get());
		ImGui::ProgressBar(_prog);
		ImGui::PopStyleColor();
		ImGui::PopID();
		//cout << _prog << endl;

		//--

		//1.0.3 bang randomize
		//AddBigButton(bRandomizeIndex, 25);//index
		ofxSurfingHelpers::AddBigButton(bRandomizeEditor, 25);//preset

		//float w = ImGui::GetWindowWidth();
		//style->FramePadding = ImVec2(4, 2);
		//style->ItemSpacing = ImVec2(6, 4);
		//style->ItemInnerSpacing = ImVec2(6, 4);
		//style->IndentSpacing = 6.0f;
		//style->ItemInnerSpacing = ImVec2(2, 4);
		//float wHalf = (ImGui::GetWindowWidth() * 0.5f) - ( 4 + 6 + 6 +4);

		if (MODE_Editor) {
			float wHalf = 100;
			if (ImGui::Button("UNDO", ImVec2(wHalf, 20)))
			{
				ofLogNotice(__FUNCTION__) << "UNDO <-";
				undoStringParams.undo();
				undoRefreshParams();
			}
			ImGui::SameLine();
			if (ImGui::Button("REDO", ImVec2(wHalf, 20)))
			{
				ofLogNotice(__FUNCTION__) << "REDO ->";
				undoStringParams.redo();
				undoRefreshParams();
			}
			//string str = "";
			//str += ofToString(undoStringParams.getUndoLength()) + "/";
			//str += ofToString(undoStringParams.getRedoLength());
			//ImGui::Text(str.c_str());
		}

		//--

		//1.1 randomizers presets
		//const bool b = true;
		//settings.windowBlock = false;//-> this hides next group
		//ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing);//not working..
		//ImGui::SetNextWindowCollapsed(true, ImGuiCond_Always);
		ofxImGui::AddGroup(params_Randomizer, settings);

#ifdef DEBUG_randomTest
		ImGui::Text("%d/%d", randomizedDice.get(), randomizedDice.getMax());
#endif
		//--

		//1.2 randomizers editor
		if (MODE_Editor) {
			ofxImGui::AddGroup(params_Editor, settings);
		}

		//-

		ofxImGui::EndTree(settings);
		//ImGui::TreePop();
	}
}

//standalone presets browser
//--------------------------------------------------------------
void ofxPresetsManager::doStandalonePresetSave(string name)//without xml extension nor path
{
	ofLogNotice(__FUNCTION__) << name << fileExtension;

	string _path;
	_path = path_UserKit_Folder + "/" + path_PresetsStandalone + "/" + name;
	_path += fileExtension;

	bool b = ofxSurfingHelpers::saveGroup(groups[0], _path);
	if (!b)
	{
		ofLogError(__FUNCTION__) << "CAN'T SAVE FILE " << _path << "!";
	}
	else {
		ofLogNotice(__FUNCTION__) << "DONE SAVE " << _path;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doLoadMainGroupPreset(string name)//without xml extension nor path
{
	ofLogNotice(__FUNCTION__) << name << fileExtension;

	string _path;
	_path = path_UserKit_Folder + "/" + path_PresetsStandalone + "/" + name;
	_path += fileExtension;

	bool b = ofxSurfingHelpers::loadGroup(groups[0], _path);
	if (!b)
	{
		ofLogError(__FUNCTION__) << "CAN'T LOAD FILE " << _path << "!";
	}
	else {
		ofLogNotice(__FUNCTION__) << "DONE LOAD " << _path;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::buildStandalonePresets()//standalone presets splitted from favourites presets
{
	//load files structure directory
	bool bLoaded = doStandaloneRefreshPresets();

	//workflow
	if (bLoaded)
	{
		//load first preset
		if (fileNames.size() > 0)
		{
			currentFile = 0;
			displayNamePreset = fileNames[currentFile];

			////workflow
			//doLoadMainGroupPreset(displayNamePreset);
		}
	}
}

////--------------------------------------------------------------
//void ofxPresetsManager::doCheckPresetsFolderIsEmpty()
//{
//	string _path = path_UserKit_Folder + "/" + path_PresetsFavourites;
//	ofLogNotice(__FUNCTION__) << "Check that not empty folder at path: " << _path;
//	ofDirectory dataDirectory(ofToDataPath(_path, true));
//
//	//check if folder exist
//	if (!dataDirectory.isDirectory())
//	{
//		ofLogError(__FUNCTION__) << "FOLDER DOES NOT EXIST!";
//	}
//
//	//check if folder is empty
//	if (dataDirectory.size() == 0) {
//		ofLogNotice(__FUNCTION__) << "Folder " << _path << " is empty. Force populate favourites files...";
//
//		//populate all favs
//		doPopulateFavs();
//		//create browser files too
//		doGetFavsToFilesBrowser();
//	}
//
//	//verify if files are created
//	ofLogNotice(__FUNCTION__) << ofToString(dataDirectory.size()) << " file preset at folder " << _path;
//}

//--------------------------------------------------------------
bool ofxPresetsManager::doStandaloneRefreshPresets()
{
	//ofLogNotice(__FUNCTION__);

	string _path = path_UserKit_Folder + "/" + path_PresetsStandalone;
	CheckFolder(_path);

	ofLogNotice(__FUNCTION__) << "Path: " << _path;
	ofDirectory dataDirectory(ofToDataPath(_path, true));

	//-

	//TODO:
	//make above code to function
	//create folder if do not exist!
	if (!dataDirectory.isDirectory())
	{
		ofLogError(__FUNCTION__) << "FOLDER DOES NOT EXIST!";
		bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));
		if (b) ofLogNotice(__FUNCTION__) << "CREATED FOLDER: " << _path;
		else ofLogError(__FUNCTION__) << "UNABLE TO CREATE FOLDER: " << _path;
	}

	//-

	//clear files and filenames vectors
	files.clear();
	fileNames.clear();

	//load all folder files in one call
	files = dataDirectory.getFiles();

	ofLogNotice(__FUNCTION__) << "Preset files:";
	for (size_t i = 0; i < files.size(); i++)
	{
		fileNames.push_back(files[i].getBaseName());
		ofLogNotice(__FUNCTION__) << "[" << i << "] " << files[i].getBaseName();
	}

	//-

	//TODO
	//void to go to 1st...

	//1. load same position preset
	//if preset is deleted goes to nextone..
	//should check names because sorting changes..
	if (fileNames.size() > 0)
	{
		bFilesError = false;
	}
	else
	{
		ofLogError(__FUNCTION__) << "BROWSER: FILES NOT FOUND ON FOLDER!";
		bFilesError = true;

		////TODO:
		////disable custom path bc error
		//ofLogError(__FUNCTION__) << "Disable custom path: " << path_BrowserPathFree;
		//bCustomBrowserPath = false;
		//ofLogError(__FUNCTION__) << "------------------------";
	}

	//workflow
	////2. always goes to 1st preset 0
	////that's because saving re sort the files
	////and we don't know the position of last saved preset..
	//if (fileNames.size() > 0)
	//{
	//   currentFile = 0;
	//   displayNamePreset = fileNames[currentFile];
	//   doLoadMainGroupPreset(displayNamePreset);
	//}
	//else
	//{
	//   ofLogError("ofxColorManager") << "NOT FOUND ANY FILE PRESET!";
	//}

	return !bFilesError;//true if ok
}

//----

#ifdef INCLUDE_ofxUndoSimple
//--------------------------------------------------------------
void ofxPresetsManager::undoStoreParams() {
	undoXmlParams.clear();
	ofParameterGroup _group = groups[0];
	ofSerialize(undoXmlParams, _group);// fill the xml with the ofParameterGroup
	undoStringParams = (undoXmlParams.toString());// fill the ofxUndoSimple with the xml as string
	undoStringParams.store();

	string str = "";
	str += "UNDO HISTORY: " + ofToString(undoStringParams.getUndoLength()) + "/";
	str += ofToString(undoStringParams.getRedoLength());
	//str += "\n";
	//str += "DESCRIPTOR\n";
	//str += undoStringParams.getUndoStateDescriptor() + "\n";

	ofLogNotice(__FUNCTION__) << str;
}

//--------------------------------------------------------------
void ofxPresetsManager::undoRefreshParams() {
	undoXmlParams.clear();
	undoXmlParams.parse(undoStringParams);// fill the xml with the string 
	ofParameterGroup _group = groups[0];
	ofDeserialize(undoXmlParams, _group);// load the ofParameterGroup

	string str = "";
	str += "UNDO HISTORY: " + ofToString(undoStringParams.getUndoLength()) + "/";
	str += ofToString(undoStringParams.getRedoLength());
	//str += "\n";
	//str += "DESCRIPTOR\n";
	//str += undoStringParams.getUndoStateDescriptor() + "\n";

	ofLogNotice(__FUNCTION__) << str;
}
#endif

//----

#ifdef INCLUDE_RANDOMIZER
//--------------------------------------------------------------
int ofxPresetsManager::doRandomizeWichSelectedPresetCheckChanged()
{
	ofLogVerbose(__FUNCTION__);

	//-

	int _r = PRESET_Selected_IndexMain;

	if (MODE_DicesProbs) {

		// 1. dice randomize

#ifndef DEBUG_randomTest
		{
			// get a random between all possible dices (from 0 to dicesTotalAmount) and then select the preset associated to the resulting dice.
			// each preset has many dices: more dices 0 more probality to be selected by the randomizer
			randomizedDice = ofRandom(0, dicesTotalAmount);

			ofLogVerbose(__FUNCTION__) << "random: " << randomizedDice.get() << "/" << dicesTotalAmount;
		}
#endif

		//-

		// 2. define limits for range dices associated to any preset

		//randomFactorsDices[0] = 0;
		for (int i = 0; i < presetsRandomFactor.size(); i++) {
			randomFactorsDices[i] = 0;

			for (int f = 0; f < i; f++) {
				randomFactorsDices[i] += presetsRandomFactor[f];
			}
		}

		//-

		// 3. check if dice is inside both ranges. to select preset (_rr) associated to dice 

		int _rr = 0;
		//for (int i = 0; i < presetsRandomFactor.size(); i++) {
		for (int i = 0; i <= presetsRandomFactor.size(); i++) {

			// define upper/lower limits for each dices/preset
			int start;
			int end;

			if (i == 0) start = 0;
			else start = randomFactorsDices[i - 1];

			//TODO:
			//bug on last preset..

			//if (i == presetsRandomFactor.size() - 1)
			//	end = presetsRandomFactor.size()-1;
			//else
			//	end = randomFactorsDices[i];
			//end = randomFactorsDices[i];

			if (i != presetsRandomFactor.size()) end = randomFactorsDices[i];
			else end = dicesTotalAmount;

#ifdef DEBUG_randomTest
			ofLogNotice(__FUNCTION__) << (start == end ? "\t\t\t\t" : "") << "[" << i << "] " << start << "-" << end;
#endif
			// check if dice is inside both limits
			if (randomizedDice >= start && randomizedDice < end)
			{
				_rr = i - 1;
			}
		}
#ifdef DEBUG_randomTest
		ofLogNotice(__FUNCTION__) << "dice: " << randomizedDice << "  ->  index preset: [" << _rr << "]";
		//ofLogNotice(__FUNCTION__) << "preset: " << _rr + 1;
#endif
		_r = _rr;
		//_r = _rr + 1;

		//debug
		ofLogVerbose(__FUNCTION__) << "DEBUG";
		//for (int i = 0; i < presetsRandomFactor.size(); i++) {
		for (int i = 0; i < presetsRandomFactor.size(); i++) {
#ifdef DEBUG_randomTest
			ofLogNotice(__FUNCTION__) << "randomFactorsDices: [" << i << "] " << randomFactorsDices[i];
			// << "-" << randomFactorsDices[i + 1] << endl;
#endif
		}
		//last
#ifdef DEBUG_randomTest
		ofLogNotice(__FUNCTION__) << "randomFactorsDices: [" << presetsRandomFactor.size() << "] " << dicesTotalAmount;
#endif

		//for (int f = 1; f < presetsRandomFactor.size(); f++) {
		//	start = presetsRandomFactor[i - 1].get();
		//}
		//
		//if (randomizedDice >= presetsRandomFactor[i - 1].get() && randomizedDice < presetsRandomFactor[i].get()) {
		//}
		//}

		//for (auto &p : presetsRandomFactor) {
		//	dicesTotalAmount += p.get();
		//}
	}
	//else {
	//	int numTryes = 0;
	//	//avoid jump to same current preset
	//	while (PRESET_Selected_IndexMain == _r)//if not changed
	//	{
	//		ofLogWarning(__FUNCTION__) << "Randomize not changed! Try #" << ofToString(++numTryes);
	//		ofLogNotice(__FUNCTION__) << "PRESET Previous was : " << ofToString(_r);
	//		ofLogNotice(__FUNCTION__) << "PRESET New Random is: " << ofToString(PRESET_Selected_IndexMain);
	//		PRESET_Selected_IndexMain = (int)ofRandom(0, mainGroupPresetsXmls.size());
	//		
	//		//if (MODE_MemoryLive) _r = (int)ofRandom(0, mainGroupPresetsXmls.size());
	//		//_r = (int)ofRandom(1, mainGroupPresetsXmls.size() + 1);
	//	}
	//}

	return _r;
}

//--------------------------------------------------------------
void ofxPresetsManager::doRandomizeWichSelectedPreset()
{
	//ofLogNotice(__FUNCTION__);
	ofLogNotice(__FUNCTION__) << "---------------------------------------------------";

	// we avoid that the random is the same previous preset (TODO:improve). we want force change, not stay in the same. 
	// bc sometimes the random gets the same current preset.

	int _PRESET_selected_PRE = PRESET_Selected_IndexMain;

	int r = doRandomizeWichSelectedPresetCheckChanged();

	// TODO:
	// if there's only one posible dice.. cant avoid repetition. so force avoid toggle to false
	if (MODE_AvoidRandomRepeat && dicesTotalAmount < 2) MODE_AvoidRandomRepeat = false;

	if (MODE_AvoidRandomRepeat)
	{
		int numTryes = 0;
		ofLogNotice(__FUNCTION__) << "Randomize Try #" << ofToString(++numTryes);
		ofLogNotice(__FUNCTION__) << "PRESET Random is: " << ofToString(r);

		while (r == _PRESET_selected_PRE && dicesTotalAmount > 1 && numTryes < 5) //while preset index not changed. TODO: avoid make more than 5 randoms..
		{
			r = doRandomizeWichSelectedPresetCheckChanged();
			ofLogWarning(__FUNCTION__) << "Randomize Try #" << ofToString(++numTryes) << " NOT changed!";
			ofLogNotice(__FUNCTION__) << "PRESET Previous was : " << ofToString(_PRESET_selected_PRE);
			ofLogNotice(__FUNCTION__) << "PRESET New Random is: " << ofToString(r);
			ofLogWarning(__FUNCTION__) << "RETRY !";
		}
	}

	//--

	// 4. apply preset selection

	ofLogNotice(__FUNCTION__) << "PRESET > " << ofToString(r);
	loadPreset(r);

	//int __r = (int)ofRandom(1.0f, 9.0f);
	//ofLogNotice(__FUNCTION__) << "\t > " << ofToString(__r);
	//loadPreset(__r);

	//--

	// 5. start timer again

	if (PLAY_RandomizeTimer)
	{
		randomizerTimer = ofGetElapsedTimeMillis();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doResetDices()
{
	for (auto &p : presetsRandomFactor) {
		p = 0;
	}
	if (presetsRandomFactor.size() > 0) {
		presetsRandomFactor[0] = 1;
		dicesTotalAmount = 1;
		randomizedDice = 0;
	}
	else {
		dicesTotalAmount = 0;
	}
	randomizedDice.setMax(dicesTotalAmount - 1);
	randomizeDuration = 1000;
	randomizeDurationShort = randomizeDuration * 0.5;
}

//--------------------------------------------------------------
void ofxPresetsManager::addGroupToEditor(ofParameterGroup& group) {
	//editorPresets.clear();//?

	// TODO:
	// if we want to make nested folders (not all toggles into same and only one level, we need to create subgroups too...)

	for (auto parameter : group)
	{
		// group
		auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
		if (parameterGroup)
		{
			addGroupToEditor(*parameterGroup);
			continue;
		}

		// exclude params not marked as serializable
		if (parameter->isSerializable())
		{
			// parameter, try everything we know how to handle.

			// x,y,z vectors
#if OF_VERSION_MINOR >= 10
			auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
			if (parameterVec2f)
			{
				ofParameter<bool> b{ parameterVec2f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
			if (parameterVec3f)
			{
				ofParameter<bool> b{ parameterVec3f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
			if (parameterVec4f)
			{
				ofParameter<bool> b{ parameterVec4f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
#endif
			auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
			if (parameterOfVec2f)
			{
				//ofxImGui::AddParameter(*parameterOfVec2f);
				ofParameter<bool> b{ parameterOfVec2f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
			if (parameterOfVec3f)
			{
				//ofxImGui::AddParameter(*parameterOfVec3f);
				ofParameter<bool> b{ parameterOfVec3f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
			if (parameterOfVec4f)
			{
				ofParameter<bool> b{ parameterOfVec4f->getName(), false };
				editorPresets.push_back(b);
				//ofxImGui::AddParameter(*parameterOfVec4f);
				continue;
			}
			// colors
			auto parameterColor = std::dynamic_pointer_cast<ofParameter<ofColor>>(parameter);
			if (parameterColor)
			{
				ofParameter<bool> b{ parameterColor->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
			if (parameterFloatColor)
			{
				ofParameter<bool> b{ parameterFloatColor->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			// normal types
			auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
			if (parameterFloat)
			{
				ofParameter<bool> b{ parameterFloat->getName(), false };
				editorPresets.push_back(b);
				//ofxImGui::AddParameter(*parameterFloat);
				continue;
			}
			auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
			if (parameterInt)
			{
				ofParameter<bool> b{ parameterInt->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
			if (parameterBool)
			{
				ofParameter<bool> b{ parameterBool->getName(), false };
				editorPresets.push_back(b);
				continue;
			}

			ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter " << parameter->getName();
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doRandomizeEditor() {
	ofLogNotice(__FUNCTION__);

	//if(params_Editor_Toggles.size() == 0) params_Editor_Toggles

	ofParameterGroup _group = groups[0];
	doRandomizeEditorGroup(_group);

#ifdef INCLUDE_ofxUndoSimple
	if (MODE_Editor.get())
		undoStoreParams();// store scene
#endif
}

//--------------------------------------------------------------
void ofxPresetsManager::doRandomizeEditorGroup(ofParameterGroup& group) {
	for (auto parameter : group)
	{
		if (parameter->isSerializable())// avoid not serailizable params that will crash
		{
			// recursive..
			auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
			if (parameterGroup)
			{
				//cout << "parameterGroup: " << ofToString(parameterGroup->getName()) << endl;
				doRandomizeEditorGroup(*parameterGroup);
				continue;
			}

			auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
			if (parameterInt)
			{
				string name = parameterInt->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					int random = ofRandom(parameterInt->getMin(), parameterInt->getMax() + 1);
					parameterInt->set(random);
				}
				continue;
			}

			auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
			if (parameterFloat)
			{
				string name = parameterFloat->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					float random = ofRandom(parameterFloat->getMin(), parameterFloat->getMax());
					parameterFloat->set(random);
				}
				continue;
			}

			auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
			if (parameterBool)
			{
				string name = parameterBool->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					bool random = (ofRandom(0, 2) >= 1);
					parameterBool->set(random);
				}
				continue;
			}

			auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
			if (parameterFloatColor)
			{
				string name = parameterFloatColor->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					ofFloatColor random;
					random = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));
					parameterFloatColor->set(random);
				}
				continue;
			}

			auto parameterColor = std::dynamic_pointer_cast<ofParameter<ofColor>>(parameter);
			if (parameterColor)
			{
				string name = parameterColor->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					ofColor random;
					random = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));
					parameterColor->set(random);
				}
				continue;
			}

			//glm::vec compatible
#if OF_VERSION_MINOR >= 10
			auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
			if (parameterVec2f)
			{
				string name = parameterVec2f->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					float randomx = ofRandom(parameterVec2f->getMin().x, parameterVec2f->getMax().x);
					float randomy = ofRandom(parameterVec2f->getMin().y, parameterVec2f->getMax().y);
					parameterVec2f->set(glm::vec2(randomx, randomy));
				}
				continue;
			}
			auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
			if (parameterVec3f)
			{
				string name = parameterVec3f->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					float randomx = ofRandom(parameterVec3f->getMin().x, parameterVec3f->getMax().x);
					float randomy = ofRandom(parameterVec3f->getMin().y, parameterVec3f->getMax().y);
					float randomz = ofRandom(parameterVec3f->getMin().z, parameterVec3f->getMax().z);
					parameterVec3f->set(glm::vec3(randomx, randomy, randomz));
				}
				continue;
			}
			auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
			if (parameterVec4f)
			{
				string name = parameterVec4f->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					float randomx = ofRandom(parameterVec4f->getMin().x, parameterVec4f->getMax().x);
					float randomy = ofRandom(parameterVec4f->getMin().y, parameterVec4f->getMax().y);
					float randomz = ofRandom(parameterVec4f->getMin().z, parameterVec4f->getMax().z);
					float randomw = ofRandom(parameterVec4f->getMin().w, parameterVec4f->getMax().w);
					parameterVec4f->set(glm::vec4(randomx, randomy, randomz, randomw));
				}
				continue;
			}
#endif

			auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
			if (parameterOfVec2f)
			{
				string name = parameterOfVec2f->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					float randomx = ofRandom(parameterOfVec2f->getMin().x, parameterOfVec2f->getMax().x);
					float randomy = ofRandom(parameterOfVec2f->getMin().y, parameterOfVec2f->getMax().y);
					parameterOfVec2f->set(ofVec2f(randomx, randomy));
				}
				continue;
			}
			auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
			if (parameterOfVec3f)
			{
				string name = parameterOfVec3f->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					float randomx = ofRandom(parameterOfVec3f->getMin().x, parameterOfVec3f->getMax().x);
					float randomy = ofRandom(parameterOfVec3f->getMin().y, parameterOfVec3f->getMax().y);
					float randomz = ofRandom(parameterOfVec3f->getMin().z, parameterOfVec3f->getMax().z);
					parameterOfVec3f->set(ofVec3f(randomx, randomy, randomz));
				}
				continue;
			}
			auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
			if (parameterOfVec4f)
			{
				string name = parameterOfVec4f->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
				if (b.get())
				{
					float randomx = ofRandom(parameterOfVec4f->getMin().x, parameterOfVec4f->getMax().x);
					float randomy = ofRandom(parameterOfVec4f->getMin().y, parameterOfVec4f->getMax().y);
					float randomz = ofRandom(parameterOfVec4f->getMin().z, parameterOfVec4f->getMax().z);
					float randomw = ofRandom(parameterOfVec4f->getMin().w, parameterOfVec4f->getMax().w);
					parameterOfVec4f->set(ofVec4f(randomx, randomy, randomz, randomw));
				}
				continue;
			}

			ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter " << parameter->getName();
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::setupRandomizerEditor()
{
	params_randomizer.setName("RANDOMIZERS");
	editorPresets.clear();//?

	//int num = groups[0].getGroupHierarchyNames().size();
	ofParameterGroup group = groups[0];
	addGroupToEditor(groups[0]);//enqueue all content params and create a toggle for each one

	//add to group
	bRandomizeEditor.set("RANDOMIZE PRESET", false);
	bRandomizeEditorPopulateFavs.set("POPULATE FAVS!", false);
	bRandomizeEditorAll.set("ALL", false);
	bRandomizeEditorNone.set("NONE", false);

	params_Editor.clear();
	params_Editor.setName("PRESET EDIT");
	params_Editor.add(bRandomizeEditor);
	params_Editor.add(bRandomizeEditorAll);
	params_Editor.add(bRandomizeEditorNone);
	params_Editor.add(bRandomizeEditorPopulateFavs);
	params_Editor_Toggles.setName("PRESET PARAMETERS");
	params_Editor_Toggles.clear();

	for (auto &p : editorPresets) {
		params_Editor_Toggles.add(p);
	}
	params_Editor.add(params_Editor_Toggles);

	//exclude
	bRandomizeEditor.setSerializable(false);
	bRandomizeEditorPopulateFavs.setSerializable(false);
	bRandomizeEditorAll.setSerializable(false);
	bRandomizeEditorNone.setSerializable(false);

	//--

	////TODO: 
	//workflow
	{
		//if there are some toggles/params enabled to allow random, we do not do nothing
		//if there's no toggle/params enabled, we enable all params toggles to the editor 
		//so random will affect to all params by default

		//workflow
		bool someFalse = false;//none is false
		for (auto &p : editorPresets)
		{
			if (!p.get()) someFalse = true;//some is false
		}

		//if all toggles are disabled after loaded settings, 
		//then put all to true to allow populate, and random make something
		if (!someFalse) bRandomizeEditorAll = true;//someone is true
	}

	//--

	//callback
	ofAddListener(params_Editor.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Editor);
}

//--------------------------------------------------------------
void ofxPresetsManager::buildRandomizers()
{
	//radomizer
	setupRandomizer();

	//preset editor tools
	setupRandomizerEditor();

}

//--------------------------------------------------------------
void ofxPresetsManager::setupRandomizer()
{
	//select a random preset (from 1 to 8)
	//params_Randomizer.setName("Randomizer");
	bRandomizeIndex.set("RANDOMIZE INDEX", false);
	PLAY_RandomizeTimer.set("PLAY RANDOMIZER", false);
	PLAY_RandomizeTimer.setSerializable(false);
	MODE_DicesProbs.set("MODE USE PROBS/DICES", true);
	MODE_LatchTrig.set("MODE LATCH", false);
	MODE_AvoidRandomRepeat.set("MODE AVOID REPEAT", false);
	randomizeDuration.set("t DURATION", 1000, 10, randomize_MAX_DURATION);
	randomizeDurationShort.set("t SHORT", 250, 10, randomize_MAX_DURATION_SHORT);
	randomizeDurationBpm.set("t BPM", 120, 10, 400);
	randomizedDice.set("DICE", 0, 0, mainGroupAmtPresetsFav - 1);
	//randomizeSpeedF.set("SPEED FACTOR", 1.f, 0.01f, 2.f);
	bResetDices.set("RESET DICES", false);

	//exclude
	bRandomizeIndex.setSerializable(false);
	bResetDices.setSerializable(false);
	//randomizeDuration.setSerializable(false);

	//erase
	presetsRandomFactor.clear();
	presetsRandomModeShort.clear();
	randomFactorsDices.clear();
	//resize
	presetsRandomFactor.resize(mainGroupAmtPresetsFav);
	presetsRandomModeShort.resize(mainGroupAmtPresetsFav);
	randomFactorsDices.resize(mainGroupAmtPresetsFav);

	int i;

	//ints as probability for every preset
	ofParameterGroup _gOdds{ "PRESETS PROBS" };
	_gOdds.clear();
	i = 0;
	for (auto &p : presetsRandomFactor) {
		string n = "PROB " + ofToString(i++);
		p.set(n, 5, 0, 10);
		_gOdds.add(p);
	}

	//toggles to enable short duration mode
	ofParameterGroup _gShort{ "MODE DURATION SHORT" };
	_gShort.clear();
	i = 0;
	for (auto &p : presetsRandomModeShort) {
		string n = "SHORT " + ofToString(i++);
		p.set(n, false);
		_gShort.add(p);
	}

	params_Randomizer.clear();
	params_Randomizer.setName("FAVOURITES");
	//params_Randomizer.clear();//should split to avoid call again when setted custom path...
	params_Randomizer.add(PLAY_RandomizeTimer);
	params_Randomizer.add(bRandomizeIndex);
	params_Randomizer.add(randomizeDuration);
	params_Randomizer.add(randomizeDurationShort);
	params_Randomizer.add(randomizeDurationBpm);
	params_Randomizer.add(MODE_DicesProbs);
	params_Randomizer.add(MODE_LatchTrig);
	params_Randomizer.add(MODE_AvoidRandomRepeat);
	params_Randomizer.add(_gOdds);
	params_Randomizer.add(_gShort);
	params_Randomizer.add(bResetDices);
#ifdef DEBUG_randomTest
	params_Randomizer.add(randomizedDice);
#endif
}
#endif