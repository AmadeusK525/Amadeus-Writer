#ifndef ELEMENTSNOTEBOOK_H_
#define ELEMENTSNOTEBOOK_H_

#pragma once

#include <wx\wx.h>
#include <wx\notebook.h>
#include <wx\listctrl.h>
#include <wx\combo.h>
#include <wx\srchctrl.h>

#include "ProjectManager.h"
#include "ElementShowcases.h"

class amdElementsNotebook : public wxNotebook {
private:
	amdProjectManager* m_manager = nullptr;

public:
	amdCharacterShowcase* m_charShow = nullptr;
	amdLocationShowcase* m_locShow = nullptr;
	amdItemShowcase* m_itemShow = nullptr;

	wxChoice* m_cSortBy = nullptr,
		* m_lSortBy = nullptr,
		* m_iSortBy = nullptr;

	wxComboCtrl* m_cShow = nullptr;

	wxSearchCtrl* m_searchBar = nullptr;

public:
	amdElementsNotebook(wxWindow* parent);

	amdCharacterShowcase* GetCharacterShowcase() { return m_charShow; }
	amdLocationShowcase* GetLocationShowcase() { return m_locShow; }

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

	void OnItemRightClick(wxListEvent& event);
	void OnEditItem(wxCommandEvent& event);
	void OnDeleteItem(wxCommandEvent& event);
	void OnItemActivated(wxListEvent& event);

	void OnCharacterSelected(wxListEvent& event);
	void OnLocationSelected(wxListEvent& event);
	void OnItemSelected(wxListEvent& event);

	void OnCharactersSortBy(wxCommandEvent& event);
	void OnLocationsSortBy(wxCommandEvent& event);
	void OnItemsSortBy(wxCommandEvent& event);

	void SetSearchAC(wxBookCtrlEvent& event);

	void ClearAll();

	void UpdateCharacter(int n, Character& character);
	void UpdateLocation(int n, Location& location);
	void UpdateItem(int n, Item& item);

	void UpdateCharacterList();
	void UpdateLocationList();
	void UpdateItemList();
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
	LISTMENU_EditLoc,
	LISTMENU_EditItem,
	LISTMENU_DeleteChar,
	LISTMENU_DeleteLoc,
	LISTMENU_DeleteItem
};


class amdCheckListBox : public wxCheckListBox, public wxComboPopup {
public:
	virtual void Init() {
		Bind(wxEVT_LEFT_UP, &amdCheckListBox::OnMouseClick, this);
	}

	virtual bool Create(wxWindow* parent) {
		return wxCheckListBox::Create(parent, -1);
	}
	
	virtual wxWindow* GetControl() { return this; }

	virtual void SetStringValue(const wxString& s) {
		int n = FindString(s);
		Check(n);
	}

	virtual wxString GetStringValue() const {
		wxString string("");
		wxArrayInt selections;
		GetCheckedItems(selections);

		bool first = true;

		for (auto& it: selections) {
			if (!first)
				string << ", ";

			string << GetString(it);
			first = false;
		}
		return string;
	}

	virtual wxSize GetAdjustedSize(int minWidth, int prefHeight, int maxHeight) {
		return wxSize(minWidth, GetBestSize().y);
	}

	void OnMouseClick(wxMouseEvent& event) {
		int n = HitTest(event.GetPosition());
		
		Check(n, !IsChecked(n));
		GetComboCtrl()->SetText(GetStringValue());
	}
};


#endif