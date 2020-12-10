#include "CharacterCreator.h"

#include "OutlineFiles.h"

#include "wx/listctrl.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(CharacterCreator, wxFrame)

EVT_BUTTON(BUTTON_CancelChar, CharacterCreator::cancel)
EVT_BUTTON(BUTTON_CreateChar, CharacterCreator::create)
EVT_BUTTON(BUTTON_NextChar, CharacterCreator::next)
EVT_BUTTON(BUTTON_BackChar, CharacterCreator::back)

EVT_BUTTON(BUTTON_NextEdit, CharacterCreator::nextEdit)
EVT_BUTTON(BUTTON_BackEdit, CharacterCreator::backEdit)
EVT_BUTTON(BUTTON_CreateEdit, CharacterCreator::edit)

EVT_BUTTON(BUTTON_CharImage, CharacterCreator::setImage)
EVT_BUTTON(BUTTON_CharRemoveImage, CharacterCreator::removeImage)

EVT_CLOSE(CharacterCreator::checkClose)

END_EVENT_TABLE()

using std::string;
using std::vector;

CharacterCreator::CharacterCreator(wxWindow* parent, ElementsNotebook* notebook,
    long id, const string& label, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, label, pos, size
    , style) {

    this->mainFrame = (MainFrame*)parent;
    this->notebook = notebook;
    this->CenterOnParent();

    SetBackgroundColour(wxColour(40, 40, 40));

    ncPanel1 = new wxPanel(this, wxID_ANY);
    ncPanel1->SetBackgroundColour(wxColour(40, 40, 40));
    ncPanel1->Show(true);

    wxString choice[] = { "Male", "Female" };

    ncFullName = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(70, 10), wxSize(420, 25),
        wxBORDER_SIMPLE);
    ncFullName->SetBackgroundColour(wxColour(70, 70, 70));
    ncFullName->SetForegroundColour(wxColour(250, 250, 250));
    ncFullName->SetFont(wxFont(wxFontInfo(10)));
    wxStaticText* label1 = new wxStaticText(ncPanel1, wxID_ANY, "Name:", wxPoint(10, 10), wxSize(50, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label1->SetFont(wxFont(wxFontInfo(12)));
    label1->SetBackgroundColour(wxColour(40, 40, 40));
    label1->SetForegroundColour(wxColour(240, 240, 240));

    wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
    firstLine->Add(label1, wxSizerFlags(0).Border(wxRIGHT, 10));
    firstLine->Add(ncFullName, wxSizerFlags(1));

    male = new wxRadioButton(ncPanel1, wxID_ANY, "Male", wxPoint(60, 40), wxSize(60, 20), wxRB_GROUP);
    male->SetFont(wxFont(wxFontInfo(10)));
    male->SetForegroundColour(wxColour(245, 245, 245));
    female = new wxRadioButton(ncPanel1, wxID_ANY, "Female", wxPoint(60, 60), wxSize(60, 20));
    female->SetFont(wxFont(wxFontInfo(10)));
    female->SetForegroundColour(wxColour(245, 245, 245));

    wxBoxSizer* btnHolderSizer1 = new wxBoxSizer(wxVERTICAL);
    btnHolderSizer1->Add(male, wxSizerFlags(0).Left());
    btnHolderSizer1->Add(female, wxSizerFlags(0).Left());

    wxStaticText* label2 = new wxStaticText(ncPanel1, wxID_ANY, "Sex:", wxPoint(10, 50), wxSize(40, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label2->SetBackgroundColour(wxColour(40, 40, 40));
    label2->SetForegroundColour(wxColour(240, 240, 240));
    label2->SetFont(wxFont(wxFontInfo(13)));

    ncAge = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(180, 50), wxSize(50, 25),
        wxBORDER_SIMPLE);
    ncAge->SetBackgroundColour(wxColour(70, 70, 70));
    ncAge->SetForegroundColour(wxColour(250, 250, 250));
    ncAge->SetFont(wxFont(wxFontInfo(10)));
    wxStaticText* label3 = new wxStaticText(ncPanel1, wxID_ANY, "Age:", wxPoint(130, 50), wxSize(40, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label3->SetBackgroundColour(wxColour(40, 40, 40));
    label3->SetForegroundColour(wxColour(240, 240, 240));
    label3->SetFont(wxFont(wxFontInfo(12)));

    ncNationality = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(335, 50), wxSize(155, 25),
        wxBORDER_SIMPLE);
    ncNationality->SetBackgroundColour(wxColour(70, 70, 70));
    ncNationality->SetForegroundColour(wxColour(250, 250, 250));
    ncNationality->SetFont(wxFont(wxFontInfo(10)));
    wxStaticText* label4 = new wxStaticText(ncPanel1, wxID_ANY, "Nationality: ", wxPoint(240, 50), wxSize(90, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label4->SetBackgroundColour(wxColour(40, 40, 40));
    label4->SetForegroundColour(wxColour(240, 240, 240));
    label4->SetFont(wxFont(wxFontInfo(12)));

    wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
    secondLine->Add(label2, wxSizerFlags(0).CenterVertical().Border(wxTOP | wxRIGHT, 7));
    secondLine->Add(btnHolderSizer1, wxSizerFlags(0).CenterVertical().Border(wxTOP | wxLEFT | wxRIGHT, 8));
    secondLine->Add(label3, wxSizerFlags(0).CenterVertical().Border(wxTOP | wxRIGHT, 7));
    secondLine->Add(ncAge, wxSizerFlags(1).CenterVertical().Border(wxTOP | wxLEFT | wxRIGHT, 7));
    secondLine->Add(label4, wxSizerFlags(0).CenterVertical().Border(wxTOP | wxLEFT, 7));
    secondLine->Add(ncNationality, wxSizerFlags(1).CenterVertical().Border(wxTOP, 7));

    ncHeight = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(75, 90), wxSize(60, 25),
        wxBORDER_SIMPLE);
    ncHeight->SetBackgroundColour(wxColour(70, 70, 70));
    ncHeight->SetForegroundColour(wxColour(250, 250, 250));
    ncHeight->SetFont(wxFont(wxFontInfo(10)));
    wxStaticText* label5 = new wxStaticText(ncPanel1, wxID_ANY, "Height: ", wxPoint(10, 90), wxSize(55, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label5->SetBackgroundColour(wxColour(40, 40, 40));
    label5->SetForegroundColour(wxColour(240, 240, 240));
    label5->SetFont(wxFont(wxFontInfo(12)));

    ncNickname = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(235, 90), wxSize(100, 25),
        wxBORDER_SIMPLE);
    ncNickname->SetBackgroundColour(wxColour(70, 70, 70));
    ncNickname->SetForegroundColour(wxColour(250, 250, 250));
    ncNickname->SetFont(wxFont(wxFontInfo(10)));
    wxStaticText* label6 = new wxStaticText(ncPanel1, wxID_ANY, "Nickname: ", wxPoint(145, 90), wxSize(90, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label6->SetBackgroundColour(wxColour(40, 40, 40));
    label6->SetForegroundColour(wxColour(240, 240, 240));
    label6->SetFont(wxFont(wxFontInfo(12)));

    main = new wxRadioButton(ncPanel1, wxID_ANY, "Main", wxPoint(395, 80), wxSize(60, 20), wxRB_GROUP);
    main->SetFont(wxFont(wxFontInfo(10)));
    main->SetForegroundColour(wxColour(245, 245, 245));
    secon = new wxRadioButton(ncPanel1, wxID_ANY, "Secondary", wxPoint(395, 100), wxSize(100, 20));
    secon->SetFont(wxFont(wxFontInfo(10)));
    secon->SetForegroundColour(wxColour(245, 245, 245));

    wxStaticText* label7 = new wxStaticText(ncPanel1, wxID_ANY, "Role: ", wxPoint(345, 90), wxSize(50, 25),
        wxNO_BORDER | wxALIGN_LEFT);
    label7->SetBackgroundColour(wxColour(40, 40, 40));
    label7->SetForegroundColour(wxColour(240, 240, 240));
    label7->SetFont(wxFont(wxFontInfo(12)));
    
    wxBoxSizer* btnHolderSizer2 = new wxBoxSizer(wxVERTICAL);
    btnHolderSizer2->Add(main, wxSizerFlags(0).Left());
    btnHolderSizer2->Add(secon, wxSizerFlags(0).Left());

    wxBoxSizer* thirdLine = new wxBoxSizer(wxHORIZONTAL);
    thirdLine->Add(label5, wxSizerFlags(0).CenterVertical().Border(wxRIGHT, 5));
    thirdLine->Add(ncHeight, wxSizerFlags(1).CenterVertical().Border(wxRIGHT, 15));
    thirdLine->Add(label6, wxSizerFlags(0).CenterVertical());
    thirdLine->Add(ncNickname, wxSizerFlags(1).CenterVertical());
    thirdLine->Add(label7, wxSizerFlags(0).CenterVertical().Border(wxLEFT, 6));
    thirdLine->Add(btnHolderSizer2, wxSizerFlags(0).CenterVertical());

    wxStaticText* label8 = new wxStaticText(ncPanel1, wxID_ANY, "Appearance", wxPoint(15, 145), wxSize(475, -1),
        wxBORDER_SIMPLE | wxALIGN_LEFT);
    label8->SetBackgroundColour(wxColour(230, 0, 20));
    label8->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncAppearance = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(15, 170), wxSize(475, 85),
        wxTE_MULTILINE | wxBORDER_SIMPLE);
    ncAppearance->SetBackgroundColour(wxColour(70, 70, 70));
    ncAppearance->SetForegroundColour(wxColour(250, 250, 250));

    wxStaticText* label9 = new wxStaticText(ncPanel1, wxID_ANY, "Personality", wxPoint(15, 265), wxSize(475, -1),
        wxBORDER_SIMPLE | wxALIGN_LEFT);
    label9->SetBackgroundColour(wxColour(230, 0, 20));
    label9->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncPersonality = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(15, 290), wxSize(475, 85),
        wxTE_MULTILINE | wxBORDER_SIMPLE);
    ncPersonality->SetBackgroundColour(wxColour(70, 70, 70));
    ncPersonality->SetForegroundColour(wxColour(250, 250, 250));

    wxStaticText* label10 = new wxStaticText(ncPanel1, wxID_ANY, "Backstory", wxPoint(15, 385), wxSize(475, -1),
        wxBORDER_SIMPLE | wxALIGN_LEFT);
    label10->SetBackgroundColour(wxColor(230, 0, 20));
    label10->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncBackstory = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(15, 410), wxSize(475, 85),
        wxTE_MULTILINE | wxBORDER_SIMPLE);
    ncBackstory->SetBackgroundColour(wxColour(70, 70, 70));
    ncBackstory->SetForegroundColour(wxColour(250, 250, 250));

    wxPanel* btnPanel = new wxPanel(this);
    btnPanel->SetBackgroundColour(wxColour(40, 40, 40));

    ncNext = new wxButton(btnPanel, BUTTON_NextChar, "Next", wxPoint(300, 505), wxSize(90, 30));
    ncBack = new wxButton(btnPanel, BUTTON_BackChar, "Back", wxPoint(15, 505), wxSize(90, 30));
    ncCancel = new wxButton(btnPanel, BUTTON_CancelChar, "Cancel", wxPoint(400, 505), wxSize(90, 30));

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
    verSizer1->Add(secondLine, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 10));
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

    ncPanel1->SetSizer(verSizer1);

    SetIcon(wxICON(characterIcon));
    this->Layout();

    ncPanel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(520, 585));
    ncPanel2->Hide();
    ncPanel2->SetBackgroundColour(wxColour(40, 40, 40));

    ncChooseImage = new wxButton(ncPanel2, BUTTON_CharImage, "Choose image file", wxPoint(285, 415), wxSize(150, 30));
    ncRemoveImage = new wxButton(ncPanel2, BUTTON_CharRemoveImage, "Remove", wxPoint(75, 415), wxSize(100, 30));
    ncRemoveImage->Hide();
    ncImagePanel = new ImagePanel(ncPanel2, wxPoint(75, 40), wxSize(360, 360));
    ncImagePanel->setBorderColour(wxColour(40, 40, 40));

    wxBoxSizer* btnSizer2 = new wxBoxSizer(wxHORIZONTAL);
    btnSizer2->Add(ncRemoveImage, wxSizerFlags(0));
    btnSizer2->AddStretchSpacer(1);
    btnSizer2->Add(ncChooseImage, wxSizerFlags(0));

    wxBoxSizer* verSizer2 = new wxBoxSizer(wxVERTICAL);
    verSizer2->Add(ncImagePanel, wxSizerFlags(0).Border(wxTOP, 40).CenterHorizontal());
    verSizer2->AddStretchSpacer(1);
    verSizer2->Add(btnSizer2, wxSizerFlags(0).Expand());
    verSizer2->AddStretchSpacer(3);

    ncPanel2->SetSizer(verSizer2);

    mainSiz = new wxBoxSizer(wxVERTICAL);
    mainSiz->Add(ncPanel1, wxSizerFlags(1).Expand());
    mainSiz->Add(ncPanel2, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 75));
    mainSiz->Add(btnPanel, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));

    SetSizer(mainSiz);
}

