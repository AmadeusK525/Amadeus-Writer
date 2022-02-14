#include "Views/Elements/ElementCreators.h"

#include "Views/MainFrame.h"
#include "Views/Elements/ElementsNotebook.h"
#include "Views/Outline/Outline.h"
#include "Views/Outline/OutlineFiles.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>

#include "Utils/wxmemdbg.h"

wxIMPLEMENT_ABSTRACT_CLASS(amElementCreator, wxFrame);

BEGIN_EVENT_TABLE(amElementCreator, wxFrame)

EVT_BUTTON(BUTTON_SetImage, amElementCreator::SetImage)
EVT_BUTTON(BUTTON_RemoveImage, amElementCreator::RemoveImage)

END_EVENT_TABLE()

amElementCreator::amElementCreator(wxWindow* parent,
	long id, const wxString& label, const wxPoint& pos, const wxSize& size, long style)
{
	Create(parent, id, label, pos, size, style);
}

bool amElementCreator::Create(wxWindow* parent, long id,
	const wxString& label, const wxPoint& pos, const wxSize& size, long style)
{
	if ( !wxFrame::Create(parent, id, label, pos, size, style) )
		return false;

	CenterOnParent();

	Bind(wxEVT_CLOSE_WINDOW, &amElementCreator::OnClose, this);

	wxColour dark(50, 50, 50);
	wxColour darker(30, 30, 30);
	wxColour txtCol(255, 255, 255);

	SetBackgroundColour(darker);

	m_panel1 = new wxPanel(this, wxID_ANY);
	m_panel1->SetBackgroundColour(darker);
	m_panel1->Show(true);

	m_panel2 = new wxScrolledWindow(this);
	m_panel2->Hide();
	m_panel2->SetBackgroundColour(darker);

	wxStaticText* customLabel = new wxStaticText(m_panel2, -1, "Custom attributes");
	customLabel->SetFont(wxFontInfo(14).Bold());
	customLabel->SetForegroundColour(wxColour(240, 240, 240));

	m_addCustom = new wxButton(m_panel2, -1, "", wxDefaultPosition, wxSize(25, 25));
	m_addCustom->SetBitmap(wxBITMAP_PNG(plus));
	m_addCustom->Bind(wxEVT_BUTTON, &amElementCreator::AddCustomAttr, this);

	wxBoxSizer* customHorSiz = new wxBoxSizer(wxHORIZONTAL);
	customHorSiz->Add(customLabel, wxSizerFlags(1));
	customHorSiz->AddStretchSpacer(1);
	customHorSiz->Add(m_addCustom, wxSizerFlags(0));

	m_customSizer = new wxBoxSizer(wxVERTICAL);
	m_customSizer->Add(customHorSiz, wxSizerFlags(0).Expand().Border(wxALL, 10));

	m_panel2->SetSizer(m_customSizer);
	m_panel2->FitInside();
	m_panel2->SetScrollRate(20, 20);

	m_panel3 = new wxPanel(this);
	m_panel3->Hide();
	m_panel3->SetBackgroundColour(darker);

	m_chooseImage = new wxButton(m_panel3, BUTTON_SetImage, "Choose image file", wxPoint(285, 415), wxSize(150, 30));
	m_removeImage = new wxButton(m_panel3, BUTTON_RemoveImage, "Remove", wxPoint(75, 415), wxSize(100, 30));
	m_removeImage->Hide();
	m_imagePanel = new ImagePanel(m_panel3, wxPoint(75, 40), wxSize(360, 360));
	m_imagePanel->SetBorderColour(darker);

	wxBoxSizer* btnSizer2 = new wxBoxSizer(wxHORIZONTAL);
	btnSizer2->Add(m_removeImage, wxSizerFlags(0));
	btnSizer2->AddStretchSpacer(1);
	btnSizer2->Add(m_chooseImage, wxSizerFlags(0));

	wxBoxSizer* verSizer3 = new wxBoxSizer(wxVERTICAL);
	verSizer3->Add(m_imagePanel, wxSizerFlags(0).Border(wxTOP, 40).CenterHorizontal());
	verSizer3->AddStretchSpacer(1);
	verSizer3->Add(btnSizer2, wxSizerFlags(0).Expand());
	verSizer3->AddStretchSpacer(3);

	m_panel3->SetSizer(verSizer3);

	m_btnPanel = new wxPanel(this);
	m_btnPanel->SetBackgroundColour(darker);

	m_next = new wxButton(m_btnPanel, BUTTON_Next1, "Next", wxPoint(300, 505), wxSize(90, 30));
	m_back = new wxButton(m_btnPanel, BUTTON_Back1, "Back", wxPoint(15, 505), wxSize(90, 30));
	m_cancel = new wxButton(m_btnPanel, BUTTON_Cancel, "Cancel", wxPoint(400, 505), wxSize(90, 30));

	m_next->Bind(wxEVT_BUTTON, &amElementCreator::Next, this);
	m_back->Bind(wxEVT_BUTTON, &amElementCreator::Back, this);
	m_cancel->Bind(wxEVT_BUTTON, &amElementCreator::Cancel, this);

	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
	btnSizer->Add(m_back, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 10));
	btnSizer->AddStretchSpacer(1);
	btnSizer->Add(m_next, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 10));
	btnSizer->AddSpacer(10);
	btnSizer->Add(m_cancel, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 10));

	m_back->Hide();
	m_btnPanel->SetSizer(btnSizer);

	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	m_mainSizer->Add(m_panel1, wxSizerFlags(1).Expand());
	m_mainSizer->Add(m_panel2, wxSizerFlags(1).Expand());
	m_mainSizer->Add(m_panel3, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 75));
	m_mainSizer->Add(m_btnPanel, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));

	SetSizer(m_mainSizer);
	am::GetMainFrame()->Enable(false);
	return true;
}

void amElementCreator::RemoveCustomAttr(wxCommandEvent& event)
{
	Freeze();
	wxButton* minus = (wxButton*)event.GetEventObject();

	auto it1 = m_custom.begin();
	auto it2 = m_minusButtons.begin();

	for ( it2 = m_minusButtons.begin(); it2 != m_minusButtons.end(); it2++ )
	{
		if ( *it2 == minus )
			break;

		it1++;
	}

	if ( minus->GetParent() == m_panel2 )
	{
		it1->first->Destroy();
		it1->second->Destroy();
		minus->Destroy();
	}
	else
	{
		minus->GetParent()->Destroy();
	}

	m_mLongAttr.erase(it1->first);
	m_custom.erase(it1);
	m_minusButtons.erase(it2);

	m_mainSizer->Layout();

	Thaw();
}

void amElementCreator::Cancel(wxCommandEvent& event)
{
	am::GetMainFrame()->Enable();
	this->Destroy();
	event.Skip();
}

void amElementCreator::Next(wxCommandEvent& event)
{
	bool show1 = false, show2 = false, show3 = false;

	switch ( event.GetId() )
	{
	case BUTTON_Next1:
		m_next->SetId(BUTTON_Next2);
		m_back->Show();
		show2 = true;
		break;

	case BUTTON_Next2:
		m_next->SetId(BUTTON_Create);
		m_next->SetLabel("Create");
		m_back->SetId(BUTTON_Back2);
		show3 = true;
		break;

	case BUTTON_NextEdit1:
		m_next->SetId(BUTTON_NextEdit2);
		m_back->Show();
		show2 = true;
		break;

	case BUTTON_NextEdit2:
		m_next->SetId(BUTTON_CreateEdit);
		m_next->SetLabel("Ok");
		m_back->SetId(BUTTON_BackEdit2);
		show3 = true;
		break;

	case BUTTON_Create:
		OnCreateElement(event);
		return;

	case BUTTON_CreateEdit:
		DoEdit(event);
		return;
	}

	m_panel1->Show(show1);
	m_panel2->Show(show2);
	m_panel3->Show(show3);

	m_mainSizer->Layout();
	event.Skip();
}

