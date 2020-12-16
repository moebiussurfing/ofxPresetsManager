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

	// subscribed to auto run update and draw without required 'manual calls'
	ofAddListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofAddListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	//--

	// default settings paths

	displayNameUserKit = "ofxPresetsManager";

	//----

	filenameMainSettings = "ofxPresetsManager_Settings" + fileExtension;// default user preset name

	// top parent folder
	// root

	// default User-Kit folder (main root container!)
	path_UserKit_Folder = "ofxPresetsManager";

	// favourites presets (live clickeable and key triggeable) 
	path_PresetsFavourites = "presets";
	// standalone presets
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
	filename_ControlSettings = "Control_Settings";
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

	//--

	modeKeySave = OF_KEY_CONTROL;
	modKeySwap = OF_KEY_ALT;
	bKeys = false;
	bModKeySave = false;
	bModKeySwap = false;
	keysNotActivated = true;
	lastMouseButtonState = false;

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
	SHOW_Panel_Click.set("SHOW CLICKER", true);
	SHOW_Panel_AllParameter.set("SHOW ALL PARAMETERS", true);
	SHOW_Panel_AllSelectors.set("SHOW SELECTORS", true);
	SHOW_Panel_StandalonePresets.set("SHOW STANDALONES", true);
	MODE_StandalonePresets_NEW.set("NEW!", false);
	SHOW_Panel_Randomizer.set("SHOW RANDOMIZERS", true);
	ENABLE_Keys.set("ENABLE KEYS", true);

	autoLoad.set("AUTO LOAD", true);
	autoSave.set("AUTO SAVE", true);
	MODE_MemoryLive.set("MODE MEMORY", false);// TODO: not implemented
	loadToMemory.set("LOAD TO MEMORY", false);
	saveFromMemory.set("SAVE FROM MEMORY", false);
	bAutosaveTimer.set("TIMER AUTO SAVE", false);

	//displayNamePreset = "NO_NAME_PRESET";//browser loaded preset name

	//-

	// layout

	Gui_Internal_Position.set("GUI INTERNAL POSITION",
		glm::vec2(ofGetWidth() * 0.5, ofGetHeight()* 0.5),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);

	ImGui_Size.set("GUI ImGui SIZE",
		glm::vec2(300, 800),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);
	ImGui_Position.set("GUI ImGui POSITION",
		glm::vec2(ofGetWidth() - (ImGui_Size.get().x + 10), 10),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);

	//-

	// exclude from settings
	loadToMemory.setSerializable(false);
	saveFromMemory.setSerializable(false);
	//SHOW_Gui_AdvancedControl.setSerializable(false);

	//--

	// internal control params

	params_Options.setName("OPTIONS");
	//params_Options.add(MODE_MemoryLive);// TODO: not implemented
	params_Options.add(loadToMemory);
	params_Options.add(saveFromMemory);
	params_Options.add(autoLoad);
	params_Options.add(autoSave);
	params_Options.add(bAutosaveTimer);

	params_Gui.setName("GUI");
	params_Gui.add(SHOW_Gui_AdvancedControl);
	params_Gui.add(SHOW_Panel_AllParameter);
	params_Gui.add(SHOW_Panel_AllSelectors);
	params_Gui.add(SHOW_Panel_Click);
	params_Gui.add(SHOW_Panel_StandalonePresets);
	params_Gui.add(SHOW_Panel_Randomizer);
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

	// gui box clicker font
	std::string str;

	//sizeTTF = 8;
	//str = "telegrama_render.otf";

	sizeTTF = 10;
	str = "overpass-mono-bold.otf";

	myTTF = "assets/fonts/" + str;
	bool bLoaded = myFont.load(myTTF, sizeTTF, true, true);
	if (!bLoaded) bLoaded = myFont.load(OF_TTF_SANS, sizeTTF, true, true);

	//--

	// custom path:
	bPathDirCustom.set("MODE CUSTOM PATH", false);
	pathDirCustom.set("Path", "DEFAULT_DATA_PATH");

	//-

	// mainly to measure performance when using hd files vs faster memory vectors
#ifdef DEBUG_PERFORMANCE_MEASURES
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
	// filenameMainSettings will use default name "ofxPresetsManager_Settings.xml"
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

		// TODO: should allow customize keys to avoid coollide with 0,1,2.. and customize amount too
		// create the extra main selector
		vector<int> _keys;
		for (int i = 0; i < groupLinkSize; i++) {
			switch (i)
			{
			case 0: _keys.push_back('0'); break;
			case 1: _keys.push_back('1'); break;
			case 2: _keys.push_back('2'); break;
			case 3: _keys.push_back('3'); break;
			case 4: _keys.push_back('4'); break;
			case 5: _keys.push_back('5'); break;
			case 6: _keys.push_back('6'); break;
			case 7: _keys.push_back('7'); break;
			case 8: _keys.push_back('8'); break;
			case 9: _keys.push_back('9'); break;
			default: break;
			}
		}
		add(params_GroupMainSelector, _keys);
		//add(params_GroupMainSelector, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

		//--

		// TODO:
		// store the main group link slector only!
		int _last = groups.size() - 1;

		GROUP_LINK_Selected_Index.setMax(groupsSizes[_last] - 1);
		GROUP_LINK_Selected_Index.makeReferenceTo(PRESETS_Selected_Index[_last]);// TODO: link

		// excludes all selectors except the main one. the other will be saved as preset
		params_GroupsSelectors.setSerializable(false);
	}

	//--

	PRESETS_Selected_Index_PRE.resize(PRESETS_Selected_Index.size());
	for (int i = 0; i < PRESETS_Selected_Index_PRE.size(); i++)
	{
		PRESETS_Selected_Index_PRE[i] = -1;
	}

	//----

	// ImGui
#ifndef USE_IMGUI_EXTERNAL	
	ImGui_Setup();
#endif

	//--

	// TODO: main group only
#ifdef INCLUDE_ofxUndoSimple
	//undoStringParams = groups[0].toString();

	undoStringsParams.clear();
	undoStringsParams.resize(groups.size());

	undoXmlsParams.clear();
	undoXmlsParams.resize(groups.size());

	for (int i = 0; i < groups.size(); i++)
	{
		undoStringsParams[i] = groups[i].toString();
	}
#endif

	//--

	MODE_EditPresetClicker.set("EDIT CLICKER", false);
	SHOW_BackGround_EditPresetClicker.set("BOX CLICKER", false);

	//----

	bSplitGroupFolders.setSerializable(false);// force to this mode. do not store

	params_Options.add(MODE_Editor);
	params_Options.add(bSplitGroupFolders);

	params_Gui.add(bThemDark);
	params_Gui.add(SHOW_ImGui_Selectors);
	params_Gui.add(SHOW_ImGui_PresetsParams);
	params_Gui.add(SHOW_Help);
	params_Gui.add(MODE_EditPresetClicker);
	params_Gui.add(SHOW_BackGround_EditPresetClicker);

	params_Control.add(params_Options);
	params_Control.add(params_Gui);

	params_Control.add(MODE_EditPresetClicker);
	params_Control.add(bThemDark);

	//--

	// user-kit
	params_UserKitSettings.setName("USER-KIT");
	params_UserKitSettings.add(params_GroupsSelectors);//includes all selectors

	// main group link selector
	int _last = groups.size() - 1;

	// This is a strange situation
	// this is to know if no groups has been added before call setup! 
	if (_last >= 0) {

		GROUP_LINK_Selected_Index.set("GROUP_LINK", 0, 0, groupsSizes[_last] - 1);
		params_UserKitSettings.add(GROUP_LINK_Selected_Index);

		//----

		// user gui selector
		GuiGROUP_Selected_Index.set("GROUP SELECT", 0, 0, groups.size() - 1);
		GuiGROUP_Selected_Index.addListener(this, &ofxPresetsManager::Changed_GuiGROUP_Selected_Index);

		groupRandomizers.resize(groups.size());

		for (int i = 0; i < groups.size(); i++)
		{
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
			//_g.add(groupRandomizers[i].getParamsRandomizers());// all params
			_g.add(groupRandomizers[i].PLAY_RandomizeTimer);// play
			_g.add(groupRandomizers[i].randomizeDurationBpm);// bpm
			//_g.add(groupRandomizers[i].bRandomizeIndex);// random index
			params_GroupsSelectors.add(_g);
		}
	}

	//----

	// custom path
	// this allows to use different kits for the same project/app
	params_Custom.setName("CUSTOM PATH");
	params_Custom.add(bPathDirCustom);
	params_Custom.add(pathDirCustom);
	params_UserKitSettings.add(params_Custom);

	ofAddListener(params_UserKitSettings.parameterChangedE(), this, &ofxPresetsManager::Changed_UserKit);

	//-

	startup();
}

