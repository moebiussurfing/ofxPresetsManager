
/*

	ofxPresetsManager.h

	ofxPresetsManager
	by moebiusSurfing, 2019 - 2022.

	this add-on is based and inspired on the original ofxGuiPresetSelector addon
	by Nicola Pisanti, MIT License, 2016
	https://github.com/npisanti/ofxGuiPresetSelector

	added modifications and several new features.
	my idea is to allow use ofParameterGroup's as managed content instead of ofxGui's panels.

*/

//----

/*

	TODO:

	+	fix random params and reset params tools (parameters window).

	+	add clicker to name labels like arrows browsing, on clicker window.
	+	add re trig when re click without index change.

	+	new edit mode:
		mark a param and when modifying current preset, save to all the others and overwrite
	+	lock (by toggle) params that we want to ignore on changing presets
		can be done enabling/disabling serializable for each param with a group of toggles.

	+	super-lite add-on player version with combo list only. maybe without any GUI at all.

	+	small flick on clicker window when minimized and changed to edit


	IDEAS:

	+	open/save dialog to project User-Kit session in a single file.
		or allowed to all the groups?

	+	add populator engine to create all preset files if it's a new empty project
		add settler to enable some params to randomize
		call populate. disable debug_display red info
		-> This is already done on ofxSurfingPresets..

	+	performance:
		restore-back memory_mode.
		(use xml objects into memory vs hd files) to extra groups too

	+	fix auto save timer. exclude log.


	BUGS:

	-	Play state is not recalled on settings ?
	-	When disabled G_LINK must fix some vector ranges..
		(presetsManager.setDisableGroupLinkSelector();)
		// protect to vector range!
		if (index_GroupSelected >= groupRandomizers.size() - 1)
			index_GroupSelected = groupRandomizers.size() - 1;

*/

//----


#pragma once

#include "ofMain.h"


//-----------------------------------------------------------------------------------------------------------------
//
//	DEFINES

//--

//	1. SURFING ENGINES

// GLOBAL BYPASS
// -> You can comment this line only 
// and will force disable too the below surfing engines directives.
//#define INCLUDE_SURFING_ENGINES 

//	Tween/Smooth transitions between presets
//	We can enable only one of the two! 
// (Tween or Smooth)
//	1.2 TWEEN (Option 1)
#define INCLUDE_ofxSurfingTween 

//	1.1 SMOOTH (Option 2)
//#define INCLUDE_ofxSurfingSmooth 

//	1.3 RANDOMIZER
#define INCLUDE_ofxSurfingRandomizer

//-

#ifndef INCLUDE_SURFING_ENGINES
#undef INCLUDE_ofxSurfingTween 
#undef INCLUDE_ofxSurfingSmooth 
#undef INCLUDE_ofxSurfingRandomizer
#endif

//----

//	2. EXTRA FEATURES

#define INCLUDE_ofxUndoSimple	
// undo engine to store after randomize a preset or manually (to browse history states)

//#define USE_JSON	
// set the file settings format (xml or json). 
// already defined into ofxSurfingHelpers

#define NUM_MAX_GROUPS 10
//TODO: should be better to push to a vector...
// but maybe hard coded sizes helps a bit on the performance.

//----

// 3. CUSTOMIZATION

#define USE_PRESETS_MANAGER__IMGUI_LAYOUT 
// -> Instantiated and using my add-on ofxSurfingImGui

//#define USE_PRESETS_MANAGER__IMGUI_INTERNAL
// -> Legacy ImGui without my ofxSurfingImGui add-on. Deprecated

//-

//#define USE_IMGUI_EXTERNAL 
// this is to group all ImGui panels into one unique instance in ofApp
// currently there's a bug when using more than one single ofxImGui instance!
// this line is proposed as debugging when adding this feature (multi instance).

#define USE_SPECIAL_WINDOWS // probably can't be disabled

//--

//#define USE__PRESETS_MANAGER__NATIVE_CLICKER 
// -> Legacy clicker. oF Native. 
// Now we use the ImGui toggles matrix!

//--

//	4. DEBUG

//#define DEBUG_PERFORMANCE_MEASURES	
// measure performance ofxTimeMeasurements. not using now. must restore!

//#define DEBUG_randomTest						
// uncomment to debug randomizer. 
// comment to normal use. if enabled, random engine stops working

//#define DEBUG_BLOCK_SAVE_SETTINGS				
// disable save settings//enable this bc sometimes there's crashes on exit...


//-----------------------------------------------------------------------------------------------------------------


//--

#include "ofxImGui.h"
#include "imgui_internal.h"
// some imgui helpers

//--

// EXTRA SURFING ENGINES

#ifdef INCLUDE_ofxSurfingSmooth
#include "ofxSurfingSmooth.h"
#endif

#ifdef INCLUDE_ofxSurfingTween
#include "ofxSurfingTween.h"
#endif

#ifdef INCLUDE_ofxSurfingRandomizer
#include "ofxSurfingRandomizer.h"
#endif

//--

#include "SurfingGroupRandomizer.h"

#include "ofxInteractiveRect.h" // engine to move the user clicker buttons panel. TODO: add resize by mouse too.
#include "ofxSurfingConstants.h" // defines (modes) are here "to share between addons" in one place
#include "ofxSurfingHelpers.h"
#include "ofxSurfingImGui.h"
#include "TextBoxWidget.h"

// undo engine
#ifdef INCLUDE_ofxUndoSimple
#include "ofxUndoSimple.h"
#endif

// optional to debug performance or delay when loading files or presets on hd or memory modes
#ifdef DEBUG_PERFORMANCE_MEASURES
#include "ofxTimeMeasurements.h"
#else
#define TS_START
#define TS_STOP
#define TSGL_START
#define TSGL_STOP
#endif

//TODO:
#define USE_IM_GUI
#define USE_JSON //uncomment to use default xml instead json for ofParameterGroup de/serializers

//-------------------------------