vector<string> CharacterCreator::getValues() {

    std::vector<std::string> vec;

    vec.push_back((std::string)ncFullName->GetValue());

    if (male->GetValue()) {
        vec.push_back((std::string)male->GetLabel());
    } else {
        vec.push_back((std::string)female->GetLabel());
    }

    vec.push_back((std::string)ncAge->GetValue());
    vec.push_back((std::string)ncNationality->GetValue());
    vec.push_back((std::string)ncHeight->GetValue());
    vec.push_back((std::string)ncNickname->GetValue());
    
    if (main->GetValue()) {
        vec.push_back((std::string)main->GetLabel());
    } else {
        vec.push_back((std::string)secon->GetLabel());
    }

    vec.push_back((std::string)ncAppearance->GetValue());
    vec.push_back((std::string)ncPersonality->GetValue());
    vec.push_back((std::string)ncBackstory->GetValue());

    return vec;
}

void CharacterCreator::setEdit(Character* character) {
    ncNext->SetId(BUTTON_NextEdit);

    ncFullName->SetValue(character->name);

    if (character->sex == "Female") {
        female->SetValue(true);
    }
    ncAge->SetValue(character->age);
    ncNationality->SetValue(character->nat);
    ncHeight->SetValue(character->height);
    ncNickname->SetValue(character->nick);

    if (character->role == "Secondary") {
        secon->SetValue(true);
    }

    ncAppearance->SetValue(character->appearance);
    ncPersonality->SetValue(character->personality);
    ncBackstory->SetValue(character->backstory);

    ncImage = character->image;

    if (ncImage.IsOk()) {
        ncImagePanel->setImage(ncImage);
        ncRemoveImage->Show();
    }

    charEdit = character;

    this->Show(true);
}

