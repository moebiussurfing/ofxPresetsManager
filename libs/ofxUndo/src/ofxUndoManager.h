/*
 The MIT License (MIT)
 
 Copyright (c) 2018 nariakiiwatani
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "ofEvents.h"
#include "ofxUndoModifyChecker.h"

namespace ofx { namespace undo {

template<typename Data>
class Manager
{
public:
	using DataType = Data;
	Manager() {
		setUndoCreator(*this);
	}
	void store() { store(create_undo_()); }
	int undo(int times=1, bool step_by_step=true);
	int redo(int times=1, bool step_by_step=true);
	bool canUndo(int times=1, int *maximum=nullptr) const;
	bool canRedo(int times=1, int *maximum=nullptr) const;
	virtual int getUndoLength() const { return current_index_; }
	virtual int getRedoLength() const { return history_.size()-current_index_; }
	
	template<typename T>
	void setUndoCreator(T &t) { create_undo_ = [&]() { return t.createUndo(); }; }

	void setHistoryLengthLimit(std::size_t length);
	void clear();
	
	ofEvent<const Data>& storeEvent() { return store_event_; }
	ofEvent<const Data>& undoEvent() { return undo_event_; }
	ofEvent<const Data>& redoEvent() { return redo_event_; }

	virtual void clearRedo();
	
	template<typename Context, typename Descriptor=decltype(std::declval<Context>().getUndoStateDescriptor())>
	void enableModifyChecker(Context &context, float check_interval_in_seconds=1);
	void enableModifyChecker(float check_interval_in_seconds=1) {
		enableModifyChecker(*this, check_interval_in_seconds);
	}
	void disableModifyChecker() { modify_checker_.reset(); }
	virtual Data getUndoStateDescriptor() const { return create_undo_(); }
protected:
	std::function<Data()> create_undo_;
	virtual Data createUndo() const { return Data(); }
	void store(const Data &data);
	virtual void loadUndo(const Data &data) {}
	virtual void loadRedo(const Data &data) { loadUndo(data); }

	std::deque<Data> history_;
	std::size_t history_length_limit_=0;
	std::size_t current_index_;
	enum Action {
		UNDO, REDO, OTHER
	};
	Action last_action_ = OTHER;

	ofEvent<const Data> store_event_;
	ofEvent<const Data> undo_event_, redo_event_;
	
	std::shared_ptr<ModifyChecker_> modify_checker_;
	
protected:
	virtual Data& getDataForUndo(int index)=0;
	virtual Data& getDataForRedo(int index)=0;
	int deleteOldHistory(int length) {
		int result = 0;
		while(length-->0) {
			if(history_.empty()) break;
			history_.pop_front();
			++result;
		}
		current_index_ = std::max<int>(0, current_index_-result);
		return result;
	}
};

template<typename Data>
void Manager<Data>::store(const Data &data)
{
	if(history_length_limit_ > 0 && history_.size() == history_length_limit_) {
		deleteOldHistory(1);
	}
	clearRedo();
	history_.emplace_back(data);
	current_index_ = history_.size();
	last_action_ = OTHER;
	store_event_.notify(this, data);
	if(modify_checker_) {
		modify_checker_->updateDescriptor();
	}
}

template<typename Data>
int Manager<Data>::undo(int times, bool step_by_step)
{
	if(times <= 0) return 0;
	int maximum = 0;
	if(!canUndo(times, &maximum)) {
		return undo(maximum, step_by_step);
	}
	if(times > 1 && step_by_step) {
		for(int i = 0; i < times; ++i) {
			undo(1);
		}
	}
	else {
		current_index_ -= times;
		auto &data = getDataForUndo(current_index_);
		loadUndo(data);
		last_action_ = UNDO;
		undo_event_.notify(this, data);
	}
	if(modify_checker_) {
		modify_checker_->updateDescriptor();
	}
	return times;
}
template<typename Data>
int Manager<Data>::redo(int times, bool step_by_step)
{
	if(times <= 0) return 0;
	int maximum = 0;
	if(!canRedo(times, &maximum)) {
		return redo(maximum, step_by_step);
	}
	if(times > 1 && step_by_step) {
		for(int i = 0; i < times; ++i) {
			redo(1);
		}
	}
	else {
		current_index_ += times;
		auto &data = getDataForRedo(current_index_);
		loadRedo(data);
		last_action_ = REDO;
		redo_event_.notify(this, data);
	}
	if(modify_checker_) {
		modify_checker_->updateDescriptor();
	}
	return times;
}
	
template<typename Data>
bool Manager<Data>::canUndo(int times, int *maximum) const
{
	int length = getUndoLength();
	if(maximum) *maximum = length;
	return times <= length;
}
template<typename Data>
bool Manager<Data>::canRedo(int times, int *maximum) const
{
	int length = getRedoLength();
	if(maximum) *maximum = length;
	return times <= length;
}

template<typename Data>
void Manager<Data>::setHistoryLengthLimit(std::size_t length)
{
	int sub = history_.size() > length;
	if(sub) {
		deleteOldHistory(sub);
	}
	history_length_limit_ = length;
}

template<typename Data>
void Manager<Data>::clear()
{
	history_.clear();
	current_index_ = 0;
	last_action_ = OTHER;
	if(modify_checker_) {
		modify_checker_->updateDescriptor();
	}
}
template<typename Data>
void Manager<Data>::clearRedo() {
	history_.resize(current_index_);
	last_action_ = OTHER;
}

template<typename Data>
template<typename Context, typename Descriptor>
void Manager<Data>::enableModifyChecker(Context &context, float check_interval_in_seconds) {
	modify_checker_ = std::make_shared<ModifyChecker<Context, Descriptor>>(context);
	modify_checker_->setInterval(check_interval_in_seconds);
	ofAddListener(modify_checker_->onModified(), this, static_cast<void(Manager::*)()>(&Manager::store));
	modify_checker_->enable();
}
}}

template<typename Data>
using ofxUndoManager = ofx::undo::Manager<Data>;
