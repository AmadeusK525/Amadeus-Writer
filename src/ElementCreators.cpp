#include "ElementCreators.h"

#include "MainFrame.h"
#include "ElementsNotebook.h"
#include "Outline.h"
#include "OutlineFiles.h"

#include <wx\sizer.h>
#include <wx\stattext.h>
#include <wx\statline.h>

BEGIN_EVENT_TABLE(amdElementCreator, wxFrame)

EVT_BUTTON(BUTTON_SetImage, amdElementCreator::SetImage)
EVT_BUTTON(BUTTON_RemoveImage, amdElementCreator::RemoveImage)

EVT_CLOSE(amdElementCreator::CheckClose)

END_EVENT_TABLE()

amdElementCreator::amdElementCreator(wxWindow* parent, amdProjectManager* manager,
    long id, const string& label, const wxPoint& pos, const wxSize& size, long style) :
    wxFrame(parent, id, label, pos, size, style) {

    m_manager = manager;
    CenterOnParent();

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
    m_addCustom->Bind(wxEVT_BUTTON, &amdElementCreator::AddCustomAttr, this);

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

    m_next->Bind(wxEVT_BUTTON, &amdElementCreator::Next, this);
    m_back->Bind(wxEVT_BUTTON, &amdElementCreator::Back, this);
    m_cancel->Bind(wxEVT_BUTTON, &amdElementCreator::Cancel, this);

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
}

vector<pair<string, string>> amdElementCreator::GetCustom() {
    vector<pair<string, string>> vec;

    for (auto& it : m_custom) {
        pair<string, string> pair(it.first->GetValue(), it.second->GetValue());
        vec.push_back(pair);
    }

    return vec;
}

void amdElementCreator::RemoveCustomAttr(wxCommandEvent& event) {
    Freeze();
    wxButton* minus = (wxButton*)event.GetEventObject();

    auto it1 = m_custom.begin();
    auto it2 = m_minusButtons.begin();

    for (it2 = m_minusButtons.begin(); it2 != m_minusButtons.end(); it2++) {
        if (*it2 == minus)
            break;

        it1++;
    }

    it1->first->Destroy();
    it1->second->Destroy();
    m_custom.erase(it1);

    (*it2)->Destroy();
    m_minusButtons.erase(it2);

    m_mainSizer->Layout();

    Thaw();
}

void amdElementCreator::Cancel(wxCommandEvent& event) {
    m_manager->GetMainFrame()->Enable();
    this->Destroy();
    event.Skip();
}

