/*
 The MIT License (MIT)
 
 Copyright (c) 2018 nariakiiwatani
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "ofxUndo.h"
#include "ofxUndoGroup.h"
#include "ofxUndoRef.h"
#include <map>
#include <string>


namespace {
	std::map<std::string, ofxUndoGroup> root_;
	std::map<std::size_t, void*> memory_;
}

template<typename T>
static void ofxRegisterUndo(T &target, float auto_check_interval=1, const std::string &group_id="")
{
	auto instance = new ofxUndoRef<T>(target);
	root_[group_id].add(*instance);
	if(auto_check_interval > 0) {
		instance->enableModifyChecker(auto_check_interval);
	}
	memory_[reinterpret_cast<std::size_t>(&target)] = instance;
}
template<typename T>
static void ofxUnregisterUndo(T &target, const std::string &group_id="")
{
	auto found = memory_.find(reinterpret_cast<std::size_t>(&target));
	if(found == std::end(memory_)) {
		return;
	}
	auto instance = static_cast<ofxUndoRef<T>*>(found->second);
	instance->disableModifyChecker();
	root_[group_id].remove(*instance);
	delete instance;
	memory_.erase(found);
}
static void ofxUndo(const std::string &group_id="", int times=1, bool step_by_step=true)
{
	root_[group_id].undo(times, step_by_step);
}
static void ofxRedo(const std::string &group_id="", int times=1, bool step_by_step=true)
{
	root_[group_id].redo(times, step_by_step);
}

