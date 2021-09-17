#include "ofxPresetsManager.h"

//--------------------------------------------------------------
ofxPresetsManager::ofxPresetsManager()
{
	//-

//#ifdef DEBUG_randomTest
//	ofSetLogLevel("ofxPresetsManager", OF_LOG_VERBOSE);
//#endif
//
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

	filenameMainSettings = "ofxPresetsManager_Settings" + fileExtension; // default user preset name

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

	bGui.set("ofxPresetsManager", false);
	bGui_Main.set("Main", false);
	//bGui_Main.set("Main Presets", false);
	//bGui.set("SHOW ImGui", false);
	bGui_PresetsParams.set("Preset Parameters", false);
	bGui_Selectors.set("Selectors", false);
	bGui_Clicker.set("Clicker", true);
	bGui_Parameters.set("Parameters", false);
	//SHOW_Panel_AllSelectors.set("SHOW SELECTORS", false);
	bGui_Standalones.set("Standalones", false);
	bGui_Players.set("Players", false);
	//bGui_PlayerEditor.set("Editors", false);
	//bGui_RandomizerParams.set("RANDOM PARAMS", false);

	bGui_Panels.set("Panels", true);
	bHelp.set("Help", false);
	bGui_AdvancedControl.set("Advanced", false);
	MODE_StandalonePresets_NEW.set("NEW!", false);
	//ENABLE_Keys.setSerializable(false);
	bKeys.set("Keys", true);
	bKeys.setSerializable(false);

	bAutoLoad.set("Auto Load", true);
	bAutoSave.set("Auto Save", true);
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

	//ImGui_Size.set("GUI ImGui SIZE",
	//	glm::vec2(300, 800),
	//	glm::vec2(0, 0),
	//	glm::vec2(ofGetWidth(), ofGetHeight())
	//);
	//ImGui_Position.set("GUI ImGui POSITION",
	//	glm::vec2(ofGetWidth() - (ImGui_Size.get().x + 10), 10),
	//	glm::vec2(0, 0),
	//	glm::vec2(ofGetWidth(), ofGetHeight())
	//);

	//-

	// exclude from settings
	loadToMemory.setSerializable(false);
	saveFromMemory.setSerializable(false);
	bGui_AdvancedControl.setSerializable(false);

	//--

	// internal control params

	params_Options.setName("OPTIONS");
	//params_Options.add(MODE_MemoryLive);// TODO: not implemented
	params_Options.add(loadToMemory);
	params_Options.add(saveFromMemory);
	params_Options.add(bAutoLoad);
	params_Options.add(bAutoSave);
	params_Options.add(bAutosaveTimer);

	params_Gui.setName("GUI");
	params_Gui.add(bGui_AdvancedControl);
	params_Gui.add(bGui_Parameters);
	//params_Gui.add(SHOW_Panel_AllSelectors);
	params_Gui.add(bGui_Clicker);
	params_Gui.add(bGui_Standalones);
	params_Gui.add(bGui_Players);
	//params_Gui.add(bGui_PlayerEditor);
	//params_Gui.add(bGui_RandomizerParams);
	params_Gui.add(bGui);
	params_Gui.add(bGui_Main);
	params_Gui.add(bGui_PresetsParams);
	params_Gui.add(bKeys);

	// layout
	//params_Gui.add(ImGui_Position);
	//params_Gui.add(ImGui_Size);
	params_Gui.add(Gui_Internal_Position);

	//--

	// all nested params for callbacks and storage settings
	params_Control.setName("ofxPresetsManager");
	ofAddListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Control);

	//--

	// gui box clicker font
	std::string str;

	sizeTTF = 11;
	str = "telegrama_render.otf";

	//sizeTTF = 10;
	////str = "overpass-mono-bold.otf";
	//str = "telegrama_render.otf";

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

	bAutoDraw = false;
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
	ofLogNotice(__FUNCTION__) << "create group selector: " << (_buildGroupSelector ? "TRUE" : "FALSE");

	DISABLE_CALLBACKS = true;

	//--

	// create main GROUP selector only when we have added more than one group. 
	// if not, we don't have nothing to group!
	if (PRESETS_Selected_Index.size() > 1 && groups.size() > 1)
		bBuildGroupSelector = _buildGroupSelector;
	else
	{
		bBuildGroupSelector = false;
		bGui_Selectors = false;
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
#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL	
	ImGui_Setup();
#endif

#ifdef USE_PRESETS_MANAGER__IMGUI_LAYOUT 
	guiManager.setImGuiAutodraw(bAutoDraw);
	guiManager.setup(); // this instantiates and configurates ofxImGui inside the class object.
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

	MODE_EditPresetClicker.set("Edit Clicker", false);
	MODE_EditPresetClicker.setSerializable(false);

	//helpPos.set("HELP POS", false);

	bGui_BgEditPresetClicker.set("Box Clicker", false);

	//----

	bSplitGroupFolders.setSerializable(false);// force to this mode. do not store

	params_Options.add(MODE_Editor);
	params_Options.add(bSplitGroupFolders);

	params_Gui.add(bThemeDarkOrLight);
	params_Gui.add(bGui_Selectors);
	params_Gui.add(bGui_PresetsParams);
	params_Gui.add(bHelp);
	params_Gui.add(MODE_EditPresetClicker);
	//params_Gui.add(helpPos);
	params_Gui.add(bGui_BgEditPresetClicker);

	params_Control.add(params_Options);
	params_Control.add(params_Gui);

	params_Control.add(MODE_EditPresetClicker);
	params_Control.add(bThemeDarkOrLight);
	params_Control.add(cellSize);

	params_Control.add(bSHOW_allGroups);
	params_Control.add(bGui_Panels);

#ifdef INCLUDE_ofxSurfingRandomizer
#ifdef INCLUDE_ofxSurfingTween
	params_Control.add(dataTween.bGui);
	params_Control.add(dataRandomizer.bGui);
#endif
#endif

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
		bSHOW_allGroups.set("Show All", false);

		//bSHOW_allGroups.setSerializable(false);

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

			//----

			// smoother
#ifdef INCLUDE_ofxSurfingSmooth//exclude group selector when this mode is active
			if (!bBuildGroupSelector) params_Smooth.add(groups[i]);
			else if (i != _last) params_Smooth.add(groups[i]);
#endif
		}

		//----

		// smoother
#ifdef INCLUDE_ofxSurfingSmooth
		if (groups.size() > 0) {
			//params_Smooth.add(groups[0]);
			smoother.setup(params_Smooth);
		}
#endif

		//----
	}

	params_UserKitSettings.add(GuiGROUP_Selected_Index);

	//----

	// custom path
	// this allows to use different kits for the same project/app
	params_Custom.setName("CUSTOM PATH");
	params_Custom.add(bPathDirCustom);
	params_Custom.add(pathDirCustom);
	params_UserKitSettings.add(params_Custom);

	ofAddListener(params_UserKitSettings.parameterChangedE(), this, &ofxPresetsManager::Changed_UserKit);

	//----

	// randomizers

	//TODO:
	// workflow
	for (int i = 0; i < groups.size(); i++)
	{
		groupRandomizers[i].PLAY_RandomizeTimer.addListener(this, &ofxPresetsManager::Changed_Randomizers);

		//listeners.push(groupRandomizers[i].PLAY_RandomizeTimer.newListener(this, &ofxPresetsManager::Changed_Randomizers));
	}

	//--

	//// randomizer panels toggles

	//for (int i = 0; i < groupRandomizers.size(); i++)
	//{
	//	groupRandomizers[i].bGui_PlayerEditor.makeReferenceTo(bGui_PlayerEditor);
	//	groupRandomizers[i].bGui_RandomizerParams.makeReferenceTo(bGui_RandomizerParams);
	//
	//	//bGui_PlayerEditor.makeReferenceTo(groupRandomizers[i].bGui_PlayerEditor);
	//	//bGui_RandomizerParams.makeReferenceTo(groupRandomizers[i].bGui_RandomizerParams);
	//}

	//-

	// help box
	helpTextBoxWidget.setPath(path_UserKit_Folder);
	helpTextBoxWidget.setup();

	// preset clicker
	doubleClicker.set(0, 0, ofGetWidth(), ofGetHeight());//full screen
	doubleClicker.setDebug(false);

	//----------------

	// SURFING ENGINES
	setupEngines();

	//----

	startup();
}