void CharacterCreator::nextEdit(wxCommandEvent& event) {
    ncNext->SetId(BUTTON_CreateEdit);
    ncNext->SetLabel("Ok");
    ncBack->SetId(BUTTON_BackEdit);
    ncBack->Show();
    ncPanel1->Hide();
    ncPanel2->Show();
    ncChooseImage->SetFocus();

    mainSiz->Layout();
    event.Skip();
}

void CharacterCreator::backEdit(wxCommandEvent& event) {
    ncNext->SetLabel("Next");
    ncNext->SetId(BUTTON_NextEdit);
    ncBack->Hide();
    ncPanel2->Hide();
    ncPanel1->Show();
    ncPanel1->SetFocus();

    mainSiz->Layout();
    event.Skip();
}

void CharacterCreator::edit(wxCommandEvent& event) {
    mainFrame->getOutline()->getOutlineFiles()->deleteCharacter(*charEdit);

    vector<string> vec = getValues();
    string temp;

    if (ncFullName->IsModified() || charEdit->role != vec[6]) {
        notebook->removeCharName(charEdit->name);
        temp = charEdit->role;
        mainFrame->characters.erase(charEdit->role + charEdit->name);
        Character dummy;
        charEdit = &dummy;
    }

    charEdit->name = vec[0];
    charEdit->sex = vec[1];
    charEdit->age = vec[2];
    charEdit->nat = vec[3];
    charEdit->height = vec[4];
    charEdit->nick = vec[5];
    charEdit->role = vec[6];

    charEdit->appearance = vec[7];
    charEdit->personality = vec[8];
    charEdit->backstory = vec[9];

    charEdit->image = ncImage;

    if (ncFullName->IsModified() || charEdit->role != vec[6]) {
        notebook->addCharName(charEdit->name);
        MainFrame::characters[charEdit->role + charEdit->name] = *charEdit;
        notebook->setSearchAC(wxBookCtrlEvent());
    }

    notebook->updateLB();
    notebook->charShow->setData(charEdit->image, vec);

    mainFrame->isSaved = false;
    mainFrame->getOutline()->getOutlineFiles()->appendCharacter(*charEdit);
    mainFrame->Enable();

    this->Destroy();

    event.Skip();
}

