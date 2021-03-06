#include "CharacterCreator.h"
#include "MyApp.h"

#include "ElementsNotebook.h"
#include "CharacterShowcase.h"

#include "Outline.h"
#include "OutlineFiles.h"

#include <wx\listctrl.h>

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(CharacterCreator, wxFrame)

EVT_BUTTON(BUTTON_CharImage, CharacterCreator::SetImage)
EVT_BUTTON(BUTTON_CharRemoveImage, CharacterCreator::RemoveImage)

EVT_CLOSE(CharacterCreator::CheckClose)

END_EVENT_TABLE()

using std::string;
using std::vector;

CharacterCreator::CharacterCreator(wxWindow* parent, amdElementsNotebook* notebook,
    long id, const string& label, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, label, pos, size
    , style) {
    
    this->mainFrame = (amdMainFrame*)(wxGetApp().GetTopWindow());
    this->notebook = notebook;
    this->CenterOnParent();

    wxColour dark(60, 60, 60);
    wxColour darker(40, 40, 40);
    wxColour txtCol(255, 255, 255);

    SetBackgroundColour(darker);

    m_panel1 = new wxPanel(this, wxID_ANY);
    m_panel1->SetBackgroundColour(darker);
    m_panel1->Show(true);

    wxString choice[] = { "Male", "Female" };

    wxFont font10(wxFontInfo(10));

    ncFullName = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(70, 10), wxSize(420, 25),
        wxBORDER_SIMPLE);
    ncFullName->SetBackgroundColour(dark);
    ncFullName->SetForegroundColour(wxColour(250, 250, 250));
    ncFullName->SetFont(font10);
    wxStaticText* label1 = new wxStaticText(m_panel1, wxID_ANY, "Name:", wxPoint(10, 10), wxSize(50, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label1->SetFont(wxFont(wxFontInfo(12)));
    label1->SetBackgroundColour(darker);
    label1->SetForegroundColour(txtCol);

    wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
    firstLine->Add(label1, wxSizerFlags(0).Border(wxRIGHT, 10));
    firstLine->Add(ncFullName, wxSizerFlags(1));

    ncMale = new wxRadioButton(m_panel1, wxID_ANY, "Male", wxPoint(60, 40), wxSize(60, 20), wxRB_GROUP);
    ncMale->SetFont(font10);
    ncMale->SetForegroundColour(wxColour(245, 245, 245));
    ncFemale = new wxRadioButton(m_panel1, wxID_ANY, "Female", wxPoint(60, 60), wxSize(60, 20));
    ncFemale->SetFont(font10);
    ncFemale->SetForegroundColour(wxColour(245, 245, 245));

    wxBoxSizer* btnHolderSizer1 = new wxBoxSizer(wxVERTICAL);
    btnHolderSizer1->Add(ncMale, wxSizerFlags(0).Left());
    btnHolderSizer1->Add(ncFemale, wxSizerFlags(0).Left());

    wxStaticText* label2 = new wxStaticText(m_panel1, wxID_ANY, "Sex:", wxPoint(10, 50), wxSize(40, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label2->SetBackgroundColour(darker);
    label2->SetForegroundColour(txtCol);
    label2->SetFont(wxFont(wxFontInfo(13)));

    ncHeight = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(75, 90), wxSize(80, 25),
        wxBORDER_SIMPLE);
    ncHeight->SetBackgroundColour(dark);
    ncHeight->SetForegroundColour(wxColour(250, 250, 250));
    ncHeight->SetFont(font10);
    wxStaticText* label3 = new wxStaticText(m_panel1, wxID_ANY, "Height: ", wxPoint(10, 90), wxSize(55, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label3->SetBackgroundColour(darker);
    label3->SetForegroundColour(txtCol);
    label3->SetFont(wxFont(wxFontInfo(12)));

    ncNationality = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(335, 50), wxSize(-1, 25),
        wxBORDER_SIMPLE);
    ncNationality->SetBackgroundColour(dark);
    ncNationality->SetForegroundColour(wxColour(250, 250, 250));
    ncNationality->SetFont(font10);
    wxStaticText* label4 = new wxStaticText(m_panel1, wxID_ANY, "Nationality: ", wxPoint(240, 50), wxDefaultSize,
        wxNO_BORDER | wxALIGN_LEFT);
    label4->SetBackgroundColour(darker);
    label4->SetForegroundColour(txtCol);
    label4->SetFont(wxFont(wxFontInfo(12)));

    wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
    secondLine->Add(label2, wxSizerFlags(0).CenterVertical().Border(wxRIGHT, 7));
    secondLine->Add(btnHolderSizer1, wxSizerFlags(0).CenterVertical().Border(wxRIGHT, 8));
    secondLine->Add(label3, wxSizerFlags(0).CenterVertical().Border(wxRIGHT, 5));
    secondLine->Add(ncHeight, wxSizerFlags(0).CenterVertical().Border(wxRIGHT, 15));
    secondLine->Add(label4, wxSizerFlags(0).CenterVertical().Border(wxLEFT, 7));
    secondLine->Add(ncNationality, wxSizerFlags(1).CenterVertical().Border(wxRIGHT, 8));

    ncAge = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(180, 50), wxSize(50, 25),
        wxBORDER_SIMPLE);
    ncAge->SetBackgroundColour(dark);
    ncAge->SetForegroundColour(wxColour(250, 250, 250));
    ncAge->SetFont(font10);
    wxStaticText* label5 = new wxStaticText(m_panel1, wxID_ANY, "Age:", wxPoint(130, 50), wxSize(40, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label5->SetBackgroundColour(darker);
    label5->SetForegroundColour(txtCol);
    label5->SetFont(wxFont(wxFontInfo(12)));

    ncNickname = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(235, 90), wxSize(-1, 25),
        wxBORDER_SIMPLE);
    ncNickname->SetBackgroundColour(dark);
    ncNickname->SetForegroundColour(wxColour(250, 250, 250));
    ncNickname->SetFont(font10);
    wxStaticText* label6 = new wxStaticText(m_panel1, wxID_ANY, "Nickname: ", wxPoint(145, 90), wxDefaultSize,
        wxNO_BORDER | wxALIGN_LEFT);
    label6->SetBackgroundColour(darker);
    label6->SetForegroundColour(txtCol);
    label6->SetFont(wxFont(wxFontInfo(12)));

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

    wxStaticText* label7 = new wxStaticText(m_panel1, wxID_ANY, "Role: ", wxPoint(345, 90), wxSize(50, 25),
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
    thirdLine->Add(label5, wxSizerFlags(0).CenterVertical().Border(wxRIGHT, 8));
    thirdLine->Add(ncAge, wxSizerFlags(0).CenterVertical().Border(wxRIGHT, 16)); 
    thirdLine->Add(label6, wxSizerFlags(0).CenterVertical());
    thirdLine->Add(ncNickname, wxSizerFlags(1).CenterVertical().Border(wxRIGHT, 16));
    thirdLine->Add(label7, wxSizerFlags(0).CenterVertical());
    thirdLine->Add(btnHolderSizer2, wxSizerFlags(0).CenterVertical());

    label8 = new wxStaticText(m_panel1, wxID_ANY, "Appearance", wxPoint(15, 145), wxSize(475, -1),
        wxBORDER_SIMPLE | wxALIGN_LEFT);
    label8->SetBackgroundColour(wxColour(230, 0, 20));
    label8->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncAppearance = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(15, 170), wxSize(475, 85),
        wxTE_MULTILINE | wxBORDER_SIMPLE);
    ncAppearance->SetBackgroundColour(dark);
    ncAppearance->SetForegroundColour(wxColour(250, 250, 250));

    wxStaticText* label9 = new wxStaticText(m_panel1, wxID_ANY, "Personality", wxPoint(15, 265), wxSize(475, -1),
        wxBORDER_SIMPLE | wxALIGN_LEFT);
    label9->SetBackgroundColour(wxColour(230, 0, 20));
    label9->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncPersonality = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(15, 290), wxSize(475, 85),
        wxTE_MULTILINE | wxBORDER_SIMPLE);
    ncPersonality->SetBackgroundColour(dark);
    ncPersonality->SetForegroundColour(wxColour(250, 250, 250));

    wxStaticText* label10 = new wxStaticText(m_panel1, wxID_ANY, "Backstory", wxDefaultPosition, wxDefaultSize,
        wxBORDER_SIMPLE | wxALIGN_LEFT);
    label10->SetBackgroundColour(wxColour(230, 0, 20));
    label10->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncBackstory = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(15, 410), wxSize(475, 85),
        wxTE_MULTILINE | wxBORDER_SIMPLE);
    ncBackstory->SetBackgroundColour(dark);
    ncBackstory->SetForegroundColour(wxColour(250, 250, 250));

    wxPanel* btnPanel = new wxPanel(this);
    btnPanel->SetBackgroundColour(darker);

    ncNext = new wxButton(btnPanel, BUTTON_NextChar1, "Next", wxPoint(300, 505), wxSize(90, 30));
    ncBack = new wxButton(btnPanel, BUTTON_BackChar1, "Back", wxPoint(15, 505), wxSize(90, 30));
    ncCancel = new wxButton(btnPanel, BUTTON_CancelChar, "Cancel", wxPoint(400, 505), wxSize(90, 30));

    ncNext->Bind(wxEVT_BUTTON, &CharacterCreator::Next, this);
    ncBack->Bind(wxEVT_BUTTON, &CharacterCreator::Back, this);
    ncCancel->Bind(wxEVT_BUTTON, &CharacterCreator::Cancel, this);

    ncBack->Hide();

    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(ncBack, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 10));
    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(ncNext, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 10));
    btnSizer->AddSpacer(10);
    btnSizer->Add(ncCancel, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 10));

    btnPanel->SetSizer(btnSizer);

    wxBoxSizer* verSizer1 = new wxBoxSizer(wxVERTICAL);
    verSizer1->Add(firstLine, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT | wxTOP, 10));
    verSizer1->Add(secondLine, wxSizerFlags(0).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    verSizer1->Add(thirdLine, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 10));
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
    this->Layout();

    m_panel2 = new wxScrolledWindow(this);
    m_panel2->Hide();
    m_panel2->SetBackgroundColour(darker);

    wxStaticText* customLabel = new wxStaticText(m_panel2, -1, "Custom attributes");
    customLabel->SetFont(wxFontInfo(14).Bold());
    customLabel->SetForegroundColour(wxColour(240, 240, 240));

    ncAddCustom = new wxButton(m_panel2, -1, "", wxDefaultPosition, wxSize(25, 25));
    ncAddCustom->SetBitmap(wxBITMAP_PNG(plus));
    ncAddCustom->Bind(wxEVT_BUTTON, &CharacterCreator::AddCustomAttr, this);

    wxBoxSizer* customHorSiz = new wxBoxSizer(wxHORIZONTAL);
    customHorSiz->Add(customLabel, wxSizerFlags(1));
    customHorSiz->AddStretchSpacer(1);
    customHorSiz->Add(ncAddCustom, wxSizerFlags(0));

    m_customSizer = new wxBoxSizer(wxVERTICAL);
    m_customSizer->Add(customHorSiz, wxSizerFlags(0).Expand().Border(wxALL, 10));

    m_panel2->SetSizer(m_customSizer);
    m_panel2->FitInside();
    m_panel2->SetScrollRate(20, 20);
        
    m_panel3 = new wxPanel(this);
    m_panel3->Hide();
    m_panel3->SetBackgroundColour(darker);

    ncChooseImage = new wxButton(m_panel3, BUTTON_CharImage, "Choose image file", wxPoint(285, 415), wxSize(150, 30));
    ncRemoveImage = new wxButton(m_panel3, BUTTON_CharRemoveImage, "Remove", wxPoint(75, 415), wxSize(100, 30));
    ncRemoveImage->Hide();
    ncImagePanel = new ImagePanel(m_panel3, wxPoint(75, 40), wxSize(360, 360));
    ncImagePanel->setBorderColour(darker);

    wxBoxSizer* btnSizer2 = new wxBoxSizer(wxHORIZONTAL);
    btnSizer2->Add(ncRemoveImage, wxSizerFlags(0));
    btnSizer2->AddStretchSpacer(1);
    btnSizer2->Add(ncChooseImage, wxSizerFlags(0));

    wxBoxSizer* verSizer3 = new wxBoxSizer(wxVERTICAL);
    verSizer3->Add(ncImagePanel, wxSizerFlags(0).Border(wxTOP, 40).CenterHorizontal());
    verSizer3->AddStretchSpacer(1);
    verSizer3->Add(btnSizer2, wxSizerFlags(0).Expand());
    verSizer3->AddStretchSpacer(3);

    m_panel3->SetSizer(verSizer3);

    mainSiz = new wxBoxSizer(wxVERTICAL);
    mainSiz->Add(m_panel1, wxSizerFlags(1).Expand());
    mainSiz->Add(m_panel2, wxSizerFlags(1).Expand());
    mainSiz->Add(m_panel3, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 75));
    mainSiz->Add(btnPanel, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));

    SetSizer(mainSiz);
}

