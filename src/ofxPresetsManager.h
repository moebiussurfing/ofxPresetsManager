
/// ofxPresetsManager.h 
///
/// ofxPresetsManager 
/// by moebiussurfing, 2020.
///
/// this addon is based in the original ofxGuiPresetSelector addon 
/// by Nicola Pisanti, MIT License, 2016
/// https://github.com/npisanti/ofxGuiPresetSelector
/// all modifications and new features by moebiussurfing
/// my idea is to allow use ofParameterGroup's as managed content instead of ofxPanel

//---
//
//	TODO:
//
///	++++		add randomizer engine to user main group selector
///					select wich group is the target to randomizer/main group extras
///	++			add big toggle to edit/live modes: enables undo, autosave, memory mode..etc
///	++++		open/save dialog to project session single file.
///					or allowed to all the groups?
///	+			add draggable rectangle to move/resize clicker
//
///	++		randomize editor preset
///				make for multigroup
///				preset mini engine. ABC dropdown list for randomizers
///				could add also several randomizer settings presets. selectable by dropdown list..
///				add limit range min/max to randomize
///				do nesting toggles to improve view. create a group for related toggles..	
///				clone using editor toggles to avoid clone disabled toggle params
///				mode state to allow overwrite only enabled toggle params
///	+++		add engine to create all preset files if it's a new empty project
///				add setter to enable randomize wich params
///				call populate. disable debug_display red info
//
///	+		change file format from xml to json
///	++		add ofColor to ImGui helpers vs ofFloatColor
///	++		lock (by toggle) params that we want to ignore on changing presets
///				can be done enabling/disabling serializable for each param with a group of toggles
///	++		performance: 
///				check memory_mode. memory mode to extra groups too
///				check workflow: when playing randomizer disable autosave
///				check disable autosave
///	+		repair autosave timer. exclude log
///	+		add workflow to not collide manual preset click with randomizer timer
///	+		add define to disable all browser/ImGui/randomize stuff to make addon minimal expression 
///				or add simpler class but compatible with preset files kits
///	+		could make tween when changing params using lerp or ofxKeyTween...
///	+		add undo to all the groups
//
//---


#pragma once

//--------------------------------------
//
//	DEFINES
//
//#define MODE_ImGui_EXTERNAL			// this must be defined here and (but not only) in ofApp (too)!!
//										// will handle the gui outisde the addon, here the parameters only!
#define INCLUDE_IMGUI_CUSTOM_FONT		// customize ImGui font
#define INCLUDE_ofxUndoSimple			// undo engine to store after randomize preset parameters (& recall)
//#define USE_ofxImGuiSimple			// TEST with an alternative ImGui addon
//#define USE_JSON						// file settings format
//										   
//	DEBUG									   
//#define INCLUDE_PERFORMANCE_MEASURES	// measure performance ofxTimeMeasurements
//#define DEBUG_randomTest				// uncomment to debug randimzer. comment to normal use. if enabled, random engine stops working
//#define DEBUG_BLOCK_SAVE_SETTINGS		// disable save settings//enable this bc sometimes there's crashes on exit
//
//--------------------------------------


#include "ofMain.h"

//--

#define INCLUDE_RANDOMIZER

//--

// gui
#ifdef USE_ofxImGuiSimple//alternative to official addon
#include "ofxImGuiSimple.h"
#include "Helpers.h"
#else
#include "ofxImGui.h"
#endif

// undo engine
#ifdef INCLUDE_ofxUndoSimple
#include "ofxUndoSimple.h"
#endif

#include "ofxSurfingHelpers.h"

// optional to debug performance or delay when loading files or presets on hd or memory modes
#ifdef INCLUDE_PERFORMANCE_MEASURES
#include "ofxTimeMeasurements.h"
#else
#define TS_START
#define TS_STOP
#define TSGL_START
#define TSGL_STOP
#endif

//-------------------------------

#pragma mark - DEFINE_DATA_TYPES

#define NUM_MAX_GROUPS 10

//---

//TODO: removable ofBaseApp? or it's mandatory for auto update()?
class ofxPresetsManager : public ofBaseApp
{
	//--

public:
	ofxPresetsManager();
	~ofxPresetsManager();

	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void exit();
	void windowResized(int w, int h);

	//--

#pragma mark - API

	//----
	//
	// API
	//
	//----

