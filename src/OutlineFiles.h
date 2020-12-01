#ifndef OUTLINEFILES_H_
#define OUTLINEFILES_H_
#pragma once

#include <wx\splitter.h>
#include <wx\dataview.h>
#include <wx\richtext\richtextctrl.h>

#include "Character.h"
#include "Location.h"

using std::vector;

class OutlineFiles: public wxSplitterWindow {
private:
	wxDataViewTreeCtrl* files = nullptr;
	wxRichTextCtrl* content = nullptr;

	wxDataViewItem researchRoot{};
	wxDataViewItem charactersRoot{};
	wxDataViewItem locationsRoot{};

	vector<wxRichTextBuffer> researchBuffers{};
	vector<wxRichTextBuffer> charBuffers{};
	vector<wxRichTextBuffer> locBuffers{};

	vector<wxDataViewItem> researchItems{};
	vector<wxDataViewItem> charItems{};
	vector<wxDataViewItem> locItems{};

	std::string current{""};

public:
	OutlineFiles(wxWindow* parent);
	void init();

	void appendCharacter(Character& character);
	void appendLocation(Location& location);

	void deleteCharacter(Character& character);
	void deleteLocation(Location& location);

	void onSelectionChanged(wxDataViewEvent& event);
	void onEditingStart(wxDataViewEvent& event);

	virtual void OnUnsplit(wxWindow* window);

	bool save();
	bool load();

	void clearAll();

	DECLARE_EVENT_TABLE()
};

enum {
	TREE_Files
};

#endif