vector<string> CharacterCreator::GetValues() {
    vector<string> vec;

    vec.push_back(ncFullName->GetValue().ToStdString());

    if (ncMale->GetValue()) {
        vec.push_back(ncMale->GetLabel().ToStdString());
    } else {
        vec.push_back(ncFemale->GetLabel().ToStdString());
    }

    vec.push_back(ncAge->GetValue().ToStdString());
    vec.push_back(ncNationality->GetValue().ToStdString());
    vec.push_back(ncHeight->GetValue().ToStdString());
    vec.push_back(ncNickname->GetValue().ToStdString());
    
    if (ncMain->GetValue())
        vec.push_back("Protagonist");
    else if (ncSupp->GetValue())
        vec.push_back("Supporting");
    else if (ncVillian->GetValue())
        vec.push_back("Villian");
    else if (ncSecon->GetValue())
        vec.push_back("Secondary");

    vec.push_back(ncAppearance->GetValue().ToStdString());
    vec.push_back(ncPersonality->GetValue().ToStdString());
    vec.push_back(ncBackstory->GetValue().ToStdString());

    return vec;
}

vector<pair<string, string>> CharacterCreator::GetCustom() {
    vector<pair<string, string>> vec;

    for (auto it : ncCustom) {
        pair<string, string> pair(it.first->GetValue(), it.second->GetValue());
        vec.push_back(pair);
    }

    return vec;
}