class ofxPresetsManager /*: public ofBaseApp*/
{
	//----

public:

	ofxPresetsManager();
	~ofxPresetsManager();

	void update(ofEventArgs& args);
	void draw();
	void windowResized(int w, int h);
	void exit();

private:

	void ImGui_Draw();
	void draw_Help();

public:

	void clear();

	//----
	//
	// API
	//
	//----

public:

	// TODO: should use &reference? it's better?
	void add(ofParameterGroup params, initializer_list<int> keysList); // adds and define keys to trig presets too
	void add(ofParameterGroup params, vector<int> keysList); // adds and define keys to trig presets too

private:

	// TODO: BUG:
	// not working if called from ofApp... must use above 'add' methods
	void add(ofParameterGroup params, int numPresets = 8); // add a param group for preset saving and how many presets on favs
	// could add a correlative keys list {q,w,e,r...}

	//--

public:

	void setup(); // must be called after params groups has been added! (above add methods)
	void setup(std::string name); // TODO: should use char array to avoid collapse with bool..?
	void setup(std::string name, bool _buildGroupSelector);
	void setup(bool _buildGroupSelector);

	void setup_ImGui();

private:

	bool bDoneSetup = false; // to ensure all setup process is done and avoid troubles if not after

public:

	void startup(); // must be called after setup (who is called after all group adds) to set initial states well

private:

	void doCheckPresetsFoldersAreEmpty(); // used on startup. check if all favorites preset are present, and creates folders and content if not

	void setupEngines();

	//----

	// SURFING ENGINES

#ifdef INCLUDE_ofxSurfingTween

//private:
protected:

	ofxSurfingTween dataTween;
	ofParameterGroup params_Tween;

public:

	float get(ofParameter<float> param) { return dataTween.get(param); }
	int get(ofParameter<int> param) { return dataTween.get(param); }

#endif

	//--

#ifdef INCLUDE_ofxSurfingSmooth

//private:
protected:

	ofxSurfingSmooth smoother;
	ofParameterGroup params_Smooth{ "Smooth" };

public:

	float get(ofParameter<float> param) { return smoother.get(param); }
	int get(ofParameter<int> param) { return smoother.get(param); }

#endif

	//--

	// Fix API getters to use when no smooth/tween modes enabled.
#if !defined INCLUDE_ofxSurfingSmooth && !defined INCLUDE_ofxSurfingTween

public:

	float get(ofParameter<float> param) { return param.get(); }
	int get(ofParameter<int> param) { return param.get(); }

#endif

	//--

#ifdef INCLUDE_ofxSurfingRandomizer

//public:
//private:
protected:

	ofxSurfingRandomizer dataRandomizer;
	ofParameterGroup params_Randomizator;

#endif

	//----

private:

	ofParameter<bool> bLockMouseByImGui{ "Mouse Locked", false }; //mouse is over gui

	//--

	// core engine

private:

	// save to a preset
	void save(int presetIndex, int guiIndex = 0);
	void save(int presetIndex, std::string guiName);

	// loads an already saved preset
	void load(int presetIndex, int guiIndex = 0);
	void load(int presetIndex, std::string guiName);

	// get the last loaded preset
	int getPresetIndex(int groupIndex) const;
	int getPresetIndex(std::string groupName) const;

	int getGuiIndex(std::string name) const; // get index for a name of group
	std::string getPresetPath(std::string groupName, int presetIndex); // get path of a preset of a group

	std::string getGroupPath(std::string groupName); // get path of a group. for external monitor only
	std::string getGroupPath(int index); // get path of a group. for external monitor only

	//--

	// main data
	// stores added ofParameterGroup's to be handled by the engine

private:

	// main settings/parameters container
	// all parameters are contained into an ofParameterGroup
	std::vector<ofParameterGroup> groups; // container to store multiple groups. 
	std::vector<int> groupsSizes; // this is the amound of presets of each (index correlative) added group

	//--

	// preset selectors to each added group

public:

	// exposed to allow external callbacks
	std::vector<ofParameter<int>> index_PresetSelected; // one selector for each group

public:

	//
	// API
	//--------------------------------------------------------------
	ofParameterGroup getSelectorsGroup()
	{
		// TODO:
		// could use a customized control group with importante parameters...
		//ofParameterGroup g{ "SelectorsGroup" };
		//for (int i = 0; i < groups.size(); i++) {
		//	g.add(index_PresetSelected[i]);
		//}
		//return g;

		return params_Index_GroupsSelectors;
	}

private:

	std::vector<int> index_PresetSelected_PRE; // remember previous selector to reduce callbacks

	// Useful to expose on externals gui's only.
	// group all group selectors indexes
	ofParameterGroup params_Index_GroupsSelectors{ "SELECTORS" };


	ofParameter<bool> bSoloSelector;

	//--

	// Select active Group 
	// to show on randomize players

public:

	ofParameter<int> index_GroupSelected;

private:

	void Changed_Index_GroupSelected(int& index);

	std::vector<SurfingGroupRandomizer> groupRandomizers;

	// selector for last group: is the main group link
	ofParameterGroup params_Index_GroupSelectorMain{ "G_LINK" };

	//--

	ofParameter<int> index_GROUP_LINK; // global group selector. this selector will control all linked groups

	// group main selector
	bool bBuildGroupSelector = true; // to allow auto build a group selector to combine all the added groups to the presets manager
	bool bLinkedSelectorGroupEnabled = true; // to allow disable main group. not always we need it..
	int groupLinkSize = 9; // default amount of presets we want to the group link

	ofParameter<bool> bGui_PlayerEditor{ "Editor", false };

public:

	//--------------------------------------------------------------
	void setGroupLinkAmountPresets(int size) {// customize how many group link presets we want to create. must be called before setup!
		groupLinkSize = size;
	}

	//--------------------------------------------------------------
	void setGroupLinkSize(int size) {// customize how many group link presets we want to create. must be called before setup!
		groupLinkSize = size;
	}

