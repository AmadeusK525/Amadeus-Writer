#include "Views/Elements/ElementsNotebook.h"

#include "Views/Elements/ElementCreators.h"
#include "Views/Elements/ElementShowcases.h"

#include "Views/MainFrame.h"
#include "Views/Outline/Outline.h"
#include "Views/Outline/OutlineFiles.h"

#include "MyApp.h"
#include "Utils/amUtility.h"
#include "Utils/SortFunctions.h"

#include "Utils/wxmemdbg.h"


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
	m_elementList->InsertColumn(1, sortByChoices.front(), wxLIST_FORMAT_CENTER, FromDIP(80));
	m_elementList->InsertColumn(2, "First Appearance", wxLIST_FORMAT_CENTER, FromDIP(130));
	m_elementList->InsertColumn(3, "Last Appearance", wxLIST_FORMAT_CENTER, FromDIP(130));
	m_elementList->InsertColumn(4, "Documents", wxLIST_FORMAT_CENTER, FromDIP(90));

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
	m_sortBy->SetBackgroundColour(wxColour(20, 20, 20));
	m_sortBy->SetForegroundColour(wxColour(255, 255, 255));
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

bool amElementNotebookPage::ShouldShow(am::StoryElement* element)
{
	if ( !element )
		return false;

	bool bShouldShow;

	if ( element->IsKindOf(wxCLASSINFO(am::TangibleElement)) )
	{
		bShouldShow = false;

		if ( m_vBooksToShow.empty() )
			return true;

		for ( am::Book* const& pBook : m_vBooksToShow )
		{
			if ( ((am::TangibleElement*)element)->IsInBook(pBook) )
			{
				bShouldShow = true;
				break;
			}
		}
	}
	else
	{
		bShouldShow = true;
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
	m_bookCheckList->Clear();
	m_bookCheckList->InsertItems(am::GetBookTitles(), 0);
	m_show->SetText(_("All"));
}

void amElementNotebookPage::GoToStoryElement(am::StoryElement * element)
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
	m_elementList->Freeze();
	m_imageList->RemoveAll();

	size_t sizeBefore = m_elementList->GetItemCount();

	wxVector<am::StoryElement*> vElements;
	if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amCharacterShowcase)) )
	{
		for ( am::Character*& pCharacter : am::GetCharacters() )
			vElements.push_back(pCharacter);
	}
	else if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amLocationShowcase)) )
	{
		for ( am::Location*& pLocation : am::GetLocations() )
			vElements.push_back(pLocation);
	}
	else if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amItemShowcase)) )
	{
		for ( am::Item*& pItem : am::GetItems() )
			vElements.push_back(pItem);
	}

	int i = 0;
	for ( am::StoryElement* const& pElement : vElements )
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

void amElementNotebookPage::UpdateElementInList(int n, am::StoryElement* element)
{
	m_elementList->SetItem(n, 0, element->name);

	wxString role;
	switch ( element->role )
	{
	case am::Role::cProtagonist:
		role = _("Protagonist");
		break;

	case am::Role::cSupporting:
		role = _("Supporting");
		break;

	case am::Role::cVillian:
		role = _("Villian");
		break;

	case am::Role::cSecondary:
		role = _("Secondary");
		break;

	case am::Role::lHigh:
	case am::Role::iHigh:
		role = _("High");
		break;

	case am::Role::lLow:
	case am::Role::iLow:
		role = _("Low");
		break;
	}

	m_elementList->SetItem(n, 1, role);

	if ( element->IsKindOf(wxCLASSINFO(am::TangibleElement)) )
	{
		am::TangibleElement* pTangible = (am::TangibleElement*)element;

		am::Document* pFirstDocument = pTangible->GetFirstDocument();
		if ( pFirstDocument )
			m_elementList->SetItem(n, 2, wxString(_("Book ")) << pFirstDocument->book->pos << _(", Document ") << pFirstDocument->position);
		else
			m_elementList->SetItem(n, 2, "-");

		am::Document* pLastDocument = pTangible->GetLastDocument();
		if ( pLastDocument )
			m_elementList->SetItem(n, 3, wxString(_("Book ")) << pLastDocument->book->pos << _(", Document ") << pLastDocument->position);
		else
			m_elementList->SetItem(n, 3, "-");

		m_elementList->SetItem(n, 4, std::to_string(pTangible->documents.size()));
	}

	if ( element->image.IsOk() )
		m_elementList->SetItemColumnImage(n, 0, m_imageList->Add(wxBitmap(am::GetScaledImage(24, 24, element->image))));
	else
		m_elementList->SetItemColumnImage(n, 0, -1);
}

void amElementNotebookPage::OnEditElement(wxCommandEvent& event)
{
	int n = m_elementList->GetFirstSelected();
	if ( n != -1 )
	{
		am::StoryElement* pElement = am::GetStoryElementByName(m_elementList->GetItemText(n));
		
		if ( pElement )
			am::StartEditingElement(pElement);
	}
}

