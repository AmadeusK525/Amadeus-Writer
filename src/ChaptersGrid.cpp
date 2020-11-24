#include "ChaptersGrid.h"
#include "ChapterWriter.h"
#include "MainFrame.h"

#include "wxmemdbg.h"

ChaptersGrid::ChaptersGrid(wxWindow* parent): wxScrolledWindow(parent, wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | wxTAB_TRAVERSAL) {
    this->parent = (ChaptersNotebook*)parent;
    mainFrame = (MainFrame*)(parent->GetParent()->GetParent()->GetParent());

    sizer = new wxWrapSizer(wxHORIZONTAL);
    sizer->SetMinSize(wxSize(300, 300));
    SetSizer(sizer);
    FitInside();
    SetScrollRate(15, 15);
}

void ChaptersGrid::addButton() {
    wxButton* button = new wxButton(this, 10000 + parent->current, "Chapter " + std::to_string(parent->current),
        wxDefaultPosition, wxDefaultSize);
    button->SetFont(wxFontInfo(14).AntiAliased().Family(wxFONTFAMILY_ROMAN));

    chapterButtons.push_back(button);
    chapterButtons[parent->current - 1]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ChaptersGrid::openChapter, this);

    sizer->Add(chapterButtons[parent->current - 1], 1, wxGROW | wxALL, 10);
    sizer->SetItemMinSize(parent->current - 1, 190, 80);
    sizer->Layout();

    FitInside();

    parent->current++;
}

void ChaptersGrid::openChapter(wxCommandEvent& event) {
    if (!boost::filesystem::is_directory(MainFrame::currentDocFolder + "\\Files")) {
        wxMessageDialog* first = new wxMessageDialog(mainFrame, "It seems like you haven't saved your project yet.\nPlease do before writing any chapters.",
            "Save before", wxOK | wxCANCEL | wxOK_DEFAULT);
        first->SetOKCancelLabels("Save", "Cancel");
        int aa = first->ShowModal();
        if (aa == wxID_OK) {
            mainFrame->saveFile(event);

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
    ChapterWriter* writer = new ChapterWriter(mainFrame, parent, chapNumber);
}

void ChaptersGrid::clearAll() {
    sizer->Clear(true);
    chapterButtons.clear();
}