void amElementCreator::Back(wxCommandEvent& event)
{
	bool show1 = false, show2 = false, show3 = false;

	switch ( event.GetId() )
	{
	case BUTTON_Back1:
		m_next->SetId(BUTTON_Next1);
		m_back->Hide();
		show1 = true;
		break;

	case BUTTON_Back2:
		m_next->SetLabel("Next");
		m_next->SetId(BUTTON_Next2);
		m_back->SetId(BUTTON_Back1);
		show2 = true;
		break;

	case BUTTON_BackEdit1:
		m_next->SetId(BUTTON_NextEdit1);
		m_back->Hide();
		show1 = true;
		break;

	case BUTTON_BackEdit2:
		m_next->SetId(BUTTON_NextEdit2);
		m_next->SetLabel("Next");
		m_back->SetId(BUTTON_BackEdit1);
		show2 = true;
		break;
	}

	m_panel1->Show(show1);
	m_panel2->Show(show2);
	m_panel3->Show(show3);

	m_mainSizer->Layout();
	event.Skip();
}

void amElementCreator::SetImage(wxCommandEvent& event)
{
	wxFileDialog choose(this, "Select an image", wxEmptyString, wxEmptyString,
		"JPG, JPEG and PNG(*.jpg;*.jpeg;*.png)|*.jpg;*.jpeg;*.png",
		wxFD_OPEN, wxDefaultPosition);

	if ( choose.ShowModal() == wxID_OK )
	{
		wxImage image;
		image.LoadFile(choose.GetPath(), wxBITMAP_TYPE_ANY);
		m_imagePanel->SetImage(image);
		m_removeImage->Show();
	}

	m_mainSizer->Layout();
	event.Skip();
}

void amElementCreator::RemoveImage(wxCommandEvent& event)
{
	m_imagePanel->SetImage(wxImage());
	m_removeImage->Hide();
	m_mainSizer->Layout();

	event.Skip();
}

wxVector<std::pair<wxString, wxString>> amElementCreator::GetShortAttributes()
{
	wxVector<std::pair<wxString, wxString>> vShortAttr;

	for ( auto& it : m_mShortAttr )
	{
		if ( it.second->GetValue().IsEmpty() )
			continue;

		vShortAttr.push_back(std::make_pair(it.first->GetLabel().BeforeLast(':'), it.second->GetValue()));
	}

	return vShortAttr;
}

wxVector<std::pair<wxString, wxString>> amElementCreator::GetLongAttributes()
{
	wxVector<std::pair<wxString, wxString>> vLongAttr;

	for ( auto& it : m_mPreDefLongAttr )
	{
		if ( it.second->GetValue().IsEmpty() )
			continue;

		vLongAttr.push_back(std::make_pair(it.first->GetLabel(), it.second->GetValue()));
	}

	for ( auto& it : m_mLongAttr )
	{
		if ( it.second->GetValue().IsEmpty() )
			continue;

		vLongAttr.push_back(std::make_pair(it.first->GetValue(), it.second->GetValue()));
	}

	return vLongAttr;
}

void amElementCreator::OnClose(wxCloseEvent& event)
{
	am::GetMainFrame()->Enable();
	event.Skip();
}


///////////////////////////////////////////////////////////////////////////////
////////////////////////////// am::Character creator //////////////////////////////
///////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(amCharacterCreator, amElementCreator);

amCharacterCreator::amCharacterCreator(wxWindow* parent, long id, const wxString& label,
	const wxPoint& pos, const wxSize& size, long style)
{
	Create(parent, id, label, pos, size, style);
}

bool amCharacterCreator::Create(wxWindow* parent,
	long id,
	const wxString& label,
	const wxPoint& pos,
	const wxSize& size,
	long style)
{
	if ( !amElementCreator::Create(parent, id, label, pos, size, style) )
		return false;

	Bind(wxEVT_CLOSE_WINDOW, &amCharacterCreator::OnClose, this);

	wxColour dark(50, 50, 50);
	wxColour darker(30, 30, 30);
	wxColour txtCol(255, 255, 255);

	wxFont font10(wxFontInfo(10));

	ncFullName = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE);
	ncFullName->SetBackgroundColour(dark);
	ncFullName->SetForegroundColour(wxColour(250, 250, 250));
	ncFullName->SetFont(font10);
	wxStaticText* label1 = new wxStaticText(m_panel1, wxID_ANY, "Name:", wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxALIGN_LEFT);
	label1->SetFont(wxFont(wxFontInfo(12)));
	label1->SetBackgroundColour(darker);
	label1->SetForegroundColour(txtCol);

	wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
	firstLine->Add(label1, wxSizerFlags(0).Border(wxRIGHT, 10));
	firstLine->Add(ncFullName, wxSizerFlags(1));

	ncSex = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE);
	ncSex->SetBackgroundColour(dark);
	ncSex->SetForegroundColour(wxColour(250, 250, 250));
	ncSex->SetFont(font10);
	ncSex->SetMinSize(FromDIP(wxSize(55, -1)));
	wxStaticText* sexLabel = new wxStaticText(m_panel1, wxID_ANY, "Sex:", wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxALIGN_LEFT);
	sexLabel->SetBackgroundColour(darker);
	sexLabel->SetForegroundColour(txtCol);
	sexLabel->SetFont(wxFont(wxFontInfo(13)));
	
	m_mShortAttr[sexLabel] = ncSex;

	ncHeight = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE);
	ncHeight->SetBackgroundColour(dark);
	ncHeight->SetForegroundColour(wxColour(250, 250, 250));
	ncHeight->SetFont(font10);
	ncHeight->SetMinSize(FromDIP(wxSize(45, -1)));
	wxStaticText* heightLabel = new wxStaticText(m_panel1, wxID_ANY, "Height: ", wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxALIGN_LEFT);
	heightLabel->SetBackgroundColour(darker);
	heightLabel->SetForegroundColour(txtCol);
	heightLabel->SetFont(wxFont(wxFontInfo(12)));

	m_mShortAttr[heightLabel] = ncHeight;

	ncNationality = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE);
	ncNationality->SetBackgroundColour(dark);
	ncNationality->SetForegroundColour(wxColour(250, 250, 250));
	ncNationality->SetFont(font10);
	wxStaticText* nationalityLabel = new wxStaticText(m_panel1, wxID_ANY, "Nationality: ", wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxALIGN_LEFT);
	nationalityLabel->SetBackgroundColour(darker);
	nationalityLabel->SetForegroundColour(txtCol);
	nationalityLabel->SetFont(wxFont(wxFontInfo(12)));

	m_mShortAttr[nationalityLabel] = ncNationality;

	wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
	secondLine->Add(sexLabel, wxSizerFlags(0).CenterVertical());
	secondLine->AddSpacer(6);
	secondLine->Add(ncSex, wxSizerFlags(1).CenterVertical());
	secondLine->AddSpacer(10);
	secondLine->Add(heightLabel, wxSizerFlags(0).CenterVertical());
	secondLine->AddSpacer(6);
	secondLine->Add(ncHeight, wxSizerFlags(1).CenterVertical());
	secondLine->AddSpacer(10);
	secondLine->Add(nationalityLabel, wxSizerFlags(0).CenterVertical());
	secondLine->AddSpacer(6);
	secondLine->Add(ncNationality, wxSizerFlags(3).CenterVertical());

	ncAge = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE);
	ncAge->SetBackgroundColour(dark);
	ncAge->SetForegroundColour(wxColour(250, 250, 250));
	ncAge->SetFont(font10);
	ncAge->SetMinSize(FromDIP(wxSize(45, -1)));
	wxStaticText* ageLabel = new wxStaticText(m_panel1, wxID_ANY, "Age:", wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxALIGN_LEFT);
	ageLabel->SetBackgroundColour(darker);
	ageLabel->SetForegroundColour(txtCol);
	ageLabel->SetFont(wxFont(wxFontInfo(12)));

	m_mShortAttr[ageLabel] = ncAge;

	ncNickname = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE);
	ncNickname->SetBackgroundColour(dark);
	ncNickname->SetForegroundColour(wxColour(250, 250, 250));
	ncNickname->SetFont(font10);
	wxStaticText* nicknameLabel = new wxStaticText(m_panel1, wxID_ANY, "Nickname: ", wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxALIGN_LEFT);
	nicknameLabel->SetBackgroundColour(darker);
	nicknameLabel->SetForegroundColour(txtCol);
	nicknameLabel->SetFont(wxFont(wxFontInfo(12)));

	m_mShortAttr[nicknameLabel] = ncNickname;

	ncMain = new wxRadioButton(m_panel1, wxID_ANY, "Protagonist", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	ncMain->SetFont(font10);
	ncMain->SetForegroundColour(wxColour(245, 245, 245));
	ncSupp = new wxRadioButton(m_panel1, wxID_ANY, "Supporting");
	ncSupp->SetFont(font10);
	ncSupp->SetForegroundColour(wxColour(245, 245, 245));
	ncVillian = new wxRadioButton(m_panel1, wxID_ANY, "Villian");
	ncVillian->SetFont(font10);
	ncVillian->SetForegroundColour(wxColour(245, 245, 245));
	ncSecon = new wxRadioButton(m_panel1, wxID_ANY, "Secondary");
	ncSecon->SetFont(font10);
	ncSecon->SetForegroundColour(wxColour(245, 245, 245));

	wxStaticText* label7 = new wxStaticText(m_panel1, wxID_ANY, "Role: ", wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER | wxALIGN_LEFT);
	label7->SetBackgroundColour(darker);
	label7->SetForegroundColour(txtCol);
	label7->SetFont(wxFont(wxFontInfo(12)));

	wxGridSizer* btnHolderSizer2 = new wxGridSizer(2, 2, 2, 2);
	btnHolderSizer2->Add(ncMain);
	btnHolderSizer2->Add(ncVillian);
	btnHolderSizer2->Add(ncSupp);
	btnHolderSizer2->Add(ncSecon);

	wxBoxSizer* thirdLine = new wxBoxSizer(wxHORIZONTAL);
	thirdLine->Add(ageLabel, wxSizerFlags(0).CenterVertical().Border(wxRIGHT, 8));
	thirdLine->Add(ncAge, wxSizerFlags(1).CenterVertical().Border(wxRIGHT, 16));
	thirdLine->Add(nicknameLabel, wxSizerFlags(0).CenterVertical());
	thirdLine->Add(ncNickname, wxSizerFlags(3).CenterVertical().Border(wxRIGHT, 16));
	thirdLine->Add(label7, wxSizerFlags(0).CenterVertical());
	thirdLine->Add(btnHolderSizer2, wxSizerFlags(0).CenterVertical());

	label8 = new wxStaticText(m_panel1, wxID_ANY, "Appearance", wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE | wxALIGN_LEFT);
	label8->SetBackgroundColour(wxColour(230, 60, 60));
	label8->SetFont(wxFont(wxFontInfo(13).Bold()));
	ncAppearance = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxBORDER_SIMPLE);
	ncAppearance->SetBackgroundColour(dark);
	ncAppearance->SetForegroundColour(wxColour(250, 250, 250));

	m_mPreDefLongAttr[label8] = ncAppearance;

	wxStaticText* label9 = new wxStaticText(m_panel1, wxID_ANY, "Personality", wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE | wxALIGN_LEFT);
	label9->SetBackgroundColour(wxColour(230, 60, 60));
	label9->SetFont(wxFont(wxFontInfo(13).Bold()));
	ncPersonality = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxBORDER_SIMPLE);
	ncPersonality->SetBackgroundColour(dark);
	ncPersonality->SetForegroundColour(wxColour(250, 250, 250));

	m_mPreDefLongAttr[label9] = ncPersonality;

	wxStaticText* label10 = new wxStaticText(m_panel1, wxID_ANY, "Backstory", wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE | wxALIGN_LEFT);
	label10->SetBackgroundColour(wxColour(230, 60, 60));
	label10->SetFont(wxFont(wxFontInfo(13).Bold()));
	ncBackstory = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxBORDER_SIMPLE);
	ncBackstory->SetBackgroundColour(dark);
	ncBackstory->SetForegroundColour(wxColour(250, 250, 250));

	m_mPreDefLongAttr[label10] = ncBackstory;

	wxBoxSizer* verSizer1 = new wxBoxSizer(wxVERTICAL);
	verSizer1->AddSpacer(15);
	verSizer1->Add(firstLine, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));
	verSizer1->AddSpacer(10);
	verSizer1->Add(secondLine, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));
	verSizer1->AddSpacer(10);
	verSizer1->Add(thirdLine, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));
	verSizer1->AddSpacer(22);
	verSizer1->Add(label8, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));
	verSizer1->Add(ncAppearance, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 15));
	verSizer1->AddSpacer(10);
	verSizer1->Add(label9, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));
	verSizer1->Add(ncPersonality, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 15));
	verSizer1->AddSpacer(10);
	verSizer1->Add(label10, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));
	verSizer1->Add(ncBackstory, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 15));

	m_panel1->SetSizer(verSizer1);

	SetIcon(wxICON(characterIcon));
	Layout();
	Show();

	return true;
}

