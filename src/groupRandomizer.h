#pragma once

#include "ofMain.h"

//-----
//
//#define INCLUDE_ofxSurfingRandomizer // randomizer engine
//
//-----

#include "ofxSurfingHelpers.h"
#include "ofxSurfing_ImGui.h"
#include "ofxSurfing_ImGuiBundle.h"
//#include "ofxImGui.h"

#ifdef INCLUDE_ofxSurfingRandomizer
#include "ofxSurfingRandomizer.h"
#endif

//#define BUTTON_BIG_HEIGHT 30
//#define PANEL_WIDGETS_WIDTH 225
//#define PANEL_WIDGETS_HEIGHT 100

class groupRandomizer
{


#ifdef INCLUDE_ofxSurfingRandomizer
private:
//protected:
//public:
	ofxSurfingRandomizer randomizer;
#endif

public:
	groupRandomizer();
	~groupRandomizer();

	void setup(ofParameterGroup &g, int _numPresets);
	void setup(ofParameterGroup &g, vector<int> keysList);

private:
	float timerPlayerPct;
public:
	float getPlayerPct() {
		return timerPlayerPct;
	}

	// TODO: target pointer
	//void setup(ofParameterGroup &g, vector<int> keysList, ofParameter<int> &index);
	//void setSelectorTARGET(ofParameter<int> &index);
	//ofParameter<int> selectorTARGET;

public:
	void startup();
	void doCheckRandomReady();

	void update();
	void keyPressed(int key);
	void exit();

private:
	ofParameterGroup group;

	void doDices();

	std::string path_RandomizerSettings;

public:
	void setPath_RandomizerSettings(string folder)
	{
		ofLogNotice(__FUNCTION__) << "Path: " << folder;

		path_RandomizerSettings = folder;
	}

public:
	void gui_RandomizersMain();
	void gui_RandomizerIndex();
	void gui_RandomizerParams();
	//ofParameter<bool> SHOW_Panel_RandomizerIndex;
	//ofParameter<bool> SHOW_Panel_RandomizerParams;
	
	//-

public:
	ofxImGui::Settings settings;

	ofParameter<int> PRESET_Selected_IndexMain;// main group preset selector (current)

	//----

private:
	ofParameterGroup params_HelperTools;
	ofParameterGroup params_Randomizer;
	ofParameterGroup params_Control;// to use on external gui
	void Changed_Control(ofAbstractParameter &e);

	int amountPresets;// amount of box-clickable handled presets on current favorites/kit
	bool bIsDoneLoad = false;
	void loadPreset(int p);
	ofParameter<bool> MODE_Editor{ "MODE EDIT", true };// this mode improves performance disabling autosave, undo history..etc
	vector<int> keys;// queued trigger keys for each group ? (all presets) (size of)

	//--

	// helper tools
private:
	ofParameter<bool> bCloneRight;
	ofParameter<bool> bCloneAll;
	ofParameter<bool> bPopulateAll;

	// easy callbacks to avoid listeners..
public:
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
	bool is_bPopulateAll() {
		if (bPopulateAll) {
			bPopulateAll = false;
			return true;
		}
		else return false;
	}

	//public:
	//	//--------------------------------------------------------------
	//	void doGetFavsToStandalonePresets()// save all favorites presets to the browser (archive) folder
	//	{
	//		ofLogNotice(__FUNCTION__);
	//
	//		//// browser path
	//		//string browser_path;
	//		//browser_path = path_UserKit_Folder + "/" + path_PresetsStandalone;
	//
	//		//// browser number of files
	//		//// iterate all presets
	//		//for (int i = 0; i < amountPresets; i++)
	//		//{
	//		//	std::string pathSrc;
	//		//	std::string pathDst;
	//
	//		//	pathSrc = getPresetPath(groups[0].getName(), i);
	//		//	boost::filesystem::path bPath(pathSrc);
	//
	//		//	//string pathFolder = ofToString(bPath.parent_path());
	//		//	string fileName = ofToString(bPath.filename().generic_string());
	//		//	pathDst = browser_path + "/" + fileName;
	//
	//		//	ofLogNotice(__FUNCTION__) << "pathSrc: " << pathSrc;
	//		//	ofLogNotice(__FUNCTION__) << "pathDst: " << pathDst;
	//
	//		//	ofFile file;
	//		//	file.copyFromTo(pathSrc, pathDst, true, true);// relative, overwrite
	//
	//		//}
	//
	//		////--
	//
	//		//// refresh files
	//		//doStandalonePresetsRefresh();
	//	}

	//----

