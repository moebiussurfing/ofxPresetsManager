
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


/////////////////////////////////////////////////////////////////////////////////////////
//
//	DOCUMENTATION
//
//	this is a very simple guide to use ofxPresetsManager.
//	we will add only ONE ofParameterGroup, but the addon can handle more groups!
//
//	* usage:
//	0. init your scene and the related parameters / settings.
//	1. add our parameters to the ofParameterGroup as container.
//  2.0 customize some settings if desired
//	2.1 add the container to the addon object. 
//	2.2 you can define how many preset you want, and what keys to associate as triggers.
//	3. done! just play with the addon gui. 
//
//	the file settings (one for each preset) will be placed into /bin/data.
//	there are lot of implemented features:
//		- 
//
//	there are two types of presets: 
//		- favourites: the clickable-boxes/key-trigged presets
//		- standalone: the archived and named presets
//
//


///------

///	BUGS:
///			! broken swap key
///			! broken key command to randomize

///-

///	TODO:

///	+++		open/save dialog to project User-Kit session in a single file.
///				or allowed to all the groups?

///	++		randomize editor preset
///				preset mini engine. ABC dropdown list for randomizers
///				could add also several randomizer settings presets. selectable by dropdown list..
///				add limit range min/max to randomize
///				do nesting toggles to improve view. create a group for related toggles..	
///				clone using editor toggles to avoid clone disabled toggle params
///				mode state to allow overwrite only enabled toggle params

///	++		add populator engine to create all preset files if it's a new empty project
///				add setter to enable some params to randomize
///				call populate. disable debug_display red info

///	+		add/restore-back undo to all the groups

///	++		lock (by toggle) params that we want to ignore on changing presets
///				can be done enabling/disabling serializable for each param with a group of toggles

///	++		performance: 
///				restore-back memory_mode. (use xml objects into memory vs hd files) to extra groups too

///	+		repair autosave timer. exclude log
///	++		make a lite minimal version compatible ! without ImGui, maybe with ofxGui or even without any GUI.
///	++		add define to disable all browser/ImGui/randomize stuff to make addon minimal expression 
///				or add simpler class but compatible with preset files kits
///	+		could make tween when changing params using ofLerp or ofxKeyTween...
///

///------


#pragma once

#include "ofMain.h"

//--------------------------------------
//
//	DEFINES
//
//#define USE_IMGUI_EXTERNAL						// this is to group all ImGui panels into one unique instance in ofApp
#define INCLUDE_IMGUI_CUSTOM_THEME_AND_FONT		// customize ImGui font
//#define INCLUDE_ofxUndoSimple					// undo engine to store after randomize preset parameters (& recall)
//#define USE_JSON								// file settings format
//										   
//	DEBUG									   
//										   
//#define INCLUDE_PERFORMANCE_MEASURES			// measure performance ofxTimeMeasurements
//#define DEBUG_randomTest						// uncomment to debug randimzer. comment to normal use. if enabled, random engine stops working
//#define DEBUG_BLOCK_SAVE_SETTINGS				// disable save settings//enable this bc sometimes there's crashes on exit
//
//--------------------------------------


#include "ofxSurfingConstants.h" // -> defines (modes) are here "to share between addons" in one place
#include "ofxInteractiveRect.h"

//--

// gui
// uasing alternative branch: https://github.com/MacFurax/ofxImGui/tree/docking
// this branch allows docking, layout store/recall, some extra widgets 

#include "ofxImGui.h"

// undo engine
#ifdef INCLUDE_ofxUndoSimple
#include "ofxUndoSimple.h"
#endif

#include "ofxSurfingHelpers.h"
#include "groupRandomizer.h"

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

// TODO: removable ofBaseApp? or it's mandatory for auto update() ??
class ofxPresetsManager : public ofBaseApp
{

public:
	// mini preview rectangles positions and sizes
	ofxInteractiveRect rectanglePresetClicker = { "rectanglePresetClicker" };
	string path_RectanglePresetClicker = "_RectanglePresetClicker";
	ofParameter<bool> MODE_EditPresetClicker;
	ofParameter<float> _rectRatio;
	ofParameter<bool> SHOW_BackGround_EditPresetClicker;
	//ofParameter<bool> bResetRects;
	float _RectClick_w;
	float _RectClick_Pad;

