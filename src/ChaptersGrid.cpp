#include "ChaptersGrid.h"
#include "ChapterWriter.h"

#include "MyApp.h"

#include "wxmemdbg.h"

ChapterGrid::ChapterGrid(wxWindow* parent, amProjectManager* manager): wxScrolledWindow(parent, wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL) {
    m_manager = manager;

    m_btnSizer = new wxWrapSizer(wxHORIZONTAL);
    m_btnSizer->SetMinSize(wxSize(300, 300));
    SetSizer(m_btnSizer);
    FitInside();
    SetScrollRate(15, 15);
}

void ChapterGrid::AddButton() {
    amChaptersNotebook* chapNote = ((amChaptersNotebook*)GetParent());
    int current = m_buttons.size() + 1;

    wxButton* button = new wxButton(this, 10000 + current,
        "Chapter " + std::to_string(current),
        wxDefaultPosition, wxDefaultSize);
    button->SetFont(wxFontInfo(14).AntiAliased().Family(wxFONTFAMILY_ROMAN));

    m_buttons.push_back(button);
    m_buttons[m_buttons.size() - 1]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ChapterGrid::OnButtonPressed, this);

    m_btnSizer->Add(m_buttons[current - 1], 1, wxGROW | wxALL, 10);
    m_btnSizer->SetItemMinSize(current - 1, 190, 80);
    m_btnSizer->Layout();

    FitInside();
}

void ChapterGrid::OpenChapter(unsigned int chapterNumber) {
    amChapterWriter* writer = new amChapterWriter(m_manager->GetMainFrame(), m_manager, chapterNumber);
    writer->Show();
}

void ChapterGrid::OnButtonPressed(wxCommandEvent& event) {
    wxBusyCursor wait;

    int chapNumber = event.GetId() - 10000;
    OpenChapter(chapNumber);
}

void ChapterGrid::ClearAll() {
    m_btnSizer->Clear(true);
    m_buttons.clear();
}
