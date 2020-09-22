#include "ofxPresetsManager.h"


//--------------------------------------------------------------
ofxPresetsManager::ofxPresetsManager()
{
	//-

#ifdef DEBUG_randomTest
	ofSetLogLevel("ofxPresetsManager", OF_LOG_VERBOSE);
#endif

	//-

#ifdef USE_XML
	fileExtension = ".xml";
#else
#ifdef USE_JSON
	fileExtension = ".json";
#endif
#endif

	//-

	DISABLE_CALLBACKS = true;

	//-

	//subscribed to auto run update and draw without required 'manual calls'
	ofAddListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofAddListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	//--

	// default settings paths

	displayNameUserKit = "ofxPresetsManager";

	//----

	filenameMainSettings = "ofxPresetsManagerSettings" + fileExtension;// default user preset name

	// top parent folder
	// root

	// default User-Kit folder (main root container!)
	path_UserKit_Folder = "ofxPresetsManager";

	// default kit folder for live/favourites presets
	path_PresetsFavourites = "presets";
	// big browser for standalone presets
	path_PresetsStandalone = "archive";
	// app settings
	path_ControlSettings = "appSettings";

	// randomizer settings for each kit
	filename_Randomizers = "_Randomizer";
#ifdef USE_XML
	filename_Randomizers += ".xml";
#else
#ifdef USE_JSON
	filename_Randomizers += ".json";
#endif
#endif

	// global app session settings
	filename_ControlSettings = "settingsControl";
#ifdef USE_XML
	filename_ControlSettings += ".xml";
#else
#ifdef USE_JSON
	filename_ControlSettings += ".json";
#endif
#endif

	// to add to all presets filenames
	filenamesPrefix = "_";//"_preset_";

	//// root folder
	//path_Root = "myAddon";

	//----

	// layout
	setSizeBox_PresetClicker(80);
	setPosition_PresetClicker(200, ofGetHeight() - getPresetClicker_Height() - 100);

	//--

	bKeys = false;
	keysNotActivated = true;
	lastMouseButtonState = false;
	modeKeySave = OF_KEY_CONTROL;
	bKeySave = false;
	modKeySwap = OF_KEY_ALT;
	bKeySwap = false;

	//--

	// reserve memory data

	groups.reserve(NUM_MAX_GROUPS);
	keys.reserve(NUM_MAX_GROUPS);
	PRESETS_Selected_Index.reserve(NUM_MAX_GROUPS);
	groupRandomizers.reserve(NUM_MAX_GROUPS);

	//--

	// callbacks to know when preset loading/save is done

	DONE_load.set("DONE LOAD", false);// callback to know (in ofApp) that preset LOAD is done
	DONE_save.set("DONE SAVE", false);// callback to know (in ofApp) that preset SAVE is done

	// easy callback: we can use too the easy isDoneLoad() to check in ofApp update() as kind of callback.

	//--

	// control parameters

	SHOW_ImGui.set("SHOW ImGui", false);
	SHOW_ImGui_PresetsParams.set("SHOW PARAMETERS", false);
	SHOW_ImGui_Selectors.set("SHOW SELECTORS", false);
	SHOW_Help.set("SHOW HELP", false);
	SHOW_Gui_AdvancedControl.set("SHOW ADVANCED", false);
	SHOW_ClickPanel.set("SHOW CLICK PANEL", true);
	SHOW_BrowserPanel.set("SHOW BROWSER PANEL", true);
	MODE_Browser_NewPreset.set("NEW!", false);
	ENABLE_Keys.set("ENABLE KEYS", true);

	autoLoad.set("AUTO LOAD", true);
	autoSave.set("AUTO SAVE", true);
	bAutosaveTimer.set("TIMER AUTO SAVE", false);
	MODE_MemoryLive.set("MODE MEMORY", false);
	loadToMemory.set("LOAD TO MEMORY", false);
	saveFromMemory.set("SAVE FROM MEMORY", false);

	displayNamePreset = "NO_NAME_PRESET";//browser loaded preset name

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

	// exclude from settings
	loadToMemory.setSerializable(false);
	saveFromMemory.setSerializable(false);
	//params_HelperTools.setSerializable(false);// only reset toggles inside
	//SHOW_Gui_AdvancedControl.setSerializable(false);

	//--

	// internal control params

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
	params_Gui.add(SHOW_BrowserPanel);
	params_Gui.add(SHOW_ImGui);
	params_Gui.add(SHOW_ImGui_PresetsParams);
	params_Gui.add(ENABLE_Keys);

	// layout
	params_Gui.add(ImGui_Position);
	params_Gui.add(ImGui_Size);
	params_Gui.add(Gui_Internal_Position);

	//--

	// all nested params for callbacks and storage settings
	params_Control.setName("ofxPresetsManager");
	ofAddListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Control);

	//--

	// gui font
	string str;
	str = "overpass-mono-bold.otf";
	myTTF = "assets/fonts/" + str;
	sizeTTF = 10;
	bool bLoaded = myFont.load(myTTF, sizeTTF, true, true);
	if (!bLoaded) bLoaded = myFont.load(OF_TTF_SANS, sizeTTF, true, true);

	//--

	// custom path:
	bPathDirCustom.set("MODE CUSTOM PATH", false);
	pathDirCustom.set("Path", "DEFAULT_DATA_PATH");

	//-

	// mainly to measure performance when using hd files vs faster memory vectors
#ifdef INCLUDE_PERFORMANCE_MEASURES
	// specify a target framerate
	//TIME_SAMPLE_SET_FRAMERATE(fps);
	//TIME_SAMPLE_ENABLE();
	TIME_SAMPLE_SET_AVERAGE_RATE(0.1);
	TIME_SAMPLE_SET_DRAW_LOCATION(TIME_SAMPLE_DRAW_LOC_BOTTOM_LEFT);
	TIME_SAMPLE_SET_PRECISION(4);
	TIME_SAMPLE_GET_INSTANCE()->setEnabled(true);
	TIME_SAMPLE_DISABLE();
#endif
}

//----

// setup methods

//--------------------------------------------------------------
void ofxPresetsManager::setup()
{
	ofLogNotice(__FUNCTION__);
	// filenameMainSettings will use default name "ofxPresetsManagerSettings.xml"
	setup(bAllowGroupSelector);// split folders by group name
}

//--------------------------------------------------------------
void ofxPresetsManager::setup(std::string name)// must be called after adding params group
{
	ofLogNotice(__FUNCTION__) << name;
	filenameMainSettings = name + fileExtension;
	setup(bAllowGroupSelector);
}