	//--

public:
	void add(ofParameterGroup params, initializer_list<int> keysList);// adds and define keys to trig presets too

private:
	// TODO: BUG:
	// not working. must use below add method. maybe can add a correlative keys list
	void add(ofParameterGroup params, int numPresets = 8);// add a param group for preset saving and how many presets on favs

	//--

public:
	void setup();// must be called after params group has been added!
	void setup(std::string name);// TODO: should use char array to avoid collapse with bool...
	void setup(std::string name, bool _buildGroupSelector);
	void setup(bool _buildGroupSelector);

	void startup();// must be called after setup to set initial states well

private:
	void drawPresetClicker();// user clickeable box panel preset selector
	
	//--

	// core engine
private:

	// save to a preset
	void save(int presetIndex, int guiIndex = 0);
	void save(int presetIndex, string guiName);

	// loads an already saved preset
	void load(int presetIndex, int guiIndex = 0);
	void load(int presetIndex, string guiName);

	// get the last loaded preset
	int getPresetIndex(int guiIndex) const;
	int getPresetIndex(string guiName) const;

	int getGuiIndex(string name) const;// get index for a name of group
	string getPresetPath(string guiName, int presetIndex);// get path of a preset of a group

	string getGroupPath(string guiName);// get path of a group. for external monitor only
	string getGroupPath(int index);// get path of a group. for external monitor only

	//--

private:
	// main settings/parameters container
	// all parameters are contained into an ofParameterGroup
	std::vector<ofParameterGroup> groups;// container to store multiple groups. 
	std::vector<int> groupsSizes;// this is the number of presets of each added group

	//--

	// preset selectors to each added group
	std::vector<ofParameter<int>> PRESETS_Selected_Index;
	std::vector<int> PRESETS_Selected_Index_PRE;// remember previous selector
	ofParameterGroup params_PRESETS_Selected{ "Preset Selectors" };// group all selectors

	//--
	
	ofParameter<int> GROUP_Selected_Index;// global selector. this selector will control all linked groups

	//group main selector
	bool bBuildGroupSelector = true;// to allow auto build a group selector to combine all the added groups to the presets manager
	bool bAllowGroupSelector = true;// to allow disable main group. not allways we need it..

public:
	void setEnableMainGroupSelector(bool b) {// disable the use of main group selector. must call before setup. enabled by default
		bAllowGroupSelector = b;
	}

private:
	ofParameter<bool> bSplitGroupFolders{ "MODE SPLIT FOLDERS", true };//on this mode we split every group on his own sub folder

	//--

	// callbacks
	// callbacks to get when 'saved/loaded done' happens from ofApp if desired
public:
	ofParameter<bool> DONE_load;// easy callback to know (in ofApp) that preset LOAD is done 
	ofParameter<bool> DONE_save;// easy callback to know (in ofApp) that preset SAVE is done

	//--

	// easy callback to faster ofApp integration 
public:
	//--------------------------------------------------------------
	bool isDoneLoad()
	{
		if (bIsDoneLoad)
		{
			bIsDoneLoad = false;
			return true;
		}
		return false;
	}
	//// to check in update() as callback
	//bool isDoneLoad()
	//{
	//    bool isDone = false;
	//    if (DONE_load)
	//    {
	//        DONE_load = false;
	//        isDone = true;
	//    }
	//    else
	//    {
	//        isDone = false;
	//    }
	//    return isDone;
	//}

private:
	bool bIsDoneLoad = false;

	//----

	// layout helpers

public:
	//--------------------------------------------------------------
	float getPresetClicker_Width()// get width of bigger group of presets row
	{
		float _maxPresets = 0;
		for (int i = 0; i < groups.size(); i++) {
			if (getAmountPresetsOfGroup(getAmountGroups() - 1) > _maxPresets) _maxPresets = getAmountPresetsOfGroup(getAmountGroups() - 1);
		}
		int _extraButtons = 2;
		float w = cellSize * (_maxPresets + _extraButtons);

		return w;
	}
	//--------------------------------------------------------------
	float getPresetClicker_BoxSize()
	{
		return cellSize;
	}
	//--------------------------------------------------------------
	void setSizeBox_PresetClicker(float size)
	{
		cellSize = size;
	}
	//--------------------------------------------------------------
	float getPresetClicker_Height()
	{
		return getAmountGroups() * cellSize;
	}
	////--------------------------------------------------------------
	//void setPosition_DEBUG(int x, int y);// where to display if we get errors (ie: data files not found) on startup

