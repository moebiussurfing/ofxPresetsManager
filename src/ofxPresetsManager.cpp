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

	bDISABLE_CALLBACKS = true;

	//-

	// subscribed to auto run update and draw without required 'manual calls'
	ofAddListener(ofEvents().update, this, &ofxPresetsManager::update);
	//ofAddListener(ofEvents().draw, this, &ofxPresetsManager::draw);

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
	path_PresetsFavorites = "presets";
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
	filenamesPrefix = "_"; //"_preset_";

	//// root folder
	//path_Root = "myAddon";

	//--

	modeKeySave = OF_KEY_CONTROL;
	modKeySwap = OF_KEY_ALT;
	guiManager.bKeys = false;
	bModKeySave = false;
	bModKeySwap = false;
	bKeys_NotYetActivated = true;

	//--

	// reserve memory data

	groups.reserve(NUM_MAX_GROUPS);
	keys.reserve(NUM_MAX_GROUPS);
	index_PresetSelected.reserve(NUM_MAX_GROUPS);
	groupRandomizers.reserve(NUM_MAX_GROUPS);

	//--

	// callbacks to know when preset loading/save is done

	DONE_load.set("DONE LOAD", false); // callback to know (in ofApp) that preset LOAD is done
	DONE_save.set("DONE SAVE", false); // callback to know (in ofApp) that preset SAVE is done

	// easy callback: we can use too the easy isDoneLoad() to check in ofApp update() as kind of callback.

	//--

	// control parameters

	bGui_Global.set("_PRESETS_GLOBAL_", true);
	bGui_Global.setSerializable(false);

	bGui.set("PRESETS", true);
	//bGui.set("ofxPresetsManager", true);
	//bGui_PanelsAll.set("PANELS", false);//TODO: deprecated
	bGui_Main.set("MAIN", true);
	bGui_PresetsParams.set("PRESET PARAMETERS", false);
	bGui_Selectors.set("SELECTORS", false);
	bGui_Clicker.set("CLICKER", true);
	bGui_Parameters.set("PARAMETERS", false);
	bGui_Standalones.set("STANDALONES", false);
	bGui_Players.set("PLAYERS", false);
	//bGui_AllSelectors.set("SHOW SELECTORS", false);
	//bGui_PlayerEditor.set("Editors", false);
	//bGui_RandomizerParams.set("RANDOM PARAMS", false);

	//bGui_Panels.set("PANELS", true);
	//bHelp.set("Help", false);
	bGui_AdvancedControl.set("Advanced", false);
	MODE_StandalonePresets_NEW.set("NEW!", false);
	//ENABLE_Keys.setSerializable(false);
	//bKeys.set("Keys", true);
	//bKeys.setSerializable(false);

	bAutoLoad.set("Auto Load", true);
	bAutoSave.set("Auto Save", true);
	MODE_MemoryLive.set("MODE MEMORY", false); // TODO: not implemented
	loadToMemory.set("LOAD TO MEMORY", false);
	saveFromMemory.set("SAVE FROM MEMORY", false);
	bAutosaveTimer.set("TIMER AUTO SAVE", false);

	//displayNamePreset = "NO_NAME_PRESET"; //browser loaded preset name

	//-

	// layout

	Gui_Internal_Position.set("GUI INTERNAL POSITION",
		glm::vec2(ofGetWidth() * 0.5, ofGetHeight() * 0.5),
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
	//params_Options.add(MODE_MemoryLive); // TODO: not implemented
	params_Options.add(loadToMemory);
	params_Options.add(saveFromMemory);
	params_Options.add(bAutoLoad);
	params_Options.add(bAutoSave);
	params_Options.add(bAutosaveTimer);

	params_Gui.setName("GUI");
	params_Gui.add(bGui_AdvancedControl);
	params_Gui.add(bGui_Parameters);
	params_Gui.add(bGui_Clicker);
	params_Gui.add(bGui_Standalones);
	params_Gui.add(bGui_Players);
	params_Gui.add(bGui);
	//params_Gui.add(bGui_PanelsAll);
	params_Gui.add(bGui_Main);
	params_Gui.add(bGui_PresetsParams);
	//params_Gui.add(bGui_PlayerEditor);
	//params_Gui.add(bGui_RandomizerParams);
	//params_Gui.add(bGui_AllSelectors);
	params_Gui.add(guiManager.bKeys);

	// layout
	//params_Gui.add(ImGui_Position);
	//params_Gui.add(ImGui_Size);
	params_Gui.add(Gui_Internal_Position);
	params_Gui.add(guiManager.bDebug);

	//--

	// all nested params for callbacks and storage settings
	params_Control.setName("ofxPresetsManager");
	ofAddListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Control);

	//--

	// gui box clicker font
	std::string str;

	sizeTTF = 9;
	str = "telegrama_render.otf";

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

	//bAutoDraw = false;
}

//----

// Setup methods

//--------------------------------------------------------------
void ofxPresetsManager::setup_ImGui()
{

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL	

	ImGui_Setup();

#endif

	//--

	// This instantiates and configures ofxImGui inside the (current ->this) class object.
#ifdef USE_PRESETS_MANAGER__IMGUI_LAYOUT 

#ifndef USE_SPECIAL_WINDOWS
	guiManager.setup();
#else

	guiManager.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
	guiManager.setup();

	guiManager.addWindowSpecial(bGui_Main);
	guiManager.addWindowSpecial(bGui_Clicker);
	guiManager.addWindowSpecial(bGui_Parameters);
	guiManager.addWindowSpecial(bGui_Standalones);

	guiManager.startup();

#endif

	//--

	// Customizations
	//guiManager.bKeys.makeReferenceTo(bKeys);
	guiManager.setEnableHelpInfoInternal(false);
	guiManager.bHelp.setName("Help Presets");

	setupGuiStyles();

#endif
}

