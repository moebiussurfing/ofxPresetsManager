/*
 The MIT License (MIT)
 
 Copyright (c) 2018 nariakiiwatani
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "ofxUndoManager.h"

namespace ofx { namespace undo {
template<typename Data>
class State : public Manager<Data>
{
public:
	int getRedoLength() const { return Manager<Data>::history_.size()-Manager<Data>::current_index_-1; }
	void clearRedo() {
		switch(Manager<Data>::last_action_) {
			case Manager<Data>::UNDO:
			case Manager<Data>::REDO:
				++Manager<Data>::current_index_;
				break;
		}
		Manager<Data>::history_.resize(Manager<Data>::current_index_);
		Manager<Data>::last_action_ = Manager<Data>::OTHER;
	}
protected:
	Data& getDataForUndo(int index)  { return Manager<Data>::history_[index]; }
	Data& getDataForRedo(int index) { return Manager<Data>::history_[index]; }
};
}}

template<typename Data>
using ofxUndoState = ofx::undo::State<Data>;