	//--------------------------------------------------------------
	void setEnableGroupLinkSelector(bool b) {// disable the use of main group selector. must call before setup. enabled by default
		bLinkedSelectorGroupEnabled = b;
	}

	//--------------------------------------------------------------
	void setDisableGroupLinkSelector() {// disable the use of main group selector. must call before setup. enabled by default
		bLinkedSelectorGroupEnabled = false;
	}

private:

	ofParameter<bool> bSplitGroupFolders{ "MODE SPLIT FOLDERS", true }; //on this mode we split every group on his own sub folder
	// finally, we will use all this mode allways, allways enabled
	// TODO: should remove this variable and use always splitted

	//----

	// callbacks engine

	// TODO: must finish D mode !

	// to know when a selector index changed on any group

	// DEPRECATED: A - B - C engines
	// Only for one group selector. We cant differ witch group selector changed...

	//--

	// A. easy callbacks
	// loaded / saved
	// to faster ofApp integration 
	// to check in update() as callback

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

private:

	bool bIsDoneLoad = false;

	//--

public:

	//--------------------------------------------------------------
	bool isDoneSave()
	{
		if (bIsDoneSave)
		{
			bIsDoneSave = false;
			return true;
		}
		return false;
	}

private:

	bool bIsDoneSave = false;

	//--

	// B. better callbacks
	// loaded / saved
	// to get (from ofApp) when it happens

public:

	ofParameter<bool> DONE_load; // easy callback to know (in ofApp) that preset LOAD is done 
	ofParameter<bool> DONE_save; // easy callback to know (in ofApp) that preset SAVE is done

	//--

	// C. easy trig-callback
	// used to get alerted when preset has not changed but we like to retrig something
	// in some situation we would like this feature:
	// 1. user clicked a preset box
	// 2. but to the same current loaded preset
	// 3. no need to reload the file settings
	// 4. but we want to use the user box click to trig something

private:

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

	//// TODO:
	//// D. better callbacks 
	//// splits callback listeners for each group !
	//// template example to uso on your ofApp:
	//// all presets selectors callback
	////--------------------------------------------------------------
	//void Changed_PresetsManagerSelectors(ofAbstractParameter &e);
	//// TODO:
	//// setup()
	//{
	//	// create callbacks
	//	for (int i = 0; i < presetsManager.index_PresetSelected.size(); i++)
	//	{
	//		ofAddListener(presetsManager.getSelectorsGroup().parameterChangedE(), this, &ofApp::Changed_PresetsManagerSelectors);
	//	}
	//}
	//// callback
	////--------------------------------------------------------------
	//void ofApp::Changed_PresetsManagerSelectors(ofAbstractParameter &e)
	//{
	//	// when a selector changes, we update the local groups too, to mantain all groups synced!
	//	if (!bDisableCallbacks)
	//	{
	//		std::string name = e.getName();
	//		ofLogNotice(__FUNCTION__) << name << " : " << e;
	//		cout << (__FUNCTION__) << name << " : " << e << endl;
	//		if (0) {}
	//		else if (name == paragraphs[0].params_BlockLayout.getName())
	//		}
	//		else if (name == paragraphs[0].params_Fonts.getName())
	//		{}
	//		else if (name == paragraphs[0].animatorPos.getParameters().getName())
	//		{}
	//	}
	//}

	//--

#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER

private:

	void draw_Gui_ClickerPresets_Native(); // user clickeable box panel preset selector

private:

	// mini preview rectangles positions and sizes
	void doRectFit();
	ofxInteractiveRect rectangle_PresetClicker = { "rectangle_PresetClicker" };
	std::string path_RectanglePresetClicker = "_PresetClicker";
	ofParameter<bool> bMODE_EditPresetClicker;
	ofParameter<bool> bGui_BgEditPresetClicker;
	float _RectClick_Pad;
	//ofParameter<bool> helpPos;
	//ofParameter<float> _rectRatio;
	//ofParameter<bool> bResetRects;

	ofColor _colorText; // lines and text color
	ofColor _colorButton; // bg selected button
	ofColor _colorBg; // background color

	DoubleClicker doubleClicker;

	ofParameter<bool> bThemeDarkOrLight{ "Theme B/W", true };
	ofParameter<bool> bMODE_LockClicker{ "Lock Clicker", false };

private:

	// preset clicker boxes matrix
	ofParameter<int> cellSize{ "ButSize", 80, 45, 120 }; // default box button size
	//int cellSize = 80; // default box button size
	ofVec2f clicker_Pos; // default clicker position

//--

// mouse

private:

#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER

	void mousePressed(int x, int y);
	bool lastMouseButtonState = false;

#endif


	//--

	//----
	//
	// API
	//
	//----

	// layout helpers

public:

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

	/*
	////--------------------------------------------------------------
	//float getPresetClicker_Width()// get width of bigger group of presets row
	//{
	//	float _maxPresets = 0;
	//	for (int i = 0; i < groups.size(); i++) {
	//		if (getAmountPresetsOfGroup(getAmountGroups() - 1) > _maxPresets) _maxPresets = getAmountPresetsOfGroup(getAmountGroups() - 1);
	//	}
	//	int _extraButtons = 2;
	//	float w = cellSize * (_maxPresets + _extraButtons);

	//	return w;
	//}
	////--------------------------------------------------------------
	//glm::vec2 getPresetClicker_Position()
	//{
	//	glm::vec2 p = glm::vec2(clicker_Pos.x, clicker_Pos.y);
	//	return p;
	//}
	////--------------------------------------------------------------
	//void setPosition_DEBUG(int x, int y); // where to display if we get errors (ie: data files not found) on startup
	*/

#endif

	//----

	// keys