void amElementNotebookPage::OnDeleteElement(wxCommandEvent& event)
{
	long sel = m_elementList->GetFirstSelected();

	wxMessageDialog deleteCheck(am::GetMainFrame(), "Are you sure you want to delete '" +
		m_elementList->GetItemText(sel) + "'?"
		"\nAll ties to this Element in other parts of the program (Outline files, Timeline, am::Document bindings...) will be permantely deleted as well.",
		"Delete element", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

	if ( deleteCheck.ShowModal() == wxID_YES )
	{
		m_elementList->DeleteItem(sel);
		am::DeleteElement(am::GetStoryElementByName(m_elementList->GetItemText(sel)));
	}
}

void amElementNotebookPage::OnElementsSortBy(wxCommandEvent& event)
{
	wxVector<am::StoryElement*> vElements;

	if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amCharacterShowcase)) )
	{
		m_sortBy->SetSelection(event.GetInt());
		am::Character::cCompType = (am::CompType)event.GetInt();

		for ( am::Character*& pCharacter : am::GetCharacters() )
			vElements.push_back(pCharacter);
	}
	else if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amLocationShowcase)) )
	{
		m_sortBy->SetSelection(event.GetInt());
		am::Location::lCompType = (am::CompType)event.GetInt();

		for ( am::Location*& pLocation : am::GetLocations() )
			vElements.push_back(pLocation);
	}
	else if ( m_elementShowcase->IsKindOf(wxCLASSINFO(amItemShowcase)) )
	{
		m_sortBy->SetSelection(event.GetInt());
		am::Item::iCompType = (am::CompType)event.GetInt();

		for ( am::Item*& pItem : am::GetItems() )
			vElements.push_back(pItem);
	}

	int currentSelection = m_elementList->GetFirstSelected();

	am::ResortElements();
	UpdateList();

	m_elementList->SetFocus();

	am::StoryElement* pCurElement = m_elementShowcase->GetElement();

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
		m_vBooksToShow.push_back(am::GetBooks()[sel]);
	}

	UpdateList();

	am::StoryElement* pDisplayedElement = m_elementShowcase->GetElement();
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
		m_elementShowcase->SetData(am::GetStoryElementByName(m_elementList->GetItemText(sel)));
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
	wxAuiNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	wxAuiSimpleTabArt* pArt = new wxAuiSimpleTabArt();
	pArt->SetColour(wxColour(50, 50, 50));
	pArt->SetActiveColour(wxColour(30, 30, 30));
	SetArtProvider(pArt);

	GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

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

void amElementNotebook::GoToStoryElement(am::StoryElement* element)
{
	if ( element->IsKindOf(wxCLASSINFO(am::Character)) )
	{
		SetSelection(0);
		m_characterPage->GoToStoryElement(element);
	}
	else if ( element->IsKindOf(wxCLASSINFO(am::Location)) )
	{
		SetSelection(1);
		m_locationPage->GoToStoryElement(element);
	}
	else if ( element->IsKindOf(wxCLASSINFO(am::Item)) )
	{
		SetSelection(2);
		m_itemPage->GoToStoryElement(element);
	}
}

bool amElementNotebook::ShouldShow(am::StoryElement* element) const
{
	if ( element->IsKindOf(wxCLASSINFO(am::Character)) )
		return m_characterPage->ShouldShow(element);
	else if ( element->IsKindOf(wxCLASSINFO(am::Location)) )
		return m_locationPage->ShouldShow(element);
	else if ( element->IsKindOf(wxCLASSINFO(am::Item)) )
		return m_itemPage->ShouldShow(element);

	return false;
}

void amElementNotebook::ClearAll()
{
	m_characterPage->ClearAll();
	m_locationPage->ClearAll();
	m_itemPage->ClearAll();
}

void amElementNotebook::RemoveElementFromList(am::StoryElement* element)
{
	wxListView* list = GetAppropriateList(element);

	if ( list )
	{
		long n = list->FindItem(0, element->name);
		if ( n )
			list->DeleteItem(n);
	}
}

void amElementNotebook::UpdateElementInList(int n, am::StoryElement* element)
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

wxListView* amElementNotebook::GetAppropriateList(am::StoryElement* element)
{
	wxListView* list = nullptr;
	amElementNotebookPage* page = GetAppropriatePage(element);

	if ( page )
		list = page->GetList();

	return list;
}

amElementNotebookPage* amElementNotebook::GetAppropriatePage(am::StoryElement* element)
{
	amElementNotebookPage* page = nullptr;

	if ( element->IsKindOf(wxCLASSINFO(am::Character)) )
		page = m_characterPage;
	else if ( element->IsKindOf(wxCLASSINFO(am::Location)) )
		page = m_locationPage;
	else if ( element->IsKindOf(wxCLASSINFO(am::Item)) )
		page = m_itemPage;

	return page;
}

void amElementNotebook::UpdateSearchAutoComplete(wxBookCtrlEvent& WXUNUSED(event))
{
	int sel = this->GetSelection();

	switch ( sel )
	{
	case 0:
		m_searchBar->AutoComplete(am::GetCharacterNames());
		m_searchBar->SetDescriptiveText("Search character");
		break;

	case 1:
		m_searchBar->AutoComplete(am::GetLocationNames());
		m_searchBar->SetDescriptiveText("Search location");
		break;

	case 2:
		m_searchBar->AutoComplete(am::GetItemNames());
		m_searchBar->SetDescriptiveText("Search item");
		break;
	}

	m_searchBar->Clear();
}