//--------------------------------------------------------------
void ofxPresetsManager::setup(std::string name, bool _buildGroupSelector)// must be called after adding params group
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

		for (int i = 0; i < PRESETS_Selected_Index.size(); i++)
		{
			params_GroupMainSelector.add(PRESETS_Selected_Index[i]);
		}

		// TODO: should allow customize keys to avoid coollide with 0,1,2..
		// create the extra main selector
		add(params_GroupMainSelector, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

		//--

		// TODO:
		// store the main slector only!
		int _last = groups.size() - 1;
		GROUP_Selected_Index.setMax(groupsSizes[_last] - 1);
		GROUP_Selected_Index.makeReferenceTo(PRESETS_Selected_Index[_last]);// TODO: link

		// excludes all selectors except the main one. the other will be saved as preset
		params_Selectors.setSerializable(false);
	}

	//--

	PRESETS_Selected_Index_PRE.resize(PRESETS_Selected_Index.size());
	for (int i = 0; i < PRESETS_Selected_Index_PRE.size(); i++)
	{
		PRESETS_Selected_Index_PRE[i] = -1;
	}

	//----

	// ImGui

#ifndef MODE_ImGui_EXTERNAL
	ImGui_Setup();
#endif

	//--

	// TODO: main group only
#ifdef INCLUDE_ofxUndoSimple
	undoStringParams = groups[0].toString();
#endif

	//----

	bSplitGroupFolders.setSerializable(false);// force to this mode. do not store

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
	params_UserKitSettings.add(params_Selectors);//includes all selectors

	// main group selector
	int _last = groups.size() - 1;
	GROUP_Selected_Index.set("GROUP_LINK", 0, 0, groupsSizes[_last] - 1);
	params_UserKitSettings.add(GROUP_Selected_Index);

	//----

	// TODO:
	// user gui selector
	GuiGROUP_Selected_Index.set("GUI GROUP SELECTOR", 0, 0, groups.size() - 1);
	GuiGROUP_Selected_Index.addListener(this, &ofxPresetsManager::Changed_GuiGROUP_Selected_Index);

	groupRandomizers.resize(groups.size());

	//listeners.resize(groups.size());

	for (int i = 0; i < groups.size(); i++)
	{
		// TODO:
		// link selectors from class to group randomizer
		groupRandomizers[i].PRESET_Selected_IndexMain.makeReferenceTo(PRESETS_Selected_Index[i]);// TODO: link

		//-

		// set the name for the randomizer settings file
		if (bSplitGroupFolders.get())
		{
			std::string _path;
			_path = path_UserKit_Folder + "/" + path_PresetsFavourites;// current kit-presets presets folder
			_path += "/" + groups[i].getName();// append group name
			_path += "/";// the folder
			ofxSurfingHelpers::CheckFolder(_path);// check parent container folder
			_path += groups[i].getName();
			_path += filename_Randomizers;// the full path with filename

			groupRandomizers[i].setPath_RandomizerSettings(_path);
		}

		//-

		groupRandomizers[i].setup(groups[i], keys[i]);// pass the group and the associated keys

		//-
		ofParameterGroup _g{ groups[i].getName() };
		_g.add(PRESETS_Selected_Index[i]);// selector
		_g.add(groupRandomizers[i].PLAY_RandomizeTimer);// play
		//_g.add(groupRandomizers[i].bRandomizeIndex);// random
		params_Selectors.add(_g);

		//ImGui::Dummy(ImVec2(0.0f, 5));
	}
	
	//----

	// custom path
	// this allows to use different kits for the same project/app
	params_Custom.setName("CUSTOM PATH");
	params_Custom.add(bPathDirCustom);
	params_Custom.add(pathDirCustom);
	params_UserKitSettings.add(params_Custom);

	ofAddListener(params_UserKitSettings.parameterChangedE(), this, &ofxPresetsManager::Changed_UserKit);

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

	// custom path
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

	// TODO:
	// refresh in other another better place?...
	buildHelpInfo();

	//--
}

//--------------------------------------------------------------
void ofxPresetsManager::update(ofEventArgs & args)
{
	if (!DISABLE_CALLBACKS)
	{
		//-

		// randomizers group

		for (int i = 0; i < groups.size(); i++) {
			groupRandomizers[i].update();

			if (groupRandomizers[i].is_bCloneAll()) doCloneAll(i);
			if (groupRandomizers[i].is_bCloneRight()) doCloneRight(i);
		}

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

		// TODO:
		////autosave
		////&& autoLoad? 
		//if (!MODE_Browser_NewPreset && autoSave && bAutosaveTimer && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
		//{
		//	ofLogNotice(__FUNCTION__) << "[AUTOSAVE]";
		//	//app settings
		//	save_ControlSettings();
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
void ofxPresetsManager::drawImGui()
{
	ImGui_Draw_WindowBegin();

	//draw content
	bImGui_mouseOver = ImGui_Draw_Window();

	ImGui_Draw_WindowEnd();
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

	//draw ImGui
#ifndef MODE_ImGui_EXTERNAL
	if (SHOW_ImGui)
	{
		drawImGui();
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

	float _round = 3.0f;
	float _pad = 3.0f;
	string _label;

	ofColor _colorText;// lines and text color
	ofColor _colorButton;// bg selected button
	ofColor _colorBg;// background color

	//-

	if (!lastMouseButtonState && ofGetMousePressed())
	{
		mousePressed(ofGetMouseX(), ofGetMouseY());
	}
	lastMouseButtonState = ofGetMousePressed();

	//-

	// dark theme
	if (bThemDark) {
		_colorText = ofColor(0, 255);
		_colorButton = ofColor(0, 64);
		_colorBg = ofColor(255, 8);
	}

	// light theme
	else {
		_colorText = ofColor(255, 128);
		_colorButton = ofColor(16, 200);
		_colorBg = ofColor(0, 128);
	}

	// display help info layout
	bool bLateralPosition = false;// false = on top of clicker
	bool bLeftPosition = true;// left or right. only if lateral pos true

	//----

	ofPushStyle();
	ofPushMatrix();
	ofTranslate(clicker_Pos);

	// draw keys buttons for each row/group

	for (size_t i = 0; i < keys.size(); ++i)
	{
		// 0. bg box of all boxes background
		ofFill();
		ofSetColor(_colorBg);

		// TODO: hide save button when not required
		//int _saveBut;
		//if (!autoSave) _saveBut = 1; else _saveBut = 0;
		//int _extraButs = (i == groups.size() - 1 ? (1 + _saveBut) : 1);// only main group has gui toggle button

		int _extraButs = (i == groups.size() - 1 ? 2 : 1);// only main group has gui toggle button

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
					cellSize * k + _pad, cellSize * i + _pad, cellSize - 2 * _pad, cellSize - 2 * _pad,
					_round);
				ofNoFill();

				// border only
				ofSetColor(_colorText);
				ofNoFill();
				ofDrawRectRounded(
					cellSize * k + _pad, cellSize * i + _pad, cellSize - 2 * _pad, cellSize - 2 * _pad,
					_round);

				ofPopStyle();
			}

			//--

			// 1.2 label boxes
			if (!myFont.isLoaded())// without ttf font
			{
				ofDrawBitmapString(ofToString((char)keys[i][k]),
					cellSize*k + 8, cellSize*i + 18);
			}
			else// custom font 
			{
				myFont.drawString(ofToString((char)keys[i][k]),
					cellSize * k + (0.5 * cellSize - 0.25 * sizeTTF), cellSize * i + (0.5 * cellSize + 0.5 * sizeTTF));
			}
		}

		// here, it's last k = keys[i].size()

		int ySave = cellSize * i + (0.5 * cellSize + 0.5 * sizeTTF);

		//--

		// TODO:
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

			// marl selected group. useful to navigate with arrow keys
			if (i == GuiGROUP_Selected_Index.get()) info = "* " + info;

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
			bShowClickerInfo = SHOW_Help.get();
			if (bShowClickerInfo && ENABLE_Keys && (i == 0))
			{
				string ss = helpInfo;
				int pad = 22;
				int x = 0;
				int y = 0;

				// A. vertical position below boxes
				if (!bLateralPosition)
				{
					float hh = ofxSurfingHelpers::getHeightBBtextBoxed(myFont, ss);
					x += 4 + pad;
					y -= hh + pad;
				}

				// B. lateral position right to the boxes
				else
				{
					float _w;
					y = ySave + pad;

					if (!bLeftPosition)// on the right
					{
						_w = getPresetClicker_Width();
						x = _w + pad + 30;
					}
					else {// on the left
						if (myFont.isLoaded())
						{
							_w = ofxSurfingHelpers::getWidthBBtextBoxed(myFont, ss);
							_w += myFont.getStringBoundingBox(groups[0].getName(), 0, 0).getWidth();
							_w += 70;
						}
						x = -_w;
					}
				}

				ofxSurfingHelpers::drawTextBoxed(myFont, ss, x, y, _colorText, _colorBg, true, _colorButton);
			}
		}
	}

	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