	//--------------------------------------------------------------
	void setEnableKeys(bool active)
	{
		if (guiManager.bKeys != active) {// avoid if state not need to change
			guiManager.bKeys = active;
		}
	}
	//--------------------------------------------------------------
	void setToggleEnableKeys()
	{
		guiManager.bKeys = !guiManager.bKeys;
	}
	//--------------------------------------------------------------
	bool isKeysEnabled()
	{
		return guiManager.bKeys;
	}
	//--------------------------------------------------------------
	void setModeKeySave(int key); // setup the key you have to hold for saving, default is OF_KEY_CONTROL
	void setModeKeySwap(int key); // setup the key you have to hold for swap, default is OF_KEY_ALT

	//--------------------------------------------------------------
	void setModeEditorOrLive(bool b) {
		bMODE_EditLive = b;
	}

	//----

	// undo engine

#ifdef INCLUDE_ofxUndoSimple

	// you can manually store all the parameters states to store points
	// only works on edit mode
	// then you can browse doing undo/redo to decide what states you like more.
	// when doing a random, the engine auto stores the states.
	// currently working automatic only when called by key command (ctrl+R) not when clicking gui "randomize parameters"
	// when called using gui button, you must store states manually (ctrl+s)

private:

	vector <ofxUndoSimple<std::string>> undoStringsParams;
	vector <ofXml> undoXmlsParams;
	void doRefreshUndoParams();
	void doStoreUndo(); // store current point to undo history
	void doUndo();
	void doRedo();
	void doClearUndoHistory();

#endif

	//----

	// paths for settings

private:

	// all folder names must go without ending with '/'
	//std::string path_Root;
	std::string path_UserKit_Folder; // User-Kit folder for both other subfolders. 
	std::string path_PresetsFavorites; // path for kit of favorite presets. live kit
	std::string path_PresetsStandalone; // path for browse other presets. archive kit
	std::string path_ControlSettings; // path for app state session settings

	std::string filename_ControlSettings;
	std::string filename_Randomizers; // path for randomizers settings
	std::string filenamesPrefix; // to add to file names to split names with index

	std::string filenameMainSettings; // file for main settings
	std::string fileExtension; // xml or json

	std::string displayNameUserKit;

	//--

	// app settings for many params

private:

	void load_AppSettings(); // handle group selectors and some settings states
	void save_ControlSettings(); // handle group selectors and some settings states

	//----

	// TODO:
	// memory mode

	// improve performance loading from xml objects from memory not xml files from hd

private:

	ofParameter<bool> MODE_MemoryLive;
	// when enabled all presets are handled from a memory vector to avoid lag of loading xml files from hd

public:

	//--------------------------------------------------------------
	void setModeMemoryPerformance(bool b)
	{
		MODE_MemoryLive = b;
	}

private:

	ofParameter<bool> loadToMemory;
	ofParameter<bool> saveFromMemory;
	void load_AllKit_ToMemory();
	void saveAllKitFromMemory();

	//--

private:

	// TODO:
	// this is not finished yet!
	// it's intended as a faster mode that uses memory loaded xmls instead of reading the files from the hard drive.
	// add json..
	// should use keys[i].size() instead of this:
#ifdef USE_JSON
	vector<ofJson> mainGroupMemoryFilesPresets;
#else
#ifdef USE_XML
	vector<ofXml> mainGroupMemoryFilesPresets;
#endif
#endif

	//--

	//----
	//
	// API
	//
	//----

	// randomizer helpers

public:

	//--------------------------------------------------------------
	void setPlayRandomizerTimer(bool b, int groupIndex = -1)// play randomizer timer
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex;
		groupRandomizers[groupIndex].setPlayRandomizerTimer(b);
	}
	//--------------------------------------------------------------
	void setTogglePlayRandomizerPreset(int groupIndex = -1)// toggle randomizer timer
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex;
		groupRandomizers[groupIndex].setTogglePlayRandomizerPreset();
	}
	//--------------------------------------------------------------
	void doRandomizePresetSelected(int groupIndex = -1) {// randomize params of current selected preset
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex;
		// check if minimum one parameter is enabled
		groupRandomizers[groupIndex].doCheckRandomReady();
		groupRandomizers[groupIndex].doRandomizePresetSelected();
	}
	//--------------------------------------------------------------
	void setRandomizerDuration(float t, int groupIndex = -1)
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex;
		groupRandomizers[groupIndex].durationLong = t;
		groupRandomizers[groupIndex].randomizeDurationBpm = 60000.f / groupRandomizers[groupIndex].durationLong;
	}
	//--------------------------------------------------------------
	void setRandomizerDurationShort(float t, int groupIndex = -1)
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex;
		groupRandomizers[groupIndex].durationShort = t;
	}
	//--------------------------------------------------------------
	void setRandomizerBpm(float bpm, int groupIndex = -1)
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex;

		//TODO:
		groupRandomizers[groupIndex].randomizeDurationBpm = bpm;

		// 60,000 ms (1 minute) / Tempo (BPM) = Delay Time in ms for quarter-note beats
		groupRandomizers[groupIndex].durationLong = 60000.f / bpm;
		groupRandomizers[groupIndex].durationShort = groupRandomizers[groupIndex].durationLong / 2.f;
	}
	////--------------------------------------------------------------
	//void doRandomizePresetFromFavs()// trig randomize and select one of the favs presets
	//{
	//	bRandomizeIndex = true;
	//}

	//--

	//----
	//
	// API
	//
	//----

public:

	//--------------------------------------------------------------
	void loadPreset(int p, int _indexGroup = -1); // load preset for each group by code from ofApp
	void savePreset(int p, int _indexGroup = -1); // save preset for each group by code from ofApp

	// legacy/to make compatible with ofxSurfingPresets
	//--------------------------------------------------------------
	void doLoad(int p, int _indexGroup = -1) {
		loadPreset(p, _indexGroup);
	};

	//--

