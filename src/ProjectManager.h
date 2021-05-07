#ifndef PROJECTMANAGER_H_
#define PROJECTMANAGER_H_
#pragma once

#include <wx\wx.h>
#include <wx\sstream.h>
#include <wx\filename.h>

#include <wx\wxsqlite3.h>

#include "ProjectWizard.h"
#include "BookElements.h"
#include "SQLEntry.h"

///////////////////////////////////////////////////////////////////
//////////////////////////// SQLStorage ///////////////////////////
///////////////////////////////////////////////////////////////////


class amProjectSQLDatabase : public wxSQLite3Database
{
public:
	amProjectSQLDatabase() = default;
	amProjectSQLDatabase(wxFileName& path);

	bool Init();
	void CreateAllTables();

	int GetSQLEntryId(amSQLEntry& sqlEntry);

	bool CreateTable(const wxString& tableName, const wxArrayString& arguments, bool ifNotExists = true);

	bool InsertSQLEntry(amSQLEntry& sqlEntry);
	bool UpdateSQLEntry(amSQLEntry& original, amSQLEntry& edit);

	bool InsertManyToMany(const wxString& tableName,
		int sqlID1, const wxString& arg1,
		int sqlID2, const wxString& arg2);

	bool InsertManyToMany(const wxString& tableName,
		amSQLEntry& sqlEntry1, const wxString& arg1,
		amSQLEntry& sqlEntry2, const wxString& arg2);

	bool DeleteSQLEntry(amSQLEntry& sqlEntry);

	bool DeleteManyToMany(const wxString& tableName,
		int docID1, const wxString& arg1,
		int docID2, const wxString& arg2);

	bool DeleteManyToMany(const wxString& tableName,
		amSQLEntry& doc1, const wxString& arg1,
		amSQLEntry& doc2, const wxString& arg2);

	wxSQLite3Statement ConstructInsertStatement(amSQLEntry& sqlEntry);
	wxSQLite3Statement ConstructUpdateStatement(amSQLEntry& sqlEntry, int id);

	bool RowExists(amSQLEntry& sqlEntry);
};


///////////////////////////////////////////////////////////////////////
//////////////////////////// ProjectManager ///////////////////////////
///////////////////////////////////////////////////////////////////////


class amMainFrame;
class amOverview;
class amElementsNotebook;
class amStoryNotebook;
class amOutline;
class amRelease;

class amStoryWriter;

class amProjectManager
{
private:
	amProject m_project{};
	amProjectSQLDatabase m_storage;

	amMainFrame* m_mainFrame = nullptr;
	amOverview* m_overview = nullptr;
	amElementsNotebook* m_elements = nullptr;
	amStoryNotebook* m_storyNotebook = nullptr;
	amOutline* m_outline = nullptr;
	amRelease* m_release = nullptr;

	amStoryWriter* m_storyWriter = nullptr;

	wxFileName m_executablePath{};

	bool m_isInitialized = false;

	int m_currentBook = 1;

public:
	amProjectManager();
	virtual ~amProjectManager();

	bool Init();

	inline amProjectSQLDatabase* GetStorage() { return &m_storage; }

	bool SaveProject();
	bool LoadProject();

	void SaveSQLEntry(amSQLEntry& original, amSQLEntry& edit);

	bool DoSaveProject(const wxString& path);
	bool DoLoadProject(const wxString& path);

	void LoadBooks();
	void LoadBookContent(Book* book);
	void LoadDocuments(wxVector<Document*>& documents, int bookId);
	void SetupDocumentHierarchy(Book* book);

	void LoadCharacters();
	void LoadLocations();
	void LoadItems();

	void LoadStandardRelations();
	void LoadCharacterRelations();

	void SetExecutablePath(const wxString& path);
	void SetProjectFileName(const wxFileName& fileName);

	amMainFrame* GetMainFrame();
	amElementsNotebook* GetElementsNotebook();
	amStoryNotebook* GetStoryNotebook();
	amOutline* GetOutline();
	amRelease* GetRelease();

	wxString GetPath(bool withSeparator);
	inline wxString GetFullPath() { return m_project.amFile.GetFullPath(); }
	inline wxString GetExecutablePath(bool withSeparator) { return m_executablePath.GetPath(withSeparator); }
	inline wxString GetTitle() { return m_project.amFile.GetName(); }

	void ClearPath();

	// These two go together when saving / loading. setLast writes to a file the path to the most recently
	// worked on project. When booting up the application, the getLast function will
	// be called and it will get the written path, opening up the project automatically,
	// with no need to load it. Makes things more convenient.
	void SetLastSave();
	bool GetLastSave();

	int GetSQLEntryId(amSQLEntry& sqlEntry);
	inline void InsertSQLEntry(amSQLEntry& sqlEntry) { m_storage.InsertSQLEntry(sqlEntry); }

	void OpenDocument(int documentIndex);
	void OpenDocument(Document* document);
	bool ScanForDocumentLinear(int toFind, int& current, Document* scanBegin, Document*& emptyPointer);
	void NullifyStoryWriter() { m_storyWriter = nullptr; }

	void AddCharacter(Character* character, bool refreshElements);
	void AddLocation(Location* location, bool refreshElements);
	void AddItem(Item* item, bool refreshElements);
	void AddDocument(Document* document, Book* book, int pos = -1);

	void EditCharacter(Character* original, Character& edit, bool sort = false);
	void EditLocation(Location* original, Location& edit, bool sort = false);
	void EditItem(Item* original, Item& edit, bool sort = false);

	void AddElementToDocument(Element* element, Document* document, bool addToDb = true);
	void RemoveElementFromDocument(Element* element, Document* document);

	void RelateElements(Element* element1, Element* element2, bool addToDb = true);
	void UnrelateElements(Element* element1, Element* element2, bool removeFromDb = true);

	Element* GetElementByName(const wxString& name);

	void DeleteCharacter(Character* character, bool clearShowcase);
	void DeleteLocation(Location* location, bool clearShowcase);
	void DeleteItem(Item* item, bool clearShowcase);
	void DeleteDocument(Document* document);

	inline int GetCurrentBookPos() { return m_currentBook; }
	inline Book* GetCurrentBook() { return m_project.books[m_currentBook - 1]; }

	inline wxVector<Book*>& GetBooks() { return m_project.books; }
	wxVector<Character*>& GetCharacters();
	wxVector<Location*>& GetLocations();
	wxVector<Item*>& GetItems();
	wxVector<Element*> GetAllElements();

	wxVector<Document*>& GetDocumentsInCurrentBook();
	Document* GetDocumentById(int id);

	wxArrayString GetCharacterNames();
	wxArrayString GetLocationNames();
	wxArrayString GetItemNames();

	wxArrayString GetBookTitles();

	inline unsigned int GetBookCount() { return m_project.books.size(); }
	inline unsigned int GetDocumentCount(int book) { return m_project.GetDocuments(book).size(); }
	inline unsigned int GetCharacterCount() { return m_project.characters.size(); }
	inline unsigned int GetLocationCount() { return m_project.locations.size(); }
	inline unsigned int GetItemCount() { return m_project.items.size(); }
};

#endif