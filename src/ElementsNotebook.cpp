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


amElementNotebookPage::amElementNotebookPage(wxWindow* parent, wxClassInfo* showcaseType, const wxArrayString& sortByChoices) :
	amSplitterWindow(parent)
{
	SetBackgroundColour(wxColour(10, 10, 10));
	SetSize(GetClientSize());
	SetSashGravity(1.0);
	SetMinimumPaneSize(20);

	wxPanel* left = new wxPanel(this);
	left->SetBackgroundColour(wxColour(20, 20, 20));

	m_elementList = new wxListView(left, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
	m_elementList->InsertColumn(0, "Name", wxLIST_FORMAT_CENTER, FromDIP(190));
	m_elementList->InsertColumn(1, sortByChoices.front(), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
	m_elementList->InsertColumn(2, "First Appearance", wxLIST_FORMAT_CENTER, FromDIP(130));
	m_elementList->InsertColumn(3, "Last Appearance", wxLIST_FORMAT_CENTER, FromDIP(130));
	m_elementList->InsertColumn(4, "Documents", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);

	m_elementList->SetBackgroundColour(wxColour(45, 45, 45));
	m_elementList->SetForegroundColour(wxColour(245, 245, 245));
	m_elementList->Bind(wxEVT_LIST_ITEM_ACTIVATED, [&](wxListEvent&) { OnEditElement(wxCommandEvent()); });
	m_elementList->Bind(wxEVT_LIST_ITEM_SELECTED, &amElementNotebookPage::OnElementSelected, this);
	m_elementList->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &amElementNotebookPage::OnElementRightClick, this);

	m_imageList = new wxImageList(24, 24);
	m_elementList->AssignImageList(m_imageList, wxIMAGE_LIST_SMALL);

	wxStaticText* sortByLabel = new wxStaticText(left, -1, "Sort by:");
	sortByLabel->SetForegroundColour(wxColour(250, 250, 250));
	sortByLabel->SetFont(wxFontInfo(11).Bold());

	m_sortBy = new wxChoice(left, -1, wxDefaultPosition, wxDefaultSize, sortByChoices);
	m_sortBy->Bind(wxEVT_CHOICE, &amElementNotebookPage::OnElementsSortBy, this);
	m_sortBy->SetSelection(0);

	wxStaticText* showLabel = new wxStaticText(left, -1, "Show:");
	showLabel->SetForegroundColour(wxColour(250, 250, 250));
	showLabel->SetFont(wxFontInfo(11).Bold());

	m_show = new wxComboCtrl(left, -1, "", wxDefaultPosition, wxSize(200, -1), wxCB_READONLY);
	m_bookCheckList = new amCheckListBox();
	m_bookCheckList->Bind(wxEVT_CHECKLISTBOX, &amElementNotebookPage::OnCheckListBox, this);

	m_show->SetPopupControl(m_bookCheckList);

	wxBoxSizer* footerSizer = new wxBoxSizer(wxHORIZONTAL);
	footerSizer->Add(sortByLabel, wxSizerFlags(0).CenterVertical());
	footerSizer->AddSpacer(5);
	footerSizer->Add(m_sortBy, wxSizerFlags(0).CenterVertical());
	footerSizer->AddStretchSpacer(1);
	footerSizer->Add(showLabel, wxSizerFlags(0).CenterVertical());
	footerSizer->AddSpacer(5);
	footerSizer->Add(m_show, wxSizerFlags(0).CenterVertical());

	wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
	leftSizer->Add(m_elementList, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP, 5));
	leftSizer->AddSpacer(10);
	leftSizer->Add(footerSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	left->SetSizer(leftSizer);

	m_elementShowcase = (amElementShowcase*)showcaseType->CreateObject();
	m_elementShowcase->Create(this);

	SplitVertically(left, m_elementShowcase);
}

bool amElementNotebookPage::ShouldShow(Element* element)
{
	if ( !element )
		return false;

	bool bShouldShow = false;

	if ( m_vBooksToShow.empty() )
		return true;

	for ( Book* const& pBook : m_vBooksToShow )
	{
		if ( element->IsInBook(pBook) )
		{
			bShouldShow = true;
			break;
		}
	}

	return bShouldShow;
}

void amElementNotebookPage::ClearAll()
{
	m_elementList->DeleteAllItems();
	m_elementShowcase->ClearAll();
}

void amElementNotebookPage::InitShowChoices()
{
	m_bookCheckList->InsertItems(amGetManager()->GetBookTitles(), 0);
	m_bookCheckList->Check(0);
	m_show->SetText(_("All"));
}

void amElementNotebookPage::GoToElement(Element * element)
{
	m_elementShowcase->Freeze();

	int n = m_elementList->FindItem(-1, element->name);
	if ( n != -1 )
	{
		m_elementList->Select(n);
		m_elementList->Focus(n);
		m_elementList->SetFocus();
	}
	else
	{
		m_elementShowcase->SetData(element);
		m_elementShowcase->SetFocus();
	}

	m_elementShowcase->ShowPage(0);
	m_elementShowcase->Thaw();
}

void amElementNotebookPage::UpdateList()
{
	amProjectManager* pManager = amGetManager();

	m_elementList->Freeze();
	m_imageList->RemoveAll();

	size_t sizeBefore = m_elementList->GetItemCount();

	wxVector<Element*> vElements;
	if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amCharacterShowcase)) )
	{
		for ( Character*& pCharacter : pManager->GetCharacters() )
			vElements.push_back(pCharacter);
	}
	else if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amLocationShowcase)) )
	{
		for ( Location*& pLocation : pManager->GetLocations() )
			vElements.push_back(pLocation);
	}
	else if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amItemShowcase)) )
	{
		for ( Item*& pItem : pManager->GetItems() )
			vElements.push_back(pItem);
	}

	int i = 0;
	for ( Element* const& pElement : vElements )
	{
		if ( !ShouldShow(pElement) )
			continue;

		if ( i >= sizeBefore )
			m_elementList->InsertItem(i, "");

		UpdateElementInList(i++, pElement);
	}

	size_t sizeAfter = i;

	for ( int i = sizeBefore; i > sizeAfter; i-- )
		m_elementList->DeleteItem(i - 1);

	m_elementList->Thaw();
}

