#ifndef STORYELEMENTS_H_
#define STORYELEMENTS_H_
#pragma once

#include <wx/bitmap.h>
#include <wx\wxsqlite3.h>

#include "Document.h"

struct Element;
struct Chapter;
struct Location;
struct Item;

using std::pair;

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

	wxVector<wxString> aliases{};

	wxVector<Chapter*> chapters{};
	wxImage image{};

	static CompType elCompType;
	int id = -1;

	Element() = default;

	virtual void Save(wxSQLite3Database* db) = 0;
	virtual bool Update(wxSQLite3Database* db) = 0;

	virtual amDocument GenerateDocumentSimple() = 0;
	virtual amDocument GenerateDocument() = 0;
	virtual amDocument GenerateDocumentForId() = 0;

	void SetId(int id) { this->id = id; }

	bool operator<(const Element& other) const;
	bool operator==(const Element& other) const;
	void operator=(const Element& other);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Character ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum CharacterRelation
{
	CR_Parent,
	CR_Grandparent,

	CR_Sibling,
	CR_Cousing,

	CR_Child,
	CR_Grandchild,

	CR_Friend,
	CR_Enemy,

	CR_Other
};

struct Character : public Element {
	wxString sex{ "" }, age{ "" }, nat{ "" },
		height{ "" }, nick{ "" }, appearance{ "" },
		personality{ "" }, backstory{ "" };

	static CompType cCompType;

	wxVector<pair<Character*, CharacterRelation>> relatedCharacters;
	wxVector<Location*> relatedLocations;
	wxVector<Item*> relatedItems;

	Character() = default;

	virtual void Save(wxSQLite3Database* db);
	virtual bool Update(wxSQLite3Database* db);

	virtual amDocument GenerateDocumentSimple();
	virtual amDocument GenerateDocument();
	virtual amDocument GenerateDocumentForId();

	bool operator<(const Character& other) const;
	void operator=(const Character& other);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Location ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum LocationType {
	LOCATION_Continent,
	LOCATION_Country,
	LOCATION_State,
	LOCATION_City,
	LOCATION_Village,
	LOCATION_District,
	LOCATION_Neighborhood,
	LOCATION_Street,
	LOCATION_House,

	LOCATION_Area,
	LOCATION_Other
};

struct Location : public Element {
	wxString general{ "" }, natural{ "" }, architecture{ "" },
		politics{ "" }, economy{ "" }, culture{ "" };

	wxVector<Character*> relatedCharacters;
	wxVector<Location*> relatedLocations;

	static CompType lCompType;

	Location() = default;

	virtual void Save(wxSQLite3Database* db);
	virtual bool Update(wxSQLite3Database* db);

	virtual amDocument GenerateDocumentSimple();
	virtual amDocument GenerateDocument();
	virtual amDocument GenerateDocumentForId();

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
	
	wxVector<Character*> relatedCharacters;
	wxVector<Location*> relatedLocations;
	wxVector<Item*> relatedItems;

	bool isMagic{ false };
	bool isManMade{ true };

	static CompType iCompType;

	virtual void Save(wxSQLite3Database* db);
	virtual bool Update(wxSQLite3Database* db);

	virtual amDocument GenerateDocumentSimple();
	virtual amDocument GenerateDocument();
	virtual amDocument GenerateDocumentForId();

	bool operator<(const Item& other) const;
	void operator=(const Item& other);
};

#endif
