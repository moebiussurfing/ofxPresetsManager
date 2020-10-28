#pragma once

#include "ofImage.h"
#include "ofPixels.h"
#include "ofTexture.h"

#if defined(TARGET_OPENGLES)
#include "EngineOpenGLES.h"
#elif defined (OF_TARGET_API_VULKAN)
#include "EngineVk.h"
#else
#include "EngineGLFW.h"
#endif

#include "DefaultTheme.h"

namespace ofxImGui
{
	class Gui
	{
	public:
		Gui();
		~Gui();

    void enableDocking(); // must be called before setup

		void setup(BaseTheme* theme = nullptr, bool autoDraw = true);
    
		void exit();

		void begin();
		void end();
    
		void draw();

    // must be called before setup
    void SetDefaultFont(int indexAtlasFont);
    int addFont(const std::string & fontPath, float fontSize = 13.0f);

		void setTheme(BaseTheme* theme);

		GLuint loadImage(ofImage& image);
		GLuint loadImage(const std::string& imagePath);

		GLuint loadPixels(const std::string& imagePath);
		GLuint loadPixels(ofPixels& pixels);

		GLuint loadTexture(const std::string& imagePath);
		GLuint loadTexture(ofTexture& texture, const std::string& imagePath);

	private:        
#if defined(TARGET_OPENGLES)
        EngineOpenGLES engine;
#elif defined (OF_TARGET_API_VULKAN) 
        EngineVk engine;
#else
        EngineGLFW engine;
#endif

    void beginDocking();
    void endDocking();
        
		float lastTime;
		bool autoDraw;
    bool dockingEnabled = false;

		BaseTheme* theme;

		std::vector<ofTexture*> loadedTextures;

		ImGuiContext* context;
	};
}