//--------------------------------------------------------------
void ofxPresetsManager::setup()
{
	ofLogNotice(__FUNCTION__);
	// filenameMainSettings will use default name "ofxPresetsManager_Settings.xml"
	setup(bAllowGroupSelector); // split folders by group name
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

	bDISABLE_CALLBACKS = true;

	//--

	// create main GROUP selector only when we have added more than one group. 
	// if not, we don't have nothing to group!
	if (!bAllowGroupSelector) { // can be disabled also with this flag!
		bBuildGroupSelector = false;
		bGui_Selectors = false;
	}
	else
		if (index_PresetSelected.size() > 1 && groups.size() > 1) {
			bBuildGroupSelector = _buildGroupSelector;
		}
		else {
			bBuildGroupSelector = false;
			bGui_Selectors = false;
		}

	//--

	// Group selector

	if (bBuildGroupSelector)
	{
		// will combine all group
		//#define NUM_MAIN_SELECTOR_PRESETS 8
		//mainSelector.setMax(NUM_MAIN_SELECTOR_PRESETS);

		for (int i = 0; i < index_PresetSelected.size(); i++)
		{
			params_Index_GroupSelectorMain.add(index_PresetSelected[i]);
		}

		// TODO: should allow customize keys to avoid collide with 0,1,2.. and customize amount too
		// create the extra main selector
		vector<int> _keys;
		char _charFirst = '1'; //1, 2, 3, 4, ...
		for (int i = 0; i < groupLinkSize; i++)
		{
			_keys.push_back(_charFirst++);
			//if(_charFirst==0) _charFirst=

			//TODO: could make a char list
			// qwerty...asdfg...
		}
		add(params_Index_GroupSelectorMain, _keys);
		//add(params_Index_GroupSelectorMain, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

		//--

		// TODO:
		// store the main group link selector only!
		int _last = groups.size() - 1;

		index_GROUP_LINK.setMax(groupsSizes[_last] - 1);
		index_GROUP_LINK.makeReferenceTo(index_PresetSelected[_last]); // TODO: link

		// excludes all selectors except the main one. the other will be saved as preset
		params_Index_GroupsSelectors.setSerializable(false);
	}

	//--

	index_PresetSelected_PRE.resize(index_PresetSelected.size());
	for (int i = 0; i < index_PresetSelected_PRE.size(); i++)
	{
		index_PresetSelected_PRE[i] = -1;
	}

	//----

	// ImGui
	setup_ImGui();

	//----

	// TODO: Main group only

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

#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER
	bMODE_EditPresetClicker.set("Edit Clicker", false);
	bMODE_EditPresetClicker.setSerializable(false);
	bGui_BgEditPresetClicker.set("Box Clicker", false);

	params_Gui.add(bThemeDarkOrLight);
	params_Gui.add(bMODE_EditPresetClicker);
	params_Gui.add(bGui_BgEditPresetClicker);

	params_Control.add(bMODE_EditPresetClicker);
	params_Control.add(bThemeDarkOrLight);
	params_Control.add(cellSize);
#endif

	//----

	bSplitGroupFolders.setSerializable(false); // force to this mode. do not store

	params_Options.add(bMODE_EditLive);
	params_Options.add(bSplitGroupFolders);

	params_Gui.add(bGui_Selectors);
	params_Gui.add(bGui_PresetsParams);
	params_Gui.add(guiManager.bHelp);
	params_Gui.add(guiManager.bHelpInternal);

	params_Control.add(params_Options);
	params_Control.add(params_Gui);

	//params_Control.add(bGui_ShowAllGroups);
	//params_Control.add(bGui_Panels);

#ifdef INCLUDE_ofxSurfingRandomizer
#ifdef INCLUDE_ofxSurfingTween
	params_Control.add(dataTween.bGui);
	params_Control.add(dataRandomizer.bGui);
#endif
#endif

	//--

	// User-Kit

	params_User.setName("USER-KIT");
	params_User.add(params_Index_GroupsSelectors); //includes all selectors

	// Main group link selector
	int _last = groups.size() - 1;

	// This is a strange situation
	// this is to know if no groups has been added before call setup! 
	if (_last >= 0) {

		index_GROUP_LINK.set("G_LINK", 0, 0, groupsSizes[_last] - 1);
		params_User.add(index_GROUP_LINK);

		//----

		// User gui selector
		//bGui_ShowAllGroups.set("Show All", false);
		//bGui_ShowAllGroups.setSerializable(false);

		index_GroupSelected.set("GROUP", 0, 0, groups.size() - 1);
		index_GroupSelected.addListener(this, &ofxPresetsManager::Changed_Index_GroupSelected);

		groupRandomizers.resize(groups.size());

		for (int i = 0; i < groups.size(); i++)
		{
			// Link selectors from class to group randomizer
			groupRandomizers[i].index_PresetSelected.makeReferenceTo(index_PresetSelected[i]); // TODO: link

			// Link minimized
			groupRandomizers[i].bMinimize.makeReferenceTo(guiManager.bMinimize);

			groupRandomizers[i].bGui_PlayerEditor.makeReferenceTo(bGui_PlayerEditor);

			//-

			// Set the name for the randomizer settings file
			if (bSplitGroupFolders.get())
			{
				std::string _path;
				_path = path_UserKit_Folder + "/" + path_PresetsFavorites; // current kit-presets presets folder
				_path += "/" + groups[i].getName(); // append group name
				_path += "/"; // the folder

				ofxSurfingHelpers::CheckFolder(_path); // check parent container folder

				_path += groups[i].getName();
				_path += filename_Randomizers; // the full path with filename

				groupRandomizers[i].setPath_RandomizerSettings(_path);
			}

			//-

			groupRandomizers[i].setup(groups[i], keys[i]); // pass the group and the associated keys

			//-

			ofParameterGroup _g{ groups[i].getName() };
			_g.add(index_PresetSelected[i]); // selector
			_g.add(groupRandomizers[i].bPLAY_RandomizeTimer); // play
			_g.add(groupRandomizers[i].randomizeDurationBpm); // bpm
			//_g.add(groupRandomizers[i].getParamsRandomizers()); // all params
			//_g.add(groupRandomizers[i].bRandomizeIndex); // random index
			params_Index_GroupsSelectors.add(_g);

			//----

			// Smoother

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

	params_User.add(index_GroupSelected);

	//----

	// custom path
	// 
	// this allows to use different kits for the same project/app
	params_Custom.setName("CUSTOM PATH");
	params_Custom.add(bPathDirCustom);
	params_Custom.add(pathDirCustom);
	params_User.add(params_Custom);

	//--

	params_User.add(bPLAY_Global);

	params_User.add(bGui_PlayerEditor);

	ofAddListener(params_User.parameterChangedE(), this, &ofxPresetsManager::Changed_User);

	//----

	// randomizers

	////TODO:
	//// Workflow:
	//for (int i = 0; i < groups.size(); i++)
	//{
	//	//groupRandomizers[i].bPLAY_RandomizeTimer.addListener(this, &ofxPresetsManager::Changed_Randomizers);
	//	////listeners.push(groupRandomizers[i].bPLAY_RandomizeTimer.newListener(this, &ofxPresetsManager::Changed_Randomizers));
	//}

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
#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL
	doubleClicker.set(0, 0, ofGetWidth(), ofGetHeight()); //full screen
	doubleClicker.setDebug(false);
#endif

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

	bDISABLE_CALLBACKS = false; // enable callbacks after setup

	//--

	// load all app session settings & randomizers (not the related presets)
	load_AppSettings(); // here bPathDirCustom is loaded (but if files are present, not in first runtime)

	//--

	// custom path
	if (bPathDirCustom.get())
	{
		if (pathDirCustom.get() != "DEFAULT_DATA_PATH") buildCustomUserKit();
		else bPathDirCustom = false; // force false
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

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL
	rectangle_PresetClicker.setLockResize(true);
	// load settings
	//doRectFit();
	rectangle_PresetClicker.loadSettings(path_RectanglePresetClicker, path_UserKit_Folder + "/" + path_ControlSettings + "/", false);
#endif

	//--

	//// TODO: 
	//// memory mode
	//load_AllKit_ToMemory();

	//--

	//// TODO:
	// Workflow:
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
	//	cout << "index_PresetSelected[_last] : " << index_PresetSelected[_last] << endl;
	//	index_PresetSelected[_last] = groupsSizes[_last];
	//	cout << "index_PresetSelected[_last] : " << index_PresetSelected[_last] << endl;
	//	index_PresetSelected[_last] = index_GROUP_LINK.get();
	//	cout << "index_PresetSelected[_last] : " << index_PresetSelected[_last] << endl;
	//}

	//--

	// ImGui Clicker labels
	labels.clear();
	for (size_t i = 0; i < keys.size(); ++i)
	{
		vector<string> _labels;
		for (auto& kk : keys[i])
		{
			string s = ofToString(char(kk));
			_labels.push_back(s);
		}
		labels.push_back(_labels);
	}

	//--

	// TODO:
	// refresh in other another better place?...
	buildHelpInfo();

	//--

#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER
	// workaround
	//bMODE_EditPresetClicker = bMODE_EditPresetClicker.get();
	//required when no xml settings yet
	bThemeDarkOrLight = bThemeDarkOrLight.get();
#endif

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
	////load_AppSettings(); // here bPathDirCustom is loaded (but if files are present, not in first runtime)

	//std::string path = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_ControlSettings;
	//ofxSurfingHelpers::loadGroup(params_Control, path);
}

//--------------------------------------------------------------
void ofxPresetsManager::update(ofEventArgs& args)
{
	//	cout << "bImGui_mouseOver " << bImGui_mouseOver.get() << endl;
	//	cout << "bImGui_mouseOver_PRE " << bImGui_mouseOver_PRE<< endl;
	//	cout << "bDoneSetup " << bDoneSetup << endl;

	if (!bDISABLE_CALLBACKS)
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

		// performance
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

		//ImGui_Draw_Windows();
		bImGui_mouseOver = ImGui_Draw_Windows(); // handling mouse over gui to disable keys callbacks

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
void ofxPresetsManager::draw()
{
	if (!bGui_Global) return;
	if (!bGui) return;

	//----

	// Clicker boxes preset selector native

#ifdef DISABLE__PRESETS_MANAGER__NATIVE_CLICKER
	if (bGui_Clicker)
	{
		draw_Gui_ClickerPresets_Native();
	}
#endif

	//----

	// ImGui

	//if (!bGui_PanelsAll && !bGui_Clicker) return;

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL	

	ImGui_Draw();

#endif

#ifdef USE_PRESETS_MANAGER__IMGUI_LAYOUT 

	guiManager.begin();
	{
		//if (bGui_PanelsAll) ImGui_Draw_Windows();
		ImGui_Draw_Windows();

		// ImGui Clicker
		if (bGui_Clicker) draw_Gui_ClickerPresets_ImGui();
	}
	guiManager.end();

#endif

	//----

	// Help info text: 

	if (guiManager.bHelp)
	{
		draw_Help();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::windowResized(int w, int h)
{
	ofLogVerbose(__FUNCTION__) << w << "," << h;
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
	index_PresetSelected.clear();
	params_Index_GroupSelectorMain.clear();
	params_Index_GroupsSelectors.clear();
	index_PresetSelected_PRE.clear();

	//params_Options.clear();
	//params_Gui.clear();
	//params_Control.clear();

	params_User.clear();
	groupRandomizers.clear();
	params_Custom.clear();
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup _params, int _amountPresets)
{
	// main adder of a ofParameterGroup's but kind of internal yet. TODO: fix to work as public..

	ofLogNotice(__FUNCTION__) << "Added group " << _params.getName() << " with " << _amountPresets << " presets";
	int _size = groups.size();

	groups.push_back(_params); // each enqueued group of params handles all presets of each added ofParameterGroup
	groupsSizes.push_back(_amountPresets); // store the size or amount of presets of each row/group

	// preset selectors
	ofParameter<int> p{ groups[_size].getName(), 0, 0,  _amountPresets - 1 };
	//p.setSerializable(false); // exclude saving all selectors except last one, that will be enalbed at setup
	index_PresetSelected.push_back(p);
	//params_Index_GroupsSelectors.add(index_PresetSelected[_size]); // add this new param (lastone)

	//-

	ofLogNotice(__FUNCTION__) << "group " << (_size) << " : " << groups[_size].getName();
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup _params, vector<int> _keysList)
{
	// adds a ofParameterGroup for preset management with the list of trigger keys

	add(_params, _keysList.size()); // re add with known size affter counted passed key triggers

	keys.resize(groups.size());
	int i = groups.size() - 1;
	keys[i].reserve(_keysList.size());

	for (const int& key : _keysList)
	{
		keys[i].push_back(key);
	}

	if (bKeys_NotYetActivated) addKeysListeners();
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup _params, initializer_list<int> _keysList)
{
	// adds a ofParameterGroup for preset management with the list of trigger keys

	add(_params, _keysList.size()); // re add with known size affter counted passed key triggers

	keys.resize(groups.size());
	int i = groups.size() - 1;
	keys[i].reserve(_keysList.size());

	for (const int& key : _keysList)
	{
		keys[i].push_back(key);
	}

	if (bKeys_NotYetActivated) addKeysListeners();
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

		TS_START("SAVE FILE 2"); //for TimeMeasurements only
		std::string _path = getPresetPath(gName, presetIndex);
		bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
		if (b) ofLogNotice(__FUNCTION__) << "name: " << groups[guiIndex].getName() << " " << guiIndex << " " << _path;
		else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
		TS_STOP("SAVE FILE 2"); //for TimeMeasurements only

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
	//			TS_START("SAVE FILE 2"); //for TimeMeasurements only
	//			std::string _path = getPresetPath(gName, presetIndex);
	//			bool b = ofxSurfingHelpers::saveGroup(groups[guiIndex], _path);
	//			if (b) ofLogNotice(__FUNCTION__) << " " << groups[guiIndex].getName() << " : " << guiIndex << " " << _path;
	//			else ofLogError(__FUNCTION__) << "Error saving: " << groups[guiIndex].getName() << " " << _path;
	//			TS_STOP("SAVE FILE 2"); //for TimeMeasurements only
	//		}

	//		//MODE B: direct from memory
	//		else
	//		{
	//			TS_START("SAVE MEM 2"); //for TimeMeasurements only

	//			if (presetIndex < mainGroupMemoryFilesPresets.size()) {
	//				ofSerialize(mainGroupMemoryFilesPresets[presetIndex], groups[guiIndex]);
	//			}
	//			else {
	//				ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE";
	//			}

	//			TS_STOP("SAVE MEM 2"); //for TimeMeasurements only
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
		index_PresetSelected[guiIndex] = presetIndex;

		//-

		// callback
		// MODE A. it's important if this line is before or after ofSerialize
		ofLogVerbose(__FUNCTION__) << "DONE_load";
		DONE_load = true;

		// simple callback
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
	//			TS_START("LOAD FILE 1"); //for TimeMeasurements only
	//			std::string _path = getPresetPath(groups[guiIndex].getName(), presetIndex);
	//			bool b = ofxSurfingHelpers::loadGroup(groups[guiIndex], _path);
	//			if (b) ofLogNotice(__FUNCTION__) << "Load: " << groups[guiIndex].getName() << " " << _path;
	//			else ofLogError(__FUNCTION__) << "Error loading: " << groups[guiIndex].getName() << " " << _path;
	//			TS_STOP("LOAD FILE 1"); //for TimeMeasurements only
	//		}
	//		else
	//		{
	//			//MODE B: direct from memory
	//			TS_START("LOAD MEM 1"); //for TimeMeasurements only
	//			//using xml array
	//			if (presetIndex < mainGroupMemoryFilesPresets.size())
	//			{
	//				ofDeserialize(mainGroupMemoryFilesPresets[presetIndex], groups[guiIndex]);
	//			}
	//			else
	//			{
	//				ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE";
	//			}
	//			TS_STOP("LOAD MEM 1"); //for TimeMeasurements only
	//		}

	//		//-

	//		//mark selected
	//		index_PresetSelected[guiIndex] = presetIndex;

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
	//		index_PresetSelected[guiIndex] = presetIndex;
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
		index_PresetSelected[guiIndex] = presetIndex;

		//-

		// callback
		// MODE A. it's important if this line is before or after ofSerialize
		ofLogVerbose(__FUNCTION__) << "DONE_load";
		DONE_load = true;

		// simple callback
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
	//			TS_START("LOAD FILE 2"); //for TimeMeasurements only
	//			std::string strPath = getPresetPath(gName, presetIndex);
	//			ofXml settings;
	//			settings.load(strPath);
	//			ofDeserialize(settings, groups[guiIndex]);
	//			TS_STOP("LOAD FILE 2"); //for TimeMeasurements only
	//		}
	//		else
	//		{
	//			//MODE B: direct from memory
	//			TS_START("LOAD MEM 2""LOAD MEM 2"); //for TimeMeasurements only
	//			if (presetIndex < mainGroupMemoryFilesPresets.size()) {
	//				ofDeserialize(mainGroupMemoryFilesPresets[presetIndex], groups[guiIndex]);
	//			}
	//			else {
	//				ofLogError(__FUNCTION__) << "mainGroupMemoryFilesPresets OUT OF RANGE LOAD";
	//			}
	//			TS_STOP("LOAD MEM 2"); //for TimeMeasurements only
	//		}

	//		//-

	//		//mark selected
	//		index_PresetSelected[guiIndex] = presetIndex;

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
	//		index_PresetSelected[guiIndex] = presetIndex;
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
		/*if (groupIndex == -1) */groupIndex = groups.size() - 1; // set last group that is the group link selector

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
		return index_PresetSelected[guiIndex];
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
		return index_PresetSelected[guiIndex];
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

	_pathFolder = path_UserKit_Folder + "/" + path_PresetsFavorites + "/";

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

	_pathFolder = path_UserKit_Folder + "/" + path_PresetsFavorites + "/";

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
void ofxPresetsManager::keyPressed(ofKeyEventArgs& eventArgs)
{
	if (!bDoneSetup) return;

	{
		const int key = eventArgs.key;

		// modifiers
		bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
		bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
		bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
		bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

		//Debug
		if (0)
		{
			ofLogNotice(__FUNCTION__)
				<< "key: " << (char)key
				<< (mod_CONTROL ? " + CONTROL" : "")
				<< (mod_SHIFT ? " + SHIFT" : "")
				<< (mod_ALT ? " + ALT" : "")
				;
		}

		//--

		// Tweener
#ifdef INCLUDE_ofxSurfingTween
		//dataTween.keyPressed(key);
#endif

		//--

		// TODO: TEST: to force disable engine
		//bImGui_mouseOver = false;

		//// always listening to K to avoid can be blocked
		//// restore keys control
		//if (key == 'K' && !bImGui_mouseOver_PRE)
		//{
		//	bKeys = !bKeys;
		//}

		if (!guiManager.bKeys) return;

		//if (!bImGui_mouseOver_PRE)// disable keys when mouse over gui
		{
			//-

			// mode key for saving with mouse or trigger keys
			if (key == modeKeySave && !bModKeySave)
			{
				bModKeySave = true;
				ofLogNotice(__FUNCTION__) << "modKey Save TRUE";
				return;
			}

			// mode key for swap with mouse or trigger keys
			else if (key == modKeySwap && !bModKeySwap)
			{
				bModKeySwap = true;
				ofLogNotice(__FUNCTION__) << "modKey Swap TRUE";
				return;
			}

			//--

			//if (false) {}

			// hide/show control gui
			else if (key == 'G')
			{
				bGui = !bGui;
				return;
			}
			//else if (key == 'H')
			//{
			//	bHelp = !bHelp;
			//	return;
			//}
			else if (key == 'E')
			{
				bMODE_EditLive = !bMODE_EditLive;
				return;
			}
			else if (key == 'P')
			{
				setToggleVisible_PresetClicker();
				return;
			}

			//----

			// randomizers

			// play
			// timer to randomize and choice a random preset from all the available kit
			// using a probability and a shorter mode for some presets.
			else if ((mod_CONTROL && !mod_ALT) && key == ' ')
			{
				setTogglePlayRandomizerPreset(index_GroupSelected);
				return;
			}

			// randomize enable parameters (look into randomizers panel) to the current selected preset
			else if ((mod_CONTROL && !mod_ALT) && (key == 'R' || key == 18))
			{
				doRandomizePresetSelected(index_GroupSelected);

				// worfklow
				// store current point to undo history
#ifdef INCLUDE_ofxUndoSimple
				doStoreUndo();
#endif
				return;
			}

			//----

			// TODO: not working on windows.. We need to add int code
#ifdef INCLUDE_ofxUndoSimple
			if (bMODE_EditLive.get())
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
			if (bKeys_ArrowBrowse && (!mod_CONTROL && !mod_ALT))
			{
				// browse groups
				if (key == OF_KEY_UP)
				{
					index_GroupSelected--;
					return;
				}

				else if (key == OF_KEY_DOWN)
				{
					index_GroupSelected++;
					return;
				}

				else if (key == OF_KEY_LEFT)
				{
					bDISABLE_CALLBACKS = true;
					int sel = index_GroupSelected.get();
					int i = index_PresetSelected[sel];
					i--;
					bDISABLE_CALLBACKS = false;
					index_PresetSelected[sel] = (int)ofClamp(i, 0, index_PresetSelected[sel].getMax());
					return;
				}

				else if (key == OF_KEY_RIGHT)
				{
					bDISABLE_CALLBACKS = true;
					int sel = index_GroupSelected.get();
					int i = index_PresetSelected[sel];
					i++;
					bDISABLE_CALLBACKS = false;
					index_PresetSelected[sel] = (int)ofClamp(i, 0, index_PresetSelected[sel].getMax());
					return;
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

								if (g < index_PresetSelected.size()) index_PresetSelected[g] = k;
							}

							//--

							// workflow
							if (index_GroupSelected != g) index_GroupSelected = g;

							return;
						}
					}
				}
			}
		}

		//--

		//// randomizers
		//if (bMODE_EditLive.get())
		//{
		//groupRandomizers[index_GroupSelected].keyPressed(key);
		//}

		//--

		//// Workflow:
		//// workaround
		//if (key == 'R')
		//	doStoreUndo();
		//}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::keyReleased(ofKeyEventArgs& eventArgs)
{
	if (!bDoneSetup) return;
	if (!guiManager.bKeys) return;

	// mod keys
	if (eventArgs.key == modeKeySave && bModKeySave)
	{
		bModKeySave = false;
		ofLogNotice(__FUNCTION__) << "MODKEY SAVE FALSE";
	}

	else if (eventArgs.key == modKeySwap && bModKeySwap)
	{
		bModKeySwap = false;
		ofLogNotice(__FUNCTION__) << "MODKEY SWAP MODKEY SWAP FALSE";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::addKeysListeners()
{
	ofAddListener(ofEvents().keyPressed, this, &ofxPresetsManager::keyPressed);
	ofAddListener(ofEvents().keyReleased, this, &ofxPresetsManager::keyReleased);

	bKeys_NotYetActivated = false;
	guiManager.bKeys = true;
}

//--------------------------------------------------------------
void ofxPresetsManager::removeKeysListeners()
{
	ofRemoveListener(ofEvents().keyPressed, this, &ofxPresetsManager::keyPressed);
	ofRemoveListener(ofEvents().keyReleased, this, &ofxPresetsManager::keyReleased);
}

//----

// helpers 

//--------------------------------------------------------------
void ofxPresetsManager::doCloneRight(int gIndex)
{
	int pIndex = index_PresetSelected[gIndex];

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
		ofLogVerbose(__FUNCTION__) << "autosave preset: " << index_PresetSelected[gIndex];
		save(index_PresetSelected[gIndex], gIndex);
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

	////CheckAllFolders(); //? required

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
//	int ig = index_GroupSelected.get();
//	int ip = index_PresetSelected[ig];
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
	//CheckAllFolders(); //? required

	//// 2. erase
	//int ig = index_GroupSelected.get();
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
	index_PresetSelected[groupIndex] = _destIndex;
	loadPreset(_destIndex, groupIndex);

	// 3. restore TEMP (dst) to src
	_file.renameTo(_path_Src);
}

//--------------------------------------------------------------
void ofxPresetsManager::setToggleKeysControl(bool active)
{
	guiManager.bKeys = active;
}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_User(ofAbstractParameter& e)
{
	if (!bDISABLE_CALLBACKS)
	{
		std::string name = e.getName();

		//if ((name != "exclude") && (name != "%"))
		{
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		//if (0) {}

		//--

		// TODO: not working
		////custom path
		//else if (name == bPathDirCustom.getName())
		//{
		//	ofLogNotice(__FUNCTION__) << "bPathDirCustom: " << (bPathDirCustom.get() ? "TRUE" : "FALSE");

		//--

		//	if (bPathDirCustom.get())
		//	{
		//		if (pathDirCustom.get() != "DEFAULT_DATA_PATH") buildCustomUserKit();
		//		else bPathDirCustom = false; //force false
		//	}
		//	if (!bPathDirCustom.get())
		//	{
		//		buildDefaultUserKit();
		//	}
		//}

		//--

		// Players Randomizers

		if (name == bPLAY_Global.getName())
		{
			//TODO:
			// Workflow:
			if (bPLAY_Global) if (bMODE_EditLive) bMODE_EditLive = false;

			for (int i = 0; i < groups.size(); i++)
			{
				if (bAllowGroupSelector && i == groups.size() - 1) break; //dont play group link
				groupRandomizers[i].bPLAY_RandomizeTimer = bPLAY_Global;
			}
		}

		//--

		// Presets Selectors

		//TODO:
		// could be too slow??
		//if (!bDISABLE_CALLBACKS_Selectors)
		{
			for (int g = 0; g < groups.size(); g++) // iterate each group
			{
				for (int p = 0; p < groupsSizes[g]; p++) // iterate each preset on each group
				{
					if (name == index_PresetSelected[g].getName()
						&& index_PresetSelected[g].get() == p)
					{
						// some preset of any group changed

						ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " group: " << g << " preset: " << p;

						//-

						// 1. NOT CHANGED 
						// Selected Preset 

						if (index_PresetSelected[g] == index_PresetSelected_PRE[g])
						{
							ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " preset " << p << " NOT changed";

							// browser
							//if (MODE_StandalonePresets_NEW)
							{
								// retrig/reload
								//if (bAutoLoad)
								{
									load(index_PresetSelected[g], g);
								}
							}
							bMustTrig = true;
						}

						//--

						// 2. CHANGED 
						// Selected Preset 

						else if (index_PresetSelected[g] != index_PresetSelected_PRE[g])
						{
							ofLogNotice(__FUNCTION__) << "name: " << groups[g].getName() << " preset " << p << " changed";

							//--

							// 2.1. mod save controlled by modeKeySave

							if (bModKeySave)
							{
								ofLogNotice(__FUNCTION__) << "SAVE";

								if (bAutoSave) save(index_PresetSelected_PRE[g], g);

								save(index_PresetSelected[g], g);

								index_PresetSelected_PRE[g] = index_PresetSelected[g];
							}

							//--

							// 2.2. mod swap controlled by modKeySwap

							else if (bModKeySwap)
							{
								ofLogNotice(__FUNCTION__) << "SWAP";

								if (bAutoSave) save(index_PresetSelected_PRE[g], g);

								// save pre
								save(index_PresetSelected_PRE[g], g);

								// swap
								doSwap(g, index_PresetSelected_PRE[g], index_PresetSelected[g]); // group index, from, to

								index_PresetSelected_PRE[g] = index_PresetSelected[g];
							}

							//--

							// 3. no key commands

							else
							{
								// Autosave

								// Workflow:
								// browser mode bypasses autosave
								//if (bAutoSave && !MODE_StandalonePresets_NEW)
								if (bAutoSave)
								{
									// Workflow: 
									// Force disable autosave when randomizer player enabled
									// no matters edit mode
									bool b = groupRandomizers[index_GroupSelected.get()].bPLAY_RandomizeTimer.get();

									if (!b) save(index_PresetSelected_PRE[g], g);
								}

								//-

								// Remember this PRE state 
								// to know if changed or not on the next time..
								index_PresetSelected_PRE[g] = index_PresetSelected[g];

								//-

								// Load

								load(index_PresetSelected[g], g);
								//if (bAutoLoad) load(index_PresetSelected[g], g);
							}
						}
					}
				}
			}
		}
	}
}

////--------------------------------------------------------------
//void ofxPresetsManager::Changed_Randomizers(bool& b)
//{
//	//if (!bDISABLE_CALLBACKS && groupRandomizers.size() > 0)
//	//{
//	//	ofLogNotice(__FUNCTION__);
//
//	//	// Workflow:
//	//	if (bMODE_EditLive.get() && b) bMODE_EditLive = false;
//
//	//	//std::string name = b.getName();
//	//	//if (name == groupRandomizers[0].bPLAY_RandomizeTimer.getName())
//	//	//{
//	//	//}
//	//}
//}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Control(ofAbstractParameter& e)
{
	if (!bDISABLE_CALLBACKS)
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

		// Workflow:

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

		// Mode Edit
		// 
		//when false, we disabled autosave to allow faster performance ! 

		else if (name == bMODE_EditLive.getName())
		{
			ofLogNotice(__FUNCTION__) << "MODE EDITOR: " << (bMODE_EditLive.get() ? "TRUE" : "FALSE");

			bAutoSave = bMODE_EditLive.get();

			//--

			for (int i = 0; i < groups.size(); i++) {
				groupRandomizers[i].setModeEditor(bMODE_EditLive);
			}


#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER
			if (!bMODE_EditLive) bMODE_EditPresetClicker = false;
#endif

			//-

			// Workflow:
//#ifdef INCLUDE_ofxSurfingTween
//			dataTween.setLiveEditMode(bMODE_EditLive);
//#endif

		}

		//-

#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER

		if (name == bMODE_LockClicker.getName())
		{
			if (bMODE_LockClicker.get()) {
				bMODE_EditPresetClicker = false;
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

		if (name == bMODE_EditPresetClicker.getName())
		{
			if (bMODE_EditPresetClicker.get())
			{
				if (!bMODE_LockClicker)
					rectangle_PresetClicker.enableEdit();

				//// Workflow:
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

#endif

		//----

		//TODO:
		// memmory mode. not implemented yet..

		else if (name == loadToMemory.getName() && loadToMemory)
		{
			ofLogNotice(__FUNCTION__) << "loadToMemory:" << e;
			loadToMemory = false;
			load_AllKit_ToMemory();
		}
		else if (name == saveFromMemory.getName() && saveFromMemory)
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

		//// Workflow:
		//else if (name == bGui_PresetsParams.getName())
		//{
		//	if (bGui_PresetsParams) bGui_Selectors = false;
		//}

		//else if (name == bGui_Selectors.getName())
		//{
		//	if (bGui_Selectors) bGui_PresetsParams = false;
		//}

		else if (name == guiManager.bDebug.getName())
		{
			buildHelpInfo();
		}
		else if (name == guiManager.bKeys.getName())
		{
			buildHelpInfo();
		}

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
	bDISABLE_CALLBACKS = false;

	std::string path3 = path_UserKit_Folder + "/" + filenameMainSettings;
	//std::string path3 = filenameMainSettings;

	b = ofxSurfingHelpers::loadGroup(params_User, path3);

	//if (!b) ofLogError(__FUNCTION__) << "CANT LOAD FILE '" << path3 << "'!";
	//else ofLogNotice(__FUNCTION__) << "LOADED " << path3;

	//--

	// TODO: update selectors
	for (int i = 0; i < index_PresetSelected.size(); i++)
	{
		if (i < index_PresetSelected_PRE.size())
			index_PresetSelected_PRE[i] = index_PresetSelected[i];
		else ofLogError(__FUNCTION__) << "Out of Range: index_PresetSelected_PRE '" << i << "'!";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::save_ControlSettings()
{
#ifndef DEBUG_BLOCK_SAVE_SETTINGS

	bDISABLE_CALLBACKS = true;

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
		path = path_UserKit_Folder + "/" + filenameMainSettings;
		b = ofxSurfingHelpers::saveGroup(params_User, path);

		//--
	}
	//catch (int n)
	//{
	//	ofLogError(__FUNCTION__) << "CATCH ERROR " << n << endl;
	//	throw;
	//}

	//----

	bDISABLE_CALLBACKS = false;
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

	//filenameMainSettings = path_UserKit_Folder + "/ofxPresetsManager_Settings" + fileExtension; // default user preset name

	CheckFolder(path_UserKit_Folder);

	// create the folders of each group into main presets folder 
	std::string _path;
	if (bSplitGroupFolders.get())
	{
		for (int i = 0; i < groups.size(); i++)
		{
			_path = path_UserKit_Folder + "/" + path_PresetsFavorites; // current kit-presets presets folder
			_path += "/" + groups[i].getName(); // append group name

			CheckFolder(_path);
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::setPath_PresetsFavorites(std::string folder)
{
	ofLogNotice(__FUNCTION__) << folder;
	path_PresetsFavorites = folder;
	CheckFolder(path_UserKit_Folder + "/" + path_PresetsFavorites);
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

		_folder = path_UserKit_Folder + "/" + path_PresetsFavorites + "/";
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
		pathFolder = path_UserKit_Folder + "/" + path_PresetsFavorites + "/";
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

	bDISABLE_CALLBACKS = true;

	// destroy callbacks
	removeKeysListeners();
	ofRemoveListener(params_User.parameterChangedE(), this, &ofxPresetsManager::Changed_User);
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Control);

	//ofRemoveListener(params_Randomizer.parameterChangedE(), this, &ofxPresetsManager::Changed_Control);
	//ofRemoveListener(params_RandomizersFiltered.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Editor);

	// TODO: required?
	ofRemoveListener(ofEvents().update, this, &ofxPresetsManager::update);
	//ofRemoveListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	index_GroupSelected.removeListener(this, &ofxPresetsManager::Changed_Index_GroupSelected);

	//--

	// main group

	//-

	//// TODO: not implemented
	//// MODE B: direct from memory
	//if (MODE_MemoryLive && bAutoSave)
	//{
	//	saveAllKitFromMemory(); // update of files is required bc presets are on memmory only
	//}

	//--

	// autosave all group presets
	if (bAutoSave)
	{
		// save selected presets
		for (int g = 0; g < groups.size(); g++)// iterate each group
		{
			save(index_PresetSelected[g], g);
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

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL
	rectangle_PresetClicker.saveSettings(path_RectanglePresetClicker, path_UserKit_Folder + "/" + path_ControlSettings + "/", false);
#endif

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

	// daan fork
	ImGuiConfigFlags flags;
	flags = ImGuiConfigFlags_None;
	flags |= ImGuiConfigFlags_DockingEnable;
	//flags |= ImGuiConfigFlags_ViewportsEnable;
	//flags |= ImGuiDockNodeFlags_NoWindowMenuButton; //from imgui_internal.h
	//flags |= ImGuiDockNodeFlags_NoCloseButton; //?
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
	auto& io = ImGui::GetIO();
	io.ConfigDockingWithShift = true;
	io.ConfigWindowsResizeFromEdges = false; //?

	//--

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
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_End()
{
	ImGui::PopStyleVar();

	//--

	gui_ImGui.end();
}
#endif

//--------------------------------------------------------------
bool ofxPresetsManager::ImGui_Draw_Windows()
{
	// Windows:
	// main
	// parameters
	// players
	// standalone
	// randomizers

	//--

	//if (bGui_PanelsAll)
	{
		// Main Panel
		if (bGui_Main) draw_Gui_Main(); // main control + extra

		//--

		//// All group selectors to set current preset
		//if (bGui_AllSelectors) draw_Gui_Selectors();

		// All parameters from all groups

		if (bGui_Parameters)
		{

#ifndef USE_SPECIAL_WINDOWS
			string n; //window name

			if (bGui_Main)
			{
				n = bGui_Main.getName();
				guiManager.setNextWindowAfterWindowNamed(n);
			}
			else if (bGui_Clicker)
			{
				n = bGui_Clicker.getName();
				guiManager.setNextWindowAfterWindowNamed(n);
			}
#endif

			draw_Gui_Parameters();
		}

		//--

		if (bGui_Players)
		{

#ifndef USE_SPECIAL_WINDOWS
			string n; //window name

			if (bGui_Main)
			{
				if (!bGui_Parameters)
				{
					n = bGui_Main.getName();
					guiManager.setNextWindowAfterWindowNamed(n);
				}
				else
				{
					n = bGui_Parameters.getName();
					guiManager.setNextWindowAfterWindowNamed(n);
				}
			}
#endif

			draw_Gui_Players();
		}

		//--

		// Standalone presets browser

		if (bGui_Standalones) draw_Gui_Standalones();

		//--

#ifdef INCLUDE_ofxSurfingRandomizer
		dataRandomizer.drawImGuiWidgets();
#endif

	}

	return bLockMouseByImGui.get();
}

// ImGui
//--------------------------------------------------------------
void ofxPresetsManager::setupGuiStyles()
{
	guiManager.ClearStyles();

	guiManager.AddStyleGroup(params_Index_GroupsSelectors, OFX_IM_GROUP_HIDDEN_HEADER);
}

//--------------------------------------------------------------
void ofxPresetsManager::draw_Gui_Parameters()
{
	string n = "PRESETS PARAMETERS";

	IMGUI_SUGAR__WINDOWS_CONSTRAINTSW;

	if (guiManager.beginWindowSpecial(bGui_Parameters))
	{
		/*
		// group selector
		// (only when more than one group)

		if (groups.size() > 1) {
			ofxImGuiSurfing::AddStepper(index_GroupSelected);

			// name of selected group
			std::string str = index_PresetSelected[index_GroupSelected].getName();
			ImGui::Text(str.c_str());
		}

		// preset selector
		//ofxImGuiSurfing::AddStepper(index_PresetSelected[0]);
		*/

		//--

		ImGui::Spacing();

		// 1. Each group parameters

		for (int i = 0; i < groups.size(); i++)
		{
			bool bLast = false;
			if (bAllowGroupSelector) bLast = (i == groups.size() - 1);

			if (groups.size() > 1) {
				if (bLast)
				{
					//ImGui::Spacing();
					guiManager.AddSpacingBigSeparated();
					//ImGui::Spacing();
				}
				else
				{
					if (i != 0)
					{
						ImGui::Separator();
						ImGui::Spacing();
					}
				}
			}

			if (!bLast)
			{
				string s = "#" + ofToString(i);
				ImGui::Text(s.c_str());
			}

			// populate group widgets
			static ImGuiCond cond = ImGuiCond_Appearing;
			bool bopen = false;

			////TODO: fails hidding some headers..
			//if (i == index_GroupSelected) {
			//	bopen = true;
			//	cond = ImGuiCond_Once;
			//}
			//else cond = ImGuiCond_Once;

			guiManager.AddGroup(groups[i], bopen, cond);

			//--

			// Link group selector

			if (bLast)
			{
				//string s = ofToString(groups.back().getName());
				//ImGui::Text(s.c_str());

				ImGui::Spacing();
				guiManager.Add(index_PresetSelected.back());
			}

			if (bLast) {}
			//if (!bLast) ImGui::Spacing();
		}

		//--

		/*

		ImGui::Spacing();

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

		// Extra params added but not included into presets

		static ImGuiCond cond = ImGuiCond_Appearing;
		bool bopen = false;
		if (bAppStateParams) guiManager.AddGroup(params_AppSettings, bopen, cond);

		//--

		guiManager.endWindowSpecial();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::draw_Gui_ClickerPresets_ImGui()
{
	//IMGUI_SUGAR__WINDOWS_CONSTRAINTS_BIG;
	if (groups.size() > 1)
	{
		ImVec2 size_min = ImVec2(330, 300);
		ImVec2 size_max = ImVec2(450, 900);
		ImGui::SetNextWindowSizeConstraints(size_min, size_max);
	}
	else {
		ImVec2 size_min = ImVec2(330, 0);
		ImVec2 size_max = ImVec2(450, 900);
		ImGui::SetNextWindowSizeConstraints(size_min, size_max);
	}

	//--

	if (guiManager.beginWindowSpecial(bGui_Clicker))
	{
		guiManager.AddLabelBig("Presets");

		float _h = 2 * guiManager.getWidgetsHeight();
		//float _h = (guiManager.bMinimize ? 1.4f : 2) * guiManager.getWidgetsHeight();

		float _h2 = 0.7f * _h;
		float _w100 = ImGui::GetContentRegionAvail().x;
		float _w50 = guiManager.getWidgetsWidth(2);

		static ofParameter<bool> bResponsive{ "Responsive", true }; // not works

		guiManager.Add(guiManager.bMinimize, OFX_IM_TOGGLE_BUTTON_ROUNDED);
		guiManager.AddSpacing();

		//deprecated
		//guiManager.Add(bGui_PanelsAll, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);

		//TODO:
		guiManager.Add(bGui_Main, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);

		//--

		guiManager.AddSpacingBigSeparated();

		//--

		// Hide when main window is visible
		// to not duplicate controls on the two windows
		if (!bGui_Main)
		{
			// Edit / Live Modes

			ofxImGuiSurfing::AddBigToggleNamed(bMODE_EditLive, _w100, _h, "EDIT MODE", "LIVE MODE", true);
			if (bMODE_EditLive) guiManager.AddTooltip("Auto saves");
			else guiManager.AddTooltip("Requires manual save!");

			//--

			// Reload/Save

			if (!guiManager.bMinimize || !bMODE_EditLive)
			{
				// Save button 

				// Blink when Live Mode!
				ImGuiStyle* style = &ImGui::GetStyle();

				bool bBlink = !bMODE_EditLive;
				if (bBlink)
				{
					const ImVec4 c_ = style->Colors[ImGuiCol_TextDisabled];
					float blinkValue = ofxImGuiSurfing::getFadeBlink();
					float a = blinkValue;
					a = ofClamp(a, 0, 1);
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(c_.x, c_.y, c_.z, c_.w * a));
				}
				if (ImGui::Button("SAVE", ImVec2(_w50, _h2)))
				{
					savePreset(index_PresetSelected[index_GroupSelected], index_GroupSelected);
				}
				guiManager.AddTooltip("Save changes");

				if (bBlink)
				{
					ImGui::PopStyleColor();
				}

				ImGui::SameLine();

				//--

				// Load button

				if (ImGui::Button("LOAD", ImVec2(_w50, _h2)))
				{
					loadPreset(index_PresetSelected[index_GroupSelected], index_GroupSelected);
				}
				guiManager.AddTooltip("Load and discard changes");
			}

			guiManager.AddSpacingBigSeparated();
		}

		//--

		// One row per group. 
		// get and set the max.
		int maxSz1 = 3;
		int maxSz2 = 3;
		ImVec2 sz2;

		for (size_t i = 0; i < keys.size(); ++i)
		{
			int sz1 = keys[i].size();
			if (sz1 > maxSz1) maxSz1 = sz1;

			sz2 = ImGui::CalcTextSize(groups[i].getName().c_str());
		}

		// Iterate groups

		for (size_t i = 0; i < keys.size(); ++i)
		{
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, sz2.x + 40);
			//ImGui::SetColumnWidth(0, 103);

			bool b = groups.size() > 1 && guiManager.bKeys.get();

			bool bLast = (i == groups.size() - 1);

			if (!bAllowGroupSelector) bLast = false;
			// main merged group, no need to treat different of last group!
			// last is true if it's the groups merge selector,
			// named GOROUP_LINK by default!

			if (bLast && groups.size() > 1)
			{
				ImGui::Separator();
			}

			//--

			// Labels info

			std::string info;

			if (!guiManager.bMinimize) // not minimized
			{
				ImGui::Dummy(ImVec2(0, 2)); //offset

				if (!bLast) // not last
				{
					info = "  #" + ofToString(i);
					ImGui::Text(info.c_str());

					if (i == index_GroupSelected.get() && b) info = "> ";
					else if (i == index_GroupSelected.get() && !b) info = "- ";
					else info = "  ";
				}

				else // last. 
				{
					//guiManager.AddSpacing();
					if (groups.size() > 1)
					{
						if (i == index_GroupSelected.get() && b) info = "> ";
						else if (i == index_GroupSelected.get() && !b) info = "- ";
						else info = "  ";

						ImGui::Dummy(ImVec2(0, 3)); //offset
					}
				}

				info += groups[i].getName();
			}
			else // minimized. name only
			{
				if (!bLast) // not last
				{
				}
				else // last
				{
					//guiManager.AddSpacing();
				}

				ImGui::Dummy(ImVec2(0, 10)); //offset

				//info = ((index_GroupSelected.get() == i && b) ? "> " : "  ") + groups[i].getName();

				if (groups.size() > 1)
				{
					if (i == index_GroupSelected.get() && b) info = "> ";
					else if (i == index_GroupSelected.get() && !b) info = "- ";
					else info = "  ";
				}

				info += groups[i].getName();;
			}

			ImGui::Text(info.c_str());

			//--

			ImGui::NextColumn();

			//--

			// Matrix Clicker for each Group!

			ofxImGuiSurfing::AddMatrixClickerLabelsStrings(
				index_PresetSelected[i], labels[i],
				bResponsive.get(), maxSz1, true, -1, false);

			//--

			ImGui::Columns(1);
		}

		//--

		if (!guiManager.bMinimize)
		{
			guiManager.AddSpacingBig();

			if (groups.size() > 1)
			{
				guiManager.AddSpacingSeparated();
				guiManager.AddLabelBig("Players");

				//guiManager.Add(bPLAY_Global, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);

				// Blink by timer
				ofxImGuiSurfing::AddBigToggleNamed(bPLAY_Global, _w100, _h,
					"GLOBAL PLAYING", "GLOBAL PLAY ", true, groupRandomizers.back().getPlayerPct());
				guiManager.AddTooltip("Play all the group's players");

				guiManager.AddSpacingSeparated();
			}

			for (int i = 0; i < groupRandomizers.size(); i++)
			{
				if (i == groupRandomizers.size() - 1) {
					guiManager.AddSpacingSeparated();
				}

				ImGui::PushID(i);
				{
					string s = groups[i].getName();
					ImGui::Text(s.c_str());

					// Blink by timer
					ofxImGuiSurfing::AddBigToggleNamed(groupRandomizers[i].bPLAY_RandomizeTimer, _w100, 0.7f * _h,
						"PLAYING", "PLAY", true, groupRandomizers[i].getPlayerPct());
				}
				ImGui::PopID();
			}
		}

		if (!guiManager.bMinimize)
		{
			guiManager.AddSpacingBigSeparated();

			guiManager.Add(guiManager.bKeys, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			guiManager.AddTooltip("Enable key commands");

			guiManager.Add(guiManager.bAutoResize, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
			guiManager.AddTooltip("Disable to allow window resize \nbut could be constrained");
		}

		//--

		guiManager.endWindowSpecial();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::draw_Gui_Main()
{
	IMGUI_SUGAR__WINDOWS_CONSTRAINTSW;

	if (guiManager.beginWindowSpecial(bGui_Main))
	{
		float _w100;
		float _w50;
		float _w33;
		float _w25;
		float _h;
		guiManager.refreshWidgetsSizes(_w100, _w50, _w33, _w25, _h);

		guiManager.AddLabelBig("Presets");

		_h = (guiManager.bMinimize ? 1.4f : 2) * _h;
		//if (guiManager.bMinimize) _h *= 0.7f;

		std::string str;

		//---

		guiManager.Add(guiManager.bMinimize, OFX_IM_TOGGLE_BUTTON_ROUNDED);

		ImGui::Spacing();

		SurfingImGuiTypes s = OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM;

		guiManager.Add(bGui_Clicker, s);
		guiManager.Add(bGui_Parameters, s);
		guiManager.Add(bGui_Players, s);

		guiManager.AddSpacingSeparated();
		guiManager.AddSpacing();

		//--

		// 0. Edit Mode

		ofxImGuiSurfing::AddBigToggleNamed(bMODE_EditLive, _w100, 2 * _h, "EDIT MODE", "LIVE MODE", true);
		if (bMODE_EditLive) guiManager.AddTooltip("Auto saves");
		else guiManager.AddTooltip("Requires manual save!");

		//--

		// Reload/Save

		if (!bMODE_EditLive)
		{
			if (ImGui::Button("SAVE", ImVec2(_w50, _h)))
			{
				savePreset(index_PresetSelected[index_GroupSelected], index_GroupSelected);
			}
			guiManager.AddTooltip("Save changes");

			ImGui::SameLine();

			if (ImGui::Button("LOAD", ImVec2(_w50, _h)))
			{
				loadPreset(index_PresetSelected[index_GroupSelected], index_GroupSelected);
			}
			guiManager.AddTooltip("Load and discard changes");
		}

		guiManager.AddSpacingBigSeparated();

		//--

		// 0. Name Group

		str = "Group";
		guiManager.AddLabel(str.c_str(), false, true);

		str = index_PresetSelected[index_GroupSelected].getName();
		guiManager.AddLabelBig(str.c_str(), false, true);
		guiManager.AddSpacing();

		//--

		// 1. Selected Group

		if (groups.size() > 1)
		{
			//str = "Group";
			//ImGui::Text(str.c_str());

			//if (bBuildGroupSelector)
			{
				guiManager.Add(index_GroupSelected); // user selected which group to edit
			}

			//--

			// Next / Previous

			ImGui::PushID("##groupArrow");

			if (ImGui::Button("<", ImVec2(_w50, _h)))
			{
				index_GroupSelected--;
			}

			ImGui::SameLine();

			if (ImGui::Button(">", ImVec2(_w50, _h)))
			{
				index_GroupSelected++;
			}

			ImGui::PopID();

			guiManager.AddSpacingSeparated();
		}

		//-

		// 2. Selected Preset 

		guiManager.AddSpacing();
		guiManager.AddLabel("Preset", false, true);
		//ImGui::Text("Preset");

		// Char key
		int sel = index_GroupSelected.get();
		int i = index_PresetSelected[sel];
		std::string ss = labels[index_GroupSelected][i];
		//ss = " " + ss;

		guiManager.AddLabelBig(ss, false, true);
		guiManager.AddSpacing();

		// Index
		guiManager.Add(index_PresetSelected[index_GroupSelected]);

		//-

		// Next / Previous

		ImGui::PushButtonRepeat(true);
		{
			ImGui::PushID("##presetsArrow");

			if (ImGui::Button("<", ImVec2(_w50, _h)))
			{
				int ig = index_GroupSelected.get();
				load_Previous(ig, true);
			}

			ImGui::SameLine();

			if (ImGui::Button(">", ImVec2(_w50, _h)))
			{
				int ig = index_GroupSelected.get();
				load_Next(ig, true);
			}

			ImGui::PopID();
		}
		ImGui::PopButtonRepeat();

		//----

		// Extra Stuff

		//--

		// 2. Selectors
		/*
		{
			if (!guiManager.bMinimize)
			{
				guiManager.AddSpacingBigSeparated();

				std::string name;
				if (groups.size() <= 1) name = "SELECTOR";
				else name = "SELECTORS";
				if (ImGui::CollapsingHeader(name.c_str()))
				{
					// Presets Selectors
					guiManager.Indent();
					guiManager.AddGroup(params_Index_GroupsSelectors);
					guiManager.Unindent();
				}
			}
		}
		*/

		//--

		//TODO:
		// 3. Players

		if (!guiManager.bMinimize)
		{
			guiManager.AddSpacingBigSeparated();

			std::string name;
			if (groups.size() <= 1) name = "PLAYER";
			else name = "PLAYERS";

			if (ImGui::CollapsingHeader(name.c_str()))
			{
				guiManager.refreshWidgetsSizes(_w100, _w50, _w33, _w25, _h);

				//ofxImGuiSurfing::AddToggleRoundedButton(bGui_Players);
				//guiManager.Add(bGui_Players, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);

				for (int i = 0; i < groupRandomizers.size(); i++)
				{
					if (i == groupRandomizers.size() - 1) {
						guiManager.AddSpacingSeparated();
					}

					ImGui::PushID(i);
					{
						string s = groups[i].getName();
						ImGui::Text(s.c_str());

						// Blink by timer
						ofxImGuiSurfing::AddBigToggleNamed(groupRandomizers[i].bPLAY_RandomizeTimer, _w100, _h, "PLAYING", "PLAY", true, groupRandomizers[i].getPlayerPct());
					}
					ImGui::PopID();
				}

				//ImGui::Dummy(ImVec2(0.0f, 2.0f));

				//if (groups.size() > 1)
				//{
				//	if (ofxImGuiSurfing::AddToggleRoundedButton(bGui_ShowAllGroups))
				//	{
				//		// Workflow:
				//		if (!bGui_Players) bGui_Players = true;
				//	}
				//}
			}
		}

		//--

		// 4. Advanced

		if (!guiManager.bMinimize)
		{
			guiManager.AddSpacingSeparated();
			ImGui::Spacing();
			guiManager.Add(guiManager.bHelp, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
			if (guiManager.isHelpInternalEnable())
				guiManager.Add(guiManager.bHelpInternal, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
			guiManager.AddSpacingSeparated();

			guiManager.Add(bGui_AdvancedControl, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
		}

		ImGui::Spacing();

		//--

		// 5. Tools / Helpers

		if (bGui_AdvancedControl)
		{
			if (ImGui::CollapsingHeader("TOOLS"))
			{
				guiManager.refreshWidgetsSizes(_w100, _w50, _w33, _w25, _h);
				_h *= 2;

				//TODO: select which added group
				int i = 0;
				//int ii = index_PresetSelected[i];

				if (ImGui::Button("CLONE ALL", ImVec2(_w100, _h / 2)))
				{
					doCloneAll(i);
				}
				if (ImGui::Button("CLONE >", ImVec2(_w100, _h / 2)))
				{
					doCloneRight(i);
				}
			}
		}

		//--

		// 6. Undo Engine

#ifdef INCLUDE_ofxUndoSimple

		if (bMODE_EditLive.get())
		{
			if (ImGui::CollapsingHeader("UNDO ENGINE"))
			{
				guiManager.refreshWidgetsSizes(_w100, _w50, _w33, _w25, _h);
				_h *= 2;

				str = "History: " + ofToString(undoStringsParams[index_GroupSelected].getUndoLength()) + "/";
				str += ofToString(undoStringsParams[index_GroupSelected].getRedoLength());
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

		// 7. Advanced

		// Workflow: force hide when live mode
		if (bGui_AdvancedControl) draw_Gui_WidgetsAdvanced();

		//-

		// 8. Mouse over GUI
		//TODO: could be removed bc it's already implemented into ofxSurinfgImGui
		{
			static bool bAutoLocKeysOnMouseHoverGui = false;
			bLockMouseByImGui = false;
			if (bAutoLocKeysOnMouseHoverGui)
			{
				bLockMouseByImGui = bLockMouseByImGui | ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
				bLockMouseByImGui = bLockMouseByImGui | ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
				bLockMouseByImGui = bLockMouseByImGui | ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
				// must be insisde some window
			}
		}

		//--

		// 9. Info Labels

		if (bGui_AdvancedControl)
		{
			if (!guiManager.bMinimize)
			{
				guiManager.AddSpacingSeparated();
				guiManager.AddSpacingBig();

				// label for User-Kit folder
				str = "USER-KIT";
				//ImGui::Text(str.c_str());
				guiManager.AddLabelBig(str, true, true);

				str = path_UserKit_Folder;
				ofStringReplace(str, "/", "/\n"); //split in lines to reduce panel width
				str += "/";
				//ImGui::Text(str.c_str());
				guiManager.AddLabel(str, false, true);
			}

			//--

			if (!guiManager.bMinimize)
			{
				ImGui::Dummy(ImVec2(0, 2));

				// aliases
				int ig = index_GroupSelected.get();
				int ip = index_PresetSelected[ig];

				// name of selected group
				str = index_PresetSelected[ig].getName();
				ImGui::Text(str.c_str());

				if (groups.size() > 1) {
					str = "Group #" + ofToString(ig) + "/" + ofToString(groups.size() - 1);
					ImGui::Text(str.c_str());
				}

				str = "Preset " + ofToString(ip) + "/" + ofToString(keys[ig].size() - 1);
				ImGui::Text(str.c_str());
			}
		}

		//--

		guiManager.endWindowSpecial();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::draw_Gui_WidgetsAdvanced()
{
	//if (ImGui::CollapsingHeader("EXTRA"))
	{
		ImGuiTreeNodeFlags flagsTree;
		flagsTree = ImGuiTreeNodeFlags_Framed;

		if (ImGui::TreeNodeEx("INTERNAL", flagsTree))
		{
			float _w100;
			float _w50;
			float _w33;
			float _w25;
			float _h;
			ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

			//--

			ofxImGuiSurfing::AddParameter(guiManager.bKeys);
			ofxImGuiSurfing::AddParameter(bAutoSave);
			ofxImGuiSurfing::AddParameter(bAutoLoad);

			// TODO: 
			//faster memory mode vs xml files not implemented yet
			//ofxImGuiSurfing::AddParameter(MODE_MemoryLive); //ImGui::SameLine();

#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER
			if (ImGui::TreeNodeEx("PRESET CLICKER", flagsTree))
			{
				ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

				ofxImGuiSurfing::AddBigToggle(bThemeDarkOrLight, _w100, _h);

				if (bMODE_EditLive)
				{
					ofxImGuiSurfing::AddToggleRoundedButton(bMODE_LockClicker);
					if (!bMODE_LockClicker) {
						ofxImGuiSurfing::AddBigToggle(bMODE_EditPresetClicker, _w100, _h);
					}
					ofxImGuiSurfing::AddBigToggle(bGui_BgEditPresetClicker, _w100, _h);

					ImGui::PushItemWidth(_w33);
					ofxImGuiSurfing::AddStepper(cellSize);
					ImGui::PopItemWidth();

					//if (bHelp)ofxImGuiSurfing::AddParameter(helpPos);
				}

				ImGui::TreePop();
			}
#endif
			//--

			//if (bMODE_EditLive)
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

					// monitor custom state
					//ofxImGuiSurfing::AddParameter(bPathDirCustom);

#ifdef USE_PRESETS_MANAGER__IMGUI_LAYOUT 
					if (guiManager.bExtra) guiManager.drawAdvanced();
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

	bool _bDebug = guiManager.bDebug;
	//bool _bDebug = false;

	helpInfo = "";
	helpInfo += "HELP \n";
	helpInfo += "\n";
	helpInfo += "PRESETS MANAGER \n";

	if (_bDebug)
	{
		//helpInfo += "  ";
		helpInfo += "USER-KIT \n";
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

		helpInfo += "\n\n";
	}

		helpInfo += "KEY COMMANDS \n";
		helpInfo += "\n";

	if (guiManager.bKeys) {
		helpInfo += "H               HELP \n";
		helpInfo += "G               GUI \n";
		helpInfo += "E               EDIT \n";
		helpInfo += "\n";

		helpInfo += "P               CLICKER \n";
		helpInfo += "\n";
		helpInfo += "                LOAD \n";
		helpInfo += "+Ctrl           SAVE/COPY \n";
		helpInfo += "+Alt            SWAP \n";
		helpInfo += "Arrows          EXPLORE \n";
		helpInfo += "left-right      PRESET \n";
		if (groups.size() > 1) helpInfo += "up-down         GROUP \n";
		helpInfo += "\n";

		helpInfo += "Ctrl + SPACE    PLAY \n";
		//helpInfo += "Ctrl + R        PRESET RANDOM \n";
		helpInfo += "\n";

		// undo engine
#ifdef INCLUDE_ofxUndoSimple
		helpInfo += "Ctrl + Z          UNDO\n";
		helpInfo += " + Shift          REDO\n";
		helpInfo += "Ctrl + C          CLEAR\n";
		helpInfo += "Ctrl + s          STORE\n";
#endif
	}
	else {
		helpInfo += "Keys toggle is disabled! \n";
		helpInfo += "Enable it on CLICKER Window \n";
	}

	helpTextBoxWidget.setText(helpInfo);
}

//--------------------------------------------------------------
void ofxPresetsManager::buildDefaultUserKit() {
	setPath_UserKit_Folder(path_UserKit_Folder); // overwrites the path name but also creates folders if required

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
	std::string temp = R"(\)"; //use '\' as splitter...should use '/' too bc Windows/macOS compatibility..
	auto ss = ofSplitString(path_UserKit_Folder, temp);
	displayNameUserKit = ss[ss.size() - 1];

	//--

	// build help info
	buildHelpInfo();
}

//--------------------------------------------------------------
void ofxPresetsManager::buildCustomUserKit() {

	setPath_UserKit_Folder(pathDirCustom); // creates subfolders for each group

	//-

	// reduce to name only
	std::string str = path_UserKit_Folder;
	std::string temp = R"(\)"; // use '\' as splitter...should use '/' too bc Windows/macOS compatibility..
	auto ss = ofSplitString(path_UserKit_Folder, temp);
	displayNameUserKit = ss[ss.size() - 1]; // get last word and use as name
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
			save(index_PresetSelected[g], g);
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

	bDISABLE_CALLBACKS = true; //TODO:
	bPathDirCustom = true;
	bDISABLE_CALLBACKS = false;

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
		index_PresetSelected[_last] = 0;
		load(index_PresetSelected[_last], _last);
		bAutoSave = bPre;

		//// load all group selected presets
		//for (int g = 0; g < groups.size(); g++)//iterate each group
		//{
		//	load(index_PresetSelected[g], g);
		//}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::draw_Gui_Standalones()
{
	if (guiManager.beginWindowSpecial(bGui_Standalones))
	{
		float _w100;
		float _w50;
		float _w33;
		float _w25;
		float _h;
		ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

		//--

		int groupIndex = index_GroupSelected.get();
		int _numfiles = standaloneFileNames[groupIndex].size();

		std::string _name;
		std::string str;

		//---

		ImGui::Spacing();

		// label for User-Kit folder
		str = "USER-KIT:";
		ImGui::Text(str.c_str());
		str = path_UserKit_Folder;
		ofStringReplace(str, "/", "/\n"); // split in lines to reduce panel width
		str += "/";
		ImGui::Text(str.c_str());

		ImGui::Spacing();

		//--

		// user selected which group to edit
		ImGui::PushItemWidth(_w50);
		if (bBuildGroupSelector) ofxImGuiSurfing::AddParameter(index_GroupSelected);
		ImGui::PopItemWidth();

		//--

		// name of selected group
		str = "Group Name:";
		ImGui::Text(str.c_str());
		str = index_PresetSelected[index_GroupSelected].getName();
		ImGui::Text(str.c_str());

		ImGui::Spacing();

		//--

		// Helpers and browser
		{
			//---

			ImGui::Spacing();

			//// label presets folder
			////str = "User-Kit";
			////ImGui::Text(str.c_str());
			//str = "/" + path_PresetsStandalone + "/" + groups[groupIndex].getName(); // append group name
			//ImGui::Text(str.c_str());

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
			//	//if (MODE_StandalonePresets_NEW) save(index_PresetSelected[groupIndex], groupIndex);
			//	
			//	// save current to favourites
			//	save(index_PresetSelected[groupIndex], groupIndex);
			//}

			ImGui::Spacing();

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
				_path += index_PresetSelected[groupIndex].getName(); // group name
				ofStringReplace(_path, "/", "/\n"); //split in lines to reduce panel width
				const char* array = _path.c_str();

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

				// 1.1 previous file

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

				ImGui::Spacing();

				if (ImGui::Button("UPDATE", ImVec2(_w50, _h)))
				{
					//_name = inputText_TEMP;
					//ofLogNotice(__FUNCTION__) << "UPDATE PRESET NAME: " << _name << endl;

					// 0. get filename of selected
					std::string _currName = standaloneFiles[groupIndex][standaloneFileIndex[groupIndex]].getBaseName();
					ofLogNotice(__FUNCTION__) << "UPDATE. filename: " << _currName;

					// 1. delete old file
					standaloneFiles[groupIndex][standaloneFileIndex[groupIndex]].remove();

					// 2. save "overwrite"
					doStandalonePresetSave(_currName, groupIndex);

					// Workflow:

					// 3. refresh files
					doStandalonePresetsRefresh(groupIndex);;

					// 4. reselect last save preset (bc directory sorting changes)
					ofLogNotice(__FUNCTION__) << "Reload last updated preset:";
					int _newIndex = -1; // search index for filename
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

					// Workflow:

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
				if (ImGui::Button("CLEAR", ImVec2(_w50, _h))) // delete all files
				{
					ofLogNotice(__FUNCTION__) << "CLEAR Presets folder: " << path_UserKit_Folder + "/" + path_PresetsStandalone;

					for (int i = 0; i < standaloneFiles[groupIndex].size(); i++) {
						ofLogWarning(__FUNCTION__) << "DELETE file: " << standaloneFiles[groupIndex][i].getAbsolutePath();

						// 1. delete file
						bool b = standaloneFiles[groupIndex][i].remove();
						if (!b) ofLogError(__FUNCTION__) << "Can not DELETE file: " << standaloneFiles[groupIndex][i].getAbsolutePath();
					}

					// Workflow:

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

				// Workflow:

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

					// Workflow:

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
				ofxImGuiSurfing::AddParameter(guiManager.bKeys);
			}
		}

		//ImGui::Checkbox("Auto-Resize", &auto_resize);

		guiManager.endWindowSpecial();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::draw_Gui_Players()
{
	// This panel will show the settings for the selected by user group (index_GroupSelected)

	//--

	if (!bGui_Players) { return; }

	if (index_GroupSelected > groupRandomizers.size() - 1) { return; }

	//-

	// Show randomizers of user selected group

	// A. All the groups

	//if (bGui_ShowAllGroups)
	//{
	//	for (int i = 0; i < groupRandomizers.size(); i++)
	//	{
	//		if (bGui_Players)
	//		{
	//			//groupRandomizers[i].drawImGui();

	//			groupRandomizers[index_GroupSelected.get()].drawImGui_Main();
	//			groupRandomizers[index_GroupSelected.get()].drawImGui_Editor();
	//		}
	//	}
	//}

	// B. Only the selected group

	//else
	{
		if (bGui_Players)
		{
			//groupRandomizers[index_GroupSelected.get()].drawImGui();

			groupRandomizers[index_GroupSelected.get()].drawImGui_Main();

			// Editor
			{
				if (groupRandomizers[index_GroupSelected.get()].getEditorWindowVisible())
				{
					string n; //window name
					n = groupRandomizers[index_GroupSelected.get()].getNameWindowMain();
					guiManager.setNextWindowAfterWindowNamed(n);
				}
			}

			groupRandomizers[index_GroupSelected.get()].drawImGui_Editor();
		}
	}
}

//----

// Standalone presets browser engine

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
	_path = path_UserKit_Folder + "/" + path_PresetsStandalone; // '/archive'
	_path += "/" + groups[groupIndex].getName(); // append group name
	//_path += "/"; // the folder

	CheckFolder(_path);

	ofLogNotice(__FUNCTION__) << "Path: " << _path;
	ofDirectory _dataDir(ofToDataPath(_path, true));

	//-

	if (groupIndex < groups.size())
	{
		// Clear files and filenames vectors
		standaloneFiles[groupIndex].clear();
		standaloneFileNames[groupIndex].clear();

		// Load all folder files in one call
		int _size = _dataDir.getFiles().size(); // amount of (standalone presets) files for this group

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

	// Workflow:
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

	return !bFilesError; //true if ok
}

//--------------------------------------------------------------
void ofxPresetsManager::doStandalonePresetsBuild(int groupIndex)// standalone presets splitted from favourites presets
{
	// load files structure directory
	bool bLoaded = doStandalonePresetsRefresh(groupIndex);

	// Workflow:
	if (bLoaded)
	{
		// load first preset
		if (standaloneFileNames[groupIndex].size() > 0)
		{
			standaloneFileIndex[groupIndex] = 0; // select first file
			standaloneNamePresetDisplay[groupIndex] = standaloneFileNames[groupIndex][0];

			//// Workflow:
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
	_path += "/" + groups[groupIndex].getName(); // append group name
	_path += "/" + name; // append preset name
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
	_path += "/" + groups[groupIndex].getName(); // append group name
	_path += "/" + name; // append preset name
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
	_path += "/" + groups[groupIndex].getName(); // append group name
	//_path += "/"; // the folder

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
		_file.copyFromTo(pathSrc, pathDst, true, true); // relative, overwrite
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
			_path = path_UserKit_Folder + "/" + path_PresetsFavorites; // current kit-presets presets folder
			_path += "/" + groups[i].getName(); // append group name
			_path += "/"; // the folder

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
	int i = index_GroupSelected.get();
	//for (int i = 0; i < groups.size(); i++)// all together
	{
		undoXmlsParams[i].clear();

		ofParameterGroup _group = groups[i];
		ofSerialize(undoXmlsParams[i], _group); // fill the xml with the ofParameterGroup

		undoStringsParams[i] = (undoXmlsParams[i].toString()); // fill the ofxUndoSimple with the xml as string
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
	ofLogNotice(__FUNCTION__) << "UNDO CLEAR : " << index_GroupSelected;
	undoStringsParams[index_GroupSelected].clear();
	//undoStringParams.clear();
}

//--------------------------------------------------------------
void ofxPresetsManager::doUndo() {
	ofLogNotice(__FUNCTION__) << "UNDO < Group #" << index_GroupSelected << " " << groups[index_GroupSelected].getName();
	undoStringsParams[index_GroupSelected].undo();
	//undoStringParams.undo();
	doRefreshUndoParams();
}

//--------------------------------------------------------------
void ofxPresetsManager::doRedo() {
	ofLogNotice(__FUNCTION__) << "REDO < Group #" << index_GroupSelected << " " << groups[index_GroupSelected].getName();
	undoStringsParams[index_GroupSelected].redo();
	//undoStringParams.redo();
	doRefreshUndoParams();
}

//--------------------------------------------------------------
void ofxPresetsManager::doRefreshUndoParams() {
	int i = index_GroupSelected.get();
	//for (int i = 0; i < groups.size(); i++)
	{
		undoXmlsParams[i].clear();
		undoXmlsParams[i].parse(undoStringsParams[i]); // fill the xml with the string 

		ofParameterGroup _group = groups[i];
		ofDeserialize(undoXmlsParams[i], _group); // load the ofParameterGroup

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
void ofxPresetsManager::Changed_Index_GroupSelected(int& index)
{
	index_GroupSelected = (int)ofClamp(index_GroupSelected.get(), 0, index_GroupSelected.getMax());
	ofLogWarning(__FUNCTION__) << index_GroupSelected;
}

//--------------------------------------------------------------
void ofxPresetsManager::draw_Help()
{
#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL
	helpTextBoxWidget.setTheme(bThemeDarkOrLight);
#endif

	helpTextBoxWidget.draw();
}

//--------------------------------------------------------------
void ofxPresetsManager::addExtra(ofParameterGroup& g)
{
	bool bAppStateParams = true;

	params_AppSettings.clear();
	//params_AppSettings.add(params_Internal);
	params_AppSettings = g;
}

//----

#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER

//--------------------------------------------------------------
void ofxPresetsManager::doRectFit()
{
	// calculate preset clicker box, depending of amout groups and subscribed keys/preset for each group

	float _ww;
	float _hh = cellSize * groups.size();
	_hh += 2 * _RectClick_Pad; //pads

	int kmax = 0;
	for (int i = 0; i < keys.size(); i++)
	{
		int sz = keys[i].size();
		if (sz > kmax) kmax = sz;
	}
	kmax += 2; //save+gui button
	_ww = kmax * cellSize;
	_ww += getGroupNamesWidth(); //name
	_ww += 2 * _RectClick_Pad; //pads

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

//--

//--------------------------------------------------------------
void ofxPresetsManager::draw_Gui_ClickerPresets_Native()
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
	bool bLateralPosition = false; // false = on top of clicker
	bool bLeftPosition = true; // left or right. only if lateral pos true

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
			//if (bMODE_EditPresetClicker)
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
				if (!bMODE_LockClicker)
				{
					// double click swap edit mode
					if (doubleClicker.isMouseDoubleClick()) {
						bMODE_EditPresetClicker = !bMODE_EditPresetClicker;
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

				if (index_PresetSelected[i] == k) // it is selected
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
						cellSize * k + 8, cellSize * i + 18);
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
					ofDrawBitmapString(_label, cellSize * k + 8, cellSize * i + 18);
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
				if (bGui_PanelsAll)
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

			if (bShow_GroupName)
			{
				std::string info = groups[i].getName();

				// mark selected group. useful to navigate with arrow keys

				bool bLast = (i == groups.size() - 1);
				if (!bLast) {
					info += "\n";
					if (i == index_GroupSelected.get() && groups.size() > 1 && guiManager.bKeys.get())
						info += ">";
					info += "#" + ofToString(i);
				}
				else
				{
					if (i == index_GroupSelected.get() && groups.size() > 1 && guiManager.bKeys.get())
						info = ">" + info;
				}

				//--

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

//--

//-----------------------------------------------------
void ofxPresetsManager::mousePressed(int x, int y)
{
	if (bDoneSetup)
	{
		//bMouseOverGui = false;

		// this method will get what box button is pressed: in which group-row (y) and which preset (x)

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

			//--

			if ((xIndex >= 0) && (xIndex < groupsSizes[yIndex]))
			{
				//--

				// 1. mod save controlled by modeKeySave

				if (bModKeySave)
				{
					ofLogNotice(__FUNCTION__) << "SAVE";

					save(xIndex, yIndex);

					// will auto load and set the already clicked preset button
					if (yIndex < index_PresetSelected.size())
					{
						index_PresetSelected[yIndex] = xIndex;
					}
				}

				//--

				// 2. mod swap controlled by modKeySwap

				else if (bModKeySwap)
				{
					ofLogNotice(__FUNCTION__) << "SWAP";

					if (yIndex < index_PresetSelected.size())// a valid group index
					{
						//// autosave first from same clicked group
						//if (bAutoSave)
						//{
						//	ofLogNotice(__FUNCTION__) << "Autosave first";
						//	save(index_PresetSelected[yIndex], yIndex);
						//	//save(index_PresetSelected_PRE[yIndex], yIndex);
						//}

						// swap
						doSwap(yIndex, index_PresetSelected[yIndex], xIndex); // group index, from, to
					}
				}

				//--

				// 3. no mod keys: 

				// normal load (not any key modifier pressed)
				else
				{
					ofLogNotice(__FUNCTION__) << "LOAD";

					if (yIndex < index_PresetSelected.size())
						index_PresetSelected[yIndex] = xIndex;
				}
			}

			//--

			// 2. Save 

			// Last Button 
			else if (xIndex == groupsSizes[yIndex])
			{
				//TODO: hide save buttons if bAutoSave 
				//if (!bAutoSave)
				{
					ofLogNotice(__FUNCTION__) << "saveButton group:" << yIndex;

					save(index_PresetSelected[yIndex], yIndex);
				}
			}
		}

		//--

		// 3. Toggle Show Gui

		// (on main group only)
		if (groups.size() > 0)
		{
			// TODO: hide save button on autosave mode...
			//int _offset = (bAutoSave ? 0 : 1);
			int _offset = 1;

			int _i;
			if (bBuildGroupSelector) _i = groups.size() - 1;
			else if (groups.size() > 1) _i = 1;
			else _i = 0;

			if ((yIndex == groups.size() - 1) && (xIndex == groupsSizes[_i] + _offset))
			{
				bGui_PanelsAll = !bGui_PanelsAll;

				ofLogNotice(__FUNCTION__) << "bGui_PanelsAll : " << (bGui_PanelsAll ? "TRUE" : "FALSE");
			}
		}
	}
}

//----

////--------------------------------------------------------------
//void ofxPresetsManager::removeMouseListeners()
//{
//	//ofRemoveListener(ofEvents().mousePressed, this, &ofxPresetsManager::mousePressed);
//}

#endif