void CharacterCreator::cancel(wxCommandEvent& event) {
    mainFrame->Enable();
    this->Destroy();
    event.Skip();
}

void CharacterCreator::next(wxCommandEvent& event) {
    ncNext->SetLabel("Create");
    ncNext->SetId(BUTTON_CreateChar);
    ncBack->Show();
    ncPanel1->Hide();
    ncPanel2->Show();
    ncChooseImage->SetFocus();

    mainSiz->Layout();
    event.Skip();
}

void CharacterCreator::back(wxCommandEvent& event) {
    ncNext->SetLabel("Next");
    ncNext->SetId(BUTTON_NextChar);
    ncBack->Hide();
    ncPanel2->Hide();
    ncPanel1->Show();

    mainSiz->Layout();
    event.Skip();
}

void CharacterCreator::setImage(wxCommandEvent& event) {
    wxFileDialog* choose = new wxFileDialog(this, "Select an image", wxEmptyString, wxEmptyString,
        "JPG, JPEG and PNG(*.jpg;*.jpeg;*.png)|*.jpg;*.jpeg;*.png",
        wxFD_OPEN, wxDefaultPosition);

    if (choose->ShowModal() == wxID_OK) {
        ncImage.LoadFile(choose->GetPath(), wxBITMAP_TYPE_ANY);
        ncImagePanel->setImage(ncImage);
        ncRemoveImage->Show();
    }

    if (choose)
        delete choose;

    mainSiz->Layout();
    event.Skip();
}