void amCharacterCreator::StartEditing(am::StoryElement* editChar)
{
	am::Character* character = dynamic_cast<am::Character*>(editChar);
	if ( !character )
	{
		wxMessageBox("There was an unexpected error. Editing could not be start.");
		Destroy();
		return;
	}

	m_next->SetId(BUTTON_NextEdit1);
	m_back->SetId(BUTTON_BackEdit1);

	ncFullName->SetValue(character->name);

	auto& it = character->mShortAttributes.find(_("Sex"));
	if ( it != character->mShortAttributes.end() )
	{
		ncSex->SetValue(it->second);
	}

	it = character->mShortAttributes.find(_("Age"));
	if ( it != character->mShortAttributes.end() )
	{
		ncAge->SetValue(it->second);
	}

	it = character->mShortAttributes.find(_("Nationality"));
	if ( it != character->mShortAttributes.end() )
	{
		ncNationality->SetValue(it->second);
	}

	it = character->mShortAttributes.find(_("Height"));
	if ( it != character->mShortAttributes.end() )
	{
		ncHeight->SetValue(it->second);
	}

	it = character->mShortAttributes.find(_("Nickname"));
	if ( it != character->mShortAttributes.end() )
	{
		ncNickname->SetValue(it->second);
	}

	switch ( character->role )
	{
	case am::cProtagonist:
		ncMain->SetValue(true);
		break;
	case am::cSupporting:
		ncSupp->SetValue(true);
		break;
	case am::cVillian:
		ncVillian->SetValue(true);
		break;
	case am::cSecondary:
		ncSecon->SetValue(true);
		break;
	default:
		break;
	}

	it = character->mLongAttributes.find(_("Appearance"));
	if ( it != character->mLongAttributes.end() )
	{
		ncAppearance->SetValue(it->second);
	}

	it = character->mLongAttributes.find(_("Personality"));
	if ( it != character->mLongAttributes.end() )
	{
		ncPersonality->SetValue(it->second);
	}

	it = character->mLongAttributes.find(_("Backstory"));
	if ( it != character->mLongAttributes.end() )
	{
		ncBackstory->SetValue(it->second);
	}
	
	int i = 0;
	for ( auto& it : character->mLongAttributes )
	{
		if ( it.first != _("Appearance") && it.first != _("Personality") && it.first != _("Backstory") )
		{
			AddCustomAttr(wxCommandEvent());
			m_custom[i].first->SetValue(it.first);
			m_custom[i].second->SetValue(it.second);

			i++;
		}
	}

	wxImage& image = m_imagePanel->GetImage();
	image = character->image;

	m_removeImage->Show(m_imagePanel->SetImage(image));

	m_elementEdit = character;
	SetTitle("Edit character - '" + m_elementEdit->name + "'");
}

void amCharacterCreator::DoEdit(wxCommandEvent& WXUNUSED(event))
{
	am::Role newRole;
	if ( ncMain->GetValue() )
		newRole = am::cProtagonist;
	else if ( ncSupp->GetValue() )
		newRole = am::cSupporting;
	else if ( ncSecon->GetValue() )
		newRole = am::cSecondary;
	else if ( ncVillian->GetValue() )
		newRole = am::cVillian;

	am::Character character;

	character.name = ncFullName->GetValue();
	character.role = newRole;

	for ( std::pair<wxString, wxString>& it : GetShortAttributes() )
	{
		character.mShortAttributes.insert({ it.first, it.second });
	}

	for ( std::pair<wxString, wxString>& it : GetLongAttributes() )
	{
		character.mLongAttributes.insert({ it.first, it.second });
	}

	character.image = m_imagePanel->GetImage();

	character.documents = ((am::Character*)m_elementEdit)->documents;
	character.aliases = m_elementEdit->aliases;
	character.relatedElements = m_elementEdit->relatedElements;
	character.dbID = m_elementEdit->dbID;

	am::Character* pCharacterToEdit = (am::Character*)m_elementEdit;
	bool dif = pCharacterToEdit->name != ncFullName->GetValue() || pCharacterToEdit->role != newRole;

	Hide();
	am::GetMainFrame()->Enable();
	am::DoEditStoryElement(pCharacterToEdit, character, dif);

	Destroy();
}

