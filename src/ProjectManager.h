#ifndef PROJECTMANAGER_H_
#define PROJECTMANAGER_H_
#pragma once

#include <wx\wx.h>
#include <wx\filename.h>

#include <wx\wxsqlite3.h>

#include "ProjectWizard.h"
#include "BookElements.h"

///////////////////////////////////////////////////////////////////
//////////////////////////// SQLStorage ///////////////////////////
///////////////////////////////////////////////////////////////////


class amdProjectSQLDatabase : public wxSQLite3Database {
public:
	amdProjectSQLDatabase() = default;
	amdProjectSQLDatabase(wxFileName& path);

	void Init();
	void CreateAllTables();
};


///////////////////////////////////////////////////////////////////////
//////////////////////////// ProjectManager ///////////////////////////
///////////////////////////////////////////////////////////////////////


class amdMainFrame;
class amdElementsNotebook;
class amdChaptersNotebook;
class amdOutline;
class amdRelease;

class amdProjectManager {
private:
	amdProject m_project{};
	amdProjectSQLDatabase m_storage;

	amdMainFrame* m_mainFrame = nullptr;
	amdElementsNotebook* m_elements = nullptr;
	amdChaptersNotebook* m_chaptersNote = nullptr;
	amdOutline* m_outline = nullptr;
	amdRelease* m_release = nullptr;

	wxVector<Character> m_characters{};
	wxVector<Location> m_locations{};
	wxVector<Item> m_items{};
	wxVector<Chapter> m_chapters{};

	wxFileName m_executablePath{};

	bool m_isInitialized = false;
	bool m_isSaved = true;

public:
	amdProjectManager();

	bool Init();

	bool SaveProject();
	bool LoadProject();
	bool DoSaveProject(const wxString& path);
	bool DoLoadProject(const wxString& path);

	void SetExecutablePath(const wxString& path);
	void SetProjectFileName(const wxFileName& fileName);

	amdMainFrame* GetMainFrame();
	amdElementsNotebook* GetElementsNotebook();
	amdChaptersNotebook* GetChaptersNotebook();
	amdOutline* GetOutline();
	amdRelease* GetRelease();

	wxString GetPath(bool withSeparator);
	wxString GetFullPath() { return m_project.amdFile.GetFullPath(); }
	wxString GetExecutablePath(bool withSeparator) { return m_executablePath.GetPath(withSeparator); }
	wxString GetTitle() { return m_project.amdFile.GetName(); }

	void ClearPath();

	bool IsSaved() { return m_isSaved; }
	void SetSaved(bool saved = true);

	// These two go together when saving / loading. setLast writes to a file the path to the most recently
	// worked on project. When booting up the application, the getLast function will
	// be called and it will get the written path, opening up the project automatically,
	// with no need to load it. Makes things more convenient.
	void SetLastSave();
	bool GetLastSave();

	void AddCharacter(Character& character, int pos = -1);
	void AddLocation(Location& location, int pos = -1);
	void AddItem(Item& item, int pos = -1);
	void AddChapter(Chapter& chapter, int pos = -1);

	void EditCharacter(Character& original, Character& edit, bool sort = false);
	void EditLocation(Location& original, Location& edit, bool sort = false);
	void EditItem(Item& original, Item& edit, bool sort = false);

	void AddChapterToCharacter(const wxString& characterName, Chapter& chapter);
	void AddChapterToLocation(const wxString& locationName, Chapter& chapter);
	void AddChapterToItem(const wxString& itemName, Chapter& chapter);
	void RemoveChapterFromCharacter(const wxString& characterName, Chapter& chapter);
	void RemoveChapterFromLocation(const wxString& locationName, Chapter& chapter);
	void RemoveChapterFromItem(const wxString& itemName, Chapter& chapter);
	void RedeclareChapsInElements();

	void DeleteCharacter(Character& character);
	void DeleteLocation(Location& location);
	void DeleteItem(Item& item);
	void DeleteChapter(Chapter& chapter);

	wxVector<Book>& GetBooks() { return m_project.books; }
	wxVector<Character> GetCharacters(int bookPos);
	wxVector<Location>& GetLocations(int bookPos);
	wxVector<Item>& GetItems(int bookPos);
	wxVector<Chapter>& GetChapters(int bookPos);

	wxArrayString GetCharacterNames();
	wxArrayString GetLocationNames();
	wxArrayString GetItemNames();

	wxArrayString GetBookTitles();

	int GetBookCount() { return m_project.books.size(); }
	int GetChapterCount() { return m_chapters.size(); }
	int GetCharacterCount() { return m_characters.size(); }
	int GetLocationCount() { return m_locations.size(); }
	int GetItemCount() { return m_items.size(); }
};

#endif