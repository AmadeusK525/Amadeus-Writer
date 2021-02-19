#include "ChapterCreator.h"


#include "MyApp.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(ChapterCreator, wxFrame)

EVT_BUTTON(BUTTON_NextChapter, ChapterCreator::Next)
EVT_BUTTON(BUTTON_BackChapter, ChapterCreator::Back)
EVT_BUTTON(BUTTON_CancelChapter, ChapterCreator::Cancel)
EVT_BUTTON(BUTTON_CreateChapter, ChapterCreator::Create)

EVT_CLOSE(ChapterCreator::CheckClose)

END_EVENT_TABLE()

ChapterCreator::ChapterCreator(wxWindow* parent, amProjectManager* manager) :
    wxFrame(parent, wxID_ANY, "Create chapter", wxDefaultPosition, wxSize(500, 350), wxMINIMIZE_BOX | wxSYSTEM_MENU |
    wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_SHAPED | wxFRAME_FLOAT_ON_PARENT),
    m_manager(manager) {

    this->CenterOnParent();
    this->SetBackgroundColour(wxColour(40, 40, 40));

    m_nchapPanel1 = new wxPanel(this, wxID_ANY);
    m_nchapPanel1->SetBackgroundColour(wxColour(40, 40, 40));
    m_nchapPanel1->Show(true);

    wxStaticText* label1 = new wxStaticText(m_nchapPanel1, wxID_ANY, "Name: ",
        wxPoint(45, 10), wxSize(50, 20), wxTE_READONLY | wxNO_BORDER);
    label1->SetFont(wxFont(wxFontInfo(12)));
    label1->SetForegroundColour(wxColour(245, 245, 245));
    label1->SetBackgroundColour(wxColour(40, 40, 40));
    m_nchapName = new wxTextCtrl(m_nchapPanel1, wxID_ANY, "Chapter " +
        std::to_string(m_manager->GetChapterCount(1) + 1), wxPoint(105, 10), wxSize(365, 25), wxBORDER_SIMPLE);
    m_nchapName->SetBackgroundColour(wxColour(70, 70, 70));
    m_nchapName->SetForegroundColour(wxColour(250, 250, 250));
    m_nchapName->SetFont(wxFont(wxFontInfo(10)));
    
    wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
    firstLine->Add(label1, wxSizerFlags().CenterVertical());
    firstLine->Add(m_nchapName, wxSizerFlags(1).CenterVertical());

    wxStaticText* label2 = new wxStaticText(m_nchapPanel1, wxID_ANY, "Summary",
        wxPoint(45, 45), wxSize(425, 20), wxBORDER_SIMPLE);
    label2->SetBackgroundColour(wxColour(230, 30, 30));
    label2->SetFont(wxFont(wxFontInfo(13).Bold()));
    m_nchapSummary = new wxTextCtrl(m_nchapPanel1, wxID_ANY, "", wxPoint(45, 65),
        wxSize(425, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    m_nchapSummary->SetBackgroundColour(wxColour(70, 70, 70));
    m_nchapSummary->SetForegroundColour(wxColour(250, 250, 250));

    wxBoxSizer* ver = new wxBoxSizer(wxVERTICAL);
    ver->Add(firstLine, wxSizerFlags(0).Expand());
    ver->AddSpacer(20);
    ver->Add(label2, wxSizerFlags(0).Expand());
    ver->Add(m_nchapSummary, wxSizerFlags(1).Expand());

    m_nchapPanel1->SetSizer(ver);

    m_nchapPanel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500, 350));
    m_nchapPanel2->SetBackgroundColour(wxColour(40, 40, 40));
    m_nchapPanel2->Hide();

    m_nchapList = new amDragList(m_nchapPanel2, wxDefaultSize);
    m_nchapList->SetBackgroundColour(wxColour(70, 70, 70));
    m_nchapList->SetForegroundColour(wxColour(240, 240, 240));

    wxBoxSizer* siz2 = new wxBoxSizer(wxHORIZONTAL);
    siz2->Add(m_nchapList, wxSizerFlags(1).Expand());

    m_nchapPanel2->SetSizer(siz2);

    m_btnPanel = new wxPanel(this);
    m_btnPanel->SetBackgroundColour(wxColour(40, 40, 40));

    m_nchapBack = new wxButton(m_btnPanel, BUTTON_BackChapter, "Back", wxPoint(45, 275), wxSize(90, 30));
    m_nchapNext = new wxButton(m_btnPanel, BUTTON_NextChapter, "Next", wxPoint(285, 275), wxSize(90, 30));
    m_nchapCancel = new wxButton(m_btnPanel, BUTTON_CancelChapter, "Cancel", wxPoint(380, 275), wxSize(90, 30));

    m_nchapBack->Hide();

    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(m_nchapBack, wxSizerFlags().CenterVertical());
    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(m_nchapNext, wxSizerFlags().CenterVertical());
    btnSizer->AddSpacer(5);
    btnSizer->Add(m_nchapCancel, wxSizerFlags().CenterVertical());

    m_btnPanel->SetSizer(btnSizer);

    wxBoxSizer* mainVer = new wxBoxSizer(wxVERTICAL);
    mainVer->Add(m_nchapPanel1, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 10));
    mainVer->Add(m_nchapPanel2, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 10));
    mainVer->AddSpacer(5);
    mainVer->Add(m_btnPanel, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 10));
    mainVer->AddSpacer(5);

    wxStaticText* mainLabel = new wxStaticText(this, wxID_ANY, "C\nH\nA\nP\nT\nE\nR",
        wxPoint(10, 55), wxSize(25, 200), wxBORDER_NONE);
    mainLabel->SetBackgroundColour(wxColour(40, 40, 40));
    mainLabel->SetForegroundColour(wxColour(210, 210, 210));
    mainLabel->SetFont(wxFont(wxFontInfo(16).Bold()));

    m_mainHor = new wxBoxSizer(wxHORIZONTAL);
    m_mainHor->AddSpacer(10);
    m_mainHor->Add(mainLabel, wxSizerFlags(0).CenterVertical());
    m_mainHor->AddSpacer(10);
    m_mainHor->Add(mainVer, wxSizerFlags(1).Expand());

    SetSizer(m_mainHor);

    SetIcon(wxICON(chapterIcon));
    Layout();
}

void ChapterCreator::Next(wxCommandEvent& event) {
    m_tempName = m_nchapName->GetValue();

    if (m_firstNext) {
        m_nchapList->AppendColumn("Drag chapter '" + m_tempName + "' to desired position", wxLIST_FORMAT_LEFT, 2000);

        int i = 0;
        for (auto& it : m_manager->GetChapters(1, 1)) {
            m_nchapList->InsertItem(i++, it.name);
        }

        m_nchapList->InsertItem(i, m_tempName);
        m_firstNext = false;
    } else {
        int item = m_nchapList->FindItem(-1, m_tempName);

        m_nchapList->SetItemText(item, m_tempName);
    }

    m_nchapList->m_tempName = this->m_tempName;
    //m_nchapList->SelectItem(wxMouseEvent());

    m_nchapPanel1->Hide();
    m_nchapPanel2->Show();
    m_nchapNext->SetId(BUTTON_CreateChapter);
    m_nchapNext->SetLabel("Create");
    m_nchapBack->Show();

    m_mainHor->Layout();
    m_nchapList->SetColumnWidth(0, m_nchapList->GetSize().x);
}

void ChapterCreator::Back(wxCommandEvent& event) {
    m_nchapPanel2->Hide();
    m_nchapPanel1->Show();
    m_nchapNext->SetId(BUTTON_NextChapter);
    m_nchapNext->SetLabel("Next");
    m_nchapBack->Hide();
   
    event.Skip();
}

void ChapterCreator::Cancel(wxCommandEvent& event) {
    m_manager->GetMainFrame()->Enable();
    this->Destroy();

    event.Skip();
}

void ChapterCreator::Create(wxCommandEvent& event) {
    Chapter chapter;

    if (m_nchapName->GetValue() != "" && m_nchapName->IsModified()) {
        chapter.name = m_nchapName->GetValue();
    } else {
        chapter.name = "Chapter " + std::to_string(m_manager->GetChapterCount(1) + 1);
    }

    chapter.synopsys = m_nchapSummary->GetValue();

    int pos = m_nchapList->FindItem(-1, m_tempName);

    chapter.position = pos + 1;

    m_manager->AddChapter(chapter, m_manager->GetBooks()[0], 1, pos);
    m_manager->GetMainFrame()->Enable();
    Destroy();

    event.Skip();
}

void ChapterCreator::CheckClose(wxCloseEvent& event) {
    if (m_nchapName->IsModified() || m_nchapSummary->IsModified()) {
        wxMessageDialog check(this, "Are you sure you want to close?",
            "Close window", wxYES_NO | wxNO_DEFAULT);

        switch (check.ShowModal()) {
        case wxID_YES:
            m_manager->GetMainFrame()->Enable();
            this->Destroy();
            break;

        case wxID_NO:
            check.Destroy();
            break;
        }
    } else {
        m_manager->GetMainFrame()->Enable();
        this->Destroy();
    }
}