void CharacterCreator::SetEdit(Character* character) {
    ncNext->SetId(BUTTON_NextEdit1);
    ncBack->SetId(BUTTON_BackEdit1);

    ncFullName->SetValue(character->name);

    if (character->sex == "Female") {
        ncFemale->SetValue(true);
    }
    ncAge->SetValue(character->age);
    ncNationality->SetValue(character->nat);
    ncHeight->SetValue(character->height);
    ncNickname->SetValue(character->nick);

    switch (character->role) {
    case Protagonist:
        ncMain->SetValue(true);
        break;
    case Supporting:
        ncSupp->SetValue(true);
        break;
    case Villian:
        ncVillian->SetValue(true);
        break;
    case Secondary:
        ncSecon->SetValue(true);
        break;
    default:
        break;
    }

    ncAppearance->SetValue(character->appearance);
    ncPersonality->SetValue(character->personality);
    ncBackstory->SetValue(character->backstory);

    int i = 0;
    for (auto it : character->custom) {
        AddCustomAttr(wxCommandEvent());
        ncCustom[i].first->SetValue(it.first);
        ncCustom[i].second->SetValue(it.second);
    
        i++;
    }

    ncImage = character->image;

    if (ncImage.IsOk()) {
        ncImagePanel->SetImage(ncImage);
        ncRemoveImage->Show();
    }

    charEdit = character;

    this->Show(true);
}

