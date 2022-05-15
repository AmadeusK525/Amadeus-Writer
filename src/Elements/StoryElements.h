#ifndef STORYELEMENTS_H_
#define STORYELEMENTS_H_
#pragma once

#include <wx/bitmap.h>
#include <wx/wxsqlite3.h>
#include <wx/wxxmlserializer/XmlSerializer.h>

#include <map>

#include "Database/SQLEntry.h"

typedef std::multimap<wxString, wxString> wxStringMultimap;
XS_DECLARE_IO_HANDLER(wxStringMultimap, xsStringMultimapIO);

namespace am
{
	struct StoryElement;
	struct Document;
	struct Book;
	struct Location;
	struct Item;
	class ProjectSQLDatabase;

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

	enum StoryElementAttrType
	{
		atSHORT,
		atLONG,

		atUNKNOWN
	};

	struct StoryElementAttr: public xsSerializable
	{
		wxString sTitle, sContent;
		wxColour titleColour, contentColour;
		StoryElementAttrType type = atUNKNOWN;
	};

	struct StoryElement: public xsSerializable
	{
		wxDECLARE_DYNAMIC_CLASS(am::StoryElement);

		wxString name{ "" };
		Role role{ None };

		wxVector<StoryElementAttr> vAttr;

		wxStringMultimap mShortAttributes;
		wxStringMultimap mLongAttributes;

		wxArrayString aliases{};

		wxImage image{};

		wxVector<StoryElement*> relatedElements{};

		static CompType elCompType;

		int dbID = -1;

		StoryElement();

		virtual void Save(ProjectSQLDatabase* db);
		virtual bool Update(ProjectSQLDatabase* db);

		virtual wxString GetXMLString();

		virtual void EditTo(const StoryElement& other);

		virtual SQLEntry GenerateSQLEntrySimple();
		virtual SQLEntry GenerateSQLEntry();
		virtual SQLEntry GenerateSQLEntryForId();

		virtual bool operator<(const StoryElement& other) const;
		virtual bool operator==(const StoryElement& other) const;
		virtual void operator=(const StoryElement& other) = delete;
	};

	struct TangibleElement : StoryElement
	{
		wxDECLARE_DYNAMIC_CLASS(am::TangibleElement);

		virtual ~TangibleElement();
		wxVector<Document*> documents{};

		bool IsInBook(Book* book) const;

		Document* GetFirstDocument() const;
		Document* GetLastDocument() const;
		Document* GetFirstDocumentInBook(Book* book) const;
		Document* GetLastDocumentInBook(Book* book) const;

		virtual void EditTo(const StoryElement& other);

		virtual bool operator<(const TangibleElement& other) const;
	};


	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////// am::Character ///////////////////////////////////
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

	struct Character : public TangibleElement
	{
		wxDECLARE_DYNAMIC_CLASS(am::Character);

		bool isAlive = true;
		static CompType cCompType;

		Character();

		virtual void EditTo(const StoryElement& other);

		virtual bool operator<(const Character& other) const;
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
		LOCATION_Building,
		LOCATION_House,

		LOCATION_Area,
		LOCATION_Other
	};

	struct Location : public TangibleElement
	{
		wxDECLARE_DYNAMIC_CLASS(am::Location);

		LocationType type = LOCATION_Other;
		static CompType lCompType;

		Location();

		virtual void EditTo(const StoryElement& other);

		virtual bool operator<(const Location& other) const;
	};


	//////////////////////////////////////////////////////////////////////////
	/////////////////////////////////// Item /////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	struct Item : public TangibleElement
	{
		wxDECLARE_DYNAMIC_CLASS(am::Item);

		bool isMagic = false;
		bool isManMade = true;

		static CompType iCompType;

		Item();

		virtual void EditTo(const StoryElement& other);

		virtual bool operator<(const Item& other) const;
	};
}
#endif
