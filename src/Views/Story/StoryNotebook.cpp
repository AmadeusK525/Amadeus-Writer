#include "Views/Story/StoryNotebook.h"
#include "Views/StoryWriter/StoryWriter.h"
#include "MyApp.h"

#include <wx/dir.h>

#include "Utils/wxmemdbg.h"

amStoryNotebook::amStoryNotebook(wxWindow* parent) :
	wxAuiNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	wxAuiSimpleTabArt* pArt = new wxAuiSimpleTabArt();
	pArt->SetColour(wxColour(50, 50, 50));
	pArt->SetActiveColour(wxColour(30, 30, 30));
	SetArtProvider(pArt);

	GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

	m_grid = new StoryGrid(this);
	m_grid->SetBackgroundColour(wxColour(150, 0, 0));

	wxPanel* pListPanel = new wxPanel(this);

	m_list = new wxListView(pListPanel, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
	m_list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, FromDIP(180));
	m_list->InsertColumn(1, "Characters", wxLIST_FORMAT_CENTER);
	m_list->InsertColumn(2, "Locations", wxLIST_FORMAT_CENTER);
	m_list->InsertColumn(3, "Items", wxLIST_FORMAT_CENTER);
	m_list->InsertColumn(4, "Point of View", wxLIST_FORMAT_CENTER, FromDIP(180));

	m_list->SetBackgroundColour(wxColour(45, 45, 45));
	m_list->SetForegroundColour(wxColour(245, 245, 245));

	wxStaticText* pWIP = new wxStaticText(pListPanel, -1, "Work in Progress");
	pWIP->SetFont(wxFontInfo(15).Bold());
	pWIP->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* pListSizer = new wxBoxSizer(wxVERTICAL);
	pListSizer->Add(m_list, wxSizerFlags(1).Expand());
	pListSizer->Add(pWIP, wxSizerFlags(0).Left().Border(wxALL, 5));

	pListPanel->SetSizer(pListSizer);

	AddPage(m_grid, "Grid");
	AddPage(pListPanel, "List");
}

void amStoryNotebook::SetBookData(am::Book* book)
{
	ClearAll();

	for ( am::Document*& pDocument : book->documents )
	{
		AddDocument(pDocument);
	}
}

void amStoryNotebook::AddDocument(am::Document* document, int pos)
{
	m_grid->AddButton();
	AddToList(document, pos);
}

void amStoryNotebook::AddToList(am::Document* document, int pos)
{
	if ( pos == -1 )
		pos = m_list->GetItemCount();

	size_t characterCount = 0, locationCount = 0, itemCount = 0;
	for ( am::TangibleElement*& pElement : document->vTangibleElements )
	{
		if ( dynamic_cast<am::Character*>(pElement) )
			characterCount++;
		else if ( dynamic_cast<am::Location*>(pElement) )
			locationCount++;
		else if ( dynamic_cast<am::Item*>(pElement) )
			itemCount++;
	}

	m_list->InsertItem(pos, document->name);
	m_list->SetItem(pos, 1, std::to_string(characterCount));
	m_list->SetItem(pos, 2, std::to_string(locationCount));
	m_list->SetItem(pos, 3, std::to_string(itemCount));
	//m_list->SetItem(pos, 3, document.pointOfView);
}

void amStoryNotebook::DeleteDocument(am::Document* document)
{
	m_grid->DeleteButton();
	RemoveFromList(document);
}

void amStoryNotebook::RemoveFromList(am::Document* document)
{
	m_list->DeleteItem(document->position - 1);
}

void amStoryNotebook::LayoutGrid()
{
	m_grid->SetVirtualSize(m_grid->GetClientSize());
	m_grid->Layout();
}

void amStoryNotebook::ClearAll()
{
	m_grid->ClearAll();
	m_list->DeleteAllItems();
}


//////////////////////////////////////////////////////////////////
//////////////////////////// StoryGrid /////////////////////////
//////////////////////////////////////////////////////////////////


StoryGrid::StoryGrid(wxWindow* parent) : wxScrolledWindow(parent, wxID_ANY,
	wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL)
{
	m_btnSizer = new wxWrapSizer(wxHORIZONTAL);
	m_btnSizer->SetMinSize(wxSize(300, 300));

	wxStaticText* pWIP = new wxStaticText(this, -1, "Work in Progress");
	pWIP->SetFont(wxFontInfo(15).Bold());
	pWIP->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
	pSizer->Add(m_btnSizer, wxSizerFlags(1).Expand());
	pSizer->Add(pWIP, wxSizerFlags(0).Left().Border(wxALL, 5));
	
	SetSizer(pSizer);
	FitInside();
	SetScrollRate(15, 15);
}

void StoryGrid::AddButton()
{
	amStoryNotebook* stroyNotebook = ((amStoryNotebook*)GetParent());
	int current = m_buttons.size() + 1;

	wxButton* button = new wxButton(this, 10000 + current,
		"Document " + std::to_string(current),
		wxDefaultPosition, FromDIP(wxSize(190, 80)));
	button->SetFont(wxFontInfo(14).AntiAliased().Family(wxFONTFAMILY_ROMAN));

	m_buttons.push_back(button);
	m_buttons[m_buttons.size() - 1]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &StoryGrid::OnButtonPressed, this);

	m_btnSizer->Add(m_buttons[current - 1], 1, wxGROW | wxALL, 10);
	m_btnSizer->Layout();

	FitInside();
}

void StoryGrid::DeleteButton()
{
	m_buttons.back()->Destroy();
	m_buttons.pop_back();
}

void StoryGrid::OpenDocument(unsigned int documentIndex)
{
	am::OpenDocument(documentIndex);
}

void StoryGrid::OnButtonPressed(wxCommandEvent& event)
{
	wxBusyCursor wait;

	int documentIndex = event.GetId() - 10001;
	OpenDocument(documentIndex);
}

void StoryGrid::ClearAll()
{
	m_btnSizer->Clear(true);
	m_buttons.clear();
}