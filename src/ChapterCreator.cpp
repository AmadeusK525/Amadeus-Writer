#include "ChapterCreator.h"
#include "Chapter.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(ChapterCreator, wxFrame)

EVT_BUTTON(BUTTON_NextChapter, ChapterCreator::next)
EVT_BUTTON(BUTTON_BackChapter, ChapterCreator::back)
EVT_BUTTON(BUTTON_CancelChapter, ChapterCreator::cancel)
EVT_BUTTON(BUTTON_CreateChapter, ChapterCreator::create)

EVT_CLOSE(ChapterCreator::checkClose)

END_EVENT_TABLE()

ChapterCreator::ChapterCreator(wxWindow* parent, ChaptersNotebook* notebook) :
    wxFrame(parent, wxID_ANY, "Create chapter", wxDefaultPosition, wxSize(500, 350), wxMINIMIZE_BOX | wxSYSTEM_MENU |
    wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_SHAPED | wxFRAME_FLOAT_ON_PARENT) {

    this->mainFrame = dynamic_cast<MainFrame*>(parent);
    this->notebook = notebook;
    this->CenterOnParent();
    this->SetBackgroundColour("WHITE");

    nchapPanel1 = new wxPanel(this, wxID_ANY);
    nchapPanel1->SetBackgroundColour("WHITE");
    nchapPanel1->Show(true);

    //wxButton* nchapCreate = new wxButton(this, BUTTON_NewChapter, "Create");

    wxStaticText* label1 = new wxStaticText(nchapPanel1, wxID_ANY, "Name: ", wxPoint(45, 10), wxSize(50, 20), wxTE_READONLY | wxNO_BORDER);
    label1->SetFont(wxFont(wxFontInfo(12)));
    label1->SetBackgroundColour("WHITE");
    nchapName = new wxTextCtrl(nchapPanel1, wxID_ANY, "Chapter " + std::to_string(notebook->current), wxPoint(105, 10), wxSize(365, 25));
    nchapName->SetBackgroundColour(wxColour(200, 200, 200));
    nchapName->SetFont(wxFont(wxFontInfo(10)));

    wxStaticText* label2 = new wxStaticText(nchapPanel1, wxID_ANY, "Summary", wxPoint(45, 45), wxSize(425, 20));
    label2->SetBackgroundColour(wxColour(230, 30, 30));
    label2->SetFont(wxFont(wxFontInfo(13).Bold()));
    nchapSummary = new wxTextCtrl(nchapPanel1, wxID_ANY, "", wxPoint(45, 65), wxSize(425, 205)), wxTE_MULTILINE;

    mainLabel = new wxStaticText(nchapPanel1, wxID_ANY, "C\nH\nA\nP\nT\nE\nR", wxPoint(10, 55), wxSize(25, 200), wxTE_MULTILINE | wxTE_READONLY | wxTE_NO_VSCROLL | wxBORDER_NONE);
    mainLabel->SetForegroundColour(wxColour(100, 100, 100));
    mainLabel->SetFont(wxFont(wxFontInfo(16).Bold()));

    nchapNext = new wxButton(nchapPanel1, BUTTON_NextChapter, "Next", wxPoint(285, 275), wxSize(90, 30));
    nchapCancel = new wxButton(nchapPanel1, BUTTON_CancelChapter, "Cancel", wxPoint(380, 275), wxSize(90, 30));

    SetIcon(wxICON(chapterIcon));
    Layout();

    nchapPanel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500, 350));
    nchapPanel2->SetBackgroundColour("WHITE");
    nchapPanel2->Hide();

    nchapBack = new wxButton(nchapPanel2, BUTTON_BackChapter, "Back", wxPoint(45, 275), wxSize(90, 30));

    nchapList = new DragList(nchapPanel2, wxSize(425, 260));
    nchapList->SetPosition(wxPoint(45, 10));
    nchapList->SetBackgroundColour(wxColour(245, 245, 245));
}

void ChapterCreator::next(wxCommandEvent& event) {

    if (firstNext) {
        tempName = nchapName->GetValue();

        nchapList->AppendColumn("Drag chapter '" + tempName + "' to desired position", wxLIST_FORMAT_LEFT, 2000);
        nchapList->SetColumnWidth(0, nchapList->GetSize().x);

        int i = 0;
        for (auto it = notebook->chapters.begin(); it != notebook->chapters.end(); it++) {
            nchapList->InsertItem(i, it->name);
            i++;
        }

        nchapList->InsertItem(nchapList->GetItemCount(), tempName);
        nchapList->SetItemBackgroundColour(nchapList->FindItem(-1, tempName), wxColour(255, 230, 230));
        firstNext = false;
    } else {
        int item = nchapList->FindItem(-1, tempName);

        tempName = nchapName->GetValue();
        nchapList->SetItemText(item, tempName);
    }

    nchapList->tempName = this->tempName;
    nchapList->EnsureVisible(nchapList->FindItem(-1, tempName));
    nchapList->Select(nchapList->FindItem(-1, tempName));

    mainLabel->Reparent(nchapPanel2);
    nchapNext->Reparent(nchapPanel2);
    nchapNext->SetLabel("Create");
    nchapCancel->Reparent(nchapPanel2);

    nchapPanel1->Hide();
    nchapPanel2->Show();
    nchapNext->SetId(BUTTON_CreateChapter);

    event.Skip();
}

void ChapterCreator::back(wxCommandEvent& event) {
    mainLabel->Reparent(nchapPanel1);
    nchapNext->Reparent(nchapPanel1);
    nchapNext->SetLabel("Next");
    nchapCancel->Reparent(nchapPanel1);
    nchapPanel2->Hide();
    nchapPanel1->Show();
    nchapNext->SetId(BUTTON_NextChapter);
   
    event.Skip();
}

void ChapterCreator::cancel(wxCommandEvent& event) {
    mainFrame->Enable();
    this->Destroy();

    event.Skip();
}

void ChapterCreator::create(wxCommandEvent& event) {
    Chapter chapter;

    if (nchapName->GetValue() != "" && nchapName->IsModified()) {
        chapter.name = nchapName->GetValue();
    } else {
        chapter.name = "Chapter " + std::to_string(notebook->current);
    }

    chapter.summary = nchapSummary->GetValue();

    int pos = nchapList->FindItem(-1, tempName);

    chapter.position = pos + 1;

    notebook->addChapter(chapter, pos);

    mainFrame->Enable();
    Destroy();

    event.Skip();
}

void ChapterCreator::checkClose(wxCloseEvent& event) {
    if (nchapName->IsModified() || nchapSummary->IsModified()) {
        wxMessageDialog* check = new wxMessageDialog(this, "Are you sure you want to close?",
            "Close window", wxYES_NO | wxNO_DEFAULT);

        switch (check->ShowModal()) {
        case wxID_YES:
            mainFrame->Enable();
            this->Destroy();
            break;

        case wxID_NO:
            check->Destroy();
            break;
        }
    } else {
        mainFrame->Enable();
        this->Destroy();
    }

    event.Skip();
}
