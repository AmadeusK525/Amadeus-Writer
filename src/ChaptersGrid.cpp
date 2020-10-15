#include "ChaptersGrid.h"
//#include "MainFrame.h"

#include "wxmemdbg.h"

ChaptersGrid::ChaptersGrid(wxWindow* parent): wxScrolledWindow(parent, wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | wxTAB_TRAVERSAL) {
    this->parent = dynamic_cast<MainFrame*>(parent->GetParent()->GetParent()->GetParent());
    sizer = new wxWrapSizer(wxHORIZONTAL);
    sizer->SetMinSize(wxSize(300, 300));
    SetSizer(sizer);
    FitInside();
    SetScrollRate(15, 15);
}

void ChaptersGrid::addChapter(Chapter& chapter, int& pos) {

    if (pos < current) {
        auto it = chapters.begin();
        for (int i = 0; i < pos; i++) {
            it++;
        }
        chapters.insert(it, chapter);
    } else {
        chapters.push_back(chapter);
    }

    MainFrame::saved[2]++;
    addButton();
    MainFrame::isSaved = false;
}

void ChaptersGrid::addButton() {
    wxButton* button = new wxButton(this, 10000 + current, "Chapter " + std::to_string(current),
        wxDefaultPosition, wxDefaultSize);
    button->SetFont(wxFont(wxFontInfo(14).AntiAliased().Family(wxFONTFAMILY_ROMAN)));

    chapterButtons.push_back(button);
    chapterButtons[current - 1]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ChaptersGrid::openChapter, this);

    sizer->Add(chapterButtons[current - 1], 1, wxGROW | wxALL, 10);
    sizer->SetItemMinSize(current - 1, 190, 80);
    sizer->Layout();

    FitInside();

    current++;
}

void ChaptersGrid::openChapter(wxCommandEvent& event) {
    if (!boost::filesystem::is_directory(MainFrame::currentDocFolder + "\\Chapters")) {
        wxMessageDialog* first = new wxMessageDialog(parent, "It seems like you haven't saved your project yet.\nPlease do before writing any chapters.",
            "Save before", wxOK | wxCANCEL | wxOK_DEFAULT);
        first->SetOKCancelLabels("Save", "Cancel");
        int aa = first->ShowModal();
        if (aa == wxID_OK) {
            parent->saveFile(event);

            if (!MainFrame::isSaved)
                return;
        } else {
            event.Skip();
            return;
        }

        if (first)
            delete first;
    }

    wxBusyCursor wait;

    int chapNumber = event.GetId() - 10000;
    ChapterWriter* writer = new ChapterWriter(parent, chapters, chapNumber);
}

list<Chapter> ChaptersGrid::getChapters() {
    return chapters;
}

void ChaptersGrid::clearAll() {
    chapters.clear();
    sizer->Clear(true);
    chapterButtons.clear();
    current = 1;
}
