#ifndef ELEMENTSNOTEBOOK_H_
#define ELEMENTSNOTEBOOK_H_

#pragma once

#include <wx\wx.h>
#include <wx\notebook.h>
#include <wx\listctrl.h>
#include <wx\srchctrl.h>

#include "ProjectManager.h"
#include "ElementShowcases.h"

class amdElementsNotebook : public wxNotebook {
private:
	amdProjectManager* m_manager = nullptr;

public:
	CharacterShowcase* m_charShow = nullptr;
	LocationShowcase* m_locShow = nullptr;

	wxChoice* m_cSortBy = nullptr,
		* m_lSortBy = nullptr,
		* m_iSortBy = nullptr;

	wxSearchCtrl* m_searchBar = nullptr;

	wxArrayString m_charNames{};
	wxArrayString m_locNames{};

public:
	amdElementsNotebook(wxWindow* parent);

	CharacterShowcase* GetCharacterShowcase() { return m_charShow; }
	LocationShowcase* GetLocationShowcase() { return m_locShow; }
	wxArrayString& GetCharacterNames() { return m_charNames; }
	wxArrayString& GetLocationNames() { return m_locNames; }
	wxSearchCtrl* GetSearchBar() { return m_searchBar; }

	void OnCharRightClick(wxListEvent& event);
	void OnEditCharName(wxListEvent& event);
	void OnEditCharacter(wxCommandEvent& event);
	void OnDeleteCharacter(wxCommandEvent& event);
	void OnCharacterActivated(wxListEvent& event);

	void OnLocRightClick(wxListEvent& event);
	void OnEditLocation(wxCommandEvent& event);
	void OnDeleteLocation(wxCommandEvent& event);
	void OnLocationActivated(wxListEvent& event);

	void OnCharacterSelected(wxListEvent& event);
	void OnLocationSelected(wxListEvent& event);

	void OnCharactersSortBy(wxCommandEvent& event);
	void OnLocationsSortBy(wxCommandEvent& event);

	void SetSearchAC(wxBookCtrlEvent& event);
	void AddCharName(string& name);
	void AddLocName(string& name);
	void RemoveCharacterName(string& name);
	void RemoveLocationName(string& name);

	void ClearAll();
	void UpdateCharacter(int n, Character& character);
	void UpdateLocation(int n, Location& location);
	void UpdateCharacterList();
	void UpdateLocationList();
	void UpdateAll();

	static wxListView* m_charList;
	static wxListView* m_locList;
	static wxListView* m_itemsList;

	DECLARE_EVENT_TABLE()
};

enum {
	NOTEBOOK_THIS,

	PANEL_Char,

	LIST_CharList,
	LIST_LocList,
	LIST_ItemsList,
	LIST_ScenesList,

	LISTMENU_EditChar,
	LISTMENU_DeleteChar,
	LISTMENU_EditLoc,
	LISTMENU_DeleteLoc
};

#endif