#ifndef STORYELEMENTS_H_
#define STORYELEMENTS_H_
#pragma once

#include <wx/bitmap.h>
#include <wx\wxsqlite3.h>

#include "SQLEntry.h"

struct Element;
struct Document;
struct Location;
struct Item;

using std::pair;

enum CompType
{
	CompRole,
	CompName,
	CompNameInverse,
	CompDocuments,
	CompFirst,
	CompLast
};

enum Role
{
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

struct Element : public wxObject
{
	wxDECLARE_DYNAMIC_CLASS(Element);

	wxString name{ "" };
	Role role{ None };

	wxVector<pair<wxString, wxString>> custom{ 0 };

	wxVector<wxString> aliases{};

	wxVector<Document*> documents{};
	wxImage image{};

	wxVector<Element*> relatedElements{};

	static CompType elCompType;
	int id = -1;

	Element() = default;

	virtual void Save(wxSQLite3Database* db) = 0;
	virtual bool Update(wxSQLite3Database* db) = 0;

	virtual amSQLEntry GenerateSQLEntrySimple() = 0;
	virtual amSQLEntry GenerateSQLEntry() = 0;
	virtual amSQLEntry GenerateSQLEntryForId() = 0;

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
	CR_Cousin,

	CR_Child,
	CR_Grandchild,

	CR_Friend,
	CR_Enemy,

	CR_Other
};

struct Character : public Element
{
	wxDECLARE_DYNAMIC_CLASS(Character);

	wxString sex{ "" }, age{ "" }, nat{ "" },
		height{ "" }, nick{ "" }, appearance{ "" },
		personality{ "" }, backstory{ "" };

	bool isAlive = true;

	static CompType cCompType;

	Character() = default;

	virtual void Save(wxSQLite3Database* db);
	virtual bool Update(wxSQLite3Database* db);

	virtual amSQLEntry GenerateSQLEntrySimple();
	virtual amSQLEntry GenerateSQLEntry();
	virtual amSQLEntry GenerateSQLEntryForId();

	bool operator<(const Character& other) const;
	void operator=(const Character& other);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Location ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum LocationType
{
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

struct Location : public Element
{
	wxDECLARE_DYNAMIC_CLASS(Location);

	wxString general{ "" }, natural{ "" }, architecture{ "" },
		politics{ "" }, economy{ "" }, culture{ "" };

	static CompType lCompType;

	Location() = default;

	virtual void Save(wxSQLite3Database* db);
	virtual bool Update(wxSQLite3Database* db);

	virtual amSQLEntry GenerateSQLEntrySimple();
	virtual amSQLEntry GenerateSQLEntry();
	virtual amSQLEntry GenerateSQLEntryForId();

	bool operator<(const Location& other) const;
	void operator=(const Location& other);
};


//////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Item /////////////////////////////////
//////////////////////////////////////////////////////////////////////////


struct Item : public Element
{
	wxDECLARE_DYNAMIC_CLASS(Item);

	wxString origin{ "" }, backstory{ "" }, appearance{ "" },
		usage{ "" }, general{ "" };

	wxString width{ "" }, height{ "" }, depth{ "" };

	bool isMagic{ false };
	bool isManMade{ true };

	static CompType iCompType;

	virtual void Save(wxSQLite3Database* db);
	virtual bool Update(wxSQLite3Database* db);

	virtual amSQLEntry GenerateSQLEntrySimple();
	virtual amSQLEntry GenerateSQLEntry();
	virtual amSQLEntry GenerateSQLEntryForId();

	bool operator<(const Item& other) const;
	void operator=(const Item& other);
};

#endif