	//--

public:
	ofxPresetsManager();
	~ofxPresetsManager();

	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void drawImGui();
	void exit();
	void windowResized(int w, int h);

	//--

	//----
	//
	// API
	//
	//----

	//--

public:
	// TODO: should use &reference? it's better?
	void add(ofParameterGroup params, initializer_list<int> keysList);// adds and define keys to trig presets too
	void add(ofParameterGroup params, vector<int> keysList);// adds and define keys to trig presets too

private:
	// TODO: BUG:
	// not working if called from ofApp... must use above 'add' methods
	void add(ofParameterGroup params, int numPresets = 8);// add a param group for preset saving and how many presets on favs
	// could add a correlative keys list {q,w,e,r...}

	//--

public:
	void setup();// must be called after params groups has been added! (above add methods)
	void setup(std::string name);// TODO: should use char array to avoid collapse with bool..?
	void setup(std::string name, bool _buildGroupSelector);
	void setup(bool _buildGroupSelector);

	bool bDoneSetup = false;// to ensure all setup process is done and avoid troubles if not after

	void startup();// must be called after setup (who is called after all group adds) to set initial states well

	void doCheckPresetsFoldersAreEmpty();// used on startup. check if all favorites preset are present, and creates folders and content if not

	//--

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
	int getPresetIndex(int groupIndex) const;
	int getPresetIndex(string groupName) const;

	int getGuiIndex(string name) const;// get index for a name of group
	string getPresetPath(string groupName, int presetIndex);// get path of a preset of a group

	string getGroupPath(string groupName);// get path of a group. for external monitor only
	string getGroupPath(int index);// get path of a group. for external monitor only

	//--

	// main data
	// stores added ofParameterGroup's to be handled by the engine

private:
	// main settings/parameters container
	// all parameters are contained into an ofParameterGroup
	std::vector<ofParameterGroup> groups;// container to store multiple groups. 
	std::vector<int> groupsSizes;// this is the amound of presets of each (index correlative) added group

	//--

	// preset selectors to each added group

public:
	// exposed to allow external callbacks
	std::vector<ofParameter<int>> PRESETS_Selected_Index;// one selector for each group

public:
	// API
	//--------------------------------------------------------------
	ofParameterGroup getSelectorsGroup()
	{
		// TODO:
		// could use a customized control group with importante parameters...
		//ofParameterGroup g{ "SelectorsGroup" };
		//for (int i = 0; i < groups.size(); i++) {
		//	g.add(PRESETS_Selected_Index[i]);
		//}
		//return g;

		return params_GroupsSelectors;
	}

private:
	std::vector<int> PRESETS_Selected_Index_PRE;// remember previous selector to reduce callbakcs
	ofParameterGroup params_GroupsSelectors{ "PRESET SELECTORS" };// group all group selectors indexes

	//--

	// select active group 
	// to show on randomize editor panel
	ofParameter<int> GuiGROUP_Selected_Index;// only this selected group will be showed on gui to edit
	void Changed_GuiGROUP_Selected_Index(int & index);
	std::vector<groupRandomizer> groupRandomizers;

	// selector for last group: is the main group link
	ofParameterGroup params_GroupMainSelector{ "GROUP_LINK" };

	//--

	ofParameter<int> GROUP_LINK_Selected_Index;// global group selector. this selector will control all linked groups

	// group main selector
	bool bBuildGroupSelector = true;// to allow auto build a group selector to combine all the added groups to the presets manager
	bool bAllowGroupSelector = true;// to allow disable main group. not allways we need it..
	int groupLinkSize = 10;// default ammount of presets we want to the group link
public:
	void setGroupLinkSize(int size) {// customize how many group link presets we want to create
		groupLinkSize = size;
	}
	void setEnableGroupLinkSelector(bool b) {// disable the use of main group selector. must call before setup. enabled by default
		bAllowGroupSelector = b;
	}

private:
	ofParameter<bool> bSplitGroupFolders{ "MODE SPLIT FOLDERS", true };//on this mode we split every group on his own sub folder
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
	ofParameter<bool> DONE_load;// easy callback to know (in ofApp) that preset LOAD is done 
	ofParameter<bool> DONE_save;// easy callback to know (in ofApp) that preset SAVE is done

	//--