void amCharacterCreator::AddCustomAttr(wxCommandEvent& event)
{
	Freeze();
	wxSize size(label8->GetSize());

	wxTextCtrl* label = new wxTextCtrl(m_panel2, -1, "Title " + std::to_string(m_custom.size() + 1),
		wxDefaultPosition, wxSize(-1, size.y), wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
	wxTextCtrl* content = new wxTextCtrl(m_panel2, -1, "", wxDefaultPosition,
		wxSize(-1, ncAppearance->GetSize().y), wxTE_MULTILINE | wxBORDER_SIMPLE);

	m_mLongAttr[label] = content;

	content->SetBackgroundColour(wxColour(60, 60, 60));
	content->SetForegroundColour(wxColour(255, 255, 255));
	label->SetBackgroundColour(wxColour(230, 60, 60));
	label->SetFont(wxFontInfo(13).Bold());

	wxButton* minus = new wxButton(m_panel2, -1, "", wxDefaultPosition, wxSize(size.y, size.y));
	minus->SetBitmap(wxBITMAP_PNG(minus));
	minus->Bind(wxEVT_BUTTON, &amCharacterCreator::RemoveCustomAttr, this);

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(label, wxSizerFlags(1));
	topSizer->Add(minus, 0);

	label->Show();
	content->Show();

	m_customSizer->Add(topSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 15));
	m_customSizer->Add(content, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));

	m_customSizer->FitInside(m_panel2);

	m_custom.push_back(pair<wxTextCtrl*, wxTextCtrl*>(label, content));
	m_minusButtons.push_back(minus);

	Thaw();
}

void amCharacterCreator::OnCreateElement(wxCommandEvent& event)
{
	if ( !ncFullName->IsEmpty() )
	{
		am::Character* pCharacter = new am::Character;
		pCharacter->name = ncFullName->GetValue();

		if ( ncMain->GetValue() )
			pCharacter->role = am::cProtagonist;
		else if ( ncSupp->GetValue() )
			pCharacter->role = am::cSupporting;
		else if ( ncVillian->GetValue() )
			pCharacter->role = am::cVillian;
		else if ( ncSecon->GetValue() )
			pCharacter->role = am::cSecondary;

		for ( std::pair<wxString, wxString>& it : GetShortAttributes() )
		{
			pCharacter->mShortAttributes.insert({ it.first, it.second });
		}

		for ( std::pair<wxString, wxString>& it : GetLongAttributes() )
		{
			pCharacter->mLongAttributes.insert({ it.first, it.second });
		}

		pCharacter->image = m_imagePanel->GetImage();

		Hide();
		am::GetMainFrame()->Enable();
		am::AddStoryElement(pCharacter, true);
	}
	else
	{
		wxMessageBox("You can't create a character without a name.");
		return;
	}

	Destroy();
	event.Skip();
}

void amCharacterCreator::OnClose(wxCloseEvent& event)
{
	if ( ncFullName->IsModified() || ncAge->IsModified() || ncNationality->IsModified() ||
		ncHeight->IsModified() || ncNickname->IsModified() ||
		ncAppearance->IsModified() || ncPersonality->IsModified() || ncBackstory->IsModified() )
	{
		wxMessageDialog check(this, "Are you sure you want to close?",
			"Close window", wxYES_NO | wxNO_DEFAULT);

		if ( check.ShowModal() != wxID_YES )
			return;
	}

	am::GetMainFrame()->Enable(true);
	event.Skip();
}


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// am::Location Creator ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(amLocationCreator, amElementCreator);

amLocationCreator::amLocationCreator(wxWindow* parent, long id, const wxString& label,
	const wxPoint& pos, const wxSize& size, long style)
{
	Create(parent, id, label, pos, size, style);
}

