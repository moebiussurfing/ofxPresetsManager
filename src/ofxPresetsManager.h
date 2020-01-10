
// ofxPresetsManager.h
// based on original addon by Nicola Pisanti, MIT License, 2016
// changes and customized by moebiussurfing:
//
// TODO:
// ++ each 8 presets can be a selection of best presets to trig handly and fast
// ++ add browser from colorManager
// + could make tween when changing params
// + save full kit of presets to avoid blocking main thread when switching presets
// because json loadings
// + use pointers bewteen classes (ofxSequencer) to share the data struct
// from DataGrid class,
// ofxSEQ, or wherever is used ... almost done. but maybe should add listeners
// re players when preset are loaded/saved..

#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

//TIMEMEASUREMENTS
#define TIME_SAMPLE_OFX_FONTSTASH2 //comment this line to remove ofxTimeMeasurements dependency
#ifdef TIME_SAMPLE_OFX_FONTSTASH2
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

// DEFINE MODE:

// un-comment one of the two modes only! can't use both together:

// A. use ofParameterGroup
#define USE_OF_PARAMETER_GROUP

//-

// B. use custom DataGrid class
//#define USE_CUSTOM_DATAGRID

//-------------------------------

#ifdef USE_CUSTOM_DATAGRID
#include "DataGrid.h"
#endif

//-

#define NUM_OF_PRESETS 8

//---

class ofxPresetsManager
{

public:

    //-

#pragma mark - OF

    ofxPresetsManager();
    ~ofxPresetsManager();

    void setup();
    void update();

    // draw some info, when the gui is drawn you can also click on the button to change / save presets
    void draw();
    void draw(int x, int y, int cellSize);

    // set the key you have to hold for saving, default is OF_KEY_CONTROL
    void setModeKey(int key);

    //-

#pragma mark - DIFFERENT DATA TYPES METHODS

    // A. ofParameterGroup

#ifdef USE_OF_PARAMETER_GROUP
    // add a gui for preset saving
    void add(ofParameterGroup params, int numPresets = 8);
    // adds and activate key switch
    void add(ofParameterGroup params, initializer_list<int> keysList);
#endif

    //-

    // B. custom DataGrid class

#ifdef USE_CUSTOM_DATAGRID
    //    // add a gui for preset saving
    //    void add( DataGrid grid, int numPresets=NUM_OF_PRESETS );
    //    // adds and activate key switch
    //    void add( DataGrid grid, initializer_list<int> keysList );

        // add a gui for preset saving
        void add( DataGrid & grid, int numPresets=8 );
        // adds and activate key switch
        void add( DataGrid & grid, initializer_list<int> keysList );
#endif

    //--

#pragma mark - CALLBACKS HELPERS

    // TODO: easy listener temp solution for ofxSEQ integration
    ofParameter<bool> DONE_load;
    ofParameter<bool> DONE_save;

    //to check in update() as callback
    bool isDoneLoad()
    {
        bool isDone = false;
        if (DONE_load)
        {
            DONE_load = false;
            isDone = true;
        }
        else
        {
            isDone = false;
        }
        return isDone;
    }

    //--

#pragma mark - API

    // API

