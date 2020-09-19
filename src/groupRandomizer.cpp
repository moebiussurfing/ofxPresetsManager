#include "groupRandomizer.h"

//--------------------------------------------------------------
groupRandomizer::groupRandomizer() {}

//--------------------------------------------------------------
groupRandomizer::~groupRandomizer()
{
	//exit();
}

//--------------------------------------------------------------
void groupRandomizer::setup(ofParameterGroup &g, int _numPresets) {
	ofLogNotice(__FUNCTION__) << "add randomizer: " << g.getName() << " amount presets: " << _numPresets;

	group = g;

	// update control gui panel params
	mainGroupAmtPresetsFav = _numPresets;
	PRESET_Selected_IndexMain.setMax(mainGroupAmtPresetsFav - 1);
	
	// TODO:
	// add keys
	for (int i = 0; i < _numPresets; i++) {
		char c = (char)(i);
		keys.push_back(c);
		ofLogNotice(__FUNCTION__) << "add key: " << c;
	}

	//// load randomizers settings
	//string path2;
	//bool b2;
	//path2 = path_UserKit_Folder + "/" + path_ControlSettings + "/" + filename_Randomizers;
	//b2 = ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path2);
	//ofLogNotice(__FUNCTION__) << "Loaded " << path2 << " " << (b2 ? "DONE" : "FAILED");	//--

	//--

	// radomizers
	buildRandomizers();

	// randomizer settings
	params_RandomizerSettings.add(params_Randomizer);
	params_RandomizerSettings.add(params_Editor);
	//bool b2 = ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path2);

	params_HelperTools.setName("HELPER TOOLS");
	params_HelperTools.add(bCloneRight);
	params_HelperTools.add(bCloneAll);

	params_Control.setName("groupRandomizer");
	//params_Control.add(bSave);
	//params_Control.add(bLoad);
	params_Control.add(params_HelperTools);

	ofAddListener(params_Control.parameterChangedE(), this, &groupRandomizer::Changed_Params_Control);
	ofAddListener(params_Randomizer.parameterChangedE(), this, &groupRandomizer::Changed_Params_Control);

}