void CharacterCreator::DoEdit(wxCommandEvent& event) {
    mainFrame->getOutline()->GetOutlineFiles()->DeleteCharacter(*charEdit);

    vector<string>& vec = GetValues();

    Role newRole;
    if (vec[6] == "Protagonist")
        newRole = Protagonist;
    else if (vec[6] == "Supporting")
        newRole = Supporting;
    else if (vec[6] == "Secondary")
        newRole = Secondary;
    else if (vec[6] == "Villian")
        newRole = Villian;

    bool dif = charEdit->name != vec[0] || charEdit->role != newRole;
    if (dif) {
        notebook->removeCharName(charEdit->name.ToStdString());
        mainFrame->characters.remove(*charEdit);
        charEdit = &Character();
    }

    charEdit->name = vec[0];
    charEdit->sex = vec[1];
    charEdit->age = vec[2];
    charEdit->nat = vec[3];
    charEdit->height = vec[4];
    charEdit->nick = vec[5];
    charEdit->role = newRole;
    charEdit->appearance = vec[7];
    charEdit->personality = vec[8];
    charEdit->backstory = vec[9];

    charEdit->image = ncImage;

    charEdit->custom = GetCustom();

    if (dif) {
        notebook->AddCharName(charEdit->name.ToStdString());
        amdMainFrame::characters.push_back(*charEdit);
        amdMainFrame::characters.sort();
        notebook->setSearchAC(wxBookCtrlEvent());
        notebook->UpdateAll();
    }

    int n = 0;
    amdElementsNotebook::m_charList->Select(-1);
    for (auto it : amdMainFrame::characters) {
        if (it == *charEdit)
            amdElementsNotebook::m_charList->RefreshItem(n++);
    }

    notebook->m_charShow->setData(*charEdit);

    mainFrame->isSaved = false;
    mainFrame->getOutline()->GetOutlineFiles()->AppendCharacter(*charEdit);
    mainFrame->Enable();

    this->Destroy();

    event.Skip();
}

