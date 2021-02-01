#ifndef STORYELEMENTS_H_
#define STORYELEMENTS_H_
#pragma once

#include <wx/bitmap.h>

#include <fstream>

struct Chapter;

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

	wxVector<pair<wxString, wxString>> custom{ 0 };

	wxVector<Chapter*> chapters{};
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
	wxString sex{ "" }, age{ "" }, nat{ "" },
		height{ "" }, nick{ "" }, appearance{ "" },
		personality{ "" }, backstory{ "" };

	static CompType cCompType;

	Character() = default;

	virtual void Save(std::ofstream& out) {}
	virtual void Load(std::ifstream& in) {}

	bool operator<(const Character& other) const;
	void operator=(const Character& other);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Location ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


struct Location : public Element {
	wxString general{ "" }, natural{ "" }, architecture{ "" },
		politics{ "" }, economy{ "" }, culture{ "" };

	static CompType lCompType;

	Location() = default;

	virtual void Save(std::ofstream& out) {}
	virtual void Load(std::ifstream& in) {}

	bool operator<(const Location& other) const;
	void operator=(const Location& other);
};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Item /////////////////////////////////
//////////////////////////////////////////////////////////////////////////


struct Item : public Element {
	wxString origin{ "" }, backstory{ "" }, appearance{ "" },
		usage{ "" }, general{ "" };

	wxString width{ "" }, height{ "" }, depth{ "" };
	
	bool isMagic{ false };
	bool isManMade{ true };

	static CompType iCompType;

	virtual void Save(std::ofstream& out) {}
	virtual void Load(std::ifstream& in) {}

	bool operator<(const Item& other) const;
	void operator=(const Item& other);
};

#endif
