
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
///	++		add multiple groups engine. look into ofxGuiPresets fron #npisanti
///	+++		lock (by toggle) params that we want to ignore on changing presets
///				can be done enabling/disabling serializable for each param with a group of toggles
///	++		performance: check memory_mode
///				check workflow: when playing randomizer disable autosave
///				check disable autosave
///	+		repair autosave timer. exclude log
///	++		preset mini engine. ABC dropdown list for randomizers
///	++		randomize editor preset
///				add limit range min/max to randomize
///				do nesting toggles to improve view. create a group for related toggles..	
///				clone using editor toggles to avoid clone disabled toggle params
///				mode state to allow overwrite only enabled toggle params
///	+		add workflow to not collide manual preset click with randomizer timer
///	+		add define to disable all browser/ImGui/randomize stuff to make addon minimal expression 
///	+		could make tween when changing params 
///
///	BUG:	
///
///	?		there's a problem when CheckFolder or setPath_GlobalFolder are something like "myApp/ofxPresetsManager" ?
///
///---


#pragma once

//----------------------------------------------

//	DEFINES

//#define MODE_ImGui_EXTERNAL			//MODE_ImGui_EXTERNAL. this must be defined here and (not only) in ofApp (too)!!
#define INCLUDE_GUI_IM_GUI				//ImGui & browser system
#define INCLUDE_IMGUI_CUSTOM_FONT		//customize ImGui font
//#define USE_ofxImGuiSimple			//TEST alternative addon
#define INCLUDE_ofxUndoSimple			//undo engine to store after randomize preset parameters (& recall)

//#define INCLUDE_PERFORMANCE_MEASURES	//measure performance ofxTimeMeasurements
#define INCLUDE_DEBUG_ERRORS			//debug errors
//#define DEBUG_randomTest				//uncomment to debug randimzer. comment to normal use. if enabled, random engine stops working
//#define DEBUG_BLOCK_SAVE_SETTINGS		//disable save settings//enable this bc sometimes there's crashes on exit

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

#ifdef INCLUDE_ofxUndoSimple
#include "ofxUndoSimple.h"
#endif

//--

//optional to debug not located files or others
#ifdef INCLUDE_DEBUG_ERRORS
#include "ofxDEBUG_errors.h"
#endif

//--

//optional to debug performance or delay when loading files or presets on hd or memory modes
#ifdef INCLUDE_PERFORMANCE_MEASURES
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
	//--

private:
#ifdef INCLUDE_ofxUndoSimple
	ofxUndoSimple<std::string> undoStringParams;
	ofXml xmlParams;
	void undoRefreshParams();
	void undoStoreParams();
#endif

	//--

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

	//-

	//improve performance loading from memory nor xml files
private:
	ofParameter<bool> loadToMemory;
	ofParameter<bool> saveFromMemory;
	void load_AllKit_ToMemory();
	void save_AllKit_FromMemory();
	ofParameter<bool> MODE_MemoryLive;//when enabled all presets are handled from a memory vector to avoid lag of loading xml files
public:
	void setModeMemoryPerformance(bool b) {
		MODE_MemoryLive = b;
	}

	//-

	//randomizer
#ifdef INCLUDE_RANDOMIZER//now cant' be disabled
public:
	ofParameter<bool> PLAY_RandomizeTimer;//play randomizer
	ofParameter<bool> bRandomizeIndex;
private:
	//ofParameterGroup params_Randomizer;
	ofParameter<bool> MODE_DicesProbs;
	ofParameter<bool> MODE_LatchTrig;//this mode trigs the preset but goes back to preset 0 after duration timer
	bool bLatchRun = false;
	ofParameter<bool> bResetDices;
	ofParameter<int> randomizedDice;//to test
	//ofParameter<string> _totalDicesStr;
public:
	//ofParameter<float> randomizeSpeedF;//speed scaler. not used
	ofParameter<int> randomizeDuration;
	ofParameter<int> randomizeDurationShort;
	ofParameter<float> randomizeDurationBpm;