//--------------------------------------------------------------
void ofxPresetsManager::startup()
{
	ofLogNotice(__FUNCTION__);

	//--

	DISABLE_CALLBACKS = false;// enable callbacks after setup

	// load all app session settings & randomizers (not the related presets)
	load_ControlSettings();// here bPathDirCustom is loaded (but if files are present, not in first runtime)

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

	//----

	// preset clicker

	// reset mini previews layout
	float _size = cellSize;
	setSizeBox_PresetClicker(_size);
	float _xx = 200;
	float _yy = ofGetHeight() - getPresetClicker_Height() - 20;

	// layout
	setPosition_PresetClicker(_xx, _yy);

	// clicker_Pos
	_RectClick_Pad = 10;
	_RectClick_w = getGroupNamesWidth();
	//_ratio = ndiReceiveTexture.getHeight() / ndiReceiveTexture.getWidth();

	rectanglePresetClicker.width = getPresetClicker_Width() + 2 * _RectClick_Pad + _RectClick_w;
	rectanglePresetClicker.height = getPresetClicker_Height() + 2 * _RectClick_Pad;
	rectanglePresetClicker.x = getPresetClicker_Position().x - _RectClick_Pad - _RectClick_w;
	rectanglePresetClicker.y = getPresetClicker_Position().y - _RectClick_Pad;
	_rectRatio = rectanglePresetClicker.width / rectanglePresetClicker.height;

	// load settings
	rectanglePresetClicker.loadSettings(path_RectanglePresetClicker, path_UserKit_Folder + "/" + path_ControlSettings + "/", false);
	clicker_Pos.x = rectanglePresetClicker.x + _RectClick_Pad + _RectClick_w;
	clicker_Pos.y = rectanglePresetClicker.y + _RectClick_Pad;

	//clamp inside window
	clicker_Pos.x = ofClamp(clicker_Pos.x, 0, ofGetWidth());
	clicker_Pos.y = ofClamp(clicker_Pos.y, 0, ofGetHeight());

	//--

	////TODO: startup bug that do not load clicker fine..
	//MODE_EditPresetClicker = true;
	////{
	////	clicker_pos.x = rectanglepresetclicker.x;
	////	clicker_pos.y = rectanglepresetclicker.y;
	////}
	//MODE_EditPresetClicker = false;

	//--

	//// TODO: 
	//// memory mode
	//load_AllKit_ToMemory();

	//--

	//// TODO:
	// workflow
	// check if presets folders is empty. then populate all elements if not
	doCheckPresetsFoldersAreEmpty();

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
	//	PRESETS_Selected_Index[_last] = GROUP_LINK_Selected_Index.get();
	//	cout << "PRESETS_Selected_Index[_last] : " << PRESETS_Selected_Index[_last] << endl;
	//}

	//--

	// TODO:
	// refresh in other another better place?...
	buildHelpInfo();

	//-------

	//// avoid troubles when all setup process have not finished property
	//if (groups.size() > 0) bDoneSetup = true;
	//else bDoneSetup = false;
	//if (!bDoneSetup) {
	//	removeKeysListeners();
	//	removeMouseListeners();
	//}
	// TODO:
	bDoneSetup = true;

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
			if (groupRandomizers[i].is_bPopulateAll()) doPopulateFavs(i);
		}

		//-

		//plotters
#ifdef DEBUG_PERFORMANCE_MEASURES 
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
		//if (!MODE_StandalonePresets_NEW && autoSave && bAutosaveTimer && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
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
		//		saveAllKitFromMemory();
		//	}
		//	//-
		//	//auto save timer
		//	timerLast_Autosave = ofGetElapsedTimeMillis();
		//}
	}
}

#ifndef USE_IMGUI_EXTERNAL	
//---------------------------------------------------------------------
void ofxPresetsManager::drawImGui()
{
	ImGui_Draw_WindowBegin();
	{
		// mouse over ImGui engine
		bMouseOver_Changed = false;

		//-

		//ImGui_Draw_Window();
		bImGui_mouseOver = ImGui_Draw_Window();// handling mouse over gui to disable keys callbacks

		//-

		// mouse over ImGui engine
		if (bImGui_mouseOver != bImGui_mouseOver_PRE)
		{
			bImGui_mouseOver_PRE = bImGui_mouseOver;
			bMouseOver_Changed = true;
		}
		if (bMouseOver_Changed)
		{
			ofLogNotice(__FUNCTION__) << "bImGui_mouseOver: " << (bImGui_mouseOver ? "\t\t\t\t > INSIDE" : "< OUTSIDE");
		}
	}
	ImGui_Draw_WindowEnd();
}
#endif

//---------------------------------------------------------------------
void ofxPresetsManager::draw(ofEventArgs & args)
{
	//----

	//user clicker boxes preset selector 
	//(from live kit/favorites)
	if (SHOW_Panel_Click)
	{
		drawPresetClicker();
	}

	//----

	// ImGui

#ifndef USE_IMGUI_EXTERNAL	
	drawImGui();
	//gui_ImGui.draw(); //    <-- Also calls gui.end()

#endif

	//----
}

//--------------------------------------------------------------
void ofxPresetsManager::windowResized(int w, int h)
{
	ofLogVerbose(__FUNCTION__) << w << "," << h;
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
	std::string _label;


	//-

	if (!lastMouseButtonState && ofGetMousePressed())
	{
		mousePressed(ofGetMouseX(), ofGetMouseY());
	}
	lastMouseButtonState = ofGetMousePressed();

	//-

	//// light theme (black lines)
	//if (!bThemDark)
	//{
	//	_colorText = ofColor(0, 255);
	//	_colorButton = ofColor(0, 128);
	//	_colorBg = ofColor(225, 64);
	//}

	//// dark theme (white lines)
	//else
	//{
	//	_colorText = ofColor(255, 150);
	//	_colorButton = ofColor(16, 225);
	//	_colorBg = ofColor(0, 128);
	//}

	// display help info layout
	bool bLateralPosition = false;// false = on top of clicker
	bool bLeftPosition = true;// left or right. only if lateral pos true

	//----

	ofPushStyle();

	//----

	// bg rectangle editor
	if (SHOW_BackGround_EditPresetClicker)
	{
		ofFill();
		ofSetColor(_colorBg);
		rectanglePresetClicker.draw();
		ofDrawRectRounded(rectanglePresetClicker, _round);
	}

	// get clicker position from being edited rectangle
	if (MODE_EditPresetClicker)
	{
		_RectClick_w = getGroupNamesWidth();
		clicker_Pos.x = rectanglePresetClicker.x + _RectClick_Pad + _RectClick_w;
		clicker_Pos.y = rectanglePresetClicker.y + _RectClick_Pad;
		//rectanglePresetClicker.width = MIN(getPresetClicker_Width() + 2 * _RectClick_Pad + _RectClick_w, 1000);
		//rectanglePresetClicker.height = rectanglePresetClicker.width / _rectRatio;
	}

	//--

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

		//if (bBuildGroupSelector && bAllowGroupSelector) _i = groups.size() - 1;
		//else if (groups.size() > 1) _i = 1;
		////else if (groups.size() > 1) _i = 0;
		//else _i = 0;

		_i = groups.size() - 1;

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
			std::string info = groups[i].getName();

			// mark selected group. useful to navigate with arrow keys
			if (i == GuiGROUP_Selected_Index.get() && groups.size() > 0 && ENABLE_Keys.get()) info = "* " + info;

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

			if (!bThemDark) ofSetColor(_colorBg);// shadow
			else ofSetColor(_colorButton);// shadow
			if (myFont.isLoaded()) myFont.drawString(info, xG + gap, ySave + gap);
			else ofDrawBitmapString(info, xG + gap, ySave + gap);

			ofSetColor(_colorText);
			if (myFont.isLoaded()) myFont.drawString(info, xG, ySave);
			else ofDrawBitmapString(info, xG, ySave);
		}

		//--

		// 8. help info text: 
		drawHelp(0, ySave);
	}

	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
ofxPresetsManager::~ofxPresetsManager()
{
	// TODO: not sure if can avoid call manually exit(), bc here groups could be externally destroyed..
	// so we would prefer to call presetsManager.exit() manually on the first place sorting.
	// if exit() is called manually on app exit(), we will call it twice... Could be a problem?
	//exit();
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::clear() {
	groups.clear();
	keys.clear();
	groupsSizes.clear();
	PRESETS_Selected_Index.clear();
	params_GroupMainSelector.clear();
	params_GroupsSelectors.clear();
	PRESETS_Selected_Index_PRE.clear();

	//params_Options.clear();
	//params_Gui.clear();
	//params_Control.clear();

	params_UserKitSettings.clear();
	groupRandomizers.clear();
	params_Custom.clear();
}

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
	//params_GroupsSelectors.add(PRESETS_Selected_Index[_size]);// add this new param (lastone)

	//-

	ofLogNotice(__FUNCTION__) << "group " << (_size) << " : " << groups[_size].getName();
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup _params, vector<int> _keysList)
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

////--------------------------------------------------------------
//void ofxPresetsManager::save(int presetIndex, int guiIndex)
//{
//	ofLogVerbose(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex;
//
//	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
//		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
//	{
//		// MODE A. it's important if this line is before or after ofSerialize
//		//DONE_save = true;
//
//		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
//
//		bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
//
//		if (b) ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex << " " << _path;
//		else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
//
//		//-
//
//		// callback
//		// MODE A. it's important if this line is before or after ofSerialize
//		ofLogVerbose(__FUNCTION__) << "DONE_save";
//		DONE_save = true;
//
//		//simple callback
//		bIsDoneSave = true;
//	}
//	else
//	{
//		ofLogError(__FUNCTION__) << "Group or Preset out of ranges! preset: " << ofToString(presetIndex) << " group: " << ofToString(guiIndex);
//	}
//}

//--------------------------------------------------------------
void ofxPresetsManager::save(int presetIndex, int guiIndex)
{
	ofLogVerbose(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex;

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		// MODE A. it's important if this line is before or after ofSerialize
		//DONE_save = true;

		std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);

		bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);

		if (b) ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << guiIndex << " preset: " << presetIndex << " " << _path;
		else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;

		//-

		// callback
		// MODE A. it's important if this line is before or after ofSerialize
		ofLogVerbose(__FUNCTION__) << "DONE_save";
		DONE_save = true;

		//simple callback
		bIsDoneSave = true;
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
void ofxPresetsManager::save(int presetIndex, std::string gName)
{
	int guiIndex = getGuiIndex(gName);

	ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " group: " << gName << " preset: " << presetIndex;

	if (((guiIndex >= 0) && (guiIndex < (int)groups.size())) &&
		(presetIndex >= 0) && (presetIndex < groupsSizes[guiIndex]))
	{
		//-

		// callback
		// MODE A. it's important if this line is before or after ofSerialize
		ofLogVerbose(__FUNCTION__) << "DONE_save";
		DONE_save = true;

		//simple callback
		bIsDoneSave = true;

		//-

		TS_START("SAVE FILE 2");//for TimeMeasurements only
		std::string _path = getPresetPath(gName, presetIndex);
		bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
		if (b) ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " " << guiIndex << " " << _path;
		else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
		TS_STOP("SAVE FILE 2");//for TimeMeasurements only

		//-
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

		// TODO: already marked
		// mark selected
		PRESETS_Selected_Index[guiIndex] = presetIndex;

		//-

		// callback
		// MODE A. it's important if this line is before or after ofSerialize
		ofLogVerbose(__FUNCTION__) << "DONE_load";
		DONE_load = true;

		//simple callback
		bIsDoneLoad = true;
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
void ofxPresetsManager::load(int presetIndex, std::string gName)
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

			// file not found so create one file instead
			save(presetIndex, guiIndex);
		}

		// TODO: already marked
		// mark selected
		PRESETS_Selected_Index[guiIndex] = presetIndex;

		//-

		// callback
		// MODE A. it's important if this line is before or after ofSerialize
		ofLogVerbose(__FUNCTION__) << "DONE_load";
		DONE_load = true;

		//simple callback
		bIsDoneLoad = true;
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
	ofLogNotice(__FUNCTION__) << "group: " << groupIndex << " preset: " << presetIndex;
	save(presetIndex, groupIndex);
}

