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
	bool InsertManyToMany(wxString& tableName,
		amDocument& doc1,
		wxString& arg1,
		amDocument& doc2,
		wxString& arg2);

	bool DeleteDocument(amDocument& document);
	bool DeleteManyToMany(wxString& tableName,
		amDocument& doc1,
		wxString& arg1,
		amDocument& doc2,
		wxString& arg2);

	wxSQLite3Statement ConstructInsertStatement(amDocument& document);
	wxSQLite3Statement ConstructUpdateStatement(amDocument& document, int id);

	bool RowExists(amDocument& document);
};


///////////////////////////////////////////////////////////////////////
//////////////////////////// ProjectManager ///////////////////////////
///////////////////////////////////////////////////////////////////////


class amMainFrame;
class amElementsNotebook;
class amChaptersNotebook;
class amOutline;
class amRelease;

class amProjectManager {
private:
	amProject m_project{};
	amProjectSQLDatabase m_storage;

	amMainFrame* m_mainFrame = nullptr;
	amElementsNotebook* m_elements = nullptr;
	amChaptersNotebook* m_chaptersNote = nullptr;
	amOutline* m_outline = nullptr;
	amRelease* m_release = nullptr;

	wxFileName m_executablePath{};

	bool m_isInitialized = false;
	bool m_isSaved = true;

public:
	amProjectManager();

	bool Init();

	bool SaveProject();
	bool LoadProject();
	bool DoSaveProject(const wxString& path);
	bool DoLoadProject(const wxString& path);

	void LoadCharacters();
	void LoadLocations();
	void LoadItems();

	void SetExecutablePath(const wxString& path);
	void SetProjectFileName(const wxFileName& fileName);

	amMainFrame* GetMainFrame();
	amElementsNotebook* GetElementsNotebook();
	amChaptersNotebook* GetChaptersNotebook();
	amOutline* GetOutline();
	amRelease* GetRelease();

	wxString GetPath(bool withSeparator);
	wxString GetFullPath() { return m_project.amFile.GetFullPath(); }
	wxString GetExecutablePath(bool withSeparator) { return m_executablePath.GetPath(withSeparator); }
	wxString GetTitle() { return m_project.amFile.GetName(); }

	void ClearPath();

	bool IsSaved() { return m_isSaved; }
	void SetSaved(bool saved = true);

	// These two go together when saving / loading. setLast writes to a file the path to the most recently
	// worked on project. When booting up the application, the getLast function will
	// be called and it will get the written path, opening up the project automatically,
	// with no need to load it. Makes things more convenient.
	void SetLastSave();
	bool GetLastSave();
	
	int GetDocumentId(amDocument& document);
	void InsertDocument(amDocument& document) { m_storage.InsertDocument(document); }

	void AddCharacter(Character& character, bool refreshElements);
	void AddLocation(Location& location, bool refreshElements);
	void AddItem(Item& item, bool refreshElements);
	void AddChapter(Chapter& chapter, Book& book, int section = 1, int pos = -1);

	void EditCharacter(Character& original, Character& edit, bool sort = false);
	void EditLocation(Location& original, Location& edit, bool sort = false);
	void EditItem(Item& original, Item& edit, bool sort = false);

	void AddChapterToCharacter(const wxString& characterName, Chapter& chapter);
	void AddChapterToLocation(const wxString& locationName, Chapter& chapter);
	void AddChapterToItem(const wxString& itemName, Chapter& chapter);

	void RemoveChapterFromCharacter(const wxString& characterName, Chapter& chapter);
	void RemoveChapterFromLocation(const wxString& locationName, Chapter& chapter);
	void RemoveChapterFromItem(const wxString& itemName, Chapter& chapter);

	void RedeclareChapsInElements(Section& section);

	void DeleteCharacter(Character& character);
	void DeleteLocation(Location& location);
	void DeleteItem(Item& item);
	void DeleteChapter(Chapter& chapter, Section& section);

	wxVector<Book>& GetBooks() { return m_project.books; }
	wxVector<Character>& GetCharacters();
	wxVector<Location>& GetLocations();
	wxVector<Item>& GetItems();
	wxVector<Chapter>& GetChapters(int bookPos, int sectionPos);

	wxVector<Chapter> GetChapters(int bookPos);

	wxArrayString GetCharacterNames();
	wxArrayString GetLocationNames();
	wxArrayString GetItemNames();

	wxArrayString GetBookTitles();

	unsigned int GetBookCount() { return m_project.books.size(); }
	unsigned int GetChapterCount(int book) { return m_project.GetChapters(book).size(); }
	unsigned int GetCharacterCount() { return m_project.characters.size(); }
	unsigned int GetLocationCount() { return m_project.locations.size(); }
	unsigned int GetItemCount() { return m_project.items.size(); }
};

#endif