private:
	int randomizeSpeed;//real time dureation
	uint32_t randomizerTimer;
	int randomize_MAX_DURATION = 6000;
	int randomize_MAX_DURATION_SHORT = 6000/2.;
	vector<ofParameter<int>> presetsRandomFactor;//probability of every preset
	vector<ofParameter<bool>> presetsRandomModeShort;//mode short for ebvery preset
	vector<int> randomFactorsDices;
	void setupRandomizer();//engine to get a random between all posible dices (from 0 to numDices) and then select the preset associated to the resulting dice.
	void doRandomizeWichSelectedPreset();//randomize wich preset (usually 1 to 8) is selected (not the params of the preset)
	int doRandomizeWichSelectedPresetCheckChanged();
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
	void addGroupToEditor(ofParameterGroup& group);//queue all contained params inside the paramGroup and nested too
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
	vector<ofXml> presetsXmlArray;
	//ofXml presetsXmlArray[NUM_OF_PRESETS];//fixed lenght mode..

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
		if (ENABLE_Keys != active) {//avoid if state not need to change
			ENABLE_Keys = active;
		}
		if (bKeys != active) {//avoid if state not need to change
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

	//--------------------------------------------------------------
	std::string getGroupName()//get the paramGroup being managed here. Useful when using more than one ofxPresetsManager instances
	{
		return groups[0].getName();
	}

	//--

	//randomizer helpers
public:
	//--------------------------------------------------------------
	void setPlayRandomizerTimer(bool b)//play randomizer timer
	{
		PLAY_RandomizeTimer = b;
	}
	//--------------------------------------------------------------
	void setToggleRandomizerPreset()//toggle randomizer timer
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
		//60,000 ms (1 minute) / Tempo (BPM) = Delay Time in ms for quarter-note beats
		randomizeDuration = 60000.f / bpm;
		randomizeDurationShort = randomizeDuration / 4.f;
	}
	//--------------------------------------------------------------
	void doRandomizePresetFromFavs()//trig randomize and select one of the favs presets
	{
		bRandomizeIndex = true;
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
	void setPath_GlobalFolder(string folder);//path for root container folder. must be called before setup()!
	void setPath_KitFolder(string folder);//path folder for favorite/live presets
	void setPath_PresetsFolder(string folder);//path folder for kit for the browser
	void setPath_ControlSettings(string str)//for the session states settings
	{
		ofLogNotice(__FUNCTION__) << str;
		path_Control = str;
	}

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
	void ImGui_Draw_Content(ofxImGui::Settings &settings);
	void ImGui_Draw_Basic(ofxImGui::Settings &settings);
	void ImGui_Draw_Browser(ofxImGui::Settings &settings);
	void ImGui_Draw_Randomizers(ofxImGui::Settings &settings);
	void ImGui_Draw_PresetPreview(ofxImGui::Settings &settings);

	//--

#ifndef USE_ofxImGuiSimple
	//ImGui theme
	void ImGui_FontCustom();
	void ImGui_ThemeMoebiusSurfing();
#endif

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
	ofParameter<glm::vec2> ImGui_Size;//ImGui browser panel position. 

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
	vector<vector<int>> keys;//queued trigger keys for each group ? (all presets) (size of)
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

	//--

private:
	//check if a folder path exist and creates one if not
	//many times when you try to save a file, this is not possible and do not happens bc the container folder do not exist
	//--------------------------------------------------------------
	void CheckFolder(string _path)
	{
		ofLogNotice(__FUNCTION__) << _path;

		////workaround to avoid error when folders are folder/subfolder
		//auto _fullPath = ofSplitString(_path, "/");
		//for (int i = 0; i < _fullPath.size(); i++) {
		//	ofLogNotice(__FUNCTION__) << ofToString(i) << " " << _fullPath[i];
		//}

		ofDirectory dataDirectory(ofToDataPath(_path, true));// /bin/data/

		//check if folder path exist
		if (!dataDirectory.isDirectory())
		{
			ofLogError(__FUNCTION__) << "FOLDER NOT FOUND! TRYING TO CREATE...";

			//try to create folder
			//bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));
			bool b = dataDirectory.createDirectory(ofToDataPath(_path, true), false, true);
			//added enable recursive to allow create nested subfolders if required

			//debug if creation has been succeded
			if (b) ofLogNotice(__FUNCTION__) << "CREATED '" << _path << "'  SUCCESSFULLY!";
			else ofLogError(__FUNCTION__) << "UNABLE TO CREATE '" << _path << "' FOLDER!";
		}
		else
		{
			ofLogNotice(__FUNCTION__) << "OK! LOCATED FOLDER: '" << _path << "'";//nothing to do
		}
	}
	//--------------------------------------------------------------
	void CheckAllFolder()
	{
		CheckFolder(path_GLOBAL_Folder);

		string _path;
		_path = path_GLOBAL_Folder + "/" + path_PresetsFolder;
		CheckFolder(_path);
		_path = path_GLOBAL_Folder + "/" + path_Kit_Folder;
		CheckFolder(_path);
		_path = path_GLOBAL_Folder + "/" + path_Control;
		CheckFolder(_path);
	}

	//--

#ifdef INCLUDE_GUI_IM_GUI			
	//my custom ImGui helpers
	////https://github.com/ocornut/imgui/issues/1537
	//--------------------------------------------------------------
	bool AddBigButton(ofParameter<bool>& parameter, float h)//button but using a bool not void param
	{
		auto tmpRef = parameter.get();
		auto name = ofxImGui::GetUniqueName(parameter);

		float w;
		w = ImGui::GetWindowWidth()*0.9f;

		ImGuiStyle *style = &ImGui::GetStyle();

		const ImVec4 colorButton = style->Colors[ImGuiCol_Button];//better for my theme
		const ImVec4 colorHover = style->Colors[ImGuiCol_Button];
		const ImVec4 colorActive = style->Colors[ImGuiCol_ButtonActive];
		//const ImVec4 colorButton = style->Colors[ImGuiCol_ButtonHovered];//better for default theme
		//const ImVec4 colorHover = style->Colors[ImGuiCol_ButtonHovered];
		//const ImVec4 colorActive = style->Colors[ImGuiCol_ButtonActive];

		ImGui::PushID(name);
		ImGui::PushStyleColor(ImGuiCol_Button, colorButton);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorHover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorActive);

		if (ImGui::Button((name), ImVec2(w, h)))
		{
			ofLogNotice(__FUNCTION__) << name << ": BANG";

			tmpRef = true;
			parameter.set(tmpRef);
		}

		ImGui::PopStyleColor(3);
		ImGui::PopID();

		return true;//not used
	}

	//--------------------------------------------------------------
	bool AddBigToggle(ofParameter<bool>& parameter, float h)
	{
		auto tmpRef = parameter.get();
		auto name = ofxImGui::GetUniqueName(parameter);

		//--

		//how to set colors
		//static float b = 1.0f;
		//static float c = 0.5f;
		//static int i = 3;// hue colors are from 0 to 7
		//ImVec4 _color1 = (ImVec4)ImColor::HSV(i / 7.0f, b, b);
		//ImVec4 _color2 = (ImVec4)ImColor::HSV(i / 7.0f, c, c);

		//--

		//button toggle

		float w;
		//float h;
		//h = 30;
		//w = 200;
		w = ImGui::GetWindowWidth()*0.9f;

		static bool _boolToggle = tmpRef;  // default value, the button is disabled 
		if (_boolToggle == true)//enabled
		{
			ImGuiStyle *style = &ImGui::GetStyle();
			const ImVec4 colorActive = style->Colors[ImGuiCol_ButtonActive];
			const ImVec4 colorButton = style->Colors[ImGuiCol_ButtonHovered];
			const ImVec4 colorHover = style->Colors[ImGuiCol_ButtonHovered];
			ImGui::PushID(name);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorActive);
			ImGui::PushStyleColor(ImGuiCol_Button, colorButton);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorHover);

			ImGui::Button(name, ImVec2(w, h));
			if (ImGui::IsItemClicked(0))
			{
				_boolToggle = !_boolToggle;
				tmpRef = _boolToggle;
				parameter.set(tmpRef);
			}

			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}
		else//disabled
		{
			ImGuiStyle *style = &ImGui::GetStyle();
			const ImVec4 colorActive = style->Colors[ImGuiCol_ButtonActive];
			const ImVec4 colorHover = style->Colors[ImGuiCol_Button];
			const ImVec4 colorButton = style->Colors[ImGuiCol_Button];//better for my theme
			//const ImVec4 colorButton = style->Colors[ImGuiCol_ButtonHovered];//better for default theme
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorActive);
			ImGui::PushStyleColor(ImGuiCol_Button, colorHover);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorHover);
			if (ImGui::Button(name, ImVec2(w, h))) {
				_boolToggle = true;
				tmpRef = _boolToggle;
				parameter.set(tmpRef);
			}
			ImGui::PopStyleColor(3);
		}

		//--

		//checkbox
		//ImGui::PushID(name);
		//ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, b, b));
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, b, b));
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, c, c));
		//if (ImGui::Checkbox(name, (bool *)&tmpRef))
		//	//if (ImGui::Checkbox(ofxImGui::GetUniqueName(parameter), (bool *)&tmpRef))
		//{
		//	parameter.set(tmpRef);
		//	ImGui::PopStyleColor(3);
		//	ImGui::PopID();
		//	return true;
		//}
		//ImGui::PopStyleColor(3);
		//ImGui::PopID();
		//return false;

		return true;//not used
	}

	//--------------------------------------------------------------
	void ImGui_ThemeModernDark()
	{
		ofLogNotice(__FUNCTION__);

		auto& style = ImGui::GetStyle();
		style.ChildRounding = 0;
		style.GrabRounding = 0;
		style.FrameRounding = 2;
		style.PopupRounding = 0;
		style.ScrollbarRounding = 0;
		style.TabRounding = 2;
		style.WindowRounding = 0;
		style.FramePadding = { 4, 4 };

		style.WindowTitleAlign = { 0.0, 0.5 };
		style.ColorButtonPosition = ImGuiDir_Left;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = { 1.0f, 1.0f, 1.0f, 1.00f };				//
		colors[ImGuiCol_TextDisabled] = { 0.25f, 0.25f, 0.25f, 1.00f };		//
		colors[ImGuiCol_WindowBg] = { 0.09f, 0.09f, 0.09f, 0.94f };			//
		colors[ImGuiCol_ChildBg] = { 0.11f, 0.11f, 0.11f, 1.00f };			//
		colors[ImGuiCol_PopupBg] = { 0.11f, 0.11f, 0.11f, 0.94f };			//
		colors[ImGuiCol_Border] = { 0.07f, 0.08f, 0.08f, 1.00f };
		colors[ImGuiCol_BorderShadow] = { 0.00f, 0.00f, 0.00f, 0.00f };
		colors[ImGuiCol_FrameBg] = { 0.35f, 0.35f, 0.35f, 0.54f };			//
		colors[ImGuiCol_FrameBgHovered] = { 0.3f, 0.3f, 0.3f, 1.00f };
		//colors[ImGuiCol_FrameBgHovered] = { 0.31f, 0.29f, 0.27f, 1.00f };
		colors[ImGuiCol_FrameBgActive] = { 0.40f, 0.36f, 0.33f, 0.67f };
		colors[ImGuiCol_TitleBg] = { 0.1f, 0.1f, 0.1f, 1.00f };
		colors[ImGuiCol_TitleBgActive] = { 0.3f, 0.3f, 0.3f, 1.00f };		//
		colors[ImGuiCol_TitleBgCollapsed] = { 0.0f, 0.0f, 0.0f, 0.61f };
		colors[ImGuiCol_MenuBarBg] = { 0.18f, 0.18f, 0.18f, 0.94f };		//
		colors[ImGuiCol_ScrollbarBg] = { 0.00f, 0.00f, 0.00f, 0.16f };
		colors[ImGuiCol_ScrollbarGrab] = { 0.24f, 0.22f, 0.21f, 1.00f };
		colors[ImGuiCol_ScrollbarGrabHovered] = { 0.3f, 0.3f, 0.3f, 1.00f };
		colors[ImGuiCol_ScrollbarGrabActive] = { 0.40f, 0.36f, 0.33f, 1.00f };
		colors[ImGuiCol_CheckMark] = { 0.84f, 0.84f, 0.84f, 1.0f };			//
		colors[ImGuiCol_SliderGrab] = { 0.4f, 0.4f, 0.4f, 1.0f };			//		
		//colors[ImGuiCol_SliderGrab] = { 0.8f, 0.8f, 0.8f, 1.0f };			//		
		colors[ImGuiCol_SliderGrabActive] = { 0.35f, 0.35f, 0.35f, 1.00f }; //
		//colors[ImGuiCol_SliderGrabActive] = { 0.55f, 0.55f, 0.55f, 1.00f }; //
		colors[ImGuiCol_Button] = { 0.55f, 0.55f, 0.55f, 0.40f };			//
		colors[ImGuiCol_ButtonHovered] = { 0.15f, 0.15f, 0.15f, 0.62f };	//	
		colors[ImGuiCol_ButtonActive] = { 0.60f, 0.60f, 0.60f, 1.00f };		//
		colors[ImGuiCol_Header] = { 0.84f, 0.36f, 0.05f, 0.0f };			//
		colors[ImGuiCol_HeaderHovered] = { 0.25f, 0.25f, 0.25f, 0.80f };	//
		colors[ImGuiCol_HeaderActive] = { 0.42f, 0.42f, 0.42f, 1.00f };
		colors[ImGuiCol_Separator] = { 0.35f, 0.35f, 0.35f, 0.50f };		//
		colors[ImGuiCol_SeparatorHovered] = { 0.3f, 0.3f, 0.3f, 0.78f };
		colors[ImGuiCol_SeparatorActive] = { 0.40f, 0.36f, 0.33f, 1.00f };
		colors[ImGuiCol_ResizeGrip] = { 1.0f, 1.0f, 1.0f, 0.25f };			//
		colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.0f, 1.0f, 0.4f };	//
		colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.0f, 0.95f };	//
		colors[ImGuiCol_Tab] = { 0.18f, 0.18f, 0.18f, 1.0f };				//
		colors[ImGuiCol_TabHovered] = { 0.58f, 0.58f, 0.58f, 0.80f };		//
		colors[ImGuiCol_TabActive] = { 0.6f, 0.60f, 0.60f, 1.00f };
		colors[ImGuiCol_TabUnfocused] = { 0.07f, 0.10f, 0.15f, 0.97f };
		colors[ImGuiCol_TabUnfocusedActive] = { 0.14f, 0.26f, 0.42f, 1.00f };
		colors[ImGuiCol_PlotLines] = { 0.66f, 0.60f, 0.52f, 1.00f };
		colors[ImGuiCol_PlotLinesHovered] = { 0.98f, 0.29f, 0.20f, 1.00f };
		colors[ImGuiCol_PlotHistogram] = { 0.60f, 0.59f, 0.10f, 1.00f };
		colors[ImGuiCol_PlotHistogramHovered] = { 0.72f, 0.73f, 0.15f, 1.00f };
		colors[ImGuiCol_TextSelectedBg] = { 0.27f, 0.52f, 0.53f, 0.35f };
		colors[ImGuiCol_DragDropTarget] = { 0.60f, 0.59f, 0.10f, 0.90f };
		colors[ImGuiCol_NavHighlight] = { 0.51f, 0.65f, 0.60f, 1.00f };
		colors[ImGuiCol_NavWindowingHighlight] = { 1.00f, 1.00f, 1.00f, 0.70f };
		colors[ImGuiCol_NavWindowingDimBg] = { 0.80f, 0.80f, 0.80f, 0.20f };
		colors[ImGuiCol_ModalWindowDimBg] = { 0.11f, 0.13f, 0.13f, 0.35f };
	}

	//--

	////https://github.com/erickjung/SwiftGUI
	////ImGui theme
	//void ImGui_ThemeDarcula()
	//{
	//	//		//
	//	//// Copyright (c) 2020, Erick Jung.
	//	//// All rights reserved.
	//	////
	//	//// This source code is licensed under the MIT-style license found in the
	//	//// LICENSE file in the root directory of this source tree.
	//	////
	//	//
	//	//		import Foundation
	//	//
	//	//			public class DarculaTheme : Theme {
	//	//
	//	//			public var colors : [GuiColorProperty:GuiColor]{
	//	//
	//	//				return[
	//	//					.text:.white,
	//	//					.textDisabled : GuiColor(r : 0.54, g : 0.54, b : 0.54, a : 1),
	//	//					.windowBg : GuiColor(r : 0.23, g : 0.24, b : 0.25, a : 1),
	//	//					.childBg : GuiColor(r : 0.23, g : 0.24, b : 0.25, a : 1),
	//	//					.popupBg : GuiColor(r : 0.23, g : 0.24, b : 0.25, a : 1),
	//	//					.border : GuiColor(r : 0.36, g : 0.36, b : 0.36, a : 1),
	//	//					.borderShadow : GuiColor(r : 0.15, g : 0.15, b : 0.15, a : 0),
	//	//					.frameBg : GuiColor(r : 0.27, g : 0.28, b : 0.29, a : 1),
	//	//					.frameBgHovered : GuiColor(r : 0.27, g : 0.28, b : 0.29, a : 1),
	//	//					.frameBgActive : GuiColor(r : 0.47, g : 0.47, b : 0.47, a : 0.67),
	//	//					.titleBg : GuiColor(r : 0.04, g : 0.04, b : 0.04, a : 1),
	//	//					.titleBgActive : GuiColor(r : 0, g : 0, b : 0, a : 0.51),
	//	//					.titleBgCollapsed : GuiColor(r : 0.16, g : 0.29, b : 0.48, a : 1),
	//	//					.menuBarBg : GuiColor(r : 0.27, g : 0.28, b : 0.29, a : 0.8),
	//	//					.scrollbarBg : GuiColor(r : 0.39, g : 0.4, b : 0.4, a : 0),
	//	//					.scrollbarGrab : GuiColor(r : 0.39, g : 0.4, b : 0.4, a : 1),
	//	//					.scrollbarGrabHovered : GuiColor(r : 0.39, g : 0.4, b : 0.4, a : 1),
	//	//					.scrollbarGrabActive : GuiColor(r : 0.39, g : 0.4, b : 0.4, a : 1),
	//	//					.checkMark : GuiColor(r : 0.65, g : 0.65, b : 0.65, a : 1),
	//	//					.sliderGrab : GuiColor(r : 0.7, g : 0.7, b : 0.7, a : 0.62),
	//	//					.sliderGrabActive : GuiColor(r : 0.3, g : 0.3, b : 0.3, a : 0.84),
	//	//					.button : GuiColor(r : 0.29, g : 0.31, b : 0.32, a : 1),
	//	//					.buttonHovered : GuiColor(r : 0.29, g : 0.31, b : 0.32, a : 1),
	//	//					.buttonActive : GuiColor(r : 0.21, g : 0.34, b : 0.5, a : 1),
	//	//					.header : GuiColor(r : 0.32, g : 0.33, b : 0.34, a : 1),
	//	//					.headerHovered : GuiColor(r : 0.30, g : 0.32, b : 0.32, a : 1),
	//	//					.headerActive : GuiColor(r : 0.47, g : 0.47, b : 0.47, a : 0.67),
	//	//					.separator : GuiColor(r : 0.31, g : 0.31, b : 0.31, a : 1),
	//	//					.separatorHovered : GuiColor(r : 0.31, g : 0.31, b : 0.31, a : 1),
	//	//					.separatorActive : GuiColor(r : 0.31, g : 0.31, b : 0.31, a : 1),
	//	//					.resizeGrip : GuiColor(r : 1, g : 1, b : 1, a : 0.85),
	//	//					.resizeGripHovered : GuiColor(r : 1, g : 1, b : 1, a : 0.6),
	//	//					.resizeGripActive : GuiColor(r : 0.47, g : 0.47, b : 0.47, a : 0.67),
	//	//					.tab : GuiColor(r : 0.32, g : 0.33, b : 0.34, a : 1),
	//	//					.tabHovered : GuiColor(r : 0.21, g : 0.34, b : 0.5, a : 1),
	//	//					.tabActive : GuiColor(r : 0.21, g : 0.34, b : 0.5, a : 1),
	//	//					.tabUnfocused : GuiColor(r : 0.06, g : 0.53, b : 0.98, a : 0.8),
	//	//					.tabUnfocusedActive : GuiColor(r : 0.06, g : 0.53, b : 0.98, a : 0.4),
	//	//					.plotLines : GuiColor(r : 0.61, g : 0.61, b : 0.61, a : 1),
	//	//					.plotLinesHovered : GuiColor(r : 1, g : 0.43, b : 0.35, a : 1),
	//	//					.plotHistogram : GuiColor(r : 0.9, g : 0.7, b : 0, a : 1),
	//	//					.plotHistogramHovered : GuiColor(r : 1, g : 0.6, b : 0, a : 1),
	//	//					.textSelectedBg : GuiColor(r : 0.18, g : 0.39, b : 0.79, a : 0.9),
	//	//					.modalWindowDimBg : GuiColor(r : 0.18, g : 0.39, b : 0.79, a : 1)
	//	//				]
	//	//			}
	//	//
	//	//				public var windowRounding : Float{
	//	//					return 5.3
	//	//					}
	//	//
	//	//						public var grabRounding : Float{
	//	//							return 2.3
	//	//					}
	//	//
	//	//						public var frameRounding : Float{
	//	//							return 2.3
	//	//					}
	//	//
	//	//						public var scrollbarRounding : Float{
	//	//							return 5
	//	//					}
	//	//
	//	//						public var frameBorderSize : Float{
	//	//							return 1
	//	//					}
	//	//
	//	//						public var itemSpacing : GuiPoint{
	//	//							return GuiPoint(x: 8, y : 6.5)
	//	//					}
	//	//
	//	//						public init() {}
	//	//		}

	//	auto& style = ImGui::GetStyle();
	//	style.ChildRounding = 0;
	//	style.GrabRounding = 0;
	//	style.FrameRounding = 2;
	//	style.PopupRounding = 0;
	//	style.ScrollbarRounding = 0;
	//	style.TabRounding = 2;
	//	style.WindowRounding = 0;
	//	style.FramePadding = { 4, 4 };
	//	style.WindowTitleAlign = { 0.0, 0.5 };
	//	style.ColorButtonPosition = ImGuiDir_Left;
	//	ImVec4* colors = ImGui::GetStyle().Colors;
	//	colors[ImGuiCol_Text] = { 1.0f, 1.0f, 1.0f, 1.00f };				//
	//	colors[ImGuiCol_TextDisabled] = { 0.25f, 0.25f, 0.25f, 1.00f };		//
	//	colors[ImGuiCol_WindowBg] = { 0.09f, 0.09f, 0.09f, 0.94f };			//
	//	colors[ImGuiCol_ChildBg] = { 0.11f, 0.11f, 0.11f, 1.00f };			//
	//	colors[ImGuiCol_PopupBg] = { 0.11f, 0.11f, 0.11f, 0.94f };			//
	//	colors[ImGuiCol_Border] = { 0.07f, 0.08f, 0.08f, 1.00f };
	//	colors[ImGuiCol_BorderShadow] = { 0.00f, 0.00f, 0.00f, 0.00f };
	//	colors[ImGuiCol_FrameBg] = { 0.35f, 0.35f, 0.35f, 0.54f };			//
	//	colors[ImGuiCol_FrameBgHovered] = { 0.31f, 0.29f, 0.27f, 1.00f };
	//	colors[ImGuiCol_FrameBgActive] = { 0.40f, 0.36f, 0.33f, 0.67f };
	//	colors[ImGuiCol_TitleBg] = { 0.1f, 0.1f, 0.1f, 1.00f };
	//	colors[ImGuiCol_TitleBgActive] = { 0.3f, 0.3f, 0.3f, 1.00f };		//
	//	colors[ImGuiCol_TitleBgCollapsed] = { 0.0f, 0.0f, 0.0f, 0.61f };
	//	colors[ImGuiCol_MenuBarBg] = { 0.18f, 0.18f, 0.18f, 0.94f };		//
	//	colors[ImGuiCol_ScrollbarBg] = { 0.00f, 0.00f, 0.00f, 0.16f };
	//	colors[ImGuiCol_ScrollbarGrab] = { 0.24f, 0.22f, 0.21f, 1.00f };
	//	colors[ImGuiCol_ScrollbarGrabHovered] = { 0.31f, 0.29f, 0.27f, 1.00f };
	//	colors[ImGuiCol_ScrollbarGrabActive] = { 0.40f, 0.36f, 0.33f, 1.00f };
	//	colors[ImGuiCol_CheckMark] = { 0.84f, 0.84f, 0.84f, 1.0f };			//
	//	colors[ImGuiCol_SliderGrab] = { 0.8f, 0.8f, 0.8f, 1.0f };			//		
	//	colors[ImGuiCol_SliderGrabActive] = { 0.55f, 0.55f, 0.55f, 1.00f }; //
	//	colors[ImGuiCol_Button] = { 0.55f, 0.55f, 0.55f, 0.40f };			//
	//	colors[ImGuiCol_ButtonHovered] = { 0.15f, 0.15f, 0.15f, 0.62f };	//	
	//	colors[ImGuiCol_ButtonActive] = { 0.60f, 0.60f, 0.60f, 1.00f };		//
	//	colors[ImGuiCol_Header] = { 0.84f, 0.36f, 0.05f, 0.0f };			//
	//	colors[ImGuiCol_HeaderHovered] = { 0.25f, 0.25f, 0.25f, 0.80f };	//
	//	colors[ImGuiCol_HeaderActive] = { 0.42f, 0.42f, 0.42f, 1.00f };
	//	colors[ImGuiCol_Separator] = { 0.35f, 0.35f, 0.35f, 0.50f };		//
	//	colors[ImGuiCol_SeparatorHovered] = { 0.31f, 0.29f, 0.27f, 0.78f };
	//	colors[ImGuiCol_SeparatorActive] = { 0.40f, 0.36f, 0.33f, 1.00f };
	//	colors[ImGuiCol_ResizeGrip] = { 1.0f, 1.0f, 1.0f, 0.25f };			//
	//	colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.0f, 1.0f, 0.4f };	//
	//	colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.0f, 0.95f };	//
	//	colors[ImGuiCol_Tab] = { 0.18f, 0.18f, 0.18f, 1.0f };				//
	//	colors[ImGuiCol_TabHovered] = { 0.58f, 0.58f, 0.58f, 0.80f };		//
	//	colors[ImGuiCol_TabActive] = { 0.6f, 0.60f, 0.60f, 1.00f };
	//	colors[ImGuiCol_TabUnfocused] = { 0.07f, 0.10f, 0.15f, 0.97f };
	//	colors[ImGuiCol_TabUnfocusedActive] = { 0.14f, 0.26f, 0.42f, 1.00f };
	//	colors[ImGuiCol_PlotLines] = { 0.66f, 0.60f, 0.52f, 1.00f };
	//	colors[ImGuiCol_PlotLinesHovered] = { 0.98f, 0.29f, 0.20f, 1.00f };
	//	colors[ImGuiCol_PlotHistogram] = { 0.60f, 0.59f, 0.10f, 1.00f };
	//	colors[ImGuiCol_PlotHistogramHovered] = { 0.72f, 0.73f, 0.15f, 1.00f };
	//	colors[ImGuiCol_TextSelectedBg] = { 0.27f, 0.52f, 0.53f, 0.35f };
	//	colors[ImGuiCol_DragDropTarget] = { 0.60f, 0.59f, 0.10f, 0.90f };
	//	colors[ImGuiCol_NavHighlight] = { 0.51f, 0.65f, 0.60f, 1.00f };
	//	colors[ImGuiCol_NavWindowingHighlight] = { 1.00f, 1.00f, 1.00f, 0.70f };
	//	colors[ImGuiCol_NavWindowingDimBg] = { 0.80f, 0.80f, 0.80f, 0.20f };
	//	colors[ImGuiCol_ModalWindowDimBg] = { 0.11f, 0.13f, 0.13f, 0.35f };
	//}

//--

	////slider enum
	//// Using the format string to display a name instead of an integer.
	//// Here we completely omit '%d' from the format string, so it'll only display a name.
	//// This technique can also be used with DragInt().
	////DemoCode_("Widgets/Basic/Inputs/Slider Enum");
	//enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };
	//static int elem = Element_Fire;
	//const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
	//const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
	//ImGui::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name);
	////ImGui::SameLine(); 
	////HelpMarker("Using the format string parameter to display a name instead of the underlying integer.");

	////drop list
	//// Using the _simplified_ one-liner Combo() api here
	//// See "Combo" section for examples of how to use the more complete BeginCombo()/EndCombo() api.
	//const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
	//static int item_current = 0;
	//ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));

#endif
};
