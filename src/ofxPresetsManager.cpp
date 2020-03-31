#include "ofxPresetsManager.h"

#pragma mark - OF

//--------------------------------------------------------------
ofxPresetsManager::ofxPresetsManager()
{
	//subscribed to auto run update and draw without required 'manual calls'
	ofAddListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofAddListener(ofEvents().draw, this, &ofxPresetsManager::draw);

	//-

	DISABLE_CALLBACKS = true;

	//-

#ifdef TIME_SAMPLE_MEASURES
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
	//default archive presets folder to browse

	//app settings
	path_Control = "settings";


	//default preset
	PRESET_name = "NO_NAME_PRESET";

	//to add to all presets filenames
	path_Prefix = "_preset_";

	//--

	modeKey = OF_KEY_CONTROL;
	bKeys = false;
	keysNotActivated = true;
	bKeySave = false;

	lastMouseButtonState = false;
	//bDelayedLoading = false;

	//--

	//this multidimension is for multiple gui/groups
	groups.reserve(NUM_MAX_GUIS);
	lastIndices.reserve(NUM_MAX_GUIS);
	keys.reserve(NUM_MAX_GUIS);

	//--

	//TODO: easy callback to ofApp integration
	DONE_load.set("DONE LOAD", false);
	DONE_save.set("DONE SAVE", false);

	//TODO:
	//can use too isDoneLoad() to check in update() as callback

	//-

	////gui font
	//myTTF = path_GLOBAL_Folder + "assets/fonts/overpass-mono-bold.otf";
	////myTTF = "assets/fonts/PragmataProR_0822.ttf";
	//sizeTTF = 8;
	//myFont.load(myTTF, sizeTTF, true, true);

	//-

	//control parameters

	PRESET_selected.set("PRESETS", 1, 1, num_presets);
	//PRESET2_selected.set("PRESETS2", 1, 1, num_presets);

	bSave.set("SAVE", false);
	MODE_MemoryLive.set("MODE MEMORY", false);
	loadToMemory.set("LOAD TO MEMORY", false);
	saveFromMemory.set("SAVE FROM MEMORY", false);
	autoLoad.set("AUTO LOAD", true);
	autoSave.set("AUTO SAVE", true);
	bAutosaveTimer.set("TIMER AUTO SAVE", false);
	bCloneRight.set("CLONE >", false);
	bCloneRight.setSerializable(false);
	SHOW_menu.set("SHOW MENU", false);
	SHOW_Gui_Internal.set("SHOW CONTROL GUI ", false);
	SHOW_ClickPanel.set("SHOW CLICK PANEL", false);
	ENABLE_Keys.set("ENABLE KEYS", true);
	Gui_Position.set("GUI POSITION",
		glm::vec2(ofGetWidth() * 0.5, ofGetHeight()* 0.5),
		glm::vec2(0, 0),
		glm::vec2(ofGetWidth(), ofGetHeight())
	);

	//-

	//exclude from xml settings
	bSave.setSerializable(false);
	loadToMemory.setSerializable(false);
	saveFromMemory.setSerializable(false);
	SHOW_Gui_Internal.setSerializable(false);

	//-

	//groups

	params_Favorites.setName("USER");
	params_Favorites.add(PRESET_selected);
	//params_Favorites.add(PRESET2_selected);
	params_Favorites.add(bSave);

	params_Options.setName("OPTIONS");
	params_Options.add(MODE_MemoryLive);
	params_Options.add(loadToMemory);
	params_Options.add(saveFromMemory);
	params_Options.add(autoLoad);
	params_Options.add(autoSave);
	params_Options.add(bAutosaveTimer);

	params_Gui.setName("GUI");
	params_Gui.add(SHOW_Gui_Internal);
	params_Gui.add(SHOW_ClickPanel);
	params_Gui.add(SHOW_menu);
	params_Gui.add(ENABLE_Keys);
	params_Gui.add(Gui_Position);

	params_Tools.setName("HELPER TOOLS");
	params_Tools.add(bCloneRight);

	//all nested params for callbacks and storage settings
	params_Control.setName("ofxPresetsManager");
	params_Control.add(params_Favorites);
	params_Control.add(params_Options);
	params_Control.add(params_Gui);
	params_Control.add(params_Tools);

	ofAddListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);
}

//--------------------------------------------------------------
void ofxPresetsManager::setPositionDEBUG(int x, int y)
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
	DISABLE_CALLBACKS = true;

	//-

	//ofSetLogLevel("ofxPresetsManager", OF_LOG_NOTICE);
	ofLogNotice("ofxPresetsManager") << "setup()";

	//-

	//gui font
	myTTF = path_GLOBAL_Folder + "/" + "fonts/overpass-mono-bold.otf";
	sizeTTF = 10;
	bool bLoaded = myFont.load(myTTF, sizeTTF, true, true);

#ifdef INCLUDE_DEBUG_ERRORS
	if (!bLoaded)
	{
		errorsDEBUG.addError(gui_LabelName + " ofxPresetsManager", "setup() myFont", myTTF);
	}
#endif

	//-

	//ofxGui theme

	string pathFont = path_GLOBAL_Folder + "/" + "fonts/overpass-mono-bold.otf";
	//must check this font file is found there
	ofFile file(pathFont);
	if (file.exists())
	{
		ofxGuiSetFont(pathFont, 9);
	}
#ifdef INCLUDE_DEBUG_ERRORS
	else
	{
		errorsDEBUG.addError(gui_LabelName + " ofxPresetsManager", "setup() ofxGui", pathFont);
	}
#endif
	//colors
	ofxGuiSetDefaultHeight(20);
	ofxGuiSetBorderColor(32);
	ofxGuiSetFillColor(ofColor(48));
	ofxGuiSetTextColor(ofColor::white);
	ofxGuiSetHeaderColor(ofColor(24));

	//control gui
	guiControl.setup(gui_LabelName);
	guiControl.add(params_Control);
	guiControl.setPosition(ofGetWidth() - 210, 10);//default

	//collapse
	//guiControl.getGroup("ofxPresetsManager").minimize();
	auto &gPanel = guiControl.getGroup("ofxPresetsManager");
	auto &gGui = gPanel.getGroup("GUI");
	gGui.minimize();
	auto &gGuiPos = gGui.getGroup("GUI POSITION");
	gGuiPos.minimize();
	auto &gOptions = gPanel.getGroup("OPTIONS");
	gOptions.minimize();
	gPanel.getGroup("HELPER TOOLS").minimize();

	//--

	//browser
	//gui_Visible = true;
	//gui.setup();
	//gui_imGui_theme();

	//--

	//timer auto save
	timerLast_Autosave = ofGetElapsedTimeMillis();

	//--

	////memory mode
	//load_AllKit_ToMemory();

	//-------

	//STARTUP

	//app settings
	DISABLE_CALLBACKS = false;
	load_ControlSettings();

	//TODO:
	//PRESET_selected_PRE = -1;

	set_GUI_Internal_Visible(false);

	//--------

	////TODO
	////moved from add
	////TODO: bug on mixerBlend.. in load_AllKit_ToMemory...
	////gui_LabelName = groups[0].getName();//TODO: one group only

	////memory mode
	//load_AllKit_ToMemory();

}

