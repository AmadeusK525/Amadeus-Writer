#ifndef PROJECTMANAGING_H_
#define PROJECTMANAGING_H_
#pragma once

#include <wx\wx.h>
#include <wx\sstream.h>
#include <wx\filename.h>

#include <wx\wxsqlite3.h>
#include <wx\wxsf\wxShapeFramework.h>

#include "ProjectWizard.h"
#include "BookElements.h"
#include "SQLEntry.h"

class amMainFrame;
class amOverview;
class amElementNotebook;
class amStoryNotebook;
class amOutline;
class amRelease;

class amStoryWriter;

///////////////////////////////////////////////////////////////////
//////////////////////////// SQLStorage ///////////////////////////
///////////////////////////////////////////////////////////////////

namespace am
{
	class ProjectSQLDatabase : public wxSQLite3Database
	{
	public:
		ProjectSQLDatabase() = default;
		ProjectSQLDatabase(wxFileName& path);

		bool Init();
		void CreateAllTables();

		int GetSQLEntryId(SQLEntry& sqlEntry);

		bool CreateTable(const wxString& tableName, const wxArrayString& arguments, bool ifNotExists = true);

		bool InsertSQLEntry(SQLEntry& sqlEntry);
		bool UpdateSQLEntry(SQLEntry& original, SQLEntry& edit);

		bool InsertManyToMany(const wxString& tableName,
			int sqlID1, const wxString& arg1,
			int sqlID2, const wxString& arg2);

		bool InsertManyToMany(const wxString& tableName,
			SQLEntry& sqlEntry1, const wxString& arg1,
			SQLEntry& sqlEntry2, const wxString& arg2);

		bool DeleteSQLEntry(SQLEntry& sqlEntry);

		bool DeleteManyToMany(const wxString& tableName,
			int docID1, const wxString& arg1,
			int docID2, const wxString& arg2);

		bool DeleteManyToMany(const wxString& tableName,
			SQLEntry& doc1, const wxString& arg1,
			SQLEntry& doc2, const wxString& arg2);

		wxSQLite3Statement ConstructInsertStatement(SQLEntry& sqlEntry);
		wxSQLite3Statement ConstructUpdateStatement(SQLEntry& sqlEntry, int id);

		bool RowExists(SQLEntry& sqlEntry);
	};


	///////////////////////////////////////////////////////////////////////
	///////////////////////// ProjectPreferences //////////////////////////
	///////////////////////////////////////////////////////////////////////


	struct ProjectPreferences : public xsSerializable
	{

		ProjectPreferences() { MarkSerializableDataMembers(); }

		void MarkSerializableDataMembers();
	};

	struct SessionAttributes : public xsSerializable
	{
		int nMainFrameSashPos = -1,
			nCharacterSashPos = -1,
			nLocationSashPos = -1,
			nItemSashPos = -1,
			nOutlineFilesSashPos = -1;

		SessionAttributes() { MarkSerializableDataMembers(); }

		void MarkSerializableDataMembers();
	};


	///////////////////////////////////////////////////////////////////////
	/////////////////////////// ProjectManaging ///////////////////////////
	///////////////////////////////////////////////////////////////////////


	bool Init(const wxString& path);
	void ShutDown();

	ProjectSQLDatabase* GetProjectDatabase();

	bool SaveProject();
	bool LoadProject();

	bool SaveSessionToDb();

	void SaveSQLEntry(SQLEntry& original, SQLEntry& edit);

	bool DoSaveProject(const wxString& path);
	bool DoLoadProject(const wxString& path);

	void LoadBooks();
	void LoadBookContent(Book* book, bool loadDocuments);
	void LoadDocuments(Book* book);
	void SetupDocumentHierarchy(Book* book);

	void LoadStoryElements();

	void LoadStandardRelations();
	void LoadCharacterRelations();

	bool LoadLastSession();
	void LoadSessionAttr(const SessionAttributes& attr);

	void SetNewProjectPath(const wxFileName& path);
	void SetExecutablePath(const wxString& path);

	bool SetCurrentBook(int bookPos);
	bool SetCurrentBook(Book* book);

	void StartCreatingBook();

	amMainFrame* GetMainFrame();
	amElementNotebook* GetElementsNotebook();
	amStoryNotebook* GetStoryNotebook();
	amOutline* GetOutline();
	amRelease* GetRelease();

	wxString GetPath(bool withSeparator);
	wxString GetFullPath();
	wxString GetExecutablePath(bool withSeparator);
	wxString GetTitle();

	void ClearPath();

	// These two go together when saving / loading. setLast writes to a file the path to the most recently
	// worked on project. When booting up the application, the getLast function will
	// be called and it will get the written path, opening up the project automatically,
	// with no need to load it. Makes things more convenient.
	void SetLastSavePath();
	wxString GetLastSavePath();

	int GetSQLEntryId(SQLEntry& sqlEntry);
	inline void InsertSQLEntry(SQLEntry& sqlEntry);

	void OpenDocument(int documentIndex);
	void OpenDocument(Document* document);
	bool ScanForDocumentLinear(int toFind, int& current, Document* scanBegin, Document*& emptyPointer);
	void NullifyStoryWriter();

	void AddBook(Book* book);
	void AddStoryElement(StoryElement* element, bool refreshElements);
	void AddDocument(Document* document, Book* book, int pos = -1);

	void ResortElements();

	void StartEditingElement(StoryElement* element);
	void DoEditStoryElement(StoryElement* original, StoryElement& edit, bool sort = false);

	void UpdateStoryElement(StoryElement* element);

	void UpdateStoryElementInGUI(StoryElement* element);
	void GoToStoryElement(StoryElement* element);

	void AddElementToDocument(TangibleElement* element, Document* document, bool addToDb = true);
	void RemoveElementFromDocument(TangibleElement* element, Document* document);

	void RelateStoryElements(StoryElement* element1, StoryElement* element2, bool addToDb = true);
	void UnrelateStoryElements(StoryElement* element1, StoryElement* element2, bool removeFromDb = true);

	StoryElement* GetStoryElementByName(const wxString& name);
	Book* GetBookByName(const wxString& name);

	void DeleteElement(StoryElement* element);
	void DeleteDocument(Document* document);

	Project* GetProject();

	int GetCurrentBookPos();
	Book* GetCurrentBook();

	wxVector<Book*>& GetBooks();
	wxVector<Character*> GetCharacters();
	wxVector<Location*> GetLocations();
	wxVector<Item*> GetItems();
	wxVector<StoryElement*>& GetAllElements();

	wxVector<Document*>& GetDocumentsInCurrentBook();
	Document* GetDocumentById(int id);
	Book* GetBookById(int id);

	wxArrayString GetCharacterNames();
	wxArrayString GetLocationNames();
	wxArrayString GetItemNames();

	wxArrayString GetBookTitles();

	unsigned int GetBookCount();
	unsigned int GetDocumentCount(int bookPos);
	unsigned int GetCharacterCount();
	unsigned int GetLocationCount();
	unsigned int GetItemCount();
}

#endif