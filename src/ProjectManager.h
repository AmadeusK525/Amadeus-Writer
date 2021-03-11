#ifndef PROJECTMANAGER_H_
#define PROJECTMANAGER_H_
#pragma once

#include <wx\wx.h>
#include <wx\sstream.h>
#include <wx\filename.h>

#include <wx\wxsqlite3.h>

#include "ProjectWizard.h"
#include "BookElements.h"
#include "Document.h"

///////////////////////////////////////////////////////////////////
//////////////////////////// SQLStorage ///////////////////////////
///////////////////////////////////////////////////////////////////


class amProjectSQLDatabase : public wxSQLite3Database {
public:
	amProjectSQLDatabase() = default;
	amProjectSQLDatabase(wxFileName& path);

	bool Init();
	void CreateAllTables();

	int GetDocumentId(amDocument& document);

	bool CreateTable(const wxString& tableName, const wxArrayString& arguments, bool ifNotExists = true);

	bool InsertDocument(amDocument& document);
	bool UpdateDocument(amDocument& original, amDocument& edit);
	
	bool InsertManyToMany(const wxString& tableName,
		int docID1, const wxString& arg1,
		int docID2, const wxString& arg2);

	bool InsertManyToMany(const wxString& tableName,
		amDocument& doc1, const wxString& arg1,
		amDocument& doc2, const wxString& arg2);

	bool DeleteDocument(amDocument& document);
	
	bool DeleteManyToMany(const wxString& tableName,
		int docID1, const wxString& arg1,
		int docID2,	const wxString& arg2);

	bool DeleteManyToMany(const wxString& tableName,
		amDocument& doc1, const wxString& arg1,
		amDocument& doc2, const wxString& arg2);

	wxSQLite3Statement ConstructInsertStatement(amDocument& document);
	wxSQLite3Statement ConstructUpdateStatement(amDocument& document, int id);

	bool RowExists(amDocument& document);
};


///////////////////////////////////////////////////////////////////////
//////////////////////////// ProjectManager ///////////////////////////
///////////////////////////////////////////////////////////////////////


class amMainFrame;
class amElementsNotebook;
class amStoryNotebook;
class amOutline;
class amRelease;

class amProjectManager {
private:
	amProject m_project{};
	amProjectSQLDatabase m_storage;

	amMainFrame* m_mainFrame = nullptr;
	amElementsNotebook* m_elements = nullptr;
	amStoryNotebook* m_storyNotebook = nullptr;
	amOutline* m_outline = nullptr;
	amRelease* m_release = nullptr;

	wxFileName m_executablePath{};

	bool m_isInitialized = false;

	int m_currentBook = 1;

public:
	amProjectManager();

	bool Init();

	inline amProjectSQLDatabase* GetStorage() { return &m_storage; }

	bool SaveProject();
	bool LoadProject();

	void SaveDocument(amDocument& original, amDocument& edit);

	bool DoSaveProject(const wxString& path);
	bool DoLoadProject(const wxString& path);

	void LoadBooks();
	void LoadSections(wxVector<Section>& sections, int bookId);
	void LoadChapters(wxVector<Chapter>& chapters, int sectionId);
	void LoadScenes(wxVector<Scene>& scenes, int chapterId, bool loadBuffers);

	void LoadCharacters();
	void LoadLocations();
	void LoadItems();

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
	
	int GetDocumentId(amDocument& document);
	inline void InsertDocument(amDocument& document) { m_storage.InsertDocument(document); }

	void AddCharacter(Character& character, bool refreshElements);
	void AddLocation(Location& location, bool refreshElements);
	void AddItem(Item& item, bool refreshElements);
	void AddChapter(Chapter& chapter, Book& book, int section = 1, int pos = -1);

	void EditCharacter(Character& original, Character& edit, bool sort = false);
	void EditLocation(Location& original, Location& edit, bool sort = false);
	void EditItem(Item& original, Item& edit, bool sort = false);

	void AddChapterToCharacter(const wxString& characterName, Chapter& chapter, bool addToDb = true);
	void AddChapterToLocation(const wxString& locationName, Chapter& chapter, bool addToDb = true);
	void AddChapterToItem(const wxString& itemName, Chapter& chapter, bool addToDb = true);

	void RemoveChapterFromCharacter(const wxString& characterName, Chapter& chapter);
	void RemoveChapterFromLocation(const wxString& locationName, Chapter& chapter);
	void RemoveChapterFromItem(const wxString& itemName, Chapter& chapter);

	void RedeclareChapsInElements(Book& book);

	void DeleteCharacter(Character& character);
	void DeleteLocation(Location& location);
	void DeleteItem(Item& item);
	void DeleteChapter(Chapter& chapter, Section& section);

	inline int GetCurrentBookPos() { return m_currentBook; }
	inline Book& GetCurrentBook() { return m_project.books[m_currentBook - 1]; }

	inline wxVector<Book>& GetBooks() { return m_project.books; }
	wxVector<Character>& GetCharacters();
	wxVector<Location>& GetLocations();
	wxVector<Item>& GetItems();
	wxVector<Chapter>& GetChapters(int bookPos, int sectionPos);

	wxVector<Chapter> GetChapters(int bookPos);

	wxArrayString GetCharacterNames();
	wxArrayString GetLocationNames();
	wxArrayString GetItemNames();

	wxArrayString GetBookTitles();

	inline unsigned int GetBookCount() { return m_project.books.size(); }
	inline unsigned int GetChapterCount(int book) { return m_project.GetChapters(book).size(); }
	inline unsigned int GetCharacterCount() { return m_project.characters.size(); }
	inline unsigned int GetLocationCount() { return m_project.locations.size(); }
	inline unsigned int GetItemCount() { return m_project.items.size(); }
};

#endif