//--------------------------------------------------------------
void ofxPresetsManager::setupEngines() {

	//----

	// RANDOMIZER

#ifdef INCLUDE_ofxSurfingRandomizer
	params_Randomizator.setName("Parameters_RANDOMiZER");
	if (groups.size() == 1)
	{
		params_Randomizator.add(groups[0]);
		dataRandomizer.setAutodraw(false);
		dataRandomizer.setup(params_Randomizator);
	}
	else
	{
		int prelast = 0;
		if (bAllowGroupSelector) prelast = groups.size() - 1;
		else  prelast = groups.size();

		for (int i = 0; i < prelast; i++)//exclude last GLOBAL_GROUP
		{
			params_Randomizator.add(groups[i]);
		}
		dataRandomizer.setup(params_Randomizator);
	}
#endif

	//----

	// TWEENER

#ifdef INCLUDE_ofxSurfingTween
	params_Tween.setName("Parameters");
	if (groups.size() == 1)
	{
		params_Tween.add(groups[0]);
		dataTween.setup(params_Tween);
	}
	else
	{
		int prelast = 0;
		if (bAllowGroupSelector) prelast = groups.size() - 1;
		else  prelast = groups.size();

		for (int i = 0; i < prelast; i++)//exclude last GLOBAL_GROUP
		{
			params_Tween.add(groups[i]);
		}
		dataTween.setup(params_Tween);
	}
#endif

	//----
}

//--------------------------------------------------------------
void ofxPresetsManager::startup()
{
	ofLogNotice(__FUNCTION__);

	DISABLE_CALLBACKS = false;// enable callbacks after setup

	//--

	// load all app session settings & randomizers (not the related presets)
	load_AppSettings();// here bPathDirCustom is loaded (but if files are present, not in first runtime)

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

	rectangle_PresetClicker.setLockResize(true);
	// load settings
	//doRectFit();
	rectangle_PresetClicker.loadSettings(path_RectanglePresetClicker, path_UserKit_Folder + "/" + path_ControlSettings + "/", false);

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

	//--

	// workaround
	//MODE_EditPresetClicker = MODE_EditPresetClicker.get();
	//required when no xml settings yet
	bThemeDarkOrLight = bThemeDarkOrLight.get();

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

	////TODO: BUG:
	////load_AppSettings();// here bPathDirCustom is loaded (but if files are present, not in first runtime)

	//std::string path = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_ControlSettings;
	//ofxSurfingHelpers::loadGroup(params_Control, path);
}

//--------------------------------------------------------------
void ofxPresetsManager::update(ofEventArgs & args)
{
	//	cout << "bImGui_mouseOver " << bImGui_mouseOver.get() << endl;
	//	cout << "bImGui_mouseOver_PRE " << bImGui_mouseOver_PRE<< endl;
	//	cout << "bDoneSetup " << bDoneSetup << endl;

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
		////&& bAutoLoad? 
		//if (!MODE_StandalonePresets_NEW && bAutoSave && bAutosaveTimer && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
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

		//--

		// tweener
#ifdef INCLUDE_ofxSurfingTween
		if (isDoneLoad()) {
			dataTween.doGo();
		}
#endif

	}
}

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL	
//---------------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw()
{
	ImGui_Begin();
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
	ImGui_End();
}
#endif

//---------------------------------------------------------------------
void ofxPresetsManager::draw(ofEventArgs & args)
{
	//----

	// user clicker boxes preset selector 

	if (bGui_Clicker)
	{
		drawPresetClicker();
	}

	// help info text: 

	if (bHelp && bGui)
	{
		drawHelp();
	}

	//----

	// ImGui

	if (!bGui) return;

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL	
	ImGui_Draw();
#endif

#ifdef USE_PRESETS_MANAGER__IMGUI_LAYOUT 
	guiManager.begin();
	ImGui_Draw_Window();
	guiManager.end();
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
	// User trigger button boxes. 
	// A clickable selector and a save button.
	// Draws some minimalistic graphics to monitor the active preset.

	//--

	float _round = 3.0f;
	float _pad = 3.0f;
	std::string _label;

	//--

	if (!lastMouseButtonState && ofGetMousePressed())
	{
		mousePressed(ofGetMouseX(), ofGetMouseY());
	}
	lastMouseButtonState = ofGetMousePressed();

	//-

	// Display help info layout
	bool bLateralPosition = false;// false = on top of clicker
	bool bLeftPosition = true;// left or right. only if lateral pos true

	//----

	ofPushStyle();
	{
		//----

		// Bg rectangle editor
		{
			if (bGui_BgEditPresetClicker)
			{
				ofFill();

				ofColor colorBg;
				if (rectangle_PresetClicker.isEditing()) {
					float a = ofxSurfingHelpers::getFadeBlink();
					ofColor c = ofColor(_colorBg, 255 * a);
					rectangle_PresetClicker.draw();

					colorBg = c;
				}
				else
				{
					colorBg = _colorBg;
				}

				ofSetColor(colorBg);
				rectangle_PresetClicker.draw();
				ofDrawRectRounded(rectangle_PresetClicker, _round);
			}

			// get clicker position from being edited rectangle
			//if (MODE_EditPresetClicker)
			{
				doRectFit();

				_RectClick_Pad = 20; // spacing to borders
				float _RectClick_w = getGroupNamesWidth();
				clicker_Pos.x = rectangle_PresetClicker.x + _RectClick_Pad + _RectClick_w;
				clicker_Pos.y = rectangle_PresetClicker.y + _RectClick_Pad;
			}

			//-

			// doubleClick
			{
				if (!bLockClicker)
				{
					// double click swap edit mode
					if (doubleClicker.isMouseDoubleClick()) {
						MODE_EditPresetClicker = !MODE_EditPresetClicker;
					}
					// triple clicks swap layout mode
					if (doubleClicker.isMouseTripleClick()) {
						bGui_BgEditPresetClicker = !bGui_BgEditPresetClicker;
					}
				}
			}
		}

		//--

		ofPushMatrix();
		ofTranslate(clicker_Pos);

		// Draw keys buttons for each row/group

		for (size_t i = 0; i < keys.size(); ++i)
		{
			// 0. Bg box of all boxes background

			ofFill();
			ofSetColor(_colorBg);

			// TODO: 
			//hide save button when not required
			//int _saveBut;
			//if (!bAutoSave) _saveBut = 1; else _saveBut = 0;
			//int _extraButs = (i == groups.size() - 1 ? (1 + _saveBut) : 1); // only main group has gui toggle button

			int _extraButs = (i == groups.size() - 1 ? 2 : 1); // only main group has gui toggle button

			ofDrawRectangle(0, i * cellSize, cellSize * (keys[i].size() + _extraButs), cellSize);
			// amount group presets + 2 save. + gui if it's main group row

			//-

			// 1. Draw each preset box button

			ofSetColor(_colorText);
			ofNoFill();

			float xx, yy;
			float ySave;

			// Keys letters

			size_t k = 0; // iterate keys
			for (; k < keys[i].size(); ++k)
			{
				// 1.1 Outbox border container

				ofDrawRectangle(cellSize * k, cellSize * i, cellSize, cellSize);

				//--

				// 2. Inner box. double mark current selected preset

				if (PRESETS_Selected_Index[i] == k) // it is selected
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

				if (!myFont.isLoaded()) // without ttf font
				{
					ofDrawBitmapString(ofToString((char)keys[i][k]),
						cellSize*k + 8, cellSize*i + 18);
				}
				else // custom font 
				{
					string ss = ofToString((char)keys[i][k]);
					auto rt = myFont.getStringBoundingBox(ss, 0, 0);

					xx = cellSize * k + (cellSize / 2) - rt.getWidth() / 2;
					yy = cellSize * i + (cellSize / 2) + sizeTTF / 2;

					myFont.drawString(ss, xx, yy);
				}
			}

			// here, it's last k = keys[i].size()

			ySave = yy;

			//--

			// TODO:
			//if (!bAutoSave)
			{
				// 3.1 Save button

				ofDrawRectangle(cellSize * k, cellSize * i, cellSize, cellSize);

				//--

				// 3.2 Save label

				_label = "SAVE";
				if (!myFont.isLoaded()) // without ttf font
				{
					ofDrawBitmapString(_label, cellSize*k + 8, cellSize*i + 18);
				}
				else // Custom font 
				{
					string ss = _label;
					float wx = myFont.getStringBoundingBox(ss, 0, 0).getWidth() / 2;

					myFont.drawString(ss, cellSize * k + cellSize / 2 - wx, ySave);
				}
			}

			//--

			k++;

			//--

			// 5. Gui toggle button box

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

				// 6. Show Gui label button

				_label = "GUI";

				// mark if gui visible
				if (bGui)
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
				if (!myFont.isLoaded()) // without ttf font
				{
					ofDrawBitmapString(_label, cellSize * k + 8, cellSize * i + 18);
				}
				else // custom font 
				{
					ofSetColor(_colorText);
					float wx = 0.5f * myFont.getStringBoundingBox(_label, 0, 0).width;
					myFont.drawString(_label,
						cellSize * k + 0.5 * cellSize - wx,
						ySave);
				}
			}

			//--

			// 7. Label paramGroup names to the left of clicker

			if (SHOW_GroupName)
			{
				std::string info = groups[i].getName();

				// mark selected group. useful to navigate with arrow keys

				bool bLast = (i == groups.size() - 1);
				if (!bLast) {
					info += "\n";
					if (i == GuiGROUP_Selected_Index.get() && groups.size() > 1 && bKeys.get())
						info += ">";
					info += "#" + ofToString(i);
				}
				else
				{
					if (i == GuiGROUP_Selected_Index.get() && groups.size() > 1 && bKeys.get())
						info = ">" + info;
				}

				// double font to improve different background colors
				int gap = 1;

				float strW = 50;
				if (myFont.isLoaded()) {
					strW = myFont.getStringBoundingBox(info, 0, 0).getWidth();
				}
				else { // not loaded font
					strW = 100;
					//ySave -= 10;
				}
				int xG = -strW - 20;

				if (!bThemeDarkOrLight) ofSetColor(_colorBg); // shadow
				else ofSetColor(_colorButton); // shadow

				if (myFont.isLoaded()) myFont.drawString(info, xG + gap, ySave + gap);
				else ofDrawBitmapString(info, xG + gap, ySave + gap);

				ofSetColor(_colorText); // text
				if (myFont.isLoaded()) myFont.drawString(info, xG, ySave);
				else ofDrawBitmapString(info, xG, ySave);
			}
		}

		ofPopMatrix();
	}
	ofPopStyle();
}