ofxPresetsManager::~ofxPresetsManager()
{
	// TODO: not sure if can avoid call manually exit(), bc here groups could be externally destroyed..
	// so we would prefer to call presetsManager.exit() manually on the first place sorting.
	//exit();
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

	//// append group name to subfolder files by each parameter group
	//if (bSplitGroupFolders) _pathFolder += _gName + "/";

	ofLogVerbose(__FUNCTION__) << "group path: " << _pathFolder;

	return _pathFolder;
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup _params, int _amountPresets)
{
	// main adder of a ofParameterGroup's but kind of internal yet. TODO: fix to work as public..

	ofLogNotice(__FUNCTION__) << "Added group " << _params.getName() << " with " << _amountPresets << " presets";
	int _size = groups.size();

	groups.push_back(_params);// each enqueued group of params handles all presets of each added ofParameterGroup
	groupsSizes.push_back(_amountPresets);// store the size or amount of presets of each row/group

	// preset selectors
	ofParameter<int> p{ groups[_size].getName(), 0, 0,  _amountPresets - 1 };
	//p.setSerializable(false);// exclude saving all slectors except last one, that will be enalbed at setup
	PRESETS_Selected_Index.push_back(p);
	//params_Selectors.add(PRESETS_Selected_Index[_size]);// add this new param (lastone)

	//-

	ofLogNotice(__FUNCTION__) << "group " << (_size) << " : " << groups[_size].getName();
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup _params, initializer_list<int> _keysList)
{
	// adds a ofParameterGroup for preset management with the list of trigger keys

	add(_params, _keysList.size());// re add with known size affter counted passed key triggers

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
	ofLogVerbose(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex;

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		//MODE A. it's important if this line is before or after ofSerialize
		DONE_save = true;

		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);

		bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);

		if (b) ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex << " " << _path;
		else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
	}

	//--

	////check clamp limiters
	//if ((guiIndex >= 0) && (guiIndex < (int)groups.size()))
	//{
	//	//A. main group
	//	if ((guiIndex == 0) &&
	//		(presetIndex >= 0) && (presetIndex < mainGroupMemoryFilesPresets.size()))
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
	//			if (b) ofLogNotice(__FUNCTION__) << " " << groups[guiIndex].getName() << " " << _path;
	//			else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
	//			TS_STOP("SAVE FILE 1");
	//		}

	//		//MODE B: direct from memory
	//		else
	//		{
	//			ofLogNotice(__FUNCTION__) << "MEMORY MODE";
	//			TS_START("SAVE MEM 1");
	//			if ((presetIndex >= 0) && (presetIndex < mainGroupMemoryFilesPresets.size()))
	//			{
	//				ofSerialize(mainGroupMemoryFilesPresets[presetIndex], groups[guiIndex]);
	//			}
	//			else
	//			{
	//				ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE";
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
	//		if (b) ofLogNotice(__FUNCTION__) << >" << groups[guiIndex].getName() << " : " << guiIndex << " " << _path;
	//		else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
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
	int guiIndex = getGuiIndex(gName);

	ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << gName << " preset: " << presetIndex;

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		DONE_save = true;
		{
			TS_START("SAVE FILE 2");//for TimeMeasurements only
			std::string _path = getPresetPath(gName, presetIndex);
			bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
			if (b) ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " " << guiIndex << " " << _path;
			else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
			TS_STOP("SAVE FILE 2");//for TimeMeasurements only
		}
	}

	//--

	////check clamp limiters
	//if ((guiIndex >= 0) && (guiIndex < (int)groups.size()))
	//{
	//	//A. main group
	//	if ((guiIndex == 0) &&
	//		(presetIndex >= 0) && (presetIndex < mainGroupMemoryFilesPresets.size()))
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
	//			if (b) ofLogNotice(__FUNCTION__) << " " << groups[guiIndex].getName() << " : " << guiIndex << " " << _path;
	//			else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
	//			TS_STOP("SAVE FILE 2");//for TimeMeasurements only
	//		}

	//		//MODE B: direct from memory
	//		else
	//		{
	//			TS_START("SAVE MEM 2");//for TimeMeasurements only

	//			if (presetIndex < mainGroupMemoryFilesPresets.size()) {
	//				ofSerialize(mainGroupMemoryFilesPresets[presetIndex], groups[guiIndex]);
	//			}
	//			else {
	//				ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE";
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
	//		ofSerialize(mainGroupMemoryFilesPresets[presetIndex], groups[guiIndex]);
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
	ofLogVerbose(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex;

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		// TODO:
		// avoid startup creators...
		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
		bool b = ofxSurfingHelpers::loadGroup(groups[guiIndex], _path);
		if (!b) {
			ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " : " << guiIndex << " " << _path;

			// file not found so create one file instead
			save(presetIndex, guiIndex);
		}
		else ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex << " " << _path;

		// TODO: ? already marked?
		// mark selected
		PRESETS_Selected_Index[guiIndex] = presetIndex;
	}

	//--

	//// check groups range clamp limiters
	//if ((guiIndex >= 0) && (guiIndex < (int)groups.size()))
	//{
	//	//A. main group
	//	if ((guiIndex == 0) &&
	//		(presetIndex >= 0) && (presetIndex < mainGroupMemoryFilesPresets.size()))
	//	{
	//		if (!MODE_MemoryLive)
	//		{
	//			//MODE A: from hd file
	//			TS_START("LOAD FILE 1");//for TimeMeasurements only
	//			std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
	//			bool b = ofxSurfingHelpers::loadGroup(groups[guiIndex], _path);
	//			if (b) ofLogNotice(__FUNCTION__) << "Load: " << groups[guiIndex].getName() << " " << _path;
	//			else ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " " << _path;
	//			TS_STOP("LOAD FILE 1");//for TimeMeasurements only
	//		}
	//		else
	//		{
	//			//MODE B: direct from memory
	//			TS_START("LOAD MEM 1");//for TimeMeasurements only
	//			//using xml array
	//			if (presetIndex < mainGroupMemoryFilesPresets.size())
	//			{
	//				ofDeserialize(mainGroupMemoryFilesPresets[presetIndex], groups[guiIndex]);
	//			}
	//			else
	//			{
	//				ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE";
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
	//			ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " : " << guiIndex << " " << _path;

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
			ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " : " << guiIndex << " " << _path;

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
	//		(presetIndex >= 0) && (presetIndex < mainGroupMemoryFilesPresets.size()))
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
	//			if (presetIndex < mainGroupMemoryFilesPresets.size()) {
	//				ofDeserialize(mainGroupMemoryFilesPresets[presetIndex], groups[guiIndex]);
	//			}
	//			else {
	//				ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE LOAD";
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
	//			ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " : " << guiIndex << " " << _path;

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