void CharacterCreator::AddCustomAttr(wxCommandEvent& event) {
    Freeze();
    wxSize size(label8->GetSize());

    wxTextCtrl* label = new wxTextCtrl(m_panel2, -1, "Title",
        wxDefaultPosition, wxSize(-1, size.y), wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    wxTextCtrl* content = new wxTextCtrl(m_panel2, -1, "", wxDefaultPosition,
        wxSize(-1, ncAppearance->GetSize().y), wxTE_MULTILINE | wxBORDER_SIMPLE);

    content->SetBackgroundColour(wxColour(60, 60, 60));
    content->SetForegroundColour(wxColour(255, 255, 255));
    label->SetBackgroundColour(wxColour(230, 0, 20));
    label->SetFont(wxFontInfo(13).Bold());

    wxButton* minus = new wxButton(m_panel2, -1, "", wxDefaultPosition, wxSize(size.y, size.y));
    minus->SetBitmap(wxBITMAP_PNG(minus));
    minus->Bind(wxEVT_BUTTON, &CharacterCreator::RemoveCustomAttr, this);
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(label, wxSizerFlags(1));
    topSizer->Add(minus, 0);

    m_customSizer->Add(topSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 15));
    m_customSizer->Add(content, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));

    m_customSizer->FitInside(m_panel2);

    ncCustom.push_back(pair<wxTextCtrl*, wxTextCtrl*>(label, content));
    minusButtons.push_back(minus);

    Thaw();
}

void CharacterCreator::RemoveCustomAttr(wxCommandEvent& event) {
    Freeze();
    wxButton* minus = (wxButton*)event.GetEventObject();

    auto it1 = ncCustom.begin();
    auto it2 = minusButtons.begin();

    for (it2 = minusButtons.begin(); it2 != minusButtons.end(); it2++) {
        if (*it2 == minus)
            break;

        it1++;
    }

    it1->first->Destroy();
    it1->second->Destroy();
    ncCustom.erase(it1);

    (*it2)->Destroy();
    minusButtons.erase(it2);

    mainSiz->Layout();

    Thaw();
}

void CharacterCreator::Cancel(wxCommandEvent& event) {
    mainFrame->Enable();
    this->Destroy();
    event.Skip();
}

void CharacterCreator::Next(wxCommandEvent& event) {
    bool show1 = false, show2 = false, show3 = false;

    switch (event.GetId()) {
    case BUTTON_NextChar1:
        ncNext->SetId(BUTTON_NextChar2);
        ncBack->Show();
        show2 = true;
        break;

    case BUTTON_NextChar2:
        ncNext->SetId(BUTTON_CreateChar);
        ncNext->SetLabel("Create");
        ncBack->SetId(BUTTON_BackChar2);
        show3 = true;
        break;

    case BUTTON_NextEdit1:
        ncNext->SetId(BUTTON_NextEdit2);
        ncBack->Show();
        show2 = true;
        break;

    case BUTTON_NextEdit2:
        ncNext->SetId(BUTTON_CreateEdit);
        ncNext->SetLabel("Ok");
        ncBack->SetId(BUTTON_BackEdit2);
        show3 = true;
        break;

    case BUTTON_CreateChar:
        Create(event);
        return;

    case BUTTON_CreateEdit:
        DoEdit(event);
        return;
    }

    m_panel1->Show(show1);
    m_panel2->Show(show2);
    m_panel3->Show(show3);

    mainSiz->Layout();
    event.Skip();
}

