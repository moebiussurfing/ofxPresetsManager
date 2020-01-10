
// ofxPresetsManager.h
// based into Nicola Pisanti' code in pisanti/ofxGuiPresetSelector, MIT License, 2016
//
// changes by moebiussurfing

#include "ofxPresetsManager.h"


#pragma mark - OF

//--------------------------------------------------------------
ofxPresetsManager::ofxPresetsManager()
{
    ofSetLogLevel("ofxPresetsManager", OF_LOG_NOTICE);

    modeKey = OF_KEY_CONTROL;
    bKeys = false;
    keysNotActivated = true;
    bKeySave = false;

    //-

    // A. ofParameterGroup

#ifdef USE_OF_PARAMETER_GROUP
    groups.reserve(32);//TODO: this multidimension is for multiple panels ?
#endif

    //-

    // B. custom DataGrid class

#ifdef USE_CUSTOM_DATAGRID
    grids.reserve(32);//TODO: this multidimension is for multiple panels ?
#endif

    //-

    lastIndices.reserve(32);//TODO: this multidimension is for multiple panels ?
    keys.reserve(32);//TODO: this multidimension is for multiple panels ?

    lastMouseButtonState = false;
    bDelayedLoading = false;

    //-

    // TODO: easy listener temp solution for ofxSEQ integration
    DONE_load.set("DONE LOAD", false);
    DONE_save.set("DONE SAVE", false);
    //can use too isDoneLoad() to check in update() as callback

    //-

    myTTF = "fonts/PragmataProR_0822.ttf";
    sizeTTF = 8;
    myFont.load(myTTF, sizeTTF, true, true);

    //-

    // PRESETS

    PRESET_selected.set("PRESETS", 1, 1, num_presets);
    bSave.set("SAVE", false);
    autoLoad.set("AUTO LOAD", false);
    autoSave.set("AUTO SAVE", false);
    bCloneRight.set("CLONE >", false);
    SHOW_menu.set("SHOW MENU", false);
    SHOW_ClickPanel.set("SHOW CLICK PANEL", false);
    ENABLE_shortcuts.set("ENABLE SHORTCUTS [TAB]", true);

    params_Favorites.setName("FAVORITES");
    params_Favorites.add(PRESET_selected);
    params_Favorites.add(bSave);
    params_Options.setName("OPTIONS");
    params_Options.add(autoLoad);
    params_Options.add(autoSave);
    params_Gui.setName("GUI");
    params_Gui.add(SHOW_ClickPanel);
    params_Gui.add(SHOW_menu);
    params_Gui.add(ENABLE_shortcuts);
    params_Tools.setName("TOOLS");
    params_Tools.add(bCloneRight);

    //nested params for callbacks and storage settings
    params.add(params_Favorites);
    params.add(params_Options);
    params.add(params_Gui);
    params.add(params_Tools);

    ofAddListener(params.parameterChangedE(), this, &ofxPresetsManager::Changed_Params);
}

//--------------------------------------------------------------
void ofxPresetsManager::setup()
{
#ifdef TIME_SAMPLE_OFX_FONTSTASH2
    //TIME_SAMPLE_SET_FRAMERATE(fps); //specify a target framerate
    TIME_SAMPLE_ENABLE();
    TIME_SAMPLE_SET_AVERAGE_RATE(0.1);
    TIME_SAMPLE_SET_DRAW_LOCATION(TIME_SAMPLE_DRAW_LOC_BOTTOM_LEFT);
    TIME_SAMPLE_SET_PRECISION(4);

    TIME_SAMPLE_GET_INSTANCE()->setEnabled(true);
#endif

    //ofSetLogLevel("ofxPresetsManager", OF_LOG_VERBOSE);

    //selected_PRE = -1;
    load_ControlSettings();

    //TODO
    selected_PRE = -1;

    gui.setup();
    gui_imGui_theme();
    gui_Visible = true;

    //TODO
    //bug on startuo, not loading fine some param
    //PRESET_selected = 0;
}