	//-

	//--------------------------------------------------------------
	void setModeKeySave(int key);// set the key you have to hold for saving, default is OF_KEY_CONTROL
	void setModeKeySwap(int key);// set the key you have to hold for swap, default is OF_KEY_ALT

	//--------------------------------------------------------------
	void setEnableKeys(bool active)
	{
		if (ENABLE_Keys != active) {// avoid if state not need to change
			ENABLE_Keys = active;
		}
		if (bKeys != active) {// avoid if state not need to change
			bKeys = active;
		}
	}
	//--------------------------------------------------------------
	void setToggleEnableKeys()
	{
		ENABLE_Keys = !ENABLE_Keys;
		bKeys = ENABLE_Keys;
	}
	//--------------------------------------------------------------
	bool isKeysEnabled()
	{
		return ENABLE_Keys;
	}

	//--

	// undo engine
private:
#ifdef INCLUDE_ofxUndoSimple
	ofxUndoSimple<std::string> undoStringParams;
	ofXml undoXmlParams;
	void undoRefreshParams();
	void undoStoreParams();
#endif

	//--

	// settings paths

	// all folder names must go without '/'
	//std::string path_Root;
	std::string path_UserKit_Folder;// User-Kit folder for both other subfolders. 
	std::string path_PresetsFavourites;// path for kit of favourite presets. live kit
	std::string path_PresetsStandalone;// path for browse other presets. archive kit
	std::string path_ControlSettings;// path for app state session settings

	std::string filename_ControlSettings;
	std::string filename_Randomizers;// path for randomizers settings
	std::string filenamesPrefix;// to add to file names to split names with index

	std::string filenameMainSettings;// file for main settings
	std::string fileExtension;// xml or json

	std::string displayNameUserKit;
	std::string displayNamePreset;

	//--

	// app settings for many params
private:
	void load_ControlSettings();
	void save_ControlSettings();

	//--

	// memory mode

	// improve performance loading from xml objects at memory nor xml files from hd
private:
	ofParameter<bool> MODE_MemoryLive;// when enabled all presets are handled from a memory vector to avoid lag of loading xml files from hd
	ofParameter<bool> loadToMemory;
	ofParameter<bool> saveFromMemory;

	void load_AllKit_ToMemory();
	void save_AllKit_FromMemory();
	
	//--

private:
	// TODO:
	// should use keys[i].size() instead of this:
	vector<ofXml> mainGroupPresetsXmls;
public:
	void setModeMemoryPerformance(bool b) {
		MODE_MemoryLive = b;
	}

	//----

	// randomizer

	ofParameterGroup params_RandomizerSettings{ "Randomizers" };
public:
	ofParameter<bool> PLAY_RandomizeTimer; //play randomizer
	ofParameter<bool> bRandomizeIndex;
private:
	ofParameter<bool> MODE_DicesProbs;
	ofParameter<bool> MODE_LatchTrig; // this mode trigs the preset but goes back to preset 0 after duration timer
	ofParameter<bool> MODE_AvoidRandomRepeat; // this mode re makes randomize again if new index preset it's the same!
	ofParameter<bool> bResetDices;
	ofParameter<int> randomizedDice; // to test
	bool bLatchRun = false;

public:
	void setModeRandomizeAvoidRepeat(bool b) {
		MODE_AvoidRandomRepeat = b;
	}
	ofParameter<int> randomizeDuration;
	ofParameter<int> randomizeDurationShort;
	ofParameter<float> randomizeDurationBpm;
	//ofParameter<float> randomizeSpeedF;// speed scaler. not used

private:
	int randomizeSpeed;// real time duration
	uint32_t randomizerTimer;
	int randomize_MAX_DURATION = 6000;
	int randomize_MAX_DURATION_SHORT = 6000 / 2.f;
	vector<ofParameter<int>> presetsRandomFactor;// probability of every preset
	vector<ofParameter<bool>> presetsRandomModeShort;// mode short for ebvery preset
	vector<int> randomFactorsDices;
	void buildRandomizers();
	void setupRandomizer();// engine to get a random between all posible dices (from 0 to dicesTotalAmount) and then select the preset associated to the resulting dice.
	void doRandomizeWichSelectedPreset();// randomize wich preset (usually 1 to 8) is selected (not the params of the preset)
	int doRandomizeWichSelectedPresetCheckChanged();
	void doResetDices();// reset all probs to 0
	int dicesTotalAmount;// total dices summing the prob of any preset probability (PROB1 + PROB2 + ...)

