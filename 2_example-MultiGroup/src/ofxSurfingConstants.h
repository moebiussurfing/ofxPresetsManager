#pragma once

//---------
//
// defines (modes) are here "to share between addons" in one place
//
//---------


//----

#define USE_IM_GUI
#define USE_ofxGui

//----

// file format to use as default xml/json. mainly for ofParameterGroup de/serializers

#define USE_JSON	// A		
#ifdef USE_XML
#undef USE_XML
#endif

//#define USE_XML	// B
//#ifdef USE_JSON
//#undef USE_JSON
//#endif

//----
