#ifndef DOCUMENT_H_
#define DOCUMENT_H_
#pragma once

#include <wx/wx.h>
#include <wx/memory.h>

#include <unordered_map>

namespace am
{
	struct SQLEntry
	{
		wxString tableName{ "" };
		wxString name{ "" };

		std::unordered_map<wxString, int> integers{};
		std::unordered_map<wxString, wxString> strings{};
		std::unordered_map<wxString, wxMemoryBuffer> memBuffers{};

		bool specialForeign = false;
		std::pair<wxString, int> foreignKey{ "", -1 };

		wxVector<SQLEntry> childEntries{};
	};
}

#endif