	int timerRandomizer;

	//--

public:
	ofParameter<bool> bRandomizeEditor;
private:
	ofParameter<bool> bRandomizeEditorAll;// put all toggles/params to true. a randomize will act over all params
	ofParameter<bool> bRandomizeEditorNone;// put to disabled all toggles
	ofParameter<bool> bRandomizeEditorPopulateFavs;// create all presets

	// system to select what params of current selected preset to: clone, randomize etc
	void setupRandomizerEditor();
	vector<ofParameter<bool>> editorPresets;
	ofParameterGroup params_Editor;
	ofParameterGroup params_Editor_Toggles;
	void addGroupToEditor(ofParameterGroup& group);// queue all contained params inside the paramGroup and nested too
	void Changed_Params_Editor(ofAbstractParameter &e);
	void doRandomizeEditor();// randomize params of current selected preset
	void doRandomizeEditorGroup(ofParameterGroup& group);// randomize params of current selected preset

	//----

private:
	bool DISABLE_CALLBACKS_SELECTORS = false;//to avoid multiple calls on multiple presets selector engine
	bool DISABLE_CALLBACKS = true;//to avoid startup crashes and objects are not initialized properly
	//updating some params before save will trigs also the group callbacks
	//so we disable this callbacks just in case params updatings are required
	//in this case we will need to update gui position param

	//--

public:

	//----
	//
	// API
	//
	//----

	//--------------------------------------------------------------
	void load_Next()// for main group
	{
		PRESET_Selected_IndexMain++;
		if (PRESET_Selected_IndexMain >= mainGroupAmtPresetsFav - 1)
		{
			PRESET_Selected_IndexMain = mainGroupAmtPresetsFav - 1;
		}
	}
	//--------------------------------------------------------------
	void load_Previous()// for main group
	{
		PRESET_Selected_IndexMain--;
		if (PRESET_Selected_IndexMain < 0)
		{
			PRESET_Selected_IndexMain = 0;
		}
	}
	//--------------------------------------------------------------
	void loadPreset(int p);// load preset for the main group by code from ofApp
	void loadPreset(int p, int _indexGroup);// load preset for extra groups by code from ofApp
	void loadPresetGroup(int presetIndex)// load preset for main group by code from ofApp
	{
		int groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex << " preset: " << presetIndex;
		load(presetIndex, groupIndex);
	}
	void savePreset(int p, int _indexGroup);// save preset for extra groups by code from ofApp

	//--------------------------------------------------------------
	int getCurrentPreset()// get index of selected preset
	{
		return PRESET_Selected_IndexMain;
	}
	//--------------------------------------------------------------
	int getCurrentPreset(int _group)// get index of selected preset on the group
	{
		int _presetIndex = -1;
		if (_group < groups.size()) _presetIndex = PRESETS_Selected_Index[_group];
		return _presetIndex;
	}

	//--------------------------------------------------------------
	int getAmoutPresetsMain()// get main group amount of presets
	{
		return mainGroupAmtPresetsFav;
	}

	// helpers main for external layout or other workflow purposes
	//--------------------------------------------------------------
	int getAmountGroups()
	{
		if (groups.size() > 0) return groups.size();
		else return 1;// workaround to set default height before groups are added
	}

	//--------------------------------------------------------------
	int getAmountPresetsOfGroup(int g)
	{
		int _size = -1;
		if (g < groups.size()) _size = groupsSizes[g];
		return _size;
	}

	//--------------------------------------------------------------
	string getGroupName(int i)
	{
		string _name = "ERROR UNKNOWN";
		if (i < groups.size()) _name = groups[i];
		return _name;
	}

	//--------------------------------------------------------------
	string getGroupsNames()
	{
		string _names = "";
		for (int i = 0; i < groups.size(); i++)
		{
			_names += groups[i].getName();
			_names += "\n";
		}
		return _names;
	}