void amElementNotebookPage::UpdateElementInList(int n, Element* element)
{
	m_elementList->SetItem(n, 0, element->name);

	wxString role;
	switch ( element->role )
	{
	case Role::cProtagonist:
		role = "Protagonist";
		break;

	case Role::cSupporting:
		role = "Supporting";
		break;

	case Role::cVillian:
		role = "Villian";
		break;

	case Role::cSecondary:
		role = "Secondary";
		break;

	case Role::lHigh:
	case Role::iHigh:
		role = _("High");
		break;

	case Role::lLow:
	case Role::iLow:
		role = _("Low");
		break;
	}

	m_elementList->SetItem(n, 1, role);

	Document* pFirstDocument = element->GetFirstDocument();
	if ( pFirstDocument )
		m_elementList->SetItem(n, 2, wxString(_("Book ")) << pFirstDocument->book->pos << _(", Document ") << pFirstDocument->position);
	else
		m_elementList->SetItem(n, 2, "-");

	Document* pLastDocument = element->GetLastDocument();
	if ( pLastDocument )
		m_elementList->SetItem(n, 3, wxString(_("Book ")) << pLastDocument->book->pos << _(", Document ") << pLastDocument->position);
	else
		m_elementList->SetItem(n, 3, "-");

	m_elementList->SetItem(n, 4, std::to_string(element->documents.size()));

	if ( element->image.IsOk() )
		m_elementList->SetItemColumnImage(n, 0, m_imageList->Add(wxBitmap(amGetScaledImage(24, 24, element->image))));
	else
		m_elementList->SetItemColumnImage(n, 0, -1);
}