//--------------------------------------------------------------
void ofxPresetsManager::update()
{
    //autosave
    //bAutosave = false;
    if (bAutosave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
    {
        //DISABLE_Callbacks = true;
        ////get gui position before save
        //Gui_Position = glm::vec2(gui.getPosition());
        //saveParams(params, path_GLOBAL + path_Params);
        //DISABLE_Callbacks = false;

        //save current preset
        doSave(PRESET_selected - 1);

        timerLast_Autosave = ofGetElapsedTimeMillis();
        ofLogNotice("ofxPresetsManager") << "Autosave DONE";
    }
}

//---------------------------------------------------------------------
void ofxPresetsManager::draw()
{
    //-

    //TODO

    //// Gui
    //bMouseOver_Changed = false;
    //bool gui_MouseOver_PRE = gui_MouseOver;
    //gui_MouseOver = false;
    //
    ////TODO
    //gui_Visible = true;
    //
    //if (gui_Visible)
    //{
    //    gui_MouseOver = gui_draw();
    //    //ofLogNotice("ofxPresetsManager") << "gui_MouseOver:" << (gui_MouseOver?"IN":"OUT");
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
    //    bMouseOver_Changed = true;
    //}
    //
    //////debug
    ////if (bMouseOver_Changed)
    ////{
    ////    if (gui_MouseOver)
    ////    {
    ////        ofLogNotice("ofxPresetsManager") << "MOUSE OVER GUI";
    ////    }
    ////    else
    ////    {
    ////        ofLogNotice("ofxPresetsManager") << "MOUSE OUT OF GUI";
    ////    }
    ////}

    //-

    if (SHOW_ClickPanel)
    {
        draw_CLICKER();
    }

    //-

    //TODO
    ////groupDebug
    //{
    //    ofParameterGroup g;
    //    if (groups.size() > 0)
    //    {
    //        g = groups[0];
    //        groupDebug(g);
    //    }
    //}

    //-
}


//--------------------------------------------------------------
ofxPresetsManager::~ofxPresetsManager()
{
    //save PRESET_selected preset on exit
    doSave(PRESET_selected - 1);

    save_ControlSettings();

    removeKeysListeners();
    ofRemoveListener(params.parameterChangedE(), this, &ofxPresetsManager::Changed_Params);
}

//-

#pragma mark - API

// A. ofParameterGroup
#ifdef USE_OF_PARAMETER_GROUP

//--------------------------------------------------------------
int ofxPresetsManager::getGuiIndex(string name) const
{
    for (size_t i = 0; i < groups.size(); ++i)
    {
        if (groups[i].getName() == name)
        {
            return i;
        }
        return -1;
    }
}

#endif

//-

// B. custom DataGrid class
#ifdef USE_CUSTOM_DATAGRID
//--------------------------------------------------------------
int ofxPresetsManager::getGuiIndex( string name ) const {

    for( size_t i = 0; i<grids.size(); ++i ){
        string myName = grids[i]->getName();
        if( myName == name ){
            return i;
        }
        return -1;
    }
}
#endif

//--------------------------------------------------------------
string ofxPresetsManager::presetName(string guiName, int presetIndex)
{
    string folder;
    //folder = "/patterns/"; //using subfolder
    folder = "/"; //without subfolder. must ends with "/"

    //-

    // A. ofParameterGroup
#ifdef USE_OF_PARAMETER_GROUP
    return (folder + guiName + "_preset_" + ofToString(presetIndex) + ".xml");
#endif

    //-

    // B. custom DataGrid class
#ifdef USE_CUSTOM_DATAGRID
    return (folder + guiName + "_preset_" + ofToString(presetIndex) + ".json" );
#endif

}

//-

// A. ofParameterGroup
#ifdef USE_OF_PARAMETER_GROUP

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup params, int _num_presets)
{
    // add a gui for preset saving

    groups.push_back(params);

    lastIndices.push_back(0);
    newIndices.push_back(0);
    presets.push_back(_num_presets);

    //-

    num_presets = _num_presets;
    PRESET_selected.setMax(num_presets);
}

//--------------------------------------------------------------
void ofxPresetsManager::add(ofParameterGroup params, initializer_list<int> keysList)
{
    add(params, keysList.size());

    keys.resize(groups.size());
    int i = groups.size() - 1;

    keys[i].reserve(keysList.size());

    for (const int &key : keysList)
        keys[i].push_back(key);

    if (keysNotActivated)
        addKeysListeners();
}

#endif

//-

// B. custom DataGrid class
#ifdef USE_CUSTOM_DATAGRID
//--------------------------------------------------------------
void ofxPresetsManager::add( DataGrid & grid, int numPresets ) {

    // add a gui for preset saving

    grids.push_back(&grid);

    lastIndices.push_back(0);
    newIndices.push_back(0);
    presets.push_back(numPresets);

    //-

    num_presets = numPresets;
    //(group->getIntSlider("PRESETS"))->setMax(num_presets);

}

//--------------------------------------------------------------
void ofxPresetsManager::add( DataGrid & grid, initializer_list<int> keysList ) {
    add( grid, keysList.size() );

    keys.resize(grids.size());
    int i = grids.size() - 1;

    keys[i].reserve(keysList.size());

    for (const int & key : keysList) keys[i].push_back( key );

    if(keysNotActivated) addKeysListeners();
}
#endif

//-

// A. ofParameterGroup
#ifdef USE_OF_PARAMETER_GROUP

//--------------------------------------------------------------
void ofxPresetsManager::save(int presetIndex, int guiIndex)
{
    TS_START("save1");

    if (guiIndex >= 0 && guiIndex < (int) groups.size())
    {
        ofLogNotice("ofxPresetsManager") << "DONE_save";
        DONE_save = true;

        //-

        std::string n = presetName(groups[guiIndex].getName(), presetIndex);

        ofXml settings;
        ofSerialize(settings, groups[guiIndex]);
        settings.save(path_KitFolder + "/" + n);
    }

    TS_STOP("save1");
}

//--------------------------------------------------------------
void ofxPresetsManager::save(int presetIndex, string guiName)
{
    TS_START(897"save2");
    int guiIndex = getGuiIndex(guiName);

    if (guiIndex >= 0 && guiIndex < (int) groups.size())
    {
        ofLogNotice("ofxPresetsManager") << "DONE_save";
        DONE_save = true;

        //-

        string n = presetName(guiName, presetIndex);

        ofXml settings;
        ofSerialize(settings, groups[guiIndex]);
        settings.save(path_KitFolder + "/" + n);
    }
TS_STOP("save2");
}

//--------------------------------------------------------------
void ofxPresetsManager::load(int presetIndex, int guiIndex)
{
    TS_START("load1");

        if (guiIndex >= 0 && guiIndex < (int) groups.size())
        {
            string str = presetName(groups[guiIndex].getName(), presetIndex);

            ofXml settings;
            settings.load(path_KitFolder + "/" + str);
            ofDeserialize(settings, groups[guiIndex]);

            lastIndices[guiIndex] = presetIndex;

            //-

            ofLogNotice("ofxPresetsManager") << "DONE_load";
            DONE_load = true;//callback
        }

    TS_STOP("load1");
}

//--------------------------------------------------------------
void ofxPresetsManager::load(int presetIndex, string guiName)
{
    TS_START("load2");

        int guiIndex = getGuiIndex(guiName);

        if (guiIndex >= 0 && guiIndex < (int) groups.size())
        {
            string n = presetName(guiName, presetIndex);

            ofXml settings;
            settings.load(path_KitFolder + "/" + n);
            ofDeserialize(settings, groups[guiIndex]);

            lastIndices[guiIndex] = presetIndex;

            //-

            ofLogNotice("ofxPresetsManager") << "DONE_load";
            DONE_load = true;
        }

    TS_STOP("load2");
}

