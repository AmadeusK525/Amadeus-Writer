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

	wxVector<pair<wxString, int>> integers{};
	wxVector<pair<wxString, wxString>> strings{};
	wxVector<pair<wxString, wxMemoryBuffer>> memBuffers{};

	bool specialForeign = false;
	pair<wxString, int> foreignKey{ "", -1 };

	wxVector<amDocument> documents{};
};

#endif