//--------------------------------------------------------------
ofxPresetsManager::~ofxPresetsManager()
{
	// TODO: not sure if can avoid to call manually exit(), bc there are groups that could be externally destroyed..
	// so we would prefer to call presetsManager.exit() manually on the first place sorting.
	// But if exit() is called manually too on app exit(), we will call it twice... Could be a problem ?
	exit();
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
		if (!b)
		{
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
	if (groups.size() == 0) {
		ofLogError(__FUNCTION__) << "Not added groups!";
		return;
	}

	if (groupIndex < 0)
		//if (groupIndex == -1)
		/*if (groupIndex == -1) */groupIndex = groups.size() - 1;// set last group that is the group link selector

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
	if (!bDoneSetup) return;

	{
		const int key = eventArgs.key;

		// modifiers
		bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
		bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
		bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
		bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

		//bool bDebug = false;
		bool bDebug = true;
		if (bDebug)
		{
			ofLogNotice(__FUNCTION__)
				<< "key: " << (char)key
				<< (mod_CONTROL ? " + CONTROL" : "")
				<< (mod_SHIFT ? " + SHIFT" : "")
				<< (mod_ALT ? " + ALT" : "")
				;
		}

		//--

		// tweener
#ifdef INCLUDE_ofxSurfingTween
		//dataTween.keyPressed(key);
#endif

		//--

		// TODO: TEST: to force disable engine
		//bImGui_mouseOver = false;

		// always listening to K to avoid can be blocked
		// restore keys control
		if (key == 'K' && !bImGui_mouseOver_PRE)
		{
			bKeys = !bKeys;
		}

		else if (bKeys && bKeys && !bImGui_mouseOver_PRE)// disable keys when mouse over gui
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
				bGui_AdvancedControl = !bGui_AdvancedControl;
				setVisible_GUI_Internal(bGui_AdvancedControl);
				setVisible_GUI_ImGui(bGui_AdvancedControl);
				setVisible_PresetClicker(bGui_AdvancedControl);
			}
			else if (key == 'H')
			{
				bHelp = !bHelp;
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
#ifdef INCLUDE_ofxUndoSimple
				doStoreUndo();
#endif
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
			//if (!mod_CONTROL && !mod_SHIFT && !mod_ALT)// exclude controls do not works bc blocks the mod keys to copy/swap presets...
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
		//}

		//--

		//// workflow
		//// workaround
		//if (key == 'R')
		//	doStoreUndo();
		//}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::keyReleased(ofKeyEventArgs &eventArgs)
{
	if (!bDoneSetup) return;

	{
		// mod keys
		if (eventArgs.key == modeKeySave && bKeys && bModKeySave)
		{
			bModKeySave = false;
			ofLogNotice(__FUNCTION__) << "modKey Save FALSE";
		}
		//else if (eventArgs.key == modKeySwap && bKeys && bModKeySwap)
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
		//bMouseOverGui = false;

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
						//if (bAutoSave)
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
				//TODO: hide save buttons if bAutoSave 
				//if (!bAutoSave)
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
			//int _offset = (bAutoSave ? 0 : 1);
			int _offset = 1;

			int _i;
			if (bBuildGroupSelector) _i = groups.size() - 1;
			else if (groups.size() > 1) _i = 1;
			else _i = 0;

			if ((yIndex == groups.size() - 1) && (xIndex == groupsSizes[_i] + _offset))
			{
				bGui = !bGui;

				ofLogNotice(__FUNCTION__) << "bGui : " << (bGui ? "TRUE" : "FALSE");
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
	//if (bAutoSave)
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
	//if (bAutoSave)
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
	//for (int i = 0; i < groups.size(); i++)
	//{
	//	doRandomizePresetSelected(i);
	//	doCloneAll(i);
	//}
}

////--------------------------------------------------------------
//void ofxPresetsManager::doResetsParams()
//{
//	ofLogNotice(__FUNCTION__);
//
//	int ig = GuiGROUP_Selected_Index.get();
//	int ip = PRESETS_Selected_Index[ig];
//
//	if (ig < groups.size()) groupRandomizers[ig].doResetGroup(groups[ig]);
//
//	//for (int i = 0; i < groups.size(); i++)
//	//{
//	//	doRandomizePresetSelected(i);
//	//	doCloneAll(i);
//	//}
//}

//--------------------------------------------------------------
void ofxPresetsManager::doPopulateFavs(int groupIndex)
{
	ofLogNotice(__FUNCTION__) << "on group: " << groupIndex;

	//if (groupIndex == -1 || groupIndex > groups.size())
	//{
	//	ofLogError(__FUNCTION__) << "groupIndex " << groupIndex << " out of range";
	//	return;
	//}

	////--

	//// TODO:

	//// 1. check
	//CheckAllFolders();//? required

	//// 2. erase
	//int ig = GuiGROUP_Selected_Index.get();
	//doCloneAll(ig);

	//// 3. randomize
	//for (int i = 0; i < groupsSizes[ig]; i++)
	//{
	//	doRandomizePresetSelected(i);
	//	//doCloneAll(i);
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
							//	if (bAutoLoad)
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
							//if (bAutoSave && !MODE_StandalonePresets_NEW)
							if (bAutoSave)
							{
								// workflow: 
								//disable autosave when randomizer player enabled
								//no matters edit mode
								bool b = groupRandomizers[GuiGROUP_Selected_Index.get()].PLAY_RandomizeTimer.get();
								if (!b) save(PRESETS_Selected_Index_PRE[g], g);
							}

							//-

							// remember this PRE state to know if changed or not on the next time..
							PRESETS_Selected_Index_PRE[g] = PRESETS_Selected_Index[g];

							//-

							if (bAutoLoad) load(PRESETS_Selected_Index[g], g);
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Randomizers(bool &b)
{
	if (!DISABLE_CALLBACKS && groupRandomizers.size() > 0)
	{
		ofLogNotice(__FUNCTION__);

		// workflow
		if (MODE_Editor.get() && b) MODE_Editor = false;

		//std::string name = b.getName();
		//if (name == groupRandomizers[0].PLAY_RandomizeTimer.getName())
		//{
		//}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Control(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		std::string name = e.getName();

		if ((name != "exclude") //&&
			//(name != ImGui_Position.getName()) &&
			//(name != ImGui_Size.getName())// &&
			//(name != "DICE") &&
			//(name != "PRESET")
			)
		{
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		if (false) {}

		//--

		// workflow

#ifdef INCLUDE_ofxSurfingRandomizer
#ifdef INCLUDE_ofxSurfingTween
		else if (name == dataTween.bGui.getName())
		{
			if (dataTween.bGui) dataRandomizer.bGui = false;
		}

		else if (name == dataRandomizer.bGui.getName())
		{
			if (dataRandomizer.bGui) dataTween.bGui = false;
		}
#endif
#endif
		//--

		// mode edit
		//when false, we disabled autosave to allow faster performance ! 

		else if (name == MODE_Editor.getName())
		{
			ofLogNotice(__FUNCTION__) << "MODE EDITOR: " << (MODE_Editor.get() ? "TRUE" : "FALSE");

			bAutoSave = MODE_Editor.get();

			//--

			for (int i = 0; i < groups.size(); i++) {
				groupRandomizers[i].setModeEditor(MODE_Editor);
			}

			if (!MODE_Editor) MODE_EditPresetClicker = false;

			//-

			// workflow
//#ifdef INCLUDE_ofxSurfingTween
//			dataTween.setLiveEditMode(MODE_Editor);
//#endif

		}

		//-

		if (name == bLockClicker.getName())
		{
			if (bLockClicker.get()) {
				MODE_EditPresetClicker = false;
				rectangle_PresetClicker.disableEdit();
				//doubleClicker.set
			}
		}

		if (name == bThemeDarkOrLight.getName())
		{
			// light theme (black lines & white bg)
			if (!bThemeDarkOrLight)
			{
				_colorText = ofColor(0, 255);
				_colorButton = ofColor(255, 64);
				_colorBg = ofColor(225, 64);
			}

			// dark theme (white lines & black bg)
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
				if (!bLockClicker)
					rectangle_PresetClicker.enableEdit();

				//// workflow
				////bGui_BgEditPresetClicker = true;
				//if (!bGui_Clicker) bGui_Clicker = true;
			}
			else
			{
				rectangle_PresetClicker.disableEdit();

				//--

				// all app settings
				//save_ControlSettings();
				//rectangle_PresetClicker.saveSettings(path_RectanglePresetClicker, path_UserKit_Folder + "/" + path_ControlSettings + "/", false);
			}
		}

		//----

		//TODO:
		// memmory mode. not implemented yet..

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
		//else if (name == bGui_PresetsParams.getName())
		//{
		//	if (bGui_PresetsParams) bGui_Selectors = false;
		//}

		//else if (name == bGui_Selectors.getName())
		//{
		//	if (bGui_Selectors) bGui_PresetsParams = false;
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
void ofxPresetsManager::load_AppSettings()
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
	//ofRemoveListener(params_RandomizersFiltered.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Editor);

	// TODO: required?
	ofRemoveListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofRemoveListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	GuiGROUP_Selected_Index.removeListener(this, &ofxPresetsManager::Changed_GuiGROUP_Selected_Index);

	//--

	// main group

	//-

	//// TODO: not implemented
	//// MODE B: direct from memory
	//if (MODE_MemoryLive && bAutoSave)
	//{
	//	saveAllKitFromMemory();// update of files is required bc presets are on memmory only
	//}

	//--

	// autosave all group presets
	if (bAutoSave)
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
	rectangle_PresetClicker.saveSettings(path_RectanglePresetClicker, path_UserKit_Folder + "/" + path_ControlSettings + "/", false);

	//--

	////TODO: BUG:
	//bGui_PresetsParams = true;
	//std::string path = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_ControlSettings;
	//ofxSurfingHelpers::saveGroup(params_Control, path);
}

//--

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL
//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Setup()
{
	ofLogNotice(__FUNCTION__);

	//--

//	// font customize
//#ifdef INCLUDE_IMGUI_CUSTOM_THEME_AND_FONT
//	ofxSurfingHelpers::ImGui_FontCustom();
//#endif

	//--

	// daan fork
	ImGuiConfigFlags flags;
	flags = ImGuiConfigFlags_None;
	flags |= ImGuiConfigFlags_DockingEnable;
	//flags |= ImGuiConfigFlags_ViewportsEnable;
	//flags |= ImGuiDockNodeFlags_NoWindowMenuButton;//from imgui_internal.h
	//flags |= ImGuiDockNodeFlags_NoCloseButton;//?
	//flags |= ImGuiDockNodeFlags_NoResizeX;
	//flags |= ImGuiDockNodeFlags_NoWindowMenuButton;

	//--

	bool bAutoDraw = true;
	bool bRestore = true;
	bool bMouse = false;
	gui_ImGui.setup(nullptr, bAutoDraw, flags, bRestore, bMouse);
	//gui_ImGui.setup(nullptr, true, flags, true, 1);


	//TODO: multicontext
	//gui_ImGui.setup(nullptr, false);
	//gui_ImGui.setup(nullptr, true); // Note: it's also possible to use autodraw

	//--	

	//TODO:
	auto &io = ImGui::GetIO();
	io.ConfigDockingWithShift = true;
	io.ConfigWindowsResizeFromEdges = false;//?

	// fonts
#ifdef INCLUDE_IMGUI_CUSTOM_THEME_AND_FONT
	auto normalCharRanges = io.Fonts->GetGlyphRangesDefault();
	float _size = 11.f;
	std::string _name = "telegrama_render.otf";
	std::string _path = "assets/fonts/" + _name;//assets folder
	customFont = gui_ImGui.addFont(_path, _size, nullptr, normalCharRanges);
	io.FontDefault = customFont;//
#endif

	//--

	// theme customize
#ifdef INCLUDE_IMGUI_CUSTOM_THEME_AND_FONT
	ofxImGuiSurfing::ImGui_ThemeMoebiusSurfing();
#endif

	settings = ofxImGui::Settings();

	//--

	// mouse over
	bImGui_mouseOver.set("mouseOverGui", false);

	//--
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Begin()
{
	gui_ImGui.begin();

	//TODO:
	// Define the ofWindow as a docking space
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0)); // Fixes imgui to expected behaviour. Otherwise add in ImGui::DockSpace() [±line 14505] : if (flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;
	ImGuiID dockNodeID = ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::PopStyleColor();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(PANEL_WIDGETS_WIDTH, PANEL_WIDGETS_HEIGHT));

	//-

#ifdef INCLUDE_IMGUI_CUSTOM_THEME_AND_FONT
	ImGui::PushFont(customFont);
#endif
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_End()
{
#ifdef INCLUDE_IMGUI_CUSTOM_THEME_AND_FONT
	ImGui::PopFont();
#endif

	ImGui::PopStyleVar();

	//--

	gui_ImGui.end();
}
#endif

//--------------------------------------------------------------
bool ofxPresetsManager::ImGui_Draw_Window()
{
	if (bGui)
	{
		static bool auto_resize = true;
		ImGuiWindowFlags flagsw;
		flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;

		float _spcx;
		float _spcy;
		float _w100;
		float _h100;
		float _w99;
		float _w50;
		float _w33;
		float _w25;
		float _h;

		// main panel
		if (bGui_Main) gui_Main(); // main control + extra

		// panels
		if (bGui_Panels) gui_Panels();

		//-

		/*
		// extra window
		// groups selectors
		// only when more than one group!

		//TODO:CRASH
		// hide this panels when no more than one group! it's for multi groups
		if (groups.size() > 1)
		{
			if (bGui_Players)
			{
				//if (ofxImGui::BeginWindow("Randomizers Selector", settings, flagsw))
				if (ofxImGui::BeginWindow("PLAYER SELECTOR", settings, flagsw))
				{
					ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);
								_h *= 2;

					std::string str;

					//ImGui::Dummy(ImVec2(0, 5));

					// 1. selected group
					str = "Group:";
					ImGui::Text(str.c_str());
					if (bBuildGroupSelector) {
						ImGui::PushItemWidth(_w50);
						ofxImGuiSurfing::AddParameter(GuiGROUP_Selected_Index);// user selected wich group to edit
						ImGui::PopItemWidth();
					}
					//str = "Group Name:";
					//ImGui::Text(str.c_str());
					str = PRESETS_Selected_Index[GuiGROUP_Selected_Index].getName();
					ImGui::Text(str.c_str());

					//-

					ImGui::Dummy(ImVec2(0, 5));

					// 2. main group link selector

					//str = "Global Group:";
					//ImGui::Text(str.c_str());
					if (bBuildGroupSelector) {
						ImGui::PushItemWidth(_w50);
						ofxImGuiSurfing::AddParameter(PRESETS_Selected_Index[groups.size() - 1]);// last group is the main link group
						ImGui::PopItemWidth();
					}

					ImGui::Dummy(ImVec2(0, 2));

					// selector to show their randomizers
					//ofxImGuiSurfing::AddParameter(bSHOW_allGroups);
					ofxImGuiSurfing::AddBigToggle(bSHOW_allGroups, _w100, _h / 2);
				}
				ofxImGui::EndWindow(settings);
			}
		}
		*/

		//-

		//// all group selectors to set current preset
		//if (SHOW_Panel_AllSelectors) gui_Selectors();

		// all parameters from all groups
		if (bGui_Parameters) gui_Parameters();

		//-

		// selected randomizers for selected group
		// handles matrix button selector too
		//TODO:CRASH
		gui_Randomizers();

		//-

		// standalone presets browser
		if (bGui_Standalones) gui_Standalones();

		//--

#ifdef INCLUDE_ofxSurfingRandomizer
		dataRandomizer.drawImGuiWidgets();
#endif
	}

	return bLockMouseByImGui.get();
	//return settings.mouseOverGui;
}

// ImGui
//--------------------------------------------------------------
void ofxPresetsManager::gui_Parameters()
{
	static bool auto_resize = true;
	ImGuiWindowFlags flagsw;
	flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;

	string n = "PRESETS PARAMETERS";

	guiManager.beginWindow(n.c_str(), (bool*)&bGui_Parameters.get(), flagsw);
	{
		float _w100;
		float _h100;
		float _w99;
		float _w50;
		float _w33;
		float _w25;
		float _h;

		_w100 = getWidgetsWidth(1);
		_w50 = getWidgetsWidth(2);
		_w33 = getWidgetsWidth(3);
		_w25 = getWidgetsWidth(4);
		_h = getWidgetsHeightUnit() * 2;

		//-

		/*
		// group selector
		// (only when more than one group)

		if (groups.size() > 1) {
			ofxImGuiSurfing::AddStepper(GuiGROUP_Selected_Index);

			// name of selected group
			std::string str = PRESETS_Selected_Index[GuiGROUP_Selected_Index].getName();
			ImGui::Text(str.c_str());
		}

		//preset selector
		//ofxImGuiSurfing::AddStepper(PRESETS_Selected_Index[0]);
		*/

		//-

		//ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 5));

		// 1. Each group parameters

		for (int i = 0; i < groups.size(); i++)
		{
			if (groups.size() > 1)
			{
				//if (i != 0) 
				{
					ImGui::Separator();
					ImGui::Dummy(ImVec2(0, 5));
				}
			}

			bool bLast = (i == groups.size() - 1);
			if (!bLast)
			{
				string s = "#" + ofToString(i);
				ImGui::Text(s.c_str());
			}

			if (bLast) {
				//ImGui::Separator();
				//ImGui::Dummy(ImVec2(0, 5));
				ImGui::PushItemWidth(_w50);
			}

			//ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
			//flags |= ImGuiTreeNodeFlags_Framed;
			//if (!bLast || groups.size() >= 1) flags |= ImGuiTreeNodeFlags_DefaultOpen;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
			if (!bLast) flags |= ImGuiTreeNodeFlags_Framed;

			// Group
			ofxImGuiSurfing::AddGroup(groups[i], flags);
			//guiManager.AddGroup(groups[i]); // -> fails

			if (bLast) {
				ImGui::PopItemWidth();
			}

			if (!bLast) ImGui::Dummy(ImVec2(0, 5));
		}

		//--

		/*

		ImGui::Dummy(ImVec2(0, 5));

		// 2. Tools
		// random and reset tools

		if (ImGui::CollapsingHeader("TOOLS", ImGuiTreeNodeFlags_None))
			//if (ImGui::CollapsingHeader("TOOLS", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("RANDOM PARAMS", ImVec2(_w50, _h)))
			{
				doRandomFiltered();
			}
			ImGui::SameLine();

			if (ImGui::Button("RANDOM FULL", ImVec2(_w50, _h)))
			{
				doRandomFull();
			}

			if (ImGui::Button("RESET FILTER", ImVec2(_w50, _h)))
			{
				doResetFiltered();
			}
			ImGui::SameLine();

			if (ImGui::Button("RESET FULL", ImVec2(_w50, _h)))
			{
				doResetFull();
			}

			if (groups.size() > 1) {
				ImGui::Dummy(ImVec2(0, 2));
				ofxImGuiSurfing::AddBigToggle(bGui_Players, _w100, _h / 2);
			}

			ImGui::Dummy(ImVec2(0, 2));

			ImGui::Checkbox("Auto-Resize", &auto_resize);
		}
		*/

		//--

		// extra params not included into presets
		if (bAppStateParams) ofxImGuiSurfing::AddGroup(params_AppSettings);
	}
	guiManager.endWindow();
}

////--------------------------------------------------------------
//void ofxPresetsManager::gui_Selectors()
//{
//	static bool auto_resize = true;
//	ImGuiWindowFlags flagsw;
//	flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;
//
//	if (ofxImGui::BeginWindow("SELECTORS", settings, flagsw))
//	{
//		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
//		flags |= ImGuiTreeNodeFlags_DefaultOpen;
//		ofxImGuiSurfing::AddGroup(params_GroupsSelectors, flags);
//		//ofxImGuiSurfing::AddGroup(params_GroupsSelectors, settings);
//
//		//ImGui::Checkbox("Auto-Resize", &auto_resize);
//	}
//	ofxImGui::EndWindow(settings);
//}

//--------------------------------------------------------------
void ofxPresetsManager::gui_Panels()
{
	static bool auto_resize = true;
	ImGuiWindowFlags flagsw;
	flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;
	bool bOpen;
	ImGuiWindowFlags _flagw;

	float _spcx;
	float _spcy;
	float _w100;
	float _h100;
	float _w99;
	float _w50;
	float _w33;
	float _w25;
	float _h;
	float _hh;

	string n = "PRESETS PANELS";

	guiManager.beginWindow(n.c_str(), (bool*)&bGui_Panels.get(), flagsw);
	{
		ofxImGuiSurfing::refreshImGui_WidgetsSizes(_spcx, _spcy, _w100, _h100, _w99, _w50, _w33, _w25, _h);

		//--

		// 4. Panels
		{
			ofxImGuiSurfing::AddToggleRoundedButton(bGui_Main);
			ofxImGuiSurfing::AddToggleRoundedButton(bGui_Clicker);
			ofxImGuiSurfing::AddToggleRoundedButton(bGui_Parameters);

			//-

			// Surfing Engines

#ifdef INCLUDE_ofxSurfingSmooth
			ofxImGuiSurfing::AddToggleRoundedButton(smoother.bGui);
#endif

			//-

#ifdef INCLUDE_ofxSurfingTween
			ImGui::Indent();
			bOpen = true;
			_flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);
			if (ImGui::CollapsingHeader("TWEENER", _flagw))
			{
				ofxImGuiSurfing::AddToggleRoundedButton(dataTween.bGui);
			}
			ImGui::Unindent();
#endif

			//-

			//ImGui::Indent();
			//bOpen = true;
			//_flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);
			//if (ImGui::CollapsingHeader("PLAYER", _flagw))
			//{
			//	ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);
			//_h *= 2;
			//	if (groups.size() >= 1)
			//	{
			//		ImGui::Dummy(ImVec2(0, 2));
			//		ofxImGuiSurfing::AddBigToggle(bGui_Players, _w100, _hh);
			//	}
			//	//ofxImGuiSurfing::AddBigToggle(bGui_PlayerEditor, _w100, _hh / 2); //ImGui::SameLine();
			//	////ofxImGuiSurfing::AddBigToggle(bGui_RandomizerParams, _w100, _hh);
			//	//ImGui::Dummy(ImVec2(0.f, 2.f));
			//	//ofxImGuiSurfing::AddBigToggle(bGui_Players, _w100, _h / 2);
			//	// selector to show their randomizers
			//	ofxImGuiSurfing::AddBigToggle(bSHOW_allGroups, _w100, _h / 2);
			//}
			//ImGui::Unindent();

			//-

			ImGui::Indent();
			bOpen = true;
			_flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);
			if (ImGui::CollapsingHeader("RANDOMIZERS", _flagw))
			{
				ImGui::PushItemWidth(_w33);
				ofxImGuiSurfing::AddParameter(GuiGROUP_Selected_Index); // user selected wich group to edit
				ImGui::PopItemWidth();

				ofxImGuiSurfing::AddToggleRoundedButton(bGui_Players);
				//ofxImGuiSurfing::AddToggleRoundedButton(bGui_PlayerEditor);

				//-

#ifdef INCLUDE_ofxSurfingRandomizer
				ofxImGuiSurfing::AddToggleRoundedButton(dataRandomizer.bGui);
				//ofxImGuiSurfing::AddToggleRoundedButton(dataRandomizer.bEditor);
#endif
			}
			ImGui::Unindent();

			//-

			// 4.2 Extra

			ImGui::Indent();
			bOpen = false;
			_flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);

			if (ImGui::CollapsingHeader("EXTRA", _flagw))
			{
				ofxImGuiSurfing::AddToggleRoundedButton(bGui_Standalones);
				if (MODE_Editor) ofxImGuiSurfing::AddToggleRoundedButton(bGui_AdvancedControl);
				ofxImGuiSurfing::AddToggleRoundedButton(bHelp);
			}
			ImGui::Unindent();
		}
	}

	//--

	guiManager.endWindow();
}

//--------------------------------------------------------------
void ofxPresetsManager::gui_Main()
{
	static bool auto_resize = true;
	ImGuiWindowFlags flagsw;
	flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;
	string n = "PRESETS MAIN";

	guiManager.beginWindow(n.c_str(), NULL, flagsw);
	{
		float _w100;
		float _w50;
		float _w33;
		float _w25;
		float _h;
		ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);
		_h *= 2;

		std::string str;

		//---

		// 0. labels

		ImGui::Dummy(ImVec2(0, 5));

		// label for User-Kit folder
		str = "USER-KIT:";
		//str = "User-Kit:";
		ImGui::Text(str.c_str());
		str = path_UserKit_Folder;
		ofStringReplace(str, "/", "/\n");//split in lines to reduce panel width
		str += "/";
		ImGui::Text(str.c_str());

		ImGui::Dummy(ImVec2(0, 2));

		// aliases
		int ig = GuiGROUP_Selected_Index.get();
		int ip = PRESETS_Selected_Index[ig];

		// name of selected group
		str = PRESETS_Selected_Index[ig].getName();
		ImGui::Text(str.c_str());

		if (groups.size() > 1) {
			str = "Group #" + ofToString(ig) + "/" + ofToString(groups.size() - 1);
			ImGui::Text(str.c_str());
		}
		//ImGui::SameLine();

		str = "Preset " + ofToString(ip) + "/" + ofToString(keys[ig].size() - 1);
		ImGui::Text(str.c_str());

		ImGui::Spacing();

		//--

		// 1. Edit Mode

		ofxImGuiSurfing::AddBigToggleNamed(MODE_Editor, _w100, 2 * _h, "EDIT MODE", "LIVE MODE", true);

		// Save
		//if (!MODE_Editor)
		{
			if (ImGui::Button("RELOAD", ImVec2(_w50, _h)))
			{
				loadPreset(PRESETS_Selected_Index[GuiGROUP_Selected_Index], GuiGROUP_Selected_Index);
			}
			ImGui::SameLine();
			if (ImGui::Button("SAVE", ImVec2(_w50, _h)))
			{
				savePreset(PRESETS_Selected_Index[GuiGROUP_Selected_Index], GuiGROUP_Selected_Index);
			}
		}

		//-

		// 1.2 Next / Previous

		ImGui::PushButtonRepeat(true);
		{
			if (ImGui::Button("<", ImVec2(_w50, _h)))
			{
				int ig = GuiGROUP_Selected_Index.get();
				load_Previous(ig, true);
			}
			ImGui::SameLine();
			if (ImGui::Button(">", ImVec2(_w50, _h)))
			{
				int ig = GuiGROUP_Selected_Index.get();
				load_Next(ig, true);
			}
		}
		ImGui::PopButtonRepeat();

		ofxImGuiSurfing::AddToggleRoundedButton(bGui_Panels);

		//--

//#ifdef INCLUDE_ofxSurfingRandomizer
//		//ofxImGuiSurfing::AddBigToggle(dataRandomizer.bGui, _w100, _h);
//		ImGui::Dummy(ImVec2(0.f, 2.f));
//#endif
		
		ImGui::Spacing();

		//--

		// 2. Selectors Sliders

		std::string name;
		if (groups.size() <= 1) name = "SELECTOR";
		else name = "SELECTORS";

		if (ImGui::CollapsingHeader(name.c_str()))
		{
			ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);
			_h *= 2;

			// multi groups ?
			//if (bBuildGroupSelector) 
			{
				//str = "Group Name:";
				//ImGui::Text(str.c_str());
				str = PRESETS_Selected_Index[GuiGROUP_Selected_Index].getName();
				ImGui::Text(str.c_str());

				//-

				// 1. selected group
				str = "Group:";
				ImGui::Text(str.c_str());
				if (bBuildGroupSelector) 
				{
					ImGui::PushItemWidth(_w33);
					ofxImGuiSurfing::AddParameter(GuiGROUP_Selected_Index); // user selected wich group to edit
					ImGui::PopItemWidth();
				}

				//-

				ImGui::Text("Preset:");
				ImGui::PushItemWidth(_w33);
				ofxImGuiSurfing::AddParameter(PRESETS_Selected_Index[GuiGROUP_Selected_Index]);
				ImGui::PopItemWidth();

				//ofxImGuiSurfing::AddBigToggle(bGui_Players, _w100, _h / 2);

				//// selector to show their randomizers
				//ofxImGuiSurfing::AddBigToggle(bSHOW_allGroups, _w100, _h / 2);

				//----

				// Presets Selectors

				//ImGui::Indent();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
				//flags |= ImGuiTreeNodeFlags_DefaultOpen;
				ImGui::PushItemWidth(_w33);
				ofxImGuiSurfing::AddGroup(params_GroupsSelectors, flags);
				ImGui::PopItemWidth();
				//ImGui::Unindent();
			}
		}

		ImGui::Spacing();

		//--

		// 3. Play buttons

		if (ImGui::CollapsingHeader("PLAYERS"))
		{
			ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

			ofxImGuiSurfing::AddToggleRoundedButton(bGui_Players);

			if (groups.size() > 1)
			{
				ofxImGuiSurfing::AddToggleRoundedButton(bSHOW_allGroups);
			}

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			for (int i = 0; i < groupRandomizers.size(); i++)
			{
				ImGui::PushID(i);
				string s = groups[i].getName();
				ImGui::Text(s.c_str());

				// Blink by timer
				ofxImGuiSurfing::AddBigToggleNamed(groupRandomizers[i].PLAY_RandomizeTimer, _w100, _h, "PLAYING", "PLAY", true, groupRandomizers[i].getPlayerPct());

				ImGui::PopID();
			}
		}

		//--

		// 3. tools / helpers

		if (MODE_Editor)
		{
			if (ImGui::CollapsingHeader("TOOLS"))
			{
				ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);
				_h *= 2;

				//TODO: select which added group
				int i = 0;
				//int ii = PRESETS_Selected_Index[i];

				if (ImGui::Button("CLONE ALL", ImVec2(_w50, _h / 2)))
				{
					doCloneAll(i);
				}
				ImGui::SameLine();
				if (ImGui::Button("CLONE >", ImVec2(_w50, _h / 2)))
				{
					doCloneRight(i);
				}
			}

			//-

			////TODO:
			////create all presets randomized
			////if (ImGui::Button("POPULATE", ImVec2(_w99, _h / 2)))
			////{
			////	int ig = GuiGROUP_Selected_Index.get();
			////	doPopulateFavs(ig);
			////}
		}

		//--

		// 5. Undo Engine

#ifdef INCLUDE_ofxUndoSimple
		if (MODE_Editor.get())
		{
			if (ImGui::CollapsingHeader("UNDO ENGINE"))
			{
				ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);
				_h *= 2;

				str = "History: " + ofToString(undoStringsParams[GuiGROUP_Selected_Index].getUndoLength()) + "/";
				str += ofToString(undoStringsParams[GuiGROUP_Selected_Index].getRedoLength());
				ImGui::Text(str.c_str());

				if (ImGui::Button("Store", ImVec2(_w50, _h / 2)))
				{
					doStoreUndo();
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear", ImVec2(_w50, _h / 2)))
				{
					doClearUndoHistory();
				}
				//ImGui::SameLine();

				if (ImGui::Button("Undo", ImVec2(_w50, _h / 2)))
				{
					doUndo();
				}
				ImGui::SameLine();
				if (ImGui::Button("Redo", ImVec2(_w50, _h / 2)))
				{
					doRedo();
				}
		}
	}
#endif

		//--

		// 6. Advanced

		// workflow: force hide when live mode
		if (bGui_AdvancedControl && MODE_Editor) gui_Advanced();

		//-

		// 7. Mouse over gui

		static bool bAutoLocKeysOnMouseHoverGui = false;
		bLockMouseByImGui = false;
		if (bAutoLocKeysOnMouseHoverGui) {
			bLockMouseByImGui = bLockMouseByImGui | ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
			bLockMouseByImGui = bLockMouseByImGui | ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
			bLockMouseByImGui = bLockMouseByImGui | ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
			// must be insisde some window
		}

		//--
}
	guiManager.endWindow();

	//crash
	//ImGui::PopStyleColor();
	//ImGui::PopStyleColor();
}