void CharacterCreator::removeImage(wxCommandEvent& event) {
    ncImage.Destroy();
    ncImagePanel->SetBackgroundBitmap(ncImage);
    ncImagePanel->Refresh(true);
    ncRemoveImage->Hide();
    mainSiz->Layout();

    event.Skip();
}

void CharacterCreator::create(wxCommandEvent& event) {

    if (!ncFullName->IsEmpty()) {
        vector<string> vec = getValues();

        Character character;
        character.name = vec[0];
        character.sex = vec[1];
        character.age = vec[2];
        character.nat = vec[3];
        character.height = vec[4];
        character.nick = vec[5];
        character.role = vec[6];

        character.appearance = vec[7];
        character.personality = vec[8];
        character.backstory = vec[9];

        character.image = ncImage;

        mainFrame->characters[vec[6] + vec[0]] = character;

        ElementsNotebook::updateLB();

        notebook->addCharName(vec[0]);
        notebook->setSearchAC(wxBookCtrlEvent());

        mainFrame->saved[0]++;
        mainFrame->isSaved = false;
        mainFrame->getOutline()->getOutlineFiles()->appendCharacter(character);
        mainFrame->Enable();
    }

    this->Destroy();
    event.Skip();
}

void CharacterCreator::checkClose(wxCloseEvent& event) {

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