public:

	//--------------------------------------------------------------
	void doSaveCurrent() {
		saveCurrentPreset(0);//for first group only
	}

	//--------------------------------------------------------------
	void saveCurrentPreset(int groupIndex = -1) {
		if (groupIndex == -1) groupIndex = groups.size() - 1;

		ofLogNotice(__FUNCTION__) << "SAVE PRESET  group: " << groupIndex << " preset: " << index_PresetSelected[groupIndex].get();
		save(index_PresetSelected[groupIndex].get(), groupIndex);
	}

//public:
private:

	//--------------------------------------------------------------
	void saveCurrentPreset(std::string groupName) {
		int _groupIndex = -1;
		for (int i = 0; i < groups.size(); i++) {
			if (groups[i].getName() == groupName) _groupIndex = i;
		}
		if (_groupIndex != -1) {
			ofLogNotice(__FUNCTION__) << "SAVE PRESET from group: " << groupName << " #" << _groupIndex << " preset: " << index_PresetSelected[_groupIndex].get();
			save(index_PresetSelected[_groupIndex].get(), _groupIndex);
		}
		else {
			ofLogError(__FUNCTION__) << "Can't found any group named: " << groupName << ". Save command failed!";
		}
	}

	// presets browsing by code from ofApp
	// default if not defined, is the last one: main group link
	//--------------------------------------------------------------
	void load_Previous(int groupIndex = -1, bool cycled = false)
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		groupIndex = (int)ofClamp(groupIndex, 0, groups.size() - 1);

		int _curr = index_PresetSelected[groupIndex];


		if (!cycled)//limited
		{
			_curr--;
			_curr = (int)ofClamp(_curr, 0, groupsSizes[groupIndex] - 1);
		}
		else {//cycled
			if (_curr == 0) _curr = groupsSizes[groupIndex] - 1;
			else _curr--;
		}

		index_PresetSelected[groupIndex] = _curr;
	}

	// default if not defined, is the last one: main group link
	//--------------------------------------------------------------
	void load_Next(int groupIndex = -1, bool cycled = false)
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		groupIndex = (int)ofClamp(groupIndex, 0, groups.size() - 1);

		int _curr = index_PresetSelected[groupIndex];

		if (!cycled)//limited
		{
			_curr++;
			_curr = (int)ofClamp(_curr, 0, groupsSizes[groupIndex] - 1);
		}
		else {//cycled
			if (_curr == groupsSizes[groupIndex] - 1) _curr = 0;
			else _curr++;
		}

		index_PresetSelected[groupIndex] = _curr;
	}

	//--------------------------------------------------------------
	void load_Previous(bool cycled)
	{
		load_Previous(index_GroupSelected, cycled);
	}

	//--------------------------------------------------------------
	void load_Next(bool cycled)
	{
		ofLogNotice(__FUNCTION__);
		load_Next(index_GroupSelected, cycled);
	}

public:

	// Legacy	
	//--------------------------------------------------------------
	void doLoadPrevious(/*bool cycled = false*/)
	{
		ofLogNotice(__FUNCTION__);
		//load_Previous(true, cycled);

		bDISABLE_CALLBACKS = true;
		int sel = index_GroupSelected.get();
		int i = index_PresetSelected[sel];
		i--;
		bDISABLE_CALLBACKS = false;
		index_PresetSelected[sel] = (int)ofClamp(i, 0, index_PresetSelected[sel].getMax());
	}
	//--------------------------------------------------------------
	void doLoadNext(/*bool cycled = false*/)
	{
		ofLogNotice(__FUNCTION__);
		//load_Next(true, cycled);

		bDISABLE_CALLBACKS = true;
		int sel = index_GroupSelected.get();
		int i = index_PresetSelected[sel];
		i++;
		bDISABLE_CALLBACKS = false;
		index_PresetSelected[sel] = (int)ofClamp(i, 0, index_PresetSelected[sel].getMax());
	}

	//--

	// Extra API methods

//public:
private:

	// Helpers for external layout or other workflow purposes

	//--------------------------------------------------------------
	int getCurrentPreset(int _group) // get index of selected preset on the group
	{
		int _presetIndex = -1;
		if (_group < groups.size()) _presetIndex = index_PresetSelected[_group];
		return _presetIndex;
	}

	//--------------------------------------------------------------
	int getAmountGroups() // how many groups we added on setup
	{
		if (groups.size() > 0) return groups.size();
		else return 1; // workaround to set default height before groups are added
	}

	//--------------------------------------------------------------
	int getAmountPresetsOfGroup(int g) // how many presets on the added group
	{
		int _size = -1;
		if (g < groups.size()) _size = groupsSizes[g];
		return _size;
	}

	//--------------------------------------------------------------
	std::string getGroupsNames()
	{
		std::string _names = "";
		for (int i = 0; i < groups.size(); i++)
		{
			_names += groups[i].getName();
			_names += "\n";
		}
		return _names;
	}

	//--------------------------------------------------------------
	std::string getGroupsPaths(bool simplified = true) // get root paths for the preset files of the groups
	{
		std::string _names = "";

		if (!simplified) // full paths info
		{
			for (int i = 0; i < groups.size(); i++)
			{
				_names += getGroupPath(i);
				_names += "\n";
			}
		}
		else // simplified path info
		{
			if (!bPathDirCustom) _names += "/data/" + path_UserKit_Folder + "/\n";
			_names += path_PresetsFavorites + "/         Favorites\n";
			_names += path_PresetsStandalone + "/         Standalones\n";
			_names += "\n";
			_names += "GROUPS\n";

			for (int i = 0; i < groups.size(); i++)
			{
				_names += "#" + ofToString(i) + " ";
				_names += groups[i].getName();
				_names += "/\n";
			}
		}
		return _names;
	}

	//--------------------------------------------------------------
	std::string getGroupsPath()
	{
		std::string _names = "";
		_names = getGroupPath(0); // simpler: get first group bc all groups are in the same parent folder
		return _names;
	}

	//--------------------------------------------------------------
	void setVisible_GroupName(bool b) // disabler for minimal design
	{
		bShow_GroupName = b;
	}

