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

class amElementNotebook : public wxNotebook
{
private:
	amProjectManager* m_manager = nullptr;

public:
	wxListView* m_charList = nullptr, * m_locList = nullptr, * m_itemList = nullptr;
	wxImageList* m_charImageList = nullptr, * m_locImageList = nullptr, * m_itemsImageList = nullptr;

	amCharacterShowcase* m_charShow = nullptr;
	amLocationShowcase* m_locShow = nullptr;
	amItemShowcase* m_itemShow = nullptr;

	wxChoice* m_cSortBy = nullptr,
		* m_lSortBy = nullptr,
		* m_iSortBy = nullptr;

	wxComboCtrl* m_cShow = nullptr;

	wxSearchCtrl* m_searchBar = nullptr;

public:
	amElementNotebook(wxWindow* parent);

	void InitShowChoices();

	inline amCharacterShowcase* GetCharacterShowcase() { return m_charShow; }
	inline amLocationShowcase* GetLocationShowcase() { return m_locShow; }
	inline amItemShowcase* GetItemShowcase() { return m_itemShow; }

	wxSearchCtrl* GetSearchBar() { return m_searchBar; }

	void GoToElement(Element* element);

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

	void UpdateCharacter(int n, Character* character);
	void UpdateLocation(int n, Location* location);
	void UpdateItem(int n, Item* item);

	void UpdateCharacterList();
	void UpdateLocationList();
	void UpdateItemList();
	void UpdateAll();

	DECLARE_EVENT_TABLE()
};

enum
{
	NOTEBOOK_THIS,

	PANEL_Char,

	LIST_CharList,
	LIST_LocList,
	LIST_ItemList,

	LISTMENU_EditChar,
	LISTMENU_EditLoc,
	LISTMENU_EditItem,
	LISTMENU_DeleteChar,
	LISTMENU_DeleteLoc,
	LISTMENU_DeleteItem
};


class amCheckListBox : public wxCheckListBox, public wxComboPopup
{
public:
	virtual void Init()
	{
		Bind(wxEVT_LEFT_UP, &amCheckListBox::OnMouseClick, this);
	}

	virtual bool Create(wxWindow* parent)
	{
		return wxCheckListBox::Create(parent, -1);
	}

	virtual wxWindow* GetControl() { return this; }

	virtual void SetStringValue(const wxString& s)
	{
		int n = FindString(s);
		Check(n);
	}

	virtual wxString GetStringValue() const
	{
		wxString string("");
		wxArrayInt selections;
		GetCheckedItems(selections);

		bool first = true;

		for ( auto& it : selections )
		{
			if ( !first )
				string << ", ";

			string << GetString(it);
			first = false;
		}
		return string;
	}

	virtual wxSize GetAdjustedSize(int minWidth, int prefHeight, int maxHeight)
	{
		return wxSize(minWidth, GetBestSize().y);
	}

	void OnMouseClick(wxMouseEvent& event)
	{
		int n = HitTest(event.GetPosition());

		Check(n, !IsChecked(n));
		GetComboCtrl()->SetText(GetStringValue());
	}
};


#endif