#ifndef BOOKELEMENTS_H_
#define BOOKELEMENTS_H_
#pragma once

#include <wx\wx.h>
#include <wx\filename.h>
#include <wx\richtext\richtextbuffer.h>

#include "StoryElements.h"


/////////////////////////////////////////////////////////////////
/////////////////////////// Document ////////////////////////////
/////////////////////////////////////////////////////////////////


struct Note
{
	wxString name{ "" };
	wxString content{ "" };

	bool isDone = false;

	Note(wxString content, wxString name);

	amSQLEntry GenerateSQLEntry();
};

enum DocumentType
{
	Section_Part,
	Section_FrontMatter,
	Section_BackMatter,

	Document_FrontMatter,
	Document_BackMatter,

	Document_Chapter,
	Document_Scene,
	Document_Other
};

struct Document
{
	Document* parent = nullptr;
	wxVector<Document*> children{};

	wxString name{ "" };
	wxString synopsys{ "" };

	DocumentType type = Document_Other;

	wxRichTextBuffer* buffer = nullptr;

	wxVector<Element*> elements{};
	wxVector<Note*> notes{};

	int position = -1;
	int id = -1;
	int bookID = -1;

	bool isInTrash = false;

	Document(Document* parent, int position, int bookId, DocumentType documentType) :
		parent(parent),
		position(position),
		bookID(bookId),
		type(documentType)
	{}
	virtual ~Document();

	void SetId(int id) { this->id = id; }

	bool HasRedNote();

	void Save(wxSQLite3Database* db);
	bool Update(wxSQLite3Database* db, bool updateContent, bool updateNotes);

	amSQLEntry GenerateSQLEntrySimple();
	amSQLEntry GenerateSQLEntry();
	amSQLEntry GenerateSQLEntryForId();

	bool operator<(const Document& other) const;
	bool operator==(const Document& other) const;
};


/////////////////////////////////////////////////////////////////
///////////////////////////// Book //////////////////////////////
/////////////////////////////////////////////////////////////////


struct Book
{
	static wxSize coverSize;
	wxImage cover;

	wxString title{ "" };
	wxString publisher{ "" };

	wxString author{ "" },
		genre{ "" },
		description{ "" },
		synopsys{ "" };

	int pos = -1;
	int id = -1;

	wxVector<Document*> documents{};

	wxVector<Character*> characters{};
	wxVector<Location*> locations{};
	wxVector<Item*> items{};

	Book() : cover(coverSize.x, coverSize.y) {}
	Book(const wxString& title, int pos) :
		title(title), pos(pos), cover(coverSize.x, coverSize.y)
	{
		InitCover();
	}
	virtual ~Book();

	bool Init();
	void InitCover();

	void SetId(int id) { this->id = id; }

	void CleanUpDocuments();

	void Save(wxSQLite3Database* db);
	bool Update(wxSQLite3Database* db, bool updateDocuments);

	amSQLEntry GenerateSQLEntrySimple();
	amSQLEntry GenerateSQLEntry(wxVector<int>* documentsToInclude = nullptr);
	amSQLEntry GenerateSQLEntryForId();

	inline static wxSize GetCoverSize() { return coverSize; }
};


/////////////////////////////////////////////////////////////////
//////////////////////////// Project ////////////////////////////
/////////////////////////////////////////////////////////////////


struct amProject
{
	wxVector<Book*> books{};

	wxVector<Element*> elements;

	wxFileName amFile{};

	virtual ~amProject();

	wxVector<Character*> GetCharacters();
	wxVector<Location*> GetLocations();
	wxVector<Item*> GetItems();

	wxVector<Document*>& GetDocuments(int bookPos);
};

#endif