#include "ofxPresetsManager.h"

#pragma mark - OF

#ifdef INCLUDE_RANDOMIZER
//--------------------------------------------------------------
int ofxPresetsManager::doRandomizeWichSelectedPresetCheckChanged()
{
	ofLogVerbose(__FUNCTION__);

	//-

	int _r = PRESET_selected;
	//int _numPresets = 8;

	if (MODE_DicesProbs) {

		//1. dice randomize

#ifndef DEBUG_randomTest
		{
			//get a random between all possible dices (from 0 to dicesTotalAmount) and then select the preset associated to the resulting dice.
			//each preset has many dices: more dices 0 more probality to be selected by the randomizer
			randomizedDice = ofRandom(0, dicesTotalAmount);

			ofLogVerbose(__FUNCTION__) << "random: " << randomizedDice.get() << "/" << dicesTotalAmount;
			//ofLogVerbose(__FUNCTION__) << "dicesTotalAmount:" << dicesTotalAmount;
		}
#endif

		//-

		//2. define limits for range dices associated to any preset

		//randomFactorsDices[0] = 0;
		for (int i = 0; i < presetsRandomFactor.size(); i++) {
			randomFactorsDices[i] = 0;

			for (int f = 0; f < i; f++) {
				randomFactorsDices[i] += presetsRandomFactor[f];
			}
		}

		//-

		//3. check if dice is inside both ranges. to select preset (_rr) associated to dice 

		int _rr = 0;
		//for (int i = 0; i < presetsRandomFactor.size(); i++) {
		for (int i = 0; i <= presetsRandomFactor.size(); i++) {

			//define upper/lower limits for each dices/preset
			int start;
			int end;

			if (i == 0) start = 0;
			else start = randomFactorsDices[i - 1];

			//TODO:
			//bug on last preset..

			//if (i == presetsRandomFactor.size() - 1)
			//	end = presetsRandomFactor.size()-1;
			//else
			//	end = randomFactorsDices[i];
			//end = randomFactorsDices[i];

			if (i != presetsRandomFactor.size()) end = randomFactorsDices[i];
			else end = dicesTotalAmount;

#ifdef DEBUG_randomTest
			ofLogNotice(__FUNCTION__) << (start == end ? "\t\t\t\t" : "") << "[" << i << "] " << start << "-" << end;
#endif
			//check if dice is inside both limits
			if (randomizedDice >= start && randomizedDice < end)
			{
				_rr = i - 1;
			}
		}
#ifdef DEBUG_randomTest
		ofLogNotice(__FUNCTION__) << "dice: " << randomizedDice << "  ->  index preset: [" << _rr << "]";
		//ofLogNotice(__FUNCTION__) << "preset: " << _rr + 1;
#endif
		_r = _rr;
		//_r = _rr + 1;

		//debug
		ofLogVerbose(__FUNCTION__) << "DEBUG";
		//for (int i = 0; i < presetsRandomFactor.size(); i++) {
		for (int i = 0; i < presetsRandomFactor.size(); i++) {
#ifdef DEBUG_randomTest
			ofLogNotice(__FUNCTION__) << "randomFactorsDices: [" << i << "] " << randomFactorsDices[i];
			// << "-" << randomFactorsDices[i + 1] << endl;
#endif
		}
		//last
#ifdef DEBUG_randomTest
		ofLogNotice(__FUNCTION__) << "randomFactorsDices: [" << presetsRandomFactor.size() << "] " << dicesTotalAmount;
#endif

		//for (int f = 1; f < presetsRandomFactor.size(); f++) {
		//	start = presetsRandomFactor[i - 1].get();
		//}
		//
		//if (randomizedDice >= presetsRandomFactor[i - 1].get() && randomizedDice < presetsRandomFactor[i].get()) {
		//}
		//}

		//for (auto &p : presetsRandomFactor) {
		//	dicesTotalAmount += p.get();
		//}
	}
	//else {
	//	int numTryes = 0;
	//	//avoid jump to same current preset
	//	while (PRESET_selected == _r)//if not changed
	//	{
	//		ofLogWarning(__FUNCTION__) << "Randomize not changed! Try #" << ofToString(++numTryes);
	//		ofLogNotice(__FUNCTION__) << "PRESET Previous was : " << ofToString(_r);
	//		ofLogNotice(__FUNCTION__) << "PRESET New Random is: " << ofToString(PRESET_selected);
	//		PRESET_selected = (int)ofRandom(0, presetsXmlArray.size());
	//		
	//		//if (MODE_MemoryLive) _r = (int)ofRandom(0, presetsXmlArray.size());
	//		//_r = (int)ofRandom(1, presetsXmlArray.size() + 1);
	//	}
	//}

	return _r;
}