public:

	//--------------------------------------------------------------
	std::string getGroupName(int i = 0)
	{
		std::string _name = "ERROR. Index out of range of vector Groups";
		if (i < groups.size()) _name = groups[i];
		return _name;
	}

	//----

	// Helper tools

public:

	void doCloneRight(int groupIndex); // clone from selected preset of index group to all others to the right
	void doCloneAll(int groupIndex); // clone all presets from the current index group
	void doPopulateFavs(int groupIndex); // fast populate random presets of index group
	void doPopulateFavsALL(); // fast populate random presets around all favs of index group
	//void doResetsParams();
	//void doResetGroup(ofParameterGroup& group);

	void doSwap(int groupIndex, int fromIndex, int toIndex);

	//----

	//----
	//
	// API
	//
	//----

	// Layout

public:

	//--------------------------------------------------------------
	void setVisible_Help(bool b)
	{
		guiManager.bHelp = b;
	}
	//--------------------------------------------------------------
	void setToggleVisible_Help()
	{
		guiManager.bHelp = !guiManager.bHelp;
	}

	//--------------------------------------------------------------
	void setVisibleGui(bool b)
	{
		//bGui_Global = b;
		bGui = b;
	}
	//--------------------------------------------------------------
	void setToggleVisibleGui()
	{
		bGui = !bGui;
	}

	//--------------------------------------------------------------
	void setVisible_PresetClicker(bool visible)
	{
		bGui_Clicker = visible;
	}
	//--------------------------------------------------------------
	void setToggleVisible_PresetClicker()
	{
		bGui_Clicker = !bGui_Clicker;
	}
	//deprecated
	//--------------------------------------------------------------
	bool getVisible_PresetClicker()
	{
		return bGui_Clicker;
	}
	//--------------------------------------------------------------
	bool isVisible_PresetClicker()
	{
		return bGui_Clicker;
	}

	////--------------------------------------------------------------
	//float getGroupNamesWidth() {
	//	float _maxw = 0;
	//	for (int i = 0; i < groups.size(); i++)
	//	{
	//		float _w;
	//		std::string info = groups[i].getName() + "* ";
	//		if (myFont.isLoaded()) { _w = myFont.getStringBoundingBox(info, 0, 0).width; }
	//		else { _w = 200; }
	//		if (_w > _maxw) _maxw = _w;
	//	}
	//	return _maxw + 22;
	//}

	////--------------------------------------------------------------
	//void setPosition_PresetClicker(int x, int y, int _cellSize)
	//{
	//	clicker_Pos.x = x;
	//	clicker_Pos.y = y;
	//	cellSize = _cellSize;
	//}

	//----

	// Customize file paths

	//--------------------------------------------------------------
	void setPath_UserKit_Folder(std::string folder);
	// path for root container folder. must be called before setup()!

	void setPath_PresetsFavorites(std::string folder);
	// path folder for favorite/live presets

	void setPath_PresetsStandalone(std::string folder);
	// path folder for standalone presets kit for the browser
	//--------------------------------------------------------------
	void setPath_ControlSettings(std::string str)// for the session states settings
	{
		ofLogNotice(__FUNCTION__) << str;
		path_ControlSettings = str;
	}

	// TODO: customize root path. this could be the user-kit path ??
	////--------------------------------------------------------------
	//void setPath_Root(std::string str)
	//{
	//	ofLogNotice(__FUNCTION__) << str;
	//	path_Root = str;
	//}

	//----

	// API

	// Modes

	//--------------------------------------------------------------
	void setModeAutoLoad(bool b)
	{
		bAutoLoad = b;
	}
	//--------------------------------------------------------------
	void setModeAutoSave(bool b)
	{
		bAutoSave = b;
	}
	//--------------------------------------------------------------
	bool getModeAutoSave()
	{
		return bAutoSave;
	}
	//--------------------------------------------------------------
	void setModeAutoSaveTimer(bool b)
	{
		bAutosaveTimer = b;
	}

	//----

private:

	int PRESET_Selected_IndexMain_PRE = -1;
	// used as callback

	//----

private:

	ofParameter<bool> bMODE_EditLive{ "EDIT MODE", true };
	// this mode improves performance disabling autosave, undo history..etc

public:

	ofParameter<bool> bGui_Global; //TODO:
	ofParameter<bool> bGui; //TODO:
	ofParameter<bool> bGui_Clicker; // to allow include as toggle parameter into external gui
	ofParameter<bool> bGui_Main;
	ofParameter<bool> bGui_Standalones;
	ofParameter<bool> bGui_Parameters;
	ofParameter<bool> bGui_Players;
	ofParameter<bool> bGui_PresetsParams;
	ofParameter<bool> bGui_AdvancedControl;

	//private:

		//ofParameter<glm::vec2> Gui_Internal_Position;

private:

	bool bShow_GroupName = true; // draws group name into clicker boxes panel
	bool bShowClickerInfo = true;

	//--

private:

	std::string helpInfo; // info text to display shortcuts or path settings
	void buildHelpInfo();

	//--

	// ImGui

public:

	void draw_Gui_Players();
	void draw_Gui_PlayersWidgets();
	void draw_Gui_Main();
	void draw_Gui_WidgetsAdvanced();
#ifdef INCLUDE_ofxUndoSimple
	void draw_Gui_WidgetsUndo();
#endif
	void draw_Gui_Standalones();
	void draw_Gui_Parameters();
	void draw_Gui_ClickerPresets_ImGui();

public:

	bool ImGui_Draw_Windows();

	//-

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL

	ofxImGui::Gui gui_ImGui;
	void ImGui_Setup();
	void ImGui_Begin();
	void ImGui_End();
	ImFont* customFont = nullptr;
	ImGuiStyle* style = nullptr;
	ofxImGui::Settings settings;

#endif