	// randomizer

private:
	ofParameterGroup params_RandomizerSettings{ "Randomizers" };

public:
//private:
	ofParameter<bool> PLAY_RandomizeTimer; //play randomizer
	ofParameter<bool> bRandomizeIndex;// trig randomize index
	ofParameter<float> randomizeDurationBpm; // bpm
	ofParameter<int> randomizeDuration;
	ofParameter<int> randomizeDurationShort;
	ofParameter<float> randomizeDurationShortRatio;

private:
	ofParameter<bool> MODE_DicesProbs;
	ofParameter<bool> MODE_LatchTrig; // this mode trigs the preset but goes back to preset 0 after duration timer
	ofParameter<bool> MODE_AvoidRandomRepeat; // this mode re makes randomize again if new index preset it's the same!
	ofParameter<bool> bResetDices;
	ofParameter<int> randomizedDice; // to test
	bool bLatchRun = false;

private:
	int randomizeSpeed;// real time duration
	uint32_t randomizerTimer;
	float MAX_DURATION_RATIO = 2.0f;
	int randomize_MAX_DURATION = MAX_DURATION_RATIO * 6000;
	//int randomize_MAX_DURATION_SHORT = 6000 / 2.f;
	vector<ofParameter<int>> presetsRandomFactor;// probability of every preset
	vector<ofParameter<bool>> presetsRandomModeShort;// mode short for ebvery preset
	vector<int> randomFactorsDices;

	void buildRandomizers();
	void setup_RandomizerIndexes();// engine to get a random between all posible dices (from 0 to dicesTotalAmount) and then select the preset associated to the resulting dice.
	void doRandomIndex();// randomize wich preset (usually 1 to 8) is selected (not the params of the preset)
	int doRandomIndexChanged();
	void doResetDices();// reset all probs to 0
	int dicesTotalAmount;// total dices summing the prob of any preset probability (PROB1 + PROB2 + ...)

	int timerRandomizer;

	//--

//public:
private:
	ofParameter<bool> bRandomizeFiltered;

private:
	ofParameter<bool> bRandomizeFiltered_All;// put all toggles/params to true. a randomize will act over all params
	ofParameter<bool> bRandomizeFiltered_None;// put to disabled all toggles
	ofParameter<bool> bRandomizeFiltered_PopulateFavs;// create all presets

	// system to select what params of current selected preset to: clone, randomize etc
	void setup_RandomizerFiletered();
	void addGroupToRandomizerFiletered(ofParameterGroup& group);// queue all contained params inside the paramGroup and nested too
	void Changed_Editor(ofAbstractParameter &e);

public:
	void doRandomPreset();// randomize params of current selected preset

	public:
	void doRandomGroup(ofParameterGroup& group);// randomize params of current selected preset

	vector<ofParameter<bool>> randomizersFiltered_TogglesVector;
	ofParameterGroup params_RandomizersFiltered;
	ofParameterGroup params_Filtered_Toggles;

	//--

	////TODO:
	////randomizer powered
	////better random engine with min- max for the params
	//ofParameterGroup params_RandomizersPowered;
	//void setup_RandomizerPowered();
	//void addGroupToRandomizerPowered(ofParameterGroup& group);
	//vector<ofAbstractParameter> randomizersPowered_Vector;
	//vector<ofParameter<bool>> randomizersPowered_TogglesVector;

	//--

//public:
private:
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
		randomizeDurationShort = randomizeDuration * randomizeDurationShortRatio;
		//randomizeDurationShort = randomizeDuration / 2.f;
	}
	//--------------------------------------------------------------
	void doRandomizePresetFromFavs()// trig randomize and select one of the favs presets
	{
		bRandomizeIndex = true;
	}
	//--------------------------------------------------------------
	void doRandomizePresetSelected() {// randomize params of current selected preset
		ofLogNotice(__FUNCTION__);
		doRandomPreset();
	}
	//--------------------------------------------------------------
	ofParameterGroup getParamsRandomizers() {
		ofParameterGroup _g{ "RANDOMIZERS" };
		_g.add(PLAY_RandomizeTimer);
		_g.add(bRandomizeIndex);
		_g.add(randomizeDurationBpm);
		_g.add(randomizeDuration);
		_g.add(randomizeDurationShort);
		_g.add(randomizeDurationShortRatio);
		_g.add(randomizerProgress);
		return _g;
	}

public:
	void setModeRandomizeAvoidRepeat(bool b) {
		MODE_AvoidRandomRepeat = b;
	}
	void setModeEditor(bool b) {
		MODE_Editor = b;
	}

