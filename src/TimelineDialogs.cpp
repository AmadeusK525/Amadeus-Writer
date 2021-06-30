#include "TimelineDialogs.h"

BEGIN_EVENT_TABLE(amTLAddThreadDlg, wxFrame)

EVT_SIZE(amTLAddThreadDlg::OnListResize)
EVT_CLOSE(amTLAddThreadDlg::OnClose)

EVT_BUTTON(BUTTON_AddThreadDone, amTLAddThreadDlg::OnDone)
EVT_UPDATE_UI(BUTTON_AddThreadDone, amTLAddThreadDlg::OnUpdateDone)

END_EVENT_TABLE()

amTLAddThreadDlg::amTLAddThreadDlg(wxWindow* parent, amTLTimeline* timeline,
	amTLThreadDlgMode mode, const wxSize& size) :
	wxFrame(parent, -1, "Add new thread", wxDefaultPosition, size, wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER)
{
	m_timeline = timeline;
	m_mode = mode;

	wxPanel* panel = new wxPanel(this);
	panel->SetBackgroundColour(wxColour(20, 20, 20));

	wxStaticText* listLabel = new wxStaticText(panel, -1, _("Please select a character:"));
	listLabel->SetForegroundColour(wxColour(255, 255, 255));
	listLabel->SetFont(wxFontInfo(13).Bold());

	m_characterList = new wxListView(panel, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE | wxLC_NO_HEADER);
	m_characterList->InsertColumn(0, "Characters", wxLIST_FORMAT_LEFT, 400);
	m_characterList->Bind(wxEVT_SIZE, &amTLAddThreadDlg::OnListResize, this);

	m_characterList->SetBackgroundColour(wxColour(45, 45, 45));
	m_characterList->SetForegroundColour(wxColour(245, 245, 245));

	m_characterImages = new wxImageList(24, 24);
	m_characterList->AssignImageList(m_characterImages, wxIMAGE_LIST_SMALL);

	wxButton* nextBtn = new wxButton(panel, BUTTON_AddThreadDone, _("Done"));

	wxBoxSizer* vertical = new wxBoxSizer(wxVERTICAL);
	vertical->AddSpacer(5);
	vertical->Add(listLabel, wxSizerFlags(0));
	vertical->AddSpacer(5);
	vertical->Add(m_characterList, wxSizerFlags(1).Expand());
	vertical->AddSpacer(10);

	if ( m_mode == MODE_THREAD_Add )
	{
		wxStaticText* colourLabel = new wxStaticText(panel, -1, _("Color:"));
		colourLabel->SetForegroundColour(wxColour(255, 255, 255));
		colourLabel->SetFont(wxFontInfo(13).Bold());

		wxColour defaultColour(std::rand() % 255, std::rand() % 255, std::rand() % 255);
		m_colourPicker = new wxColourPickerCtrl(panel, -1, defaultColour);
		m_colourPicker->GetPickerCtrl()->SetBackgroundColour(defaultColour);
		m_colourPicker->Bind(wxEVT_COLOURPICKER_CHANGED, [&](wxColourPickerEvent& event)
			{
				m_colourPicker->GetPickerCtrl()->SetBackgroundColour(event.GetColour());
			});

		wxBoxSizer* horizontal = new wxBoxSizer(wxHORIZONTAL);
		horizontal->Add(colourLabel, wxSizerFlags(0).CenterVertical());
		horizontal->Add(m_colourPicker, wxSizerFlags(1).Expand().Border(wxALL, 5));

		vertical->Add(horizontal, wxSizerFlags(0).Expand());
		vertical->AddSpacer(5);
	}
	else
		SetTitle("Change character");

	vertical->Add(nextBtn, wxSizerFlags(0).Right().Border(wxALL, 5));
	panel->SetSizer(vertical);

	SetCharacters(am::GetCharacters());
	SetIcon(wxICON(amadeus));
}

void amTLAddThreadDlg::OnDone(wxCommandEvent& event)
{
	if ( m_mode == MODE_THREAD_Add )
		m_timeline->AppendThread(m_characterList->GetItemText(m_characterList->GetFirstSelected()), m_colourPicker->GetColour());
	else
		m_timeline->EditCurrentThread(m_characterList->GetItemText(m_characterList->GetFirstSelected()));

	Close();
}