#endif

//-

// B. custom DataGrid class
#ifdef USE_CUSTOM_DATAGRID
//--------------------------------------------------------------
void ofxPresetsManager::save( int presetIndex, int guiIndex )
{
    //TS_START("save");

    if(guiIndex>=0 && guiIndex<(int)grids.size())
    {
        ofLogNotice("ofxPresetsManager") << "DONE_save";
        DONE_save = true;
        // will make in SEQ: get sequencer state before saving in preset manager
        //GRID_getFrom_Sequencer();

        //-

        std::string n = presetName( grids[guiIndex]->getName(), presetIndex);

        ofLogNotice("ofxPresetsManager") << "> save( presetIndex, guiIndex): " << presetIndex <<", "<< guiIndex;
        ofLogNotice("ofxPresetsManager") << "grids[guiIndex]->getName(): " << grids[guiIndex]->getName();
        ofLogNotice("ofxPresetsManager") << "n: " << n;

        grids[guiIndex]->save_JSON(path_KitFolder + "/" + n);
    }

    //TS_STOP("save");
}


//--------------------------------------------------------------
void ofxPresetsManager::save( int presetIndex, string guiName ) {

    //TS_START("save2");

    int guiIndex = getGuiIndex(guiName);

    if(guiIndex>=0 && guiIndex<(int)grids.size())
    {
        ofLogNotice("ofxPresetsManager") << "DONE_save";
        DONE_save = true;
        // will make in SEQ: get sequencer state before saving in preset manager
        //GRID_getFrom_Sequencer();

        //-

        ofLogNotice("ofxPresetsManager") << "> save( presetIndex, guiName): " << presetIndex <<", "<< guiName;
        ofLogNotice("ofxPresetsManager") << "guiIndex = getGuiIndex(guiName): " << guiIndex;

        string n = presetName( guiName, presetIndex);

        grids[guiIndex]->save_JSON( path_KitFolder + "/" + n );
    }

    //TS_STOP("save2");
}

//--------------------------------------------------------------
void ofxPresetsManager::load( int presetIndex, int guiIndex ) {
    if(guiIndex>=0 && guiIndex<(int)grids.size())
    {
        ofLogNotice("ofxPresetsManager") << "> load( presetIndex, guiIndex): " << presetIndex <<", "<< guiIndex;

        string str = presetName( grids[guiIndex]->getName(), presetIndex);
        grids[guiIndex]->load_JSON( path_KitFolder + "/" + str );

        lastIndices[guiIndex] = presetIndex;

        // grids[guiIndex]->dump_grid();

        //-

        ofLogNotice("ofxPresetsManager") << "DONE_load";
        DONE_load = true;
    }
}

//--------------------------------------------------------------
void ofxPresetsManager::load( int presetIndex, string guiName ) {
    int guiIndex = getGuiIndex(guiName);

    if(guiIndex>=0 && guiIndex<(int)grids.size()){

        ofLogNotice("ofxPresetsManager") << "> load( presetIndex, guiName): " << presetIndex <<", "<< guiName;

        string n = presetName( guiName, presetIndex);
        grids[guiIndex]->load_JSON( path_KitFolder + "/" + n );

        lastIndices[guiIndex] = presetIndex;

        //-

        ofLogNotice("ofxPresetsManager") << "DONE_load";
        DONE_load = true;
    }
}
#endif

//-

// A. ofParameterGroup
#ifdef USE_OF_PARAMETER_GROUP

//--------------------------------------------------------------
int ofxPresetsManager::getPresetIndex(int guiIndex) const
{
    if (guiIndex > 0 && guiIndex < (int) groups.size())
    {
        return lastIndices[guiIndex];
    }
    else
    {
        return -1;
    }
}

//--------------------------------------------------------------
int ofxPresetsManager::getPresetIndex(string guiName) const
{
    int guiIndex = getGuiIndex(guiName);

    if (guiIndex > 0 && guiIndex < (int) groups.size())
    {
        return lastIndices[guiIndex];
    }
    else
    {
        return -1;
    }
}

#endif

//-

// B. custom DataGrid class
#ifdef USE_CUSTOM_DATAGRID
//--------------------------------------------------------------
int ofxPresetsManager::getPresetIndex( int guiIndex ) const
{
    if(guiIndex>0 && guiIndex<(int)grids.size()){
        return lastIndices[guiIndex];
    }else{
        return -1;
    }
}

//--------------------------------------------------------------
int ofxPresetsManager::getPresetIndex( string guiName )const
{
    int guiIndex = getGuiIndex(guiName);

    if(guiIndex>0 && guiIndex<(int)grids.size()){
        return lastIndices[guiIndex];
    }else{
        return -1;
    }
}
#endif

//--------------------------------------------------------------
void ofxPresetsManager::setModeKey(int key)
{
    modeKey = key;
}