	//--------------------------------------------------------------
	string getGroupsPaths(bool simplified = true)
	{
		string _names = "";

		if (!simplified) {
			for (int i = 0; i < groups.size(); i++)
			{
				_names += getGroupPath(i);
				_names += "\n";
			}
		}
		else {
			//simplify
			if (!bPathDirCustom) _names += "/data/";

			_names += path_UserKit_Folder + "/" + path_PresetsFavourites;
			_names += "\n";
			for (int i = 0; i < groups.size(); i++)
			{
				_names += ofToString(i) + " /";
				_names += groups[i].getName();
				_names += "/";
				_names += "\n";
			}
		}
		return _names;
	}

	//--------------------------------------------------------------
	string getGroupsPath()
	{
		string _names = "";
		_names = getGroupPath(0);// simpler: get first group bc all are in the same folder
		return _names;
	}

	//-

	//--------------------------------------------------------------
	void setVisible_GroupName(bool b)// disabler for minimal design
	{
		SHOW_GroupName = b;
	}

	//--

	// randomizer helpers
public:
	ofParameter<int> randomizerProgress{ "%", 0, 0, 100 };
	float _prog;

	//--------------------------------------------------------------
	void setPlayRandomizerTimer(bool b)// play randomizer timer
	{
		PLAY_RandomizeTimer = b;
		if (b) doRandomizeWichSelectedPreset();
	}
	//--------------------------------------------------------------
	void setTogglePlayRandomizerPreset()// toggle randomizer timer
	{
		PLAY_RandomizeTimer = !PLAY_RandomizeTimer;
	}
	//--------------------------------------------------------------
	void setRandomizerDuration(float t)
	{
		randomizeDuration = t;
		randomizeDurationBpm = 60000.f / randomizeDuration;
	}
	//--------------------------------------------------------------
	void setRandomizerDurationShort(float t)
	{
		randomizeDurationShort = t;
	}
	//--------------------------------------------------------------
	void setRandomizerBpm(float bpm)
	{
		randomizeDurationBpm = bpm;
		// 60,000 ms (1 minute) / Tempo (BPM) = Delay Time in ms for quarter-note beats
		randomizeDuration = 60000.f / bpm;
		randomizeDurationShort = randomizeDuration / 2.f;
	}
	//--------------------------------------------------------------
	void doRandomizePresetFromFavs()// trig randomize and select one of the favs presets
	{
		bRandomizeIndex = true;
	}
	//--------------------------------------------------------------
	void doRandomizePresetSelected() {// randomize params of current selected preset
		doRandomizeEditor();
	}

	//--

	// helper tools
	void doCloneRight(int pIndex);// clone from selected preset to all others to the right
	void doCloneAll();// clone all presets from the current selected
	void doPopulateFavs();// fast populate random presets around all favs
	void doSwap(int groupIndex, int fromIndex, int toIndex);

	//--

private:
	//draws group name into clicker boxes panel
	bool SHOW_GroupName = true;

	//--

