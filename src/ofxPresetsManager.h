
//ofxPresetsManager.h
//based on original addon by Nicola Pisanti, MIT License, 2016
//changes and customized by moebiussurfing:

//TODO:

//browser
//++ add ImGui browser list like from colorManager
//+ each 8 presets can be a selection of best presets to trig handly and fast

//+++ add auto trig mode or not
//++ improve callback system ?
//+ use pointers bewween classes (ofxSequencer) to share the data struct
//+ could make tween when changing params

#pragma once

#include "ofMain.h"

#include "ofxImGui.h"
#include "ofxGui.h"

//TIMEMEASUREMENTS
#define TIME_SAMPLE_MEASURES //comment this line to remove ofxTimeMeasurements dependency
#ifdef TIME_SAMPLE_MEASURES
#include "ofxTimeMeasurements.h"
#else
#define TS_START_NIF
#define TS_STOP_NIF
#define TS_START_ACC
#define TS_STOP_ACC
#define TS_START
#define TS_STOP
#define TSGL_START
#define TSGL_STOP
#endif

//-------------------------------

#pragma mark - DEFINE DATA TYPES

//-

#define NUM_OF_PRESETS 8
#define NUM_MAX_GUIS 1//only one it's implemented! can't add more than one group!

//---

class ofxPresetsManager
{

public:

	//-

	//settings paths
	std::string groupName;//get from ofParameterGroup name
	//std::string groupName2;//get from ofParameterGroup name
	
	std::string path_GloabalFolder;//top parent folder
	std::string pathControl;//app settings
	std::string path_KitFolder;//folder for kit of selected preset
	std::string path_PresetsFolder;//for browse
	std::string PRESET_name;

	//-

	//app settings

	//this loads selected preset, autosave/autoload...gui states
	void load_ControlSettings();
	void save_ControlSettings();

	//-

	ofxPanel guiControl;
	ofParameter<bool> MODE_MemoryLive;
	ofParameter<bool> loadToMemory;
	ofParameter<bool> saveFromMemory;
	void loadAllKitToMemory();
	void saveAllKitFromMemory();

	////TODO:
	//ofParameterGroup group_TARGET;
	//void addGroup_TARGET(ofParameterGroup &g);	//vector<ofParameterGroup> groupsMem;

	//data
	ofXml settingsArray[NUM_OF_PRESETS];

	//--

#pragma mark - OF

	ofxPresetsManager();
	~ofxPresetsManager();

	void setup();
	void update();

	//draw some info, when the gui is drawn you can also click on the button to change / save presets
	void draw();
	void draw(int x, int y, int cellSize);

	//clickeable box panel
	void draw_CLICKER();

	//-

#pragma mark - DIFFERENT DATA TYPES METHODS

	//add a gui for preset saving
	void add(ofParameterGroup params, int numPresets = 8);
	//adds and activate key switch
	void add(ofParameterGroup params, initializer_list<int> keysList);

	//--

#pragma mark - CALLBACKS

	bool BLOCK_CALLBACKS = true;
	
	ofParameter<bool> DONE_load;
	ofParameter<bool> DONE_save;

