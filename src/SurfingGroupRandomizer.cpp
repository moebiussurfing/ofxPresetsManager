#include "SurfingGroupRandomizer.h"

//--------------------------------------------------------------
SurfingGroupRandomizer::SurfingGroupRandomizer() {
}

//--------------------------------------------------------------
SurfingGroupRandomizer::~SurfingGroupRandomizer()
{
	exit();
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::setup(ofParameterGroup& g, vector<int> _keysList)
{
	keys.reserve(_keysList.size());
	keys.resize(_keysList.size());
	keys = _keysList;

	labels.clear();
	for (auto& k : keys)
	{
		string s = ofToString(char(k));
		labels.push_back(s);
	}

	setup(g, keys.size());
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::setup(ofParameterGroup& _group, int _numPresets)
{
	ofLogNotice(__FUNCTION__) << "add randomizer: " << _group.getName() << " amount presets: " << _numPresets;
	group = _group;

	// update control gui panel params
	amountPresets = _numPresets;

	index_PresetSelected.set(_group.getName(), 0, 0, amountPresets - 1);

	// clicker
	if (amountPresets != 0)
	{
		//amntBtnsClicker.setMax(amountPresets - 1);
		//amntBtnsClicker.set(amountPresets - 1);

		if (amountPresets < 5)
		{
			amntBtnsClicker.setMax(amountPresets);
			amntBtnsClicker.set(amountPresets);
		}
		else {
			amntBtnsClicker.setMax(3);
			amntBtnsClicker.set(3);
		}
	}

	// list keys
	for (int i = 0; i < _numPresets; i++) {
		ofLogNotice(__FUNCTION__) << "added key: " << keys[i];
	}

	//--

	// randomizers
	buildRandomizers();

	// to store settings
	params_RandomizerSettings.add(params_Randomizer);
	params_RandomizerSettings.add(params_RandomizersFiltered);
	params_RandomizerSettings.add(bGui_PlayerEditor);

	params_RandomizerSettings.add(amntBtnsClicker);
	//params_RandomizerSettings.add(respBtnsClicker);

	//-

	bCloneRight.set("CLONE >", false);
	bCloneAll.set("CLONE ALL", false);
	bPopulateAll.set("POPULATE ALL", false);
	bGui_PlayerEditor.set("Editor", false);
	//bGui_PlayerEditor.setSerializable(false);

	params_HelperTools.setName("HELPER TOOLS");
	params_HelperTools.add(bCloneRight);
	params_HelperTools.add(bCloneAll);
	params_HelperTools.add(bPopulateAll);

	params_Control.setName("SurfingGroupRandomizer");
	params_Control.add(params_HelperTools);
	params_Control.add(index_PresetSelected);

	ofAddListener(params_Control.parameterChangedE(), this, &SurfingGroupRandomizer::Changed_Control);
	ofAddListener(params_Randomizer.parameterChangedE(), this, &SurfingGroupRandomizer::Changed_Control);

	//----

//	// randomizer
//#ifdef INCLUDE_ofxSurfingRandomizer
//	randomizer.setup(_group);
//#endif

	//----

	// startup
	startup();
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::startup()
{
	bDISABLE_CALLBACKS = false;

	// Reset to default state
	doReset();

#ifdef USE_GUI_MANAGER__GROUP_RANDOMIZER
	//TODO: crashes
	//bool bAutoDraw = true;
	//guiManager.setImGuiAutodraw(bAutoDraw);
	guiManager.setup(); // this instantiates and configurates ofxImGui inside the class object.
#endif

	// load randomizers settings
	ofxSurfingHelpers::loadGroup(params_RandomizerSettings, path_RandomizerSettings);

	doDices();

	//--

	// workflow
	doCheckRandomReady();
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::doCheckRandomReady()
{
	// if there are some toggles/params enabled to allow random, we do not do nothing
	// if there's no toggle/params enabled, we enable all params toggles to the editor 
	// so random will affect to all params by default

	ofLogNotice(__FUNCTION__) << "Check if randomizer editor has some param active...";

	//// workflow
	//bool bAllFalse = false;// if is true means that all are disabled
	//for (auto &p : randomizersFiltered_TogglesVector)
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
	//	bRandomizeFiltered_All = true;// someone is true
	//	ofLogNotice(__FUNCTION__) << "All are disabled. Force set all randomizer params enable!";
	//}

	// workflow
	bool bSomeTrue = false;
	for (auto& p : randomizersFiltered_TogglesVector)
	{
		ofLogNotice(__FUNCTION__) << p.getName() << " : " << p.get();
		if (p.get() == true) bSomeTrue = bSomeTrue || true;// some one is true
	}
	if (bSomeTrue) { //some is true
		ofLogNotice(__FUNCTION__) << "Some one is enabled. Avoid force enabler!";
	}
	else// none is true
	{
		bRandomizeFiltered_All = true;// someone is true
		ofLogNotice(__FUNCTION__) << "All are disabled. Force set all randomizer params enable!";
	}
}

//--------------------------------------------------------------
int SurfingGroupRandomizer::doGenerateRandomIndex()
{
	ofLogVerbose(__FUNCTION__);

	//-

	int _r = index_PresetSelected;//get current

	{
		// 1. Dice randomize

#ifndef DEBUG_randomTest
		{
			// Get a random between all possible dices (from 0 to dicesTotalAmount) and then select the preset associated to the resulting dice.
			// Each preset has many dices: more dices means more probability to be selected by the randomizer
			randomizedDice = ofRandom(0, dicesTotalAmount);

			ofLogVerbose(__FUNCTION__) << "random: " << randomizedDice.get() << "/" << dicesTotalAmount;
		}
#endif

		//-

		// 2. Define limits for range dices associated to any preset

		//randomFactorsDices[0] = 0;
		for (int i = 0; i < presetsRandomFactor.size(); i++) {
			randomFactorsDices[i] = 0;

			for (int f = 0; f < i; f++) {
				randomFactorsDices[i] += presetsRandomFactor[f];
			}
		}

		//-

		// 3. Check if dice is inside both ranges. to select preset (_rr) associated to dice 

		int _rr = 0;
		for (int i = 0; i <= presetsRandomFactor.size(); i++) {

			// Define upper/lower limits for each dices/preset
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

		// Debug
		ofLogVerbose(__FUNCTION__) << "DEBUG";
		for (int i = 0; i < presetsRandomFactor.size(); i++) {
#ifdef DEBUG_randomTest
			ofLogNotice(__FUNCTION__) << "randomFactorsDices: [" << i << "] " << randomFactorsDices[i];
			// << "-" << randomFactorsDices[i + 1] << endl;
#endif
		}

		// Last
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

	//else 
	//{
	//	int numTryes = 0;
	//	//avoid jump to same current preset
	//	while (index_PresetSelected == _r)//if not changed
	//	{
	//		ofLogWarning(__FUNCTION__) << "Randomize not changed! Try #" << ofToString(++numTryes);
	//		ofLogNotice(__FUNCTION__) << "PRESET Previous was : " << ofToString(_r);
	//		ofLogNotice(__FUNCTION__) << "PRESET New Random is: " << ofToString(index_PresetSelected);
	//		index_PresetSelected = (int)ofRandom(0, mainGroupMemoryFilesPresets.size());
	//		
	//		//if (MODE_MemoryLive) _r = (int)ofRandom(0, mainGroupMemoryFilesPresets.size());
	//		//_r = (int)ofRandom(1, mainGroupMemoryFilesPresets.size() + 1);
	//	}
	//}

	return _r;
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::doGoNextIndex()
{
	// Go Next cycled
	{
		if (index_PresetSelected >= index_PresetSelected.getMax()) {
			index_PresetSelected = index_PresetSelected.getMin();
		}
		else
		{
			index_PresetSelected++;
		}
	}


	// 5. Start timer again

	if (bPLAY_RandomizeTimer)
	{
		timerRandomLast = ofGetElapsedTimeMillis();
	}

	//--

	// 4. Apply preset selection
	loadPreset(index_PresetSelected);
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::doGoRandomIndex()
{
#ifdef DEBUG_randomTest
	ofLogNotice(__FUNCTION__) << "---------------------------------------------------";
#endif

	// We avoid that the random is the same previous preset (TODO:improve). 
	// we want force change, not stay in the same. 
	// bc sometimes the random gets the same current preset.

	int _indexPRE = index_PresetSelected;

	int r = doGenerateRandomIndex();

#ifdef DEBUG_randomTest
	ofLogNotice(__FUNCTION__) << "dicesTotalAmounts: " << ofToString(dicesTotalAmount);
#endif

	// TODO:
	// If there's only one possible dice.. cant avoid repetition. so force avoid toggle to false
	if (MODE_AvoidRandomRepeat && dicesTotalAmount < 2) MODE_AvoidRandomRepeat = false;

	if (MODE_AvoidRandomRepeat)
	{
		int numTryes = 0;
		ofLogVerbose(__FUNCTION__) << "Randomize Try #" << ofToString(++numTryes);
		ofLogVerbose(__FUNCTION__) << "PRESET : " << ofToString(r);

		// While preset index not changed. TODO: avoid make more than 5 randoms..
		while (r == _indexPRE && dicesTotalAmount > 1 && numTryes < 5)
		{
			r = doGenerateRandomIndex();

			ofLogVerbose(__FUNCTION__) << "Randomize Try #" << ofToString(++numTryes) << " NOT changed!";
			ofLogVerbose(__FUNCTION__) << "PRESET Previous was : " << ofToString(_indexPRE);
			ofLogVerbose(__FUNCTION__) << "PRESET New Random is: " << ofToString(r);
			ofLogVerbose(__FUNCTION__) << "RETRY !";
		}
	}
	else ofLogVerbose(__FUNCTION__) << "PRESET : " << ofToString(r);

	ofLogNotice(__FUNCTION__) << "Randomize " + group.getName() + "PRESET : " << ofToString(r);

	//--

	// 5. Start timer again

	if (bPLAY_RandomizeTimer)
	{
		timerRandomLast = ofGetElapsedTimeMillis();
	}

	//--

	// 4. Apply preset selection
	loadPreset(r);
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::doReset()
{
	ofLogNotice(__FUNCTION__);

	int sz = presetsRandomFactor.size();
	int div = sz / 4;

	for (size_t i = 0; i < sz; i++)
	{
		if (i < div + 1) {
			presetsRandomFactor[i] = 10;
			presetsRandomModeShort[i] = false;
		}
		else if (i < 3 * div) {
			presetsRandomFactor[i] = 5;
			presetsRandomModeShort[i] = false;
		}
		else {
			presetsRandomFactor[i] = 1;
			presetsRandomModeShort[i] = true;
		}
	}

	doDices();

	randomizeDurationShortRatio = 0.25;
	randomizeDurationBpm = 120;
	//durationShort = durationLong * randomizeDurationShortRatio;

	//--

	/*
	for (auto& p : presetsRandomFactor)
	{
		p = 0;
	}

	if (presetsRandomFactor.size() > 0)
	{
		presetsRandomFactor[0] = 1;
		dicesTotalAmount = 1;
		randomizedDice = 0;
	}
	else
	{
		dicesTotalAmount = 0;
	}

	randomizedDice.setMax(dicesTotalAmount - 1);

	durationLong = 1000;
	durationShort = durationLong * randomizeDurationShortRatio;

	// reset all to long
	for (auto& p : presetsRandomModeShort)
	{
		p.set(false);
	}
	*/
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::addGroupToRandomizerFiletered(ofParameterGroup& group) {
	// TODO:
	// if we want to make nested folders (not all toggles into same and only one level, we need to create subgroups too...)

	for (auto parameter : group)
	{
		// group
		auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
		if (parameterGroup)
		{
			// exclude params not marked as serializable
			if (!parameterGroup->isSerializable()) return;

			addGroupToRandomizerFiletered(*parameterGroup);
			continue;
		}

		// exclude params not marked as serializable
		if (!parameter->isSerializable()) return;

		//--

		{
			// parameter, try everything we know how to handle.

			// x,y,z vectors
#if OF_VERSION_MINOR >= 10
			auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
			if (parameterVec2f)
			{
				ofParameter<bool> b{ parameterVec2f->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}
			auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
			if (parameterVec3f)
			{
				ofParameter<bool> b{ parameterVec3f->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}
			auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
			if (parameterVec4f)
			{
				ofParameter<bool> b{ parameterVec4f->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}
#endif
			auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
			if (parameterOfVec2f)
			{
				//ofxImGuiSurfing::AddParameter(*parameterOfVec2f);
				ofParameter<bool> b{ parameterOfVec2f->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}
			auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
			if (parameterOfVec3f)
			{
				//ofxImGuiSurfing::AddParameter(*parameterOfVec3f);
				ofParameter<bool> b{ parameterOfVec3f->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}
			auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
			if (parameterOfVec4f)
			{
				ofParameter<bool> b{ parameterOfVec4f->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				//ofxImGuiSurfing::AddParameter(*parameterOfVec4f);
				continue;
			}
			// colors
			auto parameterColor = std::dynamic_pointer_cast<ofParameter<ofColor>>(parameter);
			if (parameterColor)
			{
				ofParameter<bool> b{ parameterColor->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}
			auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
			if (parameterFloatColor)
			{
				ofParameter<bool> b{ parameterFloatColor->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}
			// normal types
			auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
			if (parameterFloat)
			{
				ofParameter<bool> b{ parameterFloat->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				//ofxImGuiSurfing::AddParameter(*parameterFloat);
				continue;
			}
			auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
			if (parameterInt)
			{
				ofParameter<bool> b{ parameterInt->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}
			auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
			if (parameterBool)
			{
				ofParameter<bool> b{ parameterBool->getName(), false };
				randomizersFiltered_TogglesVector.push_back(b);
				continue;
			}

			ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter " << parameter->getName();
		}
	}
}

//TODO:
//--------------------------------------------------------------
void SurfingGroupRandomizer::doRandomPreset() {
	ofLogNotice(__FUNCTION__);

	//if(params_Filtered_Toggles.size() == 0) params_Filtered_Toggles

	//ofParameterGroup _group = group;
	//doRandomGroup(_group);

	doRandomGroup(group);

	//#ifdef INCLUDE_ofxUndoSimple
	//	if (MODE_Editor.get())
	//		doStoreUndo();// store scene
	//#endif
}

//TODO:
//make a simpler method to the local index, selected
//like .doRandomPreset();
//--------------------------------------------------------------
void SurfingGroupRandomizer::doRandomGroup(ofParameterGroup& group) {
	for (auto parameter : group)
	{
		if (parameter->isSerializable())// avoid not serializable params that will crash
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
				ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
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
void SurfingGroupRandomizer::doResetGroup(ofParameterGroup& group, bool bfull) 
{
	//TODO:
	//bfull is ignored
	//filtered are ignored
	// this features are now on ofxSurfingRandomizer

	for (auto parameter : group)
	{
		if (parameter->isSerializable()) // avoid not serializable params that will crash
		{
			// recursive..
			auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
			if (parameterGroup)
			{
				//TODO:
				//string name = parameterGroup->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);//TODO: crashes here..
				////bool b = parameterGroup->isSerializable();//another possible approach to filter..

				////auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
				////if (parameterBool)

				if (bfull) doResetGroup(*parameterGroup, bfull);
				//if (bfull || b) doResetGroup(*parameterGroup, bfull);
				continue;
			}

			auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
			if (parameterInt)
			{
				//string name = parameterInt->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterInt->set(parameterInt->getMin());
				parameterInt->set(parameterInt->getMin());
				continue;
			}

			auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
			if (parameterFloat)
			{
				string name = parameterFloat->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterFloat->set(parameterFloat->getMin());
				parameterFloat->set(parameterFloat->getMin());
				continue;
			}

			auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
			if (parameterBool)
			{
				//string name = parameterBool->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterBool->set(false);
				parameterBool->set(false);
				continue;
			}

			auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
			if (parameterFloatColor)
			{
				//string name = parameterFloatColor->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterFloatColor->set(ofFloatColor(0, 0, 0, 1));
				parameterFloatColor->set(ofFloatColor(0, 0, 0, 1));
				continue;
			}

			auto parameterColor = std::dynamic_pointer_cast<ofParameter<ofColor>>(parameter);
			if (parameterColor)
			{
				//string name = parameterColor->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterColor->set(ofColor(0, 0, 0, 255));
				parameterColor->set(ofColor(0, 0, 0, 255));
				continue;
			}

			//glm::vec compatible
#if OF_VERSION_MINOR >= 10
			auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
			if (parameterVec2f)
			{
				//string name = parameterVec2f->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterVec2f->set(glm::vec2(parameterVec2f->getMin().x, parameterVec2f->getMin().y));
				parameterVec2f->set(glm::vec2(parameterVec2f->getMin().x, parameterVec2f->getMin().y));
				continue;
			}
			auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
			if (parameterVec3f)
			{
				//string name = parameterVec3f->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b)  parameterVec3f->set(glm::vec3(
				parameterVec3f->set(glm::vec3(
					parameterVec3f->getMin().x,
					parameterVec3f->getMin().y,
					parameterVec3f->getMin().z));
				continue;
			}
			auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
			if (parameterVec4f)
			{
				//string name = parameterVec4f->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterVec4f->set(glm::vec4(
				parameterVec4f->set(glm::vec4(
					parameterVec4f->getMin().x,
					parameterVec4f->getMin().y,
					parameterVec4f->getMin().z,
					parameterVec4f->getMin().w));
				continue;
			}
#endif

			auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
			if (parameterOfVec2f)
			{
				//string name = parameterOfVec2f->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterOfVec2f->set(ofVec2f(parameterOfVec2f->getMin().x, parameterOfVec2f->getMin().y));
				parameterOfVec2f->set(ofVec2f(parameterOfVec2f->getMin().x, parameterOfVec2f->getMin().y));
				continue;
			}
			auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
			if (parameterOfVec3f)
			{
				//string name = parameterOfVec3f->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterOfVec3f->set(ofVec3f(parameterOfVec3f->getMin().x, parameterOfVec3f->getMin().y, parameterOfVec3f->getMin().z));
				parameterOfVec3f->set(ofVec3f(parameterOfVec3f->getMin().x, parameterOfVec3f->getMin().y, parameterOfVec3f->getMin().z));
				continue;
			}
			auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
			if (parameterOfVec4f)
			{
				//string name = parameterOfVec4f->getName();
				//ofParameter<bool> b = params_Filtered_Toggles.getBool(name);
				//if (bfull || b) parameterOfVec4f->set(ofVec4f(
				parameterOfVec4f->set(ofVec4f(
					parameterOfVec4f->getMin().x,
					parameterOfVec4f->getMin().y,
					parameterOfVec4f->getMin().z,
					parameterOfVec4f->getMin().w));
				continue;
			}

			ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter: " << parameter->getName();
		}
	}
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::setup_RandomizerFiletered()
{
	params_randomizer.setName("RANDOMIZERS");
	randomizersFiltered_TogglesVector.clear();
	addGroupToRandomizerFiletered(group); // Enqueue all content params and create a toggle for each one
	// This is to allow filter (by enabling toggles) each param or not.

	// Add to group
	bRandomizeFiltered.set("RANDOM PARAMS", false);
	bRandomizeFiltered_PopulateFavs.set("POPULATE FAVS!", false);
	bRandomizeFiltered_All.set("ALL", false);
	bRandomizeFiltered_None.set("NONE", false);

	params_RandomizersFiltered.clear();
	params_RandomizersFiltered.setName("RANDOM FILTER CONTROL");
	params_RandomizersFiltered.add(bRandomizeFiltered);
	params_RandomizersFiltered.add(bRandomizeFiltered_All);
	params_RandomizersFiltered.add(bRandomizeFiltered_None);
	//params_RandomizersFiltered.add(bRandomizeFiltered_PopulateFavs);// TODO: not implemented

	params_Filtered_Toggles.setName("PRESET PARAMETERS");
	params_Filtered_Toggles.clear();
	for (auto& p : randomizersFiltered_TogglesVector)
	{
		params_Filtered_Toggles.add(p);
	}
	params_RandomizersFiltered.add(params_Filtered_Toggles);

	// Exclude
	bRandomizeFiltered.setSerializable(false);
	bRandomizeFiltered_PopulateFavs.setSerializable(false);
	bRandomizeFiltered_All.setSerializable(false);
	bRandomizeFiltered_None.setSerializable(false);

	//--

	// Callback
	ofAddListener(params_RandomizersFiltered.parameterChangedE(), this, &SurfingGroupRandomizer::Changed_Editor);

	//--

	////TODO:
	//setup_RandomizerPowered();
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::buildRandomizers()
{
	// Radomizer
	setup_RandomizerIndexes();

	// Preset editor tools
	setup_RandomizerFiletered();
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::exit()
{
	// Save randomizers settings
	ofxSurfingHelpers::saveGroup(params_RandomizerSettings, path_RandomizerSettings);

	ofRemoveListener(params_Control.parameterChangedE(), this, &SurfingGroupRandomizer::Changed_Control);
	ofRemoveListener(params_Randomizer.parameterChangedE(), this, &SurfingGroupRandomizer::Changed_Control);
	ofRemoveListener(params_RandomizersFiltered.parameterChangedE(), this, &SurfingGroupRandomizer::Changed_Editor);
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::update()
{
	//----

	/*
	//TODO:
	// Randomizer timer MODE LATCH

	// TODO:
	// On this mode: when we click to some presets, we load the preset, but after duration,
	// We jump back to preset index 0
	// Easy callback
	//// Latch mode
	//if (MODE_LatchTrig)
	//	if (bIsDoneLoad && !bPLAY_RandomizeTimer)
	//	{
	//		bIsDoneLoad = false;
	//		timerRandomLast = ofGetElapsedTimeMillis();
	//		if (index_PresetSelected != 0)
	//		{
	//			bLatchRun = true;
	//		}
	//		else
	//		{
	//			bLatchRun = false;
	//		}
	//	}

	//if (MODE_LatchTrig) // latch mode trigs the preset and then trigs back to first presets
	//{
	//	if (bLatchRun) loadPreset(0);
	//}
	//else
	//{
	//TODO: can be improved calling directly the method! bc this flag will be readed on update()..
	//bRandomizeIndex = true;
	//doGoRandomIndex();
	//}

	//if (bPLAY_RandomizeTimer || MODE_LatchTrig) // ?
	*/

	//----

	if (bPLAY_RandomizeTimer)
	{
		uint32_t _time = ofGetElapsedTimeMillis();
		timerRandom = _time - timerRandomLast; // elapsed now from last trig

		// Avoid out of range
		if (index_PresetSelected < presetsRandomModeShort.size())
		{
			//--

			// A. Long mode
			// get if it's marked as short or long. by default false is long.
			if (presetsRandomModeShort[index_PresetSelected] == false)
			{
				timerPlayerPct = ofMap(timerRandom, 0, durationLong, 0, 1, true);

				if (timerRandom >= durationLong)
				{
					if (bModeSequencial) {
						doGoNextIndex();
					}
					else
					{
						doGoRandomIndex();
					}
				}
			}

			//--

			// B. Short mode
			else
			{
				timerPlayerPct = ofMap(timerRandom, 0, durationShort, 0, 1, true);

				if (timerRandom >= durationShort)
				{
					if (bModeSequencial) {
						doGoNextIndex();
					}
					else
					{
						doGoRandomIndex();
					}
				}
			}
		}
	}
	else
	{
		timerPlayerPct = 0;
	}
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::setup_RandomizerIndexes()
{
	bPLAY_RandomizeTimer.set("PLAY", false);
	bModeSequencial.set("MODE SEQUENCIAL", false);
	bRandomizeIndex.set("GO RANDOM", false);
	MODE_AvoidRandomRepeat.set("No Repeat", true);
	durationLong.set("t DURATION", 1000, 10, randomize_MAX_DURATION);
	randomizeDurationShortRatio.set("t RATIO", 0.25, 0.005, 1);
	durationShort.set("t SHORT", 250, 10, randomize_MAX_DURATION); // locked
	randomizeDurationBpm.set("t BPM", 120, 10, 400);
	randomizedDice.set("DICE", 0, 0, amountPresets - 1);
	bResetDices.set("RESET", false);
	//MODE_LatchTrig.set("LATCH", false);

	// exclude
	//bPLAY_RandomizeTimer.setSerializable(false);
	durationShort.setSerializable(false); // locked to bpm and ratio
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
	params_PresetsProbs.clear();
	i = 0;
	for (auto& p : presetsRandomFactor) {
		string n = "PROB " + ofToString(i++);
		p.set(n, 5, 0, 10);
		params_PresetsProbs.add(p);
	}

	// toggles to enable short duration mode
	params_PresetsDurations.clear();
	i = 0;
	for (auto& p : presetsRandomModeShort) {
		string n = "SHORT " + ofToString(i++);
		p.set(n, false);
		params_PresetsDurations.add(p);
	}

	params_Randomizer.clear();
	params_Randomizer.setName("SETTINGS");
	params_Randomizer.add(bPLAY_RandomizeTimer);
	params_Randomizer.add(bModeSequencial);
	params_Randomizer.add(MODE_AvoidRandomRepeat);
	params_Randomizer.add(bRandomizeIndex);
	params_Randomizer.add(bResetDices);
	//params_Randomizer.add(MODE_LatchTrig);

	params_Timer.clear();
	params_Timer.setName("TIMERS");
	params_Timer.add(randomizeDurationBpm);
	params_Timer.add(durationLong);
	params_Timer.add(randomizeDurationShortRatio);
	params_Timer.add(durationShort); // locked
	params_Randomizer.add(params_Timer);

	params_Randomizer.add(params_PresetsProbs); // probs
	params_Randomizer.add(params_PresetsDurations); // toggles

#ifdef DEBUG_randomTest
	params_Randomizer.add(randomizedDice);
#endif
}

////----------------------------------------------------------------
//void SurfingGroupRandomizer::keyPressed(int key)
//{
//
//}

//--------------------------------------------------------------
void SurfingGroupRandomizer::drawImGui_Editor()
{
	if (!bGui_PlayerEditor) return;

	static bool auto_resize = true;
	ImGuiWindowFlags flagsw;
	flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

	string n = "EDITOR " + group.getName();

	//--

	IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_SMALL;

#ifdef USE_GUI_MANAGER__GROUP_RANDOMIZER
	guiManager.beginWindow(n.c_str(), NULL, flagsw);
#endif

#ifdef USE_RAW_IM_GUI__GROUP_RANDOMIZER
	ImGui::Begin(n.c_str(), NULL, flagsw);
#endif

	{
		float _h = 2 * ofxImGuiSurfing::getWidgetsHeight();
		float _h2 = 0.5f * _h;
		float _w1 = ofxImGuiSurfing::getWidgetsWidth(1);
		float _w2 = ofxImGuiSurfing::getWidgetsWidth(2);

		ofxImGuiSurfing::AddBigToggleNamed(bModeSequencial, ImVec2(_w1, _h),
			"MODE SEQUENCIAL", "MODE RANDOM");

		if (!bModeSequencial)
			ofxImGuiSurfing::AddBigButton(bRandomizeIndex, ImVec2(_w1, _h2));
		else
			if (ImGui::Button("GO NEXT", ImVec2(_w1, _h2)))
				doGoNextIndex();

		if (!bModeSequencial) ofxImGuiSurfing::AddParameter(MODE_AvoidRandomRepeat);

		ofxImGuiSurfing::AddSpacingSeparated();

		// Groups

		// Timers
		//ofxImGuiSurfing::AddGroup(params_Timer, flags); 

		if (ofxImGuiSurfing::BeginTree("CLOCK TIMERS"))
		{
			ofxImGuiSurfing::AddParameter(randomizeDurationBpm);
			ofxImGuiSurfing::AddParameter(durationLong);

			if (!getAllAreLong())
			{
				ofxImGuiSurfing::AddParameter(durationShort); // locked
				ofxImGuiSurfing::AddParameter(randomizeDurationShortRatio);

				static float _div[]{ 1.0f, 0.5f, 1 / 3.f, 1 / 4.f , 1 / 8.f };
				static int _idiv = 0;
				if (ofxImGuiSurfing::AddButtonMini("BPM Stepped"))
				{
					_idiv++;
					if (_idiv == 5)_idiv = 0;
					randomizeDurationShortRatio = _div[_idiv];
				}
			}

			ofxImGuiSurfing::EndTree();
		}


		ofxImGuiSurfing::AddSpacingSeparated();

		// Probabilities
		if (!bModeSequencial)
		{
			ofxImGuiSurfing::AddGroup(params_PresetsProbs, flags);
			ofxImGuiSurfing::AddSpacingSeparated();
		}

		// Duration toggles
		ofxImGuiSurfing::AddGroup(params_PresetsDurations, flags);
		ofxImGuiSurfing::AddSpacingSeparated();

		// Reset
		ofxImGuiSurfing::AddBigButton(bResetDices, ImVec2(_w1, _h2));

		//--

#ifdef DEBUG_randomTest
		ImGui::Text("%d/%d", randomizedDice.get(), randomizedDice.getMax());
#endif
	}

#ifdef USE_GUI_MANAGER__GROUP_RANDOMIZER
	guiManager.endWindow();
#endif

#ifdef USE_RAW_IM_GUI__GROUP_RANDOMIZER
	ImGui::End();
#endif
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::drawImGui()
{
	drawImGui_Main();
	drawImGui_Editor();
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::drawImGui_Main()
{
	string str = "PLAYER " + group.getName();

	static bool auto_resize = true;
	ImGuiWindowFlags flagsw;
	flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;

	if (bLockedPosition)
	{
		ImGuiCond cond = ImGuiCond_Always;
		ImGui::SetNextWindowPos(positionGui, cond);
	}

	//--
	
	//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW;
	// weird resizes..

	//workaround
	ImVec2 size_min = ImVec2(150, -1);
	ImVec2 size_max = ImVec2(150, -1);
	ImGui::SetNextWindowSizeConstraints(size_min, size_max);

	//--
	
	// Window
	{
#ifdef USE_GUI_MANAGER__GROUP_RANDOMIZER
		guiManager.beginWindow(str.c_str(), NULL, flagsw);
#endif

#ifdef USE_RAW_IM_GUI__GROUP_RANDOMIZER
		ImGui::Begin(str.c_str(), NULL, flagsw);
#endif
	}

	//--

	{
		float _spcx;
		float _spcy;
		float _w100;
		float _h100;
		float _w99;
		float _w50;
		float _w33;
		float _w25;
		float _h;
		ofxImGuiSurfing::refreshImGui_WidgetsSizes(_spcx, _spcy, _w100, _h100, _w99, _w50, _w33, _w25, _h);

		//---

		// Preset Selector

		ofxImGuiSurfing::AddToggleRoundedButton(bMinimize);
		ImGui::Spacing();

		//-

		// Editor

		ofxImGuiSurfing::AddToggleRoundedButton(bGui_PlayerEditor, ImVec2(2 * _h, 2 * (2 / 3.f) * _h));

		ImGui::Spacing();

		//--

		ImGui::Separator();
		ImGui::Spacing();

		//--

		// 1. Presets Randomizers

		//--

		// 1.0.1 Play randomizer index

		// Blink by timer

		ofxImGuiSurfing::AddBigToggleNamed(bPLAY_RandomizeTimer, _w100, 2 * _h,
			"PLAYING", "PLAY", true, getPlayerPct());

		ImGui::Spacing();

		//--

		// 1.0.2 Draw progress bar for the randomizer timer
		{
			ImGuiStyle* style = &ImGui::GetStyle();

			ImGui::PushID("prog");
			const ImVec4 color = style->Colors[ImGuiCol_ButtonHovered];
			//we can force change this color on theme... only used here
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 0));//transparent
			ImGui::ProgressBar(timerPlayerPct);
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopID();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		//--

		if (!bMinimize)
		{
			ImGui::Spacing();

			// label
			str = "PRESET";
			//str = "Preset " + ofToString(index_PresetSelected.get());
			ImGui::Text(str.c_str());
			ImGui::Spacing();

			// index
			ImGui::PushItemWidth(_w33);
			ofxImGuiSurfing::AddParameter(index_PresetSelected);
			ImGui::PopItemWidth();

			ImGui::Spacing();
		}

		//--

		_w100 = getWidgetsWidth(1);
		_w50 = getWidgetsWidth(2);
		float _hm = getWidgetsHeightUnit() * 2;

		if (keys.size() == 0)
			ofxImGuiSurfing::AddMatrixClicker(index_PresetSelected, respBtnsClicker, amntBtnsClicker, true, _hm);
		else
			ofxImGuiSurfing::AddMatrixClickerLabelsStrings(index_PresetSelected, labels, true, amntBtnsClicker, true, -1);

		ImGui::Spacing();

		//--

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		if (!bMinimize)
		{
			ImGui::Text("CLOCK");
			ImGui::Spacing();
		}

		{
			ImGui::PushItemWidth(_w50);
			ofxImGuiSurfing::AddParameter(randomizeDurationBpm);
			ImGui::PopItemWidth();

			// Add mouse wheel to the float param
			{
				float wheel = ImGui::GetIO().MouseWheel;
				bool bCtrl = ImGui::GetIO().KeyCtrl; // ctrl to fine tunning
				{
					ofParameter<float> p = dynamic_cast<ofParameter<float>&>(randomizeDurationBpm);
					float resolution = -1;
					resolution = (p.getMax() - p.getMin()) / 800.f;//make smaller
					//resolution = (p.getMax() - p.getMin()) / 100.f;
					// 100 steps for all the param range

					p += wheel * (bCtrl ? resolution : resolution * 10);
					p = ofClamp(p, p.getMin(), p.getMax()); // clamp
				}
			}

			if (ImGui::Button("Half", ImVec2(_w50, _h))) {
				randomizeDurationBpm = randomizeDurationBpm / 2.0f;
			}
			ImGui::SameLine();
			if (ImGui::Button("Double", ImVec2(_w50, _h))) {
				randomizeDurationBpm = randomizeDurationBpm * 2.0f;
			}
			if (ImGui::Button("Reset", ImVec2(_w100, _h)))
			{
				randomizeDurationBpm = 120;
			}
		}
		if (!bMinimize)
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}

		//--

		/*
		if (MODE_Editor)
		{
			// 1.0.3 bang randomize

			ImGui::Spacing();

			// random index
			ofxSurfingHelpers::AddBigButton(bRandomizeIndex, _w99, _h * 2);

			// random filter
			ofxSurfingHelpers::AddBigButton(bRandomizeFiltered, _w99, _h * 2);

			//ofxSurfingHelpers::AddBigButton(bRandomizeFiltered, 2 * _h);
			//ofxImGuiSurfing::AddParameter(bRandomizeFiltered);// trig random current preset: will randomize all enabled toggle parameters
		}
		*/

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

		/*
		// workflow
		//if (MODE_Editor)
		{
			ImGui::Spacing();

			// 1.0 edit folder

			if (ImGui::CollapsingHeader("EDIT"))
			{
				// 1.1 randomizer index presets

				//drawEditor();
				ofxImGui::AddGroup(params_Randomizer, settings);

				//--

				// 1.2 randomizers filter editor

				//drawImGui_RandomizerParams();
				ofxImGui::AddGroup(params_RandomizersFiltered, settings);
			}
		}
		*/

		//--

		// Extra

		if (!bMinimize)
		{
			ofxImGuiSurfing::AddToggleRoundedButton(bExtraClicker);
			if (bExtraClicker)
			{
				ImGui::Indent();
				{
					//ofxImGuiSurfing::AddToggleRoundedButton(respBtnsClicker);
					//if (respBtnsClicker)
					{
						ofxImGuiSurfing::AddStepperInt(amntBtnsClicker);
					}
				}
				ImGui::Unindent();
			}
		}
	}

	//--

	// Window

	{
#ifdef USE_GUI_MANAGER__GROUP_RANDOMIZER
		guiManager.endWindow();
#endif

#ifdef USE_RAW_IM_GUI__GROUP_RANDOMIZER
		ImGui::End();
#endif
	}

	//--
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::Changed_Editor(ofAbstractParameter& e)
{
	if (!bDISABLE_CALLBACKS)
	{
		string name = e.getName();

		ofLogNotice(__FUNCTION__) << group.getName() << " " << name << " : " << e;

		//-

		if (name == bRandomizeFiltered.getName() && bRandomizeFiltered)// trig randomize
		{
			bRandomizeFiltered = false;

			doRandomPreset();
		}
		else if (name == bRandomizeFiltered_All.getName() && bRandomizeFiltered_All)// all
		{
			bRandomizeFiltered_All = false;

			for (auto& p : randomizersFiltered_TogglesVector) {
				p.set(true);
			}
		}
		else if (name == bRandomizeFiltered_None.getName() && bRandomizeFiltered_None)// none
		{
			bRandomizeFiltered_None = false;

			for (auto& p : randomizersFiltered_TogglesVector) {
				p.set(false);
			}
		}
		else if (name == bRandomizeFiltered_PopulateFavs.getName() && bRandomizeFiltered_PopulateFavs)// populate random all favs
		{
			bRandomizeFiltered_PopulateFavs = false;
			//doPopulateFavs();
		}
	}
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::doDices()// calculate all probabilities for all presets
{
	// sum total dices/all probs
	dicesTotalAmount = 0;
	for (auto& p : presetsRandomFactor) {
		dicesTotalAmount += p.get();
	}
	randomizedDice.setMax(dicesTotalAmount - 1);

	ofLogNotice(__FUNCTION__) << group.getName() << " dicesTotalAmount: " << dicesTotalAmount;
}

//--------------------------------------------------------------
void SurfingGroupRandomizer::Changed_Control(ofAbstractParameter& e)
{
	return;

	if (!bDISABLE_CALLBACKS)
	{
		string name = e.getName();

		//if ((name != "exclude") //&&
		//	//(name != "DICE") &&
		//	//(name != "PRESET")
		//	)
		{
			ofLogNotice(__FUNCTION__) << "name: " << group.getName() << " " << name << ": " << e;
		}

		if (0) {}

		//----

		// index preset selector
		/*
		else if (name == index_PresetSelected.getName())
		{
			ofLogNotice(__FUNCTION__) << group.getName() << " index: " << index_PresetSelected.get();

			// TODO:
			//selectorTARGET = index_PresetSelected;
		}
		*/

		//--

		/*
		// helper tools
		else if (name == "CLONE >" && bCloneRight)
		{
			ofLogNotice(__FUNCTION__) << group.getName() << "CLONE >: " << e;
			//bCloneRight = false;
			//doCloneRight(index_PresetSelected);
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
		*/

		//--

		// randomizer
		else if (name == bRandomizeIndex.getName() && bRandomizeIndex)
		{
			ofLogNotice(__FUNCTION__) << group.getName() << "RANDOMIZE !";

			bRandomizeIndex = false;
			doGoRandomIndex();
		}

		// play randomizer
		else if (name == bPLAY_RandomizeTimer.getName())
		{
			ofLogNotice(__FUNCTION__) << group.getName() << "MODE TIMER: " << e;

			if (bPLAY_RandomizeTimer)
			{
				//MODE_LatchTrig = false;

				// TODO: new test
				timerRandomLast = ofGetElapsedTimeMillis();
			}
		}

		//// latch
		//else if (name == MODE_LatchTrig.getName())
		//{
		//	ofLogNotice(__FUNCTION__) << "MODE LATCH: " << e;
		//	if (MODE_LatchTrig) 
		//	{
		//		bPLAY_RandomizeTimer = false;	
		//		
		//		// TODO: new test
		//		timerRandomLast = ofGetElapsedTimeMillis();
		//	}
		//}

		// durations
		else if (name == durationLong.getName())
		{
			ofLogNotice(__FUNCTION__) << "DURATION: " << e;

			randomizeDurationBpm = (MAX_DURATION_RATIO * 60000.f) / durationLong;
		}
		else if (name == randomizeDurationBpm.getName())
		{
			ofLogNotice(__FUNCTION__) << "BPM: " << e;

			// 60,000 ms (1 minute) / Tempo (BPM) = Delay Time in ms for quarter-note beats
			durationLong = (MAX_DURATION_RATIO * 60000.f) / randomizeDurationBpm;
			durationShort = durationLong * randomizeDurationShortRatio;
		}
		else if (name == randomizeDurationShortRatio.getName())
		{
			ofLogNotice(__FUNCTION__) << "Short ratio: " << e;
			durationShort = durationLong * randomizeDurationShortRatio;
		}
		else if (name == durationShort.getName())
		{
			ofLogNotice(__FUNCTION__) << "Lock Short ratio: " << e;
			durationShort = durationLong * randomizeDurationShortRatio;
		}

#ifdef DEBUG_randomTest
		else if (name == "DICE")// when debug enabled: set dice by user to test
		{
			ofLogNotice(__FUNCTION__) << "DICE: " << e;
			doGoRandomIndex();
		}
#endif
		else if (name == bResetDices.getName() && bResetDices)
		{
			bResetDices = false;
			doReset();
		}

		//--

		// workaround
		// all other widgets/params that are populate on the the fly, not hardcoded
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
void SurfingGroupRandomizer::loadPreset(int p)
{
	ofLogNotice(__FUNCTION__) << group.getName() << " : " << p;
	index_PresetSelected = p;

	//TODO: 
	// workaround
	// bc the done-loading happens out of the class...
	bIsDoneLoad = true;

	// TODO:
	////if (selectorTARGET) 
	//{
	//selectorTARGET = index_PresetSelected;
	//}
}

////--------------------------------------------------------------
//void SurfingGroupRandomizer::doCheckPresetsFoldersAreEmpty()
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