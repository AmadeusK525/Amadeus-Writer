#ifndef ELEMENTS_H_
#define ELEMENTS_H_
#pragma once

#include <wx/bitmap.h>

#include <string>
#include <vector>
#include <fstream>

#include "Chapter.h"

using std::string;
using std::vector;
using std::pair;
using std::ostream;
using std::istream;

enum CompType {
	CompRole,
	CompName,
	CompNameInverse,
	CompChapters,
	CompFirst,
	CompLast
};

enum Role {
	cProtagonist,
	cVillian,
	cSupporting,
	cSecondary,

	lHigh,
	lLow,

	iHigh,
	iLow,

	None
};

struct Element {
	wxString name{ "" };
	Role role{ None };

	vector<pair<string, string>> custom{ 0 };

	ChapterPtrArray chapters{};
	wxImage image{};

	static CompType elCompType;

	Element() = default;

	virtual void Save(std::ofstream& out) = 0;
	virtual void Load(std::ifstream& in) = 0;

	bool operator<(const Element& other) const;
	bool operator==(const Element& other) const;
	void operator=(const Element& other);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Character ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


struct Character : public Element {
	string sex{ "" }, age{ "" }, nat{ "" },
		height{ "" }, nick{ "" }, appearance{ "" },
		personality{ "" }, backstory{ "" };

	static CompType cCompType;

	Character() = default;

	virtual void Save(std::ofstream& out);
	virtual void Load(std::ifstream& in);

	bool operator<(const Character& other) const;
	void operator=(const Character& other);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Location ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


struct Location : public Element {
	string general{ "" }, natural{ "" }, architecture{ "" },
		politics{ "" }, economy{ "" }, culture{ "" };

	static CompType lCompType;

	Location() = default;

	virtual void Save(std::ofstream& out);
	virtual void Load(std::ifstream& in);

	bool operator<(const Location& other) const;
	void operator=(const Location& other);
};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Item /////////////////////////////////
//////////////////////////////////////////////////////////////////////////


struct Item : public Element {
	string origin{ "" }, backstory{ "" }, appearance{ "" },
		usage{ "" }, general{ "" };

	string width{ "" }, height{ "" }, depth{ "" };
	
	bool isMagic{ false };
	bool isManMade{ true };

	static CompType iCompType;

	virtual void Save(std::ofstream& out);
	virtual void Load(std::ifstream& in);

	bool operator<(const Item& other) const;
	void operator=(const Item& other);
};

#endif