//--------------------------------------------------------------
void ofxPresetsManager::doRandomizeWichSelectedPreset()
{
	//ofLogNotice(__FUNCTION__);
	ofLogNotice(__FUNCTION__) << "---------------------------------------------------";

	//we avoid that the random is the same previous preset (TODO:improve). we want force change, not stay in the same. 
	//bc sometimes the random gets the same current preset.

	int _PRESET_selected_PRE = PRESET_selected;

	int r = doRandomizeWichSelectedPresetCheckChanged();

	//TODO:
	//if there's only one posible dice.. cant avoid repetition. so force avoid toggle to false
	if (MODE_AvoidRandomRepeat && dicesTotalAmount < 2) MODE_AvoidRandomRepeat = false;

	if (MODE_AvoidRandomRepeat)
	{
		int numTryes = 0;
		ofLogNotice(__FUNCTION__) << "Randomize Try #" << ofToString(++numTryes);
		ofLogNotice(__FUNCTION__) << "PRESET Random is: " << ofToString(r);

		while (r == _PRESET_selected_PRE && dicesTotalAmount > 1 && numTryes < 5) //while preset index not changed. TODO: avoid make more than 5 randoms..
		{
			r = doRandomizeWichSelectedPresetCheckChanged();
			ofLogWarning(__FUNCTION__) << "Randomize Try #" << ofToString(++numTryes) << " NOT changed!";
			ofLogNotice(__FUNCTION__) << "PRESET Previous was : " << ofToString(_PRESET_selected_PRE);
			ofLogNotice(__FUNCTION__) << "PRESET New Random is: " << ofToString(r);
			ofLogWarning(__FUNCTION__) << "RETRY !";
		}
	}

	//--

	//4. apply preset selection

	ofLogNotice(__FUNCTION__) << "PRESET > " << ofToString(r);
	loadPreset(r);

	//int __r = (int)ofRandom(1.0f, 9.0f);
	//ofLogNotice(__FUNCTION__) << "\t > " << ofToString(__r);
	//loadPreset(__r);

	//--

	//5. start timer again

	if (PLAY_RandomizeTimer)
	{
		randomizerTimer = ofGetElapsedTimeMillis();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doResetDices()
{
	for (auto &p : presetsRandomFactor) {
		p = 0;
	}
	if (presetsRandomFactor.size() > 0) {
		presetsRandomFactor[0] = 1;
		dicesTotalAmount = 1;
		randomizedDice = 0;
	}
	else {
		dicesTotalAmount = 0;
	}
	randomizedDice.setMax(dicesTotalAmount - 1);
	randomizeDuration = 1000;
	randomizeDurationShort = randomizeDuration * 0.5;
}

//--------------------------------------------------------------
void ofxPresetsManager::addGroupToEditor(ofParameterGroup& group) {
	//editorPresets.clear();//?

	//TODO:
	//if we want to make nested folders (not all toggles into same and only one level, we need to create subgroups too...)

	for (auto parameter : group)
	{
		//group
		auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
		if (parameterGroup)
		{
			addGroupToEditor(*parameterGroup);
			continue;
		}

		//exclude params not marked as serializable
		if (parameter->isSerializable())
		{
			//parameter, try everything we know how to handle.

			//x,y,z vectors
#if OF_VERSION_MINOR >= 10
			auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
			if (parameterVec2f)
			{
				ofParameter<bool> b{ parameterVec2f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
			if (parameterVec3f)
			{
				ofParameter<bool> b{ parameterVec3f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
			if (parameterVec4f)
			{
				ofParameter<bool> b{ parameterVec4f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
#endif
			auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
			if (parameterOfVec2f)
			{
				//ofxImGui::AddParameter(*parameterOfVec2f);
				ofParameter<bool> b{ parameterOfVec2f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
			if (parameterOfVec3f)
			{
				//ofxImGui::AddParameter(*parameterOfVec3f);
				ofParameter<bool> b{ parameterOfVec3f->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
			if (parameterOfVec4f)
			{
				ofParameter<bool> b{ parameterOfVec4f->getName(), false };
				editorPresets.push_back(b);
				//ofxImGui::AddParameter(*parameterOfVec4f);
				continue;
			}
			//colors
			auto parameterColor = std::dynamic_pointer_cast<ofParameter<ofColor>>(parameter);
			if (parameterColor)
			{
				ofParameter<bool> b{ parameterColor->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
			if (parameterFloatColor)
			{
				ofParameter<bool> b{ parameterFloatColor->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			//normal types
			auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
			if (parameterFloat)
			{
				ofParameter<bool> b{ parameterFloat->getName(), false };
				editorPresets.push_back(b);
				//ofxImGui::AddParameter(*parameterFloat);
				continue;
			}
			auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
			if (parameterInt)
			{
				ofParameter<bool> b{ parameterInt->getName(), false };
				editorPresets.push_back(b);
				continue;
			}
			auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
			if (parameterBool)
			{
				ofParameter<bool> b{ parameterBool->getName(), false };
				editorPresets.push_back(b);
				continue;
			}

			ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter " << parameter->getName();
		}
	}
}


//--------------------------------------------------------------
void ofxPresetsManager::doRandomizeEditor() {
	ofLogNotice(__FUNCTION__);

	//if(params_Editor_Toggles.size() == 0) params_Editor_Toggles

	ofParameterGroup _group = groups[0];
	doRandomizeEditorGroup(_group);

#ifdef INCLUDE_ofxUndoSimple
	undoStoreParams();// store scene
#endif
}

//--------------------------------------------------------------
void ofxPresetsManager::doRandomizeEditorGroup(ofParameterGroup& group) {
	for (auto parameter : group)
	{
		if (parameter->isSerializable())//avoid not serailizable params that will crash
		{
			//recursive..
			auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
			if (parameterGroup)
			{
				//cout << "parameterGroup: " << ofToString(parameterGroup->getName()) << endl;
				doRandomizeEditorGroup(*parameterGroup);
				continue;
			}

			auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
			if (parameterInt)
			{
				string name = parameterInt->getName();
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Editor_Toggles.getBool(name);
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

			ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter " << parameter->getName();
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::setupEditor()
{
	editorPresets.clear();//?

	//int num = groups[0].getGroupHierarchyNames().size();
	ofParameterGroup group = groups[0];
	addGroupToEditor(groups[0]);//enqueue all content params and create a toggle for each one

	//add to group
	bRandomizeEditor.set("RANDOMIZE PRESET", false);
	bRandomizeEditorPopulateFavs.set("POPULATE FAVS!", false);
	bRandomizeEditorAll.set("ALL", false);
	bRandomizeEditorNone.set("NONE", false);

	bRandomizeEditor.setSerializable(false);
	bRandomizeEditorPopulateFavs.setSerializable(false);
	bRandomizeEditorAll.setSerializable(false);
	bRandomizeEditorNone.setSerializable(false);

	params_Editor.setName("PRESET EDIT");
	params_Editor.add(bRandomizeEditor);
	params_Editor.add(bRandomizeEditorAll);
	params_Editor.add(bRandomizeEditorNone);
	params_Editor.add(bRandomizeEditorPopulateFavs);

	params_Editor_Toggles.setName("PRESET PARAMETERS");
	params_Editor_Toggles.clear();
	for (auto &p : editorPresets) {
		params_Editor_Toggles.add(p);
	}
	params_Editor.add(params_Editor_Toggles);

	ofAddListener(params_Editor.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Editor);
}

//--------------------------------------------------------------
void ofxPresetsManager::setupRandomizer()
{
	//select a random preset (from 1 to 8)
	//params_Randomizer.setName("Randomizer");
	bRandomizeIndex.set("RANDOMIZE INDEX", false);
	PLAY_RandomizeTimer.set("PLAY RANDOMIZER", false);
	PLAY_RandomizeTimer.setSerializable(false);
	MODE_DicesProbs.set("MODE USE PROBS/DICES", true);
	MODE_LatchTrig.set("MODE LATCH", false);
	MODE_AvoidRandomRepeat.set("MODE AVOID REPEAT", false);
	randomizeDuration.set("T DURATION", 1000, 10, randomize_MAX_DURATION);
	randomizeDurationShort.set("T SHORT", 250, 10, randomize_MAX_DURATION_SHORT);
	randomizeDurationBpm.set("T BPM", 120, 10, 400);
	randomizedDice.set("DICE", 0, 0, numPresetsFavorites - 1);
	//randomizeSpeedF.set("SPEED FACTOR", 1.f, 0.01f, 2.f);
	bResetDices.set("RESET DICES", false);

	//exclude
	bRandomizeIndex.setSerializable(false);
	bResetDices.setSerializable(false);
	//randomizeDuration.setSerializable(false);

	//erase
	presetsRandomFactor.clear();
	presetsRandomModeShort.clear();
	randomFactorsDices.clear();

	//resize
	presetsRandomFactor.resize(numPresetsFavorites);
	presetsRandomModeShort.resize(numPresetsFavorites);
	randomFactorsDices.resize(numPresetsFavorites);

	int i;

	//ints as probability for every preset
	i = 0;
	ofParameterGroup _gOdds{ "PRESETS PROBS" };
	_gOdds.clear();
	for (auto &p : presetsRandomFactor) {
		string n = "PROB " + ofToString(i++);
		p.set(n, 5, 0, 10);
		_gOdds.add(p);
	}

	//toggles to enable short duration mode
	i = 0;
	ofParameterGroup _gShort{ "MODE DURATION SHORT" };
	_gShort.clear();
	for (auto &p : presetsRandomModeShort) {
		string n = "SHORT " + ofToString(i++);
		p.set(n, false);
		_gShort.add(p);
	}

	params_Randomizer.setName("FAVOURITES");
	params_Randomizer.clear();//TODO: should split to avoid call again when setted custom path...
	params_Randomizer.add(PLAY_RandomizeTimer);
	params_Randomizer.add(bRandomizeIndex);
	params_Randomizer.add(randomizeDuration);
	params_Randomizer.add(randomizeDurationShort);
	params_Randomizer.add(randomizeDurationBpm);
	params_Randomizer.add(MODE_DicesProbs);
	params_Randomizer.add(MODE_LatchTrig);
	params_Randomizer.add(MODE_AvoidRandomRepeat);
	params_Randomizer.add(_gOdds);
	params_Randomizer.add(_gShort);
	params_Randomizer.add(bResetDices);
#ifdef DEBUG_randomTest
	params_Randomizer.add(randomizedDice);
#endif
}
#endif

//--------------------------------------------------------------
ofxPresetsManager::ofxPresetsManager()
{
	//-

	DISABLE_CALLBACKS = true;

	//-

	//subscribed to auto run update and draw without required 'manual calls'
	ofAddListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofAddListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	//-

	//mainly to measure performance when using hd files vs faster memory vectors
#ifdef INCLUDE_PERFORMANCE_MEASURES
	//specify a target framerate
	//TIME_SAMPLE_SET_FRAMERATE(fps);
	//TIME_SAMPLE_ENABLE();
	TIME_SAMPLE_SET_AVERAGE_RATE(0.1);
	TIME_SAMPLE_SET_DRAW_LOCATION(TIME_SAMPLE_DRAW_LOC_BOTTOM_LEFT);
	TIME_SAMPLE_SET_PRECISION(4);
	TIME_SAMPLE_GET_INSTANCE()->setEnabled(true);
	TIME_SAMPLE_DISABLE();
#endif

	//--

	//default settings paths

	//top parent folder
	path_GLOBAL_Folder = "ofxPresetsManager";
	//default addon folder

	//default kit folder for live/favorites presets
	path_Kit_Folder = "presets";

	//big browser
	path_PresetsFolder = "archive";

	////TODO:
	////default absolute archive presets folder to browse
	//path_PresetsFolder_Custom = "F:\openFrameworks\addons\ofxPresetsManager\2_presetsManager\bin\data\ofxPresetsManager\archive";

	//app settings
	path_Control = "settings";

	//to add to all presets filenames
	//path_Prefix = "_preset_";
	path_Prefix = "_";

	filename_ControlSettings = "settingsControl.xml";
	filename_Randomizers = "settingsRandomizer.xml";

	//--

	bKeys = false;
	keysNotActivated = true;
	lastMouseButtonState = false;

	modeKeySave = OF_KEY_CONTROL;
	bKeySave = false;

	modKeySwap = OF_KEY_ALT;
	bKeySwap = false;

	//--

	//this multidimension is for multiple gui/groups (feature not implemented!)
	groups.reserve(NUM_MAX_GUIS);
	lastIndices.reserve(NUM_MAX_GUIS);//TODO: not sure if it's being used
	keys.reserve(NUM_MAX_GUIS);

	//--

	//callbacks to know when preset loading/save is done

	//alternative callbacks
	DONE_load.set("DONE LOAD", false);//easy callback to know (in ofApp) that preset LOAD is done
	DONE_save.set("DONE SAVE", false);//easy callback to know (in ofApp) that preset SAVE is done

	//easy callback: we can use too the easy isDoneLoad() to check in ofApp update() as kind of callback.

	//--

	//control parameters

	PRESET_selected.set("PRESET", 0, 0, numPresetsFavorites - 1);

#ifdef INCLUDE_GUI_IM_GUI
	SHOW_ImGui.set("SHOW ImGui", false);
	MODE_Browser_NewPreset.set("NEW!", false);
	browser_PresetName = "NO_NAME_PRESET";//browser loaded preset name
#endif

	bSave.set("SAVE", false);
	//bLoad.set("LOAD", false);

	MODE_MemoryLive.set("MODE MEMORY", false);
	loadToMemory.set("LOAD TO MEMORY", false);
	saveFromMemory.set("SAVE FROM MEMORY", false);
	autoLoad.set("AUTO LOAD", true);
	autoSave.set("AUTO SAVE", true);
	bAutosaveTimer.set("TIMER AUTO SAVE", false);

	SHOW_Gui_AdvancedControl.set("SHOW ADVANCED", false);
	SHOW_ClickPanel.set("SHOW CLICK PANEL", false);

	bCloneRight.set("CLONE >", false);
	bCloneAll.set("CLONE ALL", false);

	ENABLE_Keys.set("ENABLE KEYS", true);

	//-

	//layout
	Gui_Internal_Position.set("GUI INTERNAL POSITION",
		glm::vec2(ofGetWidth() * 0.5, ofGetHeight()* 0.5),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);

#ifdef INCLUDE_GUI_IM_GUI
	ImGui_Position.set("GUI ImGui POSITION",
		glm::vec2(ofGetWidth() * 0.5, 10),//top
		//glm::vec2(ofGetWidth() * 0.5, ofGetHeight()* 0.5),//center
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);
	ImGui_Size.set("GUI ImGui SIZE",
		glm::vec2(ofGetWidth() * 0.2, ofGetHeight()*0.5),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);
#endif

	//-

	//exclude from xml settings

	bSave.setSerializable(false);
	bCloneRight.setSerializable(false);
	bCloneAll.setSerializable(false);
	loadToMemory.setSerializable(false);
	saveFromMemory.setSerializable(false);
	//bLoad.setSerializable(false);
	//SHOW_Gui_AdvancedControl.setSerializable(false);
	//params_Tools.setSerializable(false);

	//-

	//params groups

	params_Favorites.setName("USER");
	params_Favorites.add(PRESET_selected);

	//params_Favorites.add(bLoad);
	params_Favorites.add(bSave);

	params_Options.setName("OPTIONS");
	params_Options.add(MODE_MemoryLive);
	params_Options.add(loadToMemory);
	params_Options.add(saveFromMemory);
	params_Options.add(autoLoad);
	params_Options.add(autoSave);
	params_Options.add(bAutosaveTimer);

	params_Gui.setName("GUI");
	params_Gui.add(SHOW_Gui_AdvancedControl);
	params_Gui.add(SHOW_ClickPanel);
#ifdef INCLUDE_GUI_IM_GUI
	params_Gui.add(SHOW_ImGui);
#endif
	params_Gui.add(ENABLE_Keys);

	//layout
#ifdef INCLUDE_GUI_IM_GUI
	params_Gui.add(ImGui_Position);
	params_Gui.add(ImGui_Size);
#endif
	params_Gui.add(Gui_Internal_Position);

	//---

	params_Tools.setName("HELPER TOOLS");
	params_Tools.add(bCloneRight);
	params_Tools.add(bCloneAll);
	//params_Tools.add(bRandomizeEditorPopulateFavs);

//#ifdef INCLUDE_RANDOMIZER
//	params_Tools.add(params_Randomizer);
//	//TODO:
//	//there's a problem here bc is dependent of the group params content..
//	params_Tools.add(params_Editor_Toggles);
//#endif

	//all nested params for callbacks and storage settings
	params_Control.setName("ofxPresetsManager");
	params_Control.add(params_Favorites);
	params_Control.add(params_Options);
	params_Control.add(params_Gui);
	params_Control.add(params_Tools);

	//custom path
	params_Custom.setName("CUSTOM PATH");
	params_Custom.add(bPathDirCustom);
	params_Custom.add(pathDirCustom);
	//params_Custom.add(bUseCustomPath);
	//params_Custom.add(path_PresetsFolder_Custom);
	params_Control.add(params_Custom);

	ofAddListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);
}

//--------------------------------------------------------------
void ofxPresetsManager::setPosition_DEBUG(int x, int y)
{
	errorsDEBUG.setPosition(x, y);
}

//--------------------------------------------------------------
void ofxPresetsManager::setup(std::string name)///must be called after adding params group
{
	gui_LabelName = name;
	setup();
}

//--------------------------------------------------------------
void ofxPresetsManager::setup()
{
	ofLogNotice(__FUNCTION__);

	DISABLE_CALLBACKS = true;

	//--

#ifdef DEBUG_randomTest
	ofSetLogLevel("ofxPresetsManager", OF_LOG_VERBOSE);
#endif

	//--

	//gui font

	string str;
	str = "overpass-mono-bold.otf";
	myTTF = "assets/fonts/" + str;//assets folder
	sizeTTF = 10;
	bool bLoaded = myFont.load(myTTF, sizeTTF, true, true);
#ifdef INCLUDE_DEBUG_ERRORS
	if (!bLoaded)
	{
		errorsDEBUG.addError(gui_LabelName + " " + ofToString(__FUNCTION__), "ttf font", myTTF);
	}
#endif

	//--

	//ofxGui theme
	str = "overpass-mono-bold.otf";
	string pathFont = "assets/fonts/" + str;//assets folder

	//must check this font file is found there
	ofFile file(pathFont);
	if (file.exists())
	{
		ofxGuiSetFont(pathFont, 9);
	}
	else
	{
#ifdef INCLUDE_DEBUG_ERRORS
		errorsDEBUG.addError(gui_LabelName + " " + ofToString(__FUNCTION__), "ofxGui font", pathFont);
#endif
	}

	//--

	//on this mode we use only a simplified version with ofxGui and without files explorer and ImGui
#ifndef INCLUDE_GUI_IM_GUI	//this must be check if it works...
	//theme
	ofxGuiSetDefaultHeight(20);
	ofxGuiSetBorderColor(32);
	ofxGuiSetFillColor(ofColor(48));
	ofxGuiSetTextColor(ofColor::white);
	ofxGuiSetHeaderColor(ofColor(24));

	//control gui
	gui_InternalControl.setup(gui_LabelName);
	gui_InternalControl.add(params_Control);
	gui_InternalControl.setPosition(ofGetWidth() - 210, 10);//default

	//collapse
	//gui_InternalControl.getGroup("ofxPresetsManager").minimize();
	auto &gPanel = gui_InternalControl.getGroup("ofxPresetsManager");
	auto &gGui = gPanel.getGroup("GUI");
	gGui.minimize();

	auto &gGuiPos = gGui.getGroup("GUI INTERNAL POSITION");
	gGuiPos.minimize();
#ifdef INCLUDE_GUI_IM_GUI
	auto &gGuiPos1 = gGui.getGroup("GUI ImGui POSITION");
	gGuiPos1.minimize();
	auto &gGuiPos2 = gGui.getGroup("GUI BROWSER SIZE");
	gGuiPos2.minimize();
#endif

	auto &gOptions = gPanel.getGroup("OPTIONS");
	//gOptions.minimize();
	//gOptions.maximize();
	gPanel.getGroup("HELPER TOOLS").minimize();
#endif

	//--

	//TODO:
	bPathDirCustom.set("MODE CUSTOM PATH", false);
	pathDirCustom.set("Path", path_GLOBAL_Folder);
	params_RandomizerSettings.add(params_Randomizer);
	params_RandomizerSettings.add(params_Editor);

	//--

	//create data folders if they are not presets: 
	//when you create a new project or added the addon to your existing project
	//and no /data files are present
	
	CheckAllFolder();

	//--

#ifdef INCLUDE_GUI_IM_GUI

	//browser
	browser_Setup();

	//radomizer
	params_randomizer.setName("RANDOMIZERS");
	setupRandomizer();

	//preset editor tools
	setupEditor();
	
	ofAddListener(params_Randomizer.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);

	//ImGui
#ifndef MODE_ImGui_EXTERNAL
	ImGui_Setup();
#endif

#endif

#ifdef INCLUDE_ofxUndoSimple
	undoStringParams = groups[0].toString();
#endif

	//-------

	startup();
}

//--------------------------------------------------------------
void ofxPresetsManager::startup()
{
	ofLogNotice(__FUNCTION__);

	DISABLE_CALLBACKS = false;

	//--

	//all app session settings (not the presets related)
	load_ControlSettings();

	nameUserKit = path_GLOBAL_Folder;

	//--

	//TODO:
	//force
	//bPathDirCustom = false;
	//pathDirCustom = " ";

	//--

	//timer auto save
	timerLast_Autosave = ofGetElapsedTimeMillis();

	setVisible_GUI_Internal(false);

	windowResized(ofGetWidth(), ofGetHeight());

	//--

	////memory mode
	//load_AllKit_ToMemory();

	//TODO:
	//PRESET_selected_PRE = -1;

	//--

	////TODO: 
	//workflow
	//if there are some toggles/params enabled to allow random, we do not do nothing
	//if there's no toggle/params enabled, we enable all params toggles to the editor 
	//so random will affect to all params by default

	//workflow
	bool someFalse = false;
	for (auto &p : editorPresets) {
		if (!p.get()) someFalse = true;
	}

	//if all toggles are disabled after loaded settings, 
	//then put all to true to allow populate, and random make something
	if (!someFalse) bRandomizeEditorAll = true;//not some false

	//--

	////TODO
	////moved from add
	////TODO: bug on mixerBlend.. in load_AllKit_ToMemory...
	////gui_LabelName = groups[0].getName();//TODO: one group only

	//--------

	//TODO:
	//refresh();

	//TODO:
	CheckAllFolder();
}

//--------------------------------------------------------------
void ofxPresetsManager::windowResized(int w, int h)
{
	ofLogNotice(__FUNCTION__) << "windowResized: " << w << "," << h;

	////use this bc save/load is not working
	//#ifdef INCLUDE_GUI_IM_GUI
	//	ImGui_Position = glm::vec2(w - ImGui_Size.get().x - 10, 10);
	//#endif
}

//--------------------------------------------------------------
void ofxPresetsManager::update(ofEventArgs & args)
{
	if (!DISABLE_CALLBACKS)
	{
		//-

		//randomizer timer mode latch
		if (bIsDoneLoad &&
			MODE_LatchTrig && !PLAY_RandomizeTimer) {

			randomizerTimer = ofGetElapsedTimeMillis();

			if (PRESET_selected != 0) {
				bLatchRun = true;
			}
			else {
				bLatchRun = false;
			}
		}

		//-

		//randomize timer
		if (PLAY_RandomizeTimer || MODE_LatchTrig)//?
		{
			uint32_t _time = ofGetElapsedTimeMillis();
			timerRandomizer = _time - randomizerTimer;

			if (PRESET_selected < presetsRandomModeShort.size()) {//avoid out of range

				//A. long mode
				if (presetsRandomModeShort[PRESET_selected] == false)//get if it's marked as shor or long by default (false)
				{
					if (timerRandomizer >= randomizeDuration)
					{
						if (MODE_LatchTrig) {
							if (bLatchRun) {
								loadPreset(0);
							}
						}
						else {
							bRandomizeIndex = true;//TODO: can be improved calling directly the method! bc this flag will be readed on update()..
						}
					}
				}

				//B. short mode
				else {
					if (timerRandomizer >= randomizeDurationShort)
					{
						if (MODE_LatchTrig) {
							if (bLatchRun) {
								loadPreset(0);
							}
						}
						else {
							bRandomizeIndex = true;
						}
					}
				}
			}
		}

		//-

		//TODO:
		//_totalDicesStr = "/ " + ofToString(randomizedDice.getMax());

		//-

		//plotters
#ifdef INCLUDE_PERFORMANCE_MEASURES 
		TS_START("LOAD 1");
		TS_STOP("LOAD 1");
		TS_START("LOAD 2");
		TS_STOP("LOAD 2");
		TS_START("LOAD MEM");
		TS_STOP("LOAD MEM");
#endif

		//-

		//TODO: this disables the easycallback feature..
		if (isDoneLoad())
		{
			ofLogNotice(__FUNCTION__) << groups[0].getName() << " PRESET " << PRESET_selected << " LOADED.";
			ofLogVerbose(__FUNCTION__) << groups[0].getName() << " -------------------------------------------------------------";
			//ofLogNotice() << endl;
		}

		//--

		//autosave
		//&& autoLoad? 
#ifndef INCLUDE_GUI_IM_GUI
		if (autoSave && bAutosaveTimer && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
#else
		if (!MODE_Browser_NewPreset && autoSave && bAutosaveTimer && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
#endif
		{
			ofLogNotice(__FUNCTION__) << "[AUTOSAVE]";

			//app settings
			save_ControlSettings();

			//save current preset
			doSave(PRESET_selected);

			if (!MODE_MemoryLive)
			{
				//MODE A: from hd file
				//not required because the files are already there
			}
			else
			{
				//MODE B: direct from memory
				save_AllKit_FromMemory();
			}

			//-

			//auto save timer
			timerLast_Autosave = ofGetElapsedTimeMillis();
		}
	}
}

//---------------------------------------------------------------------
void ofxPresetsManager::draw(ofEventArgs & args)
{
	//----

	//debug errors
#ifdef INCLUDE_DEBUG_ERRORS
	errorsDEBUG.draw();
#endif

	//----

#ifndef INCLUDE_GUI_IM_GUI
//internal ofxGui panel
	if (SHOW_Gui_AdvancedControl)
	{
		gui_InternalControl.draw();
	}
#endif

	//----

	//user clicker boxes preset selector 
	//(from live kit/favorites)
	if (SHOW_ClickPanel)
	{
		drawPresetClicker();
	}

	//----

	//gui

#ifdef INCLUDE_GUI_IM_GUI
#ifndef MODE_ImGui_EXTERNAL
	//draw ImGui
	if (SHOW_ImGui)
	{
		ImGui_Draw_WindowBegin();

		//draw content
		bImGui_mouseOver = ImGui_Draw_Window();

		ImGui_Draw_WindowEnd();
	}
#else
	bImGui_mouseOver = false;
#endif
#endif
}


//--------------------------------------------------------------
void ofxPresetsManager::drawPresetClicker()
{
	//user trigger boxes clickable selector and save button
	//draws some minimalistic graphics to monitor the active preset
	//when graphics are drawn you can also click on them for saving/loading

	if (!lastMouseButtonState && ofGetMousePressed())
	{
		mousePressed(ofGetMouseX(), ofGetMouseY());
	}
	lastMouseButtonState = ofGetMousePressed();

	//-

	ofPushStyle();
	ofPushMatrix();

	ofTranslate(clicker_Pos);

	//----

	//0. bg box of all boxes background

	ofFill();
	ofSetColor(0, 128);
	ofDrawRectangle(0, 0,
		clicker_cellSize * (keys[0].size() + 2), clicker_cellSize*groups.size());

	//--

	ofNoFill();
	ofSetColor(ofColor::white);

	for (size_t i = 0; i < keys.size(); ++i)//draw all guis/groups. now using only one paramGroup!
	{
		//draw any preset box
		size_t k = 0;
		for (; k < keys[i].size(); ++k)
		{
			//1.1 outbox border container
			ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, clicker_cellSize, clicker_cellSize);

			//--

			//1.2 label boxes
			if (!myFont.isLoaded())//without ttf font
			{
				ofDrawBitmapString(ofToString((char)keys[i][k]),
					clicker_cellSize*k + 8,
					clicker_cellSize*i + 18);
			}
			else//custom font 
			{
				myFont.drawString(ofToString((char)keys[i][k]),
					clicker_cellSize * k + 0.5 * clicker_cellSize - 0.25 * sizeTTF,
					clicker_cellSize * i + 0.5 * clicker_cellSize + 0.5 * sizeTTF);
			}

			//--

			//2. inner box. double mark current selected preset
			if (lastIndices[i] == k)//it is selected
			{
				float r = 3.0f;
				float pd = 4.0f;
				int color = 200;

				ofPushStyle();
				ofSetColor(color, 164);

				//-

				//border only
				ofNoFill();
				ofDrawRectRounded(clicker_cellSize * k + pd, clicker_cellSize * i + pd,
					clicker_cellSize - 2 * pd, clicker_cellSize - 2 * pd,
					r);

				//-

				//filled
				ofFill();
				ofSetColor(color, 64);
				ofDrawRectRounded(clicker_cellSize * k + pd, clicker_cellSize * i + pd,
					clicker_cellSize - 2 * pd, clicker_cellSize - 2 * pd,
					r);
				ofNoFill();

				ofPopStyle();
			}
		}

		//--

		//?
		//for (; k < presets[i]; ++k)
		//{
		//	ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, 
		//		clicker_cellSize, clicker_cellSize);

		//	if (lastIndices[i] == k)
		//		ofDrawRectangle(clicker_cellSize * k + 4, clicker_cellSize * i + 4, clicker_cellSize - 8, clicker_cellSize - 8);
		//}

		//--

		//3. save button

		//box
		ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, clicker_cellSize, clicker_cellSize);

		//--

		//4. save label

		string _label;

		_label = "SAVE";
		int ySave = clicker_cellSize * i + 0.5 * clicker_cellSize + 0.5 * sizeTTF;
		if (!myFont.isLoaded())//without ttf font
		{
			ofDrawBitmapString(_label,
				clicker_cellSize*k + 8,
				clicker_cellSize*i + 18);
		}
		else//custom font 
		{
			float wx = 0.5f * myFont.getStringBoundingBox(_label, 0, 0).width;
			myFont.drawString(_label,
				clicker_cellSize * k + 0.5 * clicker_cellSize - wx,
				//clicker_cellSize * k + 0.5 * clicker_cellSize - 1.8f * sizeTTF,
				ySave);
		}
		k++;

		//--

		//5. gui toggle

		//box
		ofDrawRectangle(clicker_cellSize * (k), clicker_cellSize * i, clicker_cellSize, clicker_cellSize);

		//--

		//6. show gui label button

		_label = "GUI";

		if (!myFont.isLoaded())//without ttf font
		{
			ofDrawBitmapString(_label, clicker_cellSize*(k)+8, clicker_cellSize*i + 18);
		}
		else//custom font 
		{
			float wx = 0.5f * myFont.getStringBoundingBox(_label, 0, 0).width;
			myFont.drawString(_label,
				clicker_cellSize * (k)+0.5 * clicker_cellSize - wx,
				//clicker_cellSize * k + 0.5 * clicker_cellSize - 1.8f * sizeTTF,
				ySave);
		}

		//mark if gui visible
		if (SHOW_ImGui)
		{
			float r = 3.0f;
			float pd = 4.0f;
			int color = 200;

			ofSetColor(color, 164);

			//-

			////border only
			//ofNoFill();
			//ofDrawRectRounded(clicker_cellSize * k + pd, clicker_cellSize * i + pd,
			//	clicker_cellSize - 2 * pd, clicker_cellSize - 2 * pd,
			//	r);

			//-

			//filled
			ofFill();
			ofSetColor(color, 64);
			ofDrawRectRounded(clicker_cellSize * k + pd, clicker_cellSize * i + pd,
				clicker_cellSize - 2 * pd, clicker_cellSize - 2 * pd,
				r);
		}
		k++;

		//--

		//7. paramGroup / kit name

		if (SHOW_GroupName)
		{
			string info = groups[i].getName();

			//double font to improve different background colors
			int gap = 1;
			//int xG = clicker_cellSize * k + 15;

			float strW = 50;
			if (myFont.isLoaded()) {
				strW = myFont.getStringBoundingBox(info, 0, 0).width;
			}
			else {//no font
				strW = 100;
				ySave -= 10;
			}
			int xG = -strW - 20;
			ySave = ySave - 2;//little up

			ofSetColor(ofColor::black);//shadow
			if (myFont.isLoaded()) myFont.drawString(info, xG + gap, ySave + gap);
			else ofDrawBitmapString(info, xG + gap, ySave + gap);

			ofSetColor(ofColor::white);//white
			if (myFont.isLoaded()) myFont.drawString(info, xG, ySave);
			else ofDrawBitmapString(info, xG, ySave);
		}

		//--

		//8. help info text

		bool bLateralPosition = true;
		bool bLeftPosition = true;

		debugClicker = false;
		if (debugClicker && ENABLE_Keys)
		{
			string info = "";
			bool bSimpleInfo = true;
			if (bSimpleInfo)
			{
				//keys[i][k]
				info += "[keys " + ofToString((char)keys[0][0]) + "|";
				info += ofToString((char)keys[0][keys[0].size() - 1]) + "]";
			}
			else
			{
				info += "MOUSE-CLICK OR KEYS [1-8] TO LOAD PRESET\n";
				info += "HOLD [CTRL] TO SAVE/COPY PRESET\n";
			}

			int x;
			int y;
			int gap = 1;
			int pad = 13;

			if (!bLateralPosition)//A. vertical position below boxes
			{
				x = 0;
				y = (clicker_cellSize + 15) * groups.size();
			}
			else//B. lateral position right to the boxes
			{
				if (!bLeftPosition) {
					x = clicker_cellSize * k + pad;
					y = ySave - (bSimpleInfo ? -2 : sizeTTF);
				}
				else {
					float strW = 50;
					if (myFont.isLoaded()) {
						strW = myFont.getStringBoundingBox(info, 0, 0).width;
					}
					int xG = -strW - 20;

					x = xG;
					y = ySave + sizeTTF + 10;
				}
			}

			//double font to improve different background colors
			ofSetColor(ofColor::black);//shadow
			if (myFont.isLoaded()) myFont.drawString(info, x + gap, y + gap);
			else ofDrawBitmapString(info, x + gap, y + gap);

			ofSetColor(ofColor::white);//white
			if (myFont.isLoaded()) myFont.drawString(info, x, y);
			else ofDrawBitmapString(info, x, y);
		}

		//----

		ofPopStyle();
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
ofxPresetsManager::~ofxPresetsManager()
{
	//exit();
}

//-

#pragma mark - ENGINE

//--------------------------------------------------------------
string ofxPresetsManager::getPresetName(string gName, int presetIndex)
{
	string strFolder;
	string strFile;
	string strPath;

	strFolder = path_GLOBAL_Folder + "/" + path_Kit_Folder + "/";
	strFile = groupName + path_Prefix + ofToString(presetIndex) + ".xml";
	strPath = strFolder + strFile;

	return strPath;
}

//-

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup params, int _num_presets)//main adder
{
	//main adder of a ofParameterGroup for preset management

	ofLogNotice(__FUNCTION__) << "added group";

	groups.push_back(params);//each enqueued group-param handles all presets of each added paramGroup

	//initializes?
	lastIndices.push_back(0);//?
	newIndices.push_back(0);//?
	presetsOnGroup.push_back(_num_presets);//?

	//-

	//update control gui panel params
	numPresetsFavorites = _num_presets;
	PRESET_selected.setMax(numPresetsFavorites - 1);

	//TODO:
	//PRESET2_selected.setMax(numPresetsFavorites);

	//TODO:
	presetsXmlArray.resize(numPresetsFavorites);

	//-

	//used for path folder and xml presets file names

	//TODO: 
	//one group only
	groupName = groups[0].getName();
	//groupName2 = groups[1].getName();

	ofLogNotice(__FUNCTION__) << "groupName: " << groupName;

	//-

	//temporary name only to debug purposes
	//final label name to gui display will be setted if setup("name") is called 
	gui_LabelName = groups[0].getName();
	//TODO: 
	//one group only

	//-

	//memory mode
	load_AllKit_ToMemory();
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup params, initializer_list<int> keysList)
{
	//adds a ofParameterGroup for preset management with the list of trigger keys

	add(params, keysList.size());//re add with known size counting passed key triggers

	keys.resize(groups.size());
	int i = groups.size() - 1;

	keys[i].reserve(keysList.size());

	for (const int &key : keysList)
		keys[i].push_back(key);

	if (keysNotActivated)
		addKeysListeners();
}

//-

//--------------------------------------------------------------
void ofxPresetsManager::save(int presetIndex, int guiIndex)
{
	ofLogVerbose(__FUNCTION__) << "(" << presetIndex << "," << guiIndex << ")";

	//clamp limiters
	if (guiIndex >= 0 && guiIndex < (int)groups.size()
		&& (presetIndex >= 0) && (presetIndex < presetsXmlArray.size()))
	{
		ofLogVerbose(__FUNCTION__) << "DONE_save (1)";

		//it's important if this line is before or after ofSerialize
		DONE_save = true;

		//-

		//MODE A: from hd file

		if (!MODE_MemoryLive)
		{
			TS_START("SAVE FILE 1");

			std::string strPath = getPresetName(groups[guiIndex].getName(), presetIndex);
			ofXml settings;
			ofSerialize(settings, groups[guiIndex]);
			settings.save(strPath);

			TS_STOP("SAVE FILE 1");
		}

		//-

		//MODE B: direct from memory

		else
		{
			TS_START("SAVE MEM 1");

			//ofLogNotice(__FUNCTION__) << "MEMORY MODE";

			if (presetIndex < presetsXmlArray.size()) {
				ofSerialize(presetsXmlArray[presetIndex], groups[guiIndex]);
			}
			else {
				ofLogError(__FUNCTION__) << "presetsXmlArray OUT OF RANGE";
			}

			TS_STOP("SAVE MEM 1");
		}
	}
	else
	{
		ofLogError(__FUNCTION__) << "OUT OF RANGE SAVE";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::save(int presetIndex, string gName)
{
	ofLogVerbose(__FUNCTION__) << "save(" << presetIndex << "," << gName << ")";

	int guiIndex = getGuiIndex(gName);

	if (guiIndex >= 0 && guiIndex < (int)groups.size()
		&& (presetIndex >= 0) && (presetIndex < presetsXmlArray.size()))
	{
		ofLogVerbose(__FUNCTION__) << "DONE_save (2)";

		//it's important if this line is before or after ofSerialize
		DONE_save = true;

		//-

		//MODE A: from hd file

		if (!MODE_MemoryLive)
		{
			TS_START("SAVE FILE 2");//for TimeMeasurements only

			std::string strPath = getPresetName(gName, presetIndex);
			ofXml settings;
			ofSerialize(settings, groups[guiIndex]);
			settings.save(strPath);

			TS_STOP("SAVE FILE 2");//for TimeMeasurements only
		}

		//-

		//MODE B: direct from memory

		else
		{
			TS_START("SAVE MEM 2");//for TimeMeasurements only

			if (presetIndex < presetsXmlArray.size()) {
				ofSerialize(presetsXmlArray[presetIndex], groups[guiIndex]);
			}
			else {
				ofLogError(__FUNCTION__) << "presetsXmlArray OUT OF RANGE";
			}

			TS_STOP("SAVE MEM 2");//for TimeMeasurements only
		}
	}
	else
	{
		ofLogError(__FUNCTION__) << "OUT OF RANGE SAVE";
	}

}

//--------------------------------------------------------------
void ofxPresetsManager::load(int presetIndex, int guiIndex)
{
	ofLogVerbose(__FUNCTION__) << "(" << presetIndex << "," << guiIndex << ")";

	//clamp limiters
	if (guiIndex >= 0 && guiIndex < (int)groups.size()
		&& (presetIndex >= 0) && (presetIndex < presetsXmlArray.size()))//TODO:
		//&& (presetIndex >= 0) && (presetIndex < lastIndices[0]))//TODO:
	{
		if (!MODE_MemoryLive)
		{
			//MODE A: from hd file
			TS_START("LOAD FILE 1");//for TimeMeasurements only
			std::string strPath = getPresetName(groups[guiIndex].getName(), presetIndex);
			ofXml settings;
			settings.load(strPath);
			ofDeserialize(settings, groups[guiIndex]);
			TS_STOP("LOAD FILE 1");//for TimeMeasurements only
		}
		else
		{
			//MODE B: direct from memory
			TS_START("LOAD MEM 1");//for TimeMeasurements only
			//using xml array
			if (presetIndex < presetsXmlArray.size()) {
				ofDeserialize(presetsXmlArray[presetIndex], groups[guiIndex]);
			}
			else {
				ofLogError(__FUNCTION__) << "presetsXmlArray OUT OF RANGE";
			}
			TS_STOP("LOAD MEM 1");//for TimeMeasurements only
		}

		//-

		//mark selected
		lastIndices[guiIndex] = presetIndex;

		//-

		//callback
		ofLogVerbose(__FUNCTION__) << "DONE_load (1)";
		DONE_load = true;

		//simple callback
		bIsDoneLoad = true;
	}
	else
	{
		ofLogError(__FUNCTION__) << "OUT OF RANGE LOAD";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load(int presetIndex, string gName)
{
	ofLogVerbose(__FUNCTION__) << "(" << presetIndex << "," << gName << ")";

	int guiIndex = getGuiIndex(gName);

	if (guiIndex >= 0 && guiIndex < (int)groups.size()
		&& (presetIndex >= 0) && (presetIndex < presetsXmlArray.size()))
	{
		if (!MODE_MemoryLive)
		{
			//MODE A: from hd file
			TS_START("LOAD FILE 2");//for TimeMeasurements only
			std::string strPath = getPresetName(gName, presetIndex);
			ofXml settings;
			settings.load(strPath);
			ofDeserialize(settings, groups[guiIndex]);
			TS_STOP("LOAD FILE 2");//for TimeMeasurements only
		}
		else
		{
			//MODE B: direct from memory
			TS_START("LOAD MEM 2""LOAD MEM 2");//for TimeMeasurements only
			if (presetIndex < presetsXmlArray.size()) {
				ofDeserialize(presetsXmlArray[presetIndex], groups[guiIndex]);
			}
			else {
				ofLogError(__FUNCTION__) << "presetsXmlArray OUT OF RANGE LOAD";
			}
			TS_STOP("LOAD MEM 2");//for TimeMeasurements only
		}

		//-

		//mark selected
		lastIndices[guiIndex] = presetIndex;

		//-

		ofLogVerbose(__FUNCTION__) << "DONE_load (2)";
		DONE_load = true;

		//TODO:
		//simple callback
		bIsDoneLoad = true;
	}
	else
	{
		ofLogError(__FUNCTION__) << "OUT OF RANGE LOAD";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::loadPreset(int p)
{
	if (!DISABLE_CALLBACKS)
		// && (PRESET_selected != PRESET_selected_PRE))
	{
		ofLogNotice(__FUNCTION__) << ofToString(p);

		//TODO:
		//mode latch
		if (MODE_LatchTrig && bLatchRun)
		{
			if (p != 0)
			{
				bLatchRun = true;
			}
			else if (p == 0)
			{
				bLatchRun = false;
			}

			//randomizerTimer = ofGetElapsedTimeMillis();
		}

		if (PRESET_selected >= 0 && PRESET_selected <= numPresetsFavorites - 1)
		{
			PRESET_selected = p;

			//PRESET_selected_PRE = PRESET_selected;//TODO:
		}
		else
		{
			ofLogError(__FUNCTION__) << "OUT OF RANGE! CLAMP PRESET TO 0";

			//workaround
			//clamp
			PRESET_selected = 0;//set to first as default presets when out of range
		}
	}
}

//--

//engine helpers
//--------------------------------------------------------------
int ofxPresetsManager::getPresetIndex(int guiIndex) const
{
	if (guiIndex > 0 && guiIndex < (int)groups.size())
	{
		return lastIndices[guiIndex];
	}
	else
	{
		return -1;
	}
}

//--------------------------------------------------------------
int ofxPresetsManager::getPresetIndex(string gName) const
{
	int guiIndex = getGuiIndex(gName);

	if (guiIndex > 0 && guiIndex < (int)groups.size())
	{
		return lastIndices[guiIndex];
	}
	else
	{
		return -1;
	}
}

//--------------------------------------------------------------
int ofxPresetsManager::getGuiIndex(string name) const
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
void ofxPresetsManager::setModeKeySave(int key)
{
	modeKeySave = key;
}

//--------------------------------------------------------------
void ofxPresetsManager::setModeKeySwap(int key)
{
	modKeySwap = key;
}

#pragma mark - OF_LISTENERS

//----------------------------------------------------------------
void ofxPresetsManager::keyPressed(ofKeyEventArgs &eventArgs)
{
#ifndef INCLUDE_GUI_IM_GUI
	if (bKeys && ENABLE_Keys)
#else
	if (bKeys && ENABLE_Keys && !bImGui_mouseOver)//disable keys when mouse over gui
#endif
	{
		const int &key = eventArgs.key;

		//-

		//mode key for saving with mouse or trigger keys
		if (key == modeKeySave)
		{
			bKeySave = true;
			ofLogVerbose(__FUNCTION__) << "modeKeySave TRUE" << endl;
			return;
		}

		//mode key for swap with mouse or trigger keys
		else if (key == modKeySwap)
		{
			bKeySwap = true;
			ofLogVerbose(__FUNCTION__) << "modKeySwap TRUE" << endl;
			return;
		}


		//-

		//TODO: 
		//controlled from outside
		//hide/show control gui
		if (key == 'G')
		{
			SHOW_Gui_AdvancedControl = !SHOW_Gui_AdvancedControl;
			setVisible_GUI_Internal(SHOW_Gui_AdvancedControl);
			setVisible_GUI_ImGui(SHOW_Gui_AdvancedControl);
			setVisible_PresetClicker(SHOW_Gui_AdvancedControl);
		}

		//if (key == 'g')
		//{
		//	setVisible_PresetClicker(!isVisible_PresetClicker());
		//}

		//-

#ifdef INCLUDE_RANDOMIZER
		bool bEnableKeyRandomizers = false;
		if (bEnableKeyRandomizers) {
			//timer to randomize and choice a random preset from the kit
			if (key == 'R')
			{
				setTogglePlayRandomizerPreset();
			}
			else if (key == 'r')
			{
				doRandomizeWichSelectedPreset();
			}

			else if (key == 'E')
			{
				doRandomizeEditor();
			}
		}
#ifdef INCLUDE_ofxUndoSimple
		if (key == 'A')// previous
		{
			ofLogNotice(__FUNCTION__) << "UNDO <-";
			undoStringParams.undo();
			undoRefreshParams();
		}
		else if (key == 'D')// next
		{
			ofLogNotice(__FUNCTION__) << "REDO ->";
			undoStringParams.redo();
			undoRefreshParams();
		}
		else if (key == 'Q')// clear history
		{
			ofLogNotice(__FUNCTION__) << "UNDO CLEAR";
			undoStringParams.clear();
		}
#endif

#endif

		//-

		//navigate kit/favorites presets
		if (key == OF_KEY_RIGHT && ENABLE_KeysArrowBrowse)
		{

			//browse presets
#ifdef INCLUDE_GUI_IM_GUI
			if (MODE_Browser_NewPreset)
			{
				currentFile++;
				if (currentFile > files.size() - 1) currentFile = files.size() - 1;
				browser_PresetName = fileNames[currentFile];
				ofLogNotice(__FUNCTION__) << "[>] LOAD" << endl;
				ofLogNotice(__FUNCTION__) << "Preset Name: " << browser_PresetName;
				browser_PresetLoad(browser_PresetName);
			}
			else
#endif
				//browse favorites
			{
				int i = PRESET_selected;
				i++;

				if (i > presetsXmlArray.size() - 1) i = presetsXmlArray.size() - 1;
				PRESET_selected = i;
			}
		}

		else if (key == OF_KEY_LEFT && ENABLE_KeysArrowBrowse)
		{
			//browse presets
#ifdef INCLUDE_GUI_IM_GUI
			if (MODE_Browser_NewPreset)
			{
				currentFile--;
				if (currentFile < 0) currentFile = 0;
				browser_PresetName = fileNames[currentFile];
				ofLogNotice(__FUNCTION__) << "[<] LOAD" << endl;
				ofLogNotice(__FUNCTION__) << "Preset Name: " << browser_PresetName;
				browser_PresetLoad(browser_PresetName);
			}
			else
#endif
				//browse favorites
			{
				int i = PRESET_selected;
				i--;
				if (i < 0) i = 0;
				PRESET_selected = i;
			}
		}

		//--

		//main shortcuts:
		//preset selector (like from 1 to 8)

		for (int i = 0; i < keys.size(); ++i)//i=witch group. only performs i=0 bc there's only one implemented group!
		{
			for (int k = 0; k < keys[i].size(); ++k)//performs all registered keys: one for each [8] preset
			{
				if (key == keys[i][k])
				{
					ofLogNotice(__FUNCTION__) << "Key: " << (char)key;

					if (bKeySave)
					{
						save(k, i);
					}
					else
					{
						ofLogNotice(__FUNCTION__) << "[" << k << "][" << i << "]";

						PRESET_selected = k;
					}
					return;
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::keyReleased(ofKeyEventArgs &eventArgs)
{
	//mod keys
	if (eventArgs.key == modeKeySave && ENABLE_Keys)
	{
		bKeySave = false;
		ofLogVerbose(__FUNCTION__) << "modeKeySave FALSE" << endl;
	}
	else if (eventArgs.key == modKeySwap && ENABLE_Keys)
	{
		bKeySwap = false;
		ofLogVerbose(__FUNCTION__) << "modKeySwap FALSE" << endl;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::addKeysListeners()
{
	ofAddListener(ofEvents().keyPressed, this, &ofxPresetsManager::keyPressed);
	ofAddListener(ofEvents().keyReleased, this, &ofxPresetsManager::keyReleased);
	keysNotActivated = false;
	bKeys = true;
}

//--------------------------------------------------------------
void ofxPresetsManager::removeKeysListeners()
{
	ofRemoveListener(ofEvents().keyPressed, this, &ofxPresetsManager::keyPressed);
	ofRemoveListener(ofEvents().keyReleased, this, &ofxPresetsManager::keyReleased);
}

//-----------------------------------------------------
void ofxPresetsManager::mousePressed(int x, int y)
{
	x = x - clicker_Pos.x;
	y = y - clicker_Pos.y;

	int xIndex = x / clicker_cellSize;
	int yIndex = y / clicker_cellSize;//yIndex is for when we add more groups feature! (just one group for now)

	//Index -1 for out (left) of boxes
	xIndex = (x > 0) ? xIndex : -1;
	yIndex = (y > 0) ? yIndex : -1;

	//debug mouse out of click cells
	if (xIndex != -1 && yIndex != -1)
		ofLogVerbose(__FUNCTION__) << "(" << xIndex << "," << yIndex << ")";

	//-

	//1. key presets buttons & save button
	if (yIndex >= 0 && yIndex < (int)groups.size())// + 1)//+1 for extra gui button
	{
		//avoid outer panel logs. only into the group row levels
		if (xIndex != -1 && yIndex != -1)
			ofLogVerbose(__FUNCTION__) << "(" << xIndex << "," << yIndex << ")";

		//-

		if (xIndex >= 0 && xIndex < presetsOnGroup[yIndex])//? presets[0] its's the same than numPresetsFavorites!
		//if (xIndex >= 0 && xIndex < numPresetsFavorites)
		{
			//1. mod save controlled by modeKeySave
			if (bKeySave)
			{
				//save "memory" to the clicked Index
				doSave(xIndex);

				//will auto load
				PRESET_selected = xIndex;
				//PRESET_selected = 1 + xIndex;
			}

			//-

			//2. mod swap controlled by modKeySwap
			else if (bKeySwap)
			{
				int IndexSource = PRESET_selected;
				//int IndexSource = PRESET_selected - 1;
				int IndexDest = xIndex;
				string srcName = getPresetName(groups[0].getName(), IndexSource);
				string dstName = getPresetName(groups[0].getName(), IndexDest);

				//cout << "Source: " << IndexSource + 1 << endl;
				//cout << "Dest  : " << xIndex + 1 << endl;
				//cout << "Source: " << srcName << endl;
				//cout << "Dest  : " << dstName << endl << endl;

				//1. save source preset (from memory) to temp file
				string _pathSrc = "tempSrc.xml";
				ofXml settingsSrc;
				ofSerialize(settingsSrc, groups[0]);
				settingsSrc.save(_pathSrc);

				//2. load destination "from kit" to memory
				std::string _path2 = getPresetName(groups[0].getName(), IndexDest);
				ofXml settings2;
				settings2.load(_path2);
				ofDeserialize(settings2, groups[0]);

				//3. save destination preset (from memory) to temp file
				ofXml settingsDst;
				ofSerialize(settingsDst, groups[0]);

				//4. using files
				//save source (from dest)
				settingsDst.save(srcName);
				settingsSrc.save(dstName);

				//5. delete temp file
				ofFile _file;
				_file.removeFile(_pathSrc);

				//6. auto load source (the same preset was selected befor swap clicked!)
				PRESET_selected = xIndex;
				//PRESET_selected = 1 + xIndex;
			}

			//-

			//normal load (not any key modifier pressed)
			else
			{
				//will trig autoload callback on change, calling load(p, 0);
				PRESET_selected = xIndex;
				//PRESET_selected = 1 + xIndex;
			}
		}

		//2. last button (save button)
		else if (xIndex == presetsOnGroup[yIndex])
		{
			ofLogNotice(__FUNCTION__) << "saveButton: (" << yIndex << ")";

			doSave(lastIndices[yIndex]);
			//save( lastIndices[yIndex], yIndex );
		}

	}

	//-

	//TODO:
	//3. toggle gui
	if (yIndex == 0 && xIndex == presetsOnGroup[yIndex] + 1)
	{
		ofLogNotice(__FUNCTION__) << "toggle SHOW_ImGui : " << SHOW_ImGui;
		SHOW_ImGui = !SHOW_ImGui;
	}
}

//----

//--------------------------------------------------------------
void ofxPresetsManager::doCloneRight(int pIndex)
{
	ofLogNotice(__FUNCTION__) << "from preset: " << pIndex;

	CheckAllFolder();

	for (int i = pIndex + 1; i < numPresetsFavorites; i++)
	{
		save(i, 0);
		//TODO: 0 it's bc it's the only 1st params group implemented
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doPopulateFavs()
{
	ofLogNotice(__FUNCTION__);

	CheckAllFolder();

	for (int i = 0; i < numPresetsFavorites; i++)
	{
		doRandomizeEditor();
		save(i, 0);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doCloneAll()
{
	ofLogNotice(__FUNCTION__);

	CheckAllFolder();

	//auto save current preset
	if (autoSave)
	{
		ofLogVerbose(__FUNCTION__) << "autosave preset: " << PRESET_selected.get();
		doSave(PRESET_selected);
	}

	//clone all
	for (int i = 0; i < numPresetsFavorites; i++)
	{
		save(i, 0);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doLoad(int pIndex)
{
	ofLogVerbose(__FUNCTION__) << "preset: " << pIndex;
	load(pIndex, 0);
}

//--------------------------------------------------------------
void ofxPresetsManager::doSave(int pIndex)
{
	ofLogVerbose(__FUNCTION__) << "preset: " << pIndex;
	save(pIndex, 0);
}

//--------------------------------------------------------------
void ofxPresetsManager::setToggleKeysControl(bool active)
{
	bKeys = active;
}

#pragma mark - CALLBACKS

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Params_Editor(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		ofLogNotice(__FUNCTION__) << groups[0].getName() << " " << name << " : " << e;

		//-

		if (name == bRandomizeEditor.getName() && bRandomizeEditor)//trig randomize
		{
			bRandomizeEditor = false;

			doRandomizeEditor();
		}
		else if (name == bRandomizeEditorAll.getName() && bRandomizeEditorAll)//all
		{
			bRandomizeEditorAll = false;

			for (auto &p : editorPresets) {
				p.set(true);
			}
		}
		else if (name == bRandomizeEditorNone.getName() && bRandomizeEditorNone)//none
		{
			bRandomizeEditorNone = false;

			for (auto &p : editorPresets) {
				p.set(false);
			}
		}
		else if (name == bRandomizeEditorPopulateFavs.getName() && bRandomizeEditorPopulateFavs)//populate random all favs
		{
			bRandomizeEditorPopulateFavs = false;
			doPopulateFavs();
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Params_Control(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		if ((name != "exclude")
			&&
			(name != ImGui_Position.getName()) &&
			(name != ImGui_Size.getName()) &&
			//(name != "DICE") &&
			(name != "PRESET")
			)
		{
			ofLogNotice(__FUNCTION__) << groups[0].getName() << " " << name << " : " << e;
		}

		//-

		if (name == "PRESET") {
			ofLogVerbose(__FUNCTION__) << groups[0].getName() << " -------------------------------------------------------------";
		}

		//-

		if (name == "SAVE" && bSave)
		{
			ofLogNotice(__FUNCTION__) << "SAVE: " << e;
			bSave = false;
			doSave(PRESET_selected);
		}

		//else if (name == "LOAD" && bLoad)
		//{
		//	ofLogNotice(__FUNCTION__) << "LOAD: " << e;
		//	bLoad = false;
		//	doLoad(PRESET_selected);
		//}

		//-

		//helper tools
		else if (name == "CLONE >" && bCloneRight)
		{
			ofLogNotice(__FUNCTION__) << "CLONE >: " << e;
			bCloneRight = false;
			doCloneRight(PRESET_selected);
		}
		else if (name == "CLONE ALL" && bCloneAll)
		{
			ofLogNotice(__FUNCTION__) << "CLONE ALL: " << e;
			bCloneAll = false;
			doCloneAll();
		}

		//--

		//randomizer
#ifdef INCLUDE_GUI_IM_GUI
		else if (name == "SHOW CLICK PANEL" && !SHOW_ClickPanel.get())
		{
			SHOW_ImGui = false;//workflow
		}
		else if (name == "RANDOMIZE INDEX" && bRandomizeIndex)
		{
			//ofLogNotice(__FUNCTION__) << "RANDOMIZE !";
			bRandomizeIndex = false;

			doRandomizeWichSelectedPreset();
		}
		else if (name == PLAY_RandomizeTimer.getName())//"PLAY RANDOMIZER"
		{
			ofLogNotice(__FUNCTION__) << "MODE TIMER: " << e;
			if (PLAY_RandomizeTimer) {
				MODE_LatchTrig = false;
			}
		}
		else if (name == "MODE LATCH")
		{
			ofLogNotice(__FUNCTION__) << "MODE LATCH: " << e;

			if (MODE_LatchTrig) {
				PLAY_RandomizeTimer = false;
			}
		}
		//else if (name == "SPEED FACTOR")
		//{
		//	ofLogNotice(__FUNCTION__) << "SPEED FACTOR: " << e;
		//
		//	//randomizeDuration = randomize_MAX_DURATION * (1.f - randomizeSpeedF);
		//
		//	randomizeDuration.setMax(randomizeSpeedF * randomize_MAX_DURATION);
		//	randomizeDurationShort.setMax(randomizeSpeedF * randomize_MAX_DURATION_SHORT);
		//}

		else if (name == randomizeDuration.getName())
		{
			ofLogNotice(__FUNCTION__) << "DURATION: " << e;

			randomizeDurationBpm = 60000.f / randomizeDuration;

			//randomizeSpeedF = -((float)randomizeDuration / (float)randomize_MAX_DURATION) + 1.f;
			////randomizeSpeedF = 1 + (randomizeDuration / (float)randomize_MAX_DURATION);
		}
		else if (name == randomizeDurationBpm.getName())
		{
			ofLogNotice(__FUNCTION__) << "BPM: " << e;

			//60,000 ms (1 minute) / Tempo (BPM) = Delay Time in ms for quarter-note beats
			randomizeDuration = 60000.f / randomizeDurationBpm;
			randomizeDurationShort = randomizeDuration / 4.f;
		}

		//TODO:
#ifdef DEBUG_randomTest
		else if (name == "DICE")//when debug enabled: set dice by user to test
		{
			ofLogNotice(__FUNCTION__) << "DICE: " << e;
			doRandomizeWichSelectedPreset();
		}
#endif
		else if (name == "RESET DICES" && bResetDices)
		{
			ofLogNotice(__FUNCTION__) << "RESET DICES: " << e;
			doResetDices();
			bResetDices = false;
		}
#endif

		//--

		else if (name == "ENABLE KEYS")
		{
		}

		//--

#ifndef INCLUDE_GUI_IM_GUI
		else if (name == "GUI INTERNAL POSITION")
		{
			ofLogVerbose(__FUNCTION__) << "GUI POSITION: " << e;
			//clamp inside window
			float x, y;
			x = ofClamp(Gui_Internal_Position.get().x, 0, ofGetWidth() - 200);
			y = ofClamp(Gui_Internal_Position.get().y, 0, ofGetHeight() - 20);
			gui_InternalControl.setPosition(x, y);
		}
#endif

#ifdef INCLUDE_GUI_IM_GUI
		else if (name == "GUI ImGui POSITION")
		{
			ofLogVerbose(__FUNCTION__) << "GUI BROWSER POSITION: " << e;

			////clamp inside window
			//float x, y;
			//x = ofClamp(ImGui_Position.get().x, 0, ofGetWidth() - 200);
			//y = ofClamp(ImGui_Position.get().y, 0, ofGetHeight() - 20);
			//ImGui_Position = glm::vec2(x, y);
		}
		else if (name == "GUI ImGui SIZE")
		{
			ofLogVerbose(__FUNCTION__) << "GUI BROWSER SIZE: " << e;

			////clamp inside window
			//float x, y;
			//x = ofClamp(ImGui_Position.get().x, 0, ofGetWidth() - 200);
			//y = ofClamp(ImGui_Position.get().y, 0, ofGetHeight() - 20);
			//ImGui_Position = glm::vec2(x, y);
		}
#endif
		//--

		else if (name == "LOAD TO MEMORY" && loadToMemory)
		{
			ofLogNotice(__FUNCTION__) << "loadToMemory:" << e;
			loadToMemory = false;
			load_AllKit_ToMemory();
		}

		else if (name == "SAVE FROM MEMORY" && saveFromMemory)
		{
			ofLogNotice(__FUNCTION__) << "saveFromMemory:" << e;
			saveFromMemory = false;
			save_AllKit_FromMemory();
		}

		else if (name == "MODE MEMORY")
		{
			ofLogNotice(__FUNCTION__) << "MODE MEMORY: " << e;

			if (MODE_MemoryLive)
			{
				//reload all xml preset files to memory
				load_AllKit_ToMemory();
			}
			else
			{
				//save all xml preset files to disk from memory
				save_AllKit_FromMemory();
			}
		}


		//----

		//PRESET

		//-

		//1. selected preset NOT CHANGED

		//TODO: 
		//should be nice to add toggle to auto retrig or not behavior 

		else if (name == "PRESET" && (PRESET_selected == PRESET_selected_PRE))
		{
			ofLogNotice(__FUNCTION__) << groups[0].getName() << " PRESET " << e << "  [NOT Changed]";

			//browser
#ifdef INCLUDE_GUI_IM_GUI
			if (MODE_Browser_NewPreset)
			{
				if (autoLoad)
				{
					load(PRESET_selected, 0);
				}
			}
#endif
			bMustTrig = true;
		}

		//-

		//2. selected preset CHANGED

		else if (name == "PRESET" && (PRESET_selected != PRESET_selected_PRE))
		{
			ofLogNotice(__FUNCTION__) << groups[0].getName() << " PRESET " << PRESET_selected;

			//-

#ifndef INCLUDE_GUI_IM_GUI
			if (autoSave)// && autoLoad)
#else
			if (autoSave && !MODE_Browser_NewPreset)//workflow: browser mode bypasses autosave
#endif
			{
				save(PRESET_selected_PRE, 0);
			}

			//-

			//remember this PRE state to know if changed or not on the next time..
			PRESET_selected_PRE = PRESET_selected;

			//-

			//indexes starts from 0, not from 1 like presets
			int xIndex = PRESET_selected;
			int yIndex = 0;//TODO: one group only for now

			//-

			if (autoLoad)
			{
				load(xIndex, yIndex);
			}
			else
			{
				if (lastIndices.size() > 0//amount of groups
					&& yIndex < lastIndices.size()//amount of groups
					&& xIndex < presetsXmlArray.size())
				{
					//mark selector as lastone trigged
					lastIndices[yIndex] = xIndex;
				}
				else
				{
					ofLogError(__FUNCTION__) << "lastIndices has 0 size!";
				}
			}
		}

		//--

		else
		{
			bool doDices = false;
			for (int i = 0; i < presetsRandomFactor.size(); i++) {
				if (name == "PROB " + ofToString(i)) {
					doDices = true;
				}
			}
			if (doDices)
			{
				//sum total dices/all probs
				dicesTotalAmount = 0;
				for (auto &p : presetsRandomFactor) {
					dicesTotalAmount += p.get();
				}
				randomizedDice.setMax(dicesTotalAmount - 1);

				ofLogNotice(__FUNCTION__) << "dicesTotalAmount: " << dicesTotalAmount;
			}
		}

		//else
		//{
		//	ofLogError(__FUNCTION__) << "IGNORED PRESETS CHANGE";
		//}
	}
}

#pragma mark - SETTINGS

//--------------------------------------------------------------
void ofxPresetsManager::load_ControlSettings()
{
	//ofLogNotice(__FUNCTION__);

	string path = path_Control + "/" + filename_ControlSettings;
	bool b = ofxSurfingHelpers::loadGroup(params_Control, path);
	if (!b)
	{
#ifdef INCLUDE_DEBUG_ERRORS
		errorsDEBUG.addError(ofToString(__FUNCTION__), gui_LabelName, path);
#endif
		ofLogError(__FUNCTION__) << "FILE '" << path << "' NOT FOUND!";
	}
	ofLogNotice(__FUNCTION__) << path;
	ofLogNotice(__FUNCTION__) << "PRESET " << PRESET_selected;

	//-

	//load randomizers settings
	string path2 = path_GLOBAL_Folder + "/" + path_Control + "/" + filename_Randomizers;
	bool b2 = ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path2);
	ofLogNotice(__FUNCTION__) << "Loaded " << path2 << " " << (b2?"DONE":"FAILED");


	//	ofXml settings;
	//	string path = path_Control + "/" + filename_ControlSettings;
	//	//string path = path_GLOBAL_Folder + "/" + path_Control + "/" + filename_ControlSettings;
	//	bool bLoaded = settings.load(path);
	//
	//#ifdef INCLUDE_DEBUG_ERRORS
	//	if (!bLoaded)
	//	{
	//		errorsDEBUG.addError(ofToString(__FUNCTION__), gui_LabelName, path);
	//	}
	//#endif
	//
	//	ofLogNotice(__FUNCTION__) << path;
	//	ofLogNotice(__FUNCTION__) << "PRESET " << PRESET_selected;
	//
	//	if (bLoaded)
	//	{
	//		ofDeserialize(settings, params_Control);
	//	}
	//	else
	//	{
	//		ofLogError(__FUNCTION__) << "FILE '" << path << "' NOT FOUND!";
	//	}
}

//--------------------------------------------------------------
void ofxPresetsManager::save_ControlSettings()
{
	ofLogNotice(__FUNCTION__);

#ifndef DEBUG_BLOCK_SAVE_SETTINGS

	//TODO:
	//crashes!
	//it seems related to autoSave timer?

	DISABLE_CALLBACKS = true;

	//-

	//TODO: 
	//crashes sometimes?
#ifndef INCLUDE_GUI_IM_GUI
	//get gui position from panel to update the position param
	Gui_Internal_Position = glm::vec2(gui_InternalControl.getPosition());

	//float x, y;
	//x = gui_InternalControl.getPosition().x;
	//y = gui_InternalControl.getPosition().y;
	//Gui_Internal_Position = glm::vec2(x, y);
#endif

	//---

	//TODO: 
	//crashes?
	try
	{
		//save randomizers settings
		ofLogVerbose(__FUNCTION__) << endl << params_Control.toString() << endl;
		string path = path_Control + "/" + filename_ControlSettings;
		bool b = ofxSurfingHelpers::saveGroup(params_Control, path);
		if (!b)
		{
#ifdef INCLUDE_DEBUG_ERRORS
			errorsDEBUG.addError(ofToString(__FUNCTION__), gui_LabelName, path);
#endif
			ofLogError(__FUNCTION__) << "FILE '" << path << "' NOT FOUND!";
		}
		ofLogNotice(__FUNCTION__) << path;
		ofLogNotice(__FUNCTION__) << "PRESET " << PRESET_selected;

		//ofXml settingsControl;
		////TODO: 
		////crashes here!
		//ofSerialize(settingsControl, params_Control);
		//string path = path_Control + "/" + filename_ControlSettings;
		////string path = path_GLOBAL_Folder + "/" + path_Control + "/" + filename_ControlSettings;
		//ofLogNotice(__FUNCTION__) << path;
		////TODO: 
		////crashes here!
		//settingsControl.save(path);
		////ofLogNotice(__FUNCTION__) << settingsControl << endl;

		//-

		//save randomizers settings
		string path2 = path_GLOBAL_Folder + "/" + path_Control + "/" + filename_Randomizers;
		bool b2 = ofxSurfingHelpers::saveGroup(params_RandomizerSettings, path2);
		ofLogNotice(__FUNCTION__) << "Saved " << path2 << " " << (b2 ? "DONE" : "FAILED");
	}
	catch (int n)
	{
		ofLogError(__FUNCTION__) << "CATCH ERROR" << endl;
		throw;
}

	//---

	DISABLE_CALLBACKS = false;
#else
	ofLogNotice(__FUNCTION__) << "[DEBUG] BLOCKED save_ControlSettings()";
#endif
}

//--

//--------------------------------------------------------------
void ofxPresetsManager::setPath_GlobalFolder(string folder)
{
	ofLogNotice(__FUNCTION__) << folder;
	path_GLOBAL_Folder = folder;
	CheckFolder(folder);
}

//--------------------------------------------------------------
void ofxPresetsManager::setPath_KitFolder(string folder)
{
	ofLogNotice(__FUNCTION__) << folder;
	path_Kit_Folder = folder;
	CheckFolder(path_GLOBAL_Folder + "/" + path_Kit_Folder);
}

//--------------------------------------------------------------
void ofxPresetsManager::setPath_PresetsFolder(string folder)
{
	ofLogNotice(__FUNCTION__) << folder;
	path_PresetsFolder = folder;
	CheckFolder(path_GLOBAL_Folder + "/" + path_PresetsFolder);
}

//--

//memory mode 
//(loaded from data vector instead of hd files)
//--------------------------------------------------------------
void ofxPresetsManager::save_AllKit_FromMemory()
{
	ofLogVerbose(__FUNCTION__);

	for (int i = 0; i < presetsXmlArray.size(); i++)
	{
		string strFolder;
		string strFile;
		string strPath;

		strFolder = path_GLOBAL_Folder + "/" + path_Kit_Folder + "/";
		strFile = groupName + path_Prefix + ofToString(i) + ".xml";
		strPath = strFolder + strFile;

		if (i < presetsXmlArray.size()) {
			presetsXmlArray[i].save(strPath);
		}
		else {
			ofLogError(__FUNCTION__) << "presetsXmlArray OUT OF RANGE";
		}

	}

	//debug params
	bool bDEBUG = false;
	if (bDEBUG)
	{
		for (int i = 0; i < presetsXmlArray.size(); i++)
		{
			ofLogNotice(__FUNCTION__) << "presetsXmlArray[" << i << "] " << ofToString(presetsXmlArray[i].toString());
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load_AllKit_ToMemory()
{
	ofLogNotice(__FUNCTION__) << "load_AllKit_ToMemory()";

	////TODO:
	//groupName = groups[0].getName();
	//ofLogNotice(__FUNCTION__) << "groups[0].getName(): " << groupName;
	//groupsMem.clear();
	//groupsMem.reserve(NUM_OF_PRESETS);
	//groupsMem.resize(NUM_OF_PRESETS);

	for (int i = 0; i < presetsXmlArray.size(); i++)
	{
		//TODO:
		//PROBLEM:
		//this is not working because all the groups of the vector are shallow copies,
		//so all are changing together
		//pre-create params inside the group not avoid emptyness
		//ofParameterGroup g = groups[0];
		//groupsMem[i] = g;

		//-

		string pathFolder;
		string pathFilename;
		string pathComplete;
		pathFolder = path_GLOBAL_Folder + "/" + path_Kit_Folder + "/";
		pathFilename = groupName + path_Prefix + ofToString(i) + ".xml";
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

		////TODO:
		//ofDeserialize(settings, groupsMem[i]);
		//ofLogNotice(__FUNCTION__) << "ofParameterGroup: " << i << "\n" << ofToString(groupsMem[i]);

		if (bLoaded)
		{
			if (i < presetsXmlArray.size()) {
				presetsXmlArray[i] = settings;
			}
			else {
				ofLogError(__FUNCTION__) << "presetsXmlArray OUT OF RANGE";
			}
		}
#ifdef INCLUDE_DEBUG_ERRORS
		else if (!bLoaded)
		{
			errorsDEBUG.addError(__FUNCTION__, gui_LabelName + ". NOT FOUND OR EMPTY XML FILES:", pathComplete);
			//TODO:
			//maybe we should create not found or empty xml presets with an empty preset..
		}
#endif
	}

	ofLogNotice(__FUNCTION__) << "-------------------------------------------------------------------------------------------------------";

	//debug params
	bool bDEBUG = false;
	if (bDEBUG)
	{
		for (int i = 0; i < presetsXmlArray.size(); i++)
		{
			ofLogNotice(__FUNCTION__) << "presetsXmlArray[" << i << "] " << ofToString(presetsXmlArray[i].toString());
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

	DISABLE_CALLBACKS = true;

	//destroy callbacks
	removeKeysListeners();
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);
	ofRemoveListener(params_Randomizer.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);
	ofRemoveListener(params_Editor.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Editor);

	//TODO: required?
	ofRemoveListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofRemoveListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	//autosave preset
	if (autoSave)
	{
		doSave(PRESET_selected);
	}

	//-

	//TODO: move above
	//MODE B: direct from memory
	if (MODE_MemoryLive && autoSave)
	{
		save_AllKit_FromMemory();
	}

	//--

	//app settings
	save_ControlSettings();//crashes?

	//--

	//TODO: debug crash
	//gui_ImGui.exit();
}

//--

#pragma mark - IM_GUI

#ifdef INCLUDE_GUI_IM_GUI

#ifndef USE_ofxImGuiSimple
//--------------------------------------------------------------
void ofxPresetsManager::ImGui_FontCustom() {
	ofLogNotice(__FUNCTION__);

	ImGuiIO& io = ImGui::GetIO();
	float _size = 14.f;
	string _name = "FiraCodeRegular.ttf";
	//float _size = 13.f;
	//string _name = "overpass-mono-bold.otf";
	string _path = "assets/fonts/" + _name;//assets folder
	io.Fonts->AddFontFromFileTTF(&ofToDataPath(_path)[0], _size);
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_ThemeMoebiusSurfing()
{
	ofLogNotice(__FUNCTION__);


	//must be done after setup the gui

	ImGuiStyle *style = &ImGui::GetStyle();

	style->FramePadding = ImVec2(4, 2);
	//style->WindowMinSize = ImVec2(160, 65);
	//style->ItemSpacing = ImVec2(6, 2);
	style->ItemSpacing = ImVec2(6, 4);
	style->ItemInnerSpacing = ImVec2(6, 4);
	style->Alpha = 1.0f;
	style->WindowRounding = 0.0f;
	style->FrameRounding = 0.0f;
	style->IndentSpacing = 6.0f;
	style->ItemInnerSpacing = ImVec2(2, 4);
	style->ColumnsMinSpacing = 50.0f;
	style->GrabMinSize = 14.0f;
	style->GrabRounding = 0.0f;
	style->ScrollbarSize = 12.0f;
	style->ScrollbarRounding = 0.0f;

	//my dark theme

	//ImVec4* colors = ImGui::GetStyle().Colors;
	style->Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	style->Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.54f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.26f, 0.68f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.67f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.12f, 0.11f, 0.11f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.75f, 0.77f, 0.79f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.79f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.31f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style->Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.29f, 0.29f, 0.29f, 0.78f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.44f, 0.44f, 0.44f, 0.25f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.68f, 0.68f, 0.68f, 0.67f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.68f, 0.68f, 0.68f, 0.95f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.81f, 0.79f, 0.79f, 1.00f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.50f, 0.50f, 0.50f, 0.90f);
	style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
	style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	//   //2. defaut dark theme exported
	//   style->WindowMinSize = ImVec2(160, 65);
	//   style->FramePadding = ImVec2(4, 2);
	//   style->ItemSpacing = ImVec2(6, 2);
	//   style->ItemInnerSpacing = ImVec2(6, 4);
	//   style->Alpha = 1.0f;
	//   style->WindowRounding = 0.0f;
	//   style->FrameRounding = 0.0f;
	//   style->IndentSpacing = 6.0f;
	//   style->ItemInnerSpacing = ImVec2(2, 4);
	//   style->ColumnsMinSpacing = 50.0f;
	//   style->GrabMinSize = 14.0f;
	//   style->GrabRounding = 0.0f;
	//   style->ScrollbarSize = 12.0f;
	//   style->ScrollbarRounding = 0.0f;

}
#endif

#ifndef MODE_ImGui_EXTERNAL
//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Setup()
{
	ofLogNotice(__FUNCTION__);

	//--

	//font customize
#ifndef MODE_ImGui_EXTERNAL
#ifdef INCLUDE_IMGUI_CUSTOM_FONT
	ImGui_FontCustom();
#endif
#endif

	//--

	gui_ImGui.setup();

	//--

	//theme
#ifndef MODE_ImGui_EXTERNAL
	//ImGui_ThemeMoebiusSurfing();
	ImGui_ThemeModernDark();

	//--

	//mouse over
	ImGui::GetIO().MouseDrawCursor = false;
	bImGui_mouseOver.set("mouseOverGui", false);

	//ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
	//ImGui::GetIO().= true;
	//ImGui::GetIO().

#endif

	//--
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_WindowBegin()
{
	//mouse over gui system
	bMouseOver_Changed = false;
	bImGui_mouseOver = false;

	//-

#ifndef USE_ofxImGuiSimple
	gui_ImGui.begin();
#else
	gui_ImGui.begin();
	ImGui::ShowDemoWindow();
	//TEST: params helpers..
	ofxImGui::AddParameter(PRESET_selected);
#endif

	//-
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_WindowEnd()
{
	if (bImGui_mouseOver != bImGui_mouseOver_PRE)
	{
		bImGui_mouseOver_PRE = bImGui_mouseOver;
		bMouseOver_Changed = true;
	}

	if (bMouseOver_Changed)
	{
		ofLogVerbose(__FUNCTION__) << "bImGui_mouseOver: " << (bImGui_mouseOver ? "INSIDE" : "OUTSIDE");
}

	//--

#ifndef USE_ofxImGuiSimple
	gui_ImGui.end();
#else
	gui_ImGui.end();
#endif
}

//--------------------------------------------------------------
bool ofxPresetsManager::ImGui_Draw_Window()
{
	auto mainSettings = ofxImGui::Settings();

	ofVec2f pos(ImGui_Position.get().x, ImGui_Position.get().y);
	ofVec2f size(ImGui_Size.get().x, ImGui_Size.get().y);
	//ofLogNotice(__FUNCTION__) << "ImGui position: " << ofToString(pos);
	//ofLogNotice(__FUNCTION__) << "ImGui size: " << ofToString(size);
	mainSettings.windowPos = pos;//required
	mainSettings.windowSize = size;

	auto _mode = ImGuiCond_FirstUseEver;
	//auto _mode = ImGuiCond_Always;
	ImGui::SetNextWindowPos(ofVec2f(pos.x, pos.y), _mode);
	ImGui::SetNextWindowSize(ofVec2f(size.x, size.y), _mode);

	//--

	string _name = groups[0].getName();
	bool _collapse = true;

	//--

	//set window properties
	//bool show = true;
	static bool no_titlebar = false;
	static bool no_border = true;
	static bool no_resize = true;
	static bool no_move = true;
	static bool no_scrollbar = false;
	static bool no_collapse = true;
	static bool no_close = true;
	static bool no_menu = true;
	static bool no_settings = true;
	static float bg_alpha = -0.01f; // <0: default

	// Demonstrate the various window flags. 
	// Typically you would just use the default.
	ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoBackground;//make background transparent
	//if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
	//if (!no_border)   window_flags |= ImGuiWindowFlags_NoDecoration;
	//if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
	//if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
	//if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
	//if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
	//if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
	//if (no_settings)	window_flags |= ImGuiWindowFlags_NoSavedSettings;

	//---

	//A

	if (ofxImGui::BeginWindow(_name, mainSettings, window_flags, &_collapse))
	{
		ImGui_Draw_Content(mainSettings);

		//-

		//get window position/size: must be inside begin/end
		pos = ImGui::GetWindowPos();
		size = ImGui::GetWindowSize();
		ImGui_Position = glm::vec2(pos.x, pos.y);
		ImGui_Size = glm::vec2(size.x, size.y);
		//cout << "pos:" << ofToString(pos) << endl;
		//cout << "size:" << ofToString(size) << endl;
	}
	ofxImGui::EndWindow(mainSettings);

	return mainSettings.mouseOverGui;
}

#endif
#endif

#ifdef INCLUDE_GUI_IM_GUI//ImGui pure content
//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_PresetPreview(ofxImGui::Settings &settings)
{
	if (ofxImGui::BeginTree("PRESET PARAMETERS", settings))
	{
		ofxImGui::AddGroup(groups[0], settings);
		ofxImGui::EndTree(settings);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Basic(ofxImGui::Settings &settings)
{
	{
		//---

		////0.label
		//ImGui::Text("PRESET");

		//---

		////1. preset selector slider

		//auto parameter = PRESET_selected;
		//auto tmpRef = PRESET_selected.get();
		//auto name = ofxImGui::GetUniqueName(PRESET_selected);
		//if (ImGui::SliderInt(ofxImGui::GetUniqueName(parameter), (int *)&tmpRef, parameter.getMin(), parameter.getMax()))
		//{
		//	parameter.set(tmpRef);
		//}

		//---

		//2. prest clicker matrix buttons

		//if (ImGui::TreeNode("Test"))
		{
			//preset selector
			//toggle button matrix
			ImVec2 button_sz(40, 40);
			// Manually wrapping
			// (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
			//ImGui::Text("PRESET SELECTOR:");
			ImGuiStyle& style = ImGui::GetStyle();
			int buttons_count = numPresetsFavorites;
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

			for (int n = 0; n < buttons_count; n++)
			{
				ImGui::PushID(n);
				string name = ofToString((char)(keys[0][n]));

				//customize colors
				{
					if (PRESET_selected.get() == n)//when selected
					{
						const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonHovered];//changes the color
						ImGui::PushStyleColor(ImGuiCol_Button, colorActive);
					}
					else {
						const ImVec4 colorButton = style.Colors[ImGuiCol_Button];//do not changes the color
						ImGui::PushStyleColor(ImGuiCol_Button, colorButton);
					}

					//draw button
					if (ImGui::Button(name.c_str(), button_sz))
					{
						loadPreset(n);//trig load preset
					}

					//customize colors
					ImGui::PopStyleColor();
				}

				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
				if (n + 1 < buttons_count && next_button_x2 < window_visible_x2) ImGui::SameLine();
				ImGui::PopID();
			}
		}
		//ImGui::TreePop();
	}

	//ofxImGui::AddParameter(PRESET_selected);//main preset selector

	//--

	//big toggle button
	AddBigToggle(PLAY_RandomizeTimer, 30);

	//----

	//if (ImGui::TreeNode("BASIC"))
	if (ofxImGui::BeginTree("BASIC", settings))
	{


		//--

		//main helpers
		if (ImGui::Button("CLONE ALL"))
		{
			bCloneAll = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("CLONE >"))
		{
			bCloneRight = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("POPULATE!"))
		{
			//popupale all favs
			doPopulateFavs();
			//create browser files too
			doGetFavsToFilesBrowser();
		}

		//-

		//ImGui::SetNextItemWidth(_w);
		ofxImGui::AddParameter(autoSave);
		ImGui::SameLine();
		ofxImGui::AddParameter(autoLoad);
		ofxImGui::AddParameter(MODE_MemoryLive);

		//--

		//2. panels toggles

		if (ImGui::TreeNode("EXTRA")) {
			ofxImGui::AddParameter(SHOW_ImGui);
			ofxImGui::AddParameter(SHOW_ClickPanel);
			ofxImGui::AddParameter(SHOW_Gui_AdvancedControl);

			//-

			//2. advanced

			if (SHOW_Gui_AdvancedControl)
			{
				//show ALL the addon params! mainly to debug..
				ofxImGui::AddGroup(params_Control, settings);

				////TODO:
				////to customize presets folder outside /data of our app...
				////ie: this will allow to use any folder of our computer, and share the presets between apps...
				//if (ImGui::Button("Set custom folder..."))
				//{
				//	auto dialogResult = ofSystemLoadDialog("Set presets folder", true, ofToDataPath(""));
				//	if (dialogResult.bSuccess)
				//	{
				//		path_PresetsFolder_Custom = dialogResult.filePath;
				//		bUseCustomPath = true;
				//		ofLogNotice(__FUNCTION__) << "path custom: " << path_PresetsFolder_Custom;
				//	}
				//}
			}

			ImGui::TreePop();
		}

		//-

		//ImGui::TreePop();
		ofxImGui::EndTree(settings);
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doFileDialogProcessSelection(ofFileDialogResult openFileResult) {

	ofLogNotice(__FUNCTION__) << ("getName(): " + openFileResult.getName());
	ofLogNotice(__FUNCTION__) << ("getPath(): " + openFileResult.getPath());

	bPathDirCustom = true;
	pathDirCustom = openFileResult.getPath();

	//-

	//save randomizers settings
	string path2;
	bool b2;
	path2 = path_GLOBAL_Folder + "/" + path_Control + "/" + filename_Randomizers;
	b2 = ofxSurfingHelpers::saveGroup(params_RandomizerSettings, path2);
	ofLogNotice(__FUNCTION__) << "Saved " << path2 << " " << (b2 ? "DONE" : "FAILED");

	//-

	setPath_GlobalFolder(pathDirCustom);

	nameUserKit = path_GLOBAL_Folder;

	//browser
	browser_Setup();
	//setupRandomizer(); 
	//setupEditor();
	//startup();

	//-

	//load randomizers settings
	path2 = path_GLOBAL_Folder + "/" + path_Control + "/" + filename_Randomizers;
	b2 = ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path2);
	ofLogNotice(__FUNCTION__) << "Loaded " << path2 << " " << (b2 ? "DONE" : "FAILED");
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Browser(ofxImGui::Settings &settings)
{
	if (ofxImGui::BeginTree("FILES BROWSER", settings))
	{
		int _numfiles = fileNames.size();

		//--

		if (true) {
			if (ImGui::TreeNode("ABSOLUTE PATH ")) {
				ImGui::Text("Customize path");

				//customize folder

				//button to Open File Dialog as folder
				if (ImGui::Button("Select Folder")) {

					ofFileDialogResult openFileResult = ofSystemLoadDialog("Select folder", true, ofToDataPath(path_GLOBAL_Folder, true));

					//Check if the user opened a file
					if (openFileResult.bSuccess) {

						ofLogNotice(__FUNCTION__) << ("User selected a folder");

						//We have a file, check it and process it
						doFileDialogProcessSelection(openFileResult);
					}
					else {
						ofLogNotice(__FUNCTION__) << ("User hit cancel");
					}
				}
				
				//ofxImGui::AddParameter(bPathDirCustom);
				if (bPathDirCustom) ofxImGui::AddParameter(pathDirCustom);
				
				ImGui::TreePop();
			}
		}

		//--

		if (ImGui::TreeNode("STANDALONE PRESET")) {
			//new button
			ofxImGui::AddParameter(MODE_Browser_NewPreset);

			//-

			//0. send/save current browsed (from "/archive/") preset to current presets on favorites

			//get/copy all favs presets from favs and send/save to browser folder ("archive")

			if (ImGui::Button("FROM FAVS"))
			{
				ofLogNotice(__FUNCTION__) << "FROM FAVS";
				doGetFavsToFilesBrowser();
			}

			//-

			ImGui::SameLine();
			if (ImGui::Button("TO FAVS"))
			{
				ofLogNotice(__FUNCTION__) << "TO FAVS: SAVE BROWSED PRESET: " << browser_PresetName;
				ofLogNotice(__FUNCTION__) << "TO FAVORITES PRESET: [" << PRESET_selected << "]";

				if (MODE_Browser_NewPreset)
				{
					save(PRESET_selected, 0);
				}
			}

			//---

			//label folder
			string str;
			if (!bPathDirCustom) str += "bin/data/";
			str += path_GLOBAL_Folder + "/" + path_PresetsFolder;
			ImGui::Text(str.c_str());

			//-

			//blink error when no files detected on folder

			if (_numfiles == 0)
			{
				int n = 30;
				float a = ofMap(ofGetFrameNum() % n, 0, n, 0.f, 1.f);
				ImGui::PushID(1);
				ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.5, 0.0f, 0.5f, a));
				ImGui::Text("DIR OR FILES NOT FOUND!");
				string browser_path = path_GLOBAL_Folder + "/" + path_PresetsFolder;
				const char *array = browser_path.c_str();
				ImGui::Text(array);
				ImGui::PopStyleColor(1);
				ImGui::PopID();
			}

			else if (_numfiles > 0)
			{
				//1. arrow buttons

				static int counter = currentFile;
				float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
				ImGui::PushButtonRepeat(true);

				//--

				//1.1 prev file

				if (ImGui::ArrowButton("##left", ImGuiDir_Left))
				{
					if (counter > 0)
					{
						counter--;
						currentFile = counter;
						if (currentFile < files.size())
						{
							browser_PresetName = fileNames[currentFile];
							ofLogNotice(__FUNCTION__) << "ARROW: browser_PresetName: [" + ofToString(currentFile) + "] " << browser_PresetName;
							//if (MODE_Browser_NewPreset)
							ofLogNotice(__FUNCTION__) << "LOAD" << endl;
							ofLogNotice(__FUNCTION__) << "PRESET NAME: " << browser_PresetName;
							browser_PresetLoad(browser_PresetName);
						}
					}
				}

				//--

				//1.2 next file

				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##right", ImGuiDir_Right))
				{
					if (counter < files.size() - 1)
					{
						counter++;
						currentFile = counter;
						if (currentFile < files.size())
						{
							browser_PresetName = fileNames[currentFile];
							ofLogNotice(__FUNCTION__) << "ARROW: browser_PresetName: [" + ofToString(currentFile) + "] " << browser_PresetName;

							//if (MODE_Browser_NewPreset)
							ofLogNotice(__FUNCTION__) << "LOAD PRESET NAME: " << browser_PresetName;
							browser_PresetLoad(browser_PresetName);
						}
					}
				}
				ImGui::PopButtonRepeat();

				//--

				//1.3 text preview current preset index to total amount.

				ImGui::SameLine();
				ImGui::Text("%d/%d", currentFile, _numfiles - 1);
			}

			//--

			//3. scrollable filenames list

			if (!fileNames.empty())
			{
				ImGui::SetNextItemWidth(140);

				int currentFileIndex = currentFile;
				if (ofxImGui::VectorCombo(" ", &currentFileIndex, fileNames))
				{
					ofLogNotice(__FUNCTION__) << "Preset Index: " << ofToString(currentFileIndex);
					if (currentFileIndex < fileNames.size())
					{
						currentFile = currentFileIndex;
						browser_PresetName = fileNames[currentFile];

						ofLogNotice(__FUNCTION__) << "LOAD Preset Name: " << browser_PresetName;
						browser_PresetLoad(browser_PresetName);
					}
				}
			}

			//--

			//4.2 update

			if (ImGui::Button("UPDATE"))
			{
				//browser_PresetName = textInput_temp;
				//ofLogNotice(__FUNCTION__) << "UPDATE PRESET NAME: " << browser_PresetName << endl;

				//0. get filename of selected
				string _currName = files[currentFile].getBaseName();
				ofLogNotice(__FUNCTION__) << "UPDATE Preset Name: " << _currName;

				//1. delete old file
				files[currentFile].remove();

				//2. save "ovewrite"
				browser_PresetSave(_currName);

				//workflow
				//3. refresh files
				browser_FilesRefresh();

				//4. reselect last save preset (bc directory sorting changes)
				ofLogNotice(__FUNCTION__) << "Reload last updated preset:";
				int iNew = -1;//search index for filename
				for (size_t i = 0; i < files.size(); i++)
				{
					string n = files[i].getBaseName();
					if (n == _currName)
					{
						iNew = i;
					}
				}
				if (iNew != -1)
				{
					ofLogNotice(__FUNCTION__) << "Index [" << iNew << "] " << files[iNew].getBaseName();
					currentFile = iNew;
					browser_PresetName = fileNames[currentFile];
					browser_PresetLoad(browser_PresetName);
				}
				else
				{
					ofLogError(__FUNCTION__) << "Not found! Bad Index [" << iNew << "]";
				}
			}

			//-

			//4.3 reload

			ImGui::SameLine();
			if (ImGui::Button("RELOAD"))
			{
				ofLogNotice(__FUNCTION__) << "RELOAD Preset Name: " << browser_PresetName;
				browser_PresetLoad(browser_PresetName);
			}

			//-

			//4.4 delete

			ImGui::SameLine();
			if (ImGui::Button("DELETE"))//current preset
			{
				ofLogNotice(__FUNCTION__) << "DELETE Preset Name: " << browser_PresetName;
				ofLogNotice(__FUNCTION__) << "file: " << files[currentFile].getAbsolutePath();

				//1. delete file
				files[currentFile].remove();

				//workflow
				//2. refresh files
				bool b = browser_FilesRefresh();
				if (b)
				{
					currentFile = 0;
					browser_PresetName = fileNames[currentFile];
					browser_PresetLoad(browser_PresetName);
				}
				else
				{
					ofLogError(__FUNCTION__) << "Error listing directory!";
				}
			}

			//-

			//4.5 clear. delete all!

			ImGui::SameLine();
			if (ImGui::Button("CLEAR"))//delete all files
			{
				ofLogNotice(__FUNCTION__) << "CLEAR Presets folder: " << path_GLOBAL_Folder + "/" + path_PresetsFolder;

				for (int i = 0; i < files.size(); i++) {
					ofLogWarning(__FUNCTION__) << "DELETE file: " << files[i].getAbsolutePath();

					//1. delete file
					bool b = files[i].remove();
					if (!b) ofLogError(__FUNCTION__) << "Can not DELETE file: " << files[i].getAbsolutePath();
				}

				//workflow
				//2. refresh files
				bool b = browser_FilesRefresh();
				if (b)
				{
					currentFile = 0;
					browser_PresetName = fileNames[currentFile];
					browser_PresetLoad(browser_PresetName);
				}
				else
				{
					ofLogError(__FUNCTION__) << "Error listing directory!";
				}
			}

			//----

			//mode new preset enabled

			//5. second panel

			if (MODE_Browser_NewPreset)
			{
				//5.1 new preset name

				//ImGui::Text("NEW PRESET:");

				//-

				//5.2 user input text

				//loaded string into char array
				char tab[32];
				strncpy(tab, textInput_New.c_str(), sizeof(tab));
				tab[sizeof(tab) - 1] = 0;

				if (ImGui::InputText("", tab, IM_ARRAYSIZE(tab)))
				{
					ofLogNotice(__FUNCTION__) << "InputText [tab]:" << ofToString(tab) << endl;
					textInput_New = ofToString(tab);
					ofLogNotice(__FUNCTION__) << "textInput_New:" << textInput_New << endl;

					//bBlink = true;//not workind. we like to blink when mouse_on_text_input
				}

				//--

				//5.3 save new

				//workflow
				//TODO:
				//blink when it's editing a new preset..
				bool bBlink;
				bBlink = true;
				if (bBlink)
				{
					//ImGui::PushID(1);
					int n = 30;
					float a = ofMap(ofGetFrameNum() % n, 0, n, 0.2f, 0.5f);
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5, 0.0f, 0.5f, a));
				}

				if (ImGui::Button("SAVE"))
				{
					ofLogNotice(__FUNCTION__) << "textInput_New: " << textInput_New << endl;

					//1. save
					browser_PresetSave(textInput_New);

					//workflow
					//2. disable new preset mode
					MODE_Browser_NewPreset = false;

					//3. refresh files
					browser_FilesRefresh();

					//4. reselect last save preset (bc directory sorting changes)
					ofLogNotice(__FUNCTION__) << "Reload last saved preset:";
					int iNew = -1;
					for (size_t i = 0; i < files.size(); i++)
					{
						string n = files[i].getBaseName();
						if (n == textInput_New)
						{
							iNew = i;
						}
					}
					ofLogNotice(__FUNCTION__) << "Index [" << iNew << "] " << files[iNew].getBaseName();
					currentFile = iNew;
					browser_PresetLoad(textInput_New);
				}

				if (bBlink)
				{
					ImGui::PopStyleColor(1);
				}
			}
			ofxImGui::EndTree(settings);
		}

		ImGui::TreePop();
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Content(ofxImGui::Settings &settings)
{
	//0. tittle
	ImGui::Text("PRESETS MANAGER");
	//ImGui::NewLine();

	//1. basic controls
	ImGui_Draw_Basic(settings);

	//4. preset params preview
	ImGui_Draw_PresetPreview(settings);

	//3. advanced params
	ImGui_Draw_Randomizers(settings);

	//2. browser params
	ImGui_Draw_Browser(settings);
}

//--------------------------------------------------------------
void ofxPresetsManager::ImGui_Draw_Randomizers(ofxImGui::Settings &settings)
{
	//1. randomizers

	//bool b = false;
	//settings.windowBlock = true;

	if (ofxImGui::BeginTree("RANDOMIZERS", settings))
		//if (ImGui::TreeNode("RANDOMIZERS"))
	{
		//1. presets randomizers

		ImGuiStyle *style = &ImGui::GetStyle();

		//--

		//1.0.1 play randomizer index
		AddBigToggle(PLAY_RandomizeTimer, 30);

		//-

		//1.0.1B bpm slider
		auto parameter = randomizeDurationBpm;
		auto tmpRef = randomizeDurationBpm.get();
		auto name = ofxImGui::GetUniqueName(randomizeDurationBpm);
		if (ImGui::SliderFloat(ofxImGui::GetUniqueName(parameter), (float *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
		}

		//--

		//1.0.2 draw progress bar for the randomizer timer
		float _prog;
		////long mode
		//if (presetsRandomModeShort[PRESET_selected - 1] == false) _prog = timerRandomizer / (float)randomizeDuration;
		////short mode
		//else _prog = timerRandomizer / (float)randomizeDurationShort;
		//bar relative only to long
		if (PLAY_RandomizeTimer) {
			_prog = timerRandomizer / (float)randomizeDuration;
		}
		else if (MODE_LatchTrig) {
			if (bLatchRun) {
				_prog = timerRandomizer / (float)randomizeDuration;
			}
			else {
				_prog = 0;
			}
		}
		else {
			_prog = 0;
		}

		ImGui::PushID("prog");
		const ImVec4 color = style->Colors[ImGuiCol_ButtonHovered];//we can force change this color on theme... only used here
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
		ImGui::ProgressBar(_prog);
		ImGui::PopStyleColor();
		ImGui::PopID();

		//--

		//1.0.3 bang randomize
		//AddBigButton(bRandomizeIndex, 25);//index
		AddBigButton(bRandomizeEditor, 25);//preset

		//float w = ImGui::GetWindowWidth();
		//style->FramePadding = ImVec2(4, 2);
		//style->ItemSpacing = ImVec2(6, 4);
		//style->ItemInnerSpacing = ImVec2(6, 4);
		//style->IndentSpacing = 6.0f;
		//style->ItemInnerSpacing = ImVec2(2, 4);
		//float wHalf = (ImGui::GetWindowWidth() * 0.5f) - ( 4 + 6 + 6 +4);
		float wHalf = 100;

		if (ImGui::Button("UNDO", ImVec2(wHalf, 20)))
		{
			ofLogNotice(__FUNCTION__) << "UNDO <-";
			undoStringParams.undo();
			undoRefreshParams();
		}
		ImGui::SameLine();
		if (ImGui::Button("REDO", ImVec2(wHalf, 20)))
		{
			ofLogNotice(__FUNCTION__) << "REDO ->";
			undoStringParams.redo();
			undoRefreshParams();
		}
		//string str = "";
		//str += ofToString(undoStringParams.getUndoLength()) + "/";
		//str += ofToString(undoStringParams.getRedoLength());
		//ImGui::Text(str.c_str());

		//--

		//1.1 randomizers presets
		//const bool b = true;
		//settings.windowBlock = false;//-> this hides next group
		//ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing);//not working..
		//ImGui::SetNextWindowCollapsed(true, ImGuiCond_Always);
		ofxImGui::AddGroup(params_Randomizer, settings);

#ifdef DEBUG_randomTest
		ImGui::Text("%d/%d", randomizedDice.get(), randomizedDice.getMax());
#endif
		//--

		//1.2 randomizers editor
		ofxImGui::AddGroup(params_Editor, settings);

		//-

		ofxImGui::EndTree(settings);
		//ImGui::TreePop();
	}

	////---

	////2. advanced

	//if (SHOW_Gui_AdvancedControl)
	//{
	//	//show ALL the addon params! mainly to debug..
	//	ofxImGui::AddGroup(params_Control, settings);
	//
	//	//TODO:
	//	//to customize presets folder outside /data of our app...
	//	//ie: this will allow to use any folder of our computer, and share the presets between apps...
	//	if (ImGui::Button("Set custom folder..."))
	//	{
	//		auto dialogResult = ofSystemLoadDialog("Set presets folder", true, ofToDataPath(""));
	//		if (dialogResult.bSuccess)
	//		{
	//			path_PresetsFolder_Custom = dialogResult.filePath;
	//			bUseCustomPath = true;
	//			ofLogNotice(__FUNCTION__) << "path custom: " << path_PresetsFolder_Custom;
	//		}
	//	}
	//}
}
#endif

#ifdef INCLUDE_GUI_IM_GUI//browser
//--------------------------------------------------------------
void ofxPresetsManager::browser_PresetSave(string name)//without xml extension nor path
{
	ofLogNotice(__FUNCTION__) << name << ".xml";

	string _path;
	_path = path_GLOBAL_Folder + "/" + path_PresetsFolder + "/" + name + ".xml";
	bool b = ofxSurfingHelpers::saveGroup(groups[0], _path);
	if (!b)
	{
#ifdef INCLUDE_DEBUG_ERRORS
		errorsDEBUG.addError(ofToString(__FUNCTION__), gui_LabelName, _path);
#endif
		ofLogError(__FUNCTION__) << "CAN'T SAVE FILE " << _path << "!";
	}
	else {
		ofLogNotice(__FUNCTION__) << "DONE SAVE " << _path;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::browser_PresetLoad(string name)//without xml extension nor path
{
	ofLogNotice(__FUNCTION__) << name << ".xml";

	string _path;
	_path = path_GLOBAL_Folder + "/" + path_PresetsFolder + "/" + name + ".xml";
	bool b = ofxSurfingHelpers::loadGroup(groups[0], _path);
	if (!b)
	{
#ifdef INCLUDE_DEBUG_ERRORS
		errorsDEBUG.addError(ofToString(__FUNCTION__), gui_LabelName, _path);
#endif
		ofLogError(__FUNCTION__) << "CAN'T LOAD FILE " << _path << "!";
	}
	else {
		ofLogNotice(__FUNCTION__) << "DONE LOAD " << _path;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::browser_Setup()
{	//load files structure directory
	bool bLoaded = browser_FilesRefresh();

	//workflow
	if (bLoaded)
	{
		//load first preset
		if (fileNames.size() > 0)
		{
			currentFile = 0;
			browser_PresetName = fileNames[currentFile];
			browser_PresetLoad(browser_PresetName);
		}
	}
}

//--------------------------------------------------------------
bool ofxPresetsManager::browser_FilesRefresh()
{
	//ofLogNotice(__FUNCTION__);

	string _path;
	_path = path_GLOBAL_Folder + "/" + path_PresetsFolder;
	CheckFolder(_path);

	ofLogNotice(__FUNCTION__) << "Path: " << _path;
	ofDirectory dataDirectory(ofToDataPath(_path, true));

	//-

	//TODO:
	//make above code to function
	//create folder if do not exist!
	if (!dataDirectory.isDirectory())
	{
		ofLogError(__FUNCTION__) << "FOLDER DOES NOT EXIST!";
		bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));
		if (b) ofLogNotice(__FUNCTION__) << "CREATED FOLDER: " << _path;
		else ofLogError(__FUNCTION__) << "UNABLE TO CREATE FOLDER: " << _path;
	}

	//-

	//clear files and filenames vectors
	files.clear();
	fileNames.clear();

	//load all folder files in one call
	files = dataDirectory.getFiles();

	ofLogNotice(__FUNCTION__) << "Preset files:";
	for (size_t i = 0; i < files.size(); i++)
	{
		fileNames.push_back(files[i].getBaseName());
		ofLogNotice(__FUNCTION__) << "[" << i << "] " << files[i].getBaseName();
	}

	//-

	//TODO
	//void to go to 1st...

	//1. load same position preset
	//if preset is deleted goes to nextone..
	//should check names because sorting changes..
	if (fileNames.size() > 0)
	{
		bFilesError = false;
	}
	else
	{
		ofLogError(__FUNCTION__) << "BROWSER: FILES NOT FOUND ON FOLDER!";
		bFilesError = true;

		//TODO:
		//disable custom path bc error
		ofLogError(__FUNCTION__) << "Disable custom path: " << path_BrowserPathFree;
		bCustomBrowserPath = false;

		ofLogError(__FUNCTION__) << "------------------------";
	}

	//workflow
	////2. always goes to 1st preset 0
	////that's because saving re sort the files
	////and we don't know the position of last saved preset..
	//if (fileNames.size() > 0)
	//{
	//   currentFile = 0;
	//   browser_PresetName = fileNames[currentFile];
	//   browser_PresetLoad(browser_PresetName);
	//}
	//else
	//{
	//   ofLogError("ofxColorManager") << "NOT FOUND ANY FILE PRESET!";
	//}

	return !bFilesError;//true if ok
}

#ifdef INCLUDE_ofxUndoSimple
//--------------------------------------------------------------
void ofxPresetsManager::undoStoreParams() {
	//ofLogNotice(__FUNCTION__);
	xmlParams.clear();
	ofParameterGroup _group = groups[0];
	ofSerialize(xmlParams, _group);// fill the xml with the ofParameterGroup
	undoStringParams = (xmlParams.toString());// fill the ofxUndoSimple with the xml as string
	undoStringParams.store();

	string str = "";
	str += "UNDO HISTORY: " + ofToString(undoStringParams.getUndoLength()) + "/";
	str += ofToString(undoStringParams.getRedoLength());
	//str += "\n";
	//str += "DESCRIPTOR\n";
	//str += undoStringParams.getUndoStateDescriptor() + "\n";
	ofLogNotice(__FUNCTION__) << str;
}

//--------------------------------------------------------------
void ofxPresetsManager::undoRefreshParams() {
	//ofLogNotice(__FUNCTION__);
	xmlParams.clear();
	xmlParams.parse(undoStringParams);// fill the xml with the string 
	ofParameterGroup _group = groups[0];
	ofDeserialize(xmlParams, _group);// load the ofParameterGroup

	string str = "";
	str += "UNDO HISTORY: " + ofToString(undoStringParams.getUndoLength()) + "/";
	str += ofToString(undoStringParams.getRedoLength());
	//str += "\n";
	//str += "DESCRIPTOR\n";
	//str += undoStringParams.getUndoStateDescriptor() + "\n";
	ofLogNotice(__FUNCTION__) << str;
}
#endif
#endif