void amTLAddThreadDlg::OnUpdateDone(wxUpdateUIEvent& event)
{
	event.Enable(m_characterList->GetSelectedItemCount());
}

void amTLAddThreadDlg::SetCharacters(wxVector<am::Character*>& characters)
{
	if ( !m_characterList->IsEmpty() )
		m_characterList->DeleteAllItems();

	int i = 0;
	for ( am::Character*& character : characters )
	{
		if ( m_timeline->IsCharacterPresent(character->name) )
			continue;

		m_characterList->InsertItem(i, character->name);

		if ( character->image.IsOk() )
			m_characterList->SetItemColumnImage(i, 0, m_characterImages->Add(wxBitmap(am::GetScaledImage(24, 24, character->image))));
		else
			m_characterList->SetItemColumnImage(i, 0, -1);

		i++;
	}
}

void amTLAddThreadDlg::OnListResize(wxSizeEvent& event)
{
	m_characterList->SetColumnWidth(0, m_characterList->GetClientSize().x);
	event.Skip();
}

void amTLAddThreadDlg::OnClose(wxCloseEvent& event)
{
	m_parent->Enable(true);
	event.Skip();
}


BEGIN_EVENT_TABLE(amTLCardDlg, wxFrame)

EVT_CLOSE(amTLCardDlg::OnClose)

EVT_BUTTON(BUTTON_CardDone, amTLCardDlg::OnDone)
EVT_UPDATE_UI(BUTTON_CardDone, amTLCardDlg::OnUpdateDone)

END_EVENT_TABLE()

amTLCardDlg::amTLCardDlg(wxWindow* parent, amTLTimeline* timeline, const wxString& currentString,
	amTLCardDlgMode mode, const wxSize& size) :
	wxFrame(parent, -1, "", wxDefaultPosition, size)
{
	m_timeline = timeline;
	m_mode = mode;

	wxPanel* panel = new wxPanel(this);
	panel->SetBackgroundColour(wxColour(30, 30, 30));

	wxStaticText* label = new wxStaticText(panel, -1, "");
	label->SetForegroundColour(wxColour(255, 255, 255));
	label->SetFont(wxFontInfo(13).Bold());

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(label, wxSizerFlags().CenterHorizontal().Border(wxALL, 5));

	if ( mode == MODE_CARD_Title )
	{
		label->SetLabel("Edit title");
		SetTitle("Edit card title");

		m_title = new wxTextCtrl(panel, -1, currentString, wxDefaultPosition, wxDefaultSize,
			wxTE_CENTER | wxBORDER_SIMPLE);
		m_title->SetBackgroundColour(wxColour(20, 20, 20));
		m_title->SetForegroundColour(wxColour(255, 255, 255));

		sizer->Add(m_title, wxSizerFlags(0).Expand().Border(wxALL, 5));
		sizer->AddStretchSpacer(1);
	}
	else
	{
		label->SetLabel("Edit content");
		SetTitle("Edit card content");

		m_content = new wxTextCtrl(panel, -1, currentString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxBORDER_SIMPLE);
		m_content->SetBackgroundColour(wxColour(20, 20, 20));
		m_content->SetForegroundColour(wxColour(255, 255, 255));

		sizer->Add(m_content, wxSizerFlags(1).Expand().Border(wxALL, 5));
	}

	wxButton* doneBtn = new wxButton(panel, BUTTON_CardDone, "Done");
	sizer->Add(doneBtn, wxSizerFlags(0).Right().Border(wxALL, 5));

	panel->SetSizer(sizer);
	SetIcon(wxICON(amadeus));
}

void amTLCardDlg::OnDone(wxCommandEvent& event)
{
	if ( m_mode == MODE_CARD_Title )
		m_timeline->EditCurrentCardTitle(m_title->GetValue());
	else
		m_timeline->EditCurrentCardContent(m_content->GetValue());

	m_parent->Enable(true);
	Close();
}

void amTLCardDlg::OnUpdateDone(wxUpdateUIEvent& event)
{
	if ( m_mode == MODE_CARD_Title )
		event.Enable(!m_title->IsEmpty());
	else
		event.Enable(!m_content->IsEmpty());
}