void amElementNotebookPage::OnEditElement(wxCommandEvent& event)
{
	int n = m_elementList->GetFirstSelected();
	if ( n != -1 )
	{
		amProjectManager* pManager = amGetManager();
		Element* pElement = pManager->GetElementByName(m_elementList->GetItemText(n));
		
		if ( pElement )
			pManager->StartEditingElement(pElement);
	}
}

void amElementNotebookPage::OnDeleteElement(wxCommandEvent& event)
{
	amProjectManager* pManager = amGetManager();
	long sel = m_elementList->GetFirstSelected();

	wxMessageDialog deleteCheck(pManager->GetMainFrame(), "Are you sure you want to delete '" +
		m_elementList->GetItemText(sel) + "'?"
		"\nAll ties to this Element in other parts of the program (Outline files, Timeline, Document bindings...) will be permantely deleted as well.",
		"Delete element", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

	if ( deleteCheck.ShowModal() == wxID_YES )
	{
		m_elementList->DeleteItem(sel);
		pManager->DeleteElement(pManager->GetElementByName(m_elementList->GetItemText(sel)));
	}
}

void amElementNotebookPage::OnElementsSortBy(wxCommandEvent& event)
{
	amProjectManager* pManager = amGetManager();
	wxVector<Element*> vElements;

	if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amCharacterShowcase)) )
	{
		m_sortBy->SetSelection(event.GetInt());
		Character::cCompType = (CompType)event.GetInt();

		for ( Character*& pCharacter : pManager->GetCharacters() )
			vElements.push_back(pCharacter);
	}
	else if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amLocationShowcase)) )
	{
		m_sortBy->SetSelection(event.GetInt());
		Location::lCompType = (CompType)event.GetInt();

		for ( Location*& pLocation : pManager->GetLocations() )
			vElements.push_back(pLocation);
	}
	else if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amItemShowcase)) )
	{
		m_sortBy->SetSelection(event.GetInt());
		Item::iCompType = (CompType)event.GetInt();

		for ( Item*& pItem : pManager->GetItems() )
			vElements.push_back(pItem);
	}

	int currentSelection = m_elementList->GetFirstSelected();

	amGetManager()->ResortElements();
	UpdateList();

	m_elementList->SetFocus();

	Element* pCurElement = m_elementShowcase->GetElement();

	if ( pCurElement )
	{
		int toSet = m_elementList->FindItem(0, pCurElement->name);
		m_elementList->Select(toSet);
		m_elementList->Focus(toSet);
	}
}

void amElementNotebookPage::OnCheckListBox(wxCommandEvent & event)
{
	m_vBooksToShow.clear();
	
	wxArrayInt selections;
	m_bookCheckList->GetCheckedItems(selections);

	for ( int& sel : selections )
	{
		m_vBooksToShow.push_back(amGetManager()->GetBooks()[sel]);
	}

	UpdateList();

	Element* pDisplayedElement = m_elementShowcase->GetElement();
	if ( !ShouldShow(pDisplayedElement) )
	{
		m_elementShowcase->SetData(nullptr);
	}

	event.Skip();
}

void amElementNotebookPage::OnElementSelected(wxListEvent& event)
{
	long sel = m_elementList->GetFirstSelected();

	if ( sel != -1 )
		m_elementShowcase->SetData(amGetManager()->GetElementByName(m_elementList->GetItemText(sel)));
	else
		m_elementShowcase->SetData(nullptr);
}

void amElementNotebookPage::OnElementRightClick(wxListEvent & event)
{
	wxMenu menu;
	menu.Append(LISTMENU_EditElement, "&Edit");
	menu.Append(LISTMENU_DeleteElement, "&Delete");
	PopupMenu(&menu, wxDefaultPosition);
}


