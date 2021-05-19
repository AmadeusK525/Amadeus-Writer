#include "DocumentCreator.h"

#include "MyApp.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(DocumentCreator, wxFrame)

EVT_BUTTON(BUTTON_NextDocument, DocumentCreator::Next)
EVT_BUTTON(BUTTON_BackDocument, DocumentCreator::Back)
EVT_BUTTON(BUTTON_CancelDocument, DocumentCreator::Cancel)
EVT_BUTTON(BUTTON_CreateDocument, DocumentCreator::Create)

EVT_CLOSE(DocumentCreator::CheckClose)

END_EVENT_TABLE()

DocumentCreator::DocumentCreator(wxWindow* parent, amProjectManager* manager) :
	wxFrame(parent, wxID_ANY, "Create document", wxDefaultPosition, wxSize(500, 350), wxMINIMIZE_BOX | wxSYSTEM_MENU |
	wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_SHAPED | wxFRAME_FLOAT_ON_PARENT),
	m_manager(manager)
{
	this->CenterOnParent();
	this->SetBackgroundColour(wxColour(40, 40, 40));

	m_ndocPanel1 = new wxPanel(this, wxID_ANY);
	m_ndocPanel1->SetBackgroundColour(wxColour(40, 40, 40));
	m_ndocPanel1->Show(true);

	wxStaticText* label1 = new wxStaticText(m_ndocPanel1, wxID_ANY, "Name: ",
		wxPoint(45, 10), wxSize(50, 20), wxTE_READONLY | wxNO_BORDER);
	label1->SetFont(wxFont(wxFontInfo(12)));
	label1->SetForegroundColour(wxColour(245, 245, 245));
	label1->SetBackgroundColour(wxColour(40, 40, 40));
	m_ndocName = new wxTextCtrl(m_ndocPanel1, wxID_ANY, "Document " +
		std::to_string(m_manager->GetDocumentCount(1) + 1), wxPoint(105, 10), wxSize(365, 25), wxBORDER_SIMPLE);
	m_ndocName->SetBackgroundColour(wxColour(70, 70, 70));
	m_ndocName->SetForegroundColour(wxColour(250, 250, 250));
	m_ndocName->SetFont(wxFont(wxFontInfo(10)));

	wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
	firstLine->Add(label1, wxSizerFlags().CenterVertical());
	firstLine->Add(m_ndocName, wxSizerFlags(1).CenterVertical());

	wxStaticText* label2 = new wxStaticText(m_ndocPanel1, wxID_ANY, "Summary",
		wxPoint(45, 45), wxSize(425, 20), wxBORDER_SIMPLE);
	label2->SetBackgroundColour(wxColour(230, 30, 30));
	label2->SetFont(wxFont(wxFontInfo(13).Bold()));
	m_ndocSummary = new wxTextCtrl(m_ndocPanel1, wxID_ANY, "", wxPoint(45, 65),
		wxSize(425, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
	m_ndocSummary->SetBackgroundColour(wxColour(70, 70, 70));
	m_ndocSummary->SetForegroundColour(wxColour(250, 250, 250));

	wxBoxSizer* ver = new wxBoxSizer(wxVERTICAL);
	ver->Add(firstLine, wxSizerFlags(0).Expand());
	ver->AddSpacer(20);
	ver->Add(label2, wxSizerFlags(0).Expand());
	ver->Add(m_ndocSummary, wxSizerFlags(1).Expand());

	m_ndocPanel1->SetSizer(ver);

	m_ndocPanel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500, 350));
	m_ndocPanel2->SetBackgroundColour(wxColour(40, 40, 40));
	m_ndocPanel2->Hide();

	m_ndocList = new amDragList(m_ndocPanel2, wxDefaultSize);
	m_ndocList->SetBackgroundColour(wxColour(70, 70, 70));
	m_ndocList->SetForegroundColour(wxColour(240, 240, 240));

	wxBoxSizer* siz2 = new wxBoxSizer(wxHORIZONTAL);
	siz2->Add(m_ndocList, wxSizerFlags(1).Expand());

	m_ndocPanel2->SetSizer(siz2);

	m_btnPanel = new wxPanel(this);
	m_btnPanel->SetBackgroundColour(wxColour(40, 40, 40));

	m_ndocBack = new wxButton(m_btnPanel, BUTTON_BackDocument, "Back", wxPoint(45, 275), wxSize(90, 30));
	m_ndocNext = new wxButton(m_btnPanel, BUTTON_NextDocument, "Next", wxPoint(285, 275), wxSize(90, 30));
	m_ndocCancel = new wxButton(m_btnPanel, BUTTON_CancelDocument, "Cancel", wxPoint(380, 275), wxSize(90, 30));

	m_ndocBack->Hide();

	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
	btnSizer->Add(m_ndocBack, wxSizerFlags().CenterVertical());
	btnSizer->AddStretchSpacer(1);
	btnSizer->Add(m_ndocNext, wxSizerFlags().CenterVertical());
	btnSizer->AddSpacer(5);
	btnSizer->Add(m_ndocCancel, wxSizerFlags().CenterVertical());

	m_btnPanel->SetSizer(btnSizer);

	wxBoxSizer* mainVer = new wxBoxSizer(wxVERTICAL);
	mainVer->Add(m_ndocPanel1, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 10));
	mainVer->Add(m_ndocPanel2, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 10));
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

	SetIcon(wxICON(documentIcon));
	Layout();
}

