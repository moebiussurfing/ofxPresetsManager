#pragma once

#include "ofMain.h"

//tap tempo engine

class BpmTapTempo
{

public:
	BpmTapTempo() {};
	~BpmTapTempo() {};

public:
	ofParameterGroup params{ "Tap Tempo Engine" };
	ofParameter<float> bpm;
	bool isRunning() {
		return bTapRunning;
	}
	int getCountTap() {
		return tapCount;
	}
	float getBpm() {
		return bpm.get();
	}
	bool isUpdatedBpm() {
		if (bUpdatedBpm) {
			bUpdatedBpm = false;
			return true;
		}
		else return false;
	}

private:
	ofParameter<bool> BangTapTempo;
	ofEventListener listener_BangTapTempo;
	ofParameter<bool> ENABLE_SoundTapTempo;//enable sound ticks
	vector<int> tapIntervals;
	int tapCount, lastTime, avgBarMillis;
	float bpmMeasured;
	bool bTapRunning;
	ofSoundPlayer tapTic;
	ofSoundPlayer tapTac;
	ofSoundPlayer tapBell;
	string pathSounds = "assets/sounds/";
	bool bUpdatedBpm = false;

public:
	void setPathSounds(string path) {
		pathSounds = path;
	}

public:
	//--------------------------------------------------------------
	void setup() {
		ENABLE_SoundTapTempo.set("Enable Sound Tick", true);
		BangTapTempo.set("Bang TapTempo", false);
		BangTapTempo.setSerializable(false);
		bpm.set("BPM TapTempo", 120, 10, 400);
		params.add(ENABLE_SoundTapTempo);
		params.add(BangTapTempo);
		params.add(bpm);

		bTapRunning = false;
		tapCount = 0;
		tapIntervals.clear();

		tapTic.load(pathSounds + "click1.wav");
		tapTic.setVolume(1.0f);
		tapTic.setMultiPlay(false);
		tapTac.load(pathSounds + "click2.wav");
		tapTac.setVolume(0.25f);
		tapTac.setMultiPlay(false);
		tapBell.load(pathSounds + "tapBell.wav");
		tapBell.setVolume(1.0f);
		tapBell.setMultiPlay(false);

		//custom callback for done load
		listener_BangTapTempo = BangTapTempo.newListener([this](bool &)
			{
				this->Changed_BangTapTempo();
			});
	}

	//--------------------------------------------------------------
	void update() {
		if (bTapRunning) {
			int time = ofGetElapsedTimeMillis();

			//timeout
			if (tapIntervals.size() > 0 && (time - lastTime > 3000))
			{
				ofLogNotice(__FUNCTION__) << "TAP TEMPO : TIMEOUT";
				ofLogVerbose(__FUNCTION__) << "Clear measures";
				tapIntervals.clear();
				tapCount = 0;
				bTapRunning = false;
			}

			string str = "TAP TEMPO - " + ofToString(ofMap(tapCount, 1, 4, 4, 1)) + "...";
			ofLogVerbose(__FUNCTION__) << str;
		}
	}

	//--------------------------------------------------------------
	void bang()
	{
		BangTapTempo = true;
	}

	//--------------------------------------------------------------
	void trigTapTempo()
	{
		bTapRunning = true;
		int time = ofGetElapsedTimeMillis();
		tapCount++;
		ofLogNotice(__FUNCTION__) << "> TAP < : " << tapCount;

		if (ENABLE_SoundTapTempo) {
			if (tapCount != 4) tapTac.play();
			else tapBell.play();
		}

		tapIntervals.push_back(time - lastTime);
		lastTime = time;

		if (tapCount > 3)//4th tap
		{
			tapIntervals.erase(tapIntervals.begin());
			avgBarMillis = accumulate(tapIntervals.begin(), tapIntervals.end(), 0) / tapIntervals.size();
			if (avgBarMillis == 0)
			{
				avgBarMillis = 1000;
				ofLogError(__FUNCTION__) << "Avoid divide by 0!";
				ofLogError(__FUNCTION__) << "avgBarMillis: " << ofToString(avgBarMillis);
			}
			bpmMeasured = 60 * 1000 / (float)avgBarMillis;
			ofLogNotice(__FUNCTION__) << "> TAP < : NEW BPM Tap : " << bpmMeasured;

			tapIntervals.clear();
			tapCount = 0;
			bTapRunning = false;

			//set obtained bpm
			bpm = bpmMeasured;
			bUpdatedBpm = true;
		}
		else if (tapCount > 1)
		{
			//temp update to last interval...
			float val = (float)tapIntervals[tapIntervals.size() - 1];
			if (val == 0)
			{
				val = 1000;
				ofLogError(__FUNCTION__) << "Avoid divide by 0!";
				ofLogError(__FUNCTION__) << "Force value: " << ofToString(val);
			}
			bpmMeasured = 60 * 1000 / val;
			ofLogNotice(__FUNCTION__) << "> TAP < : NEW BPM Tap : " << bpmMeasured;

			//set obtained bpm
			bpm = bpmMeasured;
			bUpdatedBpm = true;
		}
	}
private:
	//--------------------------------------------------------------
	void Changed_BangTapTempo() {
		ofLogWarning(__FUNCTION__);
		if (BangTapTempo) {
			BangTapTempo = false;

			//if (ENABLE_SoundTapTempo && !bTapRunning) tapTic.play();

			trigTapTempo();
		}
	}
};