	// C. easy trig-callback
	// used to get alerted when preset has not changed but we like to retrig something
	// in some situation we would like this feature:
	// 1. user clicked a preset box
	// 2. but to the same current loaded preset
	// 3. no need to reload the file settings
	// 4. but we want to use the user box click to trig something
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
	//	for (int i = 0; i < presetsManager.PRESETS_Selected_Index.size(); i++)
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
	//		string name = e.getName();
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

	//----
	//
	// API
	//
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
	//--------------------------------------------------------------
	glm::vec2 getPresetClicker_Position()
	{
		glm::vec2 p = glm::vec2(clicker_Pos.x, clicker_Pos.y);
		return p;
	}
	////--------------------------------------------------------------
	//void setPosition_DEBUG(int x, int y);// where to display if we get errors (ie: data files not found) on startup

	//----

	// keys

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
	//--------------------------------------------------------------
	void setModeKeySave(int key);// setup the key you have to hold for saving, default is OF_KEY_CONTROL
	void setModeKeySwap(int key);// setup the key you have to hold for swap, default is OF_KEY_ALT

	//----

	// undo engine

	// TODO:
	// should expand to all groups. here only habndles one group
private:
#ifdef INCLUDE_ofxUndoSimple
	ofxUndoSimple<std::string> undoStringParams;
	ofXml undoXmlParams;
	void undoRefreshParams();
	void undoStoreParams();
#endif

	//----

	// paths for settings

	// all folder names must go without ending with '/'
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

	//--

	// app settings for many params
private:
	void load_ControlSettings();
	void save_ControlSettings();

	//----

	// TODO:
	// memory mode

	// improve performance loading from xml objects from memory not xml files from hd
private:
	ofParameter<bool> MODE_MemoryLive;// when enabled all presets are handled from a memory vector to avoid lag of loading xml files from hd
public:
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
		groupRandomizers[groupIndex].randomizeDuration = t;
		groupRandomizers[groupIndex].randomizeDurationBpm = 60000.f / groupRandomizers[groupIndex].randomizeDuration;
	}
	//--------------------------------------------------------------
	void setRandomizerDurationShort(float t, int groupIndex = -1)
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex;
		groupRandomizers[groupIndex].randomizeDurationShort = t;
	}
	//--------------------------------------------------------------
	void setRandomizerBpm(float bpm, int groupIndex = -1)
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		ofLogNotice(__FUNCTION__) << "group: " << groupIndex;
		groupRandomizers[groupIndex].randomizeDurationBpm = bpm;
		// 60,000 ms (1 minute) / Tempo (BPM) = Delay Time in ms for quarter-note beats
		groupRandomizers[groupIndex].randomizeDuration = 60000.f / bpm;
		groupRandomizers[groupIndex].randomizeDurationShort = groupRandomizers[groupIndex].randomizeDuration / 2.f;
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
	void loadPreset(int p, int _indexGroup = -1);// load preset for each group by code from ofApp
	void savePreset(int p, int _indexGroup = -1);// save preset for each group by code from ofApp

	//--

public:
	//--------------------------------------------------------------
	void saveCurrentPreset(int groupIndex = -1) {
		if (groupIndex == -1) groupIndex = groups.size() - 1;

		ofLogNotice(__FUNCTION__) << "SAVE PRESET  group: " << groupIndex << " preset: " << PRESETS_Selected_Index[groupIndex].get();
		save(PRESETS_Selected_Index[groupIndex].get(), groupIndex);
	}