//--------------------------------------------------------------
int groupRandomizer::doRandomizeWichSelectedPresetCheckChanged()
{
	ofLogVerbose(__FUNCTION__);

	//-

	int _r = PRESET_Selected_IndexMain;

	if (MODE_DicesProbs) {

		// 1. dice randomize

#ifndef DEBUG_randomTest
		{
			// get a random between all possible dices (from 0 to dicesTotalAmount) and then select the preset associated to the resulting dice.
			// each preset has many dices: more dices 0 more probality to be selected by the randomizer
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
		//for (int i = 0; i < presetsRandomFactor.size(); i++) {
		for (int i = 0; i <= presetsRandomFactor.size(); i++) {

			// define upper/lower limits for each dices/preset
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
void groupRandomizer::doRandomizeWichSelectedPreset()
{
	//ofLogNotice(__FUNCTION__);
	ofLogNotice(__FUNCTION__) << "---------------------------------------------------";

	// we avoid that the random is the same previous preset (TODO:improve). we want force change, not stay in the same. 
	// bc sometimes the random gets the same current preset.

	int _PRESET_selected_PRE = PRESET_Selected_IndexMain;

	int r = doRandomizeWichSelectedPresetCheckChanged();

	// TODO:
	// if there's only one posible dice.. cant avoid repetition. so force avoid toggle to false
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

	// 4. apply preset selection

	ofLogNotice(__FUNCTION__) << "PRESET > " << ofToString(r);
	loadPreset(r);

	//int __r = (int)ofRandom(1.0f, 9.0f);
	//ofLogNotice(__FUNCTION__) << "\t > " << ofToString(__r);
	//loadPreset(__r);

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
	randomizeDurationShort = randomizeDuration * 0.5;
}

//--------------------------------------------------------------
void groupRandomizer::addGroupToEditor(ofParameterGroup& group) {
	//editorPresets.clear();//?

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
void groupRandomizer::doRandomizeEditor() {
	ofLogNotice(__FUNCTION__);

	//if(params_Editor_Toggles.size() == 0) params_Editor_Toggles

	ofParameterGroup _group = group;
	doRandomizeEditorGroup(_group);

#ifdef INCLUDE_ofxUndoSimple
	if (MODE_Editor.get())
		undoStoreParams();// store scene
#endif
}

//--------------------------------------------------------------
void groupRandomizer::doRandomizeEditorGroup(ofParameterGroup& group) {
	for (auto parameter : group)
	{
		if (parameter->isSerializable())// avoid not serailizable params that will crash
		{
			// recursive..
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
void groupRandomizer::setupRandomizerEditor()
{
	params_randomizer.setName("RANDOMIZERS");
	editorPresets.clear();//?

	//int num = groups[0].getGroupHierarchyNames().size();
	addGroupToEditor(group);//enqueue all content params and create a toggle for each one

	//add to group
	bRandomizeEditor.set("RANDOMIZE PRESET", false);
	bRandomizeEditorPopulateFavs.set("POPULATE FAVS!", false);
	bRandomizeEditorAll.set("ALL", false);
	bRandomizeEditorNone.set("NONE", false);

	params_Editor.clear();
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

	//exclude
	bRandomizeEditor.setSerializable(false);
	bRandomizeEditorPopulateFavs.setSerializable(false);
	bRandomizeEditorAll.setSerializable(false);
	bRandomizeEditorNone.setSerializable(false);

	//--

	////TODO: 
	//workflow
	{
		//if there are some toggles/params enabled to allow random, we do not do nothing
		//if there's no toggle/params enabled, we enable all params toggles to the editor 
		//so random will affect to all params by default

		//workflow
		bool someFalse = false;//none is false
		for (auto &p : editorPresets)
		{
			if (!p.get()) someFalse = true;//some is false
		}

		//if all toggles are disabled after loaded settings, 
		//then put all to true to allow populate, and random make something
		if (!someFalse) bRandomizeEditorAll = true;//someone is true
	}

	//--

	//callback
	ofAddListener(params_Editor.parameterChangedE(), this, &groupRandomizer::Changed_Params_Editor);
}

//--------------------------------------------------------------
void groupRandomizer::buildRandomizers()
{
	//radomizer
	setupRandomizer();

	//preset editor tools
	setupRandomizerEditor();
}

//--------------------------------------------------------------
void groupRandomizer::exit()
{
	ofRemoveListener(params_Control.parameterChangedE(), this, &groupRandomizer::Changed_Params_Control);
	ofRemoveListener(params_Randomizer.parameterChangedE(), this, &groupRandomizer::Changed_Params_Control);
	ofRemoveListener(params_Editor.parameterChangedE(), this, &groupRandomizer::Changed_Params_Editor);
}

//--------------------------------------------------------------
void groupRandomizer::update()
{
	//--

	// randomizer timer mode latch

	// callback
	//if (bIsDoneLoad &&
	//	MODE_LatchTrig &&
	//	!PLAY_RandomizeTimer) {
	//	randomizerTimer = ofGetElapsedTimeMillis();
	//	if (PRESET_Selected_IndexMain != 0)
	//	{
	//		bLatchRun = true;
	//	}
	//	else
	//	{
	//		bLatchRun = false;
	//	}
	//}

	//----

	if (PLAY_RandomizeTimer || MODE_LatchTrig)//?
	{
		uint32_t _time = ofGetElapsedTimeMillis();
		timerRandomizer = _time - randomizerTimer;

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

	//--

	// 1.0.2 draw progress bar for the randomizer timer
	//// long mode
	//if (presetsRandomModeShort[PRESET_Selected_IndexMain - 1] == false) _prog = timerRandomizer / (float)randomizeDuration;
	//// short mode
	//else _prog = timerRandomizer / (float)randomizeDurationShort;
	// bar relative only to long
	if (PLAY_RandomizeTimer) {
		randomizerProgress = 100 * timerRandomizer / (float)randomizeDuration;
	}
	else if (MODE_LatchTrig) {
		if (bLatchRun) {
			randomizerProgress = 100 * timerRandomizer / (float)randomizeDuration;
		}
		else {
			randomizerProgress = 0;
		}
	}
	else {
		randomizerProgress = 0;
	}
	_prog = randomizerProgress / 100.f;

	//--

	// TODO:
	//_totalDicesStr = "/ " + ofToString(randomizedDice.getMax());
}

//--------------------------------------------------------------
void groupRandomizer::setupRandomizer()
{
	//select a random preset (from 1 to 8)
	//params_Randomizer.setName("Randomizer");
	bRandomizeIndex.set("RANDOMIZE INDEX", false);
	PLAY_RandomizeTimer.set("PLAY RANDOMIZER", false);
	PLAY_RandomizeTimer.setSerializable(false);
	MODE_DicesProbs.set("MODE USE PROBS/DICES", true);
	MODE_LatchTrig.set("MODE LATCH", false);
	MODE_AvoidRandomRepeat.set("MODE AVOID REPEAT", false);
	randomizeDuration.set("t DURATION", 1000, 10, randomize_MAX_DURATION);
	randomizeDurationShort.set("t SHORT", 250, 10, randomize_MAX_DURATION_SHORT);
	randomizeDurationBpm.set("t BPM", 120, 10, 400);
	randomizedDice.set("DICE", 0, 0, mainGroupAmtPresetsFav - 1);
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
	presetsRandomFactor.resize(mainGroupAmtPresetsFav);
	presetsRandomModeShort.resize(mainGroupAmtPresetsFav);
	randomFactorsDices.resize(mainGroupAmtPresetsFav);

	int i;

	//ints as probability for every preset
	ofParameterGroup _gOdds{ "PRESETS PROBS" };
	_gOdds.clear();
	i = 0;
	for (auto &p : presetsRandomFactor) {
		string n = "PROB " + ofToString(i++);
		p.set(n, 5, 0, 10);
		_gOdds.add(p);
	}

	//toggles to enable short duration mode
	ofParameterGroup _gShort{ "MODE DURATION SHORT" };
	_gShort.clear();
	i = 0;
	for (auto &p : presetsRandomModeShort) {
		string n = "SHORT " + ofToString(i++);
		p.set(n, false);
		_gShort.add(p);
	}

	params_Randomizer.clear();
	params_Randomizer.setName("FAVOURITES");
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


//----------------------------------------------------------------
void groupRandomizer::keyPressed(int key)
{
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
}


//--------------------------------------------------------------
void groupRandomizer::ImGui_Draw_GroupRandomizers(ofxImGui::Settings &settings)
{
	// 1. randomizers

	//if (ImGui::TreeNode("GROUP RANDOMIZERS"))
	if (ofxImGui::BeginTree("GROUP RANDOMIZERS", settings))
	{
		//---

		// preset selector

		// group 0 

		string str = group.getName();
		//string str = "User-Kit: " + displayNameUserKit;
		ImGui::Text(str.c_str());

		//ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//--

		// preset clicker matrix buttons
		{
			// toggle button matrix
			ImVec2 button_sz(40, 40);
			// Manually wrapping
			// (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
			//ImGui::Text("PRESET SELECTOR:");
			ImGuiStyle& style = ImGui::GetStyle();
			int _amtButtons = mainGroupAmtPresetsFav;
			float _windowVisible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			for (int n = 0; n < _amtButtons; n++)
			{
				ImGui::PushID(n);
				
				string name = ofToString((char)(keys[n]));
				//string name = ofToString((char)(keys[0][n]));

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

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//--

		// main helpers
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
			// populate all favs
			doPopulateFavs();
			// create browser files too
			doGetFavsToFilesBrowser();
		}

		//--

		//ImGui::Spacing(0, 10.0f);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//--

		//1. presets randomizers

		ImGuiStyle *style = &ImGui::GetStyle();

		//--

		//1.0.1 play randomizer index
		ofxSurfingHelpers::AddBigToggle(PLAY_RandomizeTimer, 30);

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

		////1.0.2 draw progress bar for the randomizer timer

		ImGui::PushID("prog");
		const ImVec4 color = style->Colors[ImGuiCol_ButtonHovered];//we can force change this color on theme... only used here
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
		//ImGui::ProgressBar(randomizerProgress.get());
		ImGui::ProgressBar(_prog);
		ImGui::PopStyleColor();
		ImGui::PopID();
		//cout << _prog << endl;

		//--

		//1.0.3 bang randomize
		//AddBigButton(bRandomizeIndex, 25);//index
		ofxSurfingHelpers::AddBigButton(bRandomizeEditor, 25);//preset

		//float w = ImGui::GetWindowWidth();
		//style->FramePadding = ImVec2(4, 2);
		//style->ItemSpacing = ImVec2(6, 4);
		//style->ItemInnerSpacing = ImVec2(6, 4);
		//style->IndentSpacing = 6.0f;
		//style->ItemInnerSpacing = ImVec2(2, 4);
		//float wHalf = (ImGui::GetWindowWidth() * 0.5f) - ( 4 + 6 + 6 +4);


		// TODO: undo engine
		if (MODE_Editor) 
		{
			float wHalf = 100;
			if (ImGui::Button("UNDO", ImVec2(wHalf, 20)))
			{
				//ofLogNotice(__FUNCTION__) << "UNDO <-";
				//undoStringParams.undo();
				//undoRefreshParams();
			}
			ImGui::SameLine();
			if (ImGui::Button("REDO", ImVec2(wHalf, 20)))
			{
				//ofLogNotice(__FUNCTION__) << "REDO ->";
				//undoStringParams.redo();
				//undoRefreshParams();
			}
			//string str = "";
			//str += ofToString(undoStringParams.getUndoLength()) + "/";
			//str += ofToString(undoStringParams.getRedoLength());
			//ImGui::Text(str.c_str());
		}

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
		if (MODE_Editor) {
			ofxImGui::AddGroup(params_Editor, settings);
		}

		//-

		ofxImGui::EndTree(settings);
		//ImGui::TreePop();
	}
}


//--------------------------------------------------------------
void groupRandomizer::Changed_Params_Editor(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		ofLogNotice(__FUNCTION__) << group.getName() << " " << name << " : " << e;

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

// helpers

//--------------------------------------------------------------
void groupRandomizer::doCloneRight(int pIndex)
{
	ofLogNotice(__FUNCTION__) << "from preset: " << pIndex;

	//CheckAllFolders();

	//for (int i = pIndex + 1; i < mainGroupAmtPresetsFav; i++)
	//{
	//	save(i, 0);
	//}
}

//--------------------------------------------------------------
void groupRandomizer::doPopulateFavs()
{
	ofLogNotice(__FUNCTION__);

	//CheckAllFolders();//? required

	//for (int i = 0; i < mainGroupAmtPresetsFav; i++)
	//{
	//	doRandomizeEditor();
	//	save(i, 0);
	//}
}

//--------------------------------------------------------------
void groupRandomizer::doSwap(int groupIndex, int fromIndex, int toIndex)
{
	ofLogNotice(__FUNCTION__) << "group:" << groupIndex << " : " << fromIndex << "->" << toIndex;

	//string srcName = getPresetPath(groups[groupIndex].getName(), fromIndex);
	//string dstName = getPresetPath(groups[groupIndex].getName(), toIndex);

	//ofLogNotice(__FUNCTION__) << "Source: " << fromIndex;
	//ofLogNotice(__FUNCTION__) << "Dest  : " << toIndex;
	//ofLogNotice(__FUNCTION__) << "Source: " << srcName;
	//ofLogNotice(__FUNCTION__) << "Dest  : " << dstName;

	////1. save source preset (from memory) to temp file
	//string _pathSrc = "tempSrc.xml";
	//ofXml _settingsSrc;
	//ofSerialize(_settingsSrc, groups[groupIndex]);
	//_settingsSrc.save(_pathSrc);

	////2. load destination "from kit" to memory
	//std::string _pathDest = getPresetPath(groups[groupIndex].getName(), toIndex);
	//ofXml _settingsDest;
	//_settingsDest.load(_pathDest);
	//ofDeserialize(_settingsDest, groups[groupIndex]);

	////3. save destination preset (from memory) to temp file
	//ofXml _settingsDst2;
	//ofSerialize(_settingsDst2, groups[groupIndex]);

	////4. using files
	////save source (from dest)
	//_settingsDst2.save(srcName);
	//_settingsSrc.save(dstName);

	////5. delete temp file
	//ofFile _file;
	//_file.removeFile(_pathSrc);

	////workflow
	////6. auto load source (the same preset was selected befor swap clicked!)
	//PRESETS_Selected_Index[groupIndex] = toIndex;
}

//--------------------------------------------------------------
void groupRandomizer::doCloneAll()
{
	ofLogNotice(__FUNCTION__);

	//CheckAllFolders();

	////auto save current preset
	//if (autoSave)
	//{
	//	ofLogVerbose(__FUNCTION__) << "autosave preset: " << PRESET_Selected_IndexMain.get();
	//	//doSave(PRESET_Selected_IndexMain);
	//	save(PRESET_Selected_IndexMain, 0);
	//}

	////clone all
	//for (int i = 0; i < mainGroupAmtPresetsFav; i++)
	//{
	//	save(i, 0);
	//}
}




//--------------------------------------------------------------
void groupRandomizer::Changed_Params_Control(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		//if ((name != "exclude") //&&
		//	//(name != ImGui_Position.getName()) &&
		//	//(name != ImGui_Size.getName())// &&
		//	//(name != "DICE") &&
		//	//(name != "PRESET")
		//	)
		{
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		if (false) {}

		////--

		//// mode edit: when false, we disabled autosave to allow performance do faster
		//else if (name == MODE_Editor.getName())
		//{
		//	ofLogNotice(__FUNCTION__) << "MODE EDITOR: " << (MODE_Editor.get() ? "TRUE" : "FALSE");

		//	autoSave = MODE_Editor.get();

		//	//--

		//	//// TODO:
		//	//// refresh in other another better place?...
		//	//buildHelpInfo();
		//}

		////--

		//// save load
		//else if (name == "SAVE" && bSave)
		//{
		//	ofLogNotice(__FUNCTION__) << "SAVE: " << e;
		//	bSave = false;
		//	//doSave(PRESET_Selected_IndexMain);
		//	save(PRESET_Selected_IndexMain, 0);
		//}
		////else if (name == "LOAD" && bLoad)
		////{
		////	ofLogNotice(__FUNCTION__) << "LOAD: " << e;
		////	bLoad = false;
		////	doLoad(PRESET_Selected_IndexMain);
		////}

		////----

		//else if (name == "LOAD TO MEMORY" && loadToMemory)
		//{
		//	ofLogNotice(__FUNCTION__) << "loadToMemory:" << e;
		//	loadToMemory = false;
		//	load_AllKit_ToMemory();
		//}
		//else if (name == "SAVE FROM MEMORY" && saveFromMemory)
		//{
		//	ofLogNotice(__FUNCTION__) << "saveFromMemory:" << e;
		//	saveFromMemory = false;
		//	save_AllKit_FromMemory();
		//}
		//else if (name == "MODE MEMORY")
		//{
		//	ofLogNotice(__FUNCTION__) << "MODE MEMORY: " << e;

		//	if (MODE_MemoryLive)
		//	{
		//		//reload all xml preset files to memory
		//		load_AllKit_ToMemory();
		//	}
		//	else
		//	{
		//		//save all xml preset files to disk from memory
		//		save_AllKit_FromMemory();
		//	}
		//}

		//--

		//		else if (name == "GUI ImGui POSITION")
		//		{
		//			ofLogVerbose(__FUNCTION__) << "GUI BROWSER POSITION: " << e;
		//
		//			////clamp inside window
		//			//float x, y;
		//			//x = ofClamp(ImGui_Position.get().x, 0, ofGetWidth() - 200);
		//			//y = ofClamp(ImGui_Position.get().y, 0, ofGetHeight() - 20);
		//			//ImGui_Position = glm::vec2(x, y);
		//		}
		//		else if (name == "GUI ImGui SIZE")
		//		{
		//			ofLogVerbose(__FUNCTION__) << "GUI BROWSER SIZE: " << e;
		//
		//			////clamp inside window
		//			//float x, y;
		//			//x = ofClamp(ImGui_Position.get().x, 0, ofGetWidth() - 200);
		//			//y = ofClamp(ImGui_Position.get().y, 0, ofGetHeight() - 20);
		//			//ImGui_Position = glm::vec2(x, y);
		//		}

		//----

		// helper tools
		else if (name == "CLONE >" && bCloneRight)
		{
			ofLogNotice(__FUNCTION__) << "CLONE >: " << e;
			bCloneRight = false;
			doCloneRight(PRESET_Selected_IndexMain);
		}
		else if (name == "CLONE ALL" && bCloneAll)
		{
			ofLogNotice(__FUNCTION__) << "CLONE ALL: " << e;
			bCloneAll = false;
			doCloneAll();
		}

		//--

		// randomizer
		else if (name == "RANDOMIZE INDEX" && bRandomizeIndex)
		{
			ofLogNotice(__FUNCTION__) << "RANDOMIZE !";
			bRandomizeIndex = false;
			doRandomizeWichSelectedPreset();
		}
		// play randomizer
		else if (name == PLAY_RandomizeTimer.getName())
		{
			ofLogNotice(__FUNCTION__) << "MODE TIMER: " << e;
			if (PLAY_RandomizeTimer) {
				MODE_LatchTrig = false;
			}
		}
		// latch
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
		//	//randomizeDuration = randomize_MAX_DURATION * (1.f - randomizeSpeedF);
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

		//--

		//all other widgets/params
		else
		{
			//check if changed prob sliders
			{
				bool doDices = false;
				for (int i = 0; i < presetsRandomFactor.size(); i++)
				{
					if (name == "PROB " + ofToString(i)) {
						doDices = true;//TODO: would be faster making return on first 'true'
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
		}
	}
}