private:
	bool DISABLE_CALLBACKS = true;// to avoid startup crashes and objects are not initialized properly

	//---

	//TODO:

	public:

		void doResetGroup(ofParameterGroup& group, bool bfull = false);

		//--------------------------------------------------------------
		inline void doRandomGroupFull(ofParameterGroup& group) {
			for (auto parameter : group)
			{
				if (parameter->isSerializable())// avoid not serailizable params that will crash
				{
					// recursive..
					auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
					if (parameterGroup)
					{
						//cout << "parameterGroup: " << ofToString(parameterGroup->getName()) << endl;
						doRandomGroupFull(*parameterGroup);
						continue;
					}

					auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
					if (parameterInt)
					{
						string name = parameterInt->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							int random = ofRandom(parameterInt->getMin(), parameterInt->getMax() + 1);
							parameterInt->set(random);
						}
						continue;
					}

					auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
					if (parameterFloat)
					{
						string name = parameterFloat->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							float random = ofRandom(parameterFloat->getMin(), parameterFloat->getMax());
							parameterFloat->set(random);
						}
						continue;
					}

					auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
					if (parameterBool)
					{
						string name = parameterBool->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							bool random = (ofRandom(0, 2) >= 1);
							parameterBool->set(random);
						}
						continue;
					}

					auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
					if (parameterFloatColor)
					{
						string name = parameterFloatColor->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							ofFloatColor random;
							random = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));
							parameterFloatColor->set(random);
						}
						continue;
					}

					auto parameterColor = std::dynamic_pointer_cast<ofParameter<ofColor>>(parameter);
					if (parameterColor)
					{
						string name = parameterColor->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							ofColor random;
							random = ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255));
							parameterColor->set(random);
						}
						continue;
					}

					//glm::vec compatible
#if OF_VERSION_MINOR >= 10
					auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
					if (parameterVec2f)
					{
						string name = parameterVec2f->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							float randomx = ofRandom(parameterVec2f->getMin().x, parameterVec2f->getMax().x);
							float randomy = ofRandom(parameterVec2f->getMin().y, parameterVec2f->getMax().y);
							parameterVec2f->set(glm::vec2(randomx, randomy));
						}
						continue;
					}
					auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
					if (parameterVec3f)
					{
						string name = parameterVec3f->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							float randomx = ofRandom(parameterVec3f->getMin().x, parameterVec3f->getMax().x);
							float randomy = ofRandom(parameterVec3f->getMin().y, parameterVec3f->getMax().y);
							float randomz = ofRandom(parameterVec3f->getMin().z, parameterVec3f->getMax().z);
							parameterVec3f->set(glm::vec3(randomx, randomy, randomz));
						}
						continue;
					}
					auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
					if (parameterVec4f)
					{
						string name = parameterVec4f->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							float randomx = ofRandom(parameterVec4f->getMin().x, parameterVec4f->getMax().x);
							float randomy = ofRandom(parameterVec4f->getMin().y, parameterVec4f->getMax().y);
							float randomz = ofRandom(parameterVec4f->getMin().z, parameterVec4f->getMax().z);
							float randomw = ofRandom(parameterVec4f->getMin().w, parameterVec4f->getMax().w);
							parameterVec4f->set(glm::vec4(randomx, randomy, randomz, randomw));
						}
						continue;
					}
#endif

					auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
					if (parameterOfVec2f)
					{
						string name = parameterOfVec2f->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							float randomx = ofRandom(parameterOfVec2f->getMin().x, parameterOfVec2f->getMax().x);
							float randomy = ofRandom(parameterOfVec2f->getMin().y, parameterOfVec2f->getMax().y);
							parameterOfVec2f->set(ofVec2f(randomx, randomy));
						}
						continue;
					}
					auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
					if (parameterOfVec3f)
					{
						string name = parameterOfVec3f->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							float randomx = ofRandom(parameterOfVec3f->getMin().x, parameterOfVec3f->getMax().x);
							float randomy = ofRandom(parameterOfVec3f->getMin().y, parameterOfVec3f->getMax().y);
							float randomz = ofRandom(parameterOfVec3f->getMin().z, parameterOfVec3f->getMax().z);
							parameterOfVec3f->set(ofVec3f(randomx, randomy, randomz));
						}
						continue;
					}
					auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
					if (parameterOfVec4f)
					{
						string name = parameterOfVec4f->getName();
						//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
						ofParameter<bool> b{ "b",true };
						if (b.get())
						{
							float randomx = ofRandom(parameterOfVec4f->getMin().x, parameterOfVec4f->getMax().x);
							float randomy = ofRandom(parameterOfVec4f->getMin().y, parameterOfVec4f->getMax().y);
							float randomz = ofRandom(parameterOfVec4f->getMin().z, parameterOfVec4f->getMax().z);
							float randomw = ofRandom(parameterOfVec4f->getMin().w, parameterOfVec4f->getMax().w);
							parameterOfVec4f->set(ofVec4f(randomx, randomy, randomz, randomw));
						}
						continue;
					}

					ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter: " << parameter->getName();
				}
			}
		}
};

