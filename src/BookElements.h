#ifndef BOOKELEMENTS_H_
#define BOOKELEMENTS_H_
#pragma once

#include <wx\wx.h>
#include <wx\filename.h>
#include <wx\richtext\richtextctrl.h>

#include "StoryElements.h"

struct amProject;
struct Book;
struct Document;
struct Note;


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
	Book* book = nullptr;

	wxString name{ "" };
	wxString synopsys{ "" };

	DocumentType type = Document_Other;

	int nWordCountTarget = 0;
	bool bIncludeInCompile = true;
	wxString status;

	wxRichTextBuffer* buffer = nullptr;

	wxVector<Element*> elements{};
	wxVector<Note*> notes{};

	int position = -1;
	int id = -1;

	bool isInTrash = false;

	Document(Document* parent, int position, Book* book, DocumentType documentType) :
		parent(parent),
		book(book),
		type(documentType),
		position(position)
	{}
	virtual ~Document();

	void SetId(int id) { this->id = id; }
	bool IsStory();

	bool HasRedNote();
	int GetWordCount();

	void Save(wxSQLite3Database* db);
	bool Update(wxSQLite3Database* db, bool updateContent, bool updateNotes);

	void LoadSelf(wxSQLite3Database* db, wxRichTextCtrl* targetRtc);
	void CleanUp();

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
	wxString shortTitle{ "" };
	wxString publisher{ "" };

	wxString author{ "" },
		genre{ "" },
		description{ "" },
		synopsys{ "" };

	int pos = -1;
	int id = -1;

	wxVector<Document*> documents{};
	wxVector<Document*> vRecentDocuments{};

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
	void PushRecentDocument(Document* document);

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