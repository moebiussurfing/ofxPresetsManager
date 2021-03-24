#include "ofApp.h"

static std::string moduleName = "of_client";

const static std::string SERVER_IP_ADDRESS = "127.0.0.1";

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowTitle("OF CLIENT");
	ofSetWindowPosition(0, 25);

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(60);
	ofSetCircleResolution(100);
	font.load("assets/fonts/telegrama_render.otf", 11);

	paramClient.setup(params, SERVER_IP_ADDRESS, 12000, 12001);
	modelLoadedEventListener = paramClient.modelLoadedEvent.newListener([this]() {
		buildGui();
	});

	buildGui();

	gui.setup();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	/* gui */

	gui.begin();
	ofxImGui::Settings s;
	static bool auto_resize = true;
	ImGuiWindowFlags flagsw;
	flagsw = auto_resize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None;
	if (ofxImGui::BeginWindow("OF CLIENT", s, flagsw))
	{
		ImVec2 bb{ ImGui::GetContentRegionAvail().x, 100 };
		
		if (ImGui::Button("Connect", bb)) 
		{
			paramClient.connect();
		}

		ofxImGui::AddGroup(params, s);
	}
	ofxImGui::EndWindow(s);
	gui.end();
}

//--------------------------------------------------------------
void ofApp::buildGui()
{
	presetsManager.clear();
	presetsManager.add(params, { '0', '1', '2', '3', '4', '5' });
	presetsManager.setup();
}