void amdElementCreator::Next(wxCommandEvent& event) {
    bool show1 = false, show2 = false, show3 = false;

    switch (event.GetId()) {
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
        Create(event);
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

void amdElementCreator::Back(wxCommandEvent& event) {
    bool show1 = false, show2 = false, show3 = false;

    switch (event.GetId()) {
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

void amdElementCreator::SetImage(wxCommandEvent& event) {
    wxFileDialog choose(this, "Select an image", wxEmptyString, wxEmptyString,
        "JPG, JPEG and PNG(*.jpg;*.jpeg;*.png)|*.jpg;*.jpeg;*.png",
        wxFD_OPEN, wxDefaultPosition);

    if (choose.ShowModal() == wxID_OK) {
        wxImage image;
        image.LoadFile(choose.GetPath(), wxBITMAP_TYPE_ANY);
        m_imagePanel->SetImage(image);
        m_removeImage->Show();
    }

    m_mainSizer->Layout();
    event.Skip();
}

void amdElementCreator::RemoveImage(wxCommandEvent& event) {
    m_imagePanel->SetImage(wxImage());
    m_removeImage->Hide();
    m_mainSizer->Layout();

    event.Skip();
}


///////////////////////////////////////////////////////////////////////////////
////////////////////////////// Character creator //////////////////////////////
///////////////////////////////////////////////////////////////////////////////


amdCharacterCreator::amdCharacterCreator(wxWindow* parent, amdProjectManager* manager,
    long id, const string& label, const wxPoint& pos, const wxSize& size, long style) :
    amdElementCreator(parent, manager, id, label, pos, size, style) {

    wxColour dark(50, 50, 50);
    wxColour darker(30, 30, 30);
    wxColour txtCol(255, 255, 255);

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
    Layout();
}

vector<string> amdCharacterCreator::GetValues() {
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

void amdCharacterCreator::SetEdit(Element* editChar) {
    Character* character = dynamic_cast<Character*>(editChar);
    if (!character) {
        wxMessageBox("There was an unexpected error. Editing could not be start.");
        Destroy();
        return;
    }

    m_next->SetId(BUTTON_NextEdit1);
    m_back->SetId(BUTTON_BackEdit1);

    ncFullName->SetValue(character->name);

    if (character->sex == "Female") {
        ncFemale->SetValue(true);
    }
    ncAge->SetValue(character->age);
    ncNationality->SetValue(character->nat);
    ncHeight->SetValue(character->height);
    ncNickname->SetValue(character->nick);

    switch (character->role) {
    case cProtagonist:
        ncMain->SetValue(true);
        break;
    case cSupporting:
        ncSupp->SetValue(true);
        break;
    case cVillian:
        ncVillian->SetValue(true);
        break;
    case cSecondary:
        ncSecon->SetValue(true);
        break;
    default:
        break;
    }

    ncAppearance->SetValue(character->appearance);
    ncPersonality->SetValue(character->personality);
    ncBackstory->SetValue(character->backstory);

    int i = 0;
    for (auto& it : character->custom) {
        AddCustomAttr(wxCommandEvent());
        m_custom[i].first->SetValue(it.first);
        m_custom[i].second->SetValue(it.second);

        i++;
    }

    wxImage& image = m_imagePanel->GetImage();
    image = character->image;
    
    m_removeImage->Show(m_imagePanel->SetImage(image));

    m_elementEdit = character;
    SetTitle("Edit character - '" + m_elementEdit->name + "'");
}

void amdCharacterCreator::DoEdit(wxCommandEvent& WXUNUSED(event)) {
    vector<string>& vec = GetValues();

    Role newRole;
    if (vec[6] == "Protagonist")
        newRole = cProtagonist;
    else if (vec[6] == "Supporting")
        newRole = cSupporting;
    else if (vec[6] == "Secondary")
        newRole = cSecondary;
    else if (vec[6] == "Villian")
        newRole = cVillian;

    bool dif = m_elementEdit->name != vec[0] || m_elementEdit->role != newRole;

    Character character;

    character.name = vec[0];
    character.sex = vec[1];
    character.age = vec[2];
    character.nat = vec[3];
    character.height = vec[4];
    character.nick = vec[5];
    character.role = newRole;
    character.appearance = vec[7];
    character.personality = vec[8];
    character.backstory = vec[9];

    character.image = m_imagePanel->GetImage();

    character.custom = GetCustom();
    character.chapters = m_elementEdit->chapters;

    Character* pCharacterToEdit = (Character*)m_elementEdit;

    m_manager->EditCharacter(*pCharacterToEdit, character, dif);
    Destroy();
}

void amdCharacterCreator::AddCustomAttr(wxCommandEvent& event) {
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
    minus->Bind(wxEVT_BUTTON, &amdCharacterCreator::RemoveCustomAttr, this);

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

void amdCharacterCreator::Create(wxCommandEvent& event) {
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
            character.role = cProtagonist;
        else if (vec[6] == "Supporting")
            character.role = cSupporting;
        else if (vec[6] == "Villian")
            character.role = cVillian;
        else if (vec[6] == "Secondary")
            character.role = cSecondary;

        character.appearance = vec[7];
        character.personality = vec[8];
        character.backstory = vec[9];

        character.image = m_imagePanel->GetImage();

        character.custom = GetCustom();

        m_manager->AddCharacter(character);
    } else {
        wxMessageBox("You can't create a character without a name.");
        return;
    }

    m_manager->GetMainFrame()->Enable();
    this->Destroy();
    event.Skip();
}

void amdCharacterCreator::CheckClose(wxCloseEvent& event) {

    if (ncFullName->IsModified() || ncAge->IsModified() || ncNationality->IsModified() ||
        ncHeight->IsModified() || ncNickname->IsModified() ||
        ncAppearance->IsModified() || ncPersonality->IsModified() || ncBackstory->IsModified()) {

        wxMessageDialog check(this, "Are you sure you want to close?",
            "Close window", wxYES_NO | wxNO_DEFAULT);

        switch (check.ShowModal()) {
        case wxID_YES:
            m_manager->GetMainFrame()->Enable();
            this->Destroy();
            break;

        case wxID_NO:
            break;

            check.Destroy();
        }

    } else {
        m_manager->GetMainFrame()->Enable();
        this->Destroy();
    }
    event.Skip();
}


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Location Creator ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


amdLocationCreator::amdLocationCreator(wxWindow* parent, amdProjectManager* manager,
    long id, const string& label, const wxPoint& pos, const wxSize& size, long style) :
    amdElementCreator(parent, manager, id, label, pos, size, style) {

    wxColour dark(50, 50, 50);
    wxColour darker(30, 30, 30);
    wxColour grey(200, 200, 200);
    wxColour red(230, 0, 20);
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

    wxStaticText* label8 = new wxStaticText(m_panel1, wxID_ANY, "Importance:",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_SIMPLE);
    label8->SetBackgroundColour(grey);
    label8->SetFont(wxFont(wxFontInfo(12).Bold()));

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
    imSizer->Add(label8, wxSizerFlags(0).CenterVertical());
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

    wxStaticText* label2 = new wxStaticText(m_panel1, wxID_ANY, "General",
        wxPoint(10, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label2->SetBackgroundColour(red);
    label2->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlGeneral = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(10, 95),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlGeneral->SetBackgroundColour(dark);
    nlGeneral->SetForegroundColour(txtCol);
    nlGeneral->SetFont(font);

    wxStaticText* label3 = new wxStaticText(m_panel1, wxID_ANY, "Natural characteristics",
        wxPoint(220, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label3->SetBackgroundColour(grey);
    label3->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlNatural = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(220, 95),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlNatural->SetBackgroundColour(dark);
    nlNatural->SetForegroundColour(txtCol);
    nlNatural->SetFont(font);

    wxStaticText* label4 = new wxStaticText(m_panel1, wxID_ANY, "Architecture",
        wxPoint(430, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label4->SetBackgroundColour(wxColor(230, 0, 20, 0));
    label4->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlArchitecture = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(430, 95),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlArchitecture->SetBackgroundColour(dark);
    nlArchitecture->SetForegroundColour(txtCol);
    nlArchitecture->SetFont(font);

    wxStaticText* label5 = new wxStaticText(m_panel1, wxID_ANY, "Politics",
        wxPoint(10, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label5->SetBackgroundColour(grey);
    label5->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlPolitics = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlPolitics->SetBackgroundColour(dark);
    nlPolitics->SetForegroundColour(txtCol);
    nlPolitics->SetFont(font);

    label6 = new wxStaticText(m_panel1, wxID_ANY, "Economy",
        wxPoint(220, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label6->SetBackgroundColour(red);
    label6->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlEconomy = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(220, 335),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlEconomy->SetBackgroundColour(dark);
    nlEconomy->SetForegroundColour(txtCol);
    nlEconomy->SetFont(font);

    wxStaticText* label7 = new wxStaticText(m_panel1, wxID_ANY, "Culture",
        wxPoint(430, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label7->SetBackgroundColour(grey);
    label7->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlCulture = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxPoint(430, 335),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlCulture->SetBackgroundColour(dark);
    nlCulture->SetForegroundColour(txtCol);
    nlCulture->SetFont(font);

    wxBoxSizer* firstColumn = new wxBoxSizer(wxVERTICAL);
    firstColumn->Add(label2, wxSizerFlags(0).Expand());
    firstColumn->Add(nlGeneral, wxSizerFlags(1).Expand());
    firstColumn->AddSpacer(10);
    firstColumn->Add(label5, wxSizerFlags(0).Expand());
    firstColumn->Add(nlPolitics, wxSizerFlags(1).Expand());

    wxBoxSizer* secondColumn = new wxBoxSizer(wxVERTICAL);
    secondColumn->Add(label3, wxSizerFlags(0).Expand());
    secondColumn->Add(nlNatural, wxSizerFlags(1).Expand());
    secondColumn->AddSpacer(10);
    secondColumn->Add(label6, wxSizerFlags(0).Expand());
    secondColumn->Add(nlEconomy, wxSizerFlags(1).Expand());

    wxBoxSizer* thirdColumn = new wxBoxSizer(wxVERTICAL);
    thirdColumn->Add(label4, wxSizerFlags(0).Expand());
    thirdColumn->Add(nlArchitecture, wxSizerFlags(1).Expand());
    thirdColumn->AddSpacer(10);
    thirdColumn->Add(label7, wxSizerFlags(0).Expand());
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
}

vector<string> amdLocationCreator::GetValues() {
    vector<string> vec;

    vec.push_back((string)nlName->GetValue());
    vec.push_back((string)nlGeneral->GetValue());
    vec.push_back((string)nlNatural->GetValue());
    vec.push_back((string)nlArchitecture->GetValue());
    vec.push_back((string)nlPolitics->GetValue());
    vec.push_back((string)nlEconomy->GetValue());
    vec.push_back((string)nlCulture->GetValue());

    string importance;
    if (nlHigh->GetValue()) {
        importance = nlHigh->GetLabel();
    } else {
        importance = nlLow->GetLabel();
    }

    vec.push_back((string)importance);

    return vec;
}

void amdLocationCreator::SetEdit(Element* editLoc) {
    Location* location = dynamic_cast<Location*>(editLoc);
    if (!location) {
        wxMessageBox("There was an unexpected error. Editing could not start.");
        Destroy();
        return;
    }

    m_next->SetId(BUTTON_NextEdit1);
    m_back->SetId(BUTTON_BackEdit1);

    nlName->SetValue(location->name);
    nlGeneral->SetValue(location->general);
    nlNatural->SetValue(location->natural);
    nlArchitecture->SetValue(location->architecture);
    nlPolitics->SetValue(location->politics);
    nlEconomy->SetValue(location->economy);
    nlCulture->SetValue(location->culture);

    switch (location->role) {
    case lHigh:
        nlHigh->SetValue(true);
        break;

    case lLow:
        nlLow->SetValue(true);
        break;

    default:
        break;
    }

    int i = 0;
    for (auto& it : location->custom) {
        AddCustomAttr(wxCommandEvent());
        m_custom[i].first->SetValue(it.first);
        m_custom[i].second->SetValue(it.second);

        i++;
    }

    wxImage& image = m_imagePanel->GetImage();
    image = location->image;
    m_removeImage->Show(m_imagePanel->SetImage(image));

    m_elementEdit = location;

    SetTitle("Edit location - '" + m_elementEdit->name + "'");
}

void amdLocationCreator::DoEdit(wxCommandEvent& WXUNUSED(event)) {
    vector<string> vec = GetValues();

    Location location;

    location.name = vec[0];
    location.general = vec[1];
    location.natural = vec[2];
    location.architecture = vec[3];
    location.politics = vec[4];
    location.economy = vec[5];
    location.culture = vec[6];

    if (vec[7] == "High")
        location.role = lHigh;
    else if (vec[7] == "Low")
        location.role = lLow;
    else
        location.role = None;

    location.image = m_imagePanel->GetImage();

    location.custom = GetCustom();

    Location* pLocationToEdit = (Location*)m_elementEdit;
    
    bool dif = m_elementEdit->name != vec[0] || m_elementEdit->role != location.role;
    m_manager->EditLocation(*pLocationToEdit, location, dif);

    m_manager->GetMainFrame()->Enable(true);
    Destroy();
}

void amdLocationCreator::AddCustomAttr(wxCommandEvent& WXUNUSED(event)) {
    Freeze();
    wxPanel* panel = new wxPanel(m_panel2);
    wxSize size(label6->GetSize());
    wxSize size2(nlArchitecture->GetSize());

    wxTextCtrl* label = new wxTextCtrl(panel, -1, "Title",
        wxDefaultPosition, wxSize(-1, size.y), wxTE_NO_VSCROLL | wxBORDER_SIMPLE | wxTE_CENTER);
    wxTextCtrl* content = new wxTextCtrl(panel, -1, "", wxDefaultPosition,
        wxSize(size2.x - 10, size2.y), wxTE_MULTILINE | wxBORDER_SIMPLE);

    content->SetBackgroundColour(wxColour(60, 60, 60));
    content->SetForegroundColour(wxColour(250, 250, 250));
    content->SetFont(wxFontInfo(9));

    label->SetBackgroundColour(wxColour(200, 200, 200));
    label->SetFont(wxFontInfo(13).Bold());

    wxButton* minus = new wxButton(panel, -1, "", wxDefaultPosition, wxSize(size.y, size.y));
    minus->SetBitmap(wxBITMAP_PNG(minus));
    minus->Bind(wxEVT_BUTTON, &amdLocationCreator::RemoveCustomAttr, this);

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

void amdLocationCreator::RemoveCustomAttr(wxCommandEvent& event) {
    Freeze();
    wxButton* minus = (wxButton*)event.GetEventObject();

    auto it1 = m_custom.begin();
    auto it2 = m_minusButtons.begin();

    for (it2 = m_minusButtons.begin(); it2 != m_minusButtons.end(); it2++) {
        if (*it2 == minus)
            break;

        it1++;
    }

    m_custom.erase(it1);
    m_minusButtons.erase(it2);

    minus->GetParent()->Destroy();
    m_mainSizer->Layout();

    RecolorCustoms();
    Thaw();
}

void amdLocationCreator::RecolorCustoms() {
    wxColour red(230, 0, 20);
    wxColour grey(200, 200, 200);

    bool doRed = true;
    for (auto& it : m_custom) {
        if (doRed)
            it.first->SetBackgroundColour(red);
        else
            it.first->SetBackgroundColour(grey);

        doRed = !doRed;
    }
}

void amdLocationCreator::Create(wxCommandEvent& WXUNUSED(event)) {
    if (!nlName->IsEmpty()) {
        std::vector<std::string> vec = GetValues();

        Location location;

        location.name = vec[0];
        location.general = vec[1];
        location.natural = vec[2];
        location.architecture = vec[3];
        location.politics = vec[4];
        location.economy = vec[5];
        location.culture = vec[6];

        if (vec[7] == "High")
            location.role = lHigh;
        else if (vec[7] == "Low")
            location.role = lLow;
        else
            location.role = None;

        location.image = m_imagePanel->GetImage();

        location.custom = GetCustom();

        m_manager->AddLocation(location);
    } else {
        wxMessageBox("You can't create a location with no name!");
        return;
    }

    m_manager->GetMainFrame()->Enable();
    Destroy();
}

void amdLocationCreator::CheckClose(wxCloseEvent& WXUNUSED(event)) {

    if (nlArchitecture->IsModified() || nlName->IsModified() || nlGeneral->IsModified() ||
        nlNatural->IsModified() || nlEconomy->IsModified() || nlCulture->IsModified()) {

        wxMessageDialog check(this, "Are you sure you want to close?", "Close", wxYES_NO | wxNO_DEFAULT);

        if (check.ShowModal() == wxID_YES) {
            m_manager->GetMainFrame()->Enable();
            this->Destroy();
        } else {
            check.Destroy();
        }
    } else {
        m_manager->GetMainFrame()->Enable();
        this->Destroy();
    }
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Item Creator ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

amdItemCreator::amdItemCreator(wxWindow* parent, amdProjectManager* manager,
    long id, const string& label, const wxPoint& pos, const wxSize& size, long style) :
    amdElementCreator(parent, manager, id, label, pos, size, style) {

    wxColour dark(40, 40, 40);
    wxColour darker(30, 30, 30);
    wxColour txtCol(255, 255, 255);

    m_panel1->Bind(wxEVT_PAINT, &amdItemCreator::OnPaint, this);

    wxStaticText* label1 = new wxStaticText(m_panel1, wxID_ANY, "Name:",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
    label1->SetBackgroundColour(darker);
    label1->SetForegroundColour(txtCol);
    label1->SetFont(wxFont(wxFontInfo(12).Bold()));
    niName = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxBORDER_SIMPLE);
    niName->SetBackgroundColour(dark);
    niName->SetForegroundColour(txtCol);

    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    nameSizer->Add(label1);
    nameSizer->AddSpacer(3);
    nameSizer->Add(niName, 1);

    wxStaticText* label2 = new wxStaticText(m_panel1, wxID_ANY, "Type:",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
    label2->SetBackgroundColour(darker);
    label2->SetForegroundColour(txtCol);
    label2->SetFont(wxFont(wxFontInfo(12).Bold()));
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

    wxStaticText* label3 = new wxStaticText(m_panel1, wxID_ANY, "Is magic:",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
    label3->SetBackgroundColour(darker);
    label3->SetForegroundColour(txtCol);
    label3->SetFont(wxFont(wxFontInfo(12).Bold()));
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
    choicesSizer->Add(label2, wxSizerFlags(0).Center());
    choicesSizer->AddSpacer(3);
    choicesSizer->Add(typeSizer, wxSizerFlags(0).Center());
    choicesSizer->AddStretchSpacer(1);
    choicesSizer->Add(label3, wxSizerFlags(0).Center());
    choicesSizer->AddSpacer(3);
    choicesSizer->Add(magicSizer, wxSizerFlags(0).Center());

    wxBoxSizer* rightSiz1 = new wxBoxSizer(wxVERTICAL);
    rightSiz1->Add(nameSizer, wxSizerFlags(0).Expand());
    rightSiz1->AddSpacer(10);
    rightSiz1->Add(choicesSizer, wxSizerFlags(0).Expand());

    label4 = new wxStaticText(m_panel1, -1, "Measurements");
    label4->SetBackgroundColour(darker);
    label4->SetForegroundColour(txtCol);
    label4->SetFont(wxFont(wxFontInfo(12).Bold()));
    wxStaticText* label5 = new wxStaticText(m_panel1, -1, "Width:");
    label5->SetBackgroundColour(darker);
    label5->SetForegroundColour(txtCol);
    label5->SetFont(wxFont(wxFontInfo(12).Bold()));
    wxStaticText* label6 = new wxStaticText(m_panel1, -1, "Height:");
    label6->SetBackgroundColour(darker);
    label6->SetForegroundColour(txtCol);
    label6->SetFont(wxFont(wxFontInfo(12).Bold()));
    wxStaticText* label7 = new wxStaticText(m_panel1, -1, "Depth:");
    label7->SetBackgroundColour(darker);
    label7->SetForegroundColour(txtCol);
    label7->SetFont(wxFont(wxFontInfo(12).Bold()));

    wxBoxSizer* firstColumn = new wxBoxSizer(wxVERTICAL);
    firstColumn->Add(label5);
    firstColumn->AddStretchSpacer(1);
    firstColumn->Add(label6);
    firstColumn->AddStretchSpacer(1);
    firstColumn->Add(label7);
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

    wxBoxSizer* secondColumn = new wxBoxSizer(wxVERTICAL);
    secondColumn->Add(niWidth);
    secondColumn->AddStretchSpacer(1);
    secondColumn->Add(niHeight);
    secondColumn->AddStretchSpacer(1);
    secondColumn->Add(niDepth);
    secondColumn->AddStretchSpacer(1);

    wxBoxSizer* rightSiz2 = new wxBoxSizer(wxHORIZONTAL);
    rightSiz2->Add(firstColumn, wxSizerFlags(0).Expand());
    rightSiz2->AddStretchSpacer(1);
    rightSiz2->Add(secondColumn, wxSizerFlags(2).Expand());

    niMeaSizer = new wxBoxSizer(wxVERTICAL);
    niMeaSizer->Add(label4, wxSizerFlags(0).Expand());
    niMeaSizer->AddSpacer(20);
    niMeaSizer->Add(rightSiz2, wxSizerFlags(1).Expand());

    wxBoxSizer* rightVerSiz = new wxBoxSizer(wxVERTICAL);
    rightVerSiz->AddSpacer(10);
    rightVerSiz->Add(rightSiz1, wxSizerFlags(1).Expand());
    rightVerSiz->AddStretchSpacer(1);
    rightVerSiz->Add(niMeaSizer, wxSizerFlags(3).Expand());
    rightVerSiz->AddStretchSpacer(1);

    wxStaticText* label8 = new wxStaticText(m_panel1, wxID_ANY, "Appearance",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
    label8->SetBackgroundColour(darker);
    label8->SetForegroundColour(txtCol);
    label8->SetFont(wxFont(wxFontInfo(12).Bold()));
    niAppearance = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
    niAppearance->SetBackgroundColour(dark);
    niAppearance->SetForegroundColour(txtCol);

    wxStaticText* label9 = new wxStaticText(m_panel1, wxID_ANY, "Origin",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
    label9->SetBackgroundColour(darker);
    label9->SetForegroundColour(txtCol);
    label9->SetFont(wxFont(wxFontInfo(12).Bold()));
    niOrigin = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
    niOrigin->SetBackgroundColour(dark);
    niOrigin->SetForegroundColour(txtCol);

    wxStaticText* label10 = new wxStaticText(m_panel1, wxID_ANY, "Backstory",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
    label10->SetBackgroundColour(darker);
    label10->SetForegroundColour(txtCol);
    label10->SetFont(wxFont(wxFontInfo(12).Bold()));
    niBackstory = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
    niBackstory->SetBackgroundColour(dark);
    niBackstory->SetForegroundColour(txtCol);

    wxStaticText* label11 = new wxStaticText(m_panel1, wxID_ANY, "Usage",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
    label11->SetBackgroundColour(darker);
    label11->SetForegroundColour(txtCol);
    label11->SetFont(wxFont(wxFontInfo(12).Bold()));
    niUsage = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
    niUsage->SetBackgroundColour(dark);
    niUsage->SetForegroundColour(txtCol);

    wxBoxSizer* verSizer1 = new wxBoxSizer(wxVERTICAL);
    verSizer1->Add(label8, wxSizerFlags(0).Left());
    verSizer1->AddSpacer(4);
    verSizer1->Add(niAppearance, wxSizerFlags(1).Expand());
    verSizer1->AddSpacer(2);
    verSizer1->Add(label9, wxSizerFlags(0).Left());
    verSizer1->AddSpacer(4);
    verSizer1->Add(niOrigin, wxSizerFlags(1).Expand());
    verSizer1->AddSpacer(2);
    verSizer1->Add(label10, wxSizerFlags(0).Left());
    verSizer1->AddSpacer(4);
    verSizer1->Add(niBackstory, wxSizerFlags(1).Expand());
    verSizer1->AddSpacer(2);
    verSizer1->Add(label11, wxSizerFlags(0).Left());
    verSizer1->AddSpacer(4);
    verSizer1->Add(niUsage, wxSizerFlags(1).Expand());

    wxBoxSizer* mainHor = new wxBoxSizer(wxHORIZONTAL);
    mainHor->Add(rightVerSiz, wxSizerFlags(1).Expand());
    mainHor->AddSpacer(20);
    mainHor->Add(verSizer1, wxSizerFlags(2).Expand());

    wxStaticText* label12 = new wxStaticText(m_panel1, wxID_ANY, "General",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_NONE);
    label12->SetBackgroundColour(darker);
    label12->SetForegroundColour(txtCol);
    label12->SetFont(wxFont(wxFontInfo(12).Bold()));
    niGeneral = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);
    niGeneral->SetBackgroundColour(dark);
    niGeneral->SetForegroundColour(txtCol);

    wxBoxSizer* mainVer = new wxBoxSizer(wxVERTICAL);
    mainVer->AddSpacer(10);
    mainVer->Add(mainHor, wxSizerFlags(4).Expand().Border(wxLEFT | wxRIGHT, 20));
    mainVer->Add(label12, wxSizerFlags(0).Left().Border(wxLEFT | wxRIGHT, 20));
    mainVer->AddSpacer(5);
    mainVer->Add(niGeneral, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 20));

    m_panel1->SetSizer(mainVer);

    SetIcon(wxICON(itemIcon));
}

vector<string> amdItemCreator::GetValues() {
    return vector<string>();
}

void amdItemCreator::SetEdit(Element* editLoc) {}

void amdItemCreator::DoEdit(wxCommandEvent& event) {}

void amdItemCreator::AddCustomAttr(wxCommandEvent& event) {
    Freeze();
    wxSize size(label4->GetSize());

    wxTextCtrl* label = new wxTextCtrl(m_panel2, -1, "Title",
        wxDefaultPosition, wxSize(-1, size.y), wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    wxTextCtrl* content = new wxTextCtrl(m_panel2, -1, "", wxDefaultPosition,
        wxSize(-1, niGeneral->GetSize().y), wxTE_MULTILINE | wxBORDER_SIMPLE);

    content->SetBackgroundColour(wxColour(60, 60, 60));
    content->SetForegroundColour(wxColour(255, 255, 255));
    label->SetBackgroundColour(wxColour(230, 0, 20));
    label->SetFont(wxFontInfo(13).Bold());

    wxButton* minus = new wxButton(m_panel2, -1, "", wxDefaultPosition, wxSize(size.y, size.y));
    minus->SetBitmap(wxBITMAP_PNG(minus));
    minus->Bind(wxEVT_BUTTON, &amdCharacterCreator::RemoveCustomAttr, this);

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

void amdItemCreator::Create(wxCommandEvent& event) {}

void amdItemCreator::CheckClose(wxCloseEvent& event) {
    if (niGeneral->IsModified() || niName->IsModified() || niAppearance->IsModified() ||
        niBackstory->IsModified() || niOrigin->IsModified() || niUsage->IsModified()) {

        wxMessageDialog check(this, "Are you sure you want to close?", "Close", wxYES_NO | wxNO_DEFAULT);

        if (check.ShowModal() == wxID_YES) {
            m_manager->GetMainFrame()->Enable();
            this->Destroy();
        } else {
            check.Destroy();
        }
    } else {
        m_manager->GetMainFrame()->Enable();
        this->Destroy();
    }
}

void amdItemCreator::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(m_panel1);

    wxRect rect;
    dc.SetPen(wxPen(wxColour(250, 0, 0), 3));
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

    dc.SetPen(wxPen(wxColour(250, 0, 0), 5));

    rect = label4->GetRect();
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