////////////////////////////////////////////////////////////////
////////////////////// amElementNotebook ///////////////////////
////////////////////////////////////////////////////////////////


amElementNotebook::amElementNotebook(wxWindow* parent) :
	wxNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	m_manager = amGetManager();

	//Setting up first notebook tab with a characters list
	wxArrayString sortBy;
	sortBy.Add("Role");
	sortBy.Add("Name (A-Z)");
	sortBy.Add("Name (Z-A)");
	sortBy.Add("Documents");
	sortBy.Add("First appearance");
	sortBy.Add("Last appearance");

	m_characterPage = new amElementNotebookPage(this, wxCLASSINFO(amCharacterShowcase), sortBy);
	this->AddPage(m_characterPage, "Characters");

	sortBy.Remove("Role");
	sortBy.Insert("Importance", 0);

	m_locationPage = new amElementNotebookPage(this, wxCLASSINFO(amLocationShowcase), sortBy);
	this->AddPage(m_locationPage, "Locations");

	m_itemPage = new amElementNotebookPage(this, wxCLASSINFO(amItemShowcase), sortBy);
	this->AddPage(m_itemPage, "Items");
}

void amElementNotebook::InitShowChoices()
{
	m_characterPage->InitShowChoices();
	m_locationPage->InitShowChoices();
	m_itemPage->InitShowChoices();
}

void amElementNotebook::GoToElement(Element* element)
{
	if ( element->IsKindOf(wxCLASSINFO(Character)) )
	{
		SetSelection(0);
		m_characterPage->GoToElement(element);
	}
	else if ( element->IsKindOf(wxCLASSINFO(Location)) )
	{
		SetSelection(1);
		m_characterPage->GoToElement(element);
	}
	else if ( element->IsKindOf(wxCLASSINFO(Item)) )
	{
		SetSelection(2);
		m_characterPage->GoToElement(element);
	}
}

bool amElementNotebook::ShouldShow(Element* element) const
{
	if ( element->IsKindOf(wxCLASSINFO(Character)) )
		return m_characterPage->ShouldShow(element);
	else if ( element->IsKindOf(wxCLASSINFO(Location)) )
		return m_locationPage->ShouldShow(element);
	else if ( element->IsKindOf(wxCLASSINFO(Item)) )
		return m_itemPage->ShouldShow(element);

	return false;
}

void amElementNotebook::ClearAll()
{
	m_characterPage->ClearAll();
	m_locationPage->ClearAll();
	m_itemPage->ClearAll();
}

void amElementNotebook::RemoveElementFromList(Element* element)
{
	wxListView* list = GetAppropriateList(element);

	if ( list )
	{
		long n = list->FindItem(0, element->name);
		if ( n )
			list->DeleteItem(n);
	}
}

void amElementNotebook::UpdateElementInList(int n, Element* element)
{
	amElementNotebookPage* page = GetAppropriatePage(element);

	if ( page )
		page->UpdateElementInList(n, element);
}

void amElementNotebook::UpdateAll()
{
	UpdateCharacterList();
	UpdateLocationList();
	UpdateItemList();
}

wxListView* amElementNotebook::GetAppropriateList(Element* element)
{
	wxListView* list = nullptr;
	amElementNotebookPage* page = GetAppropriatePage(element);

	if ( page )
		list = page->GetList();

	return list;
}

amElementNotebookPage* amElementNotebook::GetAppropriatePage(Element* element)
{
	amElementNotebookPage* page = nullptr;

	if ( element->IsKindOf(wxCLASSINFO(Character)) )
		page = m_characterPage;
	else if ( element->IsKindOf(wxCLASSINFO(Location)) )
		page = m_locationPage;
	else if ( element->IsKindOf(wxCLASSINFO(Item)) )
		page = m_itemPage;

	return page;
}

void amElementNotebook::UpdateSearchAutoComplete(wxBookCtrlEvent& WXUNUSED(event))
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