    //set keys active
    void set_ENABLE_Keys(bool active)
    {
        bKeys = active;
        ENABLE_shortcuts = active;
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

    //--

    //BUG: trick to solve auto load fail because the sorting of xml autoSave after preset selector tag
    void refresh()
    {
        preset_filesRefresh();

        //-

        ofLogNotice("ofxPresetsManager") << "refresh";
        selected_PRE = -1;
        PRESET_selected = PRESET_selected;
        ofLogNotice("ofxPresetsManager") << "PRESET_selected:" << PRESET_selected;
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

    // API for CONTROL
    void loadPreset(int p);
    //void getNumPresets();
    //void getNumPresets();

    //-

#pragma mark - GUI

    //gui
    void set_GUI_Position(int x, int y)
    {
        guiPos = ofVec2f(x, y);
    }

    void set_GUI_Size(int w, int h)
    {
        guiSize = ofVec2f(w, h);
    }

    void set_GUI_Visible(bool visible)
    {
        SHOW_Gui = visible;
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

    //-

#pragma mark - SETTINGS

    void set_PathControl_Settings(string str)
    {
        //"assets/settings/PRESET_MANAGER_control.xml"
        pathControl = str;
    }

    void set_pathKit_Folder(string folder);
    void set_path_PresetsFolder(string folder);

    //---


#pragma mark - PRIVATE

private:

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

    ofParameter<int> PRESET_selected;//from 1 to 8 i.e.

    bool bMouseOver_Changed = false;
    bool debugClicker = true;

    //-

    // DELAYED LOADING

    // if you set it to true the preset will be loaded only when you call (false is the default behavior)
    void setDelayedLoading(bool active);
    // make preset change effective when setDelayedLoading() is set to true
    void delayedUpdate();
    // if setDelayedLoading() is set to true stages a load action
    void delayedLoad(int presetIndex, int guiIndex = 0);
    void delayedLoad(int presetIndex, string guiName);

    // switch on or off the control with the keys
    void toggleKeysControl(bool active);

    //-

    string path_KitFolder = "assets/groups/kit_1";//default kit folder for live/favorites presets
    string path_PresetsFolder = "assets/groups/presets";//default presets folder

    // save to a preset
    void save(int presetIndex, int guiIndex = 0);
    void save(int presetIndex, string guiName);

    // loads an already saved preset
    void load(int presetIndex, int guiIndex = 0);
    void load(int presetIndex, string guiName);

    // get the last loaded preset
    int getPresetIndex(int guiIndex) const;
    int getPresetIndex(string guiName) const;

    int getGuiIndex(string name) const;
    string presetName(string guiName, int presetIndex);

    void draw_CLICKER();

    //-

    // Gui

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
    ofVec2f guiSize = ofVec2f(100, 100);
    int clicker_cellSize = 80;
    ofVec2f clicker_Pos = ofVec2f(500, 500);
    //ofVec2f clicker_Size = ofVec2f(100, 100);

    //--

    // PRESETS

    //TODO
    std::string PRESET_name = "_emptyPreset";//default preset

    bool MODE_newPreset = false;
    string textInput_New = "new preset";

    // FILES
    std::vector<std::string> fileNames;
    std::vector<ofFile> files;
    int currentFile = 0;
    string textInput_temp = "type name";

    void gui_imGui_PresetManager();

    //-

    // DATA

    // A. ofParameterGroup
#ifdef USE_OF_PARAMETER_GROUP
    vector<ofParameterGroup> groups;
#endif

    //ofParameterGroup params_gui;

    //-

    // B. custom DataGrid class
#ifdef USE_CUSTOM_DATAGRID
    vector<DataGrid*> grids;
#endif

    //-

    vector<int> lastIndices;
    vector<int> presets;

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

    bool bDelayedLoading;
    vector<int> newIndices;

    //-

    ofTrueTypeFont myFont;
    string myTTF;// gui font for all gui theme
    int sizeTTF;

    //-

    void groupDebug(ofParameterGroup &group);

    //-

    // GUI

    //-

    //control panel settings/states
    string pathControl = "assets/settings/PRESET_MANAGER_control.xml";
    void load_ControlSettings();
    void save_ControlSettings();


    void Changed_Params(ofAbstractParameter &e);

    //-

    // control manager panel to selec presets, clone, save..
    ofParameterGroup params;
    //ofParameter<int> PRESET_selected;//from 1 to 8 i.e.
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

    int num_presets;
    int selected_PRE = -1;

    void doCloneRight(int patternNum);
    void doSave(int patternNum);

    //-

    bool SHOW_Gui;

    //--

    //autosave
    bool bAutosave = true;
    uint64_t timerLast_Autosave = 0;
    int timeToAutosave = 5000;

    //updating some params before save will trigs also the group callbacks
    //so we disable this callbacks just in case params updatings are required
    //in this case we will need to update gui position param
    bool DISABLE_Callbacks = false;

    //--

};
