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

    this->m_mainFrame = dynamic_cast<MainFrame*>(parent);
    this->m_notebook = notebook;
    this->CenterOnParent();
    this->SetBackgroundColour("WHITE");

    m_nchapPanel1 = new wxPanel(this, wxID_ANY);
    m_nchapPanel1->SetBackgroundColour("WHITE");
    m_nchapPanel1->Show(true);

    //wxButton* nchapCreate = new wxButton(this, BUTTON_NewChapter, "Create");

    wxStaticText* label1 = new wxStaticText(m_nchapPanel1, wxID_ANY, "Name: ", wxPoint(45, 10), wxSize(50, 20), wxTE_READONLY | wxNO_BORDER);
    label1->SetFont(wxFont(wxFontInfo(12)));
    label1->SetBackgroundColour("WHITE");
    m_nchapName = new wxTextCtrl(m_nchapPanel1, wxID_ANY, "Chapter " + std::to_string(notebook->current), wxPoint(105, 10), wxSize(365, 25));
    m_nchapName->SetBackgroundColour(wxColour(200, 200, 200));
    m_nchapName->SetFont(wxFont(wxFontInfo(10)));

    wxStaticText* label2 = new wxStaticText(m_nchapPanel1, wxID_ANY, "Summary", wxPoint(45, 45), wxSize(425, 20));
    label2->SetBackgroundColour(wxColour(230, 30, 30));
    label2->SetFont(wxFont(wxFontInfo(13).Bold()));
    m_nchapSummary = new wxTextCtrl(m_nchapPanel1, wxID_ANY, "", wxPoint(45, 65), wxSize(425, 205)), wxTE_MULTILINE;

    m_mainLabel = new wxStaticText(m_nchapPanel1, wxID_ANY, "C\nH\nA\nP\nT\nE\nR", wxPoint(10, 55), wxSize(25, 200), wxTE_MULTILINE | wxTE_READONLY | wxTE_NO_VSCROLL | wxBORDER_NONE);
    m_mainLabel->SetForegroundColour(wxColour(100, 100, 100));
    m_mainLabel->SetFont(wxFont(wxFontInfo(16).Bold()));

    m_nchapNext = new wxButton(m_nchapPanel1, BUTTON_NextChapter, "Next", wxPoint(285, 275), wxSize(90, 30));
    m_nchapCancel = new wxButton(m_nchapPanel1, BUTTON_CancelChapter, "Cancel", wxPoint(380, 275), wxSize(90, 30));

    SetIcon(wxICON(chapterIcon));
    Layout();

    m_nchapPanel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500, 350));
    m_nchapPanel2->SetBackgroundColour("WHITE");
    m_nchapPanel2->Hide();

    m_nchapBack = new wxButton(m_nchapPanel2, BUTTON_BackChapter, "Back", wxPoint(45, 275), wxSize(90, 30));

    m_nchapList = new DragList(m_nchapPanel2, wxSize(425, 260));
    m_nchapList->SetPosition(wxPoint(45, 10));
    m_nchapList->SetBackgroundColour(wxColour(245, 245, 245));
}

void ChapterCreator::next(wxCommandEvent& event) {

    if (m_firstNext) {
        m_tempName = m_nchapName->GetValue();

        m_nchapList->AppendColumn("Drag chapter '" + m_tempName + "' to desired position", wxLIST_FORMAT_LEFT, 2000);
        m_nchapList->SetColumnWidth(0, m_nchapList->GetSize().x);

        int i = 0;
        for (auto it = m_notebook->chapters.begin(); it != m_notebook->chapters.end(); it++) {
            m_nchapList->InsertItem(i, it->name);
            i++;
        }

        m_nchapList->InsertItem(m_nchapList->GetItemCount(), m_tempName);
        m_nchapList->SetItemBackgroundColour(m_nchapList->FindItem(-1, m_tempName), wxColour(255, 230, 230));
        m_firstNext = false;
    } else {
        int item = m_nchapList->FindItem(-1, m_tempName);

        m_tempName = m_nchapName->GetValue();
        m_nchapList->SetItemText(item, m_tempName);
    }

    m_nchapList->tempName = this->m_tempName;
    m_nchapList->EnsureVisible(m_nchapList->FindItem(-1, m_tempName));
    m_nchapList->Select(m_nchapList->FindItem(-1, m_tempName));

    m_mainLabel->Reparent(m_nchapPanel2);
    m_nchapNext->Reparent(m_nchapPanel2);
    m_nchapNext->SetLabel("Create");
    m_nchapCancel->Reparent(m_nchapPanel2);

    m_nchapPanel1->Hide();
    m_nchapPanel2->Show();
    m_nchapNext->SetId(BUTTON_CreateChapter);

    event.Skip();
}

void ChapterCreator::back(wxCommandEvent& event) {
    m_mainLabel->Reparent(m_nchapPanel1);
    m_nchapNext->Reparent(m_nchapPanel1);
    m_nchapNext->SetLabel("Next");
    m_nchapCancel->Reparent(m_nchapPanel1);
    m_nchapPanel2->Hide();
    m_nchapPanel1->Show();
    m_nchapNext->SetId(BUTTON_NextChapter);
   
    event.Skip();
}

void ChapterCreator::cancel(wxCommandEvent& event) {
    m_mainFrame->Enable();
    this->Destroy();

    event.Skip();
}

void ChapterCreator::create(wxCommandEvent& event) {
    Chapter chapter;

    if (m_nchapName->GetValue() != "" && m_nchapName->IsModified()) {
        chapter.name = m_nchapName->GetValue();
    } else {
        chapter.name = "Chapter " + std::to_string(m_notebook->current);
    }

    chapter.summary = m_nchapSummary->GetValue();

    int pos = m_nchapList->FindItem(-1, m_tempName);

    chapter.position = pos + 1;

    m_notebook->addChapter(chapter, pos);

    m_mainFrame->Enable();
    Destroy();

    event.Skip();
}

void ChapterCreator::checkClose(wxCloseEvent& event) {
    if (m_nchapName->IsModified() || m_nchapSummary->IsModified()) {
        wxMessageDialog* check = new wxMessageDialog(this, "Are you sure you want to close?",
            "Close window", wxYES_NO | wxNO_DEFAULT);

        switch (check->ShowModal()) {
        case wxID_YES:
            m_mainFrame->Enable();
            this->Destroy();
            break;

        case wxID_NO:
            check->Destroy();
            break;
        }
    } else {
        m_mainFrame->Enable();
        this->Destroy();
    }

    event.Skip();
}