public:
	//--------------------------------------------------------------
	void saveCurrentPreset(string groupName) {
		int _groupIndex = -1;
		for (int i = 0; i < groups.size(); i++) {
			if (groups[i].getName() == groupName) _groupIndex = i;
		}
		if (_groupIndex != -1) {
			ofLogNotice(__FUNCTION__) << "SAVE PRESET from group: "<< groupName << " #" << _groupIndex << " preset: " << PRESETS_Selected_Index[_groupIndex].get();
			save(PRESETS_Selected_Index[_groupIndex].get(), _groupIndex);
		}
		else {
			ofLogError(__FUNCTION__) << "Can't found any group named: " << groupName << ". Save command failed!";
		}
	}

	// presets browsing by code from ofApp
	//--------------------------------------------------------------
	void load_Previous(int groupIndex = -1)// default if not defined, is the last one: main group link
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		groupIndex = (int)ofClamp(groupIndex, 0, groups.size() - 1);

		int _curr = PRESETS_Selected_Index[groupIndex];
		_curr--;
		_curr = (int)ofClamp(_curr, 0, groupsSizes[groupIndex] - 1);

		PRESETS_Selected_Index[groupIndex] = _curr;
	}
	//--------------------------------------------------------------
	void load_Next(int groupIndex = -1)// default if not defined, is the last one: main group link
	{
		if (groupIndex == -1) groupIndex = groups.size() - 1;
		groupIndex = (int)ofClamp(groupIndex, 0, groups.size() - 1);

		int _curr = PRESETS_Selected_Index[groupIndex];
		_curr++;
		_curr = (int)ofClamp(_curr, 0, groupsSizes[groupIndex] - 1);

		PRESETS_Selected_Index[groupIndex] = _curr;
	}

	//--

	// helpers for external layout or other workflow purposes

	//--------------------------------------------------------------
	int getCurrentPreset(int _group)// get index of selected preset on the group
	{
		int _presetIndex = -1;
		if (_group < groups.size()) _presetIndex = PRESETS_Selected_Index[_group];
		return _presetIndex;
	}

	//--------------------------------------------------------------
	int getAmountGroups()// how many groups we added on setup
	{
		if (groups.size() > 0) return groups.size();
		else return 1;// workaround to set default height before groups are added
	}

	//--------------------------------------------------------------
	int getAmountPresetsOfGroup(int g)// how many presets on the added group
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
	string getGroupsPaths(bool simplified = true)// get root paths for the preset files of the groups
	{
		string _names = "";

		if (!simplified)// full paths info
		{
			for (int i = 0; i < groups.size(); i++)
			{
				_names += getGroupPath(i);
				_names += "\n";
			}
		}
		else// simplified path info
		{
			if (!bPathDirCustom) _names += "/data/" + path_UserKit_Folder + "/\n";
			_names += path_PresetsFavourites + "/           > favourites\n";
			_names += path_PresetsStandalone + "/           > standalones\n";
			_names += "\n";

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
	string getGroupsPath()
	{
		string _names = "";
		_names = getGroupPath(0);// simpler: get first group bc all are in the same folder
		return _names;
	}

	//--------------------------------------------------------------
	void setVisible_GroupName(bool b)// disabler for minimal design
	{
		SHOW_GroupName = b;
	}

	//----

	// helper tools

	void doCloneRight(int groupIndex);// clone from selected preset of index group to all others to the right
	void doCloneAll(int groupIndex);// clone all presets from the current index group
	void doPopulateFavs(int groupIndex);// fast populate random presets of index group
	void doPopulateFavsALL();// fast populate random presets around all favs of index group

	void doSwap(int groupIndex, int fromIndex, int toIndex);

	//----

	//----
	//
	// API
	//
	//----

	// layout

public:
	//--------------------------------------------------------------
	void setVisible_Help(bool b)
	{
		SHOW_Help = b;
	}
	//--------------------------------------------------------------
	void setToggleVisible_Help()
	{
		SHOW_Help = !SHOW_Help;
	}
	//--------------------------------------------------------------
	void setPosition_GUI_ImGui(int x, int y)
	{
		ImGui_Position = ofVec2f(x, y);
	}
	//--------------------------------------------------------------
	void setVisible_GUI(bool b)
	{
		SHOW_ImGui = b;
		SHOW_Panel_Click = b;
	}
	//--------------------------------------------------------------
	void setToggleVisible_GUI()
	{
		SHOW_ImGui = !SHOW_ImGui;
		SHOW_Panel_Click = SHOW_ImGui;
	}
	//--------------------------------------------------------------
	void setVisible_GUI_ImGui(bool b)
	{
		SHOW_ImGui = b;
	}
	//--------------------------------------------------------------
	void setToggleVisible_GUI_ImGui()
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
		SHOW_Panel_Click = visible;
	}
	//--------------------------------------------------------------
	void setToggleVisible_PresetClicker()
	{
		SHOW_Panel_Click = !SHOW_Panel_Click;
	}
	//--------------------------------------------------------------
	bool getVisible_PresetClicker()
	{
		return SHOW_Panel_Click;
	}
	//--------------------------------------------------------------
	bool isVisible_PresetClicker()
	{
		return SHOW_Panel_Click;
	}
	//--------------------------------------------------------------
	float getGroupNamesWidth() {
		float _maxw = 0;
		for (int i = 0; i < groups.size(); i++)
		{
			float _w;
			string info = groups[i].getName() + "* ";
			if (myFont.isLoaded()) { _w = myFont.getStringBoundingBox(info, 0, 0).width; }
			else { _w = 200; }

			if (_w > _maxw) _maxw = _w;
		}
		return _maxw + 22;
	}

	//----

	// customize file paths

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

	// TODO: customize root path. this could be the user-kit path ??
	////--------------------------------------------------------------
	//void setPath_Root(string str)
	//{
	//	ofLogNotice(__FUNCTION__) << str;
	//	path_Root = str;
	//}

	//----

	// API

	// modes

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

	//----

public:
	ofParameter<int> PRESET_Selected_IndexMain;// main group preset selector (current)

private:
	int PRESET_Selected_IndexMain_PRE = -1;// used as callback

	//----

private:
	ofParameter<bool> MODE_Editor{ "MODE EDIT", true };// this mode improves performance disabling autosave, undo history..etc
	ofParameter<bool> SHOW_Panel_Click;// to allow include as toggle parameter into external gui
	ofParameter<bool> SHOW_ImGui;
	ofParameter<bool> SHOW_Panel_StandalonePresets;
	ofParameter<bool> SHOW_Panel_AllParameter;
	ofParameter<bool> SHOW_Panel_Randomizer;
	ofParameter<bool> SHOW_ImGui_PresetsParams;
	ofParameter<bool> SHOW_ImGui_Selectors;
	ofParameter<bool> SHOW_Help;
	ofParameter<bool> SHOW_Gui_AdvancedControl;
	ofParameter<bool> ENABLE_Keys;
	ofParameter<bool> bThemDark{ "THEME DARK", true };
	ofParameter<glm::vec2> Gui_Internal_Position;

private:
	bool SHOW_GroupName = true;// draws group name into clicker boxes panel
	bool bShowClickerInfo = true;

	//--

private:
	string helpInfo;// info text to display shortcuts or path settings
	void buildHelpInfo();

	//--

	// ImGui

public:
	void ImGui_Draw_Randomizers();
	void ImGui_Draw_MainPanel();
	void ImGui_Draw_Extra();
	void ImGui_Draw_GroupsSelectors();
	void ImGui_Draw_StandalonePresets();
	void ImGui_Draw_PresetParameters();

public:
	//void ImGui_Draw_Window();
	bool ImGui_Draw_Window();

	//-

#ifndef USE_IMGUI_EXTERNAL
	ofxImGui::Gui gui_ImGui;
	void ImGui_Setup();
	void ImGui_Draw_WindowBegin();
	void ImGui_Draw_WindowEnd();
#endif

	//-

	// layout

private:
	// preset clicker boxes matrix
	int cellSize = 80;// default box button size
	ofVec2f clicker_Pos;// default clicker position

private:
	// ImGui 
	ofParameter<glm::vec2> ImGui_Position;// ImGui browser panel position. 
	ofParameter<glm::vec2> ImGui_Size;// ImGui browser panel position. 

	//--

private:
	// mouse over gui handler
	// detects mouse interaction on ImGui to block other mouse actions like: easy camera... 
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

	//-----

	// standalone presets browser
	// located into 'archive/' folder, not from favourites presets folders
private:
	// multi-group files
	vector <std::vector<std::string>> standaloneFileNames;// all the group standalone preset names
	vector <std::vector<ofFile>> standaloneFiles;// all the group standalone preset files
	vector <int> standaloneFileIndex;// selected standalone preset file of the group
	vector <std::string> standaloneNamePresetDisplay;
	// engine handler
	bool bFilesError = false;
	ofParameter<bool> MODE_StandalonePresets_NEW;
	string inputText_NEW = "";//user input text
	//string inputText_TEMP = "";

private:
	void doStandalonePresetsBuild(int groupIndex = -1);// standalone presets splitted from favourites presets
	void doStandalonePresetLoad(string name, int groupIndex = -1);
	void doStandalonePresetSave(string name, int groupIndex = -1);
	bool doStandalonePresetsRefresh(int groupIndex = -1);
	void doStandalonePresetsBuildFromFavs(int groupIndex = -1);// save all favourites-presets to the standalone-presets (archive) folder

	//----

	// API

	// helpers to easy integrate importane controls into external gui's

private:
	// to expose basic controls to allow use on external gui
	ofParameterGroup params_Controls{ "PRESETS MANAGER" };

public:
	// this are usefull parameters to use in our ofAPp/projects/addons gui's
	//--------------------------------------------------------------
	ofParameterGroup getParamsControls() {
		//cout << "displayNameUserKit: " << displayNameUserKit << endl;// TODO: not refreshing well on startup..

		params_Controls.clear();
		//params_Controls.setName(displayNameUserKit);
		//params_Controls.setName("OVERLAY");

        // Windows
        //params_Controls.add(getParamsPresetSelectors());
        // macOS casting...
        ofParameterGroup g1;
        g1 = getParamsPresetSelectors();
        params_Controls.add(g1);

        // Windows
		//params_Controls.add(getParamsRandomizers());
        // macOS casting...
        ofParameterGroup g2;
        g2 = getParamsRandomizers();
        params_Controls.add(g2);

        params_Controls.add(SHOW_Panel_Click);
		params_Controls.add(MODE_Editor);

		return params_Controls;
	}
	//--------------------------------------------------------------
	ofParameterGroup getParamsPresetSelectors() {// selectors index to all the added groups
		return params_GroupsSelectors;
	}
	//--------------------------------------------------------------
	ofParameterGroup getParamsRandomizers() {// important settings to handle randomizers
		ofParameterGroup _g{ "RANDOMIZERS" };
		for (int i = 0; i < groups.size(); i++)
		{
            // Windows
            //_g.add(groupRandomizers[i].getParamsRandomizers());
            // macOS casting...
            ofParameterGroup g;
            g = groupRandomizers[i].getParamsRandomizers();
            _g.add(g);

			}
		return _g;
	}

	//----

	// keys

public:
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);

