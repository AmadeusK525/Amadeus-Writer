#include "ElementShowcases.h"
#include "MyApp.h"
#include "ElementsNotebook.h"

#include <wx\statline.h>

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
	addRelated->Bind(wxEVT_BUTTON, &amRelatedElementsContainer::OnAddElement, this);
	addRelated->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(m_owner); });
	wxButton* removeRelated = new wxButton(this, -1, "", wxDefaultPosition, wxSize(25, 25));
	removeRelated->SetBitmap(wxBITMAP_PNG(minus));
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

void amRelatedElementsContainer::OnRemoveElement(wxCommandEvent & event)
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
		}
	);

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

void amRelatedElementsDialog::OnRemoveElement(wxCommandEvent & event)
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

	m_mainPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	m_mainPanel->SetBackgroundColour(parent->GetBackgroundColour());
	m_secondPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	m_secondPanel->SetBackgroundColour(parent->GetBackgroundColour());

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

	m_relatedCharacters = new amRelatedElementsContainer(m_secondPanel, this);

	m_secondVerSizer = new wxBoxSizer(wxVERTICAL);
	m_secondVerSizer->Add(m_prevPanel, wxSizerFlags(0).Left());
	m_secondVerSizer->AddSpacer(20);
	m_secondVerSizer->Add(m_relatedCharacters, wxSizerFlags(0).Expand());

	m_secondPanel->SetSizer(m_secondVerSizer);
	m_secondVerSizer->FitInside(m_secondPanel);
	m_secondPanel->SetScrollRate(0, 20);
}

void amElementShowcase::SetData(Element* element)
{
	Freeze();
	m_name->SetLabel(element->name);
	Thaw();
}