void amTLCardDlg::OnClose(wxCloseEvent& event)
{
	m_parent->Enable(true);
	event.Skip();
}


BEGIN_EVENT_TABLE(amTLAddSectionDlg, wxFrame)

EVT_BUTTON(BUTTON_AddSectionDone, amTLAddSectionDlg::OnDone)
EVT_UPDATE_UI(BUTTON_AddSectionDone, amTLAddSectionDlg::OnUpdateDone)

EVT_CLOSE(amTLAddSectionDlg::OnClose)

END_EVENT_TABLE()

amTLAddSectionDlg::amTLAddSectionDlg(wxWindow* parent, amTLTimeline* timeline,
	amTLSectionDlgMode mode, const wxSize& size) :
	wxFrame(parent, -1, "Add new section", wxDefaultPosition, size, wxFRAME_FLOAT_ON_PARENT | wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER),
	m_timeline(timeline), m_mode(mode)
{

	wxPanel* panel = new wxPanel(this);
	panel->SetBackgroundColour(wxColour(20, 20, 20));

	wxStaticText* titleLabel = new wxStaticText(panel, -1, _("Title"));
	titleLabel->SetForegroundColour(wxColour(255, 255, 255));
	titleLabel->SetFont(wxFontInfo(13).Bold());

	m_titleTc = new wxTextCtrl(panel, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxBORDER_SIMPLE);
	m_titleTc->SetBackgroundColour(wxColour(45, 45, 45));
	m_titleTc->SetForegroundColour(wxColour(255, 255, 255));

	wxButton* doneBtn = new wxButton(panel, BUTTON_AddSectionDone, _("Done"));

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	if ( m_mode == MODE_SECTION_Add )
	{
		wxStaticText* mainLabel = new wxStaticText(panel, -1, _("Add new Section:"));
		mainLabel->SetForegroundColour(wxColour(255, 255, 255));
		mainLabel->SetFont(wxFontInfo(18).Bold());

		sizer->Add(mainLabel, wxSizerFlags(0).Left().Border(wxALL, 5));
		sizer->AddSpacer(10);
	}

	sizer->Add(titleLabel, wxSizerFlags(0).CenterHorizontal().Border(wxALL, 5));
	sizer->Add(m_titleTc, wxSizerFlags(0).Expand());

	if ( m_mode == MODE_SECTION_Add )
	{
		wxStaticText* colourLabel = new wxStaticText(panel, -1, _("Colour"));
		colourLabel->SetForegroundColour(wxColour(255, 255, 255));
		colourLabel->SetFont(wxFontInfo(13).Bold());

		m_colourPicker = new wxColourPickerCtrl(panel, -1, wxColour(255, 255, 255));
		m_colourPicker->GetPickerCtrl()->SetBackgroundColour(wxColour(255, 255, 255));
		m_colourPicker->Bind(wxEVT_COLOURPICKER_CHANGED, [&](wxColourPickerEvent& event)
			{
				m_colourPicker->GetPickerCtrl()->SetBackgroundColour(event.GetColour());
			}
		);

		sizer->AddSpacer(15);
		sizer->Add(colourLabel, wxSizerFlags(0).CenterHorizontal().Border(wxALL, 5));
		sizer->Add(m_colourPicker, wxSizerFlags(0).Expand());
	}
	else
	{
		SetTitle(_("Change Section title"));
	}

	sizer->AddStretchSpacer(1);
	sizer->Add(doneBtn, wxSizerFlags(0).Right().Border(wxALL, 5));

	panel->SetSizer(sizer);
	SetIcon(wxICON(amadeus));
}

void amTLAddSectionDlg::OnDone(wxCommandEvent& event)
{
	if ( m_mode == MODE_SECTION_Add )
		m_timeline->AppendSection(m_titleTc->GetValue(), m_colourPicker->GetColour());
	else
		m_timeline->EditCurrentSection(m_titleTc->GetValue());

	Close();
}

void amTLAddSectionDlg::OnUpdateDone(wxUpdateUIEvent& event)
{
	event.Enable(!m_titleTc->IsEmpty());
}

void amTLAddSectionDlg::OnClose(wxCloseEvent& event)
{
	m_parent->Enable(true);
	event.Skip();
}