private:
	void addKeysListeners();
	void removeKeysListeners();

	void removeMouseListeners();// TODO: fix easy remover..

	vector<vector<int>> keys;// queued trigger keys for each group ? (all presets) (size of)
	bool bKeys;// enabled keys
	bool keysNotActivated;

	// save keys
	int modeKeySave;// save mod key
	bool bKeySave;// save mod key state

	// swap selected preset with the currently clicked (i.e: 4 -> 7  &  4 <- 7)
	int modKeySwap;// swap mod key 
	bool bKeySwap;// swap mod key state

public:
	// switch on or off the control with the keys
	void setToggleKeysControl(bool active);

	//--

private:
	bool ENABLE_KeysArrowBrowse = true;// allow browse presets by arrows keys by default
public:
	//--------------------------------------------------------------
	void setEnableKeysArrowBrowse(bool b)
	{
		ENABLE_KeysArrowBrowse = b;
	}

	//----

	// mouse

private:
	void mousePressed(int x, int y);
	bool lastMouseButtonState;

	//----

private:
	// font to label clicker boxes
	ofTrueTypeFont myFont;
	string myTTF;// gui font for all gui theme
	int sizeTTF;

	//----

	// callbacks

private:
	void Changed_Control(ofAbstractParameter &e);
	void Changed_UserKit(ofAbstractParameter &e);

	//-

private:
	bool DISABLE_CALLBACKS_SELECTORS = false;// to avoid multiple calls on multiple presets selector engine
	bool DISABLE_CALLBACKS = true;// to avoid startup crashes and objects are not initialized properly
	// updating some params before save will trigs also the group callbacks
	// so we disable this callbacks just in case params updatings are required

	//--

public:
	ofParameterGroup params_Control;// to use on external gui

private:
	ofParameter<bool> autoSave;// auto save current preset when user clicks to another preset. almost enabled on edit mode
	ofParameter<bool> autoLoad;// auto load the user clicked preset. almost always true

	// internal groups
	ofParameterGroup params_UserKitSettings;
	ofParameterGroup params_Gui;
	ofParameterGroup params_Options;
	ofParameterGroup params_Randomizer;
	ofParameterGroup params_Custom;

	//----

	// custom path for preset favourites
private:
	ofParameter<bool> bPathDirCustom;
	ofParameter<string> pathDirCustom;
public:
	// set custom path
	void doFileDialogProcessSelection(ofFileDialogResult openFileResult);
	void buildCustomUserKit();
	void buildDefaultUserKit();

	//--

	// TODO:
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