	// asy callback
	// used when preset has not changed but we like to retrig
	bool bMustTrig = false;
public:
	//--------------------------------------------------------------
	bool isMustTrig()// trig on select preset or not. this is useful when preset selected not changed, but we want to retrig current preset settings
	{
		if (bMustTrig)
		{
			bMustTrig = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	//--

	// browser for standalone presets
private:
	// load presets from preset folder, not from favorites presets folders
	void doLoadMainGroupPreset(string name);
	void doStandalonePresetSave(string name);
	bool doStandaloneRefreshPresets();
	void buildStandalonePresets();//standalone presets splitted from favourites presets
	//void doCheckPresetsFolderIsEmpty();

	//-

	// API

public:
	// BUG: 
	// workflow 
	// to solve auto load fail because the sorting of xml autoSave after preset selector tag
	// //(optional) on startup: called at the end of your ofApp setup() 
	//--------------------------------------------------------------
	void refreshStartup()
	{
		// //browser
		//doStandaloneRefreshPresets();

		//-

		ofLogNotice(__FUNCTION__);
		PRESET_Selected_IndexMain_PRE = -1;

		PRESET_Selected_IndexMain = PRESET_Selected_IndexMain;//just for refresh callback
		ofLogNotice(__FUNCTION__) << "Preset " << PRESET_Selected_IndexMain;
	}

	//--------------------------------------------------------------
	void setVisible_Help(bool b)
	{
		debugClicker = b;
	}
	//--------------------------------------------------------------
	void setPosition_GUI_ImGui(int x, int y)
	{
		ImGui_Position = ofVec2f(x, y);
	}
	//--------------------------------------------------------------
	void setVisible_GUI_ImGui(bool b)
	{
		SHOW_ImGui = b;
	}
	//--------------------------------------------------------------
	void toggleVisible_GUI_ImGui()
	{
		SHOW_ImGui = !SHOW_ImGui;
	}
	//--------------------------------------------------------------
	bool getVisible_GUI_ImGui()
	{
		return SHOW_ImGui;
	}
	//--------------------------------------------------------------
	void setVisible_GUI_Internal(bool visible)
	{
		SHOW_Gui_AdvancedControl = visible;
	}
	//--------------------------------------------------------------
	bool isVisible_GUI_Internal()
	{
		return SHOW_Gui_AdvancedControl;
	}
	//--------------------------------------------------------------
	void setToggleVisible_GUI_Internal()
	{
		SHOW_Gui_AdvancedControl = !SHOW_Gui_AdvancedControl;
	}
	//--------------------------------------------------------------
	void setPosition_PresetClicker(int x, int y, int _cellSize)
	{
		clicker_Pos.x = x;
		clicker_Pos.y = y;
		cellSize = _cellSize;
	}
	//--------------------------------------------------------------
	void setPosition_PresetClicker(int x, int y)
	{
		clicker_Pos.x = x;
		clicker_Pos.y = y;
	}
	//--------------------------------------------------------------
	void setVisible_PresetClicker(bool visible)
	{
		SHOW_ClickPanel = visible;
	}
	//--------------------------------------------------------------
	void toggleVisible_PresetClicker()
	{
		SHOW_ClickPanel = !SHOW_ClickPanel;
	}
	//--------------------------------------------------------------
	bool getVisible_PresetClicker()
	{
		return SHOW_ClickPanel;
	}
	//--------------------------------------------------------------
	bool isVisible_PresetClicker()
	{
		return SHOW_ClickPanel;
	}

	//--

#pragma mark - SETTINGS

	//--------------------------------------------------------------
	void setPath_UserKit_Folder(string folder);// path for root container folder. must be called before setup()!
	void setPath_PresetsFavourites(string folder);// path folder for favourite/live presets
	void setPath_PresetsStandalone(string folder);// path folder for standalone presets kit for the browser
	//--------------------------------------------------------------
	void setPath_ControlSettings(string str)// for the session states settings
	{
		ofLogNotice(__FUNCTION__) << str;
		path_ControlSettings = str;
	}
	////--------------------------------------------------------------
	//void setPath_Root(string str)
	//{
	//	ofLogNotice(__FUNCTION__) << str;
	//	path_Root = str;
	//}

	//--------------------------------------------------------------
	void setModeAutoLoad(bool b)
	{
		autoLoad = b;
	}
	//--------------------------------------------------------------
	void setModeAutoSave(bool b)
	{
		autoSave = b;
	}
	//--------------------------------------------------------------
	bool getModeAutoSave()
	{
		return autoSave;
	}
	//--------------------------------------------------------------
	void setModeAutoSaveTimer(bool b)
	{
		bAutosaveTimer = b;
	}

	//--

public:
	ofParameter<int> PRESET_Selected_IndexMain;// main group preset selector (current)
	//vector<ofParameter<int>> PRESETS_Selected_Index;// extra groups preset selector (current)

private:
	int PRESET_Selected_IndexMain_PRE = -1;// used as callback

	//--

public:
	// switch on or off the control with the keys
	void setToggleKeysControl(bool active);

	//--

public:
	//--------------------------------------------------------------
	void saveCurrentPreset() {
		ofLogNotice(__FUNCTION__) << "SAVE PRESET: " << PRESET_Selected_IndexMain.get();
		save(PRESET_Selected_IndexMain, 0);
	}

	//----

	// ImGui

public:
	void ImGui_Draw_WindowContent(ofxImGui::Settings &settings);
	void ImGui_Draw_MainPanel(ofxImGui::Settings &settings);
	void ImGui_Draw_Basic(ofxImGui::Settings &settings);
	void ImGui_Draw_Selectors(ofxImGui::Settings &settings);
	void ImGui_Draw_Browser(ofxImGui::Settings &settings);
	void ImGui_Draw_GroupRandomizers(ofxImGui::Settings &settings);
	void ImGui_Draw_PresetPreview(ofxImGui::Settings &settings);

private:
	ofParameter<bool> SHOW_ImGui_PresetsParams;
	ofParameter<bool> SHOW_ImGui_Selectors;
	ofParameter<bool> SHOW_Help;
	ofParameter<bool> MODE_Browser_NewPreset;
	ofParameter<bool> SHOW_Gui_AdvancedControl;
	ofParameter<bool> ENABLE_Keys;
	ofParameter<bool> MODE_Editor{ "MODE EDIT", true };// this mode improves performance disabling autosave, undo history..etc
	ofParameter<bool> bThemDark{ "THEME DARK", false };
	ofParameter<glm::vec2> Gui_Internal_Position;
	
private:
	string helpInfo;
	void buildHelpInfo();

	bool debugClicker = true;

	//--

private:
#ifdef USE_ofxImGuiSimple
	ofxImGuiSimple gui_ImGui;
#else

#ifndef MODE_ImGui_EXTERNAL
	ofxImGui::Gui gui_ImGui;
	void ImGui_Setup();
	void ImGui_Draw_WindowBegin();
	void ImGui_Draw_WindowEnd();
	bool ImGui_Draw_Window();
#endif

	ofParameter<glm::vec2> ImGui_Position;//ImGui browser panel position. 
	ofParameter<glm::vec2> ImGui_Size;//ImGui browser panel position. 

	//--

private:
	// mouse over gui handler
	ofParameter<bool> bImGui_mouseOver;
	bool bImGui_mouseOver_PRE;
	bool bMouseOver_Changed = false;
	//public:
	////--------------------------------------------------------------
	//bool isMouseOver()
	//{
	//	bool b;
	//	if (!SHOW_ImGui) b = false;
	//	else b = bImGui_mouseOver;
	//	return b;
	//}
	////--------------------------------------------------------------
	//bool isMouseOver_Changed()
	//{
	//	if (bMouseOver_Changed)
	//	{
	//		//bMouseOver_Changed = false;//reset
	//		return true;
	//	}
	//}

	//--

public:
	ofParameterGroup params_randomizer;

	//--

	// '/archive' folder for standalone presets browser 

	//bool MODE_newPreset = false;
	string textInput_New = "";//user input text

	// files
	std::vector<std::string> fileNames;
	std::vector<ofFile> files;
	int currentFile = 0;
	string textInput_temp = "";
	bool bFilesError = false;

	//-------

	// helpers

public:
	//--------------------------------------------------------------
	void doGetFavsToFilesBrowser()// save all favorites presets to the browser (archive) folder
	{
		ofLogNotice(__FUNCTION__);

		//browser path
		string browser_path;
		browser_path = path_UserKit_Folder + "/" + path_PresetsStandalone;

		//browser number of files
		//iterate all presets
		for (int i = 0; i < mainGroupAmtPresetsFav; i++)
		{
			std::string pathSrc;
			std::string pathDst;

			pathSrc = getPresetPath(groups[0].getName(), i);
			boost::filesystem::path bPath(pathSrc);

			//string pathFolder = ofToString(bPath.parent_path());
			string fileName = ofToString(bPath.filename().generic_string());
			pathDst = browser_path + "/" + fileName;

			ofLogNotice(__FUNCTION__) << "pathSrc: " << pathSrc;
			ofLogNotice(__FUNCTION__) << "pathDst: " << pathDst;

			ofFile file;
			file.copyFromTo(pathSrc, pathDst, true, true);//relative, overwrite

		}

		//--

		// refresh files
		doStandaloneRefreshPresets();
	}

#endif// end browser

	//----

private:

	// layout
	ofVec2f guiPos_InternalControl = ofVec2f(500, 500);
	int cellSize = 80;
	ofVec2f clicker_Pos;

	//--

	// helpers to easy integrate into external gui's

private:
	// expose basic controls to allow use on external gui
	ofParameterGroup params_Controls{ "PRESETS MANAGER" };

public:
	//--------------------------------------------------------------
	ofParameterGroup getParamsControls() {// this are usefull parameters to use in our projects/addons gui's
		params_Controls.clear();
		//cout << "displayNameUserKit: " << displayNameUserKit << endl;// TODO: not refreshing well on startup..
		//params_Controls.setName(displayNameUserKit);
		//params_Controls.setName("OVERLAY");
		params_Controls.add(getParamsPresetSelectors());
		params_Controls.add(getParamsRandomizers());
		params_Controls.add(SHOW_ClickPanel);
		params_Controls.add(MODE_Editor);
		return params_Controls;
	}
	//--------------------------------------------------------------
	ofParameterGroup getParamsPresetSelectors() {
		return params_PRESETS_Selected;
	}
	//--------------------------------------------------------------
	ofParameterGroup getParamsRandomizers() {
		ofParameterGroup _g{ "RANDOMIZERS" };
		_g.add(PLAY_RandomizeTimer);
		_g.add(randomizeDuration);
		_g.add(randomizeDurationShort);
		_g.add(randomizerProgress);
		return _g;
	}

	//--

public:
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);

private:
	void addKeysListeners();
	void removeKeysListeners();

	vector<vector<int>> keys;//queued trigger keys for each group ? (all presets) (size of)
	bool bKeys;//enabled keys
	bool keysNotActivated;

	//save
	int modeKeySave;//save mod key
	bool bKeySave;//save mod key state

	//swap selected preset with the currently clicked (i.e: 4 -> 7  &  4 <- 7)
	int modKeySwap;//swap mod key 
	bool bKeySwap;//swap mod key state

	void mousePressed(int x, int y);
	bool lastMouseButtonState;

	int mainGroupAmtPresetsFav;//amount of box-clickable handled presets on current favorites/kit [8]

	//---

private:
	//font to label clicker boxes
	ofTrueTypeFont myFont;
	string myTTF;//gui font for all gui theme
	int sizeTTF;

	//-

	//callback
private:
	void Changed_Params_Control(ofAbstractParameter &e);
	void Changed_Params_UserKit(ofAbstractParameter &e);

	//-

public:
	ofParameter<bool> SHOW_ClickPanel;//to allow include as toggle parameter into external gui

private:
	ofParameterGroup params_Control;

public:
	ofParameter<bool> SHOW_ImGui;

private:
	ofParameter<bool> bSave;
	//ofParameter<bool> bLoad;

	ofParameter<bool> autoSave;
	ofParameter<bool> autoLoad;

	ofParameter<bool> bCloneRight;
	ofParameter<bool> bCloneAll;

	// internal groups
	ofParameterGroup params_UserKitSettings;
	ofParameterGroup params_Gui;
	ofParameterGroup params_Options;
	ofParameterGroup params_HelperTools;
	ofParameterGroup params_Randomizer;
	ofParameterGroup params_Custom;

	//--

	// custom path
	ofParameter<bool> bPathDirCustom;
	ofParameter<string> pathDirCustom;

public:
	// set custom path
	void doFileDialogProcessSelection(ofFileDialogResult openFileResult);
	void buildCustomUserKit();
	void buildDefaultUserKit();

	//--

private:
	// timer autosave
	ofParameter<bool> bAutosaveTimer;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 9000;

	//--

	bool ENABLE_KeysArrowBrowse = true;// allow browse by arrows keys by default

public:
	//--------------------------------------------------------------
	void setEnableKeysArrowBrowse(bool b)
	{
		ENABLE_KeysArrowBrowse = b;
	}

	//--

private:
	// check if a folder path exist and creates one if not
	// many times when you try to save a file, this is not possible and do not happens bc the container folder do not exist
	//--------------------------------------------------------------
	void CheckFolder(string _path)
	{
		ofxSurfingHelpers::CheckFolder(_path);
	}

	//--------------------------------------------------------------
	void CheckAllFolders()// check that folders exist and create them if not
	{
		// TODO: 
		// use a container for all User-Kit together...
		// CheckFolder(path_Root);

		string _path;

		_path = path_UserKit_Folder;// current kit-preset main folder
		CheckFolder(_path);

		_path = path_UserKit_Folder + "/" + path_PresetsFavourites;// current kit-presets favourite presets folder
		CheckFolder(_path);

		_path = path_UserKit_Folder + "/" + path_PresetsStandalone;// current kit-presets standalone presets folder
		CheckFolder(_path);

		_path = path_UserKit_Folder + "/" + path_ControlSettings;// for randomizer settings (into his own kit-preset folder)
		CheckFolder(_path);

		_path = path_UserKit_Folder + "/" + path_ControlSettings;// app settings (shared from all kit-presets)
		CheckFolder(_path);

		// create the folders of each group into main presets folder 
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
};