//--------------------------------------------------------------
void ofxPresetsManager::draw_CLICKER()
{
    // draws some minimalistic graphics to monitor the active preset
    // when graphics are drawn you can also click on them for saving/loading

    if (!lastMouseButtonState && ofGetMousePressed())
    {
        mousePressed(ofGetMouseX(), ofGetMouseY());
    }
    lastMouseButtonState = ofGetMousePressed();

    ofPushStyle();
    ofPushMatrix();
    ofTranslate(clicker_Pos);

    //-

    //back box
    ofFill();
    ofSetColor(0, 128);
    ofDrawRectangle(0, 0, clicker_cellSize * (keys[0].size() + 1), clicker_cellSize);

    //-

    //trigger boxes
    ofNoFill();
    ofSetColor(ofColor::white);

    for (size_t i = 0; i < keys.size(); ++i)
    {
        size_t k = 0;
        for (; k < keys[i].size(); ++k)
        {
            ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, clicker_cellSize, clicker_cellSize);

            //ofDrawBitmapString( ofToString((char)keys[i][k]), clicker_cellSize*k+8, clicker_cellSize*i+18 );
            myFont.drawString(ofToString((char) keys[i][k]),
                clicker_cellSize * k + 0.5 * clicker_cellSize - 0.25 * sizeTTF,
                clicker_cellSize * i + 0.5 * clicker_cellSize + 0.5 * sizeTTF);

            if (lastIndices[i] == k)
                ofDrawRectangle(clicker_cellSize * k + 4, clicker_cellSize * i + 4, clicker_cellSize - 8, clicker_cellSize - 8);
        }
        for (; k < presets[i]; ++k)
        {
            ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, clicker_cellSize, clicker_cellSize);
            if (lastIndices[i] == k)
                ofDrawRectangle(clicker_cellSize * k + 4, clicker_cellSize * i + 4, clicker_cellSize - 8, clicker_cellSize - 8);
        }

        // save button
        ofDrawRectangle(clicker_cellSize * k, clicker_cellSize * i, clicker_cellSize, clicker_cellSize);
        myFont.drawString("SAVE",
            clicker_cellSize * k + 0.5 * clicker_cellSize - 1.30 * sizeTTF,
            clicker_cellSize * i + 0.5 * clicker_cellSize + 0.5 * sizeTTF);
        k++;

        //-

        // kit name

        //            // A. ofParameterGroup
        //            #ifdef USE_OF_PARAMETER_GROUP
        //            ofDrawBitmapString( groups[i].getName(), clicker_cellSize*k+8, clicker_cellSize*i+18 );
        //            #endif
        //
        //            // B. custom DataGrid class
        //            #ifdef USE_CUSTOM_DATAGRID
        //            ofDrawBitmapString( grids[i]->getName(), clicker_cellSize*k+8, clicker_cellSize*i+18 );
        //            #endif

        //-
    }

    //-

    //help text
    if (debugClicker && ENABLE_shortcuts)
    {
        string info;

        info = "MOUSE-CLICK OR KEYS [1-8] TO LOAD PRESET\n";
        info += "HOLD [CTRL] TO SAVE/COPY PRESET\n";

        //info = "MOUSE:\n";
        //info += "CLICK TO LOAD PRESET\n";
        //info += "[CTRL]+CLICK TO SAVE/COPY PRESET\n";
        //info += "KEYS:\n";
        //info += "[1-8] TO LOAD PRESET\n";
        //info += "[CTRL]+[1-8] TO SAVE/COPY PRESET\n";

        //double font to improve different background colors
        float gap = 1.0;
        ofSetColor(ofColor::black);
        myFont.drawString(info, 0 + gap, clicker_cellSize + 15 + gap);

        ofSetColor(ofColor::white);
        myFont.drawString(info, 0, clicker_cellSize + 15);
    }

    ofPopStyle();
    ofPopMatrix();
}

#pragma mark - OF LISTENERS