//--------------------------------------------------------------
void ofxPresetsManager::update(ofEventArgs & args)
{
	//plotters
	//TS_START("load1");
	//TS_STOP("load1");
	//TS_START("load2");
	//TS_STOP("load2");
	//TS_START("loadMem");
	//TS_STOP("loadMem");

	//--

	//autosave
	if (autoSave && bAutosaveTimer && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
	{
		//app settings
		save_ControlSettings();

		//save current preset
		doSave(PRESET_selected - 1);
		//doSave2(PRESET2_selected - 1);

		if (!MODE_MemoryLive)
		{
			//MODE A: from hd file
		}
		else
		{
			//MODE B: direct from memory
			save_AllKit_FromMemory();
		}

		//-

		//DISABLE_Callbacks = true;
		////get gui position before save
		//Gui_Position = glm::vec2(gui.getPosition());
		//saveParams(params_Control, path_GLOBAL + path_Params);
		//DISABLE_Callbacks = false;

		//-

		//auto save timer
		timerLast_Autosave = ofGetElapsedTimeMillis();
		ofLogNotice("ofxPresetsManager") << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t[AUTOSAVE]";
	}
}

//---------------------------------------------------------------------
void ofxPresetsManager::draw(ofEventArgs & args)
{

#ifdef INCLUDE_DEBUG_ERRORS
	//debug errors
	errorsDEBUG.draw();
#endif

	//-

	//TODO

	////Gui
	//bMouseOver_Changed = false;
	//bool gui_MouseOver_PRE = gui_MouseOver;
	//gui_MouseOver = false;
	//
	////TODO
	//gui_Visible = true;
	//
	//if (gui_Visible)
	//{
	//   gui_MouseOver = gui_draw();
	//   //ofLogNotice("ofxPresetsManager") << "gui_MouseOver:" << (gui_MouseOver?"IN":"OUT");
	//}
	//if (gui_MouseOver)
	//{
	//
	//}
	//else
	//{
	//
	//}
	//
	////TODO
	//if (gui_MouseOver != gui_MouseOver_PRE)
	//{
	//   bMouseOver_Changed = true;
	//}

	////debug
	//if (bMouseOver_Changed)
	//{
	//   if (gui_MouseOver)
	//   {
	//       ofLogNotice("ofxPresetsManager") << "MOUSE OVER GUI";
	//   }
	//   else
	//   {
	//       ofLogNotice("ofxPresetsManager") << "MOUSE OUT OF GUI";
	//   }
	//}

	//-

	if (SHOW_ClickPanel)
	{
		draw_CLICKER();
	}

	//-

	//TODO
	////groupDebug
	//{
	//   ofParameterGroup g;
	//   if (groups.size() > 0)
	//   {
	//       g = groups[0];
	//       groupDebug(g);
	//   }
	//}

	//-

	if (SHOW_Gui_Internal)
	{
		guiControl.draw();
	}

	//--
}


//--------------------------------------------------------------
void ofxPresetsManager::draw_CLICKER()
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

	//-

	//0. box of all boxes background
	ofFill();
	ofSetColor(0, 128);
	ofDrawRectangle(0, 0,
		clicker_cellSize * (keys[0].size() + 1), clicker_cellSize*groups.size());

	//-

	ofNoFill();
	ofSetColor(ofColor::white);

	for (size_t i = 0; i < keys.size(); ++i)//draw all guis/groups
	{
		//draw any preset box
		size_t k = 0;
		for (; k < keys[i].size(); ++k)
		{
			//1.1 outbox border container
			ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, clicker_cellSize, clicker_cellSize);

			//-

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

			//-

			//2. inner box. double mark current selected preset
			if (lastIndices[i] == k)
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

		//-

		//?
		//for (; k < presets[i]; ++k)
		//{
		//	ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, 
		//		clicker_cellSize, clicker_cellSize);

		//	if (lastIndices[i] == k)
		//		ofDrawRectangle(clicker_cellSize * k + 4, clicker_cellSize * i + 4, clicker_cellSize - 8, clicker_cellSize - 8);
		//}

		//-

		//3. save button

		//box
		ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, clicker_cellSize, clicker_cellSize);

		//-

		//save label
		int ySave = clicker_cellSize * i + 0.5 * clicker_cellSize + 0.5 * sizeTTF;
		if (!myFont.isLoaded())//without ttf font
		{
			ofDrawBitmapString("SAVE",
				clicker_cellSize*k + 8,
				clicker_cellSize*i + 18);
		}
		else//custom font 
		{
			float wx = 0.5f * myFont.getStringBoundingBox("SAVE", 0, 0).width;
			myFont.drawString("SAVE",
				clicker_cellSize * k + 0.5 * clicker_cellSize - wx,
				//clicker_cellSize * k + 0.5 * clicker_cellSize - 1.8f * sizeTTF,
				ySave);
		}
		k++;//?

		//-

		//group kit name
		if (SHOW_GroupName)
		{
			string info = groups[i].getName();

			//double font to improve different background colors
			int gap = 1;
			//int xG = clicker_cellSize * k + 15;

			float strW = myFont.getStringBoundingBox(info, 0, 0).width;
			int xG = -strW - 20;
			ySave = ySave - 2;//little up

			ofSetColor(ofColor::black);//shadow
			if (myFont.isLoaded())
				myFont.drawString(info, xG + gap, ySave + gap);
			else
				ofDrawBitmapString(info, xG + gap, ySave + gap);

			ofSetColor(ofColor::white);//white
			if (myFont.isLoaded())
				myFont.drawString(info, xG, ySave);
			else
				ofDrawBitmapString(info, xG, ySave);
		}

		//-

		bool bLateralPosition = true;

		//4. help info text
		if (debugClicker && ENABLE_Keys)
		{
			string info = "";
			bool bSimpleInfo = true;
			if (bSimpleInfo)
			{
				// keys[i][k]
				info += "[" + ofToString((char)keys[0][0]) + "|";
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

			if (!bLateralPosition)//vertical position below boxes
			{
				x = 0;
				y = (clicker_cellSize + 15) * groups.size();
			}
			else//lateral position right to the boxes
			{
				x = clicker_cellSize * k + pad;
				y = ySave - (bSimpleInfo ? -2 : sizeTTF);
			}

			//double font to improve different background colors
			ofSetColor(ofColor::black);//shadow
			if (myFont.isLoaded())
				myFont.drawString(info, x + gap, y + gap);
			else
				ofDrawBitmapString(info, x + gap, y + gap);

			ofSetColor(ofColor::white);//white
			if (myFont.isLoaded())
				myFont.drawString(info, x, y);
			else
				ofDrawBitmapString(info, x, y);
		}

		ofPopStyle();
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
ofxPresetsManager::~ofxPresetsManager()
{
	ofLogVerbose("ofxPresetsManager") << "~ofxPresetsManager";
	DISABLE_CALLBACKS = true;//?

	//autosave PRESET_selected preset on exit
	if (autoSave)
	{
		doSave(PRESET_selected - 1);
		//doSave2(PRESET2_selected - 1);
	}

	//TODO:
	//app settings
	save_ControlSettings();//crashes?

	//destroy callbacks
	removeKeysListeners();

	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxPresetsManager::Changed_Params_Control);

	//-

	//TODO: move above
	//MODE B: direct from memory
	if (MODE_MemoryLive && autoSave)
	{
		save_AllKit_FromMemory();
	}

	//--

	ofRemoveListener(ofEvents().update, this, &ofxPresetsManager::update);
	ofRemoveListener(ofEvents().draw, this, &ofxPresetsManager::draw);
}

//-

#pragma mark - API

//--------------------------------------------------------------
string ofxPresetsManager::presetName(string gName, int presetIndex)
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

	ofLogNotice("ofxPresetsManager") << "added group";

	groups.push_back(params);//each enqued group-param handles all (_num_presets) '8' presets

	lastIndices.push_back(0);//?
	newIndices.push_back(0);//?
	presets.push_back(_num_presets);//?

	//-

	//update control gui panel params
	num_presets = _num_presets;
	PRESET_selected.setMax(num_presets);

	//TODO:
	//PRESET2_selected.setMax(num_presets);

	//-

	//path folder and xml presets file names
	groupName = groups[0].getName();//TODO: one group only
	//groupName2 = groups[1].getName();//TODO: one group only

	ofLogNotice("ofxPresetsManager") << "groupName: " << groupName;

	//-

	//temporary name only to debug purposes
	//final label name to gui display will be setted if setup("name") is called 
	gui_LabelName = groups[0].getName();
	//TODO: one group only

	//-

	//memory mode
	load_AllKit_ToMemory();
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup params, initializer_list<int> keysList)
{
	//adds a ofParameterGroup for preset management with the list of trigger keys

	add(params, keysList.size());

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
	ofLogVerbose("ofxPresetsManager") << "save(" << presetIndex << "," << guiIndex << ")";

	if (guiIndex >= 0 && guiIndex < (int)groups.size()
		&& (presetIndex >= 0) && (presetIndex < NUM_OF_PRESETS))
	{
		ofLogVerbose("ofxPresetsManager") << "DONE_save (1)";

		//it's important if this line is before or after ofSerialize
		DONE_save = true;

		//-

		if (!MODE_MemoryLive)
		{
			//MODE A: from hd file

			TS_START("saveFile1");

			std::string strPath = presetName(groups[guiIndex].getName(), presetIndex);
			ofXml settings;
			ofSerialize(settings, groups[guiIndex]);
			settings.save(strPath);

			TS_STOP("saveFile1");
		}
		else
		{
			//MODE B: direct from memory

			TS_START("saveMem1");

			//ofLogNotice("ofxPresetsManager") << "MEMORY MODE";

			ofSerialize(settingsArray[presetIndex], groups[guiIndex]);

			TS_STOP("saveMem1");
		}

		////it's important if this line is before or after ofSerialize
		//DONE_save = true;
	}
	else
	{
		ofLogError("ofxPresetsManager") << "OUT OF RANGE SAVE";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::save(int presetIndex, string gName)
{
	ofLogVerbose("ofxPresetsManager") << "save(" << presetIndex << "," << gName << ")";

	int guiIndex = getGuiIndex(gName);

	if (guiIndex >= 0 && guiIndex < (int)groups.size()
		&& (presetIndex >= 0) && (presetIndex < NUM_OF_PRESETS))
	{
		ofLogVerbose("ofxPresetsManager") << "DONE_save (2)";

		//it's important if this line is before or after ofSerialize
		DONE_save = true;

		//-

		if (!MODE_MemoryLive)
		{
			//MODE A: from hd file

			TS_START("saveFile2");

			std::string strPath = presetName(gName, presetIndex);
			ofXml settings;
			ofSerialize(settings, groups[guiIndex]);
			settings.save(strPath);

			////it's important if this line is before or after ofSerialize
			//DONE_save = true;

			TS_STOP("saveFile2");
		}
		else
		{
			//MODE B: direct from memory

			TS_START("saveMem2");

			//ofLogNotice("ofxPresetsManager") << "MEMORY MODE";

			ofSerialize(settingsArray[presetIndex], groups[guiIndex]);

			TS_STOP("saveMem2");
		}
	}
	else
	{
		ofLogError("ofxPresetsManager") << "OUT OF RANGE SAVE";
	}

}

//--------------------------------------------------------------
void ofxPresetsManager::load(int presetIndex, int guiIndex)
{
	ofLogVerbose("ofxPresetsManager") << "load(" << presetIndex << "," << guiIndex << ")";

	//TODO:
	//guiIndex is not the preset position, it's for multiplye guis!
	//its the target group, there's only one for all the 8 presets (in only-one-gui-mode)

	if (guiIndex >= 0 && guiIndex < (int)groups.size()
		&& (presetIndex >= 0) && (presetIndex < NUM_OF_PRESETS))
	{
		if (!MODE_MemoryLive)
		{
			//MODE A: from hd file

			TS_START("loadFile1");

			std::string strPath = presetName(groups[guiIndex].getName(), presetIndex);
			ofXml settings;
			settings.load(strPath);
			ofDeserialize(settings, groups[guiIndex]);

			TS_STOP("loadFile1");
		}
		else
		{
			//MODE B: direct from memory

			TS_START("loadMem1");

			//ofLogNotice("ofxPresetsManager") << "MEMORY MODE";

			//using xml array
			ofDeserialize(settingsArray[presetIndex], groups[guiIndex]);

			//TODO:
			//ERROR do not loads the group content
			//ofParameterGroup g;
			//g = groupsMem[presetIndex];
			//groups[0] = g;

			TS_STOP("loadMem1");
		}

		//-

		//mark selected
		lastIndices[guiIndex] = presetIndex;

		//-

		//callback
		ofLogVerbose("ofxPresetsManager") << "DONE_load (1)";
		DONE_load = true;//callback

		//simple callback
		bIsDoneLoad = true;
	}
	else
	{
		ofLogError("ofxPresetsManager") << "OUT OF RANGE LOAD";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load(int presetIndex, string gName)
{
	ofLogVerbose("ofxPresetsManager") << "load(" << presetIndex << "," << gName << ")";

	int guiIndex = getGuiIndex(gName);

	if (guiIndex >= 0 && guiIndex < (int)groups.size()
		&& (presetIndex >= 0) && (presetIndex < NUM_OF_PRESETS))
	{
		if (!MODE_MemoryLive)
		{
			//MODE A: from hd file

			TS_START("loadFile2");

			std::string strPath = presetName(gName, presetIndex);
			ofXml settings;
			settings.load(strPath);
			ofDeserialize(settings, groups[guiIndex]);

			TS_STOP("loadFile2");
		}
		else
		{
			//MODE B: direct from memory

			TS_START("loadMem2");

			//ofLogNotice("ofxPresetsManager") << "MEMORY MODE";

			ofDeserialize(settingsArray[presetIndex], groups[guiIndex]);

			TS_STOP("loadMem2");
		}

		//-

		//mark selected
		lastIndices[guiIndex] = presetIndex;

		//-

		ofLogVerbose("ofxPresetsManager") << "DONE_load (1)";
		DONE_load = true;
	}
	else
	{
		ofLogError("ofxPresetsManager") << "OUT OF RANGE LOAD";
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
		return -1;
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::setModeKey(int key)
{
	modeKey = key;
}


#pragma mark - OF LISTENERS

//----------------------------------------------------------------
void ofxPresetsManager::keyPressed(ofKeyEventArgs &eventArgs)
{
	if (bKeys && ENABLE_Keys)
	{
		const int &key = eventArgs.key;

		//mode key for saving with mouse or trigger keys
		if (key == modeKey)
		{
			bKeySave = true;
			ofLogVerbose("ofxPresetsManager") << "modeKey TRUE" << endl;
			return;
		}

		//TODO: controlled from outside
		////hide/show control gui
		//if (key == 'G')
		//{
		//	SHOW_Gui_Internal = !SHOW_Gui_Internal;
		//	//bool b = get_GUI_Internal_Visible();
		//	//set_GUI_Internal_Visible(!b);
		//}
		//if (key == 'g')
		//{
		//	set_CLICKER_Visible(!is_CLICKER_Visible());
		//}

		//browse presets
		else if (key == OF_KEY_RIGHT && ENABLE_KeysArrowBrowse)
		{
			int i = PRESET_selected;
			i++;
			if (i > NUM_OF_PRESETS)
				i = NUM_OF_PRESETS;
			PRESET_selected = i;
		}
		else if (key == OF_KEY_LEFT && ENABLE_KeysArrowBrowse)
		{
			int i = PRESET_selected;
			i--;
			if (i < 1)
				i = 1;
			PRESET_selected = i;
		}

		//presets triggers
		for (int i = 0; i < keys.size(); ++i)
		{
			for (int k = 0; k < keys[i].size(); ++k)
			{
				if (key == keys[i][k])
				{
					ofLogNotice("ofxPresetsManager") << "Key: " << (char)key;

					if (bKeySave)
					{
						save(k, i);
					}
					else
					{
						ofLogNotice("ofxPresetsManager") << "["
							<< k << "][" << i << "]";

						PRESET_selected = 1 + k;

						//if (bDelayedLoading)
						//{
						//   //newIndices[i] = k;
						//   //ofLogNotice("ofxPresetsManager") << "newIndices[i] = k;" <<  k << ", " << i;
						//   //PRESET_selected = 1 + k;
						//}
						//else
						//{
						//PRESET_selected = 1 + k;//first row/gui only
						//}*/
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
	if (eventArgs.key == modeKey && ENABLE_Keys)
	{
		bKeySave = false;
		ofLogVerbose("ofxPresetsManager") << "modeKey FALSE" << endl;
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
	int yIndex = y / clicker_cellSize;

	xIndex = (x > 0) ? xIndex : -1;
	yIndex = (y > 0) ? yIndex : -1;
	//-1 for out of boxes

	//if (xIndex != -1 && yIndex != -1)
	//	ofLogNotice("ofxPresetsManager") << "mousePressed: (" << xIndex << "," << yIndex << ")";

	//-

	//1. key presets buttons
	if (yIndex >= 0 && yIndex < (int)groups.size())
	{
		//avoid outer panel logs. only into the group row levels
		if (xIndex != -1 && yIndex != -1)
			ofLogVerbose("ofxPresetsManager") << "mousePressed: (" << xIndex << "," << yIndex << ")";

		//-

		if (xIndex >= 0 && xIndex < presets[yIndex])
		{
			//save
			if (bKeySave)
			{
				doSave(xIndex);
				//auto load
				PRESET_selected = 1 + xIndex;
			}

			//load
			else
			{
				//if (bDelayedLoading)
				//{
				//   //newIndices[yIndex] = xIndex;
				//}
				//else
				//{
				PRESET_selected = 1 + xIndex;
				//   //load( xIndex, yIndex);
				//}
			}
		}

		//2. last button (save button)
		else if (xIndex == presets[yIndex])
		{
			ofLogNotice("ofxPresetsManager") << "saveButton: (" << yIndex << ")";

			//ofLogNotice("ofxPresetsManager") << "saveButton: ("
			//	<< lastIndices[yIndex]
			//	<< ", " << yIndex << ")";

			//save( lastIndices[yIndex], yIndex );
			doSave(lastIndices[yIndex]);
		}
	}
}

#pragma mark - ENGINE

//--------------------------------------------------------------
void ofxPresetsManager::doCloneRight(int pIndex)
{
	ofLogNotice("ofxPresetsManager") << "doCloneRight: pIndex: " << pIndex;
	for (int i = pIndex + 1; i < num_presets; i++)
	{
		save(i, 0);//only 1 row (gui) / data content
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::doSave(int pIndex)
{
	ofLogVerbose("ofxPresetsManager") << "doSave: pIndex: " << pIndex;
	save(pIndex, 0);
	//only 1 row (gui) / data content
}

////--------------------------------------------------------------
//void ofxPresetsManager::gui_loadFromFile(const std::string &filename, ofAbstractParameter &parameter)
//{
//   ofXml xml;
//   xml.load(filename);
//   ofDeserialize(xml, parameter);
//}
//
////--------------------------------------------------------------
//void ofxPresetsManager::gui_saveToFile(const std::string &filename, ofAbstractParameter &parameter)
//{
//   ofXml xml;
//   ofSerialize(xml, parameter);
//   xml.save(filename);
//}
//
////--------------------------------------------------------------
//void ofxPresetsManager::gui_SaveAsSettings()
//{
//
//}
//
////--------------------------------------------------------------
//static void ShowHelpMarker(const char *desc)
//{
//	ImGui::TextDisabled("(?)");
//	if (ImGui::IsItemHovered())
//	{
//		ImGui::BeginTooltip();
//		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
//		ImGui::TextUnformatted(desc);
//		ImGui::PopTextWrapPos();
//		ImGui::EndTooltip();
//	}
//}

//--------------------------------------------------------------
void ofxPresetsManager::preset_save(string name)//without xml extension
{
	ofLogNotice("ofxPresetsManager") << "preset_save: " << name << ".xml";

	ofXml settings;

	ofSerialize(settings, groups[0]);

	settings.save(path_GLOBAL_Folder + "/" + path_PresetsFolder + "/" + name + ".xml");
}

//--------------------------------------------------------------
void ofxPresetsManager::preset_load(string name)//without xml extension
{
	ofLogNotice("ofxPresetsManager") << "preset_load: " << name << ".xml";

	ofXml settings;
	settings.load(path_GLOBAL_Folder + "/" + path_PresetsFolder + "/" + name + ".xml");

	//TODO:
	//using one gui only! 0
	ofDeserialize(settings, groups[0]);

	//-

	ofLogVerbose("ofxPresetsManager") << "DONE_load";
	DONE_load = true;
}

//--------------------------------------------------------------
void ofxPresetsManager::preset_filesRefresh()
{
	ofDirectory dataDirectory(ofToDataPath(path_PresetsFolder, true));

	ofLogNotice("ofxPresetsManager") << "preset_filesRefresh path:" << path_GLOBAL_Folder + "/" + path_PresetsFolder;

	//clear files and filenames vectors
	files.clear();
	fileNames.clear();

	//load all folder files in one call
	files = dataDirectory.getFiles();

	for (size_t i = 0; i < files.size(); i++)
	{
		fileNames.push_back(files[i].getBaseName());
		ofLogNotice("ofxPresetsManager") << "file [" << i << "] " << files[i].getBaseName();
	}

	//-

	//TODO
	//void to go to 1st...

	//1. load same position preset
	//if preset is deleted goes to nextone..
	//should check names because sorting changes..
	if (fileNames.size() > 0)
	{
		if (currentFile > fileNames.size() - 1)
			currentFile = fileNames.size() - 1;
		//else if (currentFile > fileNames.size() - 1)

		PRESET_name = fileNames[currentFile];
		preset_load(PRESET_name);
	}
	else
	{
		ofLogError("ofxPresetsManager") << "FILE PRESET NOT FOUND!";
	}

	////2. always goes to 1st preset 0
	////that's because saving re sort the files
	////and we don't know the position of last saves preset..
	//if (fileNames.size() > 0)
	//{
	//   currentFile = 0;
	//   PRESET_name = fileNames[currentFile];
	//   preset_load(PRESET_name);
	//}
	//else
	//{
	//   ofLogError("ofxColorManager") << "NOT FOUND ANY FILE PRESET!";
	//}
}

//--------------------------------------------------------------
void ofxPresetsManager::loadPreset(int p)
{
	if (!DISABLE_CALLBACKS)// && (PRESET_selected != PRESET_selected_PRE))
	{
		ofLogNotice("ofxPresetsManager") << "loadPreset(" << ofToString(p) << ")";
		ofLogNotice("ofxPresetsManager") << "-------------------------------------------------------------------------------------------------------";

		if (PRESET_selected > 0 && PRESET_selected <= num_presets)
		{
			PRESET_selected = p;
			//ofLogNotice("ofxPresetsManager") << ".";

			//PRESET_selected_PRE = PRESET_selected;//TODO:
		}
		else
		{
			ofLogNotice("ofxPresetsManager") << "IGNORE LOAD PRESET";
			//workaround clamp
			PRESET_selected = 1;//set to first as default presets when out of range
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::toggleKeysControl(bool active)
{
	bKeys = active;
}

#pragma mark - CALLBACKS

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Params_Control(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		ofLogVerbose("ofxPresetsManager") << "Changed_Params_Control '" << name << "': " << e;

		//-

		if (name == "SAVE" && bSave)
		{
			ofLogNotice("ofxPresetsManager") << "SAVE: " << e;
			bSave = false;
			doSave(PRESET_selected - 1);
		}
		else if (name == "ENABLE KEYS")
		{

		}
		else if (name == "CLONE >" && bCloneRight)
		{
			ofLogNotice("ofxPresetsManager") << "CLONE >: " << e;
			bCloneRight = false;
			doCloneRight(PRESET_selected - 1);
		}
		else if (name == "GUI POSITION")
		{
			ofLogVerbose("ofxPresetsManager") << "GUI POSITION: " << e;
			float x, y;
			x = ofClamp(Gui_Position.get().x, 0, ofGetWidth() - 200);
			y = ofClamp(Gui_Position.get().y, 0, ofGetHeight() - 20);
			guiControl.setPosition(x, y);
		}

		//-

		else if (name == "LOAD TO MEMORY" && loadToMemory)
		{
			ofLogNotice("ofxPresetsManager") << "loadToMemory:" << e;
			loadToMemory = false;
			load_AllKit_ToMemory();
		}
		else if (name == "SAVE FROM MEMORY" && saveFromMemory)
		{
			ofLogNotice("ofxPresetsManager") << "saveFromMemory:" << e;
			saveFromMemory = false;
			save_AllKit_FromMemory();
		}
		else if (name == "MODE MEMORY")
		{
			ofLogNotice("ofxPresetsManager") << "MODE MEMORY: " << e;

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

		//TODO: should be nice to add toggle to auto retrig or not same pre loaded preset

		else if (name == "PRESETS" && (PRESET_selected == PRESET_selected_PRE))
		{
			//ofLogNotice("ofxPresetsManager") << "PRESET NOT Changed: " << e;
			ofLogNotice("ofxPresetsManager") << "RE-TRIG PRESET " << e;

			bMustTrig = true;

			//callbacks flag triggers
			DONE_load = true;
			bIsDoneLoad = true;
		}

		else if (name == "PRESETS" && (PRESET_selected != PRESET_selected_PRE))
		{
			ofLogNotice("ofxPresetsManager") << "PRESET: " << e;
			//ofLogNotice("ofxPresetsManager") << e;

			/*
			//   //byKey
			//   //load( k, i );
			//   //ofLogNotice("ofxPresetsManager") << "load( k, i ):" <<  k << ", " << i;
			//
			//   //byMousePressed
			//   //load( xIndex, yIndex);
			//   //ofLogNotice("ofxPresetsManager") << "load( xIndex, yIndex):" <<  xIndex << ", " << yIndex;
			*/

			//-

			//if (PRESET_selected_PRE > 0 && PRESET_selected>=1)
			if (true)
			{
				if (autoSave)
				{
					//TODO:
					//DONE_save = true;//callback

					save(PRESET_selected_PRE - 1, 0);
				}

				//-

				//remember this PRE state to know if changed or not
				PRESET_selected_PRE = PRESET_selected;

				//-

				//indexes starts from 0, not from 1 like presets
				int xIndex = PRESET_selected - 1;
				//yIndex = gui/group-parameter number
				//this should handle when using multiple gui/group-parameter kits together. 
				int yIndex = 0;//TODO: one group only

				//-

				if (autoLoad)
				{
					load(xIndex, yIndex);
					//ofLogNotice("ofxPresetsManager") << "load( xIndex, yIndex): " << xIndex << ", " << yIndex;

					//DONE_load = true;//callback
				}
				else
				{
					//TODO:
					//lastIndices it's only the gui box clicked only, not important.. ?
					if (lastIndices.size() > 0
						&& yIndex < lastIndices.size()
						&& xIndex < NUM_OF_PRESETS)
					{
						//mark selector
						lastIndices[yIndex] = xIndex;
						//ofLogNotice("ofxPresetsManager") << "lastIndices[yIndex]: " << xIndex;
						//ofLogNotice("ofxPresetsManager") << "autoLoad: " << autoLoad;
					}
					else
					{
						ofLogNotice("ofxPresetsManager") << "lastIndices has 0 size!";
					}
				}
			}
			else
			{
				ofLogError("ofxPresetsManager") << "IGNORED PRESETS CHANGE";
			}
		}
	}
}

#pragma mark - SETTINGS

//--------------------------------------------------------------
void ofxPresetsManager::load_ControlSettings()
{
	ofXml settings;
	string path = path_GLOBAL_Folder + "/" + path_Control + "/" + "control.xml";
	bool bLoaded = settings.load(path);

#ifdef INCLUDE_DEBUG_ERRORS
	if (!bLoaded)
	{
		errorsDEBUG.addError(gui_LabelName + " ofxPresetsManager", "load_ControlSettings()", path);
	}
#endif

	ofLogNotice("ofxPresetsManager") << "load_ControlSettings: " << path;
	ofLogNotice("ofxPresetsManager") << "load_ControlSettings: PRESET " << PRESET_selected;

	if (bLoaded)
	{
		ofDeserialize(settings, params_Control);
	}
	else
	{
		ofLogError("ofxPresetsManager") << "FILE '" << path << "' NOT FOUND!";
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::save_ControlSettings()
{
	ofLogNotice("ofxPresetsManager") << "save_ControlSettings()";
	DISABLE_CALLBACKS = true;//?

	//TODO: crashes?
	//get gui position to update param
	float x, y;
	x = guiControl.getPosition().x;
	y = guiControl.getPosition().y;
	Gui_Position = glm::vec2(x, y);

	ofXml settingsControl;

	//TODO: crashes?
	ofLogNotice("ofxPresetsManager") << endl << params_Control.toString() << endl;
	ofSerialize(settingsControl, params_Control);
	ofLogNotice("ofxPresetsManager") << settingsControl << endl;
	
	string path = path_GLOBAL_Folder + "/" + path_Control + "/" + "control.xml";

	ofLogNotice("ofxPresetsManager") << "path: " << path;
	settingsControl.save(path);

	//ofLogNotice("ofxPresetsManager") << "save_ControlSettings:" << path;
	DISABLE_CALLBACKS = false;//?
}

//--------------------------------------------------------------
void ofxPresetsManager::set_Path_KitFolder(string folder)
{
	path_Kit_Folder = folder;
}

//--------------------------------------------------------------
void ofxPresetsManager::set_Path_PresetsFolder(string folder)
{
	path_PresetsFolder = folder;
}

//--------------------------------------------------------------
void ofxPresetsManager::set_Path_GlobalFolder(string folder)
{
	path_GLOBAL_Folder = folder;
}

//--

#pragma mark - BROWSER_GUI

////* browser
////--------------------------------------------------------------
//bool ofxPresetsManager::gui_draw()
//{
//	static bool SHOW_About = false;
//	static bool SHOW_Gui_Internal = true;
//
//	auto mainSettings = ofxImGui::Settings();
//	mainSettings.windowPos = guiPos;
//	mainSettings.windowSize = guiSize;
//
//	gui.begin();
//	{
//		if (SHOW_Gui_Internal)
//		{
//			if (ofxImGui::BeginWindow("PRESET MANAGER", mainSettings, false))
//				//if (ofxImGui::BeginWindow("PRESET MANAGER", mainSettings,
//				//   ImGuiWindowFlags_NoCollapse |
//				//   ImGuiWindowFlags_AlwaysAutoResize))
//			{
//				ImGui::PushItemWidth(100);
//
//				ofxImGui::AddParameter(ENABLE_Keys);
//
//				//1. files browser
//				gui_imGui_PresetManager();
//
//				ImGui::Separator();
//
//				//-
//
//				//2.1 live presets
//				ofxImGui::AddGroup(this->params_Favorites, mainSettings);
//
//				//2.2 advanced
//				if (ImGui::CollapsingHeader("ADVANCED"))
//				{
//					ofxImGui::AddGroup(this->params_Options, mainSettings);
//					ofxImGui::AddGroup(this->params_Gui, mainSettings);
//					ofxImGui::AddGroup(this->params_Tools, mainSettings);
//				}
//
//				ImGui::Separator();
//
//				ImGui::PopItemWidth();
//				ofxImGui::EndWindow(mainSettings);
//			}
//		}
//
//		//-
//
//		if (SHOW_menu)
//		{
//			if (ImGui::BeginMainMenuBar())
//			{
//				if (ImGui::BeginMenu("File"))
//				{
//					gui_draw_MenuFile();
//					ImGui::EndMenu();
//				}
//				if (ImGui::BeginMenu("Help"))
//				{
//					ImGui::MenuItem("About ofxPresetsManager", NULL, &SHOW_About);
//					ImGui::EndMenu();
//				}
//				ImGui::EndMainMenuBar();
//			}
//		}
//
//		if (SHOW_About)
//		{
//			ImGui::Begin("About ofxPresetsManager", &SHOW_About,
//				ImGuiWindowFlags_NoCollapse |
//				ImGuiWindowFlags_AlwaysAutoResize);
//			ImGui::Text("ofParameterGroup presets presetsManager");
//			ImGui::Separator();
//			ImGui::Text("by MoebiusSurfing.");
//			ImGui::End();
//		}
//	}
//
//	gui.end();
//	return mainSettings.mouseOverGui;
//}
//
////--------------------------------------------------------------
//void ofxPresetsManager::gui_draw_MenuBar()
//{
//	if (ImGui::BeginMainMenuBar())
//	{
//		if (ImGui::BeginMenu("File"))
//		{
//			gui_draw_MenuFile();
//			ImGui::EndMenu();
//		}
//		ImGui::EndMainMenuBar();
//	}
//}
//
////--------------------------------------------------------------
//void ofxPresetsManager::gui_draw_MenuFile()
//{
//	if (ImGui::MenuItem("New"))
//	{
//	}
//	if (ImGui::MenuItem("Open", "l"))
//	{
//		//gui_loadFromFile("settings.xml", params_Control);
//	}
//	if (ImGui::MenuItem("Save", "s"))
//	{
//		//gui_saveToFile("settings.xml", params_Control);
//	}
//	if (ImGui::MenuItem("Save As.."))
//	{
//		//gui_SaveAsSettings();
//	}
//	ImGui::Separator();
//	ImGui::Separator();
//	if (ImGui::MenuItem("Quit", "ESQ"))
//	{
//	}
//}

////TODO: debug
////--------------------------------------------------------------
//void ofxPresetsManager::groupDebug(ofParameterGroup &group)
//{
//	string str;
//	int x = 200;
//	int y = 100;
//	int pad = 20;
//	int i = 0;
//	int ig = 0;
//
//	for (auto parameter : group)
//	{
//		//Group.
//		auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
//		if (parameterGroup)
//		{
//			//Recurse through contents.
//			//ofxImGui::AddGroup(*parameterGroup, settings);
//			groupDebug(*parameterGroup);
//
//			ig++;
//			str = "group : [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x + ig * 300, y + pad * i++);
//			continue;
//		}
//
//		//Parameter, try everything we know how to handle.
//#if OF_VERSION_MINOR >= 10
//		auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
//		if (parameterVec2f)
//		{
//			//ofxImGui::AddParameter(*parameterVec2f);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//		auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
//		if (parameterVec3f)
//		{
//			//ofxImGui::AddParameter(*parameterVec3f);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//		auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
//		if (parameterVec4f)
//		{
//			//ofxImGui::AddParameter(*parameterVec4f);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//#endif
//		auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
//		if (parameterOfVec2f)
//		{
//			//ofxImGui::AddParameter(*parameterOfVec2f);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//		auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
//		if (parameterOfVec3f)
//		{
//			//ofxImGui::AddParameter(*parameterOfVec3f);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//		auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
//		if (parameterOfVec4f)
//		{
//			//ofxImGui::AddParameter(*parameterOfVec4f);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//		auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
//		if (parameterFloatColor)
//		{
//			//ofxImGui::AddParameter(*parameterFloatColor);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//		auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
//		if (parameterFloat)
//		{
//			//ofxImGui::AddParameter(*parameterFloat);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//		auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
//		if (parameterInt)
//		{
//			//ofxImGui::AddParameter(*parameterInt);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//		auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
//		if (parameterBool)
//		{
//			//ofxImGui::AddParameter(*parameterBool);
//			str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
//			ofDrawBitmapStringHighlight(str, x, y + pad * i++);
//			continue;
//		}
//
//		ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter " << parameter->getName();
//	}
//}
//
////--------------------------------------------------------------
//void ofxPresetsManager::gui_imGui_PresetManager()
//{
//	auto mainSettings = ofxImGui::Settings();
//
//	//if (ofxImGui::BeginWindow("PRESET MANAGER", mainSettings, false))
//	//{
//	if (ofxImGui::BeginTree("BROWSER", mainSettings))
//	{
//		//-
//
//		ImGui::Text("NAME:");
//		string textInput_temp = PRESET_name;
//
//		//loaded string into char array
//		char tab2[32];
//		strncpy(tab2, textInput_temp.c_str(), sizeof(tab2));
//		tab2[sizeof(tab2) - 1] = 0;
//
//		if (ImGui::InputText("", tab2, IM_ARRAYSIZE(tab2)))
//		{
//			ofLogNotice("ofxPresetsManager") << "InputText:" << ofToString(tab2) << endl;
//
//			textInput_temp = ofToString(tab2);
//			ofLogNotice("ofxPresetsManager") << "textInput_temp:" << textInput_temp << endl;
//
//			if (MODE_newPreset)
//				MODE_newPreset = false;
//		}
//
//		//--
//
//		//arrow buttons
//		static int counter = currentFile;
//		float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
//
//		ImGui::PushButtonRepeat(true);
//
//		//prev
//		if (ImGui::ArrowButton("##left", ImGuiDir_Left))
//		{
//			if (counter > 0)
//			{
//				counter--;
//				currentFile = counter;
//				if (currentFile < files.size())
//				{
//					PRESET_name = fileNames[currentFile];
//					ofLogNotice() << "ARROW: PRESET_name: [" + ofToString(currentFile) + "] " << PRESET_name;
//					preset_load(PRESET_name);
//				}
//
//				if (MODE_newPreset)
//					MODE_newPreset = false;
//			}
//		}
//
//		//next
//		ImGui::SameLine(0.0f, spacing);
//		if (ImGui::ArrowButton("##right", ImGuiDir_Right))
//		{
//			if (counter < files.size() - 1)
//			{
//				counter++;
//				currentFile = counter;
//				if (currentFile < files.size())
//				{
//					PRESET_name = fileNames[currentFile];
//					ofLogNotice() << "ARROW: PRESET_name: [" + ofToString(currentFile) + "] " << PRESET_name;
//					preset_load(PRESET_name);
//				}
//			}
//
//			if (MODE_newPreset)
//				MODE_newPreset = false;
//		}
//
//		ImGui::PopButtonRepeat();
//
//		//preview current preset number to total
//		int numPalettes = fileNames.size() - 1;
//		ImGui::SameLine();
//		ImGui::Text("%d/%d", currentFile, numPalettes);
//
//		//-
//
//		//scrollable list
//		if (!fileNames.empty())
//		{
//			int currentFileIndex = currentFile;
//			if (ofxImGui::VectorCombo(" ", &currentFileIndex, fileNames))
//			{
//				ofLog() << "currentFileIndex: " << ofToString(currentFileIndex);
//				if (currentFileIndex < fileNames.size())
//				{
//					currentFile = currentFileIndex;
//					PRESET_name = fileNames[currentFile];
//					ofLog() << "PRESET_name: " << PRESET_name;
//					preset_load(PRESET_name);
//				}
//
//				if (MODE_newPreset)
//					MODE_newPreset = false;
//			}
//		}
//
//		//-
//
//		//2. presets
//
//		ImGui::Text("PRESETS");
//
//		if (ImGui::Button("SAVE"))
//		{
//			ofLogNotice("ofxPresetsManager") << "SAVE" << endl;
//
//			//TODO
//			//should re load by same name and get what position on vector
//			//is to reload current preset number
//			//           textInput_temp = ofToString(tab2);
//			//           ofLogNotice("ofxPresetsManager") << "textInput_temp:" << textInput_temp << endl;
//
//			PRESET_name = textInput_temp;
//			ofLogNotice("ofxPresetsManager") << "PRESET_name: " << PRESET_name << endl;
//
//			preset_save(PRESET_name);
//			preset_filesRefresh();
//		}
//
//		ImGui::SameLine();
//		if (ImGui::Button("UPDATE"))
//		{
//			ofLogNotice("ofxPresetsManager") << "UPDATE" << endl;
//
//			PRESET_name = textInput_temp;
//			ofLogNotice("ofxPresetsManager") << "PRESET_name: " << PRESET_name << endl;
//
//			//delete old file
//			files[currentFile].remove();
//			//preset_filesRefresh();
//
//			//save new one
//			preset_save(PRESET_name);
//			preset_filesRefresh();
//		}
//
//		ImGui::SameLine();
//		if (ImGui::Button("LOAD"))//not required..
//		{
//			ofLogNotice("ofxPresetsManager") << "LOAD" << endl;
//			ofLogNotice("ofxPresetsManager") << "PRESET_name: " << PRESET_name << endl;
//			preset_load(PRESET_name);
//		}
//
//		ImGui::SameLine();
//		if (ImGui::Button("DELETE"))//current preset
//		{
//			ofLogNotice("ofxPresetsManager") << "DELETE" << endl;
//
//			files[currentFile].remove();
//			preset_filesRefresh();
//
//			//string str = fileNames[currentFile];
//			//ofLogNotice("ofxPresetsManager") << "DELETE:"<<str<<endl;
//			//dir.listDir("user_kits/presets");
//			//dir.allowExt("jpg");
//			//dir.allowExt("png");
//			//dir.sort();
//		}
//
//		//ImGui::SameLine();
//		//if (ImGui::Button("REFRESH"))//current preset
//		//{
//		//   ofLogNotice("ofxPresetsManager") << "REFRESH" << endl;
//		//   preset_filesRefresh();
//		//}
//
//		//-
//
//		//TODO
//		MODE_newPreset = true;
//		if (MODE_newPreset)
//		{
//			ImGui::Separator();
//			ImGui::Text("NEW PRESET!");
//
//			//loaded string into char array
//			char tab[32];
//			strncpy(tab, textInput_New.c_str(), sizeof(tab));
//			tab[sizeof(tab) - 1] = 0;
//
//			if (ImGui::InputText("", tab, IM_ARRAYSIZE(tab)))
//			{
//				ofLogNotice("ofxPresetsManager") << "InputText [tab]:" << ofToString(tab) << endl;
//				textInput_New = ofToString(tab);
//				ofLogNotice("ofxPresetsManager") << "textInput_New:" << textInput_New << endl;
//			}
//
//			//WORKFLOW: when it's editing a new preset..
//
//			ImGui::PushID(1);
//			int n = 30;
//			float a = ofMap(ofGetFrameNum() % n, 0, n, 0.0f, 1.0f);
//			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5, 0.0f, 0.5f, a));
//
//			if (ImGui::Button("SAVE NEW"))
//			{
//				MODE_newPreset = false;
//				ofLogNotice("ofxPresetsManager") << "textInput_New: " << textInput_New << endl;
//				preset_save(textInput_New);
//				preset_filesRefresh();
//			}
//
//			ImGui::PopStyleColor(1);
//			ImGui::PopID();
//		}
//
//		//-
//
//		ofxImGui::EndTree(mainSettings);
//	}
//	//ofxImGui::EndWindow(mainSettings);
//}
//
////--------------------------------------------------------------
//void ofxPresetsManager::gui_imGui_theme()
//{
//	//must be done after setup the gui
//
//	ImGuiStyle *style = &ImGui::GetStyle();
//
//	//my dark theme
//	//   ImVec4* colors = ImGui::GetStyle().Colors;
//	style->Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
//	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
//	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
//	style->Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
//	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
//	style->Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
//	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.54f);
//	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.26f, 0.68f);
//	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.67f);
//	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
//	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
//	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
//	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
//	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
//	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.12f, 0.11f, 0.11f, 1.00f);
//	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
//	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
//	style->Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
//	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
//	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.75f, 0.77f, 0.79f);
//	style->Colors[ImGuiCol_Button] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
//	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.79f);
//	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
//	style->Colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.31f);
//	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
//	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
//	style->Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
//	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.29f, 0.29f, 0.29f, 0.78f);
//	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
//	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.44f, 0.44f, 0.44f, 0.25f);
//	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.68f, 0.68f, 0.68f, 0.67f);
//	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.68f, 0.68f, 0.68f, 0.95f);
//	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.81f, 0.79f, 0.79f, 1.00f);
//	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
//	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
//	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
//	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
//	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.50f, 0.50f, 0.50f, 0.90f);
//	style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
//	style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
//	style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
//	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
//
//	//   //2. defaut dark theme exported
//	//   style->WindowMinSize = ImVec2(160, 65);
//	//   style->FramePadding = ImVec2(4, 2);
//	//   style->ItemSpacing = ImVec2(6, 2);
//	//   style->ItemInnerSpacing = ImVec2(6, 4);
//	//   style->Alpha = 1.0f;
//	//   style->WindowRounding = 0.0f;
//	//   style->FrameRounding = 0.0f;
//	//   style->IndentSpacing = 6.0f;
//	//   style->ItemInnerSpacing = ImVec2(2, 4);
//	//   style->ColumnsMinSpacing = 50.0f;
//	//   style->GrabMinSize = 14.0f;
//	//   style->GrabRounding = 0.0f;
//	//   style->ScrollbarSize = 12.0f;
//	//   style->ScrollbarRounding = 0.0f;
//}

//--

//memory mode 
//(loaded from data vector instead of hd files)
//--------------------------------------------------------------
void ofxPresetsManager::save_AllKit_FromMemory()
{
	ofLogVerbose("ofxPresetsManager") << "save_AllKit_FromMemory()";

	for (int i = 0; i < NUM_OF_PRESETS; i++)
	{
		string strFolder;
		string strFile;
		string strPath;

		strFolder = path_GLOBAL_Folder + "/" + path_Kit_Folder + "/";
		strFile = groupName + path_Prefix + ofToString(i) + ".xml";
		strPath = strFolder + strFile;

		settingsArray[i].save(strPath);
	}

	//debug params
	bool bDEBUG = false;
	if (bDEBUG)
	{
		for (int i = 0; i < NUM_OF_PRESETS; i++)
		{
			ofLogNotice("ofxPresetsManager") << "settingsArray[" << i << "] " << ofToString(settingsArray[i].toString());
		}
	}
}

//--------------------------------------------------------------
void ofxPresetsManager::load_AllKit_ToMemory()
{
	ofLogNotice("ofxPresetsManager") << "load_AllKit_ToMemory()";

	////TODO:
	//groupName = groups[0].getName();
	//ofLogNotice("ofxPresetsManager") << "groups[0].getName(): " << groupName;
	//groupsMem.clear();
	//groupsMem.reserve(NUM_OF_PRESETS);
	//groupsMem.resize(NUM_OF_PRESETS);

	for (int i = 0; i < NUM_OF_PRESETS; i++)
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
			ofLogNotice("ofxPresetsManager") << "[" << i << "]";
			ofLogNotice("ofxPresetsManager") << "File: " << pathComplete
				<< "\n" << ofToString(settings.toString());
		}

		//-

		////TODO:
		//ofDeserialize(settings, groupsMem[i]);
		//ofLogNotice("ofxPresetsManager") << "ofParameterGroup: " << i << "\n" << ofToString(groupsMem[i]);

		if (bLoaded)
		{
			settingsArray[i] = settings;
		}
#ifdef INCLUDE_DEBUG_ERRORS
		else if (!bLoaded)
		{
			errorsDEBUG.addError("ofxPresetsManager " + gui_LabelName, "load_AllKit_ToMemory() - NOT FOUND OR EMPTY XML FILES:", pathComplete);
			//TODO:
			//maybe we should create not found or empty xml presets with an empty preset..
		}
#endif
	}

	ofLogNotice("ofxPresetsManager") << "-------------------------------------------------------------------------------------------------------";

	//debug params
	bool bDEBUG = false;
	if (bDEBUG)
	{
		for (int i = 0; i < NUM_OF_PRESETS; i++)
		{
			ofLogNotice("ofxPresetsManager") << "settingsArray[" << i << "]\n" << ofToString(settingsArray[i].toString());
		}
	}

	////debug params
	//for (int i = 0; i < NUM_OF_PRESETS; i++)
	//{
	//	ofLogNotice("ofxPresetsManager") << "groupsMem[" << i << "]\n" << ofToString(groupsMem[i]);
	//}
}

////--------------------------------------------------------------
//void ofxPresetsManager::addGroup_TARGET(ofParameterGroup &g)
//{
//	ofLogNotice("ofxPresetsManager") << "addGroup_TARGET:" << "" << endl;
//
//	group_TARGET = g;
//}