//--------------------------------------------------------------
void ofxPresetsManager::loadPreset(int presetIndex, int groupIndex)
{
	if (groupIndex == -1) groupIndex = groups.size() - 1;// set last group that is the group link selector
	ofLogNotice(__FUNCTION__) << "group: " << groupIndex << " preset: " << presetIndex;
	load(presetIndex, groupIndex);
}

//--

// engine helpers
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
int ofxPresetsManager::getPresetIndex(std::string gName) const
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
int ofxPresetsManager::getGuiIndex(std::string name) const
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
std::string ofxPresetsManager::getPresetPath(std::string _gName, int _presetIndex)
{
	std::string _pathFolder;
	std::string _pathFilename;
	std::string _path;

	_pathFolder = path_UserKit_Folder + "/" + path_PresetsFavourites + "/";

	// append group name to subfolder files by each parameter group
	if (bSplitGroupFolders) _pathFolder += _gName + "/";

	_pathFilename = _gName + filenamesPrefix + ofToString(_presetIndex);
	_path = _pathFolder + _pathFilename;
	_path += fileExtension;

	ofLogVerbose(__FUNCTION__) << "group name: " << _gName << " path: " << _path;

	return _path;
}

//--------------------------------------------------------------
std::string ofxPresetsManager::getGroupPath(std::string _gName)
{
	std::string _pathFolder;

	_pathFolder = path_UserKit_Folder + "/" + path_PresetsFavourites + "/";

	// append group name to subfolder files by each parameter group
	if (bSplitGroupFolders) _pathFolder += _gName + "/";

	ofLogVerbose(__FUNCTION__) << "group name: " << _gName << " path: " << _pathFolder;

	return _pathFolder;
}