//----------------------------------------------------------------
void ofxPresetsManager::keyPressed(ofKeyEventArgs &eventArgs)
{
    if (bKeys && ENABLE_shortcuts)
    {
        const int &key = eventArgs.key;

        //mode key for saving with mouse or trigger keys
        if (key == modeKey)
        {
            bKeySave = true;
            ofLogVerbose("ofxPresetsManager") << "modeKey TRUE" << endl;
            return;
        }

        //presets triggers
        for (int i = 0; i < keys.size(); ++i)
        {
            for (int k = 0; k < keys[i].size(); ++k)
            {
                if (key == keys[i][k])
                {
                    if (bKeySave)
                    {
                        save(k, i);
                    }
                    else
                    {
                        //if (bDelayedLoading)
                        //{
                        //    // newIndices[i] = k;
                        //    // ofLogNotice("ofxPresetsManager") << "newIndices[i] = k;" <<  k << ", " << i;
                        //    // PRESET_selected = 1 + k;
                        //}
                        //else
                        //{
                        PRESET_selected = 1 + k;//first row/gui only
                        //}
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
    if (eventArgs.key == modeKey && ENABLE_shortcuts)
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

    if (xIndex != -1 && yIndex != -1)
        ofLogNotice("ofxPresetsManager") << "mousePressed xIndex, yIndex: " << xIndex << ", " << yIndex;

    //-

    // A. ofParameterGroup

#ifdef USE_OF_PARAMETER_GROUP

    // 1. key presets buttons
    if (yIndex >= 0 && yIndex < (int) groups.size())
    {
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
                //    //newIndices[yIndex] = xIndex;
                //}
                //else
                //{
                PRESET_selected = 1 + xIndex;
                //    //load( xIndex, yIndex);
                //}
            }
        }

            // 2. last button (save button)
        else if (xIndex == presets[yIndex])
        {
            ofLogNotice("ofxPresetsManager") << "saveButton: ( lastIndices[yIndex], yIndex ): "
                                             << lastIndices[yIndex]
                                             << ", " << yIndex;

            // save
            //save( lastIndices[yIndex], yIndex );
            doSave(lastIndices[yIndex]);
        }
    }

#endif

    //-

    // B. custom DataGrid class

#ifdef USE_CUSTOM_DATAGRID

    if( yIndex >=0 &&  yIndex < (int)grids.size() )
    {
        // row buttons, preset selctors
        if(xIndex>=0 && xIndex< presets[yIndex] ){
            //load
            if(bDelayedLoading)
            {
//                newIndices[yIndex] = xIndex;
//                ofLogNotice("ofxPresetsManager") << "newIndices[yIndex] = xIndex:" <<  yIndex << " = " << xIndex;
            }
            else
            {
//                load( xIndex, yIndex);
//                ofLogNotice("ofxPresetsManager") << "load( xIndex, yIndex):" <<  xIndex << ", " << yIndex;

                PRESET_selected = 1 + xIndex;
            }
        }

        // last button (save button)
        else if( xIndex == presets[yIndex])
        {
            ofLogNotice("ofxPresetsManager") << "saveButton: ( lastIndices[yIndex], yIndex ): " <<  lastIndices[yIndex] << ", " << yIndex;

            // save

//            save( lastIndices[yIndex], yIndex );

//            //save( k, i);//this is from keyPressed
////            PRESET_selected = 1 + yIndex;

            doSave( lastIndices[yIndex] );
        }
    }

#endif

    //-
}

#pragma mark - ENGINE

//--------------------------------------------------------------
void ofxPresetsManager::doCloneRight(int patternNum)
{
    ofLogNotice("ofxPresetsManager") << "doCloneRight: patternNum: " << patternNum;
    for (int i = patternNum + 1; i < num_presets; i++)
    {
        save(i, 0);//only 1 row (gui) / data content
    }
}

//--------------------------------------------------------------
void ofxPresetsManager::doSave(int patternNum)
{
    ofLogNotice("ofxPresetsManager") << "doSave: patternNum: " << patternNum;
    save(patternNum, 0);//only 1 row (gui) / data content
}

////--------------------------------------------------------------
//void ofxPresetsManager::gui_loadFromFile(const std::string &filename, ofAbstractParameter &parameter)
//{
//    ofXml xml;
//    xml.load(filename);
//    ofDeserialize(xml, parameter);
//}
//
////--------------------------------------------------------------
//void ofxPresetsManager::gui_saveToFile(const std::string &filename, ofAbstractParameter &parameter)
//{
//    ofXml xml;
//    ofSerialize(xml, parameter);
//    xml.save(filename);
//}
//
////--------------------------------------------------------------
//void ofxPresetsManager::gui_SaveAsSettings()
//{
//
//}

//--------------------------------------------------------------
static void ShowHelpMarker(const char *desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//--------------------------------------------------------------
void ofxPresetsManager::preset_save(string name)//without xml extension
{
    ofLogNotice("ofxPresetsManager") << "preset_save: " << name << ".xml";

    ofXml settings;

    //TODO
    //ofSerialize(settings, params_FONT_ANIMATOR);
    //ofSerialize(settings, groups[PRESET_selected - 1]);
    ofSerialize(settings, groups[0]);

    settings.save(path_PresetsFolder + "/" + name + ".xml");
}

//--------------------------------------------------------------
void ofxPresetsManager::preset_load(string name)//without xml extension
{
    ofLogNotice("ofxPresetsManager") << "preset_load: " << name << ".xml";

    ofXml settings;
    settings.load(path_PresetsFolder + "/" + name + ".xml");

    //TODO
    ofDeserialize(settings, groups[0]);
    //ofDeserialize(settings, params_FONT_ANIMATOR);
    //ofDeserialize(settings, groups[PRESET_selected - 1]);

    //-

    ofLogNotice("ofxPresetsManager") << "DONE_load";
    DONE_load = true;
}

//--------------------------------------------------------------
void ofxPresetsManager::preset_filesRefresh()
{
    ofDirectory dataDirectory(ofToDataPath(path_PresetsFolder, true));
    //ofDirectory dataDirectory(ofToDataPath("user_kits/presets", true));

    ofLogNotice("ofxPresetsManager") << "preset_filesRefresh path:" << path_PresetsFolder;

    // clear files and filenames vectors
    files.clear();
    fileNames.clear();

    // load all folder files in one call
    files = dataDirectory.getFiles();

    for (size_t i = 0; i < files.size(); i++)
    {
        fileNames.push_back(files[i].getBaseName());
        ofLogNotice("ofxPresetsManager") << "file [" << i << "] " << files[i].getBaseName();
    }

    //-

    //TODO
    //void to go to 1st...

    // 1. load same position preset
    // if preset is deleted goes to nextone..
    // should check names because sorting changes..
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

    //// 2. always goes to 1st preset 0
    ////that's because saving re sort the files
    ////and we don't know the position of last saves preset..
    //if (fileNames.size() > 0)
    //{
    //    currentFile = 0;
    //    PRESET_name = fileNames[currentFile];
    //    preset_load(PRESET_name);
    //}
    //else
    //{
    //    ofLogError("ofxColorManager") << "NOT FOUND ANY FILE PRESET!";
    //}
}

//--------------------------------------------------------------
void ofxPresetsManager::loadPreset(int p)
{
    ofLogNotice("ofxPresetsManager") << "> API > LOAD PRESET " << ofToString(p);
    if (PRESET_selected > 0 && PRESET_selected <= num_presets)
        PRESET_selected = p;
}

//--------------------------------------------------------------
void ofxPresetsManager::setDelayedLoading(bool active)
{
    bDelayedLoading = active;
}

//--------------------------------------------------------------
void ofxPresetsManager::delayedLoad(int presetIndex, int guiIndex)
{
    //-

    // A. ofParameterGroup

#ifdef USE_OF_PARAMETER_GROUP
    if (guiIndex >= 0 && guiIndex < (int) groups.size())
    {
        newIndices[guiIndex] = presetIndex;
    }
#endif

    //-

    // B. custom DataGrid class

#ifdef USE_CUSTOM_DATAGRID
    if(guiIndex>=0 && guiIndex<(int)grids.size()){
        newIndices[guiIndex] = presetIndex;
    }
#endif

    //-
}

//--------------------------------------------------------------
void ofxPresetsManager::delayedLoad(int presetIndex, string guiName)
{
    int guiIndex = getGuiIndex(guiName);

    //-

    // A. ofParameterGroup

#ifdef USE_OF_PARAMETER_GROUP
    if (guiIndex >= 0 && guiIndex < (int) groups.size())
    {
        newIndices[guiIndex] = presetIndex;
    }
#endif

    //-

    // B. custom DataGrid class

#ifdef USE_CUSTOM_DATAGRID
    if(guiIndex>=0 && guiIndex<(int)grids.size()){
        newIndices[guiIndex] = presetIndex;
    }
#endif
}

void ofxPresetsManager::delayedUpdate()
{

    //-

    // A. ofParameterGroup

#ifdef USE_OF_PARAMETER_GROUP
    for (size_t i = 0; i < groups.size(); ++i)
    {
        if (newIndices[i] != lastIndices[i])
        {
            load(newIndices[i], i);
        }
    }
#endif

    //-

    // B. custom DataGrid class

#ifdef USE_CUSTOM_DATAGRID
    for(size_t i=0; i<grids.size(); ++i){
        if(newIndices[i]!=lastIndices[i]){
            load( newIndices[i], i);
        }
    }
#endif

}

//--------------------------------------------------------------
void ofxPresetsManager::toggleKeysControl(bool active)
{
    bKeys = active;
}

#pragma mark - CALLBACKS

//--------------------------------------------------------------
void ofxPresetsManager::Changed_Params(ofAbstractParameter &e)
{
    string WIDGET = e.getName();

    ofLogNotice("ofxPresetsManager") << "Changed_Params '" << WIDGET << "': " << e;

    if (WIDGET == "SAVE" && bSave)
    {
        ofLogNotice("ofxPresetsManager") << "SAVE: " << e;
        bSave = false;
        doSave(PRESET_selected - 1);
    }
    else if (WIDGET == "ENABLE SHORTCUTS [TAB]")
    {

    }
    else if (WIDGET == "CLONE >" && bCloneRight)
    {
        ofLogNotice("ofxPresetsManager") << "CLONE >: " << e;
        bCloneRight = false;
        doCloneRight(PRESET_selected - 1);
    }
        //TODO
        //BUG
        //we try to load anyway?
    else if (WIDGET == "PRESETS" && (PRESET_selected == selected_PRE))//TODO:would be nice to retrig same preset
    {
        ofLogNotice("ofxPresetsManager") << "Changed PRESET PRESET_selected: " << e;
        ofLogNotice("ofxPresetsManager") << "not changed but re trig PRESET_selected preset";
        DONE_load = true;//callback
    }
    else if (WIDGET == "PRESETS" && (PRESET_selected != selected_PRE))
    {
        ofLogNotice("ofxPresetsManager") << "Changed PRESET PRESET_selected: " << e;

        //load
        //if (bDelayedLoading)//TODO: not implemented
        //{
        //    ofLogNotice("ofxPresetsManager") << "bDelayedLoading: " << bDelayedLoading;
        //
        //    //byKey
        //    //newIndices[i] = k;
        //    //ofLogNotice("ofxPresetsManager") << "newIndices[i] = k;" <<  k << ", " << i;
        //
        //    //byMousePressed
        //    //ofLogNotice("ofxPresetsManager") << "newIndices[yIndex] = xIndex:" <<  yIndex << " = " << xIndex;
        //}
        //
        //else
        //{
        //    //byKey
        //    //load( k, i );
        //    //ofLogNotice("ofxPresetsManager") << "load( k, i ):" <<  k << ", " << i;
        //
        //    //byMousePressed
        //    //load( xIndex, yIndex);
        //    //ofLogNotice("ofxPresetsManager") << "load( xIndex, yIndex):" <<  xIndex << ", " << yIndex;

        //-

        if (autoSave)
        {
            DONE_save = true;//callback
            save(selected_PRE - 1, 0);
        }

        //-

        selected_PRE = PRESET_selected;
        int xIndex = PRESET_selected - 1;
        int yIndex = 0;//this should handle when using multiple kits together. yIndex = ?

        //-

        if (autoLoad)
        {
            load(xIndex, yIndex);
            ofLogNotice("ofxPresetsManager") << "load( xIndex, yIndex): " << xIndex << ", " << yIndex;

            DONE_load = true;//callback
        }
        else
        {
            lastIndices[yIndex] = xIndex;//?this is to move clicker selector
            ofLogNotice("ofxPresetsManager") << "lastIndices[yIndex]: " << xIndex;
            ofLogNotice("ofxPresetsManager") << "autoLoad: " << autoLoad;
        }
        //}
    }
}

#pragma mark - SETTINGS

//--------------------------------------------------------------
void ofxPresetsManager::load_ControlSettings()
{
    ofLogNotice("ofxPresetsManager") << "load_ControlSettings " << pathControl;

    ofXml settings;
    settings.load(pathControl);
    ofDeserialize(settings, params);

    cout << "load_ControlSettings > PRESET_selected: " << PRESET_selected << endl;
}

//--------------------------------------------------------------
void ofxPresetsManager::save_ControlSettings()
{
    ofLogNotice("ofxPresetsManager") << "save_ControlSettings " << pathControl;

    ofXml settings;
    ofSerialize(settings, params);
    settings.save(pathControl);
}

//--------------------------------------------------------------
void ofxPresetsManager::set_pathKit_Folder(string folder)
{
    path_KitFolder = folder;
}

//--------------------------------------------------------------
void ofxPresetsManager::set_path_PresetsFolder(string folder)
{
    path_PresetsFolder = folder;
}


#pragma mark - GUI


//--------------------------------------------------------------
bool ofxPresetsManager::gui_draw()
{
    static bool SHOW_About = false;
    static bool SHOW_Gui = true;

    auto mainSettings = ofxImGui::Settings();
    mainSettings.windowPos = guiPos;
    mainSettings.windowSize = guiSize;

    gui.begin();
    {
        if (SHOW_Gui)
        {
            if (ofxImGui::BeginWindow("PRESET MANAGER", mainSettings, false))
                //if (ofxImGui::BeginWindow("PRESET MANAGER", mainSettings,
                //    ImGuiWindowFlags_NoCollapse |
                //    ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::PushItemWidth(100);

                ofxImGui::AddParameter(ENABLE_shortcuts);

                //1. files browser
                gui_imGui_PresetManager();

                ImGui::Separator();

                //-

                //2.1 live presets
                ofxImGui::AddGroup(this->params_Favorites, mainSettings);

                //2.2 advanced
                if (ImGui::CollapsingHeader("ADVANCED"))
                {
                    ofxImGui::AddGroup(this->params_Options, mainSettings);
                    ofxImGui::AddGroup(this->params_Gui, mainSettings);
                    ofxImGui::AddGroup(this->params_Tools, mainSettings);
                }

                ImGui::Separator();

                ImGui::PopItemWidth();
                ofxImGui::EndWindow(mainSettings);
            }
        }

        //-

        if (SHOW_menu)
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    gui_draw_MenuFile();
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help"))
                {
                    ImGui::MenuItem("About ofxPresetsManager", NULL, &SHOW_About);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }

        if (SHOW_About)
        {
            ImGui::Begin("About ofxPresetsManager", &SHOW_About,
                ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("ofParameterGroup presets manager");
            ImGui::Separator();
            ImGui::Text("by MoebiusSurfing.");
            ImGui::End();
        }
    }

    gui.end();
    return mainSettings.mouseOverGui;
}

//--------------------------------------------------------------
void ofxPresetsManager::gui_draw_MenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            gui_draw_MenuFile();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

//--------------------------------------------------------------
void ofxPresetsManager::gui_draw_MenuFile()
{
    if (ImGui::MenuItem("New"))
    {
    }
    if (ImGui::MenuItem("Open", "l"))
    {
        //gui_loadFromFile("settings.xml", params);
    }
    if (ImGui::MenuItem("Save", "s"))
    {
        //gui_saveToFile("settings.xml", params);
    }
    if (ImGui::MenuItem("Save As.."))
    {
        //gui_SaveAsSettings();
    }
    ImGui::Separator();
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "ESQ"))
    {
    }
}

//TODO: debug
//--------------------------------------------------------------
void ofxPresetsManager::groupDebug(ofParameterGroup &group)
{
    string str;
    int x = 200;
    int y = 100;
    int pad = 20;
    int i = 0;
    int ig = 0;

    for (auto parameter : group)
    {
        // Group.
        auto parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameter);
        if (parameterGroup)
        {
            // Recurse through contents.
            //ofxImGui::AddGroup(*parameterGroup, settings);
            groupDebug(*parameterGroup);

            ig++;
            str = "group : [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x + ig * 300, y + pad * i++);
            continue;
        }

        // Parameter, try everything we know how to handle.
#if OF_VERSION_MINOR >= 10
        auto parameterVec2f = std::dynamic_pointer_cast<ofParameter<glm::vec2>>(parameter);
        if (parameterVec2f)
        {
            //ofxImGui::AddParameter(*parameterVec2f);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
        auto parameterVec3f = std::dynamic_pointer_cast<ofParameter<glm::vec3>>(parameter);
        if (parameterVec3f)
        {
            //ofxImGui::AddParameter(*parameterVec3f);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
        auto parameterVec4f = std::dynamic_pointer_cast<ofParameter<glm::vec4>>(parameter);
        if (parameterVec4f)
        {
            //ofxImGui::AddParameter(*parameterVec4f);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
#endif
        auto parameterOfVec2f = std::dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
        if (parameterOfVec2f)
        {
            //ofxImGui::AddParameter(*parameterOfVec2f);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
        auto parameterOfVec3f = std::dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
        if (parameterOfVec3f)
        {
            //ofxImGui::AddParameter(*parameterOfVec3f);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
        auto parameterOfVec4f = std::dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
        if (parameterOfVec4f)
        {
            //ofxImGui::AddParameter(*parameterOfVec4f);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
        auto parameterFloatColor = std::dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
        if (parameterFloatColor)
        {
            //ofxImGui::AddParameter(*parameterFloatColor);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
        auto parameterFloat = std::dynamic_pointer_cast<ofParameter<float>>(parameter);
        if (parameterFloat)
        {
            //ofxImGui::AddParameter(*parameterFloat);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
        auto parameterInt = std::dynamic_pointer_cast<ofParameter<int>>(parameter);
        if (parameterInt)
        {
            //ofxImGui::AddParameter(*parameterInt);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }
        auto parameterBool = std::dynamic_pointer_cast<ofParameter<bool>>(parameter);
        if (parameterBool)
        {
            //ofxImGui::AddParameter(*parameterBool);
            str = "parameter : [" + ofToString(ig) + "] [" + ofToString(i) + "] " + ofToString(parameter->getName());
            ofDrawBitmapStringHighlight(str, x, y + pad * i++);
            continue;
        }

        ofLogWarning(__FUNCTION__) << "Could not create GUI element for parameter " << parameter->getName();
    }
}

//--------------------------------------------------------------
void ofxPresetsManager::gui_imGui_PresetManager()
{
    auto mainSettings = ofxImGui::Settings();

    //if (ofxImGui::BeginWindow("PRESET MANAGER", mainSettings, false))
    //{
    if (ofxImGui::BeginTree("BROWSER", mainSettings))
    {
        //-

        ImGui::Text("NAME:");
        string textInput_temp = PRESET_name;

        // loaded string into char array
        char tab2[32];
        strncpy(tab2, textInput_temp.c_str(), sizeof(tab2));
        tab2[sizeof(tab2) - 1] = 0;

        if (ImGui::InputText("", tab2, IM_ARRAYSIZE(tab2)))
        {
            cout << "InputText:" << ofToString(tab2) << endl;

            textInput_temp = ofToString(tab2);
            cout << "textInput_temp:" << textInput_temp << endl;

            if (MODE_newPreset)
                MODE_newPreset = false;
        }

        //--

        // arrow buttons
        static int counter = currentFile;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

        ImGui::PushButtonRepeat(true);

        // prev
        if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        {
            if (counter > 0)
            {
                counter--;
                currentFile = counter;
                if (currentFile < files.size())
                {
                    PRESET_name = fileNames[currentFile];
                    ofLogNotice() << "ARROW: PRESET_name: [" + ofToString(currentFile) + "] " << PRESET_name;
                    preset_load(PRESET_name);
                }

                if (MODE_newPreset)
                    MODE_newPreset = false;
            }
        }

        // next
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        {
            if (counter < files.size() - 1)
            {
                counter++;
                currentFile = counter;
                if (currentFile < files.size())
                {
                    PRESET_name = fileNames[currentFile];
                    ofLogNotice() << "ARROW: PRESET_name: [" + ofToString(currentFile) + "] " << PRESET_name;
                    preset_load(PRESET_name);
                }
            }

            if (MODE_newPreset)
                MODE_newPreset = false;
        }

        ImGui::PopButtonRepeat();

        //preview current preset number to total
        int numPalettes = fileNames.size() - 1;
        ImGui::SameLine();
        ImGui::Text("%d/%d", currentFile, numPalettes);

        //-

        // scrollable list
        if (!fileNames.empty())
        {
            int currentFileIndex = currentFile;
            if (ofxImGui::VectorCombo(" ", &currentFileIndex, fileNames))
            {
                ofLog() << "currentFileIndex: " << ofToString(currentFileIndex);
                if (currentFileIndex < fileNames.size())
                {
                    currentFile = currentFileIndex;
                    PRESET_name = fileNames[currentFile];
                    ofLog() << "PRESET_name: " << PRESET_name;
                    preset_load(PRESET_name);
                }

                if (MODE_newPreset)
                    MODE_newPreset = false;
            }
        }

        //-

        // 2. presets

        ImGui::Text("PRESETS");

        if (ImGui::Button("SAVE"))
        {
            cout << "SAVE" << endl;

            //TODO
            //should re load by same name and get what position on vector
            //is to reload current preset number
            //            textInput_temp = ofToString(tab2);
            //            cout << "textInput_temp:" << textInput_temp << endl;

            PRESET_name = textInput_temp;
            cout << "PRESET_name: " << PRESET_name << endl;

            preset_save(PRESET_name);
            preset_filesRefresh();
        }

        ImGui::SameLine();
        if (ImGui::Button("UPDATE"))
        {
            cout << "UPDATE" << endl;

            PRESET_name = textInput_temp;
            cout << "PRESET_name: " << PRESET_name << endl;

            //delete old file
            files[currentFile].remove();
            // preset_filesRefresh();

            //save new one
            preset_save(PRESET_name);
            preset_filesRefresh();
        }

        ImGui::SameLine();
        if (ImGui::Button("LOAD"))//not required..
        {
            cout << "LOAD" << endl;
            cout << "PRESET_name: " << PRESET_name << endl;
            preset_load(PRESET_name);
        }

        ImGui::SameLine();
        if (ImGui::Button("DELETE"))//current preset
        {
            cout << "DELETE" << endl;

            files[currentFile].remove();
            preset_filesRefresh();

            //string str = fileNames[currentFile];
            //cout << "DELETE:"<<str<<endl;
            //dir.listDir("user_kits/presets");
            //dir.allowExt("jpg");
            //dir.allowExt("png");
            //dir.sort();
        }

        //ImGui::SameLine();
        //if (ImGui::Button("REFRESH"))//current preset
        //{
        //    cout << "REFRESH" << endl;
        //    preset_filesRefresh();
        //}

        //-

        //TODO
        MODE_newPreset = true;
        if (MODE_newPreset)
        {
            ImGui::Separator();
            ImGui::Text("NEW PRESET!");

            // loaded string into char array
            char tab[32];
            strncpy(tab, textInput_New.c_str(), sizeof(tab));
            tab[sizeof(tab) - 1] = 0;

            if (ImGui::InputText("", tab, IM_ARRAYSIZE(tab)))
            {
                cout << "InputText [tab]:" << ofToString(tab) << endl;
                textInput_New = ofToString(tab);
                cout << "textInput_New:" << textInput_New << endl;
            }

            // WORKFLOW: when it's editing a new preset..

            ImGui::PushID(1);
            int n = 30;
            float a = ofMap(ofGetFrameNum() % n, 0, n, 0.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0.5, 0.0f, 0.5f, a));

            if (ImGui::Button("SAVE NEW"))
            {
                MODE_newPreset = false;
                cout << "textInput_New: " << textInput_New << endl;
                preset_save(textInput_New);
                preset_filesRefresh();
            }

            ImGui::PopStyleColor(1);
            ImGui::PopID();
        }

        //-

        ofxImGui::EndTree(mainSettings);
    }
    //ofxImGui::EndWindow(mainSettings);
}

//--------------------------------------------------------------
void ofxPresetsManager::gui_imGui_theme()
{
    // must be done after setup the gui

    ImGuiStyle *style = &ImGui::GetStyle();

    // my dark theme
    //    ImVec4* colors = ImGui::GetStyle().Colors;
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

    //    // 2. defaut dark theme exported
    //    style->WindowMinSize = ImVec2(160, 65);
    //    style->FramePadding = ImVec2(4, 2);
    //    style->ItemSpacing = ImVec2(6, 2);
    //    style->ItemInnerSpacing = ImVec2(6, 4);
    //    style->Alpha = 1.0f;
    //    style->WindowRounding = 0.0f;
    //    style->FrameRounding = 0.0f;
    //    style->IndentSpacing = 6.0f;
    //    style->ItemInnerSpacing = ImVec2(2, 4);
    //    style->ColumnsMinSpacing = 50.0f;
    //    style->GrabMinSize = 14.0f;
    //    style->GrabRounding = 0.0f;
    //    style->ScrollbarSize = 12.0f;
    //    style->ScrollbarRounding = 0.0f;
}