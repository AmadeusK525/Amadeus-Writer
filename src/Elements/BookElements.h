#ifndef BOOKELEMENTS_H_
#define BOOKELEMENTS_H_
#pragma once

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/richtext/richtextctrl.h>

#include "Elements/StoryElements.h"


namespace am
{
	class ProjectSQLDatabase;

	struct Project;
	struct Book;
	struct Document;
	struct Note;


	/////////////////////////////////////////////////////////////////
	/////////////////////////// am::Document ////////////////////////////
	/////////////////////////////////////////////////////////////////


	struct Note
	{
		wxString name{ "" };
		wxString content{ "" };

		bool isDone = false;

		Note(wxString content, wxString name);

		SQLEntry GenerateSQLEntry();
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

		int nWordCount = 0;
		int nWordCountTarget = 0;
		bool bIncludeInCompile = true;
		wxString status;

		wxRichTextBuffer* buffer = nullptr;
		bool bIsDirty = true;

		wxVector<TangibleElement*> vTangibleElements{};
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

		void Save(ProjectSQLDatabase* db);
		bool Update(ProjectSQLDatabase* db, bool updateContent, bool updateNotes);

		void LoadSelf(ProjectSQLDatabase* db, wxRichTextCtrl* targetRtc);
		void CleanUp();

		SQLEntry GenerateSQLEntrySimple();
		SQLEntry GenerateSQLEntry();
		SQLEntry GenerateSQLEntryForId();

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

		void Save(ProjectSQLDatabase* db);
		bool Update(ProjectSQLDatabase* db, bool updateDocuments);

		SQLEntry GenerateSQLEntrySimple();
		SQLEntry GenerateSQLEntry(wxVector<int>* documentsToInclude = nullptr);
		SQLEntry GenerateSQLEntryForId();

		inline static wxSize GetCoverSize() { return coverSize; }
	};


	/////////////////////////////////////////////////////////////////
	//////////////////////////// Project ////////////////////////////
	/////////////////////////////////////////////////////////////////


	struct Project
	{
		wxVector<Book*> books{};
		wxVector<StoryElement*> vStoryElements;

		wxFileName amFile{};

		virtual ~Project();

		wxVector<Character*> GetCharacters();
		wxVector<Location*> GetLocations();
		wxVector<Item*> GetItems();

		wxVector<Document*>& GetDocuments(int bookPos);
	};
}

#endif