void CharacterCreator::Back(wxCommandEvent& event) {
    bool show1 = false, show2 = false, show3 = false;

    switch (event.GetId()) {
    case BUTTON_BackChar1:
        ncNext->SetId(BUTTON_NextChar1);
        ncBack->Hide();
        show1 = true;
        break;

    case BUTTON_BackChar2:
        ncNext->SetLabel("Next");
        ncNext->SetId(BUTTON_NextChar2);
        ncBack->SetId(BUTTON_BackChar1);
        show2 = true;
        break;

    case BUTTON_BackEdit1:
        ncNext->SetId(BUTTON_NextEdit1);
        ncBack->Hide();
        show1 = true;
        break;

    case BUTTON_BackEdit2:
        ncNext->SetId(BUTTON_NextEdit2);
        ncNext->SetLabel("Next");
        ncBack->SetId(BUTTON_BackEdit1);
        show2 = true;
        break;
    }

    m_panel1->Show(show1);
    m_panel2->Show(show2);
    m_panel3->Show(show3);

    mainSiz->Layout();
    event.Skip();
}

void CharacterCreator::SetImage(wxCommandEvent& event) {
    wxFileDialog choose(this, "Select an image", wxEmptyString, wxEmptyString,
        "JPG, JPEG and PNG(*.jpg;*.jpeg;*.png)|*.jpg;*.jpeg;*.png",
        wxFD_OPEN, wxDefaultPosition);

    if (choose.ShowModal() == wxID_OK) {
        ncImage.LoadFile(choose.GetPath(), wxBITMAP_TYPE_ANY);
        ncImagePanel->SetImage(ncImage);
        ncRemoveImage->Show();
    }

    mainSiz->Layout();
    event.Skip();
}

void CharacterCreator::RemoveImage(wxCommandEvent& event) {
    ncImage.Destroy();
    ncImagePanel->SetBackgroundBitmap(ncImage);
    ncImagePanel->Refresh(true);
    ncRemoveImage->Hide();
    mainSiz->Layout();

    event.Skip();
}

void CharacterCreator::Create(wxCommandEvent& event) {
    if (!ncFullName->IsEmpty()) {
        vector<string>& vec = GetValues();

        Character character;
        character.name = vec[0];
        character.sex = vec[1];
        character.age = vec[2];
        character.nat = vec[3];
        character.height = vec[4];
        character.nick = vec[5];

        if (vec[6] == "Protagonist")
            character.role = Protagonist;
        else if (vec[6] == "Supporting")
            character.role = Supporting;
        else if (vec[6] == "Villian")
            character.role = Villian;
        else if (vec[6] == "Secondary")
            character.role = Secondary;

        character.appearance = vec[7];
        character.personality = vec[8];
        character.backstory = vec[9];

        character.image = ncImage;

        character.custom = GetCustom();

        mainFrame->characters.push_back(character);
        mainFrame->characters.sort();
        mainFrame->updateElements(wxCommandEvent());

        notebook->AddCharName(vec[0]);
        notebook->setSearchAC(wxBookCtrlEvent());

        mainFrame->saved[0]++;
        mainFrame->isSaved = false;
        mainFrame->getOutline()->GetOutlineFiles()->AppendCharacter(character);
    } else {
        wxMessageBox("You can't create a character without a name.");
        return;
    }

    mainFrame->Enable();
    this->Destroy();
    event.Skip();
}

void CharacterCreator::CheckClose(wxCloseEvent& event) {

    if (ncFullName->IsModified() || ncAge->IsModified() || ncNationality->IsModified() ||
        ncHeight->IsModified() || ncNickname->IsModified() ||
        ncAppearance->IsModified() || ncPersonality->IsModified() || ncBackstory->IsModified()) {

        wxMessageDialog* check = new wxMessageDialog(this, "Are you sure you want to close?",
            "Close window", wxYES_NO | wxNO_DEFAULT);

        switch (check->ShowModal()) {
        case wxID_YES:
            mainFrame->Enable();
            this->Destroy();
            break;

        case wxID_NO:
            break;

            check->Destroy();
        }

    } else {
        mainFrame->Enable();
        this->Destroy();
    }
    event.Skip();
}