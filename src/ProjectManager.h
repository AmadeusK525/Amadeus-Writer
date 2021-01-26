#ifndef PROJECTMANAGER_H_
#define PROJECTMANAGER_H_
#pragma once

#include <wx\wx.h>
#include <wx\filename.h>
//#include <wx\wxsqlite3.h>

#include <list>

#include "Elements.h"
#include "Chapter.h"

class amdMainFrame;
class amdElementsNotebook;
class amdChaptersNotebook;
class amdOutline;
class amdRelease;

using std::string;
using std::list;
using std::vector;

class amdProjectManager {
private:
	amdMainFrame* m_mainFrame = nullptr;
	
	amdElementsNotebook* m_elements = nullptr;
	amdChaptersNotebook* m_chaptersNote = nullptr;
	amdOutline* m_outline = nullptr;
	amdRelease* m_release = nullptr;

	wxVector<Character> m_characters{};
	wxVector<Location> m_locations{};
	wxVector<Chapter> m_chapters{};

	wxFileName m_curDoc{};
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
	void SetCurrentDocPath(const wxString& path);

	amdMainFrame* GetMainFrame();
	amdElementsNotebook* GetElementsNotebook();
	amdChaptersNotebook* GetChaptersNotebook();
	amdOutline* GetOutline();
	amdRelease* GetRelease();

	wxString GetPath(bool withSeparator);
	wxString GetFullPath() { return m_curDoc.GetFullPath(); }
	wxString GetMainDir() { return m_curDoc.GetPathWithSep() + m_curDoc.GetName(); }
	wxString GetExecutablePath(bool withSeparator) { return m_executablePath.GetPath(withSeparator); }
	wxString GetTitle() { return m_curDoc.GetName(); }

	void ClearPath();

	bool IsSaved() { return m_isSaved; }
	void SetSaved(bool saved = true);

	void SetLastSave();
	bool GetLastSave();

	void AddCharacter(Character& character, int pos = -1);
	void AddLocation(Location& location, int pos = -1);
	void AddChapter(Chapter& chapter, int pos = -1);

	void EditCharacter(Character& original, Character& edit, bool sort = false);
	void EditLocation(Location& original, Location& edit, bool sort = false);

	void AddChapterToCharacter(const wxString& characterName, Chapter& chapter);
	void AddChapterToLocation(const wxString& locationName, Chapter& chapter);
	void RemoveChapterFromCharacter(const wxString& characterName, Chapter& chapter);
	void RemoveChapterFromLocation(const wxString& locationName, Chapter& chapter);
	void RedeclareChapsInElements();

	void DeleteCharacter(Character& character);
	void DeleteLocation(Location& location);
	void DeleteChapter(Chapter& chapter);

	wxVector<Character>& GetCharacters() { return m_characters; }
	wxVector<Location>& GetLocations() { return m_locations; }
	wxVector<Chapter>& GetChapters() { return m_chapters; }

	int GetChapterCount() { return m_chapters.size(); }
	int GetCharacterCount() { return m_characters.size(); }
	int GetLocationCount() { return m_locations.size(); }
};

#endif