//----------------------------------------------------------------
void ofxPresetsManager::keyPressed(ofKeyEventArgs &eventArgs)
{
	const int &key = eventArgs.key;

	if (key == 'K' && !bImGui_mouseOver)// restore keys control
	{
		ENABLE_Keys = !ENABLE_Keys;
	}

	else if (bKeys && ENABLE_Keys && !bImGui_mouseOver)// disable keys when mouse over gui
	{
		//-

		// mode key for saving with mouse or trigger keys
		if (key == modeKeySave)
		{
			bKeySave = true;
			ofLogVerbose(__FUNCTION__) << "modeKeySave TRUE" << endl;
			return;
		}

		// mode key for swap with mouse or trigger keys
		else if (key == modKeySwap)
		{
			bKeySwap = true;
			ofLogVerbose(__FUNCTION__) << "modKeySwap TRUE" << endl;
			return;
		}

		//--

		// TODO: 
		if (false) {}

		// hide/show control gui
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

		// randomizers

		// timer to randomize and choice a random preset from the kit
		else if (key == ' ')
		{
			for (int i = 0; i < groups.size(); i++) {
				setTogglePlayRandomizerPreset(GuiGROUP_Selected_Index);
			}
		}
		else if (key == 'R')
		{
			doRandomizePresetSelected(GuiGROUP_Selected_Index);
		}
		//// timer to randomize and choice a random preset from the kit
		//if (key == 'R')
		//{
		//	setTogglePlayRandomizerPreset();
		//		//doRandomIndex(GuiGROUP_Selected_Index);
		//}
		//			else if (key == 'E')
		//			{
		//				doRandomPreset();
		//			}
		//		}

		//----

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


		//----

		// navigate kit/favorites presets
		if (ENABLE_KeysArrowBrowse)
		{
			// browse groups
			if (key == OF_KEY_UP)
			{
				GuiGROUP_Selected_Index--;
			}

			else if (key == OF_KEY_DOWN)
			{
				GuiGROUP_Selected_Index++;
			}

			else if (key == OF_KEY_LEFT)
			{
				DISABLE_CALLBACKS = true;
				int sel = GuiGROUP_Selected_Index.get();
				int i = PRESETS_Selected_Index[sel];
				i--;
				DISABLE_CALLBACKS = false;
				PRESETS_Selected_Index[sel] = (int)ofClamp(i, 0, PRESETS_Selected_Index[sel].getMax());
			}
			else if (key == OF_KEY_RIGHT)
			{
				DISABLE_CALLBACKS = true;
				int sel = GuiGROUP_Selected_Index.get();
				int i = PRESETS_Selected_Index[sel];
				i++;
				DISABLE_CALLBACKS = false;
				PRESETS_Selected_Index[sel] = (int)ofClamp(i, 0, PRESETS_Selected_Index[sel].getMax());
			}
		}

		//----

		// presets selectors

		for (int g = 0; g < keys.size(); g++)
		{
			for (int k = 0; k < keys[g].size(); k++)// performs all registered keys: one for each [8] preset
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
	// mod keys
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

//----

//-----------------------------------------------------
void ofxPresetsManager::mousePressed(int x, int y)
{
	// this method will get what box button is pressed: in wich group-row (y) and wich preset (x)

	x = x - clicker_Pos.x;
	y = y - clicker_Pos.y;

	int xIndex = x / cellSize;
	int yIndex = y / cellSize;

	// Index -1 for out (left) of boxes
	xIndex = (x > 0) ? xIndex : -1;
	yIndex = (y > 0) ? yIndex : -1;

	//-

	// 1. presets buttons & save button

	// click is iniside allowed presets/groups
	if ((yIndex >= 0) && (yIndex < (int)groups.size()))//valid group
	{
		//avoid outer panel logs. only into the group row levels
		if ((xIndex != -1) && (yIndex != -1) &&//valid preset. to the left of panels
			(xIndex >= 0) && (xIndex < groupsSizes[yIndex]))//valid group. to the right of panels

			ofLogNotice(__FUNCTION__) << groups[yIndex].getName() << " group: " << yIndex << " preset: " << xIndex;

		//-

		if ((xIndex >= 0) && (xIndex < groupsSizes[yIndex]))
		{
			// 1. mod save controlled by modeKeySave
			if (bKeySave)
			{
				ofLogNotice(__FUNCTION__) << "SAVE";

				save(xIndex, yIndex);

				//will auto load and set the already clicked preset button
				if (yIndex < PRESETS_Selected_Index.size()) PRESETS_Selected_Index[yIndex] = xIndex;
			}

			//-

			// 2. mod swap controlled by modKeySwap
			else if (bKeySwap)
			{
				ofLogNotice(__FUNCTION__) << "SWAP";

				if (yIndex < PRESETS_Selected_Index.size()) doSwap(yIndex, PRESETS_Selected_Index[yIndex], xIndex);
			}

			//-

			// 3. no mod keys: normal load (not any key modifier pressed)
			else
			{
				ofLogNotice(__FUNCTION__) << "LOAD";

				if (yIndex < PRESETS_Selected_Index.size()) PRESETS_Selected_Index[yIndex] = xIndex;
			}
		}

		//-

		// 2. last button (save button)
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

	// 3. toggle show gui (on main group only)
	// TODO: hide save button on autosave mode...
	//int _offset = (autoSave ? 0 : 1);
	int _offset = 1;

	int _i;
	if (bBuildGroupSelector) _i = groups.size() - 1;
	else if (groups.size() > 1) _i = 1;
	else _i = 0;

	if ((yIndex == groups.size() - 1) && (xIndex == groupsSizes[_i] + _offset))
	{
		SHOW_ImGui = !SHOW_ImGui;

		ofLogNotice(__FUNCTION__) << "SHOW_ImGui : " << (SHOW_ImGui ? "TRUE" : "FALSE");
	}
}

//----

// helpers 

//--------------------------------------------------------------
void ofxPresetsManager::doCloneRight(int gIndex)
{
	int pIndex = PRESETS_Selected_Index[gIndex];

	//auto save current preset
	if (autoSave)
	{
		ofLogVerbose(__FUNCTION__) << "autosave preset: " << pIndex;
		save(pIndex, gIndex);
	}
	ofLogNotice(__FUNCTION__) << "on group: " << gIndex << " from preset: " << pIndex;

	CheckAllFolders();

	for (int i = pIndex + 1; i < groupsSizes[gIndex]; i++)
	{
		save(i, gIndex);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doCloneAll(int gIndex)
{
	ofLogNotice(__FUNCTION__) << "on group: " << gIndex;

	CheckAllFolders();

	//auto save current preset
	if (autoSave)
	{
		ofLogVerbose(__FUNCTION__) << "autosave preset: " << PRESETS_Selected_Index[gIndex];
		save(PRESETS_Selected_Index[gIndex], gIndex);
	}

	//clone all
	for (int i = 0; i < groupsSizes[gIndex]; i++)
	{
		save(i, gIndex);
	}
}

////--------------------------------------------------------------
//void ofxPresetsManager::doPopulateFavs(int gIndex)
//{
//	ofLogNotice(__FUNCTION__);
//
//	CheckAllFolders();//? required
//
//	for (int i = 0; i < groups.size(); i++)
//	{
//		doRandomizePresetSelected(i);// TODO: should avoid update()?
//		doCloneAll(i);
//	}
//}

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

//----

//--------------------------------------------------------------
void ofxPresetsManager::setToggleKeysControl(bool active)
{
	bKeys = active;
}

//#ifdef INCLUDE_RANDOMIZER
////--------------------------------------------------------------
//void ofxPresetsManager::Changed_Params_Editor(ofAbstractParameter &e)
//{
//	if (!DISABLE_CALLBACKS)
//	{
//		string name = e.getName();
//
//		ofLogNotice(__FUNCTION__) << groups[0].getName() << " " << name << " : " << e;
//
//		//-
//
//		if (name == bRandomizeEditor.getName() && bRandomizeEditor)//trig randomize
//		{
//			bRandomizeEditor = false;
//
//			doRandomPreset();
//		}
//		else if (name == bRandomizeEditorAll.getName() && bRandomizeEditorAll)//all
//		{
//			bRandomizeEditorAll = false;
//
//			for (auto &p : editorPresets) {
//				p.set(true);
//			}
//		}
//		else if (name == bRandomizeEditorNone.getName() && bRandomizeEditorNone)//none
//		{
//			bRandomizeEditorNone = false;
//
//			for (auto &p : editorPresets) {
//				p.set(false);
//			}
//		}
//		else if (name == bRandomizeEditorPopulateFavs.getName() && bRandomizeEditorPopulateFavs)//populate random all favs
//		{
//			bRandomizeEditorPopulateFavs = false;
//			doPopulateFavs();
//		}
//	}
//}
//#endif

//--------------------------------------------------------------
void ofxPresetsManager::Changed_UserKit(ofAbstractParameter &e)
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

		// TODO: not working
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

		// ALL GROUPS PRESETS 

		// presets selectors
		if (!DISABLE_CALLBACKS_SELECTORS)
		{
			// exclude group 0
			for (int g = 0; g < groups.size(); g++)// iterate each group
			{
				for (int p = 0; p < groupsSizes[g]; p++)// iterate each preset on each group
				{
					if (name == PRESETS_Selected_Index[g].getName() && PRESETS_Selected_Index[g].get() == p)
					{
						// some preset of any group changed

						ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " group: " << g << " preset: " << p;

						//-

						//if (g == 0)
						//{
						//	PRESETS_Selected_Index[p] = 
						//}

						//-

						// 1. main group selected preset NOT CHANGED

						if (PRESETS_Selected_Index[g] == PRESETS_Selected_Index_PRE[g])
						{
							ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " preset " << p << " Not Changed";

							// browser
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

						// 2. main group selected preset CHANGED

						else if (PRESETS_Selected_Index[g] != PRESETS_Selected_Index_PRE[g])
						{
							ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " preset " << p << " Changed";

							//-

							// workflow
							// browser mode bypasses autosave
							//if (autoSave && !MODE_Browser_NewPreset)
							if (autoSave)
							{
								save(PRESETS_Selected_Index_PRE[g], g);
							}

							//-

							// remember this PRE state to know if changed or not on the next time..
							PRESETS_Selected_Index_PRE[g] = PRESETS_Selected_Index[g];

							//-

							if (autoLoad) load(PRESETS_Selected_Index[g], g);
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Control(ofAbstractParameter &e)
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

		//--

		// mode edit: when false, we disabled autosave to allow performance do faster
		else if (name == MODE_Editor.getName())
		{
			ofLogNotice(__FUNCTION__) << "MODE EDITOR: " << (MODE_Editor.get() ? "TRUE" : "FALSE");

			autoSave = MODE_Editor.get();

			//--

			//// TODO:
			//// refresh in other another better place?...
			//buildHelpInfo();
		}

		//----

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

		// workflow
		else if (name == SHOW_ImGui_PresetsParams.getName())
		{
			if (SHOW_ImGui_PresetsParams) SHOW_ImGui_Selectors = false;
		}
		else if (name == SHOW_ImGui_Selectors.getName())
		{
			if (SHOW_ImGui_Selectors) SHOW_ImGui_PresetsParams = false;
		}

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
	}
}

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

	//user settings

	//TODO:
	//testing to enable here to avoid create empty xml settings on global root?
	DISABLE_CALLBACKS = false;

	string path3 = filenameMainSettings;

	b = ofxSurfingHelpers::loadGroup(params_UserKitSettings, path3);

	//if (!b) ofLogError(__FUNCTION__) << "CANT LOAD FILE '" << path3 << "'!";
	//else ofLogNotice(__FUNCTION__) << "LOADED " << path3;

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

	// TODO: test to avoid crashes..?
	try
	{
		// save randomizers settings
		ofLogVerbose(__FUNCTION__) << endl << params_Control.toString() << endl;

		string path;
		bool b;

		path = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_ControlSettings;
		b = ofxSurfingHelpers::saveGroup(params_Control, path);

		//-

		//// save randomizers settings
		//path = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_Randomizers;
		//b = ofxSurfingHelpers::saveGroup(params_RandomizerSettings, path);

		//--

		// user settings
		path = filenameMainSettings;
		b = ofxSurfingHelpers::saveGroup(params_UserKitSettings, path);

		//--
	}
	catch (int n)
	{
		ofLogError(__FUNCTION__) << "CATCH ERROR " << n << endl;
		throw;
	}

	//----

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

	// create the folders of each group into main presets folder 
	string _path;
	if (bSplitGroupFolders.get())
	{
		for (int i = 0; i < groups.size(); i++)
		{
			_path = path_UserKit_Folder + "/" + path_PresetsFavourites;// current kit-presets presets folder
			_path += "/" + groups[i].getName();// append group name

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

// memory mode 
// (loaded from data vector instead of hd files)
//--------------------------------------------------------------
void ofxPresetsManager::save_AllKit_FromMemory()
{
	ofLogVerbose(__FUNCTION__);

	for (int i = 0; i < mainGroupMemoryFilesPresets.size(); i++)
	{
		string _folder, _file, _path;

		_folder = path_UserKit_Folder + "/" + path_PresetsFavourites + "/";
		_file = groups[0].getName() + filenamesPrefix + ofToString(i);
		_path = _folder + _file;
		_path += fileExtension;

		if (i < mainGroupMemoryFilesPresets.size())
		{
#ifdef USE_XML
			mainGroupMemoryFilesPresets[i].save(_path);
#else
#ifdef USE_JSON
			bool b = ofSavePrettyJson(_path, mainGroupMemoryFilesPresets[i]);
			if (!b) ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets > " << _path;
#endif
#endif
		}
		else {
			ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE";
		}

		}

	// debug params
	if (true)
	{
		for (int i = 0; i < mainGroupMemoryFilesPresets.size(); i++)
		{
#ifdef USE_XML
			ofLogNotice(__FUNCTION__) << "mainGroupMemoryFilesPresets[" << i << "] " << ofToString(mainGroupMemoryFilesPresets[i].toString());
#else
#ifdef USE_JSON
#endif
#endif
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load_AllKit_ToMemory()
{
	ofLogNotice(__FUNCTION__);

	//// TODO:
	//groupsMem.clear();
	//groupsMem.reserve(NUM_OF_PRESETS);
	//groupsMem.resize(NUM_OF_PRESETS);

	for (int i = 0; i < mainGroupMemoryFilesPresets.size(); i++)
	{
		// TODO:
		// PROBLEM:
		// this is not working because all the groups of the vector are shallow copies,
		// so all are changing together
		// pre-create params inside the group not avoid emptyness
		//ofParameterGroup g = groups[0];
		//groupsMem[i] = g;

		//-

		string pathFolder, pathFilename, pathComplete;
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

		//// TODO:
		//ofDeserialize(settings, groupsMem[i]);
		//ofLogNotice(__FUNCTION__) << "ofParameterGroup: " << i << "\n" << ofToString(groupsMem[i]);

		if (bLoaded)
		{
			if (i < mainGroupMemoryFilesPresets.size()) {

#ifdef USE_XML
				mainGroupMemoryFilesPresets[i] = settings;
#endif
			}
			else {
				ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE";
			}
		}
	}

	ofLogNotice(__FUNCTION__) << "-------------------------------------------------------------------------------------------------------";

	// debug params
	if (false)
	{
		for (int i = 0; i < mainGroupMemoryFilesPresets.size(); i++)
		{
#ifdef USE_XML
			ofLogNotice(__FUNCTION__) << "mainGroupMemoryFilesPresets[" << i << "] " << ofToString(mainGroupMemoryFilesPresets[i].toString());
#endif
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

	// destroy callbacks
	removeKeysListeners();
	ofRemoveListener(params_UserKitSettings.parameterChangedE(), this, &ofxPresetsManager::Changed_UserKit);
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Control);

	//ofRemoveListener(params_Randomizer.parameterChangedE(), this, &ofxPresetsManager::Changed_Control);
	//ofRemoveListener(params_Editor.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Editor);

	// TODO: required?
	ofRemoveListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofRemoveListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	GuiGROUP_Selected_Index.removeListener(this, &ofxPresetsManager::Changed_GuiGROUP_Selected_Index);

	//--

	// main group
	
	//-

	// TODO:
	// MODE B: direct from memory
	if (MODE_MemoryLive && autoSave)
	{
		save_AllKit_FromMemory();// update of files is required bc presets are on memmory only
	}

	//--

	// autosave all group presets
	if (autoSave)
	{
		// save selected presets
		for (int g = 0; g < groups.size(); g++)// iterate each group
		{
			save(PRESETS_Selected_Index[g], g);
		}
	}

	//// populate
	//for (int g = 0; g < groups.size(); g++)//iterate each group
	//{
	//	for (int p = 0; p < groupsSizes[g]; p++)//save each preset on each group
	//	{
	//		save(p, g);
	//	}
	//}

	//--

	// all app settings
	save_ControlSettings();

	//--

	// TODO: debug crash
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

	// font customize
#ifndef MODE_ImGui_EXTERNAL
#ifdef INCLUDE_IMGUI_CUSTOM_FONT
	ofxSurfingHelpers::ImGui_FontCustom();
#endif
#endif

	//--

	gui_ImGui.setup();

	//--

	// theme
#ifndef MODE_ImGui_EXTERNAL
	ofxSurfingHelpers::ImGui_ThemeModernDark();
	//ofxSurfingHelpers::ImGui_ThemeMoebiusSurfing();

	//--

	// mouse over
	ImGui::GetIO().MouseDrawCursor = false;
	bImGui_mouseOver.set("mouseOverGui", false);
	//ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
#endif

	//--
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_WindowBegin()
{
	// mouse over gui system
	bMouseOver_Changed = false;
	bImGui_mouseOver = false;

	//-

#ifndef USE_ofxImGuiSimple
	gui_ImGui.begin();
#else
	gui_ImGui.begin();
	ImGui::ShowDemoWindow();
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

	mainSettings.windowPos = _pos;
	mainSettings.windowSize = _size;

	auto _mode = ImGuiCond_FirstUseEver;//ImGuiCond_Always;
	ImGui::SetNextWindowPos(ofVec2f(_pos.x, _pos.y), _mode);
	ImGui::SetNextWindowSize(ofVec2f(_size.x, _size.y), _mode);

	//--

	// User-Kit name
	string _name = "ofxPresetsManager : " + displayNameUserKit;

	bool _collapse = false;// TODO: don't do nothing?

	//--

	// set window properties
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

	// A window: main (1st) group
	_collapse = true;
	if (ofxImGui::BeginWindow(_name, mainSettings, window_flags, &_collapse))
	{
		ImGui_Draw_WindowContent(mainSettings);

		// get window position/size: must be inside begin/end
		_pos = ImGui::GetWindowPos();
		_size = ImGui::GetWindowSize();

		ImGui_Position = glm::vec2(_pos.x, _pos.y);
		ImGui_Size = glm::vec2(_size.x, _size.y);
	}
	ofxImGui::EndWindow(mainSettings);

	//----

	// B window
	// window for all group parameters contained into presetsManager
	// all parameters in all added groups

	if (SHOW_ImGui_PresetsParams) {

		_size = ofVec2f(400, 800);
		_pos = _pos - ofVec2f(_size.x + 10, 0);

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

	// C window
	// each group selectors 
	// window for each group selector

	if (SHOW_ImGui_Selectors) {

		_size = ofVec2f(350, 500);
		_pos = _pos - ofVec2f(_size.x + 10, 0);

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

// ImGui pure content
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
	// 0.1 sliders preset selectors
	//for (int i = 0; i < PRESETS_Selected_Index.size(); i++)
	//{
	//	ofxImGui::AddParameter(PRESETS_Selected_Index[i]);
	//}

	ofxImGui::AddGroup(params_Selectors, settings);
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_MainPanel(ofxImGui::Settings &settings)
{
	if (ofxImGui::BeginTree("MAIN CONTROL", settings))
	{
		// mode edit
		ofxImGui::AddParameter(MODE_Editor);
		//ofxSurfingHelpers::AddBigToggle(MODE_Editor, 30);// TODO: repair. collides when multiple toggles..

		//---

		//ImGui::SameLine();
		if (ImGui::TreeNode("PANELS"))
		{
			ofxImGui::AddParameter(SHOW_ClickPanel);
			if (bBuildGroupSelector) ofxImGui::AddParameter(SHOW_ImGui_Selectors);
			ofxImGui::AddParameter(SHOW_ImGui_PresetsParams);
			ofxImGui::AddParameter(SHOW_BrowserPanel);

			ImGui::TreePop();
		}

		// extra
		if (MODE_Editor) ImGui_Draw_Extra(settings);

		//--

		//ImGui::TreePop();
		ofxImGui::EndTree(settings);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Extra(ofxImGui::Settings &settings)
{
	//----

	if (ImGui::TreeNode("EXTRA")) {

		//-

		if (ImGui::TreeNode("MODES"))
		{
			ofxImGui::AddParameter(autoSave); ImGui::SameLine();
			//ofxImGui::AddParameter(autoLoad);//ImGui::SameLine();
			ofxImGui::AddParameter(MODE_MemoryLive);
			ofxImGui::AddParameter(SHOW_Help); ImGui::SameLine();
			ofxImGui::AddParameter(bThemDark);

			ImGui::TreePop();
		}

		//--

		if (MODE_Editor)
		{
			if (ImGui::TreeNode("USER-KIT"))
			{
				// User-Kit name
				string str;
				str = "Name: ";
				str += displayNameUserKit;
				ImGui::Text(str.c_str());

				// button to Open File Dialog as folder
				if (ImGui::Button("Set Custom folder")) {

					ofFileDialogResult openFileResult = ofSystemLoadDialog("Select User-Kit folder", true, ofToDataPath(path_UserKit_Folder, true));

					// check if the user opened a file
					if (openFileResult.bSuccess) {

						ofLogNotice(__FUNCTION__) << ("User selected a folder");

						// we have a file, check it and process it
						doFileDialogProcessSelection(openFileResult);
					}
					else ofLogNotice(__FUNCTION__) << ("User hit cancel");
				}

				ImGui::Dummy(ImVec2(0.0f, 5));

				// monitor custom state
				//ofxImGui::AddParameter(bPathDirCustom);

				ImGui::TreePop();
			}
		}

		//--

		//// show extra
		//if (ImGui::TreeNode("EXTRA")) {
		//	ofxImGui::AddParameter(SHOW_ImGui);
		//	ofxImGui::AddParameter(SHOW_Gui_AdvancedControl);
		//	//-
		//	// 2. advanced
		//	if (SHOW_Gui_AdvancedControl)
		//	{
		//		// show ALL the addon internal params! mainly to debug all settings or to use without ImGui..
		//		ofxImGui::AddGroup(params_Control, settings);
		//	}
		//	ImGui::TreePop();
		//}

		ImGui::TreePop();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::buildHelpInfo() {
	// build help info
	helpInfo = "";
	helpInfo += "USER-KIT\n";
	//helpInfo += "  ";
	helpInfo += displayNameUserKit;
	helpInfo += "       ";
#ifdef USE_XML
	helpInfo += ".xml";
#else
#ifdef USE_JSON
	helpInfo += ".json";
#endif
#endif
	//helpInfo += "                            ";

	helpInfo += "\n\nPATHS\n";
	helpInfo += getGroupsPaths();
	//helpInfo += "                            ";

	// TODO:
	//bool bKeysinfo = false;
	//if (bKeysinfo)
	//{
	//	//keys[i][k]
	//	helpInfo += "[keys " + ofToString((char)keys[0][0]) + "|";
	//	helpInfo += ofToString((char)keys[0][keys[0].size() - 1]) + "]";
	//}

	helpInfo += "\n";
	helpInfo += "KEYS\n";
	helpInfo += "MOUSE|KEYS             LOAD\n";
	helpInfo += "CTRL                   SAVE/COPY\n";
	helpInfo += "ALT                    SWAP\n";
	helpInfo += "ARROWS                 NAVIGATE\n";
	helpInfo += "E                      EDIT\n";
	helpInfo += "H                      HELP\n";
	helpInfo += "G                      GUI";
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
//#ifdef INCLUDE_RANDOMIZER
//	buildRandomizers();
//#endif
}

// TODO:
//--------------------------------------------------------------
void ofxPresetsManager::buildCustomUserKit() {

	setPath_UserKit_Folder(pathDirCustom);//creates subfolders for each group

	//-

	// reduce to name only
	string str = path_UserKit_Folder;
	std::string temp = R"(\)";//use '\' as splitter...should use '/' too bc Windows/macOS compatibility..
	auto ss = ofSplitString(path_UserKit_Folder, temp);
	displayNameUserKit = ss[ss.size() - 1];//get last word and use as name

	//-

	// TODO:
	// load standalone presets for browser gui
	buildStandalonePresets();

	//-

	//// load randomizers settings
	//string path2;
	//bool b2;
	//path2 = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_Randomizers;
	//b2 = ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path2);
	//ofLogNotice(__FUNCTION__) << "Loaded " << path2 << " " << (b2 ? "DONE" : "FAILED");	//--

	// build help info
	buildHelpInfo();

	//--

	// radomizers
	//buildRandomizers();
}

//--------------------------------------------------------------
void ofxPresetsManager::doFileDialogProcessSelection(ofFileDialogResult openFileResult) {

	ofLogNotice(__FUNCTION__) << ("getName(): " + openFileResult.getName());
	ofLogNotice(__FUNCTION__) << ("getPath(): " + openFileResult.getPath());

	//--

	// autosave all group presets
	if (autoSave)
	{
		// save selected presets
		for (int g = 0; g < groups.size(); g++)//iterate each group
		{
			save(PRESETS_Selected_Index[g], g);
		}
	}

	//--

	//// save randomizers settings
	//string path2;
	//bool b2;
	//path2 = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_Randomizers;
	//b2 = ofxSurfingHelpers::saveGroup(params_RandomizerSettings, path2);
	//ofLogNotice(__FUNCTION__) << "Saved " << path2 << " " << (b2 ? "DONE" : "FAILED");

	//--

	DISABLE_CALLBACKS = true;//TODO:
	bPathDirCustom = true;
	DISABLE_CALLBACKS = false;

	pathDirCustom = openFileResult.getPath();

	//--

	buildCustomUserKit();

	//--

	// autoload all group presets
	if (autoLoad)
	{
		// must disable autosave to avoid overwrite between user-kits
		bool bPre = autoSave;
		autoSave = false;

		// set selected first preset o last/main group
		int _last = groups.size() - 1;
		PRESETS_Selected_Index[_last] = 0;
		load(PRESETS_Selected_Index[_last], _last);
		autoSave = bPre;

		//// load all group selected presets
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
				//ofLogNotice(__FUNCTION__) << "TO FAVORITES PRESET: [" << PRESET_Selected_IndexMain << "]";

				if (MODE_Browser_NewPreset)
				{
					//save(PRESET_Selected_IndexMain, 0);
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
	// tittle
	//ImGui::Text("PRESETS MANAGER");
	//ImGui::NewLine();

	// main panel
	ImGui_Draw_MainPanel(settings);

	// main group
	if (bBuildGroupSelector) ofxImGui::AddParameter(PRESETS_Selected_Index[groups.size() - 1]);

	// randomizers
	ofxImGui::AddParameter(GuiGROUP_Selected_Index);// user selected group
	groupRandomizers[GuiGROUP_Selected_Index.get()].ImGui_Draw_GroupRandomizers(settings);// show randomizers of user selected group

	// standalone presets browser
	if (MODE_Editor && SHOW_BrowserPanel) {
		ImGui_Draw_Browser(settings);
	}
}

// standalone presets browser
//--------------------------------------------------------------
void ofxPresetsManager::doStandalonePresetSave(string name)// without xml extension nor path
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
void ofxPresetsManager::doLoadMainGroupPreset(string name)// without xml extension nor path
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
void ofxPresetsManager::buildStandalonePresets()// standalone presets splitted from favourites presets
{
	// load files structure directory
	bool bLoaded = doStandaloneRefreshPresets();

	// workflow
	if (bLoaded)
	{
		// load first preset
		if (fileNames.size() > 0)
		{
			currentFile = 0;
			displayNamePreset = fileNames[currentFile];

			//// workflow
			//doLoadMainGroupPreset(displayNamePreset);
		}
	}
}


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

	// clear files and filenames vectors
	files.clear();
	fileNames.clear();

	// load all folder files in one call
	files = dataDirectory.getFiles();

	ofLogNotice(__FUNCTION__) << "Preset files:";
	for (size_t i = 0; i < files.size(); i++)
	{
		fileNames.push_back(files[i].getBaseName());
		ofLogNotice(__FUNCTION__) << "[" << i << "] " << files[i].getBaseName();
	}

	//-

	// TODO
	//void to go to 1st...

	// 1. load same position preset
	// if preset is deleted goes to nextone..
	// should check names because sorting changes..
	if (fileNames.size() > 0)
	{
		bFilesError = false;
	}
	else
	{
		ofLogError(__FUNCTION__) << "BROWSER: FILES NOT FOUND ON FOLDER!";
		bFilesError = true;

		//// TODO:
		//// disable custom path bc error
		//ofLogError(__FUNCTION__) << "Disable custom path: " << path_BrowserPathFree;
		//bCustomBrowserPath = false;
		//ofLogError(__FUNCTION__) << "------------------------";
	}

	//workflow
	//// 2. always goes to 1st preset 0
	//// that's because saving re sort the files
	//// and we don't know the position of last saved preset..
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

//--------------------------------------------------------------
void ofxPresetsManager::Changed_GuiGROUP_Selected_Index(int & index)
{
	GuiGROUP_Selected_Index = (int)ofClamp(GuiGROUP_Selected_Index.get(), 0, GuiGROUP_Selected_Index.getMax());
	ofLogWarning(__FUNCTION__) << " : " << GuiGROUP_Selected_Index;
}
//
////--------------------------------------------------------------
//void groupRandomizer::doCheckPresetsFolderIsEmpty()
//{
//	//string _path = path_UserKit_Folder + "/" + path_PresetsFavourites;
//	//ofLogNotice(__FUNCTION__) << "Check that not empty folder at path: " << _path;
//	//ofDirectory dataDirectory(ofToDataPath(_path, true));
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