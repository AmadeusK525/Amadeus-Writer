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


amRelatedElementsContainer::amRelatedElementsContainer(wxWindow* parent, amElementShowcase* showcase) :
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
	amRelatedElementsDialog* addDialog = new amRelatedElementsDialog((wxWindow*)amGetManager()->GetMainFrame(),
		m_owner, this, amRelatedElementsDialog::MODE::ADD);
	addDialog->CenterOnScreen();
	addDialog->Show();
}

void amRelatedElementsContainer::OnRemoveElement(wxCommandEvent& event)
{
	amRelatedElementsDialog* addDialog = new amRelatedElementsDialog((wxWindow*)amGetManager()->GetMainFrame(),
		m_owner, this, amRelatedElementsDialog::MODE::REMOVE);
	addDialog->CenterOnScreen();
	addDialog->Show();
}

void amRelatedElementsContainer::OnElementButtons(wxCommandEvent& event)
{
	amElementButton* button = (amElementButton*)event.GetEventObject();
	amGetManager()->GetElementsNotebook()->GoToElement(button->element);
}

void amRelatedElementsContainer::LoadAllElements(Element* element)
{
	DoLoad(element, [](Element* related) { return true; });
}

void amRelatedElementsContainer::LoadCharacters(Element* element)
{
	DoLoad(element, [](Element* related) { return dynamic_cast<Character*>(related) != nullptr; });
}

void amRelatedElementsContainer::LoadLocations(Element* element)
{
	DoLoad(element, [](Element* related) { return dynamic_cast<Location*>(related) != nullptr; });
}

void amRelatedElementsContainer::LoadLocationsByType(Element* element)
{}

void amRelatedElementsContainer::LoadItems(Element* element)
{
	DoLoad(element, [](Element* related) { return dynamic_cast<Item*>(related) != nullptr; });
}

