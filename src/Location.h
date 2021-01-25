#ifndef LOCATION_H_
#define LOCATION_H_
#pragma once

#include <wx\bitmap.h>

#include <fstream>
#include <string>
#include <vector>

#include "Chapter.h"
#include "Character.h"

using std::vector;
using std::string;
using std::pair;

struct Location {
	wxString name{ "" };
	string general{ "" }, natural{ "" }, architecture{ "" },
		politics{ "" }, economy{ "" }, culture{ "" }, importance{ "" };

	vector<pair<string, string>> custom{};

	ChapterPtrArray chapters{};
	wxImage image{};

	static CompType compType;

	Location() = default;

	void save(std::ofstream& out);
	void load(std::ifstream& in);

	bool operator<(const Location& other) const;
	bool operator==(const Location& other) const;
};
#endif