#ifdef USE_PRESETS_MANAGER__IMGUI_LAYOUT 

	ofxSurfing_ImGui_Manager guiManager;
	void setupGuiStyles();

	//----

#endif

	//--

private:

	// Mouse over gui handler
	// Detects mouse interaction on ImGui to block other mouse actions like: easy camera... 
	ofParameter<bool> bImGui_mouseOver;
	bool bImGui_mouseOver_PRE;
	bool bMouseOver_Changed = false;

	//-----

	// Standalone presets browser
	// Located into 'archive/' folder, not from favorites presets folders

private:

	// Multi-group files
	vector <std::vector<std::string>> standaloneFileNames; // all the group standalone preset names
	vector <std::vector<ofFile>> standaloneFiles; // all the group standalone preset files
	vector <int> standaloneFileIndex; // selected standalone preset file of the group
	vector <std::string> standaloneNamePresetDisplay;

	// Engine handler
	bool bFilesError = false;
	ofParameter<bool> MODE_StandalonePresets_NEW;
	std::string inputText_NEW = ""; //user input text
	//std::string inputText_TEMP = "";

private:

	void doStandalonePresetsBuild(int groupIndex = -1); // standalone presets splitted from favorites presets
	void doStandalonePresetLoad(std::string name, int groupIndex = -1);
	void doStandalonePresetSave(std::string name, int groupIndex = -1);
	bool doStandalonePresetsRefresh(int groupIndex = -1);
	void doStandalonePresetsBuildFromFavs(int groupIndex = -1); // save all favorites-presets to the standalone-presets (archive) folder

	//----

	// API

	// Helpers to easy integrate useful controls into external gui's

private:

	// to expose basic controls to allow use on external gui
	ofParameterGroup params_Controls{ "PRESETS MANAGER" };

public:

	//TODO:
	//// This are useful parameters to use in our ofAPp/projects/add-ons gui's
	////--------------------------------------------------------------
	//ofParameterGroup getParamsControls()
	//{
	//	//cout << "displayNameUserKit: " << displayNameUserKit << endl; // TODO: not refreshing well on startup..

	//	params_Controls.clear();
	//	//params_Controls.setName(displayNameUserKit);
	//	//params_Controls.setName("OVERLAY");

	//	// Windows
	//	//params_Controls.add(getParamsPresetSelectors());
	//	// macOS casting...
	//	ofParameterGroup g1;
	//	g1 = getParamsPresetSelectors();
	//	params_Controls.add(g1);

	//	// Windows
	//	//params_Controls.add(getParamsRandomizers());
	//	// macOS casting...
	//	ofParameterGroup g2;
	//	g2 = getParamsRandomizers();
	//	params_Controls.add(g2);

	//	params_Controls.add(bGui_Clicker);
	//	params_Controls.add(bMODE_EditLive);

	//	return params_Controls;
	//}

	//--------------------------------------------------------------
	ofParameterGroup getParamsPresetSelectors() { // selectors index to all the added groups
		return params_Index_GroupsSelectors;
	}

	//TODO:
	////--------------------------------------------------------------
	//ofParameterGroup getParamsRandomizers() {// Common/important settings to handle randomizers from outer scope.
	//	ofParameterGroup _g{ "RANDOMIZERS" };
	//	for (int i = 0; i < groups.size(); i++)
	//	{
	//		// Windows
	//		//_g.add(groupRandomizers[i].getParamsRandomizers());
	//		// macOS casting...
	//		ofParameterGroup g;
	//		g = groupRandomizers[i].getParamsRandomizers();
	//		_g.add(g);
	//	}
	//	return _g;
	//}

	//----

	// Keys

public:

	void keyPressed(ofKeyEventArgs& eventArgs);
	void keyReleased(ofKeyEventArgs& eventArgs);

private:

	void addKeysListeners();
	void removeKeysListeners();

	//void removeMouseListeners(); // TODO: fix easy remover..

	vector<vector<int>> keys; // queued trigger keys for each group ? (all presets) (size of)
	vector<vector<string>> labels; // converted to strings
	bool bKeys_NotYetActivated;

	// save keys
	int modeKeySave; // save mod key
	bool bModKeySave; // save mod key state

	// swap selected preset with the currently clicked (i.e: 4 -> 7  &  4 <- 7)
	int modKeySwap; // swap mod key 
	bool bModKeySwap; // swap mod key state

public:

	// switch on or off the control with the keys
	void setToggleKeysControl(bool active);

	//--

private:

	bool bKeys_ArrowBrowse = true; // allow browse presets by arrows keys by default

public:

	//--------------------------------------------------------------
	void setEnableKeysArrowBrowse(bool b)
	{
		bKeys_ArrowBrowse = b;
	}

	//----

private:

	// font to label clicker boxes
#ifdef USE__PRESETS_MANAGER__NATIVE_CLICKER
	ofTrueTypeFont myFont;
	std::string myTTF; // gui font for all gui theme
	int sizeTTF;
#endif

	TextBoxWidget helpTextBoxWidget;

	//----

	// Callbacks

private:

	void Changed_Control(ofAbstractParameter& e);
	void Changed_User(ofAbstractParameter& e);

	//--

private:

	bool bDISABLE_CALLBACKS = true; // to avoid startup crashes and objects are not initialized properly
	// updating some params before save will trigs also the group callbacks
	// so we disable this callbacks just in case params updating are required
	//bool bDISABLE_CALLBACKS_Selectors = false; // to avoid multiple calls on multiple presets selector engine

	//--

public:

	ofParameterGroup params_Control; // to use on external gui

public:

	ofParameter<bool> bAutoSave;
	// auto save current preset when user clicks to another preset. almost enabled on edit mode

	ofParameter<bool> bAutoLoad;
	// auto load the user clicked preset. almost always true

private:

	// Internal groups
	ofParameterGroup params_User;
	ofParameterGroup params_Gui;
	ofParameterGroup params_Options;
	ofParameterGroup params_Randomizer;
	ofParameterGroup params_Custom;

	//----

	// Custom path for preset favorites