//--------------------------------------------------------------
void ofxPresetsManager::gui_Advanced()
{
	//if (ImGui::CollapsingHeader("EXTRA"))
	{
		ImGuiTreeNodeFlags flagsTree;
		flagsTree = ImGuiTreeNodeFlags_Framed;

		if (ImGui::TreeNodeEx("ADVANCED", flagsTree))
		{
			float _w100;
			float _w50;
			float _w33;
			float _w25;
			float _h;
			ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

			//--

			ofxImGuiSurfing::AddParameter(bKeys);
			ofxImGuiSurfing::AddParameter(bAutoSave);
			ofxImGuiSurfing::AddParameter(bAutoLoad);

			// TODO: 
			//faster memory mode vs xml files not implemented yet
			//ofxImGuiSurfing::AddParameter(MODE_MemoryLive);//ImGui::SameLine();
			//ofxImGuiSurfing::AddParameter(MODE_EditPresetClicker);
			//ofxImGuiSurfing::AddParameter(bHelp);//ImGui::SameLine();

			if (ImGui::TreeNodeEx("PRESET CLICKER", flagsTree))
			{
				ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

				ofxImGuiSurfing::AddBigToggle(bThemeDarkOrLight, _w100, _h);

				if (MODE_Editor)
				{
					ofxImGuiSurfing::AddToggleRoundedButton(bLockClicker);
					if (!bLockClicker) {
						ofxImGuiSurfing::AddBigToggle(MODE_EditPresetClicker, _w100, _h);
					}
					ofxImGuiSurfing::AddBigToggle(bGui_BgEditPresetClicker, _w100, _h);
					ofxImGuiSurfing::AddStepper(cellSize);
					//if (bHelp)ofxImGuiSurfing::AddParameter(helpPos);
				}

				ImGui::TreePop();
			}

			//--

			if (MODE_Editor)
			{
				if (ImGui::TreeNodeEx("USER-KIT", flagsTree))
				{
					ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);
					_h *= 2;

					//--

					// User-Kit name
					std::string str;
					str = "Name:\n";
					str += displayNameUserKit;
					ImGui::Text(str.c_str());

					// button to Open File Dialog as folder
					if (ImGui::Button("Set Custom folder", ImVec2(_w100, _h / 2)))
					{
						ofFileDialogResult openFileResult = ofSystemLoadDialog("Select User-Kit folder", true, ofToDataPath(path_UserKit_Folder, true));

						// check if the user opened a file
						if (openFileResult.bSuccess) {

							ofLogNotice(__FUNCTION__) << ("User selected a folder");

							// we have a file, check it and process it
							doFileDialogProcessSelection(openFileResult);
						}
						else ofLogNotice(__FUNCTION__) << ("User hit cancel");
					}

					ImGui::Spacing();
					//ImGui::Dummy(ImVec2(0, 5));

					// monitor custom state
					//ofxImGuiSurfing::AddParameter(bPathDirCustom);

#ifdef USE_PRESETS_MANAGER__IMGUI_LAYOUT 
					ofxImGuiSurfing::AddToggleRoundedButton(guiManager.bAdvanced);
					if (guiManager.bExtra) guiManager.drawAdvancedSubPanel();
#endif
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
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

	//// file format
	//helpInfo += "       ";
	//#ifdef USE_XML
	//helpInfo += ".xml";
	//#else
	//#ifdef USE_JSON
	//helpInfo += ".json";
	//#endif
	//#endif

	helpInfo += "\n\nPATHS\n";
	helpInfo += getGroupsPaths();
	//helpInfo += "                            ";

	//TODO:
	//bool bKeysinfo = false;
	//if (bKeysinfo)
	//{
	//	//keys[i][k]
	//	helpInfo += "[keys " + ofToString((char)keys[0][0]) + "|";
	//	helpInfo += ofToString((char)keys[0][keys[0].size() - 1]) + "]";
	//}

	helpInfo += "\n";
	helpInfo += "\n";

	helpInfo += "KEYS\n";
	helpInfo += "\n";

	helpInfo += "H                HELP\n";
	helpInfo += "Ctrl+g           GUI\n";
	helpInfo += "E                EDIT\n";
	helpInfo += "\n";

	helpInfo += "P                CLICKER\n";
	helpInfo += "KEYS & MOUSE     LOAD\n";
	helpInfo += " +Ctrl           SAVE\n";
	helpInfo += " +Alt            SWAP\n";
	helpInfo += "Arrows           EXPLORE\n";
	helpInfo += "\n";

	helpInfo += "Ctrl+Space       PLAY RANDOMIZER\n";
	helpInfo += "Ctrl+R           PRESET RANDOMIZE\n";
	helpInfo += "\n";

	helpInfo += "Ctrl+Z           UNDO\n";
	helpInfo += " +Shift          REDO\n";
	helpInfo += "Ctrl+C           CLEAR\n";
	helpInfo += "Ctrl+s           STORE\n";
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
	if (bAutoSave)
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
	if (bAutoLoad)
	{
		// must disable autosave to avoid overwrite between user-kits
		bool bPre = bAutoSave;
		bAutoSave = false;

		// set selected first preset o last/main group
		int _last = groups.size() - 1;
		PRESETS_Selected_Index[_last] = 0;
		load(PRESETS_Selected_Index[_last], _last);
		bAutoSave = bPre;

		//// load all group selected presets
		//for (int g = 0; g < groups.size(); g++)//iterate each group
		//{
		//	load(PRESETS_Selected_Index[g], g);
		//}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::gui_Standalones()
{
	{
		static bool auto_resize = true;
		ImGuiWindowFlags flagsw;
		flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;

		string n = "STANDALONES";
		guiManager.beginWindow(n.c_str(), (bool*)&bGui_Standalones.get(), flagsw);
		{
			float _w100;
			float _w50;
			float _w33;
			float _w25;
			float _h;
			ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

			//--

			int groupIndex = GuiGROUP_Selected_Index.get();
			int _numfiles = standaloneFileNames[groupIndex].size();

			std::string _name;
			std::string str;

			//---

			ImGui::Dummy(ImVec2(0, 5));

			// label for User-Kit folder
			str = "USER-KIT:";
			//str = "User-Kit:";
			ImGui::Text(str.c_str());
			str = path_UserKit_Folder;
			ofStringReplace(str, "/", "/\n"); // split in lines to reduce panel width
			str += "/";
			ImGui::Text(str.c_str());

			ImGui::Dummy(ImVec2(0, 5));

			//--

			// user selected wich group to edit
			ImGui::PushItemWidth(_w50);
			if (bBuildGroupSelector) ofxImGuiSurfing::AddParameter(GuiGROUP_Selected_Index);
			ImGui::PopItemWidth();

			//ImGui::Dummy(ImVec2(0, 5));

			//--

			// name of selected group
			str = "Group Name:";
			ImGui::Text(str.c_str());
			str = PRESETS_Selected_Index[GuiGROUP_Selected_Index].getName();
			ImGui::Text(str.c_str());

			ImGui::Dummy(ImVec2(0, 5));

			//--

			// helpers and browser
			{
				//---

				ImGui::Dummy(ImVec2(0, 5));

				//// label presets folder
				////str = "User-Kit";
				////ImGui::Text(str.c_str());
				//str = "/" + path_PresetsStandalone + "/" + groups[groupIndex].getName();// append group name
				//ImGui::Text(str.c_str());
				//ImGui::Dummy(ImVec2(0, 5));

				//-

				// 0. send/save current browsed (from "/archive/") preset to current presets on favorites

				// get/copy all favs presets from favs and send/save to browser folder ("archive")

				if (ImGui::Button("CREATE FROM FAVS", ImVec2(_w100, 2 * _h)))
				{
					ofLogNotice(__FUNCTION__) << "CREATE FROM FAVS";

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

				ImGui::Dummy(ImVec2(0, 5));

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
					ofStringReplace(_path, "/", "/\n");//split in lines to reduce panel width
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
					ImGui::SetNextItemWidth(_w100 - 20);
					//ImGui::SetNextItemWidth(140);

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

					ImGui::Dummy(ImVec2(0, 5));

					if (ImGui::Button("UPDATE", ImVec2(_w50, _h)))
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
					if (ImGui::Button("RELOAD", ImVec2(_w50, _h)))
					{
						_name = standaloneFileNames[groupIndex][standaloneFileIndex[groupIndex]];
						ofLogNotice(__FUNCTION__) << "RELOAD. filename: " << _name;

						doStandalonePresetLoad(_name, groupIndex);
					}

					//-

					// 4.4 delete

					//ImGui::SameLine();
					if (ImGui::Button("DELETE", ImVec2(_w50, _h)))// current preset
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
					if (ImGui::Button("CLEAR", ImVec2(_w50, _h)))// delete all files
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

				//ofxImGuiSurfing::AddParameter(MODE_StandalonePresets_NEW);
				ofxImGuiSurfing::AddBigToggle(MODE_StandalonePresets_NEW, _w100, _h);

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
					ofxImGuiSurfing::AddParameter(bKeys);
				}
			}
		}
		//ImGui::Checkbox("Auto-Resize", &auto_resize);
		guiManager.endWindow();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::gui_Randomizers()
{
	if (!bGui_RandomizerParams && !bGui_Players) { return; }
	//if (!bGui_PlayerEditor && !bGui_RandomizerParams && !bGui_Players) { return; }

	//--

	// this panel will show the settings for the selected by user group (GuiGROUP_Selected_Index)

	//if (bGui_Players)
	{
		if (GuiGROUP_Selected_Index > groupRandomizers.size() - 1) {
			return;
		}

		//-

		// show randomizers of user selected group

		//TODO:
		// should merge both toggles inside the group class...

		// all
		if (bSHOW_allGroups)
		{
			for (int i = 0; i < groupRandomizers.size(); i++)
			{
				if (bGui_Players) groupRandomizers[i].drawImGui();

				//if (bGui_Players) groupRandomizers[i].drawImGui_RandomizersMain();
				//if (bGui_PlayerEditor) groupRandomizers[i].drawImGui_RandomizerEditPlayer();
				////if (bGui_RandomizerParams) groupRandomizers[i].drawImGui_RandomizerParams();
			}
		}

		// only selected
		else
		{
			if (bGui_Players) groupRandomizers[GuiGROUP_Selected_Index.get()].drawImGui();

			//if (bGui_Players) groupRandomizers[GuiGROUP_Selected_Index.get()].drawImGui_RandomizersMain();
			//if (bGui_PlayerEditor) groupRandomizers[GuiGROUP_Selected_Index.get()].drawImGui_RandomizerEditPlayer();
			//if (bGui_RandomizerParams) groupRandomizers[GuiGROUP_Selected_Index.get()].drawImGui_RandomizerParams();
		}
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

	if (groupIndex < 0)
		//if (groupIndex == -1)
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

	if (groupIndex < 0)
		//if (groupIndex == -1)
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

	if (groupIndex < 0)
		//if (groupIndex == -1)
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
					<< groups[i].getName() << " is empty! Force populate favorites files...";

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
	ofLogWarning(__FUNCTION__) << GuiGROUP_Selected_Index;
}

//--------------------------------------------------------------
void ofxPresetsManager::drawHelp()
{
	helpTextBoxWidget.setText(helpInfo);
	helpTextBoxWidget.setTheme(bThemeDarkOrLight);
	helpTextBoxWidget.draw();
}

//--------------------------------------------------------------
void ofxPresetsManager::addExtra(ofParameterGroup &g)
{
	bool bAppStateParams = true;

	params_AppSettings.clear();
	//params_AppSettings.add(params_Internal);
	params_AppSettings = g;
}

//--------------------------------------------------------------
void ofxPresetsManager::doRectFit()
{
	// calculate preset clicker box, depending of amout groups and subscribed keys/preset for each group

	float _ww;
	float _hh = cellSize * groups.size();
	_hh += 2 * _RectClick_Pad;//pads

	int kmax = 0;
	for (int i = 0; i < keys.size(); i++)
	{
		int sz = keys[i].size();
		if (sz > kmax) kmax = sz;
	}
	kmax += 2;//save+gui button
	_ww = kmax * cellSize;
	_ww += getGroupNamesWidth();//name
	_ww += 2 * _RectClick_Pad;//pads

	rectangle_PresetClicker.setWidth(_ww);
	rectangle_PresetClicker.setHeight(_hh);

	//-

	// fit iniside window
	{
		float _w = ofGetWidth();
		float _h = ofGetHeight();

		int _padx = 10;
		int _pady = 10;

		int _xx;
		int _yy;

		float _ww = rectangle_PresetClicker.getWidth();
		float _hh = rectangle_PresetClicker.getHeight();

		float _xmax = _w - _ww - _padx;
		float _ymax = _h - _hh - _pady;

		if (rectangle_PresetClicker.getY() > _ymax)
		{
			rectangle_PresetClicker.setY(_ymax);
		}
		else if (rectangle_PresetClicker.getX() < _padx)
		{
			rectangle_PresetClicker.setX(_padx);
		}
		else if (rectangle_PresetClicker.getX() > _xmax)
		{
			rectangle_PresetClicker.setX(_xmax);
		}
		else if (rectangle_PresetClicker.getY() < _pady)
		{
			rectangle_PresetClicker.setY(_pady);
		}
	}

	//-

	// doubleClicker
	//update
	int _x = rectangle_PresetClicker.getX();
	int _y = rectangle_PresetClicker.getY();
	int _w = rectangle_PresetClicker.getWidth();
	int _h = rectangle_PresetClicker.getHeight();
	doubleClicker.set(_x, _y, _w, _h);
}