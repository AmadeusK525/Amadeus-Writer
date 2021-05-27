#ifndef ELEMENTSNOTEBOOK_H_
#define ELEMENTSNOTEBOOK_H_

#pragma once

#include <wx\wx.h>
#include <wx\aui\auibook.h>
#include <wx\listctrl.h>
#include <wx\combo.h>
#include <wx\srchctrl.h>

#include "ProjectManager.h"
#include "ElementShowcases.h"

class amCheckListBox;

enum
{
	LISTMENU_EditElement,
	LISTMENU_DeleteElement
};

class amElementNotebookPage : public amSplitterWindow 
{
private:
	wxClassInfo* m_elementType = nullptr;
	wxListView* m_elementList = nullptr;
	wxImageList* m_imageList = nullptr;

	wxChoice* m_sortBy = nullptr;
	wxVector<Book*> m_vBooksToShow;

	wxComboCtrl* m_show = nullptr;
	amCheckListBox* m_bookCheckList = nullptr;

	amElementShowcase* m_elementShowcase = nullptr;

public:
	amElementNotebookPage(wxWindow* parent, wxClassInfo* showcaseType, const wxArrayString& sortByChoices);

	bool ShouldShow(Element* element);
	void ClearAll();
	void InitShowChoices();

	void GoToElement(Element* element);

	void UpdateList();
	void UpdateElementInList(int n, Element* element);

	inline wxListView* GetList() { return m_elementList; }
	inline amElementShowcase* GetShowcase() { return m_elementShowcase; }

	void OnElementSelected(wxListEvent& event);
	void OnElementRightClick(wxListEvent& event);

	void OnEditElement(wxCommandEvent& event);
	void OnDeleteElement(wxCommandEvent& event);

	void OnElementsSortBy(wxCommandEvent& event);
	void OnCheckListBox(wxCommandEvent& event);
};

class amElementNotebook : public wxAuiNotebook
{
private:
	amProjectManager* m_manager = nullptr;

public:
	amElementNotebookPage* m_characterPage = nullptr,
		* m_locationPage = nullptr,
		* m_itemPage = nullptr;

	wxSearchCtrl* m_searchBar = nullptr;

public:
	amElementNotebook(wxWindow* parent);

	void InitShowChoices();

	wxListView* GetAppropriateList(Element* element);
	amElementNotebookPage* GetAppropriatePage(Element* element);

	inline amElementShowcase* GetCharacterShowcase() { return m_characterPage->GetShowcase(); }
	inline amElementShowcase* GetLocationShowcase() { return m_locationPage->GetShowcase(); }
	inline amElementShowcase* GetItemShowcase() { return m_itemPage->GetShowcase(); }

	inline wxListView* GetCharacterList() { return m_characterPage->GetList(); }
	inline wxListView* GetLocationList() { return m_locationPage->GetList(); }
	inline wxListView* GetItemList() { return m_itemPage->GetList(); }

	wxSearchCtrl* GetSearchBar() { return m_searchBar; }

	void GoToElement(Element* element);
	bool ShouldShow(Element* element) const;

	void UpdateSearchAutoComplete(wxBookCtrlEvent& event);

	void ClearAll();

	void RemoveElementFromList(Element* element);
	void UpdateElementInList(int n, Element* element);

	inline void UpdateCharacterList() { m_characterPage->UpdateList(); }
	inline void UpdateLocationList() { m_locationPage->UpdateList(); }
	inline void UpdateItemList() { m_itemPage->UpdateList(); }
	void UpdateAll();
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

		for ( int& it : selections )
		{
			if ( !first )
				string << ", ";
			else
				first = false;

			string << GetString(it);
		}

		if ( string.IsEmpty() )
			string = "All";

		return string;
	}

	virtual wxSize GetAdjustedSize(int minWidth, int prefHeight, int maxHeight)
	{
		return wxSize(minWidth, GetBestSize().y);
	}

	void OnMouseClick(wxMouseEvent& event)
	{
		GetComboCtrl()->SetText(GetStringValue());
		event.Skip();
	}
};


#endif