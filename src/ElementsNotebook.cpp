#include "ElementsNotebook.h"

#include "ElementCreators.h"
#include "ElementShowcases.h"

#include "MainFrame.h"
#include "Outline.h"
#include "OutlineFiles.h"

#include "MyApp.h"
#include "amUtility.h"
#include "SortFunctions.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(amElementNotebook, wxNotebook)

EVT_LIST_ITEM_FOCUSED(LIST_CharList, amElementNotebook::OnCharacterSelected)
EVT_LIST_ITEM_SELECTED(LIST_LocList, amElementNotebook::OnLocationSelected)
EVT_LIST_ITEM_SELECTED(LIST_ItemList, amElementNotebook::OnItemSelected)

EVT_LIST_ITEM_RIGHT_CLICK(LIST_CharList, amElementNotebook::OnCharRightClick)
EVT_LIST_ITEM_ACTIVATED(LIST_CharList, amElementNotebook::OnCharacterActivated)
EVT_MENU(LISTMENU_EditChar, amElementNotebook::OnEditCharacter)
EVT_LIST_END_LABEL_EDIT(LIST_CharList, amElementNotebook::OnEditCharName)
EVT_MENU(LISTMENU_DeleteChar, amElementNotebook::OnDeleteCharacter)

EVT_LIST_ITEM_RIGHT_CLICK(LIST_LocList, amElementNotebook::OnLocRightClick)
EVT_LIST_ITEM_ACTIVATED(LIST_LocList, amElementNotebook::OnLocationActivated)
EVT_MENU(LISTMENU_EditLoc, amElementNotebook::OnEditLocation)
EVT_MENU(LISTMENU_DeleteLoc, amElementNotebook::OnDeleteLocation)

EVT_LIST_ITEM_RIGHT_CLICK(LIST_ItemList, amElementNotebook::OnItemRightClick)
EVT_LIST_ITEM_ACTIVATED(LIST_ItemList, amElementNotebook::OnItemActivated)
EVT_MENU(LISTMENU_EditItem, amElementNotebook::OnEditItem)
EVT_MENU(LISTMENU_DeleteItem, amElementNotebook::OnDeleteItem)

EVT_NOTEBOOK_PAGE_CHANGED(NOTEBOOK_THIS, amElementNotebook::SetSearchAC)

END_EVENT_TABLE()

