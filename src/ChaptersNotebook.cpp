#include "ChaptersNotebook.h"

#include "ChaptersGrid.h"

#include "MyApp.h"

#include <wx\dir.h>


amdChaptersNotebook::amdChaptersNotebook(wxWindow* parent, amdProjectManager* manager) :
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

void amdChaptersNotebook::AddChapter(Chapter& chapter, int pos, bool Reposition) {
    m_grid->AddButton();
    AddToList(chapter, pos);

    // Redeclare all chapter positions  
    if (Reposition)
        RepositionChapters();
}

void amdChaptersNotebook::AddToList(Chapter& chapter, int pos) {
    m_list->InsertItem(pos, chapter.name);
    m_list->SetItem(pos, 1, std::to_string(chapter.characters.size()));
    m_list->SetItem(pos, 2, std::to_string(chapter.locations.size()));
    //m_list->SetItem(pos, 3, chapter.pointOfView);
}

void amdChaptersNotebook::RepositionChapters() {
    int i = 1;
    wxVector<Chapter>& chapters = m_manager->GetChapters();

    for (auto it = chapters.begin(); it != chapters.end(); it++) {
        it->position = i++;
    }

    wxFileName::Rmdir(m_manager->GetPath(true).ToStdString() + "Files\\Chapters", wxPATH_RMDIR_RECURSIVE);
    wxFileName::Mkdir(m_manager->GetPath(true).ToStdString() + "Files\\Chapters");
    m_manager->SaveProject();
}

void amdChaptersNotebook::ClearAll() {
    m_grid->ClearAll();
    m_list->DeleteAllItems();
}
