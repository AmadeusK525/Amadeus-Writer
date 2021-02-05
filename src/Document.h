#ifndef DOCUMENT_H_
#define DOCUMENT_H_
#pragma once

#include <wx\wx.h>
#include <wx\memory.h>

#include <map>

using std::map;
using std::pair;

struct amDocument {
	wxString tableName{ "" };
	wxString name{ "" };

	map<wxString, int> integers{};
	map<wxString, wxString> strings{};
	map<wxString, wxMemoryBuffer> memBuffers{};

	bool needsForeign = false;
	pair<wxString, int> foreignKey{ "", -1 };

	wxVector<amDocument> documents{};
};

#endif