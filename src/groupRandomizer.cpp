#include "groupRandomizer.h"

//--------------------------------------------------------------
groupRandomizer::groupRandomizer() {}

//--------------------------------------------------------------
groupRandomizer::~groupRandomizer()
{
	exit();
}

//--------------------------------------------------------------
void groupRandomizer::setup(ofParameterGroup &g, vector<int> _keysList)
{
	keys.reserve(_keysList.size());
	keys.resize(_keysList.size());
	keys = _keysList;
	setup(g, keys.size());
}

//--------------------------------------------------------------
void groupRandomizer::setup(ofParameterGroup &_group, int _numPresets) {
	ofLogNotice(__FUNCTION__) << "add randomizer: " << _group.getName() << " amount presets: " << _numPresets;
	group = _group;

	// update control gui panel params
	amountPresets = _numPresets;

	PRESET_Selected_IndexMain.set(_group.getName(), 0, 0, amountPresets - 1);

	// list keys
	for (int i = 0; i < _numPresets; i++) {
		ofLogNotice(__FUNCTION__) << "added key: " << keys[i];
	}

	//--

	// randomizers
	buildRandomizers();

	// to store settings
	params_RandomizerSettings.add(params_Randomizer);
	params_RandomizerSettings.add(params_Editor);

	//-

	bCloneRight.set("CLONE >", false);
	bCloneAll.set("CLONE ALL", false);
	bPopulateAll.set("POPULATE ALL", false);

	params_HelperTools.setName("HELPER TOOLS");
	params_HelperTools.add(bCloneRight);
	params_HelperTools.add(bCloneAll);
	params_HelperTools.add(bPopulateAll);

	params_Control.setName("groupRandomizer");
	params_Control.add(params_HelperTools);
	params_Control.add(PRESET_Selected_IndexMain);

	ofAddListener(params_Control.parameterChangedE(), this, &groupRandomizer::Changed_Control);
	ofAddListener(params_Randomizer.parameterChangedE(), this, &groupRandomizer::Changed_Control);

	//----

	// startup
	startup();
}

//--------------------------------------------------------------
void groupRandomizer::startup()
{
	DISABLE_CALLBACKS = false;

	// load randomizers settings
	ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path_RandomizerSettings);

	doDices();

	settings = ofxImGui::Settings();

	//--

	// workflow
	doCheckRandomReady();
}

//--------------------------------------------------------------
void groupRandomizer::doCheckRandomReady()
{
	// if there are some toggles/params enabled to allow random, we do not do nothing
	// if there's no toggle/params enabled, we enable all params toggles to the editor 
	// so random will affect to all params by default

	ofLogNotice(__FUNCTION__) << "Check if randomizer editor has some param active...";

	//// workflow
	//bool bAllFalse = false;// if is true means that all are disabled
	//for (auto &p : editorPresets)
	//{
	//	ofLogNotice(__FUNCTION__) << p.getName() << " : " << p.get();
	//	if (!p.get()) bAllFalse = true;// some one is false
	//}

	//// if all toggles are disabled after loaded settings, 
	//// then put all to true to allow populate, and random make something
	//if (!bAllFalse) {
	//	ofLogNotice(__FUNCTION__) << "Some one is enabled. Avoid force ";
	//}
	//else
	//{
	//	bRandomizeEditorAll = true;// someone is true
	//	ofLogNotice(__FUNCTION__) << "All are disabled. Force set all randomizer params enable!";
	//}


	// workflow
	bool bSomeTrue = false;
	for (auto &p : editorPresets)
	{
		ofLogNotice(__FUNCTION__) << p.getName() << " : " << p.get();
		if (p.get() == true) bSomeTrue = bSomeTrue || true;// some one is true
	}
	if (bSomeTrue) { //some is true
		ofLogNotice(__FUNCTION__) << "Some one is enabled. Avoid force enabler!";
	}
	else// none is true
	{
		bRandomizeEditorAll = true;// someone is true
		ofLogNotice(__FUNCTION__) << "All are disabled. Force set all randomizer params enable!";
	}
}

