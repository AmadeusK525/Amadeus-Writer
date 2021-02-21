#include "ChaptersNotebook.h"
#include "ChaptersGrid.h"
#include "MyApp.h"

#include <wx\dir.h>

amChaptersNotebook::amChaptersNotebook(wxWindow* parent, amProjectManager* manager) :
    wxNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE), m_manager(manager) {
    m_grid = new ChapterGrid(this, m_manager);
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

void amChaptersNotebook::AddChapter(Chapter& chapter, int pos) {
    m_grid->AddButton();
    AddToList(chapter, pos);
}

void amChaptersNotebook::AddToList(Chapter& chapter, int pos) {
    if (pos == -1)
        pos = m_list->GetItemCount();

    m_list->InsertItem(pos, chapter.name);
    m_list->SetItem(pos, 1, std::to_string(chapter.characters.size()));
    m_list->SetItem(pos, 2, std::to_string(chapter.locations.size()));
    //m_list->SetItem(pos, 3, chapter.pointOfView);
}

void amChaptersNotebook::LayoutGrid() {
    m_grid->SetVirtualSize(m_grid->GetClientSize());
    m_grid->Layout();
}

void amChaptersNotebook::ClearAll() {
    m_grid->ClearAll();
    m_list->DeleteAllItems();
}
