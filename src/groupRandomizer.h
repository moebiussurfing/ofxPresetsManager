#pragma once
#include "ofMain.h"
#include "ofxSurfingHelpers.h"
#include "ofxImGui.h"

//#define DEBUG_randomTest

class groupRandomizer
{

public:
	//void doCheckPresetsFolderIsEmpty();
	
	
	groupRandomizer();
	~groupRandomizer();


	void setup(ofParameterGroup &g, int _numPresets);
	void setup(ofParameterGroup &g, vector<int> keysList);
	//void setup(ofParameterGroup &g, vector<int> keysList, ofParameter<int> &index);

	//void setSelectorTARGET(ofParameter<int> &index);

	void update();
	//void draw();
	void keyPressed(int key);
	void exit();

private:
	ofParameterGroup group;
	//ofParameter<int> selectorTARGET;

	void doDices();

	std::string path_RandomizerSettings;
public:
	void setPath_RandomizerSettings(string folder)
	{
		ofLogNotice(__FUNCTION__) << "path: " << folder;

		path_RandomizerSettings = folder;
	}

public:
	void ImGui_Draw_GroupRandomizers(ofxImGui::Settings &settings);
	
	ofParameter<int> PRESET_Selected_IndexMain;// main group preset selector (current)
	
	//----

private:
	ofParameterGroup params_HelperTools;
	ofParameterGroup params_Randomizer;
	ofParameterGroup params_Control;// to use on external gui
	void Changed_Control(ofAbstractParameter &e);

	//ofParameterGroup params_Randomizer;
	int mainGroupAmtPresetsFav;// amount of box-clickable handled presets on current favorites/kit
	bool bIsDoneLoad = false;
	void loadPreset(int p);
	ofParameter<bool> MODE_Editor{ "MODE EDIT", true };// this mode improves performance disabling autosave, undo history..etc
	vector<int> keys;// queued trigger keys for each group ? (all presets) (size of)

	//-

	// helper tools
public:
	ofParameter<bool> bCloneRight;
	ofParameter<bool> bCloneAll;
	// easy callbacks to avoid listeners..
	bool is_bCloneRight() {
		if (bCloneRight) {
			bCloneRight = false;
			return true;
		}
		else return false;
	}
	bool is_bCloneAll() {
		if (bCloneAll) {
			bCloneAll = false;
			return true;
		}
		else return false;
	}

//public:
	//void doCloneRight(int pIndex);// clone from selected preset to all others to the right
	//void doCloneAll();// clone all presets from the current selected
	//void doPopulateFavs();// fast populate random presets around all favs
	//void doSwap(int groupIndex, int fromIndex, int toIndex);

	// helpers
public:
	//--------------------------------------------------------------
	void doGetFavsToFilesBrowser()// save all favorites presets to the browser (archive) folder
	{
		ofLogNotice(__FUNCTION__);

		//// browser path
		//string browser_path;
		//browser_path = path_UserKit_Folder + "/" + path_PresetsStandalone;

		//// browser number of files
		//// iterate all presets
		//for (int i = 0; i < mainGroupAmtPresetsFav; i++)
		//{
		//	std::string pathSrc;
		//	std::string pathDst;

		//	pathSrc = getPresetPath(groups[0].getName(), i);
		//	boost::filesystem::path bPath(pathSrc);

		//	//string pathFolder = ofToString(bPath.parent_path());
		//	string fileName = ofToString(bPath.filename().generic_string());
		//	pathDst = browser_path + "/" + fileName;

		//	ofLogNotice(__FUNCTION__) << "pathSrc: " << pathSrc;
		//	ofLogNotice(__FUNCTION__) << "pathDst: " << pathDst;

		//	ofFile file;
		//	file.copyFromTo(pathSrc, pathDst, true, true);// relative, overwrite

		//}

		////--

		//// refresh files
		//doStandaloneRefreshPresets();
	}

	//----


	// randomizer

private:
	ofParameterGroup params_RandomizerSettings{ "Randomizers" };

	public:
	ofParameter<bool> PLAY_RandomizeTimer; //play randomizer
	ofParameter<bool> bRandomizeIndex;// trig randomize index

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

private:
	ofParameter<int> randomizeDuration;
	ofParameter<int> randomizeDurationShort;
	ofParameter<float> randomizeDurationBpm;
	//ofParameter<float> randomizeSpeedF;// speed scaler. not used

private:
	int randomizeSpeed;// real time duration
	uint32_t randomizerTimer;
	float MAX_DURATION_RATIO = 2.0f;
	int randomize_MAX_DURATION = MAX_DURATION_RATIO * 6000;
	int randomize_MAX_DURATION_SHORT = 6000 / 2.f;
	vector<ofParameter<int>> presetsRandomFactor;// probability of every preset
	vector<ofParameter<bool>> presetsRandomModeShort;// mode short for ebvery preset
	vector<int> randomFactorsDices;

	void buildRandomizers();
	void setupRandomizer();// engine to get a random between all posible dices (from 0 to dicesTotalAmount) and then select the preset associated to the resulting dice.
	void doRandomIndex();// randomize wich preset (usually 1 to 8) is selected (not the params of the preset)
	int doRandomIndexChanged();
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
	void addGroupToEditor(ofParameterGroup& group);// queue all contained params inside the paramGroup and nested too
	void Changed_Editor(ofAbstractParameter &e);
	void doRandomPreset();// randomize params of current selected preset
	void doRandomGroup(ofParameterGroup& group);// randomize params of current selected preset

	vector<ofParameter<bool>> editorPresets;
	ofParameterGroup params_Editor;
	ofParameterGroup params_Editor_Toggles;

	//--

public:
	ofParameterGroup params_randomizer;

private:
	ofParameter<int> randomizerProgress{ "%", 0, 0, 100 };
	float _prog;

	//--

	//----
	//
	// API
	//
	//----

	// randomizer helpers

public:

	//--------------------------------------------------------------
	void setPlayRandomizerTimer(bool b)// play randomizer timer
	{
		PLAY_RandomizeTimer = b;
		if (b) doRandomIndex();
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
		doRandomPreset();
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
private:
	bool DISABLE_CALLBACKS = true;// to avoid startup crashes and objects are not initialized properly

};