bool amLocationCreator::Create(wxWindow* parent,
	long id,
	const wxString& label,
	const wxPoint& pos,
	const wxSize& size,
	long style)
{
	if ( !amElementCreator::Create(parent, id, label, pos, size, style) )
		return false;
	
	Bind(wxEVT_CLOSE_WINDOW, &amLocationCreator::OnClose, this);

	wxColour dark(50, 50, 50);
	wxColour darker(30, 30, 30);
	wxColour grey(200, 200, 200);
	wxColour red(230, 60, 60);
	wxColour txtCol(255, 255, 255);

	wxFont font(wxFontInfo(9));

	wxPanel* panel = new wxPanel(m_panel1, wxID_ANY, wxPoint(80, 20), wxSize(130, 35));
	panel->SetBackgroundColour(red);

	wxStaticText* label1 = new wxStaticText(panel, wxID_ANY, "Name of location:",
		wxDefaultPosition, wxSize(200, 25), wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxNO_BORDER);
	label1->SetBackgroundColour(darker);
	label1->SetForegroundColour(txtCol);
	label1->SetFont(wxFont(wxFontInfo(12)));

	nlName = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(220, 25),
		wxSize(330, 25), wxBORDER_SIMPLE);
	nlName->SetBackgroundColour(dark);
	nlName->SetForegroundColour(txtCol);

	nlName->SetFont(wxFont(wxFontInfo(10)));

	wxStaticText* pImportanceLabel = new wxStaticText(m_panel1, wxID_ANY, "Importance:",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_SIMPLE);
	pImportanceLabel->SetBackgroundColour(grey);
	pImportanceLabel->SetFont(wxFont(wxFontInfo(12).Bold()));

	nlHigh = new wxRadioButton(m_panel1, wxID_ANY, "High", wxPoint(115, 545), wxSize(60, 20), wxRB_GROUP);
	nlHigh->SetFont(wxFont(wxFontInfo(10)));
	nlHigh->SetForegroundColour(txtCol);

	nlLow = new wxRadioButton(m_panel1, wxID_ANY, "Low", wxPoint(115, 565), wxSize(60, 20));
	nlLow->SetFont(wxFont(wxFontInfo(10)));
	nlLow->SetForegroundColour(txtCol);

	wxBoxSizer* hlSizer = new wxBoxSizer(wxVERTICAL);
	hlSizer->Add(nlHigh, wxSizerFlags(0));
	hlSizer->Add(nlLow, wxSizerFlags(0));

	wxBoxSizer* imSizer = new wxBoxSizer(wxHORIZONTAL);
	imSizer->Add(pImportanceLabel, wxSizerFlags(0).CenterVertical());
	imSizer->AddSpacer(5);
	imSizer->Add(hlSizer, wxSizerFlags(0).Expand());

	wxBoxSizer* siz = new wxBoxSizer(wxHORIZONTAL);
	siz->Add(label1, wxSizerFlags(1).CenterVertical());
	panel->SetSizer(siz);

	wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
	firstLine->Add(panel, wxSizerFlags(0).Border(wxRIGHT, 10));
	firstLine->Add(nlName, wxSizerFlags(1).CenterVertical());
	firstLine->AddSpacer(30);
	firstLine->Add(imSizer, wxSizerFlags(0).CenterVertical());

	wxStaticText* pGeneralLabel = new wxStaticText(m_panel1, wxID_ANY, "General",
		wxPoint(10, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
	pGeneralLabel->SetBackgroundColour(red);
	pGeneralLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	nlGeneral = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(10, 95),
		wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
	nlGeneral->SetBackgroundColour(dark);
	nlGeneral->SetForegroundColour(txtCol);
	nlGeneral->SetFont(font);

	m_mPreDefLongAttr[pGeneralLabel] = nlGeneral;

	wxStaticText* pNaturalLabel = new wxStaticText(m_panel1, wxID_ANY, "Natural characteristics",
		wxPoint(220, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
	pNaturalLabel->SetBackgroundColour(grey);
	pNaturalLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	nlNatural = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(220, 95),
		wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
	nlNatural->SetBackgroundColour(dark);
	nlNatural->SetForegroundColour(txtCol);
	nlNatural->SetFont(font);

	m_mPreDefLongAttr[pNaturalLabel] = nlNatural;

	wxStaticText* pArchitectureLabel = new wxStaticText(m_panel1, wxID_ANY, "Architecture",
		wxPoint(430, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
	pArchitectureLabel->SetBackgroundColour(red);
	pArchitectureLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	nlArchitecture = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(430, 95),
		wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
	nlArchitecture->SetBackgroundColour(dark);
	nlArchitecture->SetForegroundColour(txtCol);
	nlArchitecture->SetFont(font);

	m_mPreDefLongAttr[pArchitectureLabel] = nlArchitecture;

	wxStaticText* pPoliticsLabel = new wxStaticText(m_panel1, wxID_ANY, "Politics",
		wxPoint(10, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
	pPoliticsLabel->SetBackgroundColour(grey);
	pPoliticsLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	nlPolitics = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE | wxBORDER_SIMPLE);
	nlPolitics->SetBackgroundColour(dark);
	nlPolitics->SetForegroundColour(txtCol);
	nlPolitics->SetFont(font);

	m_mPreDefLongAttr[pPoliticsLabel] = nlPolitics;

	pEconomyLabel = new wxStaticText(m_panel1, wxID_ANY, "Economy",
		wxPoint(220, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
	pEconomyLabel->SetBackgroundColour(red);
	pEconomyLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	nlEconomy = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(220, 335),
		wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
	nlEconomy->SetBackgroundColour(dark);
	nlEconomy->SetForegroundColour(txtCol);
	nlEconomy->SetFont(font);

	m_mPreDefLongAttr[pEconomyLabel] = nlEconomy;

	wxStaticText* pCultureLabel = new wxStaticText(m_panel1, wxID_ANY, "Culture",
		wxPoint(430, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
	pCultureLabel->SetBackgroundColour(grey);
	pCultureLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	nlCulture = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(430, 335),
		wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
	nlCulture->SetBackgroundColour(dark);
	nlCulture->SetForegroundColour(txtCol);
	nlCulture->SetFont(font);

	m_mPreDefLongAttr[pCultureLabel] = nlCulture;

	wxBoxSizer* firstColumn = new wxBoxSizer(wxVERTICAL);
	firstColumn->Add(pGeneralLabel, wxSizerFlags(0).Expand());
	firstColumn->Add(nlGeneral, wxSizerFlags(1).Expand());
	firstColumn->AddSpacer(10);
	firstColumn->Add(pPoliticsLabel, wxSizerFlags(0).Expand());
	firstColumn->Add(nlPolitics, wxSizerFlags(1).Expand());

	wxBoxSizer* secondColumn = new wxBoxSizer(wxVERTICAL);
	secondColumn->Add(pNaturalLabel, wxSizerFlags(0).Expand());
	secondColumn->Add(nlNatural, wxSizerFlags(1).Expand());
	secondColumn->AddSpacer(10);
	secondColumn->Add(pEconomyLabel, wxSizerFlags(0).Expand());
	secondColumn->Add(nlEconomy, wxSizerFlags(1).Expand());

	wxBoxSizer* thirdColumn = new wxBoxSizer(wxVERTICAL);
	thirdColumn->Add(pArchitectureLabel, wxSizerFlags(0).Expand());
	thirdColumn->Add(nlArchitecture, wxSizerFlags(1).Expand());
	thirdColumn->AddSpacer(10);
	thirdColumn->Add(pCultureLabel, wxSizerFlags(0).Expand());
	thirdColumn->Add(nlCulture, wxSizerFlags(1).Expand());

	wxBoxSizer* horSizer = new wxBoxSizer(wxHORIZONTAL);
	horSizer->Add(firstColumn, wxSizerFlags(1).Expand());
	horSizer->AddSpacer(10);
	horSizer->Add(secondColumn, wxSizerFlags(1).Expand());
	horSizer->AddSpacer(10);
	horSizer->Add(thirdColumn, wxSizerFlags(1).Expand());

	wxBoxSizer* verSizer1 = new wxBoxSizer(wxVERTICAL);
	verSizer1->Add(firstLine, wxSizerFlags(0).CenterHorizontal().Border(wxTOP, 20));
	verSizer1->Add(horSizer, wxSizerFlags(1).Expand().Border(wxALL, 10));

	m_panel1->SetSizer(verSizer1);

	m_nlcustomSizer = new wxWrapSizer(wxHORIZONTAL);

	m_customSizer->Add(m_nlcustomSizer, wxSizerFlags(1).Expand().Border(wxLEFT, 5));
	m_customSizer->FitInside(m_panel2);

	SetIcon(wxICON(locationIcon));

	m_mainSizer->Layout();
	this->Show();

	return true;
}

void amLocationCreator::StartEditing(am::StoryElement* editLoc)
{
	am::Location* location = dynamic_cast<am::Location*>(editLoc);
	if ( !location )
	{
		wxMessageBox("There was an unexpected error. Editing could not start.");
		Destroy();
		return;
	}

	m_next->SetId(BUTTON_NextEdit1);
	m_back->SetId(BUTTON_BackEdit1);

	nlName->SetValue(location->name);

	auto& it = location->mLongAttributes.find(_("General"));
	if ( it != location->mLongAttributes.end() )
	{
		nlGeneral->SetValue(it->second);
	}

	it = location->mLongAttributes.find(_("Natural characteristics"));
	if ( it != location->mLongAttributes.end() )
	{
		nlNatural->SetValue(it->second);
	}

	it = location->mLongAttributes.find(_("Architecture"));
	if ( it != location->mLongAttributes.end() )
	{
		nlArchitecture->SetValue(it->second);
	}

	it = location->mLongAttributes.find(_("Politics"));
	if ( it != location->mLongAttributes.end() )
	{
		nlPolitics->SetValue(it->second);
	}

	it = location->mLongAttributes.find(_("Economy"));
	if ( it != location->mLongAttributes.end() )
	{
		nlEconomy->SetValue(it->second);
	}

	it = location->mLongAttributes.find(_("Culture"));
	if ( it != location->mLongAttributes.end() )
	{
		nlCulture->SetValue(it->second);
	}

	switch ( location->role )
	{
	case am::lHigh:
		nlHigh->SetValue(true);
		break;

	case am::lLow:
		nlLow->SetValue(true);
		break;

	default:
		break;
	}

	int i = 0;
	for ( auto& it : location->mLongAttributes )
	{
		if ( it.first != _("General") && it.first != _("Natural characteristics") && it.first != _("Architecture") &&
			it.first != _("Politics") && it.first != _("Economy") && it.first != _("Culture") )
		{
			AddCustomAttr(wxCommandEvent());
			m_custom[i].first->SetValue(it.first);
			m_custom[i].second->SetValue(it.second);

			i++;
		}
	}

	wxImage& image = m_imagePanel->GetImage();
	image = location->image;
	m_removeImage->Show(m_imagePanel->SetImage(image));

	m_elementEdit = location;

	SetTitle("Edit location - '" + m_elementEdit->name + "'");
}

void amLocationCreator::DoEdit(wxCommandEvent& WXUNUSED(event))
{
	am::Location location;
	location.name = nlName->GetValue();

	for ( std::pair<wxString, wxString>& it : GetShortAttributes() )
	{
		location.mShortAttributes.insert({ it.first, it.second });
	}

	for ( std::pair<wxString, wxString>& it : GetLongAttributes() )
	{
		location.mLongAttributes.insert({ it.first, it.second });
	}

	if ( nlHigh->GetValue() )
		location.role = am::lHigh;
	else if ( nlLow->GetValue() )
		location.role = am::lLow;
	else
		location.role = am::None;

	location.image = m_imagePanel->GetImage();

	location.documents = ((am::Location*)m_elementEdit)->documents;
	location.aliases = m_elementEdit->aliases;
	location.relatedElements = m_elementEdit->relatedElements;
	location.dbID = m_elementEdit->dbID;

	am::Location* pLocationToEdit = (am::Location*)m_elementEdit;
	bool dif = pLocationToEdit->name != nlName->GetValue() || pLocationToEdit->role != location.role;

	Hide();
	am::GetMainFrame()->Enable();
	am::DoEditStoryElement(pLocationToEdit, location, dif);
	Destroy();
}

void amLocationCreator::AddCustomAttr(wxCommandEvent& WXUNUSED(event))
{
	Freeze();
	wxPanel* panel = new wxPanel(m_panel2);
	wxSize size(pEconomyLabel->GetSize());
	wxSize size2(nlArchitecture->GetSize());

	wxTextCtrl* label = new wxTextCtrl(panel, -1, "Title " + std::to_string(m_custom.size() + 1),
		wxDefaultPosition, wxSize(-1, size.y), wxTE_NO_VSCROLL | wxBORDER_SIMPLE | wxTE_CENTER);
	wxTextCtrl* content = new wxTextCtrl(panel, -1, "", wxDefaultPosition,
		wxSize(size2.x - 10, size2.y), wxTE_MULTILINE | wxBORDER_SIMPLE);

	m_mLongAttr[label] = content;

	content->SetBackgroundColour(wxColour(60, 60, 60));
	content->SetForegroundColour(wxColour(250, 250, 250));
	content->SetFont(wxFontInfo(9));

	label->SetBackgroundColour(wxColour(200, 200, 200));
	label->SetFont(wxFontInfo(13).Bold());

	wxButton* minus = new wxButton(panel, -1, "", wxDefaultPosition, wxSize(size.y, size.y));
	minus->SetBitmap(wxBITMAP_PNG(minus));
	minus->Bind(wxEVT_BUTTON, &amLocationCreator::RemoveCustomAttr, this);

	wxBoxSizer* hor = new wxBoxSizer(wxHORIZONTAL);
	hor->Add(label, wxSizerFlags(1));
	hor->Add(minus, wxSizerFlags(0).Expand());

	wxBoxSizer* ver = new wxBoxSizer(wxVERTICAL);
	ver->Add(hor, wxSizerFlags(0).Expand());
	ver->Add(content, wxSizerFlags(1).Expand());

	panel->SetSizer(ver);

	m_nlcustomSizer->Add(panel, wxSizerFlags(0).Border(wxALL, 5));
	m_mainSizer->Layout();

	m_custom.push_back(pair<wxTextCtrl*, wxTextCtrl*>(label, content));
	m_minusButtons.push_back(minus);

	RecolorCustoms();
	Thaw();
}

void amLocationCreator::RemoveCustomAttr(wxCommandEvent& event)
{
	Freeze();
	amElementCreator::RemoveCustomAttr(event);
	RecolorCustoms();
	Thaw();
}

void amLocationCreator::RecolorCustoms()
{
	wxColour red(230, 60, 60);
	wxColour grey(200, 200, 200);

	bool doRed = true;
	for ( auto& it : m_custom )
	{
		if ( doRed )
			it.first->SetBackgroundColour(red);
		else
			it.first->SetBackgroundColour(grey);

		doRed = !doRed;
	}
}

void amLocationCreator::OnCreateElement(wxCommandEvent& WXUNUSED(event))
{
	if ( !nlName->IsEmpty() )
	{
		am::Location* pLocation = new am::Location;

		pLocation->name = nlName->GetValue();

		for ( std::pair<wxString, wxString>& it : GetShortAttributes() )
		{
			pLocation->mShortAttributes.insert({ it.first, it.second });
		}

		for ( std::pair<wxString, wxString>& it : GetLongAttributes() )
		{
			pLocation->mLongAttributes.insert({ it.first, it.second });
		}

		if ( nlHigh->GetValue() )
			pLocation->role = am::lHigh;
		else if ( nlLow->GetValue() )
			pLocation->role = am::lLow;
		else
			pLocation->role = am::None;

		pLocation->image = m_imagePanel->GetImage();

		//pLocation->custom = GetCustom();

		Hide();
		am::GetMainFrame()->Enable();
		am::AddStoryElement(pLocation, true);
	}
	else
	{
		wxMessageBox("You can't create a location with no name!");
		return;
	}

	Destroy();
}

void amLocationCreator::OnClose(wxCloseEvent& event)
{
	if ( nlArchitecture->IsModified() || nlName->IsModified() || nlGeneral->IsModified() ||
		nlNatural->IsModified() || nlEconomy->IsModified() || nlCulture->IsModified() )
	{
		wxMessageDialog check(this, "Are you sure you want to close?", "Close", wxYES_NO | wxNO_DEFAULT);

		if ( check.ShowModal() != wxID_YES )
			return;
	}

	am::GetMainFrame()->Enable(true);
	event.Skip();
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// am::Item Creator ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(amItemCreator, amElementCreator);

amItemCreator::amItemCreator(wxWindow* parent, long id, const wxString& label,
	const wxPoint& pos, const wxSize& size, long style)
{
	Create(parent, id, label, pos, size, style);
}

bool amItemCreator::Create(wxWindow* parent,
	long id,
	const wxString& label,
	const wxPoint& pos,
	const wxSize& size,
	long style)
{
	if ( !amElementCreator::Create(parent, id, label, pos, size, style) )
		return false;

	Bind(wxEVT_CLOSE_WINDOW, &amItemCreator::OnClose, this);

	wxColour dark(40, 40, 40);
	wxColour darker(30, 30, 30);
	wxColour txtCol(255, 255, 255);

	m_panel1->Bind(wxEVT_PAINT, &amItemCreator::OnPaint, this);

	wxStaticText* nameLabel = new wxStaticText(m_panel1, wxID_ANY, "Name:",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	nameLabel->SetBackgroundColour(darker);
	nameLabel->SetForegroundColour(txtCol);
	nameLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	niName = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_SIMPLE);
	niName->SetBackgroundColour(dark);
	niName->SetForegroundColour(txtCol);

	wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
	nameSizer->Add(nameLabel);
	nameSizer->AddSpacer(3);
	nameSizer->Add(niName, 1);

	wxStaticText* typeLabel = new wxStaticText(m_panel1, wxID_ANY, "Type:",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	typeLabel->SetBackgroundColour(darker);
	typeLabel->SetForegroundColour(txtCol);
	typeLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	niNatural = new wxRadioButton(m_panel1, -1, "Natural", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	niNatural->SetForegroundColour(txtCol);
	niNatural->SetFont(wxFontInfo(11));
	niManMade = new wxRadioButton(m_panel1, -1, "Man-made");
	niManMade->SetForegroundColour(txtCol);
	niManMade->SetFont(wxFontInfo(11));

	wxBoxSizer* typeSizer = new wxBoxSizer(wxVERTICAL);
	typeSizer->Add(niNatural);
	typeSizer->AddSpacer(3);
	typeSizer->Add(niManMade);

	wxStaticText* magicLabel = new wxStaticText(m_panel1, wxID_ANY, "Is magic:",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	magicLabel->SetBackgroundColour(darker);
	magicLabel->SetForegroundColour(txtCol);
	magicLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	niMagic = new wxRadioButton(m_panel1, -1, "Yes", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	niMagic->SetForegroundColour(txtCol);
	niMagic->SetFont(wxFontInfo(11));
	niNonMagic = new wxRadioButton(m_panel1, -1, "No");
	niNonMagic->SetForegroundColour(txtCol);
	niNonMagic->SetFont(wxFontInfo(11));

	wxBoxSizer* magicSizer = new wxBoxSizer(wxVERTICAL);
	magicSizer->Add(niMagic);
	magicSizer->AddSpacer(3);
	magicSizer->Add(niNonMagic);

	wxBoxSizer* choicesSizer = new wxBoxSizer(wxHORIZONTAL);
	choicesSizer->Add(typeLabel, wxSizerFlags(0).Center());
	choicesSizer->AddSpacer(3);
	choicesSizer->Add(typeSizer, wxSizerFlags(0).Center());
	choicesSizer->AddStretchSpacer(1);
	choicesSizer->Add(magicLabel, wxSizerFlags(0).Center());
	choicesSizer->AddSpacer(3);
	choicesSizer->Add(magicSizer, wxSizerFlags(0).Center());

	wxBoxSizer* rightSiz1 = new wxBoxSizer(wxVERTICAL);
	rightSiz1->Add(nameSizer, wxSizerFlags(0).Expand());
	rightSiz1->AddSpacer(10);
	rightSiz1->Add(choicesSizer, wxSizerFlags(0).Expand());

	meaLabel = new wxStaticText(m_panel1, -1, "Measurements");
	meaLabel->SetBackgroundColour(darker);
	meaLabel->SetForegroundColour(txtCol);
	meaLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	wxStaticText* widthLabel = new wxStaticText(m_panel1, -1, "Width:");
	widthLabel->SetBackgroundColour(darker);
	widthLabel->SetForegroundColour(txtCol);
	widthLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	wxStaticText* heightLabel = new wxStaticText(m_panel1, -1, "Height:");
	heightLabel->SetBackgroundColour(darker);
	heightLabel->SetForegroundColour(txtCol);
	heightLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	wxStaticText* depthLabel = new wxStaticText(m_panel1, -1, "Depth:");
	depthLabel->SetBackgroundColour(darker);
	depthLabel->SetForegroundColour(txtCol);
	depthLabel->SetFont(wxFont(wxFontInfo(12).Bold()));

	wxBoxSizer* firstColumn = new wxBoxSizer(wxVERTICAL);
	firstColumn->Add(widthLabel);
	firstColumn->AddStretchSpacer(1);
	firstColumn->Add(heightLabel);
	firstColumn->AddStretchSpacer(1);
	firstColumn->Add(depthLabel);
	firstColumn->AddStretchSpacer(1);

	niWidth = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_SIMPLE);
	niWidth->SetBackgroundColour(dark);
	niWidth->SetForegroundColour(txtCol);

	niHeight = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_SIMPLE);
	niHeight->SetBackgroundColour(dark);
	niHeight->SetForegroundColour(txtCol);

	niDepth = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_SIMPLE);
	niDepth->SetBackgroundColour(dark);
	niDepth->SetForegroundColour(txtCol);

	m_mShortAttr[widthLabel] = niWidth;
	m_mShortAttr[heightLabel] = niHeight;
	m_mShortAttr[depthLabel] = niDepth;

	wxBoxSizer* secondColumn = new wxBoxSizer(wxVERTICAL);
	secondColumn->Add(niWidth);
	secondColumn->AddStretchSpacer(1);
	secondColumn->Add(niHeight);
	secondColumn->AddStretchSpacer(1);
	secondColumn->Add(niDepth);
	secondColumn->AddStretchSpacer(1);

	wxBoxSizer* leftSiz2 = new wxBoxSizer(wxHORIZONTAL);
	leftSiz2->Add(firstColumn, wxSizerFlags(0).Expand());
	leftSiz2->AddStretchSpacer(1);
	leftSiz2->Add(secondColumn, wxSizerFlags(2).Expand());

	niMeaSizer = new wxBoxSizer(wxVERTICAL);
	niMeaSizer->Add(meaLabel, wxSizerFlags(0).Expand());
	niMeaSizer->AddSpacer(20);
	niMeaSizer->Add(leftSiz2, wxSizerFlags(1).Expand());

	wxStaticText* importanceLabel = new wxStaticText(m_panel1, wxID_ANY, "Importance:",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	importanceLabel->SetBackgroundColour(darker);
	importanceLabel->SetForegroundColour(txtCol);
	importanceLabel->SetFont(wxFont(wxFontInfo(12).Bold()));

	niHigh = new wxRadioButton(m_panel1, -1, "High", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	niHigh->SetForegroundColour(txtCol);
	niMagic->SetFont(wxFontInfo(11));
	niLow = new wxRadioButton(m_panel1, -1, "Low");
	niLow->SetForegroundColour(txtCol);
	niLow->SetFont(wxFontInfo(11));

	wxBoxSizer* importanceSizer = new wxBoxSizer(wxHORIZONTAL);
	importanceSizer->Add(importanceLabel);
	importanceSizer->AddSpacer(10);
	importanceSizer->Add(niHigh);
	importanceSizer->AddSpacer(5);
	importanceSizer->Add(niLow);

	wxBoxSizer* leftVerSiz = new wxBoxSizer(wxVERTICAL);
	leftVerSiz->AddSpacer(10);
	leftVerSiz->Add(rightSiz1, wxSizerFlags(1).Expand());
	leftVerSiz->AddStretchSpacer(1);
	leftVerSiz->Add(niMeaSizer, wxSizerFlags(3).Expand());
	leftVerSiz->Add(importanceSizer, wxSizerFlags(0).Expand());
	leftVerSiz->AddStretchSpacer(1);

	wxStaticText* appLabel = new wxStaticText(m_panel1, wxID_ANY, "Appearance",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	appLabel->SetBackgroundColour(darker);
	appLabel->SetForegroundColour(txtCol);
	appLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	niAppearance = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
	niAppearance->SetBackgroundColour(dark);
	niAppearance->SetForegroundColour(txtCol);

	wxStaticText* originLabel = new wxStaticText(m_panel1, wxID_ANY, "Origin",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	originLabel->SetBackgroundColour(darker);
	originLabel->SetForegroundColour(txtCol);
	originLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	niOrigin = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
	niOrigin->SetBackgroundColour(dark);
	niOrigin->SetForegroundColour(txtCol);

	wxStaticText* backLabel = new wxStaticText(m_panel1, wxID_ANY, "Backstory",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	backLabel->SetBackgroundColour(darker);
	backLabel->SetForegroundColour(txtCol);
	backLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	niBackstory = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
	niBackstory->SetBackgroundColour(dark);
	niBackstory->SetForegroundColour(txtCol);

	wxStaticText* usageLabel = new wxStaticText(m_panel1, wxID_ANY, "Usage",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	usageLabel->SetBackgroundColour(darker);
	usageLabel->SetForegroundColour(txtCol);
	usageLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	niUsage = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
	niUsage->SetBackgroundColour(dark);
	niUsage->SetForegroundColour(txtCol);

	wxBoxSizer* verSizer1 = new wxBoxSizer(wxVERTICAL);
	verSizer1->Add(appLabel, wxSizerFlags(0).Left());
	verSizer1->AddSpacer(4);
	verSizer1->Add(niAppearance, wxSizerFlags(1).Expand());
	verSizer1->AddSpacer(2);
	verSizer1->Add(originLabel, wxSizerFlags(0).Left());
	verSizer1->AddSpacer(4);
	verSizer1->Add(niOrigin, wxSizerFlags(1).Expand());
	verSizer1->AddSpacer(2);
	verSizer1->Add(backLabel, wxSizerFlags(0).Left());
	verSizer1->AddSpacer(4);
	verSizer1->Add(niBackstory, wxSizerFlags(1).Expand());
	verSizer1->AddSpacer(2);
	verSizer1->Add(usageLabel, wxSizerFlags(0).Left());
	verSizer1->AddSpacer(4);
	verSizer1->Add(niUsage, wxSizerFlags(1).Expand());

	wxBoxSizer* mainHor = new wxBoxSizer(wxHORIZONTAL);
	mainHor->Add(leftVerSiz, wxSizerFlags(1).Expand());
	mainHor->AddSpacer(20);
	mainHor->Add(verSizer1, wxSizerFlags(2).Expand());

	wxStaticText* generalLabel = new wxStaticText(m_panel1, wxID_ANY, "General",
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
	generalLabel->SetBackgroundColour(darker);
	generalLabel->SetForegroundColour(txtCol);
	generalLabel->SetFont(wxFont(wxFontInfo(12).Bold()));
	niGeneral = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
	niGeneral->SetBackgroundColour(dark);
	niGeneral->SetForegroundColour(txtCol);

	m_mPreDefLongAttr[generalLabel] = niGeneral;
	m_mPreDefLongAttr[appLabel] = niAppearance;
	m_mPreDefLongAttr[originLabel] = niOrigin;
	m_mPreDefLongAttr[backLabel] = niBackstory;
	m_mPreDefLongAttr[usageLabel] = niUsage;

	wxBoxSizer* mainVer = new wxBoxSizer(wxVERTICAL);
	mainVer->AddSpacer(10);
	mainVer->Add(mainHor, wxSizerFlags(4).Expand().Border(wxLEFT | wxRIGHT, 20));
	mainVer->Add(generalLabel, wxSizerFlags(0).Left().Border(wxLEFT | wxRIGHT, 20));
	mainVer->AddSpacer(5);
	mainVer->Add(niGeneral, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 20));

	m_panel1->SetSizer(mainVer);
	Layout();
	SetIcon(wxICON(itemIcon));

	return true;
}

void amItemCreator::StartEditing(am::StoryElement* editItem)
{
	am::Item* item = dynamic_cast<am::Item*>(editItem);
	if ( !item )
	{
		wxMessageBox("There was an unexpected error. Editing could not start.");
		Destroy();
		return;
	}

	m_next->SetId(BUTTON_NextEdit1);
	m_back->SetId(BUTTON_BackEdit1);

	niName->SetValue(item->name);

	if ( item->isManMade )
		niManMade->SetValue(true);

	if ( !item->isMagic )
		niNonMagic->SetValue(true);

	if ( item->role == am::iLow )
		niLow->SetValue(true);

	auto& it = item->mShortAttributes.find(_("Width"));
	if ( it != item->mShortAttributes.end() )
	{
		niWidth->SetValue(it->second);
	}

	it = item->mShortAttributes.find(_("Height"));
	if ( it != item->mShortAttributes.end() )
	{
		niHeight->SetValue(it->second);
	}

	it = item->mShortAttributes.find(_("Depth"));
	if ( it != item->mShortAttributes.end() )
	{
		niDepth->SetValue(it->second);
	}

	it = item->mLongAttributes.find(_("Appearance"));
	if ( it != item->mLongAttributes.end() )
	{
		niAppearance->SetValue(it->second);
	}

	it = item->mLongAttributes.find(_("Origin"));
	if ( it != item->mLongAttributes.end() )
	{
		niOrigin->SetValue(it->second);
	}

	it = item->mLongAttributes.find(_("Backstory"));
	if ( it != item->mLongAttributes.end() )
	{
		niBackstory->SetValue(it->second);
	}

	it = item->mLongAttributes.find(_("Usage"));
	if ( it != item->mLongAttributes.end() )
	{
		niUsage->SetValue(it->second);
	}

	it = item->mLongAttributes.find(_("General"));
	if ( it != item->mLongAttributes.end() )
	{
		niGeneral->SetValue(it->second);
	}

	int i = 0;
	for ( auto& it : item->mLongAttributes )
	{
		if ( it.first != _("General") && it.first != _("Appearance") && it.first != _("Origin") &&
			it.first != _("Backstory") && it.first != _("Usage") )
		{
			AddCustomAttr(wxCommandEvent());
			m_custom[i].first->SetValue(it.first);
			m_custom[i].second->SetValue(it.second);

			i++;
		}
	}

	m_removeImage->Show(m_imagePanel->SetImage(item->image));

	m_elementEdit = item;
	SetTitle("Edit item - '" + m_elementEdit->name + "'");
}

void amItemCreator::DoEdit(wxCommandEvent& event)
{
	am::Item item;
	item.name = niName->GetValue();

	if ( niNatural->GetValue() )
		item.isManMade = false;
	else
		item.isManMade = true;

	if ( niMagic->GetValue() )
		item.isMagic = true;
	else
		item.isMagic = false;

	if ( niHigh->GetValue() )
		item.role = am::iHigh;
	else
		item.role = am::iLow;

	for ( std::pair<wxString, wxString>& it : GetShortAttributes() )
	{
		item.mShortAttributes.insert({ it.first, it.second });
	}

	for ( std::pair<wxString, wxString>& it : GetLongAttributes() )
	{
		item.mLongAttributes.insert({ it.first, it.second });
	}

	item.image = m_imagePanel->GetImage();

	item.documents = ((am::Item*)m_elementEdit)->documents;
	item.aliases = m_elementEdit->aliases;
	item.relatedElements = m_elementEdit->relatedElements;
	item.dbID = m_elementEdit->dbID;

	am::Item* pItemToEdit = (am::Item*)m_elementEdit;
	bool dif = pItemToEdit->name != niName->GetValue() || pItemToEdit->role != item.role;

	Hide();
	am::GetMainFrame()->Enable();
	am::DoEditStoryElement(pItemToEdit, item, dif);

	Destroy();
}

void amItemCreator::AddCustomAttr(wxCommandEvent& event)
{
	Freeze();
	wxSize size(meaLabel->GetSize());

	wxTextCtrl* label = new wxTextCtrl(m_panel2, -1, "Title " + std::to_string(m_custom.size() + 1),
		wxDefaultPosition, wxSize(-1, size.y), wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
	wxTextCtrl* content = new wxTextCtrl(m_panel2, -1, "", wxDefaultPosition,
		wxSize(-1, niGeneral->GetSize().y), wxTE_MULTILINE | wxBORDER_SIMPLE);

	m_mLongAttr[label] = content;

	content->SetBackgroundColour(wxColour(60, 60, 60));
	content->SetForegroundColour(wxColour(255, 255, 255));
	label->SetBackgroundColour(wxColour(230, 60, 60));
	label->SetFont(wxFontInfo(13).Bold());

	wxButton* minus = new wxButton(m_panel2, -1, "", wxDefaultPosition, wxSize(size.y, size.y));
	minus->SetBitmap(wxBITMAP_PNG(minus));
	minus->Bind(wxEVT_BUTTON, &amCharacterCreator::RemoveCustomAttr, this);

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(label, wxSizerFlags(1));
	topSizer->Add(minus, 0);

	m_customSizer->Add(topSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 15));
	m_customSizer->Add(content, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));

	m_customSizer->FitInside(m_panel2);

	m_custom.push_back(pair<wxTextCtrl*, wxTextCtrl*>(label, content));
	m_minusButtons.push_back(minus);

	Thaw();
}

void amItemCreator::OnCreateElement(wxCommandEvent& event)
{
	if ( !niName->IsEmpty() )
	{
		am::Item* pItem = new am::Item;
		pItem->name = niName->GetValue();

		if ( niNatural->GetValue() )
			pItem->isManMade = false;
		else
			pItem->isManMade = true;

		if ( niMagic->GetValue() )
			pItem->isMagic = true;
		else
			pItem->isMagic = false;

		if ( niHigh->GetValue() )
			pItem->role = am::iHigh;
		else
			pItem->role = am::iLow;

		for ( std::pair<wxString, wxString>& it : GetShortAttributes() )
		{
			pItem->mShortAttributes.insert({ it.first, it.second });
		}

		for ( std::pair<wxString, wxString>& it : GetLongAttributes() )
		{
			pItem->mLongAttributes.insert({ it.first, it.second });
		}

		pItem->image = m_imagePanel->GetImage();

		Hide();
		am::GetMainFrame()->Enable();
		am::AddStoryElement(pItem, true);
	}
	else
	{
		wxMessageBox("You can't create an item with no name!");
		return;
	}

	Destroy();
}

void amItemCreator::OnClose(wxCloseEvent& event)
{
	if ( niGeneral->IsModified() || niName->IsModified() || niAppearance->IsModified() ||
		niBackstory->IsModified() || niOrigin->IsModified() || niUsage->IsModified() )
	{
		wxMessageDialog check(this, "Are you sure you want to close?", "Close", wxYES_NO | wxNO_DEFAULT);

		if ( check.ShowModal() != wxID_YES )
			return;
	}

	am::GetMainFrame()->Enable(true);
	event.Skip();
}

void amItemCreator::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(m_panel1);

	wxRect rect;
	dc.SetPen(wxPen(wxColour(230, 60, 60), 3));
	dc.SetBackgroundMode(wxTRANSPARENT);

	rect = niGeneral->GetRect();
	dc.DrawLine(wxPoint(rect.x, rect.y - 3), wxPoint(rect.x + rect.width, rect.y - 3));

	rect = niAppearance->GetRect();
	dc.DrawLine(wxPoint(rect.x, rect.y - 3), wxPoint(rect.x + rect.width, rect.y - 3));

	rect = niOrigin->GetRect();
	dc.DrawLine(wxPoint(rect.x, rect.y - 3), wxPoint(rect.x + rect.width, rect.y - 3));

	rect = niBackstory->GetRect();
	dc.DrawLine(wxPoint(rect.x, rect.y - 3), wxPoint(rect.x + rect.width, rect.y - 3));

	rect = niUsage->GetRect();
	dc.DrawLine(wxPoint(rect.x, rect.y - 3), wxPoint(rect.x + rect.width, rect.y - 3));

	dc.SetPen(wxPen(wxColour(230, 60, 60), 5));

	rect = meaLabel->GetRect();
	dc.DrawLine(wxPoint(rect.x + 3, rect.y + rect.height + 7),
		wxPoint(rect.x - 3 + rect.width, rect.y + rect.height + 7));

	dc.SetPen(wxPen(wxColour(200, 200, 200), 5));

	rect = wxRect(niMeaSizer->GetPosition(), niMeaSizer->GetSize());
	rect.x -= 7;
	rect.y -= 7;
	rect.width += 14;

	wxPoint points[]{ rect.GetTopLeft(), rect.GetTopRight(), rect.GetBottomRight(), rect.GetBottomLeft(), rect.GetTopLeft() };
	dc.DrawLines(5, points);

	event.Skip();
}