//--------------------------------------------------------------
int groupRandomizer::doRandomIndexChanged()
{
	ofLogVerbose(__FUNCTION__);

	//-

	int _r = PRESET_Selected_IndexMain;

	if (MODE_DicesProbs)
	{
		// 1. dice randomize

#ifndef DEBUG_randomTest
		{
			// get a random between all possible dices (from 0 to dicesTotalAmount) and then select the preset associated to the resulting dice.
			// each preset has many dices: more dices means more probality to be selected by the randomizer
			randomizedDice = ofRandom(0, dicesTotalAmount);

			ofLogVerbose(__FUNCTION__) << "random: " << randomizedDice.get() << "/" << dicesTotalAmount;
		}
#endif

		//-

		// 2. define limits for range dices associated to any preset

		//randomFactorsDices[0] = 0;
		for (int i = 0; i < presetsRandomFactor.size(); i++) {
			randomFactorsDices[i] = 0;

			for (int f = 0; f < i; f++) {
				randomFactorsDices[i] += presetsRandomFactor[f];
			}
		}

		//-

		// 3. check if dice is inside both ranges. to select preset (_rr) associated to dice 

		int _rr = 0;
		for (int i = 0; i <= presetsRandomFactor.size(); i++) {

			// define upper/lower limits for each dices/preset
			int start;
			int end;

			if (i == 0) start = 0;
			else start = randomFactorsDices[i - 1];

			// TODO:
			// bug on last preset..

			//if (i == presetsRandomFactor.size() - 1)
			//	end = presetsRandomFactor.size()-1;
			//else
			//	end = randomFactorsDices[i];
			//end = randomFactorsDices[i];

			if (i != presetsRandomFactor.size()) end = randomFactorsDices[i];
			else end = dicesTotalAmount;

#ifdef DEBUG_randomTest
			//ofLogNotice(__FUNCTION__) << "[" << i << "] " << start << "-" << end;
			ofLogNotice(__FUNCTION__) << (start == end ? "\t\t\t\t\t" : "") << "[" << i << "] " << start << "-" << end;// this are empty probs
#endif
			// check if dice is inside both limits
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

		// debug
		ofLogVerbose(__FUNCTION__) << "DEBUG";
		for (int i = 0; i < presetsRandomFactor.size(); i++) {
#ifdef DEBUG_randomTest
			ofLogNotice(__FUNCTION__) << "randomFactorsDices: [" << i << "] " << randomFactorsDices[i];
			// << "-" << randomFactorsDices[i + 1] << endl;
#endif
		}

		// last
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
	//	while (PRESET_Selected_IndexMain == _r)//if not changed
	//	{
	//		ofLogWarning(__FUNCTION__) << "Randomize not changed! Try #" << ofToString(++numTryes);
	//		ofLogNotice(__FUNCTION__) << "PRESET Previous was : " << ofToString(_r);
	//		ofLogNotice(__FUNCTION__) << "PRESET New Random is: " << ofToString(PRESET_Selected_IndexMain);
	//		PRESET_Selected_IndexMain = (int)ofRandom(0, mainGroupMemoryFilesPresets.size());
	//		
	//		//if (MODE_MemoryLive) _r = (int)ofRandom(0, mainGroupMemoryFilesPresets.size());
	//		//_r = (int)ofRandom(1, mainGroupMemoryFilesPresets.size() + 1);
	//	}
	//}

	return _r;
}

//--------------------------------------------------------------
void groupRandomizer::doRandomIndex()
{
#ifdef DEBUG_randomTest
	ofLogNotice(__FUNCTION__) << "---------------------------------------------------";
#endif

	// we avoid that the random is the same previous preset (TODO:improve). we want force change, not stay in the same. 
	// bc sometimes the random gets the same current preset.

	int _PRESET_selected_PRE = PRESET_Selected_IndexMain;

	int r = doRandomIndexChanged();

#ifdef DEBUG_randomTest
	ofLogNotice(__FUNCTION__) << "dicesTotalAmounts: " << ofToString(dicesTotalAmount);
#endif

	// TODO:
	// if there's only one posible dice.. cant avoid repetition. so force avoid toggle to false
	if (MODE_AvoidRandomRepeat && dicesTotalAmount < 2) MODE_AvoidRandomRepeat = false;

	if (MODE_AvoidRandomRepeat)
	{
		int numTryes = 0;
		ofLogVerbose(__FUNCTION__) << "Randomize Try #" << ofToString(++numTryes);
		ofLogVerbose(__FUNCTION__) << "PRESET : " << ofToString(r);

		while (r == _PRESET_selected_PRE && dicesTotalAmount > 1 && numTryes < 5) //while preset index not changed. TODO: avoid make more than 5 randoms..
		{
			r = doRandomIndexChanged();
			ofLogVerbose(__FUNCTION__) << "Randomize Try #" << ofToString(++numTryes) << " NOT changed!";
			ofLogVerbose(__FUNCTION__) << "PRESET Previous was : " << ofToString(_PRESET_selected_PRE);
			ofLogVerbose(__FUNCTION__) << "PRESET New Random is: " << ofToString(r);
			ofLogVerbose(__FUNCTION__) << "RETRY !";
		}
	}
	else ofLogVerbose(__FUNCTION__) << "PRESET : " << ofToString(r);

	ofLogNotice(__FUNCTION__) << "Randomize " + group.getName() + "PRESET : " << ofToString(r);

	//--

	// 4. apply preset selection

	loadPreset(r);

	//--

	// 5. start timer again

	if (PLAY_RandomizeTimer)
	{
		randomizerTimer = ofGetElapsedTimeMillis();
	}
}

//--------------------------------------------------------------
void groupRandomizer::doResetDices()
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
	randomizeDurationShort = randomizeDuration * randomizeDurationShortRatio;
}

//--------------------------------------------------------------
void groupRandomizer::addGroupToEditor(ofParameterGroup& group) {
	//editorPresets.clear();// ?

	// TODO:
	// if we want to make nested folders (not all toggles into same and only one level, we need to create subgroups too...)

	for (auto parameter : group)
	{
		// group
		auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
		if (parameterGroup)
		{
			addGroupToEditor(*parameterGroup);
			continue;
		}

		// exclude params not marked as serializable
		if (parameter->isSerializable())
		{
			// parameter, try everything we know how to handle.

			// x,y,z vectors
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
			// colors
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
			// normal types
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
void groupRandomizer::doRandomPreset() {
	ofLogNotice(__FUNCTION__);

	//if(params_Editor_Toggles.size() == 0) params_Editor_Toggles

	ofParameterGroup _group = group;
	doRandomGroup(_group);

	//#ifdef INCLUDE_ofxUndoSimple
	//	if (MODE_Editor.get())
	//		doStoreUndo();// store scene
	//#endif
}

//--------------------------------------------------------------
void groupRandomizer::doRandomGroup(ofParameterGroup& group) {
	for (auto parameter : group)
	{
		if (parameter->isSerializable())// avoid not serailizable params that will crash
		{
			// recursive..
			auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
			if (parameterGroup)
			{
				//cout << "parameterGroup: " << ofToString(parameterGroup->getName()) << endl;
				doRandomGroup(*parameterGroup);
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

			ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter: " << parameter->getName();
		}
	}
}

//--------------------------------------------------------------
void groupRandomizer::setupRandomizerParams()
{
	params_randomizer.setName("RANDOMIZERS");
	editorPresets.clear();

	addGroupToEditor(group);// enqueue all content params and create a toggle for each one

	// add to group
	bRandomizeEditor.set("RANDOMIZE PARAMETERS", false);
	bRandomizeEditorPopulateFavs.set("POPULATE FAVS!", false);
	bRandomizeEditorAll.set("ALL", false);
	bRandomizeEditorNone.set("NONE", false);

	params_Editor.clear();
	params_Editor.setName("RANDOM EDIT");
	params_Editor.add(bRandomizeEditor);
	params_Editor.add(bRandomizeEditorAll);
	params_Editor.add(bRandomizeEditorNone);
	//params_Editor.add(bRandomizeEditorPopulateFavs);// TODO: not implemented

	params_Editor_Toggles.setName("PRESET PARAMETERS");
	params_Editor_Toggles.clear();

	for (auto &p : editorPresets) {
		params_Editor_Toggles.add(p);
	}
	params_Editor.add(params_Editor_Toggles);

	// exclude
	bRandomizeEditor.setSerializable(false);
	bRandomizeEditorPopulateFavs.setSerializable(false);
	bRandomizeEditorAll.setSerializable(false);
	bRandomizeEditorNone.setSerializable(false);

	//--

	// callback
	ofAddListener(params_Editor.parameterChangedE(), this, &groupRandomizer::Changed_Editor);
}

//--------------------------------------------------------------
void groupRandomizer::buildRandomizers()
{
	// radomizer
	setupRandomizerIndex();

	// preset editor tools
	setupRandomizerParams();
}

//--------------------------------------------------------------
void groupRandomizer::exit()
{
	// save randomizers settings
	ofxSurfingHelpers::saveGroup(params_RandomizerSettings, path_RandomizerSettings);

	ofRemoveListener(params_Control.parameterChangedE(), this, &groupRandomizer::Changed_Control);
	ofRemoveListener(params_Randomizer.parameterChangedE(), this, &groupRandomizer::Changed_Control);
	ofRemoveListener(params_Editor.parameterChangedE(), this, &groupRandomizer::Changed_Editor);
}

//--------------------------------------------------------------
void groupRandomizer::update()
{
	//----

	// randomizer timer mode latch

	// TODO:
	// on this mode: when we click to some presets, we load the preset, but after duration, 
	// we jump back to preset index 0

	// easy callback
	// latch mode
	if (bIsDoneLoad && MODE_LatchTrig && !PLAY_RandomizeTimer)
	{
		bIsDoneLoad = false;
		randomizerTimer = ofGetElapsedTimeMillis();

		if (PRESET_Selected_IndexMain != 0)
		{
			bLatchRun = true;
		}
		else
		{
			bLatchRun = false;
		}
	}

	//----

	if (PLAY_RandomizeTimer || MODE_LatchTrig)// ?
	{
		uint32_t _time = ofGetElapsedTimeMillis();
		timerRandomizer = _time - randomizerTimer;

		//ofLogNotice(__FUNCTION__) << " : " << timerRandomizer;

		if (PRESET_Selected_IndexMain < presetsRandomModeShort.size()) {// avoid out of range

			// A. long mode
			if (presetsRandomModeShort[PRESET_Selected_IndexMain] == false)// get if it's marked as shor or long by default (false)
			{
				if (timerRandomizer >= randomizeDuration)
				{
					if (MODE_LatchTrig) {
						if (bLatchRun) {
							loadPreset(0);
						}
					}
					else {
						bRandomizeIndex = true;// TODO: can be improved calling directly the method! bc this flag will be readed on update()..
					}
				}
			}

			// B. short mode
			else
			{
				if (timerRandomizer >= randomizeDurationShort)
				{
					if (MODE_LatchTrig) {
						if (bLatchRun) loadPreset(0);
					}
					else bRandomizeIndex = true;
				}
			}
		}
	}

	//--

	// 1.0.2 draw progress bar for the randomizer timer

	//// long mode
	//if (presetsRandomModeShort[PRESET_Selected_IndexMain - 1] == false) _prog = timerRandomizer / (float)randomizeDuration;
	//// short mode
	//else _prog = timerRandomizer / (float)randomizeDurationShort;
	// bar relative only to long

	if (PLAY_RandomizeTimer)
	{
		randomizerProgress = 100 * timerRandomizer / (float)randomizeDuration;
	}
	else if (MODE_LatchTrig)
	{
		if (bLatchRun)
		{
			randomizerProgress = 100 * timerRandomizer / (float)randomizeDuration;
		}
		else
		{
			randomizerProgress = 0;
		}
	}
	else
	{
		randomizerProgress = 0;
	}
	_prog = MIN(100, randomizerProgress / 100.f);
}

//--------------------------------------------------------------
void groupRandomizer::setupRandomizerIndex()
{
	bRandomizeIndex.set("RANDOMIZE INDEX", false);
	PLAY_RandomizeTimer.set("PLAY RANDOMIZER", false);
	PLAY_RandomizeTimer.setSerializable(false);
	MODE_DicesProbs.set("ENABLE MODE RANDOMIZER", true);
	MODE_LatchTrig.set("MODE LATCH-N-BACK", false);
	MODE_AvoidRandomRepeat.set("MODE AVOID REPEAT", true);
	randomizeDuration.set("t DURATION", 1000, 10, randomize_MAX_DURATION);
	randomizeDurationShortRatio.set("t RATIO", 0.25, 0.005, 1);
	randomizeDurationShort.set("t SHORT", 250, 10, randomize_MAX_DURATION);// locked
	randomizeDurationBpm.set("t BPM", 120, 10, 400);
	randomizedDice.set("DICE", 0, 0, amountPresets - 1);
	bResetDices.set("RESET PROBS", false);

	// exclude
	randomizeDurationShort.setSerializable(false);// lock
	bRandomizeIndex.setSerializable(false);
	bResetDices.setSerializable(false);

	// erase
	presetsRandomFactor.clear();
	presetsRandomModeShort.clear();
	randomFactorsDices.clear();

	// resize
	presetsRandomFactor.resize(amountPresets);
	presetsRandomModeShort.resize(amountPresets);
	randomFactorsDices.resize(amountPresets);

	int i;

	// ints as probability for every preset
	ofParameterGroup _gOdds{ "PRESETS PROBS" };
	_gOdds.clear();
	i = 0;
	for (auto &p : presetsRandomFactor) {
		string n = "PROB " + ofToString(i++);
		p.set(n, 5, 0, 10);
		_gOdds.add(p);
	}

	// toggles to enable short duration mode
	ofParameterGroup _gShort{ "PRESETS DURATION" };
	_gShort.clear();
	i = 0;
	for (auto &p : presetsRandomModeShort) {
		string n = "SHORT " + ofToString(i++);
		p.set(n, false);
		_gShort.add(p);
	}

	params_Randomizer.clear();
	params_Randomizer.setName("RANDOM SELECTED INDEX");
	params_Randomizer.add(PLAY_RandomizeTimer);
	params_Randomizer.add(bRandomizeIndex);
	params_Randomizer.add(MODE_DicesProbs);
	params_Randomizer.add(randomizeDurationBpm);
	params_Randomizer.add(randomizeDuration);
	params_Randomizer.add(randomizeDurationShortRatio);
	params_Randomizer.add(randomizeDurationShort);// locked
	params_Randomizer.add(MODE_LatchTrig);
	params_Randomizer.add(MODE_AvoidRandomRepeat);
	params_Randomizer.add(_gOdds);// probs
	params_Randomizer.add(_gShort);// toggles
	params_Randomizer.add(bResetDices);
#ifdef DEBUG_randomTest
	params_Randomizer.add(randomizedDice);
#endif
}

//----------------------------------------------------------------
void groupRandomizer::keyPressed(int key)
{
	////if ((mod_CONTROL && !mod_ALT) && key == 'R')
	//if (key == 'R')
	//{
	//	doRandomPreset();
	//}

	////bool bEnableKeyRandomizers = false;
	////if (bEnableKeyRandomizers) 
	//{
	//	//timer to randomize and choice a random preset from the kit
	//	if (key == 'R')
	//	{
	//		setTogglePlayRandomizerPreset();
	//	}
	//	else if (key == 'r')
	//	{
	//		doRandomIndex();
	//	}
	//	else if (key == 'E')
	//	{
	//		doRandomPreset();
	//	}
	//}
}

//--------------------------------------------------------------
void groupRandomizer::ImGui_Draw_GroupRandomizers()
{
	// 1. randomizers
	string str;
	str = "GROUP " + group.getName();

	static bool auto_resize = true;
	ImGuiWindowFlags flagsw;
	flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;

	if (ofxImGui::BeginWindow(str.c_str(), settings, flagsw))
	{
		float _h = WIDGET_HEIGHT;
		float _spc = ImGui::GetStyle().ItemInnerSpacing.x;
		float _w100 = ImGui::GetWindowWidth();
		float _w99 = _w100 - 20;
		float _w50 = _w99 / 2 - _spc;

		// preset selector

		ImGui::Dummy(ImVec2(0, 5));

		////string str = "User-Kit: " + displayNameUserKit;
		//str = "  Group    " + group.getName();
		//ImGui::Text(str.c_str());

		str = "  Preset   " + ofToString(PRESET_Selected_IndexMain.get());
		ImGui::Text(str.c_str());

		ImGui::Dummy(ImVec2(0, 5));

		ofxImGui::AddParameter(PRESET_Selected_IndexMain);

		//ImGui::SameLine();
		//ImGui::Dummy(ImVec2(0, 5));

		//--

		// TODO: copy this code to my ImGui hewlpers..

		// preset clicker matrix buttons
		{
			//ImGui::Text("PRESET SELECTOR:");
			// toggle button matrix
			ImVec2 button_sz(40, 40);
			// Manually wrapping
			// (we should eventually provide this as an automatic layout feature, but for now you can do it manually)

			ImGuiStyle& style = ImGui::GetStyle();
			int _amtButtons = amountPresets;
			float _windowVisible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

			for (int n = 0; n < _amtButtons; n++)
			{
				ImGui::PushID(n);

				string name = ofToString((char)(keys[n]));

				// customize colors
				{
					if (PRESET_Selected_IndexMain.get() == n)// when selected
					{
						const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonHovered];// changes the color
						ImGui::PushStyleColor(ImGuiCol_Button, colorActive);
					}
					else {
						const ImVec4 colorButton = style.Colors[ImGuiCol_Button];// do not changes the color
						ImGui::PushStyleColor(ImGuiCol_Button, colorButton);
					}
					// draw button
					if (ImGui::Button(name.c_str(), button_sz))
					{
						loadPreset(n);// trig load preset
					}
					// customize colors
					ImGui::PopStyleColor();
				}
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
				if (n + 1 < _amtButtons && next_button_x2 < _windowVisible_x2) ImGui::SameLine();

				ImGui::PopID();
			}
		}

		//--

		ImGui::Dummy(ImVec2(0, 5));

		// TODO: not implemented bc must be backward..
		// main helpers
		if (MODE_Editor)
		{
			if (ImGui::Button("CLONE ALL", ImVec2(_w50, _h)))
			{
				bCloneAll = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("CLONE >", ImVec2(_w50, _h)))
			{
				bCloneRight = true;
			}

			// TODO:
			//ImGui::SameLine();
			//if (ImGui::Button("POPULATE ALL!"))
			//{
			//	bPopulateAll = true;
			//	//// populate all favs
			//	//doPopulateFavs();
			//	//// create browser files too
			//	//doGetFavsToStandalonePresets();
			//}
		}

		//--

		// 1. presets randomizers

		//--

		ImGui::Dummy(ImVec2(0, 10));

		str = "RANDOMIZER";
		ImGui::Text(str.c_str());
		ImGui::Dummy(ImVec2(0, 5));

		// 1.0.1 play randomizer index

		//ofxImGui::AddParameter(PLAY_RandomizeTimer);
		//ofxSurfingHelpers::AddBigToggle(PLAY_RandomizeTimer, 30);
		ofxSurfingHelpers::AddBigToggle(PLAY_RandomizeTimer, 2 * _h, "STOP RANDOMIZER", "PLAY RANDOMIZER");

		//-

		// 1.0.1B bpm slider

		auto parameter = randomizeDurationBpm;
		auto tmpRef = randomizeDurationBpm.get();
		auto name = ofxImGui::GetUniqueName(randomizeDurationBpm);

		if (ImGui::SliderFloat(ofxImGui::GetUniqueName(parameter), (float *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
		}

		//--

		// 1.0.2 draw progress bar for the randomizer timer
		{
			ImGuiStyle *style = &ImGui::GetStyle();

			ImGui::PushID("prog");
			const ImVec4 color = style->Colors[ImGuiCol_ButtonHovered];//we can force change this color on theme... only used here
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
			//ImGui::ProgressBar(randomizerProgress.get());
			ImGui::ProgressBar(_prog);
			ImGui::PopStyleColor();
			ImGui::PopID();
			//cout << _prog << endl;
		}

		//--

		if (MODE_Editor)
		{
			// 1.0.3 bang randomize

			ImGui::Dummy(ImVec2(0, 5));

			ofxSurfingHelpers::AddBigButton(bRandomizeEditor, _w99, 2 * _h);
			//ofxSurfingHelpers::AddBigButton(bRandomizeEditor, 2 * _h);
			//ofxImGui::AddParameter(bRandomizeEditor);// trig random current preset: will randomize all enabled toggle parameters
		}

		//--

		//// TODO: undo engine
		//if (MODE_Editor)
		//{
		//	float wHalf = 100;
		//	if (ImGui::Button("UNDO", ImVec2(wHalf, 20)))
		//	{
		//		//ofLogNotice(__FUNCTION__) << "UNDO <-";
		//		//undoStringParams.undo();
		//		//doRefreshUndoParams();
		//	}
		//	ImGui::SameLine();
		//	if (ImGui::Button("REDO", ImVec2(wHalf, 20)))
		//	{
		//		//ofLogNotice(__FUNCTION__) << "REDO ->";
		//		//undoStringParams.redo();
		//		//doRefreshUndoParams();
		//	}
		//	//string str = "";
		//	//str += ofToString(undoStringParams.getUndoLength()) + "/";
		//	//str += ofToString(undoStringParams.getRedoLength());
		//	//ImGui::Text(str.c_str());
		//}

		//--

		if (MODE_Editor)
		{
			ImGui::Dummy(ImVec2(0, 5));

			if (ImGui::CollapsingHeader("EDIT RANDOMIZERS"))
			{
				// 1.1 randomizers presets

				//if (MODE_DicesProbs) 
				ofxImGui::AddGroup(params_Randomizer, settings);

#ifdef DEBUG_randomTest
				ImGui::Text("%d/%d", randomizedDice.get(), randomizedDice.getMax());
#endif
				//--

				// 1.2 randomizers editor

				if (MODE_Editor)
				{
					ofxImGui::AddGroup(params_Editor, settings);
				}
			}
		}

		ImGui::Checkbox("Auto-Resize", &auto_resize);
	}
	ofxImGui::EndWindow(settings);
}

//--------------------------------------------------------------
void groupRandomizer::Changed_Editor(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		ofLogNotice(__FUNCTION__) << group.getName() << " " << name << " : " << e;

		//-

		if (name == bRandomizeEditor.getName() && bRandomizeEditor)// trig randomize
		{
			bRandomizeEditor = false;

			doRandomPreset();
		}
		else if (name == bRandomizeEditorAll.getName() && bRandomizeEditorAll)// all
		{
			bRandomizeEditorAll = false;

			for (auto &p : editorPresets) {
				p.set(true);
			}
		}
		else if (name == bRandomizeEditorNone.getName() && bRandomizeEditorNone)// none
		{
			bRandomizeEditorNone = false;

			for (auto &p : editorPresets) {
				p.set(false);
			}
		}
		else if (name == bRandomizeEditorPopulateFavs.getName() && bRandomizeEditorPopulateFavs)// populate random all favs
		{
			bRandomizeEditorPopulateFavs = false;
			//doPopulateFavs();
		}
	}
}

//--------------------------------------------------------------
void groupRandomizer::doDices()// calculate all probabilities for all presets
{
	// sum total dices/all probs
	dicesTotalAmount = 0;
	for (auto &p : presetsRandomFactor) {
		dicesTotalAmount += p.get();
	}
	randomizedDice.setMax(dicesTotalAmount - 1);

	ofLogNotice(__FUNCTION__) << group.getName() << " dicesTotalAmount: " << dicesTotalAmount;
}

//--------------------------------------------------------------
void groupRandomizer::Changed_Control(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		//if ((name != "exclude") //&&
		//	//(name != "DICE") &&
		//	//(name != "PRESET")
		//	)
		{
			ofLogNotice(__FUNCTION__) << "name: " << group.getName() << " " << name << ": " << e;
		}

		if (false) {}

		//----

		// index preset selector
		else if (name == PRESET_Selected_IndexMain.getName())
		{
			ofLogNotice(__FUNCTION__) << group.getName() << " index: " << PRESET_Selected_IndexMain.get();

			// TODO:
			//selectorTARGET = PRESET_Selected_IndexMain;
		}

		//--

		// helper tools
		else if (name == "CLONE >" && bCloneRight)
		{
			ofLogNotice(__FUNCTION__) << group.getName() << "CLONE >: " << e;
			//bCloneRight = false;
			//doCloneRight(PRESET_Selected_IndexMain);
		}
		else if (name == "CLONE ALL" && bCloneAll)
		{
			ofLogNotice(__FUNCTION__) << group.getName() << "CLONE ALL: " << e;
			//bCloneAll = false;
			//doCloneAll();
		}
		else if (name == "POPULATE ALL" && bPopulateAll)
		{
			ofLogNotice(__FUNCTION__) << group.getName() << "POPULATE ALL: " << e;
			//bCloneAll = false;
			//doCloneAll();
		}

		//--

		// randomizer
		else if (name == bRandomizeIndex.getName() && bRandomizeIndex)
		{
			ofLogNotice(__FUNCTION__) << group.getName() << "RANDOMIZE !";
			bRandomizeIndex = false;
			doRandomIndex();
		}
		// play randomizer
		else if (name == PLAY_RandomizeTimer.getName())
		{
			ofLogNotice(__FUNCTION__) << group.getName() << "MODE TIMER: " << e;
			if (PLAY_RandomizeTimer) {
				MODE_LatchTrig = false;

				// TODO: new test
				randomizerTimer = ofGetElapsedTimeMillis();
			}
		}
		// latch
		else if (name == "MODE LATCH-N-BACK")
		{
			ofLogNotice(__FUNCTION__) << "MODE LATCH: " << e;
			if (MODE_LatchTrig) {
				PLAY_RandomizeTimer = false;
			}
		}

		// durations
		else if (name == randomizeDuration.getName())
		{
			ofLogNotice(__FUNCTION__) << "DURATION: " << e;

			randomizeDurationBpm = (MAX_DURATION_RATIO * 60000.f) / randomizeDuration;
		}
		else if (name == randomizeDurationBpm.getName())
		{
			ofLogNotice(__FUNCTION__) << "BPM: " << e;

			// 60,000 ms (1 minute) / Tempo (BPM) = Delay Time in ms for quarter-note beats
			randomizeDuration = (MAX_DURATION_RATIO * 60000.f) / randomizeDurationBpm;
			randomizeDurationShort = randomizeDuration * randomizeDurationShortRatio;
		}
		else if (name == randomizeDurationShortRatio.getName())
		{
			ofLogNotice(__FUNCTION__) << "Short ratio: " << e;
			randomizeDurationShort = randomizeDuration * randomizeDurationShortRatio;
		}
		else if (name == randomizeDurationShort.getName())
		{
			ofLogNotice(__FUNCTION__) << "Lock Short ratio: " << e;
			randomizeDurationShort = randomizeDuration * randomizeDurationShortRatio;
		}

#ifdef DEBUG_randomTest
		else if (name == "DICE")// when debug enabled: set dice by user to test
		{
			ofLogNotice(__FUNCTION__) << "DICE: " << e;
			doRandomIndex();
		}
#endif
		else if (name == bResetDices.getName() && bResetDices)
		{
			ofLogNotice(__FUNCTION__) << "RESET DICES: " << e;
			doResetDices();
			bResetDices = false;
		}

		//--

		// all other widgets/params that are populate on the thw fly, not hardcoded
		else
		{
			// check if changed prob sliders
			bool _doDices = false;
			for (int i = 0; i < presetsRandomFactor.size(); i++)
			{
				if (name == "PROB " + ofToString(i)) {
					ofLogNotice(__FUNCTION__) << name << " : " << e;
					_doDices = true;// TODO: would be faster making return on first 'true'
				}
			}
			if (_doDices) doDices();
		}
	}
}

//--------------------------------------------------------------
void groupRandomizer::loadPreset(int p)
{
	ofLogNotice(__FUNCTION__) << group.getName() << " : " << p;
	PRESET_Selected_IndexMain = p;
	bIsDoneLoad = true;// TODO: workaround bc the done-loading happens out of the class..

	// TODO:
	////if (selectorTARGET) 
	//{
	//selectorTARGET = PRESET_Selected_IndexMain;
	//}
}

////--------------------------------------------------------------
//void groupRandomizer::doCheckPresetsFoldersAreEmpty()
//{
//	//string _path = path_UserKit_Folder + "/" + path_PresetsFavourites;
//	//ofLogNotice(__FUNCTION__) << "Check that not empty folder at path: " << _path;
//	//ofDirectory dataDirectory(ofToDataPath(_path, true));
//
//	//check if folder exist
//	if (!dataDirectory.isDirectory())
//	{
//		ofLogError(__FUNCTION__) << "FOLDER DOES NOT EXIST!";
//	}
//
//	//check if folder is empty
//	if (dataDirectory.size() == 0) {
//		ofLogNotice(__FUNCTION__) << "Folder " << _path << " is empty. Force populate favourites files...";
//
//		//populate all favs
//		doPopulateFavs();
//		//create browser files too
//		doGetFavsToStandalonePresets();
//	}
//
//	//verify if files are created
//	ofLogNotice(__FUNCTION__) << ofToString(dataDirectory.size()) << " file preset at folder " << _path;
//}


// TODO: alternative pointer target
////--------------------------------------------------------------
//void groupRandomizer::setSelectorTARGET(ofParameter<int> &index) {
//	ofLogNotice(__FUNCTION__) << "target registered: " << index.getName();
//
//	// TODO:
//	//PRESET_Selected_IndexMain.makeReferenceTo(index);// TODO: it brokes saveing well..
//	//PRESET_Selected_IndexMain.makeReferenceTo(selectorTARGET);
//
//	// TODO:
//	//selectorTARGET = index;
//}
////--------------------------------------------------------------
//void groupRandomizer::setup(ofParameterGroup &g, int _numPresets, ofParameter<int> &index) {
//	setup(g, _numPresets);
//	setSelectorTARGET(index);
//}