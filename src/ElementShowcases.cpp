#include "ElementShowcases.h"
#include "MyApp.h"
#include "ElementsNotebook.h"
#include "amUtility.h"
#include "MainFrame.h"

#include <wx\popupwin.h>

#include "wxmemdbg.h"


///////////////////////////////////////////////////////////////////
///////////////////// RelatedElementsContainer ////////////////////
///////////////////////////////////////////////////////////////////


amRelatedElementsContainer::amRelatedElementsContainer(wxWindow* parent, amTangibleElementShowcase* showcase) :
	wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE), m_showcase(showcase)
{
	SetBackgroundColour(wxColour(10, 10, 10));

	m_label = new wxStaticText(this, -1, "Related elements");
	m_label->SetForegroundColour(wxColour(255, 255, 255));
	m_label->SetFont(wxFontInfo(12).Bold());

	wxButton* addRelated = new wxButton(this, -1, "", wxDefaultPosition, wxSize(25, 25));
	addRelated->SetBitmap(wxBITMAP_PNG(plus));
	addRelated->SetToolTip("Add related elements");
	addRelated->Bind(wxEVT_BUTTON, &amRelatedElementsContainer::OnAddElement, this);
	addRelated->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(m_owner); });
	wxButton* removeRelated = new wxButton(this, -1, "", wxDefaultPosition, wxSize(25, 25));
	removeRelated->SetBitmap(wxBITMAP_PNG(minus));
	removeRelated->SetToolTip("Remove related elements");
	removeRelated->Bind(wxEVT_BUTTON, &amRelatedElementsContainer::OnRemoveElement, this);
	removeRelated->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(m_owner && !m_owner->relatedElements.empty()); });

	wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
	firstLine->Add(m_label, wxSizerFlags().CenterVertical());
	firstLine->AddStretchSpacer(1);
	firstLine->Add(removeRelated);
	firstLine->AddSpacer(2);
	firstLine->Add(addRelated);

	m_grid = new wxWrapSizer(wxHORIZONTAL);

	m_vertical = new wxBoxSizer(wxVERTICAL);
	m_vertical->Add(firstLine, wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_vertical->Add(m_grid, wxSizerFlags(1));

	SetSizer(m_vertical);
}

void amRelatedElementsContainer::OnAddElement(wxCommandEvent& event)
{
	amRelatedElementsDialog* addDialog = new amRelatedElementsDialog((wxWindow*)am::GetMainFrame(),
		m_owner, this, amRelatedElementsDialog::MODE::ADD);
	addDialog->CenterOnScreen();
	addDialog->Show();
}

void amRelatedElementsContainer::OnRemoveElement(wxCommandEvent& event)
{
	amRelatedElementsDialog* addDialog = new amRelatedElementsDialog((wxWindow*)am::GetMainFrame(),
		m_owner, this, amRelatedElementsDialog::MODE::REMOVE);
	addDialog->CenterOnScreen();
	addDialog->Show();
}

void amRelatedElementsContainer::OnElementButtons(wxCommandEvent& event)
{
	amElementButton* button = (amElementButton*)event.GetEventObject();
	am::GetElementsNotebook()->GoToStoryElement(button->element);
}

void amRelatedElementsContainer::LoadAllElements(am::StoryElement* element)
{
	DoLoad(element, [](am::StoryElement* related) { return true; });
}

void amRelatedElementsContainer::LoadCharacters(am::StoryElement* element)
{
	DoLoad(element, [](am::StoryElement* related) { return dynamic_cast<am::Character*>(related) != nullptr; });
}

void amRelatedElementsContainer::LoadLocations(am::StoryElement* element)
{
	DoLoad(element, [](am::StoryElement* related) { return dynamic_cast<am::Location*>(related) != nullptr; });
}

void amRelatedElementsContainer::LoadLocationsByType(am::StoryElement* element)
{}

void amRelatedElementsContainer::LoadItems(am::StoryElement* element)
{
	DoLoad(element, [](am::StoryElement* related) { return dynamic_cast<am::Item*>(related) != nullptr; });
}

void amRelatedElementsContainer::DoLoad(am::StoryElement* element, bool(*ShouldAdd)(am::StoryElement*))
{
	m_owner = element;

	int buttonCount = m_grid->GetItemCount();
	int relatedCount = element->relatedElements.size();

	int i = 0;
	for ( am::StoryElement*& pRelated : element->relatedElements )
	{
		if ( ShouldAdd(pRelated) )
		{
			amElementButton* button;

			if ( i >= buttonCount )
			{
				button = new amElementButton(this, pRelated, "");
				button->Bind(wxEVT_BUTTON, &amRelatedElementsContainer::OnElementButtons, this);
				m_grid->Add(button, wxSizerFlags(0).Border(wxALL, 2));
			}
			else
			{
				button = (amElementButton*)(m_grid->GetItem(i)->GetWindow());
				button->SetElement(pRelated);
			}

			i++;
		}
	}

	size_t buttonCountAfter = i;
	for ( i = buttonCount; i > buttonCountAfter; i-- )
		m_grid->GetItem(i - 1)->GetWindow()->Destroy();

	Layout();
	SendSizeEventToParent();
	Refresh();
}

void amRelatedElementsContainer::ClearAll()
{
	m_grid->Clear(true);
	m_owner = nullptr;

}


///////////////////////////////////////////////////////////////////
////////////////////// RelatedElementsDialog  /////////////////////
///////////////////////////////////////////////////////////////////