private:

	ofParameter<bool> bPathDirCustom;
	ofParameter<std::string> pathDirCustom;

public:

	// Set custom path
	void doFileDialogProcessSelection(ofFileDialogResult openFileResult);
	void buildCustomUserKit();
	void buildDefaultUserKit();

	//--

	// TODO:
	// not finished yet...
	// timer autosave

private:

	ofParameter<bool> bAutosaveTimer;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 9000;

	//--

	// check if a folder path exist and creates one if not

private:

	// many times when you try to save a file, this is not possible and do not happens bc the container folder do not exist
	//--------------------------------------------------------------
	void CheckFolder(std::string _path)
	{
		ofxSurfingHelpers::CheckFolder(_path);
	}

	//--------------------------------------------------------------
	void CheckAllFolders() // check that folders exist and create them if not
	{
		// TODO: 
		// use a container for all User-Kit together...
		// CheckFolder(path_Root);

		std::string _path;

		_path = path_UserKit_Folder; // current kit-preset main folder
		CheckFolder(_path);

		_path = path_UserKit_Folder + "/" + path_PresetsFavorites; // current kit-presets favorite presets folder
		CheckFolder(_path);

		_path = path_UserKit_Folder + "/" + path_PresetsStandalone; // current kit-presets standalone presets folder
		CheckFolder(_path);

		//_path = path_UserKit_Folder + "/" + path_ControlSettings; // for randomizer settings (into his own kit-preset folder)
		//CheckFolder(_path);

		_path = path_UserKit_Folder + "/" + path_ControlSettings; // app settings (shared from all kit-presets)
		CheckFolder(_path);

		// create the folders of each group into main presets folder 
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

	//--

	//TODO:
	// extra params not included into presets
	// but sometimes is useful to centralize all params together
	// to use the populated widgets together

private:

	ofParameterGroup params_AppSettings{ "AppState" };
	bool bAppStateParams = false;

public:

	void addExtra(ofParameterGroup& g); // Extra params added but not included into presets

	//--

	// Tools

	//--------------------------------------------------------------
	void doResetFull() {//reset all params to min
		int ig = index_GroupSelected.get();
		if (ig < groups.size())
			groupRandomizers[ig].doResetGroup(groups[ig], true);
	}

	////--------------------------------------------------------------
	//void doResetFiltered() {//reset filter enabled params to random min/max
	//	int ig = index_GroupSelected.get();
	//	if (ig < groups.size())
	//		groupRandomizers[ig].doResetGroup(groups[ig], false);
	//}

	//--------------------------------------------------------------
	void doRandomFull() {//random all params to min
		int ig = index_GroupSelected.get();
		if (ig < groups.size())
			groupRandomizers[ig].doRandomGroupFull(groups[ig]);
	}

	////--------------------------------------------------------------
	//void doRandomFiltered() {//random filter enabled params to random min/max
	//	int ig = index_GroupSelected.get();
	//	if (ig < groups.size())
	//		groupRandomizers[ig].doRandomPreset();
	//}

	//--

	//--------------------------------------------------------------
	ofParameter<bool> getPlayToggle() {
		return groupRandomizers[0].bPLAY_RandomizeTimer;
	}

#ifdef USE_PRESETS_MANAGER__IMGUI_INTERNAL
	//--------------------------------------------------------------
	void setThemeDarkOrLight(bool b) {
		bThemeDarkOrLight = b;
	}

	bool bMouseOverGui;
	//--------------------------------------------------------------
	bool isMouseOverGui() {
		bool b = rectangle_PresetClicker.inside(ofGetMouseX(), ofGetMouseY());
		//return bMouseOverGui;
		return b;
	}
#endif

	//--

	ofParameter<bool> bPLAY_Global{ "PLAY GLOBAL", false };
};





/////////////////////////////////////////////////////////////////////////////////////////
//
//	DOCUMENTATION
//
//	This is a very simple guide to use ofxPresetsManager.
//
//	* EXAMPLES
//	1_example-Basic: simpler example using one group only
//	2_example-MultiGroup: more complex example using several and independent groups.
//
//	* BASIC USAGE
//	Here we will control ONE ofParameterGroup only, but the add-on can handle more groups too!
//
//	0. init your scene and the related parameters / settings.
//	1. add your parameters to the ofParameterGroup acting as the main container.
//  2.0 optionally customize some settings if desired.
//	2.1 add the container to the add-on instantiated class object. 
//	2.2 you can define how many presets you want for the group, 
//	and what keys to associate as triggers.
//	3. done! just play with the add-on gui. 
//
//	* Preset Files
//		- There's one file for each preset (includes all settings/parameters). 
//		files will be placed into /bin/data by default. but path can be customized, 
//		mainly to share the same settings folder between apps.
//		- When using multiple groups, each group has his own folder.
//
//	* Two Types of Presets
//		- Favorites: the clickable-boxes/key-trigged presets
//		- Standalones: the archived and named presets files that you can load from/to favorites
//
//	* Two Modes
//		- Edit mode: auto save when browsing bank presets, undo engine allowed, all gui panels visible or enabled.
//		- Live mode: simpler GUI and maybe better performance intended when no need to edit or see parameters.
//
//	* Several Features
//		- Store standalone name presets to make variations and help your favorite banks organization.
//		- Keys interaction can be disabled to avoid colliding with other add-ons/ofApp key commands.
//		- Layout customization and 2 colors themes.
//		- Randomization engine to allow more complex scenes, jumping between presets using different timers.
//		- You don't need to use another gui like ofxGui: just ofParameters! Sometimes ImGui is enough.
//		- Clone presets, move placing/sorting into your favorites.
//		- All your favorite presets (left-right) from your groups (up-down) can be browsed using arrow keys.
//		- Active group is marked with an '*' (when keys control are enabled).	
//
/////////////////////////////////////////////////////////////////////////////////////////


