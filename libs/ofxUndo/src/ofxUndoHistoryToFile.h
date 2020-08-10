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

class HistoryToFile : public State<std::filesystem::path>
{
public:
	void setFileExt(const std::string &ext) { extension_ = ext; }
	
	void setDirectory(const std::filesystem::path &path, bool load=true) {
		clear();
		directory_.open(path);
		directory_.sortByDate();
		if(load) {
			for(auto &&f : directory_) {
				std::filesystem::path path(ofFilePath::join(directory_.path(), f.getFileName()));
				store(path);
			}
		}
	}
protected:
	std::filesystem::path createUndo() const {
		auto path = createFileName(extension_);
		save(path);
		return path;
	}
	void loadUndo(const std::filesystem::path &path) {
		load(path);
	}
	virtual std::filesystem::path createFileName(const std::string &ext) const {
		std::string filename = ofJoinString({ofGetTimestampString("%Y%m%d_%H%M_%S%i"), extension_}, ".");
		return ofFilePath::join(directory_.path(), filename);
	}
	virtual void save(const std::filesystem::path &path) const=0;
	virtual void load(const std::filesystem::path &path)=0;
	ofDirectory directory_;
	std::string extension_ = "txt";
};
}}

using ofxUndoFile = ofx::undo::HistoryToFile;