void amElementShowcase::ClearAll()
{

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

	m_ageLabel = new wxStaticText(m_mainPanel, -1, _("Age: "));
	m_ageLabel->SetFont(font);
	m_ageLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_ageLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_ageLabel->Show(false);
	m_age = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(45, 20), wxBORDER_NONE);
	m_age->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_age->SetBackgroundColour(wxColour(10, 10, 10));
	m_age->SetForegroundColour(wxColour(255, 255, 255));
	m_age->SetFont(font2);
	m_age->Show(false);

	m_sexLabel = new wxStaticText(m_mainPanel, -1, _("Sex: "));
	m_sexLabel->SetFont(font);
	m_sexLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_sexLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_sexLabel->Show(false);
	m_sex = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_NONE);
	m_sex->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_sex->SetBackgroundColour(wxColour(225, 225, 225));
	m_sex->SetFont(font2);
	m_sex->Show(false);

	m_heightLabel = new wxStaticText(m_mainPanel, -1, _("Height: "));
	m_heightLabel->SetFont(font);
	m_heightLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_heightLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_heightLabel->Show(false);
	m_height = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_NONE);
	m_height->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_height->SetBackgroundColour(wxColour(10, 10, 10));
	m_height->SetForegroundColour(wxColour(255, 255, 255));
	m_height->SetFont(font2);
	m_height->Show(false);

	wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
	firstLine->Add(m_ageLabel, wxSizerFlags(0).Border(wxLEFT, 10));
	firstLine->Add(m_age, wxSizerFlags(1));
	firstLine->Add(m_sexLabel, wxSizerFlags(0).Border(wxLEFT, 20));
	firstLine->Add(m_sex, wxSizerFlags(2));
	firstLine->Add(m_heightLabel, wxSizerFlags(0).Border(wxLEFT, 20));
	firstLine->Add(m_height, wxSizerFlags(2).Border(wxRIGHT, 5));

	m_nickLabel = new wxStaticText(m_mainPanel, -1, _("Nickname: "));
	m_nickLabel->SetFont(font);
	m_nickLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_nickLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_nickLabel->Show(false);
	m_nick = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(100, 20), wxBORDER_NONE);
	m_nick->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_nick->SetBackgroundColour(wxColour(10, 10, 10));
	m_nick->SetForegroundColour(wxColour(255, 255, 255));
	m_nick->SetFont(font2);
	m_nick->Show(false);

	m_natLabel = new wxStaticText(m_mainPanel, -1, _("Nationality: "));
	m_natLabel->SetFont(font);
	m_natLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_natLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_natLabel->Show(false);
	m_nat = new wxStaticText(m_mainPanel, -1, "", wxDefaultPosition, wxSize(120, 20), wxBORDER_NONE);
	m_nat->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_nat->SetBackgroundColour(wxColour(10, 10, 10));
	m_nat->SetForegroundColour(wxColour(255, 255, 255));
	m_nat->SetFont(font2);
	m_nat->Show(false);

	wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
	secondLine->Add(m_nickLabel, wxSizerFlags(0));
	secondLine->Add(m_nick, wxSizerFlags(1));
	secondLine->Add(m_natLabel, wxSizerFlags(0).Border(wxLEFT, 20));
	secondLine->Add(m_nat, wxSizerFlags(1));

	m_appLabel = new wxStaticText(m_mainPanel, -1, _("Appearance"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_appLabel->SetFont(font);
	m_appLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_appLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_appLabel->Show(false);
	m_appearance = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_appearance->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_appearance->SetBackgroundColour(wxColour(10, 10, 10));
	m_appearance->SetForegroundColour(wxColour(255, 255, 255));
	m_appearance->SetFont(wxFontInfo(9));
	m_appearance->Show(false);
	m_appearance->Bind(wxEVT_LEFT_DOWN, &amCharacterShowcase::EmptyMouseEvent, this);
	m_appearance->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

	m_perLabel = new wxStaticText(m_mainPanel, -1, _("Personality"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_perLabel->SetFont(font);
	m_perLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_perLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_perLabel->Show(false);
	m_personality = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_personality->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_personality->SetBackgroundColour(wxColour(10, 10, 10));
	m_personality->SetForegroundColour(wxColour(255, 255, 255));
	m_personality->SetFont(wxFontInfo(9));
	m_personality->Show(false);
	m_personality->Bind(wxEVT_LEFT_DOWN, &amCharacterShowcase::EmptyMouseEvent, this);
	m_personality->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

	m_bsLabel = new wxStaticText(m_mainPanel, -1, _("Backstory"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_bsLabel->SetFont(font);
	m_bsLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_bsLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_bsLabel->Show(false);
	m_backstory = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_backstory->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_backstory->SetBackgroundColour(wxColour(10, 10, 10));
	m_backstory->SetForegroundColour(wxColour(255, 255, 255));
	m_backstory->SetFont(wxFontInfo(9));
	m_backstory->Show(false);
	m_backstory->Bind(wxEVT_LEFT_DOWN, &amCharacterShowcase::EmptyMouseEvent, this);
	m_backstory->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

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

	m_appearance->SetCursor(wxCURSOR_DEFAULT);
	m_personality->SetCursor(wxCURSOR_DEFAULT);
	m_backstory->SetCursor(wxCURSOR_DEFAULT);
}

void amCharacterShowcase::SetData(Character* character)
{
	if ( !character )
	{
		ClearAll();
		return;
	}

	wxBusyCursor cursor;

	Freeze();
	m_name->SetLabel(character->name);
	m_isAlive->Show();

	bool isNotEmpty = !character->sex.IsEmpty();
	m_sex->Show(isNotEmpty);
	m_sexLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_sex->SetLabel(character->sex);
		wxColour bg;
		wxColour fg;

		if ( character->sex == "Female" )
		{
			bg = { 255,182,193 };
			fg = { 0, 0, 0 };
		}
		else if ( character->sex == "Male" )
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

	isNotEmpty = !character->age.IsEmpty();
	m_age->Show(isNotEmpty);
	m_ageLabel->Show(isNotEmpty);
	if ( isNotEmpty )
		m_age->SetLabel(character->age);

	isNotEmpty = !character->height.IsEmpty();
	m_height->Show(isNotEmpty);
	m_heightLabel->Show(isNotEmpty);
	if ( isNotEmpty )
		m_height->SetLabel(character->height);

	isNotEmpty = !character->nat.IsEmpty();
	m_nat->Show(isNotEmpty);
	m_natLabel->Show(isNotEmpty);
	if ( isNotEmpty )
		m_nat->SetLabel(character->nat);

	isNotEmpty = !character->nick.IsEmpty();
	m_nick->Show(isNotEmpty);
	m_nickLabel->Show(isNotEmpty);
	if ( isNotEmpty )
		m_nick->SetLabel(character->nick);

	wxString role;
	wxColour rolebg;
	wxColour rolefg;

	switch ( character->role )
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

	isNotEmpty = !character->appearance.IsEmpty();
	m_appearance->Show(isNotEmpty);
	m_appLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_appearance->SetValue(character->appearance);
		vTextCtrls.push_back(m_appearance);
	}

	isNotEmpty = !character->personality.IsEmpty();
	m_personality->Show(isNotEmpty);
	m_perLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_personality->SetValue(character->personality);
		vTextCtrls.push_back(m_personality);
	}

	isNotEmpty = !character->backstory.IsEmpty();
	m_backstory->Show(isNotEmpty);
	m_bsLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_backstory->SetValue(character->backstory);
		vTextCtrls.push_back(m_backstory);
	}

	m_image->Show(m_image->SetImage(character->image));

	int tcsize = m_custom.size();
	int ccsize = character->custom.size();

	if ( tcsize > ccsize )
	{
		pair<wxStaticText*, wxTextCtrl*>* it = m_custom.end() - 1;

		for ( int i = (tcsize - 1); i > (ccsize - 1); i-- )
		{
			it->first->Destroy();
			it->second->Destroy();
			m_custom.erase(it--);
		}
	}

	wxSize size(-1, 80);

	for ( int i = 0; i < character->custom.size(); i++ )
	{
		if ( character->custom[i].second.IsEmpty() )
			continue;

		if ( i >= tcsize )
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
		m_custom[i].first->SetLabel(character->custom[i].first);
		m_custom[i].second->SetLabel(character->custom[i].second);
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

	m_relatedCharacters->LoadAllElements(character);
	m_secondVerSizer->FitInside(m_secondPanel);

	Thaw();
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

	Thaw();
}


///////////////////////////////////////////////////////////////////
///////////////////////// LocationShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


amLocationShowcase::amLocationShowcase(wxWindow* parent) :amElementShowcase(parent)
{
	wxFont font(wxFontInfo(12).Bold());
	wxFont font2(wxFontInfo(9));

	m_generalLabel = new wxStaticText(m_mainPanel, -1, "General", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_generalLabel->SetFont(font);
	m_generalLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_generalLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_generalLabel->Show(false);
	m_general = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_general->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_general->SetBackgroundColour(wxColour(10, 10, 10));
	m_general->SetForegroundColour(wxColour(255, 255, 255));
	m_general->SetFont(font2);
	m_general->Show(false);
	m_general->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
	m_general->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

	m_natLabel = new wxStaticText(m_mainPanel, -1, _("Natural Aspects"), wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_natLabel->SetFont(font);
	m_natLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_natLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_natLabel->Show(false);
	m_natural = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_natural->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_natural->SetBackgroundColour(wxColour(10, 10, 10));
	m_natural->SetForegroundColour(wxColour(255, 255, 255));
	m_natural->SetFont(font2);
	m_natural->Show(false);
	m_natural->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
	m_natural->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

	m_archLabel = new wxStaticText(m_mainPanel, -1, _("Architecture"), wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_archLabel->SetFont(font);
	m_archLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_archLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_archLabel->Show(false);
	m_architecture = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_architecture->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_architecture->SetBackgroundColour(wxColour(10, 10, 10));
	m_architecture->SetForegroundColour(wxColour(255, 255, 255));
	m_architecture->SetFont(font2);
	m_architecture->Show(false);
	m_architecture->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
	m_architecture->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

	m_ecoLabel = new wxStaticText(m_mainPanel, -1, _("Economy"), wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_ecoLabel->SetFont(font);
	m_ecoLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_ecoLabel->SetForegroundColour(wxColour(230, 230, 230));
	m_ecoLabel->Show(false);
	m_economy = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_economy->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_economy->SetBackgroundColour(wxColour(10, 10, 10));
	m_economy->SetForegroundColour(wxColour(255, 255, 255));
	m_economy->SetFont(font2);
	m_economy->Show(false);
	m_economy->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
	m_economy->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

	m_culLabel = new wxStaticText(m_mainPanel, -1, _("Culture"), wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_SIMPLE);
	m_culLabel->SetFont(font);
	m_culLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_culLabel->SetForegroundColour(wxColour(255, 255, 255));
	m_culLabel->Show(false);
	m_culture = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_culture->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_culture->SetBackgroundColour(wxColour(10, 10, 10));
	m_culture->SetForegroundColour(wxColour(255, 255, 255));
	m_culture->SetFont(font2);
	m_culture->Show(false);
	m_culture->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
	m_culture->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

	m_poliLabel = new wxStaticText(m_mainPanel, -1, _("Politics"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_SIMPLE);
	m_poliLabel->SetFont(font);
	m_poliLabel->SetBackgroundColour(wxColour(15, 15, 15));
	m_poliLabel->SetForegroundColour(wxColour(255, 255, 255));
	m_poliLabel->Show(false);
	m_politics = new wxTextCtrl(m_mainPanel, -1, "", wxDefaultPosition, wxSize(-1, 80),
		wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_NONE);
	m_politics->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_politics->SetBackgroundColour(wxColour(10, 10, 10));
	m_politics->SetForegroundColour(wxColour(255, 255, 255));
	m_politics->SetFont(font2);
	m_politics->Show(false);
	m_politics->Bind(wxEVT_LEFT_DOWN, &amLocationShowcase::EmptyMouseEvent, this);
	m_politics->Bind(wxEVT_MOUSEWHEEL, &wxScrolledWindow::HandleOnMouseWheel, m_mainPanel);

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

	m_general->SetCursor(wxCURSOR_DEFAULT);
	m_natural->SetCursor(wxCURSOR_DEFAULT);
	m_architecture->SetCursor(wxCURSOR_DEFAULT);
	m_economy->SetCursor(wxCURSOR_DEFAULT);
	m_culture->SetCursor(wxCURSOR_DEFAULT);
	m_politics->SetCursor(wxCURSOR_DEFAULT);
}

void amLocationShowcase::SetData(Location* location)
{
	if ( !location )
	{
		ClearAll();
		return;
	}

	wxBusyCursor cursor;

	Freeze();
	m_name->SetLabel(location->name);

	switch ( location->role )
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

	bool isNotEmpty = !location->general.IsEmpty();
	m_general->Show(isNotEmpty);
	m_generalLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_general->SetValue(location->general);
		vTextCtrls.push_back(m_general);
	}

	isNotEmpty = !location->natural.IsEmpty();
	m_natural->Show(isNotEmpty);
	m_natLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_natural->SetValue(location->natural);
		vTextCtrls.push_back(m_natural);
	}

	isNotEmpty = !location->architecture.IsEmpty();
	m_architecture->Show(isNotEmpty);
	m_archLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_architecture->SetValue(location->architecture);
		vTextCtrls.push_back(m_architecture);
	}

	isNotEmpty = !location->politics.IsEmpty();
	m_politics->Show(isNotEmpty);
	m_poliLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_politics->SetValue(location->politics);
		vTextCtrls.push_back(m_politics);
	}

	isNotEmpty = !location->economy.IsEmpty();
	m_economy->Show(isNotEmpty);
	m_ecoLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_economy->SetValue(location->economy);
		vTextCtrls.push_back(m_economy);
	}

	isNotEmpty = !location->culture.IsEmpty();
	m_culture->Show(isNotEmpty);
	m_culLabel->Show(isNotEmpty);
	if ( isNotEmpty )
	{
		m_culture->SetValue(location->culture);
		vTextCtrls.push_back(m_culture);
	}

	m_image->Show(m_image->SetImage(location->image));

	int tcsize = m_custom.size();
	int ccsize = location->custom.size();

	if ( tcsize > ccsize )
	{
		pair<wxStaticText*, wxTextCtrl*>* it = m_custom.end() - 1;

		for ( int i = (tcsize - 1); i > (ccsize - 1); i-- )
		{
			it->first->Destroy();
			it->second->Destroy();
			m_custom.erase(it--);

			m_first = !m_first;
		}
	}

	wxSize size(-1, 80);

	for ( int i = 0; i < location->custom.size(); i++ )
	{
		if ( location->custom[i].second.IsEmpty() )
			continue;

		if ( i >= tcsize )
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

		m_custom[i].first->SetLabel(location->custom[i].first);
		m_custom[i].second->SetLabel(location->custom[i].second);

		m_custom[i].first->Show(location->custom[i].second != "");
		m_custom[i].second->Show(location->custom[i].second != "");

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
	
	m_relatedCharacters->LoadAllElements(location);
	m_secondVerSizer->FitInside(m_secondPanel);

	Thaw();
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

	Thaw();
}

amItemShowcase::amItemShowcase(wxWindow* parent) : amElementShowcase(parent) {}

void amItemShowcase::SetData(Item* item) {}

void amItemShowcase::ClearAll()
{}