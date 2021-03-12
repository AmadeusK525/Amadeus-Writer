#include "StoryNotebook.h"
#include "StoryWriter.h"
#include "MyApp.h"

#include <wx\dir.h>

amStoryNotebook::amStoryNotebook(wxWindow* parent, amProjectManager* manager) :
    wxNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE), m_manager(manager) {
    m_grid = new StoryGrid(this, m_manager);
    m_grid->SetBackgroundColour(wxColour(150, 0, 0));

    m_list = new wxListView(this, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
    m_list->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, FromDIP(180));
    m_list->InsertColumn(1, "Characters", wxLIST_FORMAT_CENTER);
    m_list->InsertColumn(2, "Locations", wxLIST_FORMAT_CENTER);
    m_list->InsertColumn(3, "Point of View", wxLIST_FORMAT_CENTER, FromDIP(180));

    m_list->SetBackgroundColour(wxColour(45, 45, 45));
    m_list->SetForegroundColour(wxColour(245, 245, 245));

    AddPage(m_grid, "Grid");
    AddPage(m_list, "List");
}

void amStoryNotebook::AddChapter(Chapter& chapter, int pos) {
    m_grid->AddButton();
    AddToList(chapter, pos);
}

void amStoryNotebook::AddToList(Chapter& chapter, int pos) {
    if (pos == -1)
        pos = m_list->GetItemCount();

    m_list->InsertItem(pos, chapter.name);
    m_list->SetItem(pos, 1, std::to_string(chapter.characters.size()));
    m_list->SetItem(pos, 2, std::to_string(chapter.locations.size()));
    //m_list->SetItem(pos, 3, chapter.pointOfView);
}

void amStoryNotebook::LayoutGrid() {
    m_grid->SetVirtualSize(m_grid->GetClientSize());
    m_grid->Layout();
}

void amStoryNotebook::ClearAll() {
    m_grid->ClearAll();
    m_list->DeleteAllItems();
}


//////////////////////////////////////////////////////////////////
//////////////////////////// StoryGrid /////////////////////////
//////////////////////////////////////////////////////////////////


StoryGrid::StoryGrid(wxWindow* parent, amProjectManager* manager) : wxScrolledWindow(parent, wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL) {
    m_manager = manager;

    m_btnSizer = new wxWrapSizer(wxHORIZONTAL);
    m_btnSizer->SetMinSize(wxSize(300, 300));
    SetSizer(m_btnSizer);
    FitInside();
    SetScrollRate(15, 15);
}

void StoryGrid::AddButton() {
    amStoryNotebook* stroyNotebook = ((amStoryNotebook*)GetParent());
    int current = m_buttons.size() + 1;

    wxButton* button = new wxButton(this, 10000 + current,
        "Chapter " + std::to_string(current),
        wxDefaultPosition, wxDefaultSize);
    button->SetFont(wxFontInfo(14).AntiAliased().Family(wxFONTFAMILY_ROMAN));

    m_buttons.push_back(button);
    m_buttons[m_buttons.size() - 1]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &StoryGrid::OnButtonPressed, this);

    m_btnSizer->Add(m_buttons[current - 1], 1, wxGROW | wxALL, 10);
    m_btnSizer->SetItemMinSize(current - 1, 190, 80);
    m_btnSizer->Layout();

    FitInside();
}

void StoryGrid::OpenChapter(unsigned int chapterIndex) {
    m_manager->OpenChapter(chapterIndex, 0);
}

void StoryGrid::OnButtonPressed(wxCommandEvent& event) {
    wxBusyCursor wait;

    int chapterIndex = event.GetId() - 10001;
    OpenChapter(chapterIndex);
}

void StoryGrid::ClearAll() {
    m_btnSizer->Clear(true);
    m_buttons.clear();
}