//--------------------------------------------------------------
std::string ofxPresetsManager::getGroupPath(int _index)
{
	std::string _pathFolder;
	std::string _gName;
	_gName = getGroupPath(groups[_index].getName());
	if (_index < groups.size()) _pathFolder = _gName;
	else return "UNKNOWN";

	//// append group name to subfolder files by each parameter group
	//if (bSplitGroupFolders) _pathFolder += _gName + "/";

	ofLogVerbose(__FUNCTION__) << "group path: " << _pathFolder;

	return _pathFolder;
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
	if (bDoneSetup)
	{
		const int key = eventArgs.key;

		// modifiers
		bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
		bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
		bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
		bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

		bool bDebug = false;
		if (bDebug)
		{
			ofLogNotice(__FUNCTION__)
				<< " key: " << key
				<< " mod_CONTROL: " << mod_CONTROL
				<< " mod_SHIFT: " << mod_SHIFT
				<< " mod_ALT: " << mod_ALT
				//<< " mod_COMMAND: " << mod_COMMAND
				;
		}

		// TODO: TEST: to force disable engine
		//bImGui_mouseOver = false;

		// always listening to K to avoid can be blocked
		// restore keys control
		if (key == 'K' && !bImGui_mouseOver_PRE)
		{
			ENABLE_Keys = !ENABLE_Keys;
		}

		else if (bKeys && ENABLE_Keys && !bImGui_mouseOver_PRE)// disable keys when mouse over gui
		{
			//-

			// mode key for saving with mouse or trigger keys
			if (key == modeKeySave && !bModKeySave)
			{
				bModKeySave = true;
				ofLogNotice(__FUNCTION__) << "modKey Save TRUE";
				//return;
			}

			// mode key for swap with mouse or trigger keys
			else if (key == modKeySwap && !bModKeySwap)
			{
				bModKeySwap = true;
				ofLogNotice(__FUNCTION__) << "modKey Swap TRUE";
				//return;
			}

			//--

			if (false) {}

			// hide/show control gui
			else if ((key == 'g' || key == 7) && (mod_CONTROL && !mod_ALT && !mod_SHIFT))
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
			else if (key == 'P')
			{
				setVisible_PresetClicker(!isVisible_PresetClicker());
			}

			//----

			// randomizers

			// play
			// timer to randomize and choice a random preset from all the available kit
			// using a probability and a shorter mode for some presets.
			else if ((mod_CONTROL && !mod_ALT) && key == ' ')
			{
				setTogglePlayRandomizerPreset(GuiGROUP_Selected_Index);
			}

			// randomize enable parameters (look into randomizers panel) to the current selected preset
			else if ((mod_CONTROL && !mod_ALT) && (key == 'R' || key == 18))
			{
				doRandomizePresetSelected(GuiGROUP_Selected_Index);

				// worfklow
				// store current point to undo history
				doStoreUndo();
			}

			//else if ((mod_CONTROL && !mod_ALT)  && key == 'e')
			//{
			//	MODE_EditPresetClicker = !MODE_EditPresetClicker;
			//}
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

			// TODO: not working on windows.. We need to add int code
#ifdef INCLUDE_ofxUndoSimple
			if (MODE_Editor.get())
			{
				if (!mod_SHIFT && mod_CONTROL && (key == 'z' || key == 26))// previous
				{
					doUndo();
				}
				else if (mod_SHIFT && mod_CONTROL && (key == 'Z' || key == 26))// next
				{
					doRedo();
				}
				else if (mod_SHIFT && mod_CONTROL && key == 'C' || key == 3)// clear
				{
					doClearUndoHistory();
				}
				else if (!mod_SHIFT && mod_CONTROL && key == 's' || key == 19)// store
				{
					doStoreUndo();
				}
			}
#endif

			//----

			// navigate kit/favorites presets
			if (ENABLE_KeysArrowBrowse && (!mod_CONTROL && !mod_ALT))
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
			//if (!mod_CONTROL && !mod_SHIFT && !mod_ALT)// exclude controls do notr works bc blocks the mod keys to copy/swap presets...
			{
				for (int g = 0; g < keys.size(); g++)
				{
					for (int k = 0; k < keys[g].size(); k++)// performs all registered keys: one for each [8] preset
					{
						if (key == keys[g][k])
						{
							ofLogNotice(__FUNCTION__) << "key: " << (char)key;

							if (bModKeySave)
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

		//--

		//// randomizers
		//if (MODE_Editor.get())
		//{
		groupRandomizers[GuiGROUP_Selected_Index].keyPressed(key);

		//	// workflow
		//	// workaround
		//	if (key == 'R')
		//		doStoreUndo();
		//}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::keyReleased(ofKeyEventArgs &eventArgs)
{
	if (bDoneSetup)
	{
		// mod keys
		if (eventArgs.key == modeKeySave && ENABLE_Keys && bModKeySave)
		{
			bModKeySave = false;
			ofLogNotice(__FUNCTION__) << "modKey Save FALSE";
		}
		else if (eventArgs.key == modKeySwap && ENABLE_Keys && bModKeySwap)
		{
			bModKeySwap = false;
			ofLogNotice(__FUNCTION__) << "modKey Swap FALSE";
		}
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

//--------------------------------------------------------------
void ofxPresetsManager::removeMouseListeners()
{
	//ofRemoveListener(ofEvents().mousePressed, this, &ofxPresetsManager::mousePressed);
}

//----

//-----------------------------------------------------
void ofxPresetsManager::mousePressed(int x, int y)
{
	if (bDoneSetup)
	{
		// this method will get what box button is pressed: in wich group-row (y) and wich preset (x)

		x = x - clicker_Pos.x;
		y = y - clicker_Pos.y;

		// get what matrix button has been just clicked
		int xIndex = x / cellSize;
		int yIndex = y / cellSize;

		// Index -1 for out (left) of boxes
		xIndex = (x > 0) ? xIndex : -1;
		yIndex = (y > 0) ? yIndex : -1;

		//-

		// 1. presets buttons & save button

		// click is iniside allowed presets/groups
		if ((yIndex >= 0) && (yIndex < (int)groups.size()) && groups.size() > 0)// check if valid group
		{
			// avoid outer panel logs. only into the group row levels
			if ((xIndex != -1) && (yIndex != -1) &&// valid preset. to the left of panels
				(xIndex >= 0) && (xIndex < groupsSizes[yIndex]))// valid group. to the right of panels

				ofLogNotice(__FUNCTION__) << groups[yIndex].getName() << " group: " << yIndex << " preset: " << xIndex;

			//-

			if ((xIndex >= 0) && (xIndex < groupsSizes[yIndex]))
			{
				// 1. mod save controlled by modeKeySave

				if (bModKeySave)
				{
					ofLogNotice(__FUNCTION__) << "SAVE";

					save(xIndex, yIndex);

					// will auto load and set the already clicked preset button
					if (yIndex < PRESETS_Selected_Index.size())
					{
						PRESETS_Selected_Index[yIndex] = xIndex;
					}
				}

				//-

				// 2. mod swap controlled by modKeySwap

				else if (bModKeySwap)
				{
					//ofLogNotice(__FUNCTION__) << "SWAP";

					if (yIndex < PRESETS_Selected_Index.size())// a valid group index
					{
						//// autosave first from same clicked group
						//if (autoSave)
						//{
						//	ofLogNotice(__FUNCTION__) << "Autosave first";
						//	save(PRESETS_Selected_Index[yIndex], yIndex);
						//	//save(PRESETS_Selected_Index_PRE[yIndex], yIndex);
						//}

						// swap
						doSwap(yIndex, PRESETS_Selected_Index[yIndex], xIndex);// group index, from, to
					}
				}

				//-

				// 3. no mod keys: normal load (not any key modifier pressed)

				else
				{
					ofLogNotice(__FUNCTION__) << "LOAD";

					if (yIndex < PRESETS_Selected_Index.size())
						PRESETS_Selected_Index[yIndex] = xIndex;
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

		if (groups.size() > 0)
		{
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
	}
}

//----

// helpers 

//--------------------------------------------------------------
void ofxPresetsManager::doCloneRight(int gIndex)
{
	int pIndex = PRESETS_Selected_Index[gIndex];

	//auto save current preset
	//if (autoSave)
	{
		ofLogVerbose(__FUNCTION__) << "autosave preset: " << pIndex;
		save(pIndex, gIndex);
	}
	ofLogNotice(__FUNCTION__) << "on group: " << gIndex << " from preset: " << pIndex;

	//CheckAllFolders();

	for (int i = pIndex + 1; i < groupsSizes[gIndex]; i++)
	{
		save(i, gIndex);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doCloneAll(int gIndex)
{
	ofLogNotice(__FUNCTION__) << "on group: " << gIndex;

	//CheckAllFolders();

	//auto save current preset
	//if (autoSave)
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

//--------------------------------------------------------------
void ofxPresetsManager::doPopulateFavsALL()
{
	ofLogNotice(__FUNCTION__) << "on all groups";

	// TODO:

	////CheckAllFolders();//? required

	//for (int i = 0; i < groups.size(); i++)
	//{
	//	doRandomizePresetSelected(i);
	//	doCloneAll(i);
	//}
}

//--------------------------------------------------------------
void ofxPresetsManager::doPopulateFavs(int groupIndex)
{
	ofLogNotice(__FUNCTION__) << "on group: " << groupIndex;

	if (groupIndex == -1)
	{
		ofLogError(__FUNCTION__) << "groupIndex " << groupIndex;
		return;
	}

	// TODO:

	//CheckAllFolders();//? required

	//doCloneAll(gIndex);

	//for (int i = 0; i < groupsSizes[gIndex]; i++)
	//{
	//	doRandomizePresetSelected(i);
	//	doCloneAll(i);
	//}
}

//--------------------------------------------------------------
void ofxPresetsManager::doSwap(int groupIndex, int _srcIndex, int _destIndex)
{
	ofLogNotice(__FUNCTION__)
		<< "group: #" << groupIndex << " "
		<< groups[groupIndex].getName() << " : "
		<< _srcIndex << " <-> "
		<< _destIndex;

	std::string _path_Src = getPresetPath(groups[groupIndex].getName(), _srcIndex);
	std::string _path_Dst = getPresetPath(groups[groupIndex].getName(), _destIndex);

	ofLogNotice(__FUNCTION__) << "From: " << _path_Src;
	ofLogNotice(__FUNCTION__) << "\t To: " << _path_Dst;

	// 0. rename dest file to temp file
	std::string _path_TEMP = "TEMP_Src.xml";
	ofFile _file;
	_file.open(_path_Dst);
	_file.renameTo(_path_TEMP);

	// 1. save current (src) to dest
	ofXml _settings;
	ofSerialize(_settings, groups[groupIndex]);
	_settings.save(_path_Dst);

	// 2. load dest (now is already src)
	PRESETS_Selected_Index[groupIndex] = _destIndex;
	loadPreset(_destIndex, groupIndex);

	// 3. restore TEMP (dst) to src
	_file.renameTo(_path_Src);
}

//--------------------------------------------------------------
void ofxPresetsManager::setToggleKeysControl(bool active)
{
	bKeys = active;
}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_UserKit(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		std::string name = e.getName();

		if ((name != "exclude") && (name != "%"))
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

						// 1. group selected preset NOT CHANGED

						if (PRESETS_Selected_Index[g] == PRESETS_Selected_Index_PRE[g])
						{
							ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " preset " << p << " not changed";

							// browser
							//if (MODE_StandalonePresets_NEW)
							//{
							//	if (autoLoad)
							//	{
							//		load(PRESET_Selected_IndexMain, g);
							//	}
							//}
							//bMustTrig = true;
						}

						//--

						// 2. group selected preset CHANGED

						else if (PRESETS_Selected_Index[g] != PRESETS_Selected_Index_PRE[g])
						{
							ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " preset " << p << " changed";

							//-

							// workflow
							// browser mode bypasses autosave
							//if (autoSave && !MODE_StandalonePresets_NEW)
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
		std::string name = e.getName();

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

		// mode edit: when false, we disabled autosave to allow faster performance ! 
		else if (name == MODE_Editor.getName())
		{
			ofLogNotice(__FUNCTION__) << "MODE EDITOR: " << (MODE_Editor.get() ? "TRUE" : "FALSE");

			autoSave = MODE_Editor.get();

			//--

			for (int i = 0; i < groups.size(); i++) {
				groupRandomizers[i].setModeEditor(MODE_Editor);
			}

			if (!MODE_Editor) MODE_EditPresetClicker = false;
		}

		//-

		if (name == bThemDark.getName())
		{	// light theme (black lines)
			if (!bThemDark)
			{
				_colorText = ofColor(0, 255);
				_colorButton = ofColor(0, 128);
				_colorBg = ofColor(225, 64);
			}

			// dark theme (white lines)
			else
			{
				_colorText = ofColor(255, 150);
				_colorButton = ofColor(16, 225);
				_colorBg = ofColor(0, 128);
			}
		}

		if (name == MODE_EditPresetClicker.getName())
		{
			if (MODE_EditPresetClicker.get())
			{
				rectanglePresetClicker.enableEdit();

				// workflow
				//SHOW_BackGround_EditPresetClicker = true;
				if (!SHOW_Panel_Click) SHOW_Panel_Click = true;
			}
			else
			{
				rectanglePresetClicker.disableEdit();
			}
		}

		//----

		//TODO:
		//memmory mode. not implemented...
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
			saveAllKitFromMemory();
		}
		//else if (name == "MODE MEMORY")// TODO: not implemented
		//{
		//	ofLogNotice(__FUNCTION__) << "MODE MEMORY: " << e;
		//	if (MODE_MemoryLive)
		//	{
		//		//reload all xml preset files to memory
		//		load_AllKit_ToMemory();
		//	}
		//	else
		//	{
		//		//save all xml preset files to disk from memory
		//		saveAllKitFromMemory();
		//	}
		//}

		//--

		//// workflow
		//else if (name == SHOW_ImGui_PresetsParams.getName())
		//{
		//	if (SHOW_ImGui_PresetsParams) SHOW_ImGui_Selectors = false;
		//}
		//else if (name == SHOW_ImGui_Selectors.getName())
		//{
		//	if (SHOW_ImGui_Selectors) SHOW_ImGui_PresetsParams = false;
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
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load_ControlSettings()
{
	//--

	// control settings

	std::string path = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_ControlSettings;

	bool b = ofxSurfingHelpers::loadGroup(params_Control, path);

	ofLogNotice(__FUNCTION__) << "Loaded " << path << " " << (b ? "DONE" : "FAILED");
	if (!b) ofLogError(__FUNCTION__) << "FILE '" << path << "' NOT FOUND!";

	//--

	// user settings

	// TODO:
	// testing to enable here to avoid create empty xml settings on global root?
	DISABLE_CALLBACKS = false;

	std::string path3 = filenameMainSettings;

	b = ofxSurfingHelpers::loadGroup(params_UserKitSettings, path3);

	//if (!b) ofLogError(__FUNCTION__) << "CANT LOAD FILE '" << path3 << "'!";
	//else ofLogNotice(__FUNCTION__) << "LOADED " << path3;

	//--

	// TODO: update selectors
	for (int i = 0; i < PRESETS_Selected_Index.size(); i++)
	{
		if (i < PRESETS_Selected_Index_PRE.size())
			PRESETS_Selected_Index_PRE[i] = PRESETS_Selected_Index[i];
		else ofLogError(__FUNCTION__) << "Out of Range: PRESETS_Selected_Index_PRE '" << i << "'!";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::save_ControlSettings()
{
#ifndef DEBUG_BLOCK_SAVE_SETTINGS

	DISABLE_CALLBACKS = true;

	//---

	// TODO: test to avoid crashes..?
	//try
	{
		// save randomizers settings
		ofLogVerbose(__FUNCTION__) << endl << params_Control.toString() << endl;

		std::string path;
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
	//catch (int n)
	//{
	//	ofLogError(__FUNCTION__) << "CATCH ERROR " << n << endl;
	//	throw;
	//}

	//----

	DISABLE_CALLBACKS = false;
#else
	ofLogNotice(__FUNCTION__) << "[DEBUG] BLOCKED save_ControlSettings()";
#endif
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::setPath_UserKit_Folder(std::string folder)
{
	ofLogNotice(__FUNCTION__) << folder;

	path_UserKit_Folder = folder;

	CheckFolder(folder);

	// create the folders of each group into main presets folder 
	std::string _path;
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
void ofxPresetsManager::setPath_PresetsFavourites(std::string folder)
{
	ofLogNotice(__FUNCTION__) << folder;
	path_PresetsFavourites = folder;
	CheckFolder(path_UserKit_Folder + "/" + path_PresetsFavourites);
}

//--------------------------------------------------------------
void ofxPresetsManager::setPath_PresetsStandalone(std::string folder)
{
	ofLogNotice(__FUNCTION__) << folder;
	path_PresetsStandalone = folder;
	CheckFolder(path_UserKit_Folder + "/" + path_PresetsStandalone);
}

//--

// memory mode 
// (loaded from data vector instead of hd files)
//--------------------------------------------------------------
void ofxPresetsManager::saveAllKitFromMemory()
{
	ofLogVerbose(__FUNCTION__);

	for (int i = 0; i < mainGroupMemoryFilesPresets.size(); i++)
	{
		std::string _folder, _file, _path;

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

		std::string pathFolder, pathFilename, pathComplete;
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

	//// TODO: not implemented
	//// MODE B: direct from memory
	//if (MODE_MemoryLive && autoSave)
	//{
	//	saveAllKitFromMemory();// update of files is required bc presets are on memmory only
	//}

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

	rectanglePresetClicker.saveSettings(path_RectanglePresetClicker, path_UserKit_Folder + "/" + path_ControlSettings + "/", false);

	//--

	// TODO: debug crash
	//gui_ImGui.exit();
}

//--

#pragma mark - IM_GUI

#ifndef USE_IMGUI_EXTERNAL
//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Setup()
{
	ofLogNotice(__FUNCTION__);

	//--

	// font customize
#ifdef INCLUDE_IMGUI_CUSTOM_THEME_AND_FONT
	ofxSurfingHelpers::ImGui_FontCustom();
#endif

	//--

	//gui_ImGui.enableDocking();

	gui_ImGui.setup();
	//gui_ImGui.setup(false, true); // No autodraw, Allow chaining

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	//--

	// theme customize
#ifdef INCLUDE_IMGUI_CUSTOM_THEME_AND_FONT
	ofxSurfingHelpers::ImGui_ThemeMoebiusSurfing();
	//ofxSurfingHelpers::ImGui_ThemeModernDark();
#endif

	//--

	// mouse over
	bImGui_mouseOver.set("mouseOverGui", false);

	//--
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_WindowBegin()
{
	//-

	gui_ImGui.begin();

	//-
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_WindowEnd()
{
	//--

	gui_ImGui.end();

	//--
}
#endif

//#ifdef USE_IMGUI_EXTERNAL
//inline void ofxPresetsManager::ImGui_Draw_Window()
//#else
//void ofxPresetsManager::ImGui_Draw_Window()
//#endif

//--------------------------------------------------------------
//void ofxPresetsManager::ImGui_Draw_Window()
bool ofxPresetsManager::ImGui_Draw_Window()
{
	ofxImGui::Settings settings;

	if (SHOW_ImGui)
	{
		bool b = false;

		// main panel
		ImGui_Draw_MainPanel();// main control + extra

		//-

		// group selector
		if (SHOW_Panel_AllSelectors)
		{
			// hide this panels when no more than one group! it's for multi groups
			if (groups.size() > 1)
			{
				if (ofxImGui::BeginWindow("Group select", settings, &b))
				{
					ImGui::Dummy(ImVec2(0.0f, 5));

					// main group link selector
					if (bBuildGroupSelector) ofxImGui::AddParameter(PRESETS_Selected_Index[groups.size() - 1]);// last group is the main link group

					//-

					ImGui::Dummy(ImVec2(0.0f, 5));

					// selector to show their randomizers
					if (bBuildGroupSelector) ofxImGui::AddParameter(GuiGROUP_Selected_Index);// user selected wich group to edit

					//-

					//ImGui::Dummy(ImVec2(0.0f, 5));

					// name of selected group
					std::string str;
					str = "Group Name";
					ImGui::Text(str.c_str());
					str = PRESETS_Selected_Index[GuiGROUP_Selected_Index].getName();
					ImGui::Text(str.c_str());
				}
				ofxImGui::EndWindow(settings);
			}
		}

		// all group selectors to set current preset
		if (SHOW_Panel_AllSelectors) ImGui_Draw_GroupsSelectors();

		//--

		// all parameters from all groups
		if (SHOW_Panel_AllParameter) ImGui_Draw_PresetParameters();

		// selected randomizers for selected group
		// handles matrix button selector too
		ImGui_Draw_Randomizers();

		// standalone presets browser
		//if (MODE_Editor)
		{
			ImGui_Draw_StandalonePresets();
		}
	}

	return settings.mouseOverGui;
}

// ImGui
//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_PresetParameters()
{
	ofxImGui::Settings settings;

	if (ofxImGui::BeginWindow("All Parameters", settings, false))
	{
		//ofxImGui::AddDrag(PRESETS_Selected_Index[0]);

		for (int i = 0; i < groups.size(); i++)
		{
			ofxImGui::AddGroup(groups[i], settings);
		}
	}
	ofxImGui::EndWindow(settings);
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_GroupsSelectors()
{
	ofxImGui::Settings settings;

	if (ofxImGui::BeginWindow("Group Selectors", settings, false))
	{
		ofxImGui::AddGroup(params_GroupsSelectors, settings);
	}
	ofxImGui::EndWindow(settings);
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_MainPanel()
{
	ofxImGui::Settings settings;

	bool b = true;

	if (ofxImGui::BeginWindow("Main Panel", settings, false))
	{
		//---

		ImGui::Dummy(ImVec2(0.0f, 5));

		// label for User-Kit folder
		std::string str;
		str = "User-Kit";
		ImGui::Text(str.c_str());
		str = path_UserKit_Folder;
		ImGui::Text(str.c_str());

		ImGui::Dummy(ImVec2(0.0f, 5));

		//--

		// mode edit
		//ofxImGui::AddParameter(MODE_Editor);
		ofxSurfingHelpers::AddBigToggle(MODE_Editor, 30, "EDIT MODE", "LIVE MODE");
		//ofxSurfingHelpers::AddBigToggle(MODE_Editor, 30); // TODO: repair method. collides when multiple toggles..

		//--

#ifdef INCLUDE_ofxUndoSimple
		if (MODE_Editor.get())
		{
			// undo engine
			ImGui::Dummy(ImVec2(0.0f, 5));
			str = "UNDO ENGINE";
			ImGui::Text(str.c_str());
			str = "History: " + ofToString(undoStringsParams[GuiGROUP_Selected_Index].getUndoLength()) + "/";
			str += ofToString(undoStringsParams[GuiGROUP_Selected_Index].getRedoLength());
			ImGui::Text(str.c_str());

			if (ImGui::Button("Store"))
			{
				doStoreUndo();
			}
			ImGui::SameLine();
			if (ImGui::Button("Undo"))
			{
				doUndo();
			}
			ImGui::SameLine();
			if (ImGui::Button("Redo"))
			{
				doRedo();
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear History"))
			{
				doClearUndoHistory();
			}
		}
#endif

		ImGui::Dummy(ImVec2(0.0f, 5));

		ofxImGui::AddParameter(ENABLE_Keys);//ImGui::SameLine(); 

		ImGui::Dummy(ImVec2(0.0f, 5));

		//---

		// panels
		//if (MODE_Editor)
		{
			if (ImGui::TreeNode("PANELS"))
			{
				// handled by docker mode
				//if (bBuildGroupSelector) ofxImGui::AddParameter(SHOW_ImGui_Selectors);
				//ofxImGui::AddParameter(SHOW_ImGui_PresetsParams);

				ofxImGui::AddParameter(SHOW_Panel_Click);//ImGui::SameLine();
				ofxImGui::AddParameter(SHOW_Panel_AllParameter);//ImGui::SameLine();
				ofxImGui::AddParameter(SHOW_Panel_StandalonePresets);//ImGui::SameLine();

				//if (MODE_Editor)
				{
					ofxImGui::AddParameter(SHOW_Panel_Randomizer);//ImGui::SameLine();
					ofxImGui::AddParameter(SHOW_Panel_AllSelectors);//ImGui::SameLine();
				}
				ofxImGui::AddParameter(SHOW_Help);//ImGui::SameLine();

				//ofxImGui::AddParameter(MODE_StandalonePresets_NEW);

				ImGui::TreePop();
			}
		}

		// extra
		if (MODE_Editor) ImGui_Draw_Extra();

		//--
	}
	ofxImGui::EndWindow(settings);
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Extra()
{
	if (ImGui::TreeNode("EXTRA")) {

		//-

		if (ImGui::TreeNode("ADVANCED"))
		{
			ofxImGui::AddParameter(ENABLE_Keys);
			ofxImGui::AddParameter(autoSave);//ImGui::SameLine();
			//ofxImGui::AddParameter(autoLoad);//ImGui::SameLine();
			// TODO: not implemented
			//ofxImGui::AddParameter(MODE_MemoryLive);//ImGui::SameLine();
			//ofxImGui::AddParameter(MODE_EditPresetClicker);
			//ofxImGui::AddParameter(SHOW_Help);//ImGui::SameLine();
			ofxImGui::AddParameter(bThemDark);

			if (MODE_Editor)
			{
				ofxImGui::AddParameter(MODE_EditPresetClicker); ImGui::SameLine();
				ofxImGui::AddParameter(SHOW_BackGround_EditPresetClicker);
			}

			ImGui::TreePop();
		}

		//--

		if (MODE_Editor)
		{
			if (ImGui::TreeNode("USER-KIT"))
			{
				// User-Kit name
				std::string str;
				str = "Name:\n";
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
		//ofxImGui::Settings settings;
		//		// show ALL the addon internal params! mainly to debug all settings or to use without ImGui..
		//		ofxImGui::AddGroup(params_Control, settings);
		//	}
		//	ImGui::TreePop();
		//}

		ImGui::TreePop();
	}
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::buildHelpInfo() {

	// build help info

	helpInfo = "";
	helpInfo += "USER-KIT\n";
	//helpInfo += "  ";
	helpInfo += displayNameUserKit;

	//	// file format
	//	helpInfo += "       ";
	//#ifdef USE_XML
	//	helpInfo += ".xml";
	//#else
	//#ifdef USE_JSON
	//	helpInfo += ".json";
	//#endif
	//#endif

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
	helpInfo += "H                HELP\n";
	helpInfo += "Ctrl+g           GUI\n";
	helpInfo += "P                CLICKER\n";
	helpInfo += "Keys&Mouse       LOAD\n";
	helpInfo += "+Ctrl            SAVE/COPY\n";
	helpInfo += "+Alt             SWAP\n";
	helpInfo += "Arrows           NAVIGATE\n";
	helpInfo += "E                EDIT/LIVE\n";
	helpInfo += "Ctrl+Space       PLAY RANDOMIZER\n";
	helpInfo += "Ctrl+R           RANDOMIZE PRESET\n";
	helpInfo += "Ctrl+Z           UNDO\n";
	helpInfo += "Ctrl+Shift+Z     REDO\n";
	helpInfo += "Ctrl+C           CLEAR HISTORY\n";
	helpInfo += "Ctrl+s           STORE POINT\n";
}

//--------------------------------------------------------------
void ofxPresetsManager::buildDefaultUserKit() {
	setPath_UserKit_Folder(path_UserKit_Folder);// overwrites the path name but also creates folders if required

	//--

	// standalone presets

	int _size = groups.size();

	standaloneFileNames.clear();
	standaloneFiles.clear();
	standaloneFileIndex.clear();
	standaloneNamePresetDisplay.clear();

	standaloneFileNames.resize(_size);
	standaloneFiles.resize(_size);
	standaloneFileIndex.resize(_size);
	standaloneNamePresetDisplay.resize(_size);

	for (int i = 0; i < _size; i++) {
		standaloneNamePresetDisplay[i] = "NO_NAME_PRESET";

		doStandalonePresetsBuild(i);
	}

	//--

	// user-kit
	// is a bigger session preset
	// will contain all the groups, all the settings
	// then we can load different user-kits on same project.
	// reduce path to name only (to display purposes only)
	std::string str = path_UserKit_Folder;
	std::string temp = R"(\)";//use '\' as splitter...should use '/' too bc Windows/macOS compatibility..
	auto ss = ofSplitString(path_UserKit_Folder, temp);
	displayNameUserKit = ss[ss.size() - 1];

	//--

	// build help info
	buildHelpInfo();
}

//--------------------------------------------------------------
void ofxPresetsManager::buildCustomUserKit() {

	setPath_UserKit_Folder(pathDirCustom);// creates subfolders for each group

	//-

	// reduce to name only
	std::string str = path_UserKit_Folder;
	std::string temp = R"(\)";// use '\' as splitter...should use '/' too bc Windows/macOS compatibility..
	auto ss = ofSplitString(path_UserKit_Folder, temp);
	displayNameUserKit = ss[ss.size() - 1];// get last word and use as name
	//https://openframeworks.cc/documentation/utils/ofFilePath/
	//removeTrailingSlash(...) "images/" -> "images".

	//-

	// TODO:
	// load standalone presets for browser gui
	doStandalonePresetsBuild();

	//-

	// build help info
	buildHelpInfo();
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
	//std::string path2;
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
void ofxPresetsManager::ImGui_Draw_StandalonePresets()
{
	if (SHOW_Panel_StandalonePresets)
	{
		bool b = true;

		ofxImGui::Settings settings;

		if (ofxImGui::BeginWindow("Standalone Presets", settings, &b))
		{
			int groupIndex = GuiGROUP_Selected_Index.get();
			int _numfiles = standaloneFileNames[groupIndex].size();

			std::string _name;
			std::string str;

			//---

			ImGui::Dummy(ImVec2(0.0f, 5));

			// label for User-Kit folder
			str = "User-Kit";
			ImGui::Text(str.c_str());
			str = path_UserKit_Folder;
			ImGui::Text(str.c_str());

			ImGui::Dummy(ImVec2(0.0f, 5));

			//--

			// user selected wich group to edit
			if (bBuildGroupSelector) ofxImGui::AddParameter(GuiGROUP_Selected_Index);

			//ImGui::Dummy(ImVec2(0.0f, 5));

			//--

			// name of selected group
			str = "Group Name";
			ImGui::Text(str.c_str());
			str = PRESETS_Selected_Index[GuiGROUP_Selected_Index].getName();
			ImGui::Text(str.c_str());

			ImGui::Dummy(ImVec2(0.0f, 5));

			//--

			// helpers and browser
			{
				//---

				ImGui::Dummy(ImVec2(0.0f, 5));

				//// label presets folder
				////str = "User-Kit";
				////ImGui::Text(str.c_str());
				//str = "/" + path_PresetsStandalone + "/" + groups[groupIndex].getName();// append group name
				//ImGui::Text(str.c_str());
				//ImGui::Dummy(ImVec2(0.0f, 5));

				//-

				// 0. send/save current browsed (from "/archive/") preset to current presets on favorites

				// get/copy all favs presets from favs and send/save to browser folder ("archive")

				if (ImGui::Button("FROM FAVS"))
				{
					ofLogNotice(__FUNCTION__) << "FROM FAVS";

					doStandalonePresetsBuildFromFavs(groupIndex);
				}

				//-

				// TODO:
				// should iterate all files and overwrite all favourites

				//ImGui::SameLine();
				//if (ImGui::Button("TO FAVS"))
				//{
				//	ofLogNotice(__FUNCTION__) << "TO FAVS: SAVE STANDALONE PRESET: " << standaloneNamePresetDisplay[groupIndex];
				//	//if (MODE_StandalonePresets_NEW) save(PRESETS_Selected_Index[groupIndex], groupIndex);
				//	
				//	// save current to favourites
				//	save(PRESETS_Selected_Index[groupIndex], groupIndex);
				//}

				ImGui::Dummy(ImVec2(0.0f, 5));

				//-

				// 0. blink error when no files detected on folder

				if (_numfiles == 0)
				{
					int n = 30;
					float a = ofMap(ofGetFrameNum() % n, 0, n, 0.f, 1.f);
					ImGui::PushID(1);
					ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.5, 0.0f, 0.5f, a));
					ImGui::Text("FILES NOT FOUND!");

					std::string _path = path_UserKit_Folder + "/" + path_PresetsStandalone + "/";
					_path += PRESETS_Selected_Index[groupIndex].getName();// group name

					const char *array = _path.c_str();

					ImGui::Text(array);
					ImGui::PopStyleColor(1);
					ImGui::PopID();
				}
				else if (_numfiles > 0)
				{
					// 1. arrow buttons

					static int counter = standaloneFileIndex[groupIndex];
					float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
					ImGui::PushButtonRepeat(true);

					//--

					// 1.1 prev file

					if (ImGui::ArrowButton("##left", ImGuiDir_Left))
					{
						if (counter > 0)
						{
							counter--;
							standaloneFileIndex[groupIndex] = counter;
							if (standaloneFileIndex[groupIndex] < standaloneFiles[groupIndex].size())
							{
								_name = standaloneFileNames[groupIndex][standaloneFileIndex[groupIndex]];
								ofLogNotice(__FUNCTION__) << "ARROW: displayNamePreset: [" + ofToString(standaloneFileIndex[groupIndex]) + "] " << _name;

								//if (MODE_StandalonePresets_NEW)
								ofLogNotice(__FUNCTION__) << "LOAD. filename: " << _name;

								doStandalonePresetLoad(_name, groupIndex);
							}
						}
					}

					//--

					// 1.2 next file

					ImGui::SameLine(0.0f, spacing);
					if (ImGui::ArrowButton("##right", ImGuiDir_Right))
					{
						if (counter < standaloneFiles[groupIndex].size() - 1)
						{
							counter++;
							standaloneFileIndex[groupIndex] = counter;
							if (standaloneFileIndex[groupIndex] < standaloneFiles[groupIndex].size())
							{
								_name = standaloneFileNames[groupIndex][standaloneFileIndex[groupIndex]];
								ofLogNotice(__FUNCTION__) << "ARROW: displayNamePreset: [" + ofToString(standaloneFileIndex[groupIndex]) + "] " << _name;

								//if (MODE_StandalonePresets_NEW)
								ofLogNotice(__FUNCTION__) << "LOAD. filename: " << _name;

								doStandalonePresetLoad(_name, groupIndex);
							}
						}
					}
					ImGui::PopButtonRepeat();

					//--

					// 1.3 text preview current preset index to total amount.

					ImGui::SameLine();
					ImGui::Text("%d/%d", standaloneFileIndex[groupIndex], _numfiles - 1);
				}

				//--

				// 3. scrollable filenames list

				if (!standaloneFileNames[groupIndex].empty())
				{
					ImGui::SetNextItemWidth(140);

					int _currentFileIndex = standaloneFileIndex[groupIndex];

					if (ofxImGui::VectorCombo(" ", &_currentFileIndex, standaloneFileNames[groupIndex]))
					{
						ofLogNotice(__FUNCTION__) << "Preset Index: " << ofToString(_currentFileIndex);
						if (_currentFileIndex < standaloneFileNames[groupIndex].size())
						{
							standaloneFileIndex[groupIndex] = _currentFileIndex;
							standaloneFileIndex[groupIndex] = _currentFileIndex;

							_name = standaloneFileNames[groupIndex][standaloneFileIndex[groupIndex]];

							ofLogNotice(__FUNCTION__) << "LOAD. filename: " << _name;
							doStandalonePresetLoad(_name, groupIndex);
						}
					}
				}

				//--

				// update, reload, delete, clear
				{
					// 4.2 update

					ImGui::Dummy(ImVec2(0.0f, 5));

					if (ImGui::Button("UPDATE"))
					{
						//_name = inputText_TEMP;
						//ofLogNotice(__FUNCTION__) << "UPDATE PRESET NAME: " << _name << endl;

						// 0. get filename of selected
						std::string _currName = standaloneFiles[groupIndex][standaloneFileIndex[groupIndex]].getBaseName();
						ofLogNotice(__FUNCTION__) << "UPDATE. filename: " << _currName;

						// 1. delete old file
						standaloneFiles[groupIndex][standaloneFileIndex[groupIndex]].remove();

						// 2. save "ovewrite"
						doStandalonePresetSave(_currName, groupIndex);

						// workflow

						// 3. refresh files
						doStandalonePresetsRefresh(groupIndex);;

						// 4. reselect last save preset (bc directory sorting changes)
						ofLogNotice(__FUNCTION__) << "Reload last updated preset:";
						int _newIndex = -1;// search index for filename
						for (size_t i = 0; i < standaloneFiles[groupIndex].size(); i++)
						{
							std::string n = standaloneFiles[groupIndex][i].getBaseName();
							if (n == _currName) _newIndex = i;
						}
						if (_newIndex != -1)
						{
							ofLogNotice(__FUNCTION__) << "Index [" << _newIndex << "] " << standaloneFiles[groupIndex][_newIndex].getBaseName();
							standaloneFileIndex[groupIndex] = _newIndex;
							_name = standaloneFileNames[groupIndex][standaloneFileIndex[groupIndex]];

							doStandalonePresetLoad(_name, groupIndex);
						}
						else ofLogError(__FUNCTION__) << "Not found! Bad Index [" << _newIndex << "]";
					}

					//-

					// 4.3 reload

					ImGui::SameLine();
					if (ImGui::Button("RELOAD"))
					{
						_name = standaloneFileNames[groupIndex][standaloneFileIndex[groupIndex]];
						ofLogNotice(__FUNCTION__) << "RELOAD. filename: " << _name;

						doStandalonePresetLoad(_name, groupIndex);
					}

					//-

					// 4.4 delete

					ImGui::SameLine();
					if (ImGui::Button("DELETE"))// current preset
					{
						ofLogNotice(__FUNCTION__) << "DELETE. filename: " << _name;
						ofLogNotice(__FUNCTION__) << "File: " << standaloneFiles[groupIndex][standaloneFileIndex[groupIndex]].getAbsolutePath();

						// 1. delete file
						standaloneFiles[groupIndex][standaloneFileIndex[groupIndex]].remove();

						// workflow

						// 2. refresh files
						bool b = doStandalonePresetsRefresh(groupIndex);;
						if (b)
						{
							standaloneFileIndex[groupIndex] = 0;
							_name = standaloneFileNames[groupIndex][standaloneFileIndex[groupIndex]];

							doStandalonePresetLoad(_name, groupIndex);
						}
						else
						{
							ofLogError(__FUNCTION__) << "Error listing directory!";
						}
					}

					//-

					// 4.5 clear. delete all!

					ImGui::SameLine();
					if (ImGui::Button("CLEAR"))// delete all files
					{
						ofLogNotice(__FUNCTION__) << "CLEAR Presets folder: " << path_UserKit_Folder + "/" + path_PresetsStandalone;

						for (int i = 0; i < standaloneFiles[groupIndex].size(); i++) {
							ofLogWarning(__FUNCTION__) << "DELETE file: " << standaloneFiles[groupIndex][i].getAbsolutePath();

							// 1. delete file
							bool b = standaloneFiles[groupIndex][i].remove();
							if (!b) ofLogError(__FUNCTION__) << "Can not DELETE file: " << standaloneFiles[groupIndex][i].getAbsolutePath();
						}

						// workflow

						// 2. refresh files
						bool b = doStandalonePresetsRefresh(groupIndex);;
						if (b)
						{
							standaloneFileIndex[groupIndex] = 0;
							_name = standaloneFileNames[groupIndex][standaloneFileIndex[groupIndex]];

							doStandalonePresetLoad(_name, groupIndex);
						}
						else ofLogError(__FUNCTION__) << "Error listing directory!";
					}
				}

				//----

				// new preset button

				ofxImGui::AddParameter(MODE_StandalonePresets_NEW);

				//-

				// mode new preset enabled

				// 5. second panel

				if (MODE_StandalonePresets_NEW)
				{
					// 5.1 new preset name

					//ImGui::Text("NEW PRESET:");

					//-

					// 5.2 user input text

					// loaded string into char array
					char tab[32];
					strncpy(tab, inputText_NEW.c_str(), sizeof(tab));
					tab[sizeof(tab) - 1] = 0;

					if (ImGui::InputText("", tab, IM_ARRAYSIZE(tab)))
					{
						ofLogNotice(__FUNCTION__) << "InputText [tab]: \t\t" << ofToString(tab);

						inputText_NEW = ofToString(tab);

						ofLogNotice(__FUNCTION__) << "inputText_NEW: \t\t" << inputText_NEW;

						//bBlink = true; // not workind. we like to blink when mouse_on_text_input
					}

					//--

					// 5.3 save new

					// workflow

					// TODO:
					// blink when it's editing a new preset..
					bool bBlink = true;

					if (bBlink)
					{
						//ImGui::PushID(1);
						int n = 30;
						float a = ofMap(ofGetFrameNum() % n, 0, n, 0.2f, 0.5f);
						ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5, 0.0f, 0.5f, a));
					}

					if (ImGui::Button("SAVE"))
					{
						ofLogNotice(__FUNCTION__) << "inputText_NEW: " << inputText_NEW << endl;

						// 1. save
						doStandalonePresetSave(inputText_NEW, groupIndex);

						// workflow

						// 2. disable new preset mode
						MODE_StandalonePresets_NEW = false;

						// 3. refresh files
						doStandalonePresetsRefresh(groupIndex);

						// 4. reselect last save preset (bc directory sorting changes)
						ofLogNotice(__FUNCTION__) << "Reload last saved preset:";
						int _newIndex = -1;
						for (size_t i = 0; i < standaloneFiles[groupIndex].size(); i++)
						{
							std::string n = standaloneFiles[groupIndex][i].getBaseName();
							if (n == inputText_NEW) _newIndex = i;
						}
						ofLogNotice(__FUNCTION__) << "Index [" << _newIndex << "] " << standaloneFiles[groupIndex][_newIndex].getBaseName();
						standaloneFileIndex[groupIndex] = _newIndex;

						doStandalonePresetLoad(inputText_NEW);
					}

					if (bBlink) ImGui::PopStyleColor(1);

					ImGui::SameLine();
					if (ImGui::Button("CANCEL"))
					{
						ofLogNotice(__FUNCTION__) << "Cancel";
						inputText_NEW = "";
						MODE_StandalonePresets_NEW = false;
					}

					//--

					// workaround
					// to disable global keys to not collide with ImGui textInput
					ofxImGui::AddParameter(ENABLE_Keys);
				}
			}
		}
		ofxImGui::EndWindow(settings);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Randomizers()
{
	// this panel will show the settings for the selected by user group (GuiGROUP_Selected_Index)

	if (SHOW_Panel_Randomizer)
	{
		groupRandomizers[GuiGROUP_Selected_Index.get()].ImGui_Draw_GroupRandomizers();// show randomizers of user selected group
	}
}

//----

// standalone presets browser engine

//--------------------------------------------------------------
bool ofxPresetsManager::doStandalonePresetsRefresh(int groupIndex)
{
	ofLogNotice(__FUNCTION__) << groupIndex;

	if (groupIndex == -1)
	{
		ofLogError(__FUNCTION__) << "groupIndex " << groupIndex;
		return false;
	}

	if (groupIndex >= groups.size()) {
		ofLogError(__FUNCTION__) << "groupIndex Out of groups range: " << groupIndex;
		groupIndex = groups.size() - 1;

		bFilesError = true;
		return !bFilesError;
	}

	//-

	std::string _path;
	_path = path_UserKit_Folder + "/" + path_PresetsStandalone;// '/archive'
	_path += "/" + groups[groupIndex].getName();// append group name
	//_path += "/";// the folder

	CheckFolder(_path);

	ofLogNotice(__FUNCTION__) << "Path: " << _path;
	ofDirectory _dataDir(ofToDataPath(_path, true));

	//-

	if (groupIndex < groups.size())
	{
		// clear files and filenames vectors
		standaloneFiles[groupIndex].clear();
		standaloneFileNames[groupIndex].clear();

		// load all folder files in one call
		int _size = _dataDir.getFiles().size();// amount of (standalone presets) files for this group

		standaloneFiles[groupIndex].resize(_size);
		standaloneFileNames[groupIndex].resize(_size);

		ofLogNotice(__FUNCTION__) << "Preset files " << groups[groupIndex].getName() << " #" << groupIndex;

		for (size_t i = 0; i < _size; i++)
		{
			ofFile f = _dataDir.getFiles()[i];

			standaloneFiles[groupIndex][i] = f;
			standaloneFileNames[groupIndex][i] = f.getBaseName();

			ofLogNotice(__FUNCTION__) << "[" << i << "] " << f.getBaseName();
		}
	}

	//-

	// TODO
	//void to go to 1st...

	// 1. load same position preset
	// if preset is deleted goes to next one..
	// should check names because sorting changes..
	if (standaloneFileNames[groupIndex].size() > 0)
	{
		bFilesError = false;
	}
	else
	{
		ofLogWarning(__FUNCTION__) << "STANDALONE PRESETS: FILES NOT FOUND ON FOLDER!";
		bFilesError = true;

		//// TODO:
		//// disable custom path bc error
		//ofLogError(__FUNCTION__) << "Disable custom path: " << path_BrowserPathFree;
		//bCustomBrowserPath = false;
		//ofLogError(__FUNCTION__) << "------------------------";
	}

	// workflow
	//// 2. always goes to 1st preset 0
	//// that's because saving re sort the files
	//// and we don't know the position of last saved preset..
	//if (standaloneFileNames.size() > 0)
	//{
	//   standaloneFileIndex = 0;
	//   displayNamePreset = standaloneFileNames[standaloneFileIndex];
	//   doStandalonePresetLoad(displayNamePreset);
	//}
	//else
	//{
	//   ofLogError("ofxColorManager") << "NOT FOUND ANY FILE PRESET!";
	//}

	return !bFilesError;//true if ok
}

//--------------------------------------------------------------
void ofxPresetsManager::doStandalonePresetsBuild(int groupIndex)// standalone presets splitted from favourites presets
{
	// load files structure directory
	bool bLoaded = doStandalonePresetsRefresh(groupIndex);

	// workflow
	if (bLoaded)
	{
		// load first preset
		if (standaloneFileNames[groupIndex].size() > 0)
		{
			standaloneFileIndex[groupIndex] = 0;// select first file
			standaloneNamePresetDisplay[groupIndex] = standaloneFileNames[groupIndex][0];

			//// workflow
			//doStandalonePresetLoad(displayNamePreset, groupIndex);
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doStandalonePresetSave(std::string name, int groupIndex)// without xml extension nor path
{
	ofLogNotice(__FUNCTION__) << "name: " << name << fileExtension << " group: " << groupIndex;

	if (groupIndex == -1)
	{
		ofLogError(__FUNCTION__) << "groupIndex " << groupIndex;
		return;
	}

	std::string _path;
	_path = path_UserKit_Folder + "/" + path_PresetsStandalone;
	_path += "/" + groups[groupIndex].getName();// append group name
	_path += "/" + name;// append preset name
	_path += fileExtension;
	ofLogNotice(__FUNCTION__) << _path;

	if (groupIndex < groups.size()) {
		bool b = ofxSurfingHelpers::saveGroup(groups[groupIndex], _path);

		if (!b) ofLogError(__FUNCTION__) << "CAN'T SAVE FILE " << _path << "!";
		else ofLogNotice(__FUNCTION__) << "DONE SAVED " << _path;
	}
	else ofLogError(__FUNCTION__) << "groupIndex Out of groups range: " << groupIndex;
}

//--------------------------------------------------------------
void ofxPresetsManager::doStandalonePresetLoad(std::string name, int groupIndex)// without xml extension nor path
{
	ofLogNotice(__FUNCTION__) << "name: " << name << fileExtension << " group: " << groupIndex;

	if (groupIndex == -1)
	{
		ofLogError(__FUNCTION__) << "groupIndex " << groupIndex;
		return;
	}

	std::string _path;
	_path = path_UserKit_Folder + "/" + path_PresetsStandalone;
	_path += "/" + groups[groupIndex].getName();// append group name
	_path += "/" + name;// append preset name
	_path += fileExtension;
	ofLogNotice(__FUNCTION__) << _path;

	if (groupIndex < groups.size()) {
		bool b = ofxSurfingHelpers::loadGroup(groups[groupIndex], _path);

		if (!b) ofLogError(__FUNCTION__) << "CAN'T LOAD FILE " << _path << "!";
		else ofLogNotice(__FUNCTION__) << "DONE LOADED " << _path;
	}
	else ofLogError(__FUNCTION__) << "groupIndex Out of groups range: " << groupIndex;
}

//--------------------------------------------------------------
void ofxPresetsManager::doStandalonePresetsBuildFromFavs(int groupIndex)// save all favorites presets to the browser (archive) folder
{
	ofLogNotice(__FUNCTION__) << " group: " << groupIndex;

	if (groupIndex == -1)
	{
		ofLogError(__FUNCTION__) << "groupIndex " << groupIndex;
		return;
	}

	std::string _path;
	_path = path_UserKit_Folder + "/" + path_PresetsStandalone;
	_path += "/" + groups[groupIndex].getName();// append group name
	//_path += "/";// the folder
	ofLogNotice(__FUNCTION__) << _path;

	// amount of files
	// iterate all presets
	for (int i = 0; i < groupsSizes[groupIndex]; i++)
	{
		std::string pathSrc;
		std::string pathDst;

		pathSrc = getPresetPath(groups[groupIndex].getName(), i);
		boost::filesystem::path bPath(pathSrc);

		//string pathFolder = ofToString(bPath.parent_path());
		std::string _fileName = ofToString(bPath.filename().generic_string());
		pathDst = _path + "/" + _fileName;

		ofLogNotice(__FUNCTION__) << "from: " << pathSrc;
		ofLogNotice(__FUNCTION__) << "\t\t to: " << pathDst;

		ofFile _file;
		_file.copyFromTo(pathSrc, pathDst, true, true);// relative, overwrite
	}

	//--

	// refresh files
	doStandalonePresetsRefresh(groupIndex);;
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::doCheckPresetsFoldersAreEmpty()
{
	for (int i = 0; i < groups.size(); i++)
	{
		// set the name for the randomizer settings file
		if (bSplitGroupFolders.get())
		{
			std::string _path;
			_path = path_UserKit_Folder + "/" + path_PresetsFavourites;// current kit-presets presets folder
			_path += "/" + groups[i].getName();// append group name
			_path += "/";// the folder

			CheckFolder(_path);

			ofDirectory dataDirectoryGroupPresets(ofToDataPath(_path, true));

			// check if folder is empty: no files into folder
			if (dataDirectoryGroupPresets.size() == 0)
			{
				ofLogWarning(__FUNCTION__) << "Folder " << _path << ", used for group " << i << " "
					<< groups[i].getName() << " is empty! Force populate favourites files...";

				// 1. populate all favourites preset files for this group
				doPopulateFavs(i);

				//// 2. create standalone files too: one file for each favourite preset
				//doStandalonePresetsBuildFromFavs();
			}

			// verify if files are created
			ofLogNotice(__FUNCTION__) << ofToString(dataDirectoryGroupPresets.size()) << " file preset at folder " << _path;
		}
	}
}

//----

#ifdef INCLUDE_ofxUndoSimple

//--------------------------------------------------------------
void ofxPresetsManager::doStoreUndo() {
	// the current selected only
	int i = GuiGROUP_Selected_Index.get();
	//for (int i = 0; i < groups.size(); i++)// all together
	{
		undoXmlsParams[i].clear();

		ofParameterGroup _group = groups[i];
		ofSerialize(undoXmlsParams[i], _group);// fill the xml with the ofParameterGroup

		undoStringsParams[i] = (undoXmlsParams[i].toString());// fill the ofxUndoSimple with the xml as string
		undoStringsParams[i].store();

		std::string str = "";
		str += "UNDO HISTORY: " + ofToString(undoStringsParams[i].getUndoLength()) + "/";
		str += ofToString(undoStringsParams[i].getRedoLength());
		//str += "\n";
		//str += "DESCRIPTOR\n";
		//str += undoStringParams.getUndoStateDescriptor() + "\n";

		ofLogNotice(__FUNCTION__) << str;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doClearUndoHistory() {
	ofLogNotice(__FUNCTION__) << "UNDO CLEAR : " << GuiGROUP_Selected_Index;
	undoStringsParams[GuiGROUP_Selected_Index].clear();
	//undoStringParams.clear();
}

//--------------------------------------------------------------
void ofxPresetsManager::doUndo() {
	ofLogNotice(__FUNCTION__) << "UNDO < Group #" << GuiGROUP_Selected_Index << " " << groups[GuiGROUP_Selected_Index].getName();
	undoStringsParams[GuiGROUP_Selected_Index].undo();
	//undoStringParams.undo();
	doRefreshUndoParams();
}

//--------------------------------------------------------------
void ofxPresetsManager::doRedo() {
	ofLogNotice(__FUNCTION__) << "REDO < Group #" << GuiGROUP_Selected_Index << " " << groups[GuiGROUP_Selected_Index].getName();
	undoStringsParams[GuiGROUP_Selected_Index].redo();
	//undoStringParams.redo();
	doRefreshUndoParams();
}

//--------------------------------------------------------------
void ofxPresetsManager::doRefreshUndoParams() {
	int i = GuiGROUP_Selected_Index.get();
	//for (int i = 0; i < groups.size(); i++)
	{
		undoXmlsParams[i].clear();
		undoXmlsParams[i].parse(undoStringsParams[i]);// fill the xml with the string 

		ofParameterGroup _group = groups[i];
		ofDeserialize(undoXmlsParams[i], _group);// load the ofParameterGroup

		std::string str = "";
		str += "UNDO HISTORY: " + ofToString(undoStringsParams[i].getUndoLength()) + "/";
		str += ofToString(undoStringsParams[i].getRedoLength());
		//str += "\n";
		//str += "DESCRIPTOR\n";
		//str += undoStringParams.getUndoStateDescriptor() + "\n";

		ofLogNotice(__FUNCTION__) << str;
	}
}

#endif

//----

//--------------------------------------------------------------
void ofxPresetsManager::Changed_GuiGROUP_Selected_Index(int & index)
{
	GuiGROUP_Selected_Index = (int)ofClamp(GuiGROUP_Selected_Index.get(), 0, GuiGROUP_Selected_Index.getMax());
	ofLogWarning(__FUNCTION__) << " : " << GuiGROUP_Selected_Index;
}

//--------------------------------------------------------------
void ofxPresetsManager::drawHelp(int _x, int ySave)
{
	bool bLeftPosition = true;// left or right. only if lateral pos true
	bool bLateralPosition = false;// false = on top of clicker

	//--

	//if (rectanglePresetClicker.getY() > (ofGetHeight() * 0.5)) 
	//else drawHelp(0, ySave + groups.size()*cellSize + 20);

	{
		bShowClickerInfo = SHOW_Help.get();

		if (bShowClickerInfo)// && (i == 0))//&& ENABLE_Keys . only on first (0) for groups iterate pass..
		{
			std::string ss = helpInfo;
			int _padx = 22;
			int _pady = 30;
			int x = 0;
			int y = 0;

			// A. vertical position below boxes
			if (!bLateralPosition)
			{
				float hh = ofxSurfingHelpers::getHeightBBtextBoxed(myFont, ss);
				x += 4 + _padx;
				y -= hh + _pady;
			}

			// B. lateral position right to the boxes
			else
			{
				float _w;
				y = ySave + _pady;

				if (!bLeftPosition)// on the right
				{
					_w = getPresetClicker_Width();
					x = _w + _padx + 30;
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