	bool bIsDoneLoad = false;
	bool isDoneLoad() {
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

	//--

#pragma mark - API

	//API

	//set the key you have to hold for saving, default is OF_KEY_CONTROL
	void setModeKey(int key);

	//set keys active
	void set_ENABLE_Keys(bool active)
	{
		bKeys = active;
		ENABLE_shortcuts = active;
	}

	bool isKeysEnabled()
	{
		return ENABLE_shortcuts;
	}

	int getCurrentPreset()
	{
		return PRESET_selected;
	}

	void load_Next()
	{
		PRESET_selected++;
		if (PRESET_selected > num_presets)
		{
			PRESET_selected = num_presets;
		}
	}

	void load_Previous()
	{
		PRESET_selected--;
		if (PRESET_selected < 1)
		{
			PRESET_selected = 1;
		}
	}

	void loadPreset(int p);
	//void getNumPresets();
	//void getNumPresets();

	//draws group name into clicker boxes panel
	bool SHOW_GroupName = false;
	void setShowGroupName(bool b)
	{
		SHOW_GroupName = b;
	}

	//--
	
	//used when preset has not changed but we like to retrig
	bool bMustTrig = false;
	bool mustTrig()
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

	//BUG: trick to solve auto load fail because the sorting of xml autoSave after preset selector tag
	void refresh()
	{
		////browser
		//preset_filesRefresh();

		//-

		ofLogNotice("ofxPresetsManager") << "> refresh()";
		selected_PRE = -1;
		PRESET_selected = PRESET_selected;
		ofLogNotice("ofxPresetsManager") << "PRESET " << PRESET_selected;
	}


	//load presets from preset folder, not from favorites presets folders
	void preset_load(string name);
	void preset_save(string name);
	void preset_filesRefresh();

	void setHelpVisible(bool b)
	{
		debugClicker = b;
	}

	//-

#pragma mark - GUI

	void set_GUI_Position(int x, int y)
	{
		guiPos = ofVec2f(x, y);
	}
	void set_GUI_ControlPosition(int x, int y)
	{
		guiPos_Control = ofVec2f(x, y);
		guiControl.setPosition(guiPos_Control.x, guiPos_Control.y);
	}

	void set_GUI_Size(int w, int h)
	{
		guiSize = ofVec2f(w, h);
	}

	void set_GUI_Visible(bool visible)
	{
		SHOW_Gui = visible;
	}

	bool is_GUI_Visible()
	{
		return SHOW_Gui;
	}

	void set_CLICKER_Position(int x, int y, int _cellSize)
	{
		clicker_Pos.x = x;
		clicker_Pos.y = y;
		clicker_cellSize = _cellSize;
	}

	void set_CLICKER_Visible(bool visible)
	{
		SHOW_ClickPanel = visible;
	}

	//--

#pragma mark - SETTINGS

	void set_Path_ControlSettings(string str)
	{
		pathControl = str;
	}

	void set_Path_KitFolder(string folder);
	void set_Path_PresetsFolder(string folder);
	void set_Path_GlobalFolder(string folder);

	void setAutoLoad(bool b)
	{
		autoLoad = b;
	}

	void setAutoSave(bool b)
	{
		autoSave = b;
	}

	void setAutoSaveTimer(bool b)
	{
		bAutosaveTimer = b;
	}

	//--

	//from 1 to 8. (indexed vector vars starts from 0)
	ofParameter<int> PRESET_selected;
	int selected_PRE = -1;

	//ofParameter<int> PRESET2_selected;
	//int selected2_PRE = -1;
	
	//--

#pragma mark - PRIVATE

private:

	//browse
	bool isMouseOver_Changed()
	{
		if (bMouseOver_Changed)
		{
			//bMouseOver_Changed = false;//reset
			return true;
		}
	}

	bool getIsMouseOver()
	{
		return gui_MouseOver;
	}

	bool bMouseOver_Changed = false;
	bool debugClicker = true;

	//--

	//TODO: what is this for ?
	//delayed loading

	////if you set it to true the preset will be loaded only when you call (false is the default behavior)
	//void setDelayedLoading(bool active);
	////make preset change effective when setDelayedLoading() is set to true
	//void delayedUpdate();
	////if setDelayedLoading() is set to true stages a load action
	//void delayedLoad(int presetIndex, int guiIndex = 0);
	//void delayedLoad(int presetIndex, string guiName);

	//-

	//switch on or off the control with the keys
	void toggleKeysControl(bool active);

	//-

	//engine

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
	string presetName(string guiName, int presetIndex);

	//-

	//browser

	bool SHOW_Gui;

	ofxImGui::Gui gui;
	//ofxImGui::Settings mainSettings = ofxImGui::Settings();

	bool gui_draw();
	bool gui_Visible;
	bool gui_MouseOver;

	void gui_draw_MenuBar();
	void gui_draw_MenuFile();
	void gui_imGui_theme();

	//void gui_saveToFile(const std::string &filename, ofAbstractParameter &parameter);
	//void gui_loadFromFile(const std::string &filename, ofAbstractParameter &parameter);
	//void gui_SaveAsSettings();

	ofVec2f guiPos = ofVec2f(500, 500);
	ofVec2f guiPos_Control = ofVec2f(500, 500);
	ofVec2f guiSize = ofVec2f(100, 100);
	int clicker_cellSize = 80;
	ofVec2f clicker_Pos = ofVec2f(500, 500);
	//ofVec2f clicker_Size = ofVec2f(100, 100);

	//--

	//presets

	bool MODE_newPreset = false;
	string textInput_New = "new preset";

	//files
	std::vector<std::string> fileNames;
	std::vector<ofFile> files;
	int currentFile = 0;
	string textInput_temp = "type name";

	void gui_imGui_PresetManager();

	//-

	//data

	//A. ofParameterGroup
	std::vector<ofParameterGroup> groups;
	//to store multiple group targets. 
	//when using only one ofParameterGroup, there's only one group element!
	//ofParameterGroup params_gui;

	//-

	std::vector<int> lastIndices;//?
	std::vector<int> presets;//?

	//-

	void addKeysListeners();
	void removeKeysListeners();
	void keyPressed(ofKeyEventArgs &eventArgs);
	void keyReleased(ofKeyEventArgs &eventArgs);

	bool bKeys;
	vector<vector<int>> keys;
	bool keysNotActivated;
	int modeKey;
	bool bKeySave;

	bool lastMouseButtonState;
	void mousePressed(int x, int y);

	//bool bDelayedLoading;
	std::vector<int> newIndices;

	//-

	int num_presets;
	
	void doCloneRight(int pIndex);
	
	void doSave(int pIndex);
	//void doSave2(int pIndex);

	//-

	ofTrueTypeFont myFont;
	string myTTF;//gui font for all gui theme
	int sizeTTF;

	//-

	void groupDebug(ofParameterGroup &group);

	//-

	//GUI

	//-

	//control panel settings/states

	void Changed_Params(ofAbstractParameter &e);

	//-

	//control manager params
	//to selec presets, clone, save..
	ofParameterGroup params;
	ofParameter<bool> SHOW_menu;
	ofParameter<bool> SHOW_ClickPanel;
	ofParameter<bool> bSave;
	ofParameter<bool> autoSave;
	ofParameter<bool> autoLoad;
	ofParameter<bool> bCloneRight;
	ofParameter<bool> ENABLE_shortcuts;

	ofParameterGroup params_Favorites;
	ofParameterGroup params_Gui;
	ofParameterGroup params_Options;
	ofParameterGroup params_Tools;

	//--

	//timer autosave
	ofParameter<bool> bAutosaveTimer;
	uint64_t timerLast_Autosave = 0;
	int timeToAutosave = 5000;

	//updating some params before save will trigs also the group callbacks
	//so we disable this callbacks just in case params updatings are required
	//in this case we will need to update gui position param
	//bool DISABLE_Callbacks = false;

	//--

};
