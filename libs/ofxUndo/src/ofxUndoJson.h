/*
 The MIT License (MIT)
 
 Copyright (c) 2018 nariakiiwatani
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "ofxUndoState.h"

namespace ofx { namespace undo {
template<typename T>
class Json : public State<ofJson>, public T
{
public:
#ifndef OFXUNDOJSON_NOT_USE_OFXJSONUTILS
	ofJson createUndo() const { return ofxJsonUtils::convert((T&)*this); }
	void loadUndo(const ofJson &json) { ofxJsonUtils::parse(json, (T&)*this); }
#else
	ofJson createUndo() const { return ((T&)*this).toJson(); }
	void loadUndo(const ofJson &json) { ((T&)*this).loadJson(json); }
#endif
};
}}

template<typename T>
using ofxUndoJson = ofx::undo::Json<T>;