amRelatedElementsDialog::amRelatedElementsDialog(wxWindow* parent, am::StoryElement* element, amRelatedElementsContainer* container, MODE mode) :
	wxFrame(parent, -1, "", wxDefaultPosition, wxSize(500, 500), wxCAPTION | wxSYSTEM_MENU | wxFRAME_FLOAT_ON_PARENT)
{
	m_element = element;
	m_container = container;
	m_mode = mode;

	wxPanel* panel = new wxPanel(this);
	panel->SetBackgroundColour(wxColour(20, 20, 20));

	wxStaticText* label = new wxStaticText(panel, -1, element->name);
	label->SetFont(wxFontInfo(14).Bold());
	label->SetForegroundColour(wxColour(255, 255, 255));

	m_notebook = new wxNotebook(panel, -1);

	m_characters = new wxListView(m_notebook, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE | wxLC_NO_HEADER);
	m_characters->AppendColumn("");
	m_characters->SetBackgroundColour(wxColour(20, 20, 20));
	m_characters->SetForegroundColour(wxColour(255, 255, 255));

	m_locations = new wxListView(m_notebook, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE | wxLC_NO_HEADER);
	m_locations->AppendColumn("");
	m_locations->SetBackgroundColour(wxColour(20, 20, 20));
	m_locations->SetForegroundColour(wxColour(255, 255, 255));

	m_items = new wxListView(m_notebook, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE | wxLC_NO_HEADER);
	m_items->AppendColumn("");
	m_items->SetBackgroundColour(wxColour(20, 20, 20));
	m_items->SetForegroundColour(wxColour(255, 255, 255));

	m_characterImages = new wxImageList(24, 24);
	m_locationImages = new wxImageList(24, 24);
	m_itemImages = new wxImageList(24, 24);

	m_characters->AssignImageList(m_characterImages, wxIMAGE_LIST_SMALL);
	m_locations->AssignImageList(m_locationImages, wxIMAGE_LIST_SMALL);
	m_items->AssignImageList(m_itemImages, wxIMAGE_LIST_SMALL);

	m_characters->Bind(wxEVT_SIZE, &amRelatedElementsDialog::OnListResize, this);
	m_locations->Bind(wxEVT_SIZE, &amRelatedElementsDialog::OnListResize, this);
	m_items->Bind(wxEVT_SIZE, &amRelatedElementsDialog::OnListResize, this);

	m_notebook->AddPage(m_characters, "Characters");
	m_notebook->AddPage(m_locations, "Locations");
	m_notebook->AddPage(m_items, "Items");

	auto lightenBG = [](wxMouseEvent& event)
	{
		((wxWindow*)event.GetEventObject())->SetBackgroundColour(wxColour(60, 60, 60));
	};

	auto darkenBG = [](wxMouseEvent& event)
	{
		((wxWindow*)event.GetEventObject())->SetBackgroundColour(wxColour(45, 45, 45));
	};

	wxButton* mainButton = new wxButton(panel, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	mainButton->SetBackgroundColour(wxColour(45, 45, 45));
	mainButton->SetForegroundColour(wxColour(255, 255, 255));
	mainButton->Bind(wxEVT_ENTER_WINDOW, lightenBG);
	mainButton->Bind(wxEVT_LEAVE_WINDOW, darkenBG);
	mainButton->Bind(wxEVT_BUTTON, &amRelatedElementsDialog::OnMainButton, this);
	if ( mode == ADD )
		mainButton->SetLabel("Add");

	else
		mainButton->SetLabel("Remove");

	wxButton* done = new wxButton(panel, -1, "Done", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	done->SetBackgroundColour(wxColour(45, 45, 45));
	done->SetForegroundColour(wxColour(255, 255, 255));
	done->Bind(wxEVT_LEAVE_WINDOW, darkenBG);
	done->Bind(wxEVT_ENTER_WINDOW, lightenBG);
	done->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
		{
			m_container->LoadAllElements(m_element);
			Close();
		});

	wxBoxSizer* vertical = new wxBoxSizer(wxVERTICAL);
	vertical->Add(label, wxSizerFlags(0).CenterHorizontal().Border(wxALL, 5));
	vertical->Add(m_notebook, wxSizerFlags(1).Expand());
	vertical->Add(mainButton, wxSizerFlags(0).Expand().Border(wxALL, 5));
	vertical->AddSpacer(10);
	vertical->Add(done, wxSizerFlags(0).Right().Border(wxALL, 5));

	panel->SetSizer(vertical);

	LoadPossibleCharacters();
	LoadPossibleLocations();
	LoadPossibleItems();

	wxString title;
	if ( m_mode == ADD )
		title = "Add related elements";
	else
		title = "Remove related elements";

	SetTitle(title);
	SetIcon(wxICON(amadeus));
}

void amRelatedElementsDialog::LoadPossibleCharacters()
{
	if ( m_mode == ADD )
	{
		int i = 0;
		for ( am::Character*& pCharacter : am::GetCharacters() )
		{
			if ( pCharacter == m_element )
				continue;

			bool has = false;
			for ( am::StoryElement*& pPresent : m_element->relatedElements )
			{
				if ( pPresent == pCharacter )
				{
					has = true;
					break;
				}
			}

			if ( !has )
			{
				m_characters->InsertItem(i, pCharacter->name);
				if ( pCharacter->image.IsOk() )
					m_characters->SetItemColumnImage(i, 0, m_characterImages->Add(wxBitmap(am::GetScaledImage(24, 24, pCharacter->image))));
				else
					m_characters->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
	else
	{
		int i = 0;
		for ( am::StoryElement*& pPresent : m_element->relatedElements )
		{
			am::Character* pCharacter = dynamic_cast<am::Character*>(pPresent);
			if ( pCharacter )
			{
				m_characters->InsertItem(i, pCharacter->name);
				if ( pCharacter->image.IsOk() )
					m_characters->SetItemColumnImage(i, 0, m_characterImages->Add(wxBitmap(am::GetScaledImage(24, 24, pCharacter->image))));
				else
					m_characters->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
}

void amRelatedElementsDialog::LoadPossibleLocations()
{
	if ( m_mode == ADD )
	{
		int i = 0;
		for ( am::Location*& pLocation : am::GetLocations() )
		{
			if ( pLocation == m_element )
				continue;

			bool has = false;
			for ( am::StoryElement*& pPresent : m_element->relatedElements )
			{
				if ( pPresent == pLocation )
				{
					has = true;
					break;
				}
			}

			if ( !has )
			{
				m_locations->InsertItem(i, pLocation->name);
				if ( pLocation->image.IsOk() )
					m_locations->SetItemColumnImage(i, 0, m_locationImages->Add(wxBitmap(am::GetScaledImage(24, 24, pLocation->image))));
				else
					m_locations->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
	else
	{
		int i = 0;
		for ( am::StoryElement*& pPresent : m_element->relatedElements )
		{
			am::Location* pLocation = dynamic_cast<am::Location*>(pPresent);
			if ( pLocation )
			{
				m_locations->InsertItem(i, pLocation->name);
				if ( pLocation->image.IsOk() )
					m_locations->SetItemColumnImage(i, 0, m_locationImages->Add(wxBitmap(am::GetScaledImage(24, 24, pLocation->image))));
				else
					m_locations->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
}

void amRelatedElementsDialog::LoadPossibleItems()
{
	if ( m_mode == ADD )
	{
		int i = 0;
		for ( am::Item*& pItem : am::GetItems() )
		{
			if ( pItem == m_element )
				continue;

			bool has = false;
			for ( am::StoryElement*& pPresent : m_element->relatedElements )
			{
				if ( pPresent == pItem )
				{
					has = true;
					break;
				}
			}

			if ( !has )
			{
				m_items->InsertItem(i, pItem->name);
				if ( pItem->image.IsOk() )
					m_items->SetItemColumnImage(i, 0, m_itemImages->Add(wxBitmap(am::GetScaledImage(24, 24, pItem->image))));
				else
					m_items->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
	else
	{
		int i = 0;
		for ( am::StoryElement*& pPresent : m_element->relatedElements )
		{
			am::Item* pItem = dynamic_cast<am::Item*>(pPresent);
			if ( pItem )
			{
				m_items->InsertItem(i, pItem->name);
				if ( pItem->image.IsOk() )
					m_items->SetItemColumnImage(i, 0, m_itemImages->Add(wxBitmap(am::GetScaledImage(24, 24, pItem->image))));
				else
					m_items->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
}

void amRelatedElementsDialog::OnMainButton(wxCommandEvent& event)
{
	wxBusyCursor cursor;
	
	int n;
	wxString name;
	am::StoryElement* toApply = nullptr;

	switch ( m_notebook->GetSelection() )
	{
	case 0:
		n = m_characters->GetFirstSelected();

		if ( n != -1 )
		{
			name = m_characters->GetItemText(n);
			for ( am::Character*& pCharacter : am::GetCharacters() )
			{
				if ( pCharacter->name == name )
				{
					toApply = pCharacter;
					m_characters->DeleteItem(n);
					break;
				}
			}
		}

		break;

	case 1:
		n = m_locations->GetFirstSelected();

		if ( n != -1 )
		{
			name = m_locations->GetItemText(n);
			for ( am::Location*& pLocation : am::GetLocations() )
			{
				if ( pLocation->name == name )
				{
					toApply = pLocation;
					m_locations->DeleteItem(n);
					break;
				}
			}
		}
		break;

	case 2:
		n = m_items->GetFirstSelected();

		if ( n != -1 )
		{
			name = m_items->GetItemText(n);
			for ( am::Item*& pItem : am::GetItems() )
			{
				if ( pItem->name == name )
				{
					toApply = pItem;
					m_items->DeleteItem(n);
					break;
				}
			}
		}
		break;
	}

	if ( toApply )
	{
		if ( m_mode == ADD )
			am::RelateStoryElements(m_element, toApply);
		else
			am::UnrelateStoryElements(m_element, toApply);
	}
}

void amRelatedElementsDialog::OnRemoveElement(wxCommandEvent& event)
{
	wxBusyCursor cursor;
	
	int n;
	wxString name;
	am::StoryElement* toAdd = nullptr;

	switch ( m_notebook->GetSelection() )
	{
	case 0:
		n = m_characters->GetFirstSelected();

		if ( n != -1 )
		{
			name = m_characters->GetItemText(n);
			for ( am::Character*& pCharacter : am::GetCharacters() )
			{
				if ( pCharacter->name == name )
				{
					toAdd = pCharacter;
					m_characters->DeleteItem(n);
					break;
				}
			}
		}

		break;

	case 1:
		n = m_locations->GetFirstSelected();

		if ( n != -1 )
		{
			name = m_locations->GetItemText(n);
			for ( am::Location*& pLocation : am::GetLocations() )
			{
				if ( pLocation->name == name )
				{
					toAdd = pLocation;
					m_locations->DeleteItem(n);
					break;
				}
			}
		}
		break;

	case 2:
		n = m_items->GetFirstSelected();

		if ( n != -1 )
		{
			name = m_items->GetItemText(n);
			for ( am::Item*& pItem : am::GetItems() )
			{
				if ( pItem->name == name )
				{
					toAdd = pItem;
					m_items->DeleteItem(n);
					break;
				}
			}
		}
		break;
	}

	if ( toAdd )
		am::UnrelateStoryElements(m_element, toAdd);
}

void amRelatedElementsDialog::OnListResize(wxSizeEvent& event)
{
	wxListView* list = (wxListView*)event.GetEventObject();
	list->SetColumnWidth(0, event.GetSize().x);
}


///////////////////////////////////////////////////////////////////
////////////////////////// ElementShowcase ////////////////////////
///////////////////////////////////////////////////////////////////
wxIMPLEMENT_ABSTRACT_CLASS(amElementShowcase, wxWindow);

bool amElementShowcase::Create(wxWindow* parent)
{
	return wxWindow::Create(parent, -1);
}

wxIMPLEMENT_ABSTRACT_CLASS(amTangibleElementShowcase, amElementShowcase);

amTangibleElementShowcase::amTangibleElementShowcase(wxWindow* parent)
{
	Create(parent);
}

bool amTangibleElementShowcase::Create(wxWindow* parent)
{
	if ( !amElementShowcase::Create(parent) )
		return false;

	wxFont font(wxFontInfo(12).Bold());

	SetDoubleBuffered(true);
	SetBackgroundColour(wxColour(20, 20, 20));

	m_mainPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	m_mainPanel->SetBackgroundColour(wxColour(20, 20, 20));
	m_secondPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	m_secondPanel->SetBackgroundColour(wxColour(20, 20, 20));

	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	m_mainSizer->Add(m_mainPanel, wxSizerFlags(1).Expand());
	m_mainSizer->Add(m_secondPanel, wxSizerFlags(1).Expand());

	m_secondPanel->Show(false);
	SetSizer(m_mainSizer);

	//////////////////////////////// First panel ///////////////////////////////

	m_nextPanel = new wxButton(m_mainPanel, -1, "", wxDefaultPosition, wxSize(25, 25));
	m_nextPanel->SetBitmap(wxBITMAP_PNG(arrowRight));
	m_nextPanel->Bind(wxEVT_BUTTON, &amTangibleElementShowcase::OnNextPanel, this);

	m_role = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(150, -1), wxALIGN_CENTER | wxBORDER_SIMPLE);
	m_role->SetBackgroundColour(wxColour(220, 220, 220));
	m_role->SetFont(font);

	m_name = new wxStaticText(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	m_name->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_name->SetBackgroundColour(wxColour(0, 0, 0));
	m_name->SetForegroundColour(wxColour(255, 255, 255));
	m_name->SetFont(font);

	m_image = new ImagePanel(m_mainPanel, wxDefaultPosition, wxSize(180, 180));
	m_image->SetBackgroundColour(wxColour(150, 150, 150));
	m_image->SetBorderColour(wxColour(20, 20, 20));
	m_image->Hide();

	m_pShortAttrSizer = new wxWrapSizer(wxHORIZONTAL);

	m_mainVerSizer = new  wxBoxSizer(wxVERTICAL);
	m_mainVerSizer->Add(m_nextPanel, wxSizerFlags(0).Right());
	m_mainVerSizer->Add(m_role, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
	m_mainVerSizer->Add(m_image, wxSizerFlags(0).CenterHorizontal());
	m_mainVerSizer->SetItemMinSize(2, wxSize(200, 200));
	m_mainVerSizer->Add(m_name, wxSizerFlags(0).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 15));
	m_mainVerSizer->AddSpacer(10);
	m_mainVerSizer->Add(m_pShortAttrSizer, wxSizerFlags(0).Expand());

	m_mainPanel->SetSizer(m_mainVerSizer);
	m_mainVerSizer->FitInside(m_mainPanel);
	m_mainPanel->SetScrollRate(20, 20);

	/////////////////////////////// Second Panel ///////////////////////////////

	m_prevPanel = new wxButton(m_secondPanel, -1, "", wxDefaultPosition, wxSize(25, 25));
	m_prevPanel->SetBitmap(wxBITMAP_PNG(arrowLeft));
	m_prevPanel->Bind(wxEVT_BUTTON, &amTangibleElementShowcase::OnPreviousPanel, this);

	m_nameSecondPanel = new wxStaticText(m_secondPanel, -1, "");
	m_nameSecondPanel->SetBackgroundColour(wxColour(10, 10, 10));
	m_nameSecondPanel->SetForegroundColour(wxColour(255, 255, 255));
	m_nameSecondPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_nameSecondPanel->SetFont(wxFontInfo(18).Bold());

	wxStaticText* documentsLabel = new wxStaticText(m_secondPanel, -1, "Present in the following documents:",
		wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	documentsLabel->SetBackgroundColour(wxColour(10, 10, 10));
	documentsLabel->SetForegroundColour(wxColour(255, 255, 255));
	documentsLabel->SetFont(wxFontInfo(11).Bold());

	m_documentViewModel = new StoryTreeModel;

#ifdef __WXMSW__
	m_documentView = m_documentViewHandler.Create(m_secondPanel, -1, m_documentViewModel.get(),
		wxBORDER_NONE | wxDV_NO_HEADER | wxDV_MULTIPLE);
#else
	m_documentView = new wxDataViewCtrl(m_secondPanel, -1, wxDefaultPosition, wxDefaultSize,
		wxDV_NO_HEADER | wxDV_SINGLE | wxBORDER_NONE);
	m_documentView->AssociateModel(&m_documentViewModel);
#endif

	m_documentView->SetBackgroundColour(wxColour(90, 90, 90));
	m_documentView->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &amTangibleElementShowcase::OnDocumentActivated, this);

	wxDataViewColumn* pColumn = new wxDataViewColumn(_("Documents"), new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType(),
		wxDATAVIEW_CELL_EDITABLE), 0, FromDIP(200), wxALIGN_LEFT);
	m_documentView->AppendColumn(pColumn);

	auto enableBut = [&](wxUpdateUIEvent& event) { event.Enable(GetElement()); };

	m_addDocumentBtn = new wxButton(m_secondPanel, -1, "Add", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_addDocumentBtn->SetBackgroundColour(wxColour(15, 15, 15));
	m_addDocumentBtn->SetForegroundColour(wxColour(255, 255, 255));
	m_addDocumentBtn->Bind(wxEVT_BUTTON, &amTangibleElementShowcase::OnAddDocument, this);
	m_addDocumentBtn->Bind(wxEVT_ENTER_WINDOW, am::OnEnterDarkButton);
	m_addDocumentBtn->Bind(wxEVT_LEAVE_WINDOW, am::OnLeaveDarkButton);
	m_addDocumentBtn->Bind(wxEVT_UPDATE_UI, enableBut);
	wxButton* removeDocument = new wxButton(m_secondPanel, -1, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	removeDocument->SetBackgroundColour(wxColour(15, 15, 15));
	removeDocument->SetForegroundColour(wxColour(255, 255, 255));
	removeDocument->Bind(wxEVT_ENTER_WINDOW, am::OnEnterDarkButton);
	removeDocument->Bind(wxEVT_LEAVE_WINDOW, am::OnLeaveDarkButton);
	removeDocument->Bind(wxEVT_BUTTON, &amTangibleElementShowcase::OnRemoveDocument, this);
	removeDocument->Bind(wxEVT_UPDATE_UI, enableBut);
	wxButton* openDocument = new wxButton(m_secondPanel, -1, "Open", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	openDocument->SetBackgroundColour(wxColour(15, 15, 15));
	openDocument->SetForegroundColour(wxColour(255, 255, 255));
	openDocument->Bind(wxEVT_ENTER_WINDOW, am::OnEnterDarkButton);
	openDocument->Bind(wxEVT_LEAVE_WINDOW, am::OnLeaveDarkButton);
	openDocument->Bind(wxEVT_BUTTON, &amTangibleElementShowcase::OnOpenDocument, this);
	openDocument->Bind(wxEVT_UPDATE_UI, enableBut);

	wxBoxSizer* documentButtonsSizer = new wxBoxSizer(wxVERTICAL);
	documentButtonsSizer->Add(m_addDocumentBtn, wxSizerFlags(0).Expand());
	documentButtonsSizer->AddSpacer(2);
	documentButtonsSizer->Add(removeDocument, wxSizerFlags(0).Expand());
	documentButtonsSizer->AddStretchSpacer(1);
	documentButtonsSizer->Add(openDocument, wxSizerFlags(0).Expand());

	wxBoxSizer* documentsSizer = new wxBoxSizer(wxHORIZONTAL);
	documentsSizer->Add(m_documentView, wxSizerFlags(1).Expand());
	documentsSizer->Add(documentButtonsSizer, wxSizerFlags(1).Expand());

	m_relatedElements = new amRelatedElementsContainer(m_secondPanel, this);

	m_secondVerSizer = new wxBoxSizer(wxVERTICAL);
	m_secondVerSizer->Add(m_prevPanel, wxSizerFlags(0).Left());
	m_secondVerSizer->Add(m_nameSecondPanel, wxSizerFlags(0).Expand().Border(wxALL, 5));
	m_secondVerSizer->AddSpacer(20);
	m_secondVerSizer->Add(documentsLabel, wxSizerFlags(0).Expand());
	m_secondVerSizer->Add(documentsSizer, wxSizerFlags(1).Expand());
	m_secondVerSizer->AddSpacer(20);
	m_secondVerSizer->Add(m_relatedElements, wxSizerFlags(0).Expand());

	m_secondPanel->SetSizerAndFit(m_secondVerSizer);
	m_secondPanel->SetScrollRate(0, 20);

	return true;
}

void amTangibleElementShowcase::SetData(am::StoryElement* element)
{
	if ( !element || !element->IsKindOf(wxCLASSINFO(am::TangibleElement)) )
	{
		ClearAll();
		return;
	}

	Freeze();

	am::TangibleElement* pTangible = (am::TangibleElement*)element;

	if ( !LoadFirstPanel(pTangible) || !LoadSecondPanel(pTangible) )
		ClearAll();
	else
		m_element = pTangible;

	Thaw();
}

bool amTangibleElementShowcase::LoadFirstPanel(am::TangibleElement* element)
{
	m_mainPanel->Freeze();

	m_name->SetLabel(element->name);
	m_image->Show(m_image->SetImage(element->image));

	wxString role;
	wxColour rolebg;
	wxColour rolefg;

	switch ( element->role )
	{
	case am::cProtagonist:
		role = _("Protagonist");
		rolebg = { 230, 60, 60 };
		rolefg = { 10, 10, 10 };
		break;

	case am::cSupporting:
		role = _("Supporting");
		rolebg = { 130, 230, 180 };
		rolefg = { 10, 10, 10 };
		break;

	case am::cVillian:
		role = _("Villian");
		rolebg = { 100, 20, 20 };
		rolefg = { 255, 255, 255 };
		break;

	case am::cSecondary:
	case am::lLow:
	case am::iLow:
		role = _("Secondary");
		rolebg = { 220, 220, 220 };
		rolefg = { 10, 10, 10 };
		break;

	case am::lHigh:
	case am::iHigh:
		role = "Main";
		rolebg = { 230, 60, 60 };
		rolefg = { 10,10,10 };
		break;

	default:
		rolebg = { 220, 220, 220 };
		rolefg = { 10, 10, 10 };
	}

	m_role->SetLabel(role);
	m_role->SetBackgroundColour(rolebg);
	m_role->SetForegroundColour(rolefg);

	LoadShortAttr(element);
	LoadLongAttr(element);

	m_mainPanel->Thaw();
	return false;
}

bool amTangibleElementShowcase::LoadSecondPanel(am::TangibleElement* element)
{
	m_secondPanel->Freeze();

	m_nameSecondPanel->SetLabel(element->name);

	m_documentViewModel->ClearAll();
	for ( int i = 0; i < element->documents.size(); i++ )
	{
		am::Document*& pDocument = element->documents[i];
		if ( pDocument->isInTrash )
			continue;

		wxDataViewItem bookItem = m_documentViewModel->GetBookItem(pDocument->book);
		if ( !bookItem.IsOk() )
		{
			bookItem = wxDataViewItem(new StoryTreeModelNode(pDocument->book));
			m_documentViewModel->GetBooks().push_back((StoryTreeModelNode*)bookItem.GetID());
			m_documentViewModel->ItemAdded(wxDataViewItem(nullptr), bookItem);
		}

		m_documentViewModel->ItemAdded(
			bookItem,
			wxDataViewItem(new StoryTreeModelNode((StoryTreeModelNode*)bookItem.GetID(), nullptr, pDocument, -1))
		);
	}

	for ( StoryTreeModelNode* pBookNode : m_documentViewModel->GetBooks() )
		m_documentView->Expand(wxDataViewItem(pBookNode));

	m_relatedElements->LoadAllElements(element);

	m_secondVerSizer->FitInside(m_secondPanel);
	m_secondPanel->Thaw();

	return true;
}

void amTangibleElementShowcase::LoadShortAttr(am::StoryElement* element)
{
	size_t nWrapItemSize = m_pShortAttrSizer->GetItemCount();

	int i = 0;
	for ( auto& it : element->mShortAttributes )
	{
		wxStaticText* pShortAttrName;
		wxStaticText* pShortAttrContent;
		wxPanel* pHolderPanel;

		if ( i >= nWrapItemSize )
		{
			pHolderPanel = new wxPanel(m_mainPanel);

			pShortAttrName = new wxStaticText(pHolderPanel, -1, it.first + ": ");
			pShortAttrContent = new wxStaticText(pHolderPanel, -1, it.second, wxDefaultPosition, wxDefaultSize,
				wxBORDER_NONE);

			pShortAttrName->SetFont(wxFontInfo(12).Bold());
			pShortAttrName->SetBackgroundColour(wxColour(15, 15, 15));
			pShortAttrName->SetForegroundColour(wxColour(230, 230, 230));

			pShortAttrContent->SetBackgroundStyle(wxBG_STYLE_PAINT);
			pShortAttrContent->SetBackgroundColour(wxColour(10, 10, 10));
			pShortAttrContent->SetForegroundColour(wxColour(255, 255, 255));
			pShortAttrContent->SetFont(wxFontInfo(11));

			wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
			pSizer->Add(pShortAttrName);
			pSizer->Add(pShortAttrContent, 1);

			pHolderPanel->SetSizerAndFit(pSizer);

			m_pShortAttrSizer->Add(pHolderPanel, 0, wxALL, 10);
		}
		else
		{
			pHolderPanel = (wxPanel*)m_pShortAttrSizer->GetItem(i)->GetWindow();

			wxWindowList& lChildren = pHolderPanel->GetChildren();
			pShortAttrName = (wxStaticText*)lChildren.front();
			pShortAttrContent = (wxStaticText*)lChildren.back();

			pShortAttrName->SetLabel(it.first + ": ");
			pShortAttrContent->SetLabel(it.second);
		}

		wxSize szBestSize(pHolderPanel->GetSizer()->GetMinSize() + wxSize(15, 0));
		pHolderPanel->SetMinSize(szBestSize);

		i++;
	}

	nWrapItemSize = m_pShortAttrSizer->GetItemCount();
	int nShortAttrSize = element->mShortAttributes.size();
	for ( int j = (nWrapItemSize - 1); j > (nShortAttrSize - 1); j-- )
	{
		m_pShortAttrSizer->GetItem(j)->GetWindow()->Destroy();
	}
}

void amTangibleElementShowcase::LoadLongAttr(am::StoryElement * element)
{
	wxVector<wxTextCtrl*> vTextCtrls;

	size_t nLongItemSize = m_custom.size();
	wxSize size(-1, 80);

	int i = 0;
	for ( auto& it : element->mLongAttributes )
	{
		wxStaticText* pLongAttrName;
		wxTextCtrl* pLongAttrContent;

		if ( i >= nLongItemSize )
		{
			pLongAttrName = new wxStaticText(m_mainPanel, -1, it.first, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
			pLongAttrContent = new wxTextCtrl(m_mainPanel, -1, it.second, wxDefaultPosition,
				size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);

			pLongAttrName->SetFont(wxFontInfo(12).Bold());
			pLongAttrName->SetBackgroundColour(wxColour(15, 15, 15));
			pLongAttrName->SetForegroundColour(wxColour(230, 230, 230));

			pLongAttrContent->SetBackgroundStyle(wxBG_STYLE_PAINT);
			pLongAttrContent->SetBackgroundColour(wxColour(10, 10, 10));
			pLongAttrContent->SetForegroundColour(wxColour(255, 255, 255));
			pLongAttrContent->SetFont(wxFontInfo(9));
			pLongAttrContent->SetCursor(wxCURSOR_DEFAULT);
			pLongAttrContent->Bind(wxEVT_LEFT_DOWN, &amCharacterShowcase::EmptyMouseEvent, this);
			pLongAttrContent->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

			m_mainVerSizer->Add(pLongAttrName, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
			m_mainVerSizer->Add(pLongAttrContent, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());

			m_custom.push_back(pair<wxStaticText*, wxTextCtrl*>(pLongAttrName, pLongAttrContent));
		}
		else
		{
			pLongAttrName = m_custom[i].first;
			pLongAttrContent = m_custom[i].second;

			pLongAttrName->SetLabel(it.first);
			pLongAttrContent->SetValue(it.second);
		}

		vTextCtrls.push_back(pLongAttrContent);
		i++;
	}

	nLongItemSize = m_custom.size();
	int nLongAttrSize = element->mLongAttributes.size();
	for ( int j = (nLongItemSize - 1); j > (nLongAttrSize - 1); j-- )
	{
		pair<wxStaticText*, wxTextCtrl*>& it = m_custom[j];

		it.first->Destroy();
		it.second->Destroy();
		m_custom.erase(&it);
	}

	m_mainVerSizer->FitInside(m_mainPanel);

	int nol;
	for ( wxTextCtrl*& pTextCtrl : vTextCtrls )
	{
		nol = pTextCtrl->GetNumberOfLines();
		if ( nol > 5 )
			pTextCtrl->SetMinSize(wxSize(-1, (nol)*pTextCtrl->GetCharHeight()));
		else
			pTextCtrl->SetMinSize(size);
	}

	m_mainVerSizer->FitInside(m_mainPanel);
}

void amTangibleElementShowcase::ShowPage(int index)
{
	Freeze();

	if ( index == 0 )
	{
		m_secondPanel->Show(false);
		m_mainPanel->Show(true);
	}
	else
	{
		m_mainPanel->Show(false);
		m_secondPanel->Show(true);
	}

	SendSizeEvent();
	Thaw();
}

void amTangibleElementShowcase::OnNextPanel(wxCommandEvent& event)
{
	ShowPage(1);
}

void amTangibleElementShowcase::OnDocumentActivated(wxDataViewEvent& event)
{
	StoryTreeModelNode* pNode = (StoryTreeModelNode*)event.GetItem().GetID();

	if ( pNode && pNode->IsDocument() )
		am::OpenDocument(pNode->GetDocument());
}

void amTangibleElementShowcase::OnPreviousPanel(wxCommandEvent& event)
{
	ShowPage(0);
}

void amTangibleElementShowcase::OnAddDocument(wxCommandEvent& event)
{
	wxPopupTransientWindow* win = new wxPopupTransientWindow(m_addDocumentBtn, wxBORDER_SIMPLE | wxPU_CONTAINS_CONTROLS);

	wxDataViewCtrl* dvc;
	m_addDocumentViewModel = new StoryTreeModel;
#ifdef __WXMSW__
	dvc = m_addDocumentViewHandler.Create(win, -1, m_addDocumentViewModel.get());
#else
	dvc = new wxDataViewCtrl(win, -1, wxDefaultPosition, wxDefaultSize,
		wxDV_NO_HEADER | wxDV_SINGLE | wxBORDER_NONE);
	dvc->AssociateModel(&m_addDocumentViewModel);
#endif
	dvc->AppendColumn(new wxDataViewColumn("", new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType(),
		wxDATAVIEW_CELL_EDITABLE), 0, FromDIP(200), wxALIGN_LEFT));
	dvc->SetBackgroundColour(wxColour(90, 90, 90));

	for ( am::Book*& pBook : am::GetBooks() )
	{
		StoryTreeModelNode* pBookNode = new StoryTreeModelNode(pBook);
		wxDataViewItem bookItem(pBookNode);
		m_addDocumentViewModel->GetBooks().push_back(pBookNode);
		m_addDocumentViewModel->ItemAdded(wxDataViewItem(nullptr), bookItem);

		for ( am::Document*& pDocument : pBook->documents )
		{
			bool bIsPresent = false;
			for ( am::Document*& pDocumentInElement : ((am::TangibleElement*)m_element)->documents )
			{
				if ( pDocumentInElement == pDocument )
				{
					bIsPresent = true;
					break;
				}
			}

			if ( !bIsPresent && !pDocument->isInTrash && !pDocument->parent )
			{
				m_addDocumentViewModel->AddDocument(pDocument, bookItem);
			}
		}
	}

	dvc->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, [this, dvc](wxDataViewEvent& event)
		{
			wxBusyCursor busy;

			StoryTreeModelNode* pNode = (StoryTreeModelNode*)event.GetItem().GetID();
			if ( !pNode )
				return;

			if ( !pNode->IsDocument() )
				return;

			am::AddElementToDocument((am::TangibleElement*)m_element, pNode->GetDocument());
			m_addDocumentViewModel->DeleteItem(wxDataViewItem(pNode));
			LoadSecondPanel((am::TangibleElement*)m_element);
		});

	wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
	siz->Add(dvc, wxSizerFlags(1).Expand());
	win->SetSizer(siz);

	wxSize addButtonSize(m_addDocumentBtn->GetSize());
	win->SetPosition(m_addDocumentBtn->ClientToScreen(wxPoint(0, addButtonSize.y)));
	win->SetSize(wxSize(addButtonSize.x, 300));

	win->Popup();
	event.Skip();
}

void amTangibleElementShowcase::OnRemoveDocument(wxCommandEvent& event)
{
	wxBusyCursor cursor;
	
	wxDataViewItemArray selections;
	wxVector<am::Document*> toBeRemoved;
	size_t selCount = m_documentView->GetSelections(selections);
	
	for ( int i = 0; i < selCount; i++ )
	{
		StoryTreeModelNode* pNode = (StoryTreeModelNode*)selections[i].GetID();
		if ( pNode->IsDocument() )
		{
			toBeRemoved.push_back(pNode->GetDocument());
		}
	}

	for ( am::Document*& pDocument : toBeRemoved )
	{
		am::RemoveElementFromDocument((am::TangibleElement*)m_element, pDocument);
	}
}

void amTangibleElementShowcase::OnOpenDocument(wxCommandEvent& event)
{
	wxBusyCursor busy;
	
	wxDataViewItemArray selections;
	size_t selCount = m_documentView->GetSelections(selections);

	for ( int i = 0; i < selCount; i++ )
	{
		StoryTreeModelNode* pNode = (StoryTreeModelNode*)selections[i].GetID();
		if ( pNode->IsDocument() )
		{
			am::OpenDocument(pNode->GetDocument());
		}
	}
}


///////////////////////////////////////////////////////////////////
///////////////////////// CharacterShowcase ///////////////////////
///////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(amCharacterShowcase, amTangibleElementShowcase);

amCharacterShowcase::amCharacterShowcase(wxWindow* parent)
{
	Create(parent);
}

bool amCharacterShowcase::Create(wxWindow* parent)
{
	if ( !amTangibleElementShowcase::Create(parent) )
		return false;

	wxFont font(wxFontInfo(12).Bold());
	wxFont font2(wxFontInfo(11));

	m_isAlive = new wxSwitchCtrl(m_mainPanel, -1, true, "Is alive:");
	m_isAlive->SetBackgroundColour(wxColour(20, 20, 20));
	m_isAlive->SetForegroundColour(wxColour(240, 240, 240));
	m_isAlive->SetFont(wxFontInfo(11).Bold());
	m_isAlive->Show(false);
	m_isAlive->Bind(wxEVT_SWITCHING, &amCharacterShowcase::OnIsAlive, this);
	
	m_mainVerSizer->Insert(5, m_isAlive, wxSizerFlags(0).Right().Border(wxRIGHT, 15));
	m_mainVerSizer->InsertSpacer(6, 10);

	return true;
}

void amCharacterShowcase::OnIsAlive(wxCommandEvent& event)
{
	if ( !m_element )
		return;

	wxBusyCursor busy;
	((am::Character*)m_element)->isAlive = event.GetInt();
	m_element->Update(am::GetProjectDatabase());

	event.Skip();
}

bool amCharacterShowcase::LoadFirstPanel(am::TangibleElement* element)
{
	am::Character* pCharacter = dynamic_cast<am::Character*>(element);
	if ( !pCharacter )
		return false;

	m_isAlive->Show();
	m_isAlive->SetValue(pCharacter->isAlive);

	amTangibleElementShowcase::LoadFirstPanel(pCharacter);
	return true;
}

void amCharacterShowcase::ClearAll()
{
	Freeze();
	m_name->SetLabel(" ");
	m_name->Show(true);

	m_role->SetLabel("");
	m_role->SetBackgroundColour(wxColour(220, 220, 220));

	m_image->Show(false);

	m_isAlive->Show(false);

	for ( pair<wxStaticText*, wxTextCtrl*>& custom : m_custom )
	{
		custom.first->Show(false);
		custom.second->Show(false);
	}

	m_nameSecondPanel->SetLabel("");
	m_documentViewModel->ClearAll();
	m_relatedElements->ClearAll();

	Layout();
	Thaw();
}


///////////////////////////////////////////////////////////////////
///////////////////////// LocationShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(amLocationShowcase, amTangibleElementShowcase);

amLocationShowcase::amLocationShowcase(wxWindow* parent)
{
	Create(parent);
}

bool amLocationShowcase::Create(wxWindow* parent)
{
	if ( !amTangibleElementShowcase::Create(parent) )
		return false;

	wxFont font(wxFontInfo(12).Bold());
	wxFont font2(wxFontInfo(9));

	m_firstColumn = new wxBoxSizer(wxVERTICAL);
	m_secondColumn = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* horSizer = new wxBoxSizer(wxHORIZONTAL);
	horSizer->Add(m_firstColumn, wxSizerFlags(1).Expand().Border(wxRIGHT, 10));
	horSizer->Add(m_secondColumn, wxSizerFlags(1).Expand().Border(wxLEFT, 10));

	m_mainVerSizer->Add(horSizer, wxSizerFlags(1).Expand().Border(wxALL, 15));

	return true;
}

bool amLocationShowcase::LoadFirstPanel(am::TangibleElement* element)
{
	am::Location* pLocation = dynamic_cast<am::Location*>(element);
	if ( !pLocation )
		return false;

	Freeze();
	amTangibleElementShowcase::LoadFirstPanel(element);
	Thaw();
	return true;
}

void amLocationShowcase::ClearAll()
{
	Freeze();
	m_name->SetLabel(" ");
	m_name->Show(true);

	m_role->SetLabel("");
	m_role->SetBackgroundColour(wxColour(220, 220, 220));

	m_image->Show(false);

	for ( pair<wxStaticText*, wxTextCtrl*>& custom : m_custom )
	{
		custom.first->Show(false);
		custom.second->Show(false);
	}

	m_nameSecondPanel->SetLabel("");
	m_documentViewModel->ClearAll();
	m_relatedElements->ClearAll();

	Layout();
	Thaw();
}

void amLocationShowcase::LoadLongAttr(am::StoryElement* element)
{
	wxVector<wxTextCtrl*> vTextCtrls;

	size_t nLongItemSize = m_custom.size();
	wxSize size(-1, 80); 
	
	int i = 0;
	for ( auto& it : element->mLongAttributes )
	{
		wxStaticText* pLongAttrName;
		wxTextCtrl* pLongAttrContent;

		if ( i >= nLongItemSize )
		{
			pLongAttrName = new wxStaticText(m_mainPanel, -1, it.first, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
			pLongAttrContent = new wxTextCtrl(m_mainPanel, -1, it.second, wxDefaultPosition,
				size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);

			pLongAttrName->SetFont(wxFontInfo(12).Bold());
			pLongAttrName->SetBackgroundColour(wxColour(15, 15, 15));
			pLongAttrName->SetForegroundColour(wxColour(230, 230, 230));

			pLongAttrContent->SetBackgroundStyle(wxBG_STYLE_PAINT);
			pLongAttrContent->SetBackgroundColour(wxColour(10, 10, 10));
			pLongAttrContent->SetForegroundColour(wxColour(255, 255, 255));
			pLongAttrContent->SetFont(wxFontInfo(9));
			pLongAttrContent->SetCursor(wxCURSOR_DEFAULT);
			pLongAttrContent->Bind(wxEVT_LEFT_DOWN, &amCharacterShowcase::EmptyMouseEvent, this);
			pLongAttrContent->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

			if ( m_bAddInFirstCol )
			{
				m_firstColumn->Add(pLongAttrName, wxSizerFlags(0).Left());
				m_firstColumn->Add(pLongAttrContent, wxSizerFlags(0).Border(wxBOTTOM, 20).Expand());
			}
			else
			{
				m_secondColumn->Add(pLongAttrName, wxSizerFlags(0).Left());
				m_secondColumn->Add(pLongAttrContent, wxSizerFlags(0).Border(wxBOTTOM, 20).Expand());
			}

			m_bAddInFirstCol = !m_bAddInFirstCol;
			m_custom.push_back(pair<wxStaticText*, wxTextCtrl*>(pLongAttrName, pLongAttrContent));
		}
		else
		{
			pLongAttrName = m_custom[i].first;
			pLongAttrContent = m_custom[i].second;

			pLongAttrName->SetLabel(it.first);
			pLongAttrContent->SetValue(it.second);
		}

		vTextCtrls.push_back(pLongAttrContent);
		i++;
	}

	nLongItemSize = m_custom.size();
	int nLongAttrSize = element->mLongAttributes.size();
	for ( int i = (nLongItemSize - 1); i > (nLongAttrSize - 1); i-- )
	{
		pair<wxStaticText*, wxTextCtrl*>& it = m_custom[i];

		it.first->Destroy();
		it.second->Destroy();
		m_custom.erase(&it);

		m_bAddInFirstCol = !m_bAddInFirstCol;
	}

	m_mainVerSizer->FitInside(m_mainPanel);

	int nol;
	for ( wxTextCtrl*& pTextCtrl : vTextCtrls )
	{
		nol = pTextCtrl->GetNumberOfLines();
		if ( nol > 5 )
			pTextCtrl->SetMinSize(wxSize(-1, (nol + 4) * pTextCtrl->GetCharHeight()));
	}

	m_mainVerSizer->FitInside(m_mainPanel);
}


///////////////////////////////////////////////////////////////////
////////////////////////// ItemShowcase ///////////////////////////
///////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(amItemShowcase, amTangibleElementShowcase);

amItemShowcase::amItemShowcase(wxWindow* parent)
{
	Create(parent);
}

bool amItemShowcase::Create(wxWindow* parent)
{
	if ( !amTangibleElementShowcase::Create(parent) )
		return false;

	wxFont labelFont(wxFontInfo(12).Bold());
	wxFont contentFont(wxFontInfo(11));

	m_stManMade = new wxStaticText(m_mainPanel, -1, _("Man-made"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_stManMade->SetBackgroundColour(wxColour(130, 234, 114));
	m_stManMade->SetFont(labelFont);
	m_stManMade->Show(false);

	m_stMagic = new wxStaticText(m_mainPanel, -1, _("Magic"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_stMagic->SetBackgroundColour(wxColour(226, 96, 255));
	m_stMagic->SetFont(labelFont);
	m_stMagic->Show(false);

	wxBoxSizer* natureLine = new wxBoxSizer(wxHORIZONTAL);
	natureLine->Add(m_stManMade, wxSizerFlags(0).CenterVertical());
	natureLine->AddStretchSpacer(1);
	natureLine->Add(m_stMagic, wxSizerFlags(0).CenterVertical());

	m_mainVerSizer->Add(natureLine, wxSizerFlags().Expand().Border(wxALL, 10));

	return true;
}

bool amItemShowcase::LoadFirstPanel(am::TangibleElement* element)
{
	am::Item* pItem = dynamic_cast<am::Item*>(element);
	if ( !pItem )
		return false;

	m_stManMade->Show(pItem->isManMade);
	m_stMagic->Show(pItem->isMagic);

	Freeze();
	amTangibleElementShowcase::LoadFirstPanel(element);
	Thaw(); 

	return true;
}

void amItemShowcase::ClearAll()
{
	Freeze();

	m_role->SetLabel("");
	m_role->SetBackgroundColour(wxColour(220, 220, 220));

	m_name->SetLabel("");
	m_name->Show();

	m_image->Show(false);

	m_stManMade->Show(false);
	m_stMagic->Show(false);

	m_nameSecondPanel->SetLabel("");
	m_documentViewModel->ClearAll();
	m_relatedElements->ClearAll();

	Layout();
	Thaw();
}