amElementNotebook::amElementNotebook(wxWindow* parent) :
	wxNotebook(parent, NOTEBOOK_THIS, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	m_manager = amGetManager();

	//Setting up first notebook tab with a characters list
	amSplitterWindow* charFrame = new amSplitterWindow(this, PANEL_Char);
	charFrame->SetBackgroundColour(wxColour(10, 10, 10));
	charFrame->SetSize(GetClientSize());
	charFrame->SetSashGravity(1.0);
	charFrame->SetMinimumPaneSize(20);

	wxPanel* cLeft = new wxPanel(charFrame);
	cLeft->SetBackgroundColour(wxColour(20, 20, 20));

	m_charList = new wxListView(cLeft, LIST_CharList, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
	m_charList->InsertColumn(0, "Name", wxLIST_FORMAT_CENTER, 190);
	m_charList->InsertColumn(1, "Role", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
	m_charList->InsertColumn(2, "First Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
	m_charList->InsertColumn(3, "Last Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
	m_charList->InsertColumn(4, "Documents", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);

	m_charList->SetBackgroundColour(wxColour(45, 45, 45));
	m_charList->SetForegroundColour(wxColour(245, 245, 245));

	m_charImageList = new wxImageList(24, 24);
	m_charList->AssignImageList(m_charImageList, wxIMAGE_LIST_SMALL);

	wxArrayString sortBy;
	sortBy.Add("Role");
	sortBy.Add("Name (A-Z)");
	sortBy.Add("Name (Z-A)");
	sortBy.Add("Documents");
	sortBy.Add("First appearance");
	sortBy.Add("Last appearance");

	wxStaticText* cSortByLabel = new wxStaticText(cLeft, -1, "Sort by:");
	cSortByLabel->SetForegroundColour(wxColour(250, 250, 250));
	cSortByLabel->SetFont(wxFontInfo(11).Bold());

	m_cSortBy = new wxChoice(cLeft, -1, wxDefaultPosition, wxDefaultSize, sortBy);
	m_cSortBy->Bind(wxEVT_CHOICE, &amElementNotebook::OnCharactersSortBy, this);
	m_cSortBy->SetSelection(0);

	wxStaticText* cShowLabel = new wxStaticText(cLeft, -1, "Show:");
	cShowLabel->SetForegroundColour(wxColour(250, 250, 250));
	cShowLabel->SetFont(wxFontInfo(11).Bold());

	m_cShow = new wxComboCtrl(cLeft, -1, "", wxDefaultPosition, wxSize(200, -1), wxCB_READONLY);
	amCheckListBox* cBooks = new amCheckListBox();

	m_cShow->SetPopupControl(cBooks);

	wxBoxSizer* cFooterSizer = new wxBoxSizer(wxHORIZONTAL);
	cFooterSizer->Add(cSortByLabel, wxSizerFlags(0).CenterVertical());
	cFooterSizer->AddSpacer(5);
	cFooterSizer->Add(m_cSortBy, wxSizerFlags(0).CenterVertical());
	cFooterSizer->AddStretchSpacer(1);
	cFooterSizer->Add(cShowLabel, wxSizerFlags(0).CenterVertical());
	cFooterSizer->AddSpacer(5);
	cFooterSizer->Add(m_cShow, wxSizerFlags(0).CenterVertical());

	wxBoxSizer* cLeftSizer = new wxBoxSizer(wxVERTICAL);
	cLeftSizer->Add(m_charList, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP, 5));
	cLeftSizer->AddSpacer(10);
	cLeftSizer->Add(cFooterSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	cLeft->SetSizer(cLeftSizer);

	m_charShow = new amCharacterShowcase(charFrame);

	charFrame->SplitVertically(cLeft, m_charShow);
	this->AddPage(charFrame, "Characters");

	//Setting up second notebook tab with a locations list
	amSplitterWindow* locFrame = new amSplitterWindow(this, wxID_ANY);
	locFrame->SetBackgroundColour(wxColour(0, 0, 0));
	locFrame->SetSize(GetClientSize());
	locFrame->SetMinimumPaneSize(20);
	locFrame->SetSashGravity(1.0);

	wxPanel* lLeft = new wxPanel(locFrame);
	lLeft->SetBackgroundColour(wxColour(20, 20, 20));

	m_locList = new wxListView(lLeft, LIST_LocList, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
	m_locList->InsertColumn(0, "Name of location", wxLIST_FORMAT_CENTER, 125);
	m_locList->InsertColumn(1, "Importance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
	m_locList->InsertColumn(2, "First Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
	m_locList->InsertColumn(3, "Last Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
	m_locList->InsertColumn(4, "Documents", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);

	m_locList->SetBackgroundColour(wxColour(45, 45, 45));
	m_locList->SetForegroundColour(wxColour(245, 245, 245));
	m_locList->SetMinSize(wxSize(300, 400));

	m_locImageList = new wxImageList(24, 24);
	m_locList->AssignImageList(m_locImageList, wxIMAGE_LIST_SMALL);

	wxStaticText* lSortByLabel = new wxStaticText(lLeft, -1, "Sort by:");
	lSortByLabel->SetForegroundColour(wxColour(250, 250, 250));
	lSortByLabel->SetFont(wxFontInfo(11).Bold());

	sortBy.Remove("Role");
	sortBy.Insert("Importance", 0);

	m_lSortBy = new wxChoice(lLeft, -1, wxDefaultPosition, wxDefaultSize, sortBy);
	m_lSortBy->Bind(wxEVT_CHOICE, &amElementNotebook::OnLocationsSortBy, this);
	m_lSortBy->SetSelection(0);

	wxBoxSizer* lSortBySizer = new wxBoxSizer(wxHORIZONTAL);
	lSortBySizer->Add(lSortByLabel, wxSizerFlags(0).CenterVertical());
	lSortBySizer->AddSpacer(5);
	lSortBySizer->Add(m_lSortBy, wxSizerFlags(0).CenterVertical());

	wxBoxSizer* lLeftSizer = new wxBoxSizer(wxVERTICAL);
	lLeftSizer->Add(m_locList, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP, 5));
	lLeftSizer->AddSpacer(10);
	lLeftSizer->Add(lSortBySizer, wxSizerFlags(0).Left().Border(wxLEFT | wxBOTTOM, 5));
	lLeft->SetSizer(lLeftSizer);

	m_locShow = new amLocationShowcase(locFrame);

	locFrame->SplitVertically(lLeft, m_locShow);
	this->AddPage(locFrame, "Locations");

	//Setting up third notebook tab
	amSplitterWindow* itemsFrame = new amSplitterWindow(this, wxID_ANY);
	itemsFrame->SetBackgroundColour(wxColour(0, 0, 0));
	itemsFrame->SetSize(GetClientSize());
	itemsFrame->SetMinimumPaneSize(20);
	itemsFrame->SetSashGravity(1.0);

	wxPanel* iLeft = new wxPanel(itemsFrame);
	iLeft->SetBackgroundColour(wxColour(20, 20, 20));

	m_itemList = new wxListView(iLeft, LIST_ItemList, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
	m_itemList->InsertColumn(0, "Name of item", wxLIST_FORMAT_CENTER, 120);
	m_itemList->InsertColumn(1, "Is Magic", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
	m_itemList->InsertColumn(2, "Importance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
	m_itemList->InsertColumn(3, "First Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
	m_itemList->InsertColumn(4, "Last Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
	m_itemList->InsertColumn(5, "Documents", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);

	m_itemList->SetBackgroundColour(wxColour(45, 45, 45));
	m_itemList->SetForegroundColour(wxColour(245, 245, 245));
	m_itemList->SetMinSize(wxSize(300, 400));

	m_itemsImageList = new wxImageList(24, 24);
	m_itemList->AssignImageList(m_itemsImageList, wxIMAGE_LIST_SMALL);

	wxStaticText* iSortByLabel = new wxStaticText(iLeft, -1, "Sort by:");
	iSortByLabel->SetForegroundColour(wxColour(250, 250, 250));
	iSortByLabel->SetFont(wxFontInfo(11).Bold());

	m_iSortBy = new wxChoice(iLeft, -1, wxDefaultPosition, wxDefaultSize, sortBy);
	m_iSortBy->Bind(wxEVT_CHOICE, &amElementNotebook::OnItemsSortBy, this);
	m_iSortBy->SetSelection(0);

	wxBoxSizer* iSortBySizer = new wxBoxSizer(wxHORIZONTAL);
	iSortBySizer->Add(iSortByLabel, wxSizerFlags(0).CenterVertical());
	iSortBySizer->AddSpacer(5);
	iSortBySizer->Add(m_iSortBy, wxSizerFlags(0).CenterVertical());

	wxBoxSizer* iLeftSizer = new wxBoxSizer(wxVERTICAL);
	iLeftSizer->Add(m_itemList, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP, 5));
	iLeftSizer->AddSpacer(10);
	iLeftSizer->Add(iSortBySizer, wxSizerFlags(0).Left().Border(wxLEFT | wxBOTTOM, 5));
	iLeft->SetSizer(iLeftSizer);

	m_itemShow = new amItemShowcase(itemsFrame);

	itemsFrame->SplitVertically(iLeft, m_itemShow);
	this->AddPage(itemsFrame, "Items");
}

void amElementNotebook::InitShowChoices()
{
	amCheckListBox* pList = ((amCheckListBox*)m_cShow->GetPopupControl());
	pList->InsertItems(m_manager->GetBookTitles(), 0);
	pList->Check(0);
	m_cShow->SetText(m_manager->GetBookTitles()[0]);
}

void amElementNotebook::GoToElement(Element* element)
{
	Character* pCharacterToSet = dynamic_cast<Character*>(element);
	if ( pCharacterToSet )
	{
		SetSelection(0);

		int n = m_charList->FindItem(-1, pCharacterToSet->name);
		if ( n != -1 )
		{
			m_charShow->Freeze();
			m_charList->Select(n);
			m_charList->Focus(n);
			m_charShow->ShowPage(0);
			m_charShow->Thaw();

			m_charList->SetFocus();
		}

		return;
	}

	Location* pLocationToSet = dynamic_cast<Location*>(element);
	if ( pLocationToSet )
	{
		SetSelection(1);

		int n = m_locList->FindItem(-1, pLocationToSet->name);
		if ( n != -1 )
		{
			m_locShow->Freeze();
			m_locList->Select(n);
			m_locList->Focus(n);
			m_locShow->ShowPage(0);
			m_locShow->Thaw();

			m_locList->SetFocus();
		}

		return;
	}

	Item* pItemToSet = dynamic_cast<Item*>(element);
	if ( pItemToSet )
	{
		SetSelection(2);

		int n = m_itemList->FindItem(-1, pItemToSet->name);
		if ( n != -1 )
		{
			m_itemShow->Freeze();
			m_itemList->Select(n);
			m_itemList->Focus(n);
			m_itemShow->ShowPage(0);
			m_itemShow->Thaw();

			m_itemList->SetFocus();
		}

		return;
	}
}

void amElementNotebook::OnCharRightClick(wxListEvent& WXUNUSED(event))
{
	wxMenu menu;
	menu.Append(LISTMENU_EditChar, "&Edit");
	menu.Append(LISTMENU_DeleteChar, "&Delete");
	PopupMenu(&menu, wxDefaultPosition);
}

void amElementNotebook::OnEditCharacter(wxCommandEvent& WXUNUSED(event))
{
	amCharacterCreator* edit = new amCharacterCreator(m_manager->GetMainFrame(), m_manager, -1,
		"Edit character", wxDefaultPosition, FromDIP(wxSize(650, 650)));

	edit->CenterOnParent();
	edit->SetEdit(m_manager->GetCharacters()[m_charList->GetFirstSelected()]);
	edit->Show(true);

	m_manager->GetMainFrame()->Enable(false);
}

void amElementNotebook::OnEditCharName(wxListEvent& event)
{
	m_manager->GetCharacters()[m_charList->GetFirstSelected()]->name = event.GetLabel();
}

void amElementNotebook::OnDeleteCharacter(wxCommandEvent& WXUNUSED(event))
{
	long sel = m_charList->GetFirstSelected();

	wxMessageDialog deleteCheck(m_manager->GetMainFrame(), "Are you sure you want to delete '" + m_charList->GetItemText(sel) + "'?"
		"\nIf you have a thread bound to this character in the Timeline, it will be permantely deleted as well.",
		"Delete character", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

	if ( deleteCheck.ShowModal() == wxID_YES )
	{
		m_charList->DeleteItem(sel);
		m_manager->DeleteCharacter(m_manager->GetCharacters()[sel], true);
	}
}

void amElementNotebook::OnCharacterActivated(wxListEvent& WXUNUSED(event))
{
	OnEditCharacter(wxCommandEvent());
}

void amElementNotebook::OnLocRightClick(wxListEvent& WXUNUSED(event))
{
	wxMenu menu;
	menu.Append(LISTMENU_EditLoc, "&Edit");
	menu.Append(LISTMENU_DeleteLoc, "Delete");
	PopupMenu(&menu, wxDefaultPosition);
}

void amElementNotebook::OnEditLocation(wxCommandEvent& WXUNUSED(event))
{
	amLocationCreator* edit = new amLocationCreator(m_manager->GetMainFrame(), m_manager, -1,
		"Edit location - ''", wxDefaultPosition, FromDIP(wxSize(900, 650)));

	edit->CenterOnParent();
	edit->SetEdit(m_manager->GetLocations()[m_locList->GetFirstSelected()]);
	edit->Show(true);

	m_manager->GetMainFrame()->Enable(false);
}

void amElementNotebook::OnDeleteLocation(wxCommandEvent& WXUNUSED(event))
{
	long sel = m_locList->GetFirstSelected();

	wxMessageDialog deleteCheck(m_manager->GetMainFrame(), "Are you sure you want to delete '" + m_locList->GetItemText(sel) + "'?"
		"\nIf you have a thread bound to this location in the Timeline, it will be permantely deleted as well.",
		"Delete location", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

	if ( deleteCheck.ShowModal() == wxID_YES )
	{
		m_locList->DeleteItem(sel);
		m_manager->DeleteLocation(m_manager->GetLocations()[sel], true);
	}
}

void amElementNotebook::OnLocationActivated(wxListEvent& event)
{
	OnEditLocation(wxCommandEvent());
}

void amElementNotebook::OnItemRightClick(wxListEvent& event)
{
	wxMenu menu;
	menu.Append(LISTMENU_EditItem, "&Edit");
	menu.Append(LISTMENU_DeleteItem, "Delete");
	PopupMenu(&menu, wxDefaultPosition);
}

void amElementNotebook::OnEditItem(wxCommandEvent& event)
{
	amItemCreator* edit = new amItemCreator(m_manager->GetMainFrame(), m_manager, -1,
		"Edit item - ''", wxDefaultPosition, FromDIP(wxSize(900, 720)));

	edit->CenterOnParent();
	edit->SetEdit(m_manager->GetItems()[m_itemList->GetFirstSelected()]);
	edit->Show(true);

	m_manager->GetMainFrame()->Enable(false);
}

void amElementNotebook::OnDeleteItem(wxCommandEvent& event)
{
	long sel = m_itemList->GetFirstSelected();

	wxMessageDialog deleteCheck(m_manager->GetMainFrame(), "Are you sure you want to delete '" + m_itemList->GetItemText(sel) + "'?"
		"\nIf you have a thread bound to this item in the Timeline, it will be permantely deleted as well.",
		"Delete item", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

	if ( deleteCheck.ShowModal() == wxID_YES )
	{
		m_itemList->DeleteItem(sel);
		m_manager->DeleteItem(m_manager->GetItems()[sel], true);
	}
}

void amElementNotebook::OnItemActivated(wxListEvent& event)
{
	OnEditItem(wxCommandEvent());
}

void amElementNotebook::OnCharacterSelected(wxListEvent& event)
{
	long sel = m_charList->GetFirstSelected();

	if ( sel != -1 )
		m_charShow->SetData(m_manager->GetCharacters()[sel]);
	else
		m_charShow->SetData(nullptr);
}

void amElementNotebook::OnLocationSelected(wxListEvent& WXUNUSED(event))
{
	long sel = m_locList->GetFirstSelected();

	if ( sel != -1 )
		m_locShow->SetData(m_manager->GetLocations()[sel]);
	else
		m_locShow->SetData(nullptr);
}

void amElementNotebook::OnItemSelected(wxListEvent& event)
{
	long sel = m_itemList->GetFirstSelected();

	if ( sel != -1 )
		m_itemShow->SetData(m_manager->GetItems()[sel]);
	else
		m_itemShow->SetData(nullptr);
}

void amElementNotebook::OnCharactersSortBy(wxCommandEvent& event)
{
	m_cSortBy->SetSelection(event.GetInt());
	Character::cCompType = (CompType)event.GetInt();

	int currentSelection = m_charList->GetFirstSelected();

	wxVector<Element*>& vElements = m_manager->GetAllElements();
	std::sort(vElements.begin(), vElements.end(), amSortElements);

	for ( Element*& pElement : vElements )
	{
		std::sort(pElement->relatedElements.begin(), pElement->relatedElements.end(), amSortElements);
	}

	UpdateCharacterList();
	m_charList->SetFocus();

	Element* pCurElement = m_charShow->GetElement();

	if ( pCurElement )
	{
		int toSet = m_charList->FindItem(0, pCurElement->name);
    	m_charList->Select(toSet);
		m_charList->Focus(toSet);
	}
}

void amElementNotebook::OnLocationsSortBy(wxCommandEvent& event)
{
	m_lSortBy->SetSelection(event.GetInt());
	Location::lCompType = (CompType)event.GetInt();

	wxVector<Element*>& vElements = m_manager->GetAllElements();
	std::sort(vElements.begin(), vElements.end(), amSortElements);

	for ( Element*& pElement : vElements )
	{
		std::sort(pElement->relatedElements.begin(), pElement->relatedElements.end(), amSortElements);
	}


	UpdateLocationList();
	m_locList->SetFocus();

	Element* pCurElement = m_locShow->GetElement();

	if ( pCurElement )
	{
		int toSet = m_locList->FindItem(0, pCurElement->name);
		m_locList->Select(toSet);
		m_locList->Focus(toSet);
	}
}

void amElementNotebook::OnItemsSortBy(wxCommandEvent& event)
{
	m_iSortBy->SetSelection(event.GetInt());
	Item::iCompType = (CompType)event.GetInt();

	wxVector<Element*>& vElements = m_manager->GetAllElements();
	std::sort(vElements.begin(), vElements.end(), amSortElements);

	UpdateItemList();

	Element* pCurElement = m_itemShow->GetElement();

	if ( pCurElement )
	{
		int toSet = m_itemList->FindItem(0, pCurElement->name);
		m_itemList->SetFocus();
		m_itemList->Select(toSet);
		m_itemList->Focus(toSet);
	}
}

void amElementNotebook::ClearAll()
{
	m_charList->DeleteAllItems();
	m_locList->DeleteAllItems();
	m_itemList->DeleteAllItems();
	m_charShow->SetData(nullptr);
	m_locShow->SetData(nullptr);
	m_itemShow->SetData(nullptr);
}

void amElementNotebook::UpdateCharacter(int n, Character* character)
{
	m_charList->SetItem(n, 0, character->name);

	wxString role;
	switch ( character->role )
	{
	case cProtagonist:
		role = "Protagonist";
		break;

	case cSupporting:
		role = "Supporting";
		break;

	case cVillian:
		role = "Villian";
		break;

	case cSecondary:
		role = "Secondary";
		break;

	default:
		role = "-";
	}

	m_charList->SetItem(n, 1, role);

	Document* pFirstDocument = character->GetFirstDocument();
	if ( pFirstDocument )
		m_charList->SetItem(n, 2, wxString(_("Book ")) << pFirstDocument->book->pos << _(", Document ") << pFirstDocument->position);
	else
		m_charList->SetItem(n, 2, "-");

	Document* pLastDocument = character->GetLastDocument();
	if ( pLastDocument )
		m_charList->SetItem(n, 3, wxString(_("Book ")) << pLastDocument->book->pos << _(", Document ") << pLastDocument->position);
	else
		m_charList->SetItem(n, 3, "-");

	m_charList->SetItem(n, 4, std::to_string(character->documents.size()));

	if ( character->image.IsOk() )
		m_charList->SetItemColumnImage(n, 0, m_charImageList->Add(wxBitmap(amGetScaledImage(24, 24, character->image))));
	else
		m_charList->SetItemColumnImage(n, 0, -1);
}

void amElementNotebook::UpdateLocation(int n, Location* location)
{
	m_locList->SetItem(n, 0, location->name);

	wxString role("");
	switch ( location->role )
	{
	case lHigh:
		role = "High";
		break;

	case lLow:
		role = "Low";
		break;

	default:
		role = "-";
	}

	m_locList->SetItem(n, 1, role);

	if ( !location->documents.empty() )
	{
		int first = 999999;
		int last = -1;

		for ( Document* document : location->documents )
		{
			if ( document->position < first )
				first = document->position;

			if ( document->position > last )
				last = document->position;
		}

		m_locList->SetItem(n, 2, wxString("Document ") << first);
		m_locList->SetItem(n, 3, wxString("Document ") << last);
	}
	else
	{
		m_locList->SetItem(n, 2, "-");
		m_locList->SetItem(n, 3, "-");
	}
	m_locList->SetItem(n, 4, std::to_string(location->documents.size()));

	if ( location->image.IsOk() )
		m_locList->SetItemColumnImage(n, 0, m_locImageList->Add(wxBitmap(amGetScaledImage(24, 24, location->image))));
	else
		m_locList->SetItemColumnImage(n, 0, -1);
}

void amElementNotebook::UpdateItem(int n, Item* item)
{
	m_itemList->SetItem(n, 0, item->name);

	if ( item->isMagic )
		m_itemList->SetItem(n, 1, "Yes");
	else
		m_itemList->SetItem(n, 1, "No");

	wxString role("");
	switch ( item->role )
	{
	case iHigh:
		role = "High";
		break;

	case iLow:
		role = "Low";
		break;

	default:
		role = "-";
	}
	m_itemList->SetItem(n, 2, role);

	if ( !item->documents.empty() )
	{
		int first = 999999;
		int last = -1;

		for ( Document* document : item->documents )
		{
			if ( document->position < first )
				first = document->position;

			if ( document->position > last )
				last = document->position;
		}

		m_itemList->SetItem(n, 3, wxString("Document ") << first);
		m_itemList->SetItem(n, 4, wxString("Document ") << last);
	}
	else
	{
		m_itemList->SetItem(n, 3, "-");
		m_itemList->SetItem(n, 4, "-");
	}
	m_itemList->SetItem(n, 5, std::to_string(item->documents.size()));

	if ( item->image.IsOk() )
		m_itemList->SetItemColumnImage(n, 0, m_itemsImageList->Add(wxBitmap(amGetScaledImage(24, 24, item->image))));
	else
		m_itemList->SetItemColumnImage(n, 0, -1);
}

void amElementNotebook::UpdateCharacterList()
{
	m_charList->Freeze();

	int i = 0;
	int tlsize = m_charList->GetItemCount();
	int mfsize = m_manager->GetCharacterCount();
	int dif;

	m_charImageList->RemoveAll();

	if ( tlsize > mfsize )
	{
		dif = tlsize - mfsize;
		for ( int j = 0; j < dif; j++ )
			m_charList->DeleteItem(mfsize - j + 1);
	}
	else if ( mfsize > tlsize )
	{
		dif = mfsize - tlsize;
		for ( int j = 0; j < dif; j++ )
			m_charList->InsertItem(0, "");
	}

	for ( Character*& character : m_manager->GetCharacters() )
		UpdateCharacter(i++, character);

	m_charList->Thaw();
}

void amElementNotebook::UpdateLocationList()
{
	m_locList->Freeze();

	int i = 0;
	int tlsize = m_locList->GetItemCount();
	int mfsize = m_manager->GetLocationCount();
	int dif;

	if ( tlsize > mfsize )
	{
		dif = tlsize - mfsize;
		for ( int j = 0; j < dif; j++ )
			m_locList->DeleteItem(mfsize - j + 1);
	}
	else if ( mfsize > tlsize )
	{
		dif = mfsize - tlsize;
		for ( int j = 0; j < dif; j++ )
			m_locList->InsertItem(0, "");
	}

	for ( Location*& location : m_manager->GetLocations() )
		UpdateLocation(i++, location);

	m_locList->Thaw();
}

void amElementNotebook::UpdateItemList()
{
	m_itemList->Freeze();

	int i = 0;
	int tlsize = m_itemList->GetItemCount();
	int mfsize = m_manager->GetItemCount();
	int dif;

	if ( tlsize > mfsize )
	{
		dif = tlsize - mfsize;
		for ( int j = 0; j < dif; j++ )
			m_itemList->DeleteItem(mfsize - j + 1);
	}
	else if ( mfsize > tlsize )
	{
		dif = mfsize - tlsize;
		for ( int j = 0; j < dif; j++ )
			m_itemList->InsertItem(0, "");
	}

	for ( Item*& item : m_manager->GetItems() )
		UpdateItem(i++, item);

	m_itemList->Thaw();
}

void amElementNotebook::UpdateAll()
{
	UpdateCharacterList();
	UpdateLocationList();
	UpdateItemList();
}

void amElementNotebook::SetSearchAC(wxBookCtrlEvent& WXUNUSED(event))
{
	int sel = this->GetSelection();

	switch ( sel )
	{
	case 0:
		m_searchBar->AutoComplete(m_manager->GetCharacterNames());
		m_searchBar->SetDescriptiveText("Search character");
		break;

	case 1:
		m_searchBar->AutoComplete(m_manager->GetLocationNames());
		m_searchBar->SetDescriptiveText("Search location");
		break;

	case 2:
		m_searchBar->AutoComplete(m_manager->GetItemNames());
		m_searchBar->SetDescriptiveText("Search item");
		break;
	}

	m_searchBar->Clear();
}