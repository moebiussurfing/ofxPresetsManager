
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

///---
///
///	TODO:
///
///
///	++	ImGui: switch out of ofxImGui to simpleImGui
///			it seems that when using more than one instance it crashes on exit and windows collides
///			improve ImGui trees collapse and sizes
///	+	repair autosave timer. exclude log
///	++	performance: check memory_mode
///			check disable autosave
///			check 
///
///	+	add workflow to not collide manual preset click with randomizer timer
///	++	randomize editor preset
///			add limit range min/max to randomize
///			do nesting toggles to improve view. create a group for related toggles..	
///			clone using editor toggles to avoid clone disabled toggle params
///			mode state to allow overwrite only enabled toggle params
///	++	add ofxUndo to randomizers
///	+	add define to disable all browser/ImGui/randomize stuff to make addon minimal expression 
///	+	could make tween when changing params 
///
///	BUG:
///
///	?	there's a problem when CheckFolder or setPath_GlobalFolder are something like "myApp/ofxPresetsManager"
///
///---


#pragma once

//----------------------------------------------
//
//	DEFINES
//
#define MODE_ImGui_EXTERNAL			//MODE_ImGui_EXTERNAL. this must be defined here and (not only) in ofApp (too)!!
#define INCLUDE_GUI_IM_GUI				//ImGui & browser system
#define INCLUDE_IMGUI_CUSTOM_FONT		//customize ImGui font
//#define USE_ofxImGuiSimple			//TEST alternative addon
//
//#define TIME_SAMPLE_MEASURES			//measure performance ofxTimeMeasurements
#define INCLUDE_DEBUG_ERRORS			//debug errors
//#define DEBUG_randomTest				//uncomment to debug randimzer. comment to normal use. if enabled, random engine stops working
//#define DEBUG_BLOCK_SAVE_SETTINGS		//disable save settings//enable this bc sometimes there's crashes on exit
//
//----------------------------------------------


#include "ofMain.h"

//--

#ifdef INCLUDE_GUI_IM_GUI
#define INCLUDE_RANDOMIZER
#endif
//BUG: seems to make exceptions when multiple ImGui/ofxPresetsManager instances...

//internal control
#include "ofxGui.h"

//--

//browser system
#ifdef INCLUDE_GUI_IM_GUI
#ifdef USE_ofxImGuiSimple
#include "ofxImGuiSimple.h"
#include "Helpers.h"
#else
#include "ofxImGui.h"
#endif
#endif

//--

//optional to debug not located files or others
#ifdef INCLUDE_DEBUG_ERRORS
#include "ofxDEBUG_errors.h"
#endif

//--

//optional to debug performance or delay when loading files or presets on hd or memory modes
#ifdef TIME_SAMPLE_MEASURES
#include "ofxTimeMeasurements.h"
#else
#define TS_START
#define TS_STOP
#define TSGL_START
#define TSGL_STOP
#endif

//--

//-------------------------------

#pragma mark - DEFINE_DATA_TYPES

//-

//TODO
//#define NUM_OF_PRESETS 8//delete this

//only one it's implemented! can't add more than one parameter group!
#define NUM_MAX_GUIS 1

//---

class ofxPresetsManager : public ofBaseApp
{

private:

#ifdef INCLUDE_DEBUG_ERRORS
	ofxDEBUG_errors errorsDEBUG;
#endif

	//-

	//settings paths
	std::string groupName;//get from ofParameterGroup name
	//TODO:
	//std::string groupName2;//get from ofParameterGroup name

	//all folder names must go without '/'
	std::string path_GLOBAL_Folder;//top parent folder for all other subfolders
	std::string path_Control;//path for app state session settings
	std::string path_Kit_Folder;//path for kit of favorite presets. live kit
	std::string path_PresetsFolder;//path for browse other presets. archive kit

	ofParameter<std::string> path_PresetsFolder_Custom;//path for browse other presets. archive kit
	ofParameter<bool> bUseCustomPath{ "bUseCustomPath", false };
	std::string path_Prefix;//to add to file names

	//TODO:
	//must store settings
	//customize browser path-free (out of global path)
	bool bCustomBrowserPath = false;
	std::string path_BrowserPathFree = "";

	std::string browser_PresetName;

	std::string gui_LabelName = "NO_NAME";//default gui panel name

	//-

private:

	//app settings

	//this loads selected preset, autosave/autoload...gui states
	void load_ControlSettings();
	//public://TEST crash?
	void save_ControlSettings();

	//-

private:
#ifndef INCLUDE_GUI_IM_GUI
	ofxPanel gui_InternalControl;
#endif

private:
#ifdef INCLUDE_RANDOMIZER

	ofParameter<bool> MODE_MemoryLive;
	ofParameter<bool> loadToMemory;
	ofParameter<bool> saveFromMemory;
	void load_AllKit_ToMemory();
	void save_AllKit_FromMemory();

	//randomizer
public:
	ofParameter<bool> ENABLE_RandomizeTimer;//play randomizer
	ofParameter<bool> bRandomizeSelect;
private:
	//ofParameterGroup params_Randomizer;
	ofParameter<bool> MODE_DicesProbs;
	ofParameter<bool> MODE_LatchTrig;//this mode trigs preset but goes back to preset 1 after duration timer
	bool bLatchRun = false;
	ofParameter<bool> bResetDices;
	ofParameter<int> randomizedDice;//to test
	//ofParameter<string> _totalDicesStr;
public:
	//ofParameter<float> randomizeSpeedF;
	ofParameter<int> randomizeDuration;
	ofParameter<int> randomizeDurationShort;
private:
	int randomizeSpeed;//real time dureation
	uint32_t randomizerTimer;
	int randomize_MAX_DURATION = 5000;
	int randomize_MAX_DURATION_SHORT = 1000;
	vector<ofParameter<int>> presetsRandomFactor;//probability of every preset
	vector<ofParameter<bool>> presetsRandomModeShort;//mode short for ebvery preset
	vector<int> randomFactorsDices;
	void setupRandomizer();//engine to get a random between all posible dices (from 0 to numDices) and then select the preset associated to the resulting dice.
	void doRandomizeWichSelectedPreset();//randomize wich preset (usually 1 to 8) is selected (not the params of the preset)
	void doResetDices();//reset all probs to 0
	int numDices;//total dices summing the prob of any preset probability (PROB1 + PROB2 + ...)

	int timerRandomizer;
#endif
public:
	ofParameter<bool> bRandomizeEditor;
private:
	ofParameter<bool> bRandomizeEditorAll;
	ofParameter<bool> bRandomizeEditorNone;
	ofParameter<bool> bRandomizeEditorPopulateFavs;

	//TODO:
	//system to select what params of current selected preset to: clone, randomize etc
	void setupEditor();
	vector<ofParameter<bool>> editorPresets;
	ofParameterGroup params_Editor;
	ofParameterGroup params_Editor_Toggles;
	void addGroupToEditor(ofParameterGroup& group);
	void Changed_Params_Editor(ofAbstractParameter &e);
	void doRandomizeEditor();//randomize params of current selected preset
	void doRandomizeEditorGroup(ofParameterGroup& group);//randomize params of current selected preset

	////TODO:
	//ofParameterGroup group_TARGET;
	//void addGroup_TARGET(ofParameterGroup &g);	
	//vector<ofParameterGroup> groupsMem;

	//data
private:
	//TODO:
	//should use keys[i].size() instead of this:
	vector<ofXml> settingsArray;
	//ofXml settingsArray[NUM_OF_PRESETS];//fixed lenght mode..

	//--

#pragma mark - CALLBACKS
private:

	bool DISABLE_CALLBACKS = true;//to avoid startup crashes and objects are not initialized properly
	//updating some params before save will trigs also the group callbacks
	//so we disable this callbacks just in case params updatings are required
	//in this case we will need to update gui position param

	bool bIsDoneLoad = false;

	//--

#pragma mark - OF

public:
	ofxPresetsManager();
	~ofxPresetsManager();

	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void exit();
	void windowResized(int w, int h);

	void drawPresetClicker();//user clickeable box panel preset selector

	//--

#pragma mark - API

	//---------------
	//
	//	API
	//
	//-----

	//--

private:
	//TODO:
	//BUG:
	//not working
	void add(ofParameterGroup params, int numPresets = 8);//add a param group for preset saving and how many presets on favs

public:

	void add(ofParameterGroup params, initializer_list<int> keysList);//adds and define keys to trig presets too

	void setup();//must be called after params group has been added!
	void setup(std::string name);//optional to set gui panel name header label
	void startup();//must be called after setup to se initial states

	//-

	//easy callback to get from ofApp if required
public:
	ofParameter<bool> DONE_load;//easy callback to know (in ofApp) that preset LOAD is done 
	ofParameter<bool> DONE_save;//easy callback to know (in ofApp) that preset SAVE is done

	//--

public:
	//--------------------------------------------------------------
	bool isDoneLoad()//easy callback to ofApp integration 
	{
		if (bIsDoneLoad)
		{
			bIsDoneLoad = false;
			return true;
		}
		return false;
	}

	////to check in update() as callback
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

	//--------------------------------------------------------------
	void setPosition_DEBUG(int x, int y);//where to display if we get errors (ie: data files not found) on startup

	//--------------------------------------------------------------
	float getPresetClicker_Width()
	{
		return clicker_cellSize * (keys[0].size() + 2);
	}
	//--------------------------------------------------------------
	float getPresetClicker_BoxSize()
	{
		return clicker_cellSize;
	}
	//--------------------------------------------------------------
	float getPresetClicker_Height()
	{
		return clicker_cellSize;
	}

	//-

	void setModeKeySave(int key);//set the key you have to hold for saving, default is OF_KEY_CONTROL
	void setModeKeySwap(int key);//set the key you have to hold for swap, default is OF_KEY_ALT

	//set keys active
	//--------------------------------------------------------------
	void setEnableKeys(bool active)
	{
		bKeys = active;
		ENABLE_Keys = active;
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

	//-

	//--------------------------------------------------------------
	void load_Next()
	{
		PRESET_selected++;
		if (PRESET_selected > numPresetsFavorites)
		{
			PRESET_selected = numPresetsFavorites;
		}
	}

	//--------------------------------------------------------------
	void load_Previous()
	{
		PRESET_selected--;
		if (PRESET_selected < 1)
		{
			PRESET_selected = 1;
		}
	}

	void loadPreset(int p);//load preset by code from ofApp

	//--------------------------------------------------------------
	int getNumPresets()
	{
		return numPresetsFavorites;
	}

	//--------------------------------------------------------------
	int getCurrentPreset()//get index of selected preset
	{
		return PRESET_selected;
	}

	//-

	//--------------------------------------------------------------
	void setVisible_GroupName(bool b)//disabler for minimal design
	{
		SHOW_GroupName = b;
	}

	//--

	//randomizer helpers
public:
	//--------------------------------------------------------------
	void setModeRandomizerPreset(bool b)//play randomizer timer
	{
		ENABLE_RandomizeTimer = b;
	}
	//--------------------------------------------------------------
	void setToggleRandomizerPreset()//toggle randomizer timer
	{
		ENABLE_RandomizeTimer = !ENABLE_RandomizeTimer;
	}
	//--------------------------------------------------------------
	void doRandomizePresetFromFavs()//trig randomize and select one of the favs presets
	{
		bRandomizeSelect = true;
	}
	//--------------------------------------------------------------
	void doRandomizePresetSelected() {//randomize params of current selected preset
		doRandomizeEditor();
	}
	////--------------------------------------------------------------
	//void populateFavs()//create 
	//{
	//	doPopulateFavs();
	//}

	void doCloneRight(int pIndex);//clone from selected preset to all others to the right
	void doCloneAll();//clone all presets from the current selected
	void doPopulateFavs();//fast populate random presets around all favs

	//--

private:
	//draws group name into clicker boxes panel
	bool SHOW_GroupName = true;

	//--

	//used when preset has not changed but we like to retrig
	bool bMustTrig = false;

public:
	//--------------------------------------------------------------
	bool isMustTrig()//trig on select preset or not
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

	//-

	//browser
private:
#ifdef INCLUDE_GUI_IM_GUI
	//load presets from preset folder, not from favorites presets folders
	void browser_PresetLoad(string name);
	void browser_PresetSave(string name);
	bool browser_FilesRefresh();
	void browser_Setup();
	float _w;
#endif

	//-

#pragma mark - GUI

	//-

public:

	//API

	//BUG: 
	//workflow 
	//to solve auto load fail because the sorting of xml autoSave after preset selector tag
	////(optional) on startup: called at the end of your ofApp setup() 
	//--------------------------------------------------------------
	void refresh()
	{
		windowResized(ofGetWidth(), ofGetHeight());

		////browser
		//browser_FilesRefresh();

		//-

		ofLogNotice(__FUNCTION__);
		PRESET_selected_PRE = -1;
		PRESET_selected = PRESET_selected;
		ofLogNotice(__FUNCTION__) << "Preset " << PRESET_selected;
	}

	//--------------------------------------------------------------
	void setVisible_Help(bool b)
	{
		debugClicker = b;
	}

#ifdef INCLUDE_GUI_IM_GUI
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
#endif

#ifndef INCLUDE_GUI_IM_GUI
	//--------------------------------------------------------------
	void setPosition_GUI_InternalControl(int x, int y)
	{
		guiPos_InternalControl = ofVec2f(x, y);
		gui_InternalControl.setPosition(guiPos_InternalControl.x, guiPos_InternalControl.y);
	}
#endif

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
		clicker_cellSize = _cellSize;
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
	void setPath_ControlSettings(string str)//for the session states settings
	{
		ofLogNotice(__FUNCTION__) << str;
		path_Control = str;
	}
	void setPath_KitFolder(string folder);//path folder for favorite/live presets
	void setPath_PresetsFolder(string folder);//path folder for kit for the browser
	void setPath_GlobalFolder(string folder);//path for root container folder

	//--------------------------------------------------------------
	void setAutoLoad(bool b)
	{
		autoLoad = b;
	}
	//--------------------------------------------------------------
	void setAutoSave(bool b)
	{
		autoSave = b;
	}
	//--------------------------------------------------------------
	void setAutoSaveTimer(bool b)
	{
		bAutosaveTimer = b;
	}

	//--

public:
	ofParameter<int> PRESET_selected;//main current preset selector

private:
	int PRESET_selected_PRE = -1;//used as callback

	//--

public:
	//switch on or off the control with the keys
	void setToggleKeysControl(bool active);

	//-

	//engine
private:

	//save to a preset
	void save(int presetIndex, int guiIndex = 0);
	void save(int presetIndex, string guiName);

	//loads an already saved preset
	void load(int presetIndex, int guiIndex = 0);
	void load(int presetIndex, string guiName);

	//get the last loaded preset
	int getPresetIndex(int guiIndex) const;
	int getPresetIndex(string guiName) const;

	int getGuiIndex(string name) const;
	string getPresetName(string guiName, int presetIndex);

	//--

	ofParameter<bool> SHOW_Gui_AdvancedControl;

	bool debugClicker = true;

	//-

	//ImGui
#ifdef INCLUDE_GUI_IM_GUI

	//ImGui pure content
public:
	void ImGui_Draw_Basic(ofxImGui::Settings &settings);
	void ImGui_Draw_Browser(ofxImGui::Settings &settings);
	void ImGui_Draw_Advanced(ofxImGui::Settings &settings);

	void ImGui_Theme();
	void ImGui_FontCustom();

	//-

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
#endif

	ofParameter<glm::vec2> ImGui_Position;//ImGui browser panel position. 

	ofParameter<bool> MODE_Browser_NewPreset;

	//-

private:
	ofParameter<bool> bImGui_mouseOver;
	bool bImGui_mouseOver_PRE;
	bool bMouseOver_Changed = false;

public:
	////--------------------------------------------------------------
	//bool isMouseOver()
	//{
	//	bool b;
	//	if (!SHOW_ImGui) b = false;
	//	else b = bImGui_mouseOver;
	//	return b;
	//}

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

	//browser presets
	//bool MODE_newPreset = false;
	string textInput_New = "";//user input text

	//files
	std::vector<std::string> fileNames;
	std::vector<ofFile> files;
	int currentFile = 0;
	string textInput_temp = "";
	bool bFilesError = false;

	//-------

	//helpers

public:
	//--------------------------------------------------------------
	void doGetFavsToFilesBrowser()//save all favorites presets to the browser (archive) folder
	{
		ofLogNotice(__FUNCTION__);

		//browser path
		string browser_path;
		browser_path = path_GLOBAL_Folder + "/" + path_PresetsFolder;

		//browser number of files

		//iterate all presets
		for (int i = 0; i < numPresetsFavorites; i++)
		{
			std::string pathSrc;
			std::string pathDst;

			pathSrc = getPresetName(groups[0].getName(), i);
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

		//refresh files
		browser_FilesRefresh();
	}

#endif//end browser

	//----

private:

	//layout
	ofVec2f guiPos_InternalControl = ofVec2f(500, 500);
	int clicker_cellSize = 80;
	ofVec2f clicker_Pos = ofVec2f(500, 500);

	//--

	//A. ofParameterGroup
	std::vector<ofParameterGroup> groups;
	//to store multiple group targets. 
	//when using only one ofParameterGroup, there's only one group element!
	//ofParameterGroup params_gui;

	//--

	std::vector<int> lastIndices;//? seems to be the size of any group or:
	//? this seems to be the last selected of any group(?)
	//TODO:
	//lastIndices it's the gui box clicked only, not important.. ?

	std::vector<int> presetsOnGroup;//?? this seems to be the number of presets of each added group(?)

	//--

public:
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);

private:
	void addKeysListeners();
	void removeKeysListeners();

	bool bKeys;//enabled keys
	vector<vector<int>> keys;//trigger keys for all presets
	bool keysNotActivated;

	//save
	int modeKeySave;//save mod key
	bool bKeySave;//save mod key state

	//swap selected preset with the currently clicked (i.e: 4 -> 7  &  4 <- 7)
	int modKeySwap;//swap mod key 
	bool bKeySwap;//swap mod key state

	void mousePressed(int x, int y);
	bool lastMouseButtonState;

	std::vector<int> newIndices;//? this seems to be the number of the groups(? )

	int numPresetsFavorites;//amount of box-clickable handled presets on current favorites/kit [8]

	//---

private:
	void doLoad(int pIndex);//engine loader. not used? bc bug on bLoad trigger..
	void doSave(int pIndex);//engine saver starting at 0

	//-

private:
	//font to label clicker boxes
	ofTrueTypeFont myFont;
	string myTTF;//gui font for all gui theme
	int sizeTTF;

	//-

	//callback
private:
	void Changed_Params_Control(ofAbstractParameter &e);

	//-

public:
	ofParameter<bool> SHOW_ClickPanel;

private:
	ofParameterGroup params_Control;

#ifdef INCLUDE_GUI_IM_GUI
public:
	ofParameter<bool> SHOW_ImGui;
#endif

private:
	ofParameter<bool> bSave;
	//ofParameter<bool> bLoad;

	ofParameter<bool> autoSave;
	ofParameter<bool> autoLoad;

	ofParameter<bool> bCloneRight;
	ofParameter<bool> bCloneAll;

	//internal groups
	ofParameterGroup params_Favorites;
	ofParameterGroup params_Gui;
	ofParameterGroup params_Options;
	ofParameterGroup params_Tools;
	ofParameterGroup params_Randomizer;
	ofParameterGroup params_Custom;

	ofParameter<glm::vec2> Gui_Internal_Position;
	ofParameter<bool> ENABLE_Keys;

	//--

	//timer autosave
	ofParameter<bool> bAutosaveTimer;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 9000;

	//--

	bool ENABLE_KeysArrowBrowse = true;//allow browse by arrows keys by default

public:
	//--------------------------------------------------------------
	void setEnableKeysArrowBrowse(bool b)
	{
		ENABLE_KeysArrowBrowse = b;
	}

private:
	//--------------------------------------------------------------
	void CheckFolder(string _path)
	{
		ofLogNotice(__FUNCTION__) << _path;

		ofDirectory dataDirectory(ofToDataPath(_path, true));

		//check if folder path exist
		if (!dataDirectory.isDirectory())
		{
			ofLogError(__FUNCTION__) << "FOLDER NOT FOUND! TRYING TO CREATE...";

			//try to create folder
			bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));

			//debug if creation has been succeded
			if (b) ofLogNotice(__FUNCTION__) << "CREATED '" << _path << "'  SUCCESSFULLY!";
			else ofLogError(__FUNCTION__) << "UNABLE TO CREATE '" << _path << "' FOLDER!";
		}
		else
		{
			ofLogNotice(__FUNCTION__) << "OK! LOCATED FOLDER: '" << _path << "'";//nothing to do
		}
	}
};