void amRelatedElementsContainer::DoLoad(Element* element, bool(*ShouldAdd)(Element*))
{
	m_owner = element;

	int buttonCount = m_grid->GetItemCount();
	int relatedCount = element->relatedElements.size();

	if ( buttonCount > relatedCount )
	{
		for ( int i = buttonCount - 1; i > relatedCount - 1; i-- )
		{
			m_grid->GetItem(i)->GetWindow()->Destroy();
		}
	}

	int i = 0;
	for ( Element*& pRelated : element->relatedElements )
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


amRelatedElementsDialog::amRelatedElementsDialog(wxWindow* parent, Element* element, amRelatedElementsContainer* container, MODE mode) :
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
		amProjectManager* manager = amGetManager();

		int i = 0;
		for ( Character*& pCharacter : manager->GetCharacters() )
		{
			if ( pCharacter == m_element )
				continue;

			bool has = false;
			for ( Element*& pPresent : m_element->relatedElements )
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
					m_characters->SetItemColumnImage(i, 0, m_characterImages->Add(wxBitmap(amGetScaledImage(24, 24, pCharacter->image))));
				else
					m_characters->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
	else
	{
		int i = 0;
		for ( Element*& pPresent : m_element->relatedElements )
		{
			Character* pCharacter = dynamic_cast<Character*>(pPresent);
			if ( pCharacter )
			{
				m_characters->InsertItem(i, pCharacter->name);
				if ( pCharacter->image.IsOk() )
					m_characters->SetItemColumnImage(i, 0, m_characterImages->Add(wxBitmap(amGetScaledImage(24, 24, pCharacter->image))));
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
		amProjectManager* manager = amGetManager();

		int i = 0;
		for ( Location*& pLocation : manager->GetLocations() )
		{
			if ( pLocation == m_element )
				continue;

			bool has = false;
			for ( Element*& pPresent : m_element->relatedElements )
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
					m_locations->SetItemColumnImage(i, 0, m_locationImages->Add(wxBitmap(amGetScaledImage(24, 24, pLocation->image))));
				else
					m_locations->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
	else
	{
		int i = 0;
		for ( Element*& pPresent : m_element->relatedElements )
		{
			Location* pLocation = dynamic_cast<Location*>(pPresent);
			if ( pLocation )
			{
				m_locations->InsertItem(i, pLocation->name);
				if ( pLocation->image.IsOk() )
					m_locations->SetItemColumnImage(i, 0, m_locationImages->Add(wxBitmap(amGetScaledImage(24, 24, pLocation->image))));
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
		amProjectManager* manager = amGetManager();

		int i = 0;
		for ( Item*& pItem : manager->GetItems() )
		{
			if ( pItem == m_element )
				continue;

			bool has = false;
			for ( Element*& pPresent : m_element->relatedElements )
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
					m_items->SetItemColumnImage(i, 0, m_itemImages->Add(wxBitmap(amGetScaledImage(24, 24, pItem->image))));
				else
					m_items->SetItemColumnImage(i, 0, -1);

				i++;
			}
		}
	}
	else
	{
		int i = 0;
		for ( Element*& pPresent : m_element->relatedElements )
		{
			Item* pItem = dynamic_cast<Item*>(pPresent);
			if ( pItem )
			{
				m_items->InsertItem(i, pItem->name);
				if ( pItem->image.IsOk() )
					m_items->SetItemColumnImage(i, 0, m_itemImages->Add(wxBitmap(amGetScaledImage(24, 24, pItem->image))));
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
	amProjectManager* manager = amGetManager();

	int n;
	wxString name;
	Element* toApply = nullptr;

	switch ( m_notebook->GetSelection() )
	{
	case 0:
		n = m_characters->GetFirstSelected();

		if ( n != -1 )
		{
			name = m_characters->GetItemText(n);
			for ( Character*& pCharacter : manager->GetCharacters() )
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
			for ( Location*& pLocation : manager->GetLocations() )
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
			for ( Item*& pItem : manager->GetItems() )
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
			manager->RelateElements(m_element, toApply);
		else
			manager->UnrelateElements(m_element, toApply);
	}
}

void amRelatedElementsDialog::OnRemoveElement(wxCommandEvent& event)
{
	wxBusyCursor cursor;
	amProjectManager* manager = amGetManager();

	int n;
	wxString name;
	Element* toAdd = nullptr;

	switch ( m_notebook->GetSelection() )
	{
	case 0:
		n = m_characters->GetFirstSelected();

		if ( n != -1 )
		{
			name = m_characters->GetItemText(n);
			for ( Character*& pCharacter : manager->GetCharacters() )
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
			for ( Location*& pLocation : manager->GetLocations() )
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
			for ( Item*& pItem : manager->GetItems() )
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
		manager->UnrelateElements(m_element, toAdd);
}

void amRelatedElementsDialog::OnListResize(wxSizeEvent& event)
{
	wxListView* list = (wxListView*)event.GetEventObject();
	list->SetColumnWidth(0, event.GetSize().x);
}


///////////////////////////////////////////////////////////////////
////////////////////////// ElementShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


amElementShowcase::amElementShowcase(wxWindow* parent) :
	wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
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
	m_nextPanel->Bind(wxEVT_BUTTON, &amElementShowcase::OnNextPanel, this);

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

	m_mainVerSizer = new  wxBoxSizer(wxVERTICAL);
	m_mainVerSizer->Add(m_nextPanel, wxSizerFlags(0).Right());
	m_mainVerSizer->Add(m_role, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
	m_mainVerSizer->Add(m_image, wxSizerFlags(0).CenterHorizontal());
	m_mainVerSizer->SetItemMinSize(2, wxSize(200, 200));
	m_mainVerSizer->Add(m_name, wxSizerFlags(0).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 15));

	m_mainPanel->SetSizer(m_mainVerSizer);
	m_mainVerSizer->FitInside(m_mainPanel);
	m_mainPanel->SetScrollRate(20, 20);

	/////////////////////////////// Second Panel ///////////////////////////////

	m_prevPanel = new wxButton(m_secondPanel, -1, "", wxDefaultPosition, wxSize(25, 25));
	m_prevPanel->SetBitmap(wxBITMAP_PNG(arrowLeft));
	m_prevPanel->Bind(wxEVT_BUTTON, &amElementShowcase::OnPreviousPanel, this);

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

	m_documents = new wxListView(m_secondPanel, -1, wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE | wxLC_LIST | wxLC_HRULES | wxLC_NO_HEADER);
	m_documents->SetBackgroundColour(wxColour(30, 30, 30));
	m_documents->SetForegroundColour(wxColour(255, 255, 255));
	m_documents->AppendColumn("Documents");
	m_documents->Bind(wxEVT_SIZE, [&](wxSizeEvent& event)
		{
			m_documents->SetColumnWidth(0, m_documents->GetClientSize().x);
			event.Skip();
		});

	auto enableBut = [&](wxUpdateUIEvent& event) { event.Enable(GetElement()); };

	m_addDocumentBtn = new wxButton(m_secondPanel, -1, "Add", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_addDocumentBtn->SetBackgroundColour(wxColour(15, 15, 15));
	m_addDocumentBtn->SetForegroundColour(wxColour(255, 255, 255));
	m_addDocumentBtn->Bind(wxEVT_BUTTON, &amElementShowcase::OnAddDocument, this);
	m_addDocumentBtn->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	m_addDocumentBtn->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);
	m_addDocumentBtn->Bind(wxEVT_UPDATE_UI, enableBut);
	wxButton* removeDocument = new wxButton(m_secondPanel, -1, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	removeDocument->SetBackgroundColour(wxColour(15, 15, 15));
	removeDocument->SetForegroundColour(wxColour(255, 255, 255));
	removeDocument->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	removeDocument->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);
	removeDocument->Bind(wxEVT_BUTTON, &amElementShowcase::OnRemoveDocument, this);
	removeDocument->Bind(wxEVT_UPDATE_UI, enableBut);
	wxButton* openDocument = new wxButton(m_secondPanel, -1, "Open", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	openDocument->SetBackgroundColour(wxColour(15, 15, 15));
	openDocument->SetForegroundColour(wxColour(255, 255, 255));
	openDocument->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	openDocument->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);
	openDocument->Bind(wxEVT_BUTTON, &amElementShowcase::OnOpenDocument, this);
	openDocument->Bind(wxEVT_UPDATE_UI, enableBut);

	wxBoxSizer* documentButtonsSizer = new wxBoxSizer(wxVERTICAL);
	documentButtonsSizer->Add(m_addDocumentBtn, wxSizerFlags(0).Expand());
	documentButtonsSizer->AddSpacer(2);
	documentButtonsSizer->Add(removeDocument, wxSizerFlags(0).Expand());
	documentButtonsSizer->AddStretchSpacer(1);
	documentButtonsSizer->Add(openDocument, wxSizerFlags(0).Expand());

	wxBoxSizer* documentsSizer = new wxBoxSizer(wxHORIZONTAL);
	documentsSizer->Add(m_documents, wxSizerFlags(1).Expand());
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
}

void amElementShowcase::SetData(Element* element)
{
	Freeze();

	if ( !LoadFirstPanel(element) || !LoadSecondPanel(element) )
		ClearAll();
	else
		m_element = element;

	Thaw();
}

bool amElementShowcase::LoadSecondPanel(Element* element)
{
	m_secondPanel->Freeze();

	m_nameSecondPanel->SetLabel(element->name);

	m_documents->DeleteAllItems();
	for ( int i = 0; i < element->documents.size(); i++ )
		m_documents->InsertItem(i, element->documents[i]->name);

	m_relatedElements->LoadAllElements(element);

	m_secondVerSizer->FitInside(m_secondPanel);
	m_secondPanel->Thaw();

	return true;
}

void amElementShowcase::OnNextPanel(wxCommandEvent& event)
{
	ShowPage(1);
}

void amElementShowcase::OnPreviousPanel(wxCommandEvent& event)
{
	ShowPage(0);
}

void amElementShowcase::ShowPage(int index)
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

void amElementShowcase::OnAddDocument(wxCommandEvent& event)
{
	wxPopupTransientWindow* win = new wxPopupTransientWindow(m_addDocumentBtn, wxBORDER_NONE | wxPU_CONTAINS_CONTROLS);

	wxVector<Document*> documents = amGetManager()->GetDocumentsInCurrentBook();
	wxArrayString documentNames;

	for ( Document*& pDocument : documents )
	{
		if ( m_documents->FindItem(0, pDocument->name) == -1 )
			documentNames.Add(pDocument->name);
	}

	wxListBox* list = new wxListBox(win, -1, wxDefaultPosition, wxDefaultSize,
		documentNames, wxBORDER_SIMPLE | wxLB_NEEDED_SB | wxLB_SINGLE);
	list->Bind(wxEVT_LISTBOX_DCLICK, [this, list](wxCommandEvent& event)
		{
			wxBusyCursor cursor;

			amProjectManager* manager = amGetManager();
			for ( Document*& pDocument : manager->GetDocumentsInCurrentBook() )
			{
				if ( pDocument->name == event.GetString() )
				{
					manager->AddElementToDocument(m_element, pDocument);
					list->Delete(list->FindString(pDocument->name));
					LoadSecondPanel(m_element);
					break;
				}
			}
		});
	list->SetBackgroundColour(wxColour(55, 55, 55));
	list->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
	siz->Add(list, wxSizerFlags(1).Expand());
	win->SetSizer(siz);

	wxSize addButtonSize(m_addDocumentBtn->GetSize());
	win->SetPosition(m_addDocumentBtn->ClientToScreen(wxPoint(0, addButtonSize.y)));
	win->SetSize(wxSize(addButtonSize.x, 150));

	win->Popup();
	event.Skip();
}

void amElementShowcase::OnRemoveDocument(wxCommandEvent & event)
{
	wxBusyCursor cursor;

	amProjectManager* manager = amGetManager();
	wxString documentName;
	long sel = m_documents->GetFirstSelected();

	while ( sel != -1 )
	{
		documentName = m_documents->GetItemText(sel);
		for ( Document*& pDocument : manager->GetDocumentsInCurrentBook() )
		{
			if ( pDocument->name == documentName )
			{
				manager->RemoveElementFromDocument(GetElement(), pDocument);
				m_documents->DeleteItem(sel);
			}
		}

		sel = m_documents->GetNextSelected(sel - 1);
	}

	amElementsNotebook* notebook = manager->GetElementsNotebook();

	long elementIndex = notebook->m_charList->FindItem(0, GetElement()->name);
	if ( elementIndex != -1 )
	{
		notebook->UpdateCharacter(elementIndex, (Character*)GetElement());
		return;
	}

	elementIndex = notebook->m_locList->FindItem(0, GetElement()->name);
	if ( elementIndex != -1 )
	{
		notebook->UpdateLocation(elementIndex, (Location*)GetElement());
		return;
	}

	elementIndex = notebook->m_itemList->FindItem(0, GetElement()->name);
	if ( elementIndex != -1 )
	{
		notebook->UpdateItem(elementIndex, (Item*)GetElement());
		return;
	}
}

void amElementShowcase::OnOpenDocument(wxCommandEvent& event)
{
	amProjectManager* manager = amGetManager();
	long sel = m_documents->GetFirstSelected();

	while ( sel != -1 )
	{
		manager->OpenDocument(m_element->documents[sel]);

		sel = m_documents->GetNextSelected(sel);
	}
}


///////////////////////////////////////////////////////////////////
///////////////////////// CharacterShowcase ///////////////////////
///////////////////////////////////////////////////////////////////


amCharacterShowcase::amCharacterShowcase(wxWindow* parent) : amElementShowcase(parent)
{
	wxFont font(wxFontInfo(12).Bold());
	wxFont font2(wxFontInfo(11));

	m_isAlive = new wxSwitchCtrl(m_mainPanel, -1, true, "Is alive:");
	m_isAlive->SetBackgroundColour(wxColour(20, 20, 20));
	m_isAlive->SetForegroundColour(wxColour(240, 240, 240));
	m_isAlive->SetFont(wxFontInfo(11).Bold());
	m_isAlive->Show(false);

	wxVector<wxStaticText*> vSimpleLabels;
	wxVector<wxStaticText*> vSimpleContentCtrls;

	m_ageLabel = new wxStaticText(m_mainPanel, -1, _("Age: "));
	m_age = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(45, 20), wxBORDER_NONE);
	vSimpleLabels.push_back(m_ageLabel);
	vSimpleContentCtrls.push_back(m_age);
	m_sexLabel = new wxStaticText(m_mainPanel, -1, _("Sex: "));
	m_sex = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_NONE);
	vSimpleLabels.push_back(m_sexLabel);
	vSimpleContentCtrls.push_back(m_sex);
	m_heightLabel = new wxStaticText(m_mainPanel, -1, _("Height: "));
	m_height = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_NONE);
	vSimpleLabels.push_back(m_heightLabel);
	vSimpleContentCtrls.push_back(m_height);
	m_nickLabel = new wxStaticText(m_mainPanel, -1, _("Nickname: "));
	m_nick = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(100, 20), wxBORDER_NONE);
	vSimpleLabels.push_back(m_nickLabel);
	vSimpleContentCtrls.push_back(m_nick);
	m_natLabel = new wxStaticText(m_mainPanel, -1, _("Nationality: "));
	m_nat = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(120, 20), wxBORDER_NONE);
	vSimpleLabels.push_back(m_natLabel);
	vSimpleContentCtrls.push_back(m_nat);

	for ( int i = 0; i < vSimpleLabels.size(); i++ )
	{
		wxStaticText*& label = vSimpleLabels[i];
		wxStaticText*& content = vSimpleContentCtrls[i];

		label->SetFont(font);
		label->SetBackgroundColour(wxColour(15, 15, 15));
		label->SetForegroundColour(wxColour(230, 230, 230));
		label->Show(false);

		content->SetBackgroundStyle(wxBG_STYLE_PAINT);
		content->SetBackgroundColour(wxColour(10, 10, 10));
		content->SetForegroundColour(wxColour(255, 255, 255));
		content->SetFont(font2);
		content->Show(false);
	}

	wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
	firstLine->Add(m_ageLabel, wxSizerFlags(0).Border(wxLEFT, 10));
	firstLine->Add(m_age, wxSizerFlags(1));
	firstLine->Add(m_sexLabel, wxSizerFlags(0).Border(wxLEFT, 20));
	firstLine->Add(m_sex, wxSizerFlags(2));
	firstLine->Add(m_heightLabel, wxSizerFlags(0).Border(wxLEFT, 20));
	firstLine->Add(m_height, wxSizerFlags(2).Border(wxRIGHT, 5));

	wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
	secondLine->Add(m_nickLabel, wxSizerFlags(0));
	secondLine->Add(m_nick, wxSizerFlags(1));
	secondLine->Add(m_natLabel, wxSizerFlags(0).Border(wxLEFT, 20));
	secondLine->Add(m_nat, wxSizerFlags(1));

	wxVector<wxStaticText*> vLabels;
	wxVector<wxTextCtrl*> vContent;

	m_appLabel = new wxStaticText(m_mainPanel, -1, _("Appearance"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_appearance = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_appLabel);
	vContent.push_back(m_appearance);
	m_perLabel = new wxStaticText(m_mainPanel, -1, _("Personality"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_personality = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_perLabel);
	vContent.push_back(m_personality);
	m_bsLabel = new wxStaticText(m_mainPanel, -1, _("Backstory"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_backstory = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_bsLabel);
	vContent.push_back(m_backstory);

	for ( int i = 0; i < vLabels.size(); i++ )
	{
		wxStaticText*& label = vLabels[i];
		wxTextCtrl*& content = vContent[i];

		label->SetFont(font);
		label->SetBackgroundColour(wxColour(15, 15, 15));
		label->SetForegroundColour(wxColour(230, 230, 230));
		label->Show(false);
		
		content->SetBackgroundStyle(wxBG_STYLE_PAINT);
		content->SetBackgroundColour(wxColour(10, 10, 10));
		content->SetForegroundColour(wxColour(255, 255, 255));
		content->SetFont(wxFontInfo(9));
		content->Show(false);
		content->Bind(wxEVT_LEFT_DOWN, &amCharacterShowcase::EmptyMouseEvent, this);
		content->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);
		content->SetCursor(wxCURSOR_DEFAULT);
	}

	m_mainVerSizer->AddSpacer(10);
	m_mainVerSizer->Add(m_isAlive, wxSizerFlags(0).Right().Border(wxRIGHT, 15));
	m_mainVerSizer->AddSpacer(10);
	m_mainVerSizer->Add(firstLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM | wxLEFT | wxRIGHT, 10));
	m_mainVerSizer->Add(secondLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM | wxLEFT | wxRIGHT, 10));
	m_mainVerSizer->Add(m_appLabel, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
	m_mainVerSizer->Add(m_appearance, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
	m_mainVerSizer->Add(m_perLabel, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
	m_mainVerSizer->Add(m_personality, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
	m_mainVerSizer->Add(m_bsLabel, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
	m_mainVerSizer->Add(m_backstory, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
}

bool amCharacterShowcase::LoadFirstPanel(Element* element)
{
	Character* pCharacter = dynamic_cast<Character*>(element);
	if ( !pCharacter )
		return false;

	m_mainPanel->Freeze();

	m_name->SetLabel(pCharacter->name);
	m_isAlive->Show();

	bool isNotEmpty = !pCharacter->sex.IsEmpty();
	m_sex->Show(isNotEmpty);
	m_sexLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_sex->SetLabel(pCharacter->sex);
		wxColour bg;
		wxColour fg;

		if ( pCharacter->sex == "Female" )
		{
			bg = { 255,182,193 };
			fg = { 0, 0, 0 };
		}
		else if ( pCharacter->sex == "Male" )
		{
			bg = { 139,186,255 };
			fg = { 0,0,0 };
		}
		else
		{
			bg = { 10,10,10 };
			fg = { 255,255,255 };
		}

		m_sex->SetBackgroundColour(bg);
		m_sex->SetForegroundColour(fg);
	}

	isNotEmpty = !pCharacter->age.IsEmpty();
	m_age->Show(isNotEmpty);
	m_ageLabel->Show(isNotEmpty);
	if ( isNotEmpty )
		m_age->SetLabel(pCharacter->age);

	isNotEmpty = !pCharacter->height.IsEmpty();
	m_height->Show(isNotEmpty);
	m_heightLabel->Show(isNotEmpty);
	if ( isNotEmpty )
		m_height->SetLabel(pCharacter->height);

	isNotEmpty = !pCharacter->nat.IsEmpty();
	m_nat->Show(isNotEmpty);
	m_natLabel->Show(isNotEmpty);
	if ( isNotEmpty )
		m_nat->SetLabel(pCharacter->nat);

	isNotEmpty = !pCharacter->nick.IsEmpty();
	m_nick->Show(isNotEmpty);
	m_nickLabel->Show(isNotEmpty);
	if ( isNotEmpty )
		m_nick->SetLabel(pCharacter->nick);

	wxString role;
	wxColour rolebg;
	wxColour rolefg;

	switch ( pCharacter->role )
	{
	case cProtagonist:
		role = "Protagonist";
		rolebg = { 230, 60, 60 };
		rolefg = { 10, 10, 10 };
		break;

	case cSupporting:
		role = "Supporting";
		rolebg = { 130, 230, 180 };
		rolefg = { 10, 10, 10 };
		break;

	case cVillian:
		role = "Villian";
		rolebg = { 100, 20, 20 };
		rolefg = { 255, 255, 255 };
		break;

	case cSecondary:
		role = "Secondary";
		rolebg = { 220, 220, 220 };
		rolefg = { 10, 10, 10 };
		break;

	default:
		role = "";
		rolebg = { 220, 220, 220 };
		rolefg = { 10, 10, 10 };
	}

	m_role->SetLabel(role);
	m_role->SetBackgroundColour(rolebg);
	m_role->SetForegroundColour(rolefg);

	wxVector<wxTextCtrl*> vTextCtrls;
	int width = m_appearance->GetClientSize().x;

	isNotEmpty = !pCharacter->appearance.IsEmpty();
	m_appearance->Show(isNotEmpty);
	m_appLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_appearance->SetValue(pCharacter->appearance);
		vTextCtrls.push_back(m_appearance);
	}

	isNotEmpty = !pCharacter->personality.IsEmpty();
	m_personality->Show(isNotEmpty);
	m_perLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_personality->SetValue(pCharacter->personality);
		vTextCtrls.push_back(m_personality);
	}

	isNotEmpty = !pCharacter->backstory.IsEmpty();
	m_backstory->Show(isNotEmpty);
	m_bsLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_backstory->SetValue(pCharacter->backstory);
		vTextCtrls.push_back(m_backstory);
	}

	m_image->Show(m_image->SetImage(pCharacter->image));

	int currentCustomCount = m_custom.size();
	int characterCustomCount = pCharacter->custom.size();

	if ( currentCustomCount > characterCustomCount )
	{
		pair<wxStaticText*, wxTextCtrl*>* it = m_custom.end() - 1;

		for ( int i = (currentCustomCount - 1); i > (characterCustomCount - 1); i-- )
		{
			it->first->Destroy();
			it->second->Destroy();
			m_custom.erase(it--);
		}
	}

	wxSize size(-1, 80);

	for ( int i = 0; i < pCharacter->custom.size(); i++ )
	{
		if ( pCharacter->custom[i].second.IsEmpty() )
			continue;

		if ( i >= currentCustomCount )
		{
			wxStaticText* label = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition,
				wxDefaultSize, wxBORDER_SIMPLE);
			label->SetFont(wxFontInfo(12).Bold());
			label->SetBackgroundColour(wxColour(15, 15, 15));
			label->SetForegroundColour(wxColour(230, 230, 230));

			wxTextCtrl* content = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition,
				size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
			content->SetBackgroundStyle(wxBG_STYLE_PAINT);
			content->SetBackgroundColour(wxColour(10, 10, 10));
			content->SetForegroundColour(wxColour(255, 255, 255));
			content->SetFont(wxFontInfo(9));
			content->SetCursor(wxCURSOR_DEFAULT);
			content->Bind(wxEVT_LEFT_DOWN, &amCharacterShowcase::EmptyMouseEvent, this);
			content->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

			m_mainVerSizer->Add(label, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
			m_mainVerSizer->Add(content, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());

			m_custom.push_back(pair<wxStaticText*, wxTextCtrl*>(label, content));
		}

		vTextCtrls.push_back(m_custom[i].second);
		m_custom[i].first->SetLabel(pCharacter->custom[i].first);
		m_custom[i].second->SetLabel(pCharacter->custom[i].second);
	}

	m_mainVerSizer->FitInside(m_mainPanel);

	int nol;
	for ( wxTextCtrl*& textCtrl : vTextCtrls )
	{
		nol = textCtrl->GetNumberOfLines();

		if ( nol > 5 )
			textCtrl->SetMinSize(wxSize(-1, (nol * textCtrl->GetCharHeight()) + 5));
		else
			textCtrl->SetMinSize(size);
	}

	m_mainVerSizer->FitInside(m_mainPanel);
	m_mainPanel->Thaw();
	return true;
}

void amCharacterShowcase::ClearAll()
{
	Freeze();
	m_name->SetLabel("");

	m_role->SetLabel("");
	m_role->SetBackgroundColour(wxColour(220, 220, 220));

	m_image->SetImage(wxImage());

	m_isAlive->Show(false);

	m_sex->Show(false);
	m_sexLabel->Show(false);
	m_age->Show(false);
	m_ageLabel->Show(false);
	m_height->Show(false);
	m_heightLabel->Show(false);
	m_nat->Show(false);
	m_natLabel->Show(false);
	m_nick->Show(false);
	m_nickLabel->Show(false);
	m_appearance->Show(false);
	m_appLabel->Show(false);
	m_personality->Show(false);
	m_perLabel->Show(false);
	m_backstory->Show(false);
	m_bsLabel->Show(false);

	for ( pair<wxStaticText*, wxTextCtrl*>& custom : m_custom )
	{
		custom.first->Show(false);
		custom.second->Show(false);
	}

	m_relatedElements->ClearAll();
	Thaw();
}


///////////////////////////////////////////////////////////////////
///////////////////////// LocationShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


amLocationShowcase::amLocationShowcase(wxWindow* parent) :amElementShowcase(parent)
{
	wxFont font(wxFontInfo(12).Bold());
	wxFont font2(wxFontInfo(9));

	wxVector<wxStaticText*> vLabels;
	wxVector<wxTextCtrl*> vContentCtrls;

	m_generalLabel = new wxStaticText(m_mainPanel, -1, "General", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_general = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_generalLabel);
	vContentCtrls.push_back(m_general);
	m_natLabel = new wxStaticText(m_mainPanel, -1, _("Natural Aspects"), wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_natural = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_natLabel);
	vContentCtrls.push_back(m_natural);
	m_archLabel = new wxStaticText(m_mainPanel, -1, _("Architecture"), wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_architecture = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_archLabel);
	vContentCtrls.push_back(m_architecture);
	m_ecoLabel = new wxStaticText(m_mainPanel, -1, _("Economy"), wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_economy = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_ecoLabel);
	vContentCtrls.push_back(m_economy);
	m_culLabel = new wxStaticText(m_mainPanel, -1, _("Culture"), wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_culture = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_culLabel);
	vContentCtrls.push_back(m_culture);
	m_poliLabel = new wxStaticText(m_mainPanel, -1, _("Politics"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_SIMPLE);
	m_politics = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_poliLabel);
	vContentCtrls.push_back(m_politics);

	for ( int i = 0; i < vLabels.size(); i++ )
	{
		wxStaticText*& label = vLabels[i];
		wxTextCtrl*& content = vContentCtrls[i];

		label->SetFont(font);
		label->SetBackgroundColour(wxColour(15, 15, 15));
		label->SetForegroundColour(wxColour(230, 230, 230));
		label->Show(false);

		content->SetBackgroundStyle(wxBG_STYLE_PAINT);
		content->SetBackgroundColour(wxColour(10, 10, 10));
		content->SetForegroundColour(wxColour(255, 255, 255));
		content->SetFont(font2);
		content->Show(false);
		content->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
		content->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);
		content->SetCursor(wxCURSOR_DEFAULT);
	}

	m_firstColumn = new wxBoxSizer(wxVERTICAL);
	m_firstColumn->Add(m_generalLabel, wxSizerFlags(0));
	m_firstColumn->Add(m_general, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
	m_firstColumn->Add(m_archLabel, wxSizerFlags(0));
	m_firstColumn->Add(m_architecture, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
	m_firstColumn->Add(m_culLabel, wxSizerFlags(0));
	m_firstColumn->Add(m_culture, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));

	m_secondColumn = new wxBoxSizer(wxVERTICAL);
	m_secondColumn->Add(m_natLabel, wxSizerFlags(0));
	m_secondColumn->Add(m_natural, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
	m_secondColumn->Add(m_ecoLabel, wxSizerFlags(0));
	m_secondColumn->Add(m_economy, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
	m_secondColumn->Add(m_poliLabel, wxSizerFlags(0));
	m_secondColumn->Add(m_politics, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));

	wxBoxSizer* horSizer = new wxBoxSizer(wxHORIZONTAL);
	horSizer->Add(m_firstColumn, wxSizerFlags(1).Expand().Border(wxRIGHT, 10));
	horSizer->Add(m_secondColumn, wxSizerFlags(1).Expand().Border(wxLEFT, 10));

	m_mainVerSizer->Add(horSizer, wxSizerFlags(1).Expand().Border(wxALL, 15));
}

bool amLocationShowcase::LoadFirstPanel(Element* element)
{
	Location* pLocation = dynamic_cast<Location*>(element);
	if ( !pLocation )
		return false;

	m_mainPanel->Freeze();
	m_name->SetLabel(pLocation->name);

	switch ( pLocation->role )
	{
	case lHigh:
		m_role->SetBackgroundColour(wxColour(230, 60, 60));
		m_role->SetLabel("Main");
		break;

	case lLow:
		m_role->SetBackgroundColour(wxColour(220, 220, 220));
		m_role->SetLabel("Secondary");
		break;

	default:
		m_role->SetBackgroundColour(wxColour(220, 220, 220));
		m_role->SetLabel("");
		break;
	}

	wxVector<wxTextCtrl*> vTextCtrls;

	bool isNotEmpty = !pLocation->general.IsEmpty();
	m_general->Show(isNotEmpty);
	m_generalLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_general->SetValue(pLocation->general);
		vTextCtrls.push_back(m_general);
	}

	isNotEmpty = !pLocation->natural.IsEmpty();
	m_natural->Show(isNotEmpty);
	m_natLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_natural->SetValue(pLocation->natural);
		vTextCtrls.push_back(m_natural);
	}

	isNotEmpty = !pLocation->architecture.IsEmpty();
	m_architecture->Show(isNotEmpty);
	m_archLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_architecture->SetValue(pLocation->architecture);
		vTextCtrls.push_back(m_architecture);
	}

	isNotEmpty = !pLocation->politics.IsEmpty();
	m_politics->Show(isNotEmpty);
	m_poliLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_politics->SetValue(pLocation->politics);
		vTextCtrls.push_back(m_politics);
	}

	isNotEmpty = !pLocation->economy.IsEmpty();
	m_economy->Show(isNotEmpty);
	m_ecoLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_economy->SetValue(pLocation->economy);
		vTextCtrls.push_back(m_economy);
	}

	isNotEmpty = !pLocation->culture.IsEmpty();
	m_culture->Show(isNotEmpty);
	m_culLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_culture->SetValue(pLocation->culture);
		vTextCtrls.push_back(m_culture);
	}

	m_image->Show(m_image->SetImage(pLocation->image));

	int currentCustomCount = m_custom.size();
	int locationCustomCount = pLocation->custom.size();

	if ( currentCustomCount > locationCustomCount )
	{
		pair<wxStaticText*, wxTextCtrl*>* it = m_custom.end() - 1;

		for ( int i = (currentCustomCount - 1); i > (locationCustomCount - 1); i-- )
		{
			it->first->Destroy();
			it->second->Destroy();
			m_custom.erase(it--);

			m_first = !m_first;
		}
	}

	wxSize size(-1, 80);

	for ( int i = 0; i < pLocation->custom.size(); i++ )
	{
		if ( pLocation->custom[i].second.IsEmpty() )
			continue;

		if ( i >= currentCustomCount )
		{
			wxStaticText* label = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition,
				wxDefaultSize, wxBORDER_SIMPLE);
			wxTextCtrl* content = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition,
				size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);

			if ( m_first )
			{
				m_firstColumn->Add(label, wxSizerFlags(0).Left());
				m_firstColumn->Add(content, wxSizerFlags(0).Border(wxBOTTOM, 20).Expand());
			}
			else
			{
				m_secondColumn->Add(label, wxSizerFlags(0).Left());
				m_secondColumn->Add(content, wxSizerFlags(0).Border(wxBOTTOM, 20).Expand());
			}

			label->SetFont(wxFontInfo(12).Bold());
			label->SetBackgroundColour(wxColour(15, 15, 15));
			label->SetForegroundColour(wxColour(230, 230, 230));

			content->SetBackgroundStyle(wxBG_STYLE_PAINT);
			content->SetBackgroundColour(wxColour(10, 10, 10));
			content->SetForegroundColour(wxColour(255, 255, 255));
			content->SetFont(wxFontInfo(9));
			content->SetCursor(wxCURSOR_DEFAULT);
			content->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
			content->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

			m_first = !m_first;
			m_custom.push_back(pair<wxStaticText*, wxTextCtrl*>{label, content});
		}

		m_custom[i].first->SetLabel(pLocation->custom[i].first);
		m_custom[i].second->SetLabel(pLocation->custom[i].second);

		m_custom[i].first->Show(pLocation->custom[i].second != "");
		m_custom[i].second->Show(pLocation->custom[i].second != "");

		vTextCtrls.push_back(m_custom[i].second);
	}

	m_mainVerSizer->FitInside(m_mainPanel);

	int nol;
	for ( wxTextCtrl*& textCtrl : vTextCtrls )
	{
		nol = textCtrl->GetNumberOfLines();

		if ( nol > 5 )
			textCtrl->SetMinSize(wxSize(-1, (nol * textCtrl->GetCharHeight()) + 5));
		else
			textCtrl->SetMinSize(size);
	}

	m_mainVerSizer->FitInside(m_mainPanel);
	m_mainPanel->Thaw();

	return true;
}

void amLocationShowcase::ClearAll()
{
	Freeze();
	m_name->SetLabel("");

	m_role->SetLabel("");
	m_role->SetBackgroundColour(wxColour(220, 220, 220));

	m_image->SetImage(wxImage());

	m_general->Show(false);
	m_generalLabel->Show(false);
	m_natural->Show(false);
	m_natLabel->Show(false);
	m_architecture->Show(false);
	m_archLabel->Show(false);
	m_politics->Show(false);
	m_poliLabel->Show(false);
	m_economy->Show(false);
	m_ecoLabel->Show(false);
	m_culture->Show(false);
	m_culLabel->Show(false);

	for ( pair<wxStaticText*, wxTextCtrl*>& custom : m_custom )
	{
		custom.first->Show(false);
		custom.second->Show(false);
	}

	m_relatedElements->ClearAll();
	Thaw();
}


///////////////////////////////////////////////////////////////////
////////////////////////// ItemShowcase ///////////////////////////
///////////////////////////////////////////////////////////////////


amItemShowcase::amItemShowcase(wxWindow* parent) : amElementShowcase(parent) {
	wxFont labelFont(wxFontInfo(12).Bold());
	wxFont contentFont(wxFontInfo(11));

	m_stHeightLabel = new wxStaticText(m_mainPanel, -1, _("Height: "));
	m_stHeightLabel->SetFont(labelFont);
	m_stHeightLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_stHeightLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_stHeightLabel->Show(false);
	m_stHeight = new wxStaticText(m_mainPanel, -1, "");
	m_stHeight->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_stHeight->SetBackgroundColour(wxColour(10, 10, 10));
	m_stHeight->SetForegroundColour(wxColour(255, 255, 255));
	m_stHeight->SetFont(contentFont);
	m_stHeight->Show(false);

	wxBoxSizer* heightLine = new wxBoxSizer(wxHORIZONTAL);
	heightLine->Add(m_stHeightLabel, wxSizerFlags().CenterVertical());
	heightLine->Add(m_stHeight, wxSizerFlags().CenterVertical());

	m_stWidthLabel = new wxStaticText(m_mainPanel, -1, _("Width: "));
	m_stWidthLabel->SetFont(labelFont);
	m_stWidthLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_stWidthLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_stWidthLabel->Show(false);
	m_stWidth = new wxStaticText(m_mainPanel, -1, "");
	m_stWidth->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_stWidth->SetBackgroundColour(wxColour(10, 10, 10));
	m_stWidth->SetForegroundColour(wxColour(255, 255, 255));
	m_stWidth->SetFont(contentFont);
	m_stWidth->Show(false);

	wxBoxSizer* widthLine = new wxBoxSizer(wxHORIZONTAL);
	widthLine->Add(m_stWidthLabel, wxSizerFlags().CenterVertical());
	widthLine->Add(m_stWidth, wxSizerFlags().CenterVertical());

	m_stDepthLabel = new wxStaticText(m_mainPanel, -1, _("Depth: "));
	m_stDepthLabel->SetFont(labelFont);
	m_stDepthLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_stDepthLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_stDepthLabel->Show(false);
	m_stDepth = new wxStaticText(m_mainPanel, -1, "");
	m_stDepth->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_stDepth->SetBackgroundColour(wxColour(10, 10, 10));
	m_stDepth->SetForegroundColour(wxColour(255, 255, 255));
	m_stDepth->SetFont(contentFont);
	m_stDepth->Show(false);

	wxBoxSizer* depthLine = new wxBoxSizer(wxHORIZONTAL);
	depthLine->Add(m_stDepthLabel, wxSizerFlags().CenterVertical());
	depthLine->Add(m_stDepth, wxSizerFlags().CenterVertical());

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

	wxVector<wxStaticText*> vLabels;
	wxVector<wxTextCtrl*> vContentCtrls;

	m_stGeneralLabel = new wxStaticText(m_mainPanel, -1, _("General"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcGeneral = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_stGeneralLabel);
	vContentCtrls.push_back(m_tcGeneral);
	m_stAppLabel = new wxStaticText(m_mainPanel, -1, _("Appearance"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcAppearance = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_stAppLabel);
	vContentCtrls.push_back(m_tcAppearance);
	m_stOriginLabel = new wxStaticText(m_mainPanel, -1, _("Origin"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcOrigin = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_stOriginLabel);
	vContentCtrls.push_back(m_tcOrigin);
	m_stHistoryLabel = new wxStaticText(m_mainPanel, -1, _("History"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcHistory = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_stHistoryLabel);
	vContentCtrls.push_back(m_tcHistory);
	m_stUsageLabel = new wxStaticText(m_mainPanel, -1, _("Usage"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcUsage = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	vLabels.push_back(m_stUsageLabel);
	vContentCtrls.push_back(m_tcUsage);

	for ( int i = 0; i < vContentCtrls.size(); i++ )
	{
		wxStaticText* label = vLabels[i];
		label->SetFont(labelFont);
		label->SetBackgroundColour(wxColour(15, 15, 15));
		label->SetForegroundColour(wxColour(230, 230, 230));
		label->Show(false);

		wxTextCtrl* content = vContentCtrls[i];
		content->SetBackgroundStyle(wxBG_STYLE_PAINT);
		content->SetBackgroundColour(wxColour(10, 10, 10));
		content->SetForegroundColour(wxColour(255, 255, 255));
		content->SetFont(wxFontInfo(9));
		content->Show(false);
		content->Bind(wxEVT_LEFT_DOWN, &amItemShowcase::EmptyMouseEvent, this);
		content->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);
		content->SetCursor(wxCURSOR_DEFAULT);
	}

	m_mainVerSizer->Add(natureLine, wxSizerFlags().Expand().Border(wxALL, 10));
	m_mainVerSizer->Add(heightLine, wxSizerFlags(0).CenterHorizontal().Border(wxTOP, 5));
	m_mainVerSizer->Add(widthLine, wxSizerFlags(0).CenterHorizontal().Border(wxTOP, 5));
	m_mainVerSizer->Add(depthLine, wxSizerFlags(0).CenterHorizontal().Border(wxTOP, 5));
	m_mainVerSizer->Add(m_stGeneralLabel, wxSizerFlags(0).Left().Border(wxLEFT | wxTOP | wxRIGHT, 10));
	m_mainVerSizer->Add(m_tcGeneral, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT, 10));
	m_mainVerSizer->Add(m_stAppLabel, wxSizerFlags(0).Left().Border(wxLEFT | wxTOP | wxRIGHT, 10));
	m_mainVerSizer->Add(m_tcAppearance, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT, 10));
	m_mainVerSizer->Add(m_stOriginLabel, wxSizerFlags(0).Left().Border(wxLEFT | wxTOP | wxRIGHT, 10));
	m_mainVerSizer->Add(m_tcOrigin, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT, 10));
	m_mainVerSizer->Add(m_stHistoryLabel, wxSizerFlags(0).Left().Border(wxLEFT | wxTOP | wxRIGHT, 10));
	m_mainVerSizer->Add(m_tcHistory, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT, 10));
	m_mainVerSizer->Add(m_stUsageLabel, wxSizerFlags(0).Left().Border(wxLEFT | wxTOP | wxRIGHT, 10));
	m_mainVerSizer->Add(m_tcUsage, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT, 10));
}

bool amItemShowcase::LoadFirstPanel(Element* element)
{
	Item* pItem = dynamic_cast<Item*>(element);
	if ( !pItem )
		return false;

	m_mainPanel->Freeze();

	m_image->Show(m_image->SetImage(pItem->image));
	m_name->SetLabel(pItem->name);

	wxString strRole;
	wxColour colRoleColour;
	switch ( pItem->role )
	{
	case iHigh:
		strRole = "Main";
		colRoleColour = { 230, 60, 60 };
		break;

	case iLow:
		strRole = "Secondary";
		colRoleColour = { 220, 220, 220, };
		break;
	}
	m_role->SetLabel(strRole);
	m_role->SetBackgroundColour(colRoleColour);

	m_stManMade->Show(pItem->isManMade);
	m_stMagic->Show(pItem->isMagic);

	bool isntEmpty = !pItem->height.IsEmpty();
	m_stHeightLabel->Show(isntEmpty);
	m_stHeight->Show(isntEmpty);
	if ( isntEmpty )
		m_stHeight->SetLabel(pItem->height);

	isntEmpty = !pItem->width.IsEmpty();
	m_stWidthLabel->Show(isntEmpty);
	m_stWidth->Show(isntEmpty);
	if ( isntEmpty )
		m_stWidth->SetLabel(pItem->width);
	
	isntEmpty = !pItem->depth.IsEmpty();
	m_stDepthLabel->Show(isntEmpty);
	m_stDepth->Show(isntEmpty);
	if ( isntEmpty )
		m_stDepth->SetLabel(pItem->depth);

	wxVector<wxTextCtrl*> vTextCtrls;

	isntEmpty = !pItem->general.IsEmpty();
	m_stGeneralLabel->Show(isntEmpty);
	m_tcGeneral->Show(isntEmpty);
	if ( isntEmpty )
	{
		m_tcGeneral->SetValue(pItem->general);
		vTextCtrls.push_back(m_tcGeneral);
	}

	isntEmpty = !pItem->appearance.IsEmpty();
	m_stAppLabel->Show(isntEmpty);
	m_tcAppearance->Show(isntEmpty);
	if ( isntEmpty )
	{
		m_tcAppearance->SetValue(pItem->appearance);
		vTextCtrls.push_back(m_tcAppearance);
	}

	isntEmpty = !pItem->origin.IsEmpty();
	m_stOriginLabel->Show(isntEmpty);
	m_tcOrigin->Show(isntEmpty);
	if ( isntEmpty )
	{
		m_tcOrigin->SetValue(pItem->origin);
		vTextCtrls.push_back(m_tcOrigin);
	}

	isntEmpty = !pItem->backstory.IsEmpty();
	m_stHistoryLabel->Show(isntEmpty);
	m_tcHistory->Show(isntEmpty);
	if ( isntEmpty )
	{
		m_tcHistory->SetValue(pItem->backstory);
		vTextCtrls.push_back(m_tcHistory);
	}

	isntEmpty = !pItem->usage.IsEmpty();
	m_stUsageLabel->Show(isntEmpty);
	m_tcUsage->Show(isntEmpty);
	if ( isntEmpty )
	{
		m_tcUsage->SetValue(pItem->usage);
		vTextCtrls.push_back(m_tcUsage);
	}

	int currentCustomCount = m_custom.size();
	int itemCustomCount = pItem->custom.size();

	if ( currentCustomCount > itemCustomCount )
	{
		pair<wxStaticText*, wxTextCtrl*>* it = m_custom.end() - 1;

		for ( int i = (currentCustomCount - 1); i > (itemCustomCount - 1); i-- )
		{
			it->first->Destroy();
			it->second->Destroy();
			m_custom.erase(it--);
		}
	}

	wxSize size(-1, 80);

	for ( int i = 0; i < pItem->custom.size(); i++ )
	{
		if ( pItem->custom[i].second.IsEmpty() )
			continue;

		if ( i >= currentCustomCount )
		{
			wxStaticText* label = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition,
				wxDefaultSize, wxBORDER_SIMPLE);
			label->SetFont(wxFontInfo(12).Bold());
			label->SetBackgroundColour(wxColour(15, 15, 15));
			label->SetForegroundColour(wxColour(230, 230, 230));
	
			wxTextCtrl* content = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition,
				size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
			content->SetBackgroundStyle(wxBG_STYLE_PAINT);
			content->SetBackgroundColour(wxColour(10, 10, 10));
			content->SetForegroundColour(wxColour(255, 255, 255));
			content->SetFont(wxFontInfo(9));
			content->SetCursor(wxCURSOR_DEFAULT);
			content->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
			content->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

			m_mainVerSizer->Add(label, wxSizerFlags(0).Left().Border(wxLEFT | wxTOP | wxRIGHT, 10));
			m_mainVerSizer->Add(content, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT, 10));

			m_custom.push_back(pair<wxStaticText*, wxTextCtrl*>{label, content});
		}

		m_custom[i].first->SetLabel(pItem->custom[i].first);
		m_custom[i].second->SetLabel(pItem->custom[i].second);

		m_custom[i].first->Show(pItem->custom[i].second != "");
		m_custom[i].second->Show(pItem->custom[i].second != "");

		vTextCtrls.push_back(m_custom[i].second);
	}

	m_mainVerSizer->FitInside(m_mainPanel);
	for ( wxTextCtrl*& textCtrl : vTextCtrls )
	{
		int nol = textCtrl->GetNumberOfLines();
		textCtrl->SetMinSize(wxSize(-1, nol > 5 ? ((nol * textCtrl->GetCharHeight()) + 5) : 80));
	}

	m_mainVerSizer->FitInside(m_mainPanel);
	m_mainPanel->Thaw();
	return true;
}

void amItemShowcase::ClearAll()
{
	Freeze();

	m_role->SetLabel("");
	m_role->SetBackgroundColour(wxColour(220, 220, 220));

	m_name->SetLabel("");
	m_image->Show(false);

	m_stManMade->Show(false);
	m_stMagic->Show(false);

	m_stHeightLabel->Show(false);
	m_stHeight->Show(false);
	m_stWidthLabel->Show(false); 
	m_stWidth->Show(false);
	m_stDepthLabel->Show(false);
	m_stDepth->Show(false);
	
	m_stGeneralLabel->Show(false);
	m_tcGeneral->Show(false);
	m_stAppLabel->Show(false);
	m_tcAppearance->Show(false);
	m_stOriginLabel->Show(false);
	m_tcOrigin->Show(false);
	m_stHistoryLabel->Show(false);
	m_tcHistory->Show(false);
	m_stUsageLabel->Show(false);
	m_tcUsage->Show(false);
	
	for ( pair<wxStaticText*, wxTextCtrl*>& custom : m_custom )
	{
		custom.first->Show(false);
		custom.second->Show(false);
	}

	m_documents->DeleteAllItems();
	m_relatedElements->ClearAll();

	Thaw();
}