void DocumentCreator::Next(wxCommandEvent& event)
{
	m_tempName = m_ndocName->GetValue();

	if ( m_firstNext )
	{
		m_ndocList->AppendColumn("Drag document '" + m_tempName + "' to desired position", wxLIST_FORMAT_LEFT, 2000);

		int i = 0;
		for ( Document*& it : m_manager->GetDocumentsInCurrentBook() )
			m_ndocList->InsertItem(i++, it->name);

		m_ndocList->InsertItem(i, m_tempName);
		m_firstNext = false;
	}
	else
	{
		int item = m_ndocList->FindItem(-1, m_tempName);

		m_ndocList->SetItemText(item, m_tempName);
	}

	m_ndocList->m_tempName = this->m_tempName;
	//m_ndocList->SelectItem(wxMouseEvent());

	m_ndocPanel1->Hide();
	m_ndocPanel2->Show();
	m_ndocNext->SetId(BUTTON_CreateDocument);
	m_ndocNext->SetLabel("Create");
	m_ndocBack->Show();

	m_mainHor->Layout();
	m_ndocList->SetColumnWidth(0, m_ndocList->GetSize().x);
}

void DocumentCreator::Back(wxCommandEvent& event)
{
	m_ndocPanel2->Hide();
	m_ndocPanel1->Show();
	m_ndocNext->SetId(BUTTON_NextDocument);
	m_ndocNext->SetLabel("Next");
	m_ndocBack->Hide();

	event.Skip();
}

void DocumentCreator::Cancel(wxCommandEvent& event)
{
	m_manager->GetMainFrame()->Enable();
	this->Destroy();

	event.Skip();
}

void DocumentCreator::Create(wxCommandEvent& event)
{
	Document* pDocument = new Document(nullptr, -1, m_manager->GetCurrentBook(), Document_Chapter);

	if ( m_ndocName->GetValue() != "" && m_ndocName->IsModified() )
		pDocument->name = m_ndocName->GetValue();
	else
		pDocument->name = "Document " + std::to_string(m_manager->GetDocumentCount(1) + 1);

	pDocument->synopsys = m_ndocSummary->GetValue();

	int pos = m_ndocList->FindItem(-1, m_tempName);

	pDocument->position = pos + 1;

	m_manager->AddDocument(pDocument, m_manager->GetCurrentBook(), pos);
	m_manager->GetMainFrame()->Enable();
	Destroy();

	event.Skip();
}

void DocumentCreator::CheckClose(wxCloseEvent& event)
{
	if ( m_ndocName->IsModified() || m_ndocSummary->IsModified() )
	{
		wxMessageDialog check(this, "Are you sure you want to close?",
			"Close window", wxYES_NO | wxNO_DEFAULT);

		switch ( check.ShowModal() )
		{
		case wxID_YES:
			m_manager->GetMainFrame()->Enable();
			this->Destroy();
			break;

		case wxID_NO:
			check.Destroy();
			break;
		}
	}
	else
	{
		m_manager->GetMainFrame()->Enable();
		this->Destroy();
	}
}
