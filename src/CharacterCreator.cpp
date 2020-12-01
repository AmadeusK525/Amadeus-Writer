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

CharacterCreator::CharacterCreator(wxWindow* parent, ElementsNotebook* notebook):
    wxFrame(parent, wxID_ANY, "Create character", wxDefaultPosition, wxSize(520, 585)
    , wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT) {

    this->mainFrame = (MainFrame*)parent;
    this->notebook = notebook;
    this->CenterOnParent();

    ncPanel1 = new wxPanel(this, wxID_ANY);
    ncPanel1->SetBackgroundColour("WHITE");
    ncPanel1->Show(true);

    wxString choice[] = { "Male", "Female" };

    ncFullName = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(70, 10), wxSize(420, 25));
    ncFullName->SetBackgroundColour(wxColour(200, 200, 200));
    ncFullName->SetFont(wxFont(wxFontInfo(10)));
    wxTextCtrl* label1 = new wxTextCtrl(ncPanel1, wxID_ANY, "Name:", wxPoint(10, 10), wxSize(50, 25), wxNO_BORDER | wxTE_READONLY | wxTE_LEFT);
    label1->SetFont(wxFont(wxFontInfo(12)));
    label1->SetBackgroundColour("WHITE");

    male = new wxRadioButton(ncPanel1, wxID_ANY, "Male", wxPoint(60, 40), wxSize(60, 20), wxRB_GROUP);
    male->SetFont(wxFont(wxFontInfo(10)));
    female = new wxRadioButton(ncPanel1, wxID_ANY, "Female", wxPoint(60, 60), wxSize(60, 20));
    female->SetFont(wxFont(wxFontInfo(10)));

    wxTextCtrl* label2 = new wxTextCtrl(ncPanel1, wxID_ANY, "Sex:", wxPoint(10, 50), wxSize(40, 25), wxNO_BORDER | wxTE_READONLY | wxTE_LEFT);
    label2->SetBackgroundColour("WHITE");
    label2->SetFont(wxFont(wxFontInfo(13)));

    ncAge = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(180, 50), wxSize(50, 25));
    ncAge->SetBackgroundColour(wxColour(200, 200, 200));
    ncAge->SetFont(wxFont(wxFontInfo(10)));
    wxTextCtrl* label3 = new wxTextCtrl(ncPanel1, wxID_ANY, "Age:", wxPoint(130, 50), wxSize(40, 25), wxNO_BORDER | wxTE_READONLY | wxTE_LEFT);
    label3->SetBackgroundColour("WHITE");
    label3->SetFont(wxFont(wxFontInfo(12)));

    ncNationality = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(335, 50), wxSize(155, 25));
    ncNationality->SetBackgroundColour(wxColour(200, 200, 200));
    ncNationality->SetFont(wxFont(wxFontInfo(10)));
    wxTextCtrl* label4 = new wxTextCtrl(ncPanel1, wxID_ANY, "Nationality:", wxPoint(240, 50), wxSize(90, 25), wxNO_BORDER | wxTE_READONLY | wxTE_LEFT);
    label4->SetBackgroundColour("WHITE");
    label4->SetFont(wxFont(wxFontInfo(12)));

    ncHeight = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(75, 90), wxSize(60, 25));
    ncHeight->SetBackgroundColour(wxColour(200, 200, 200));
    ncHeight->SetFont(wxFont(wxFontInfo(10)));
    wxTextCtrl* label5 = new wxTextCtrl(ncPanel1, wxID_ANY, "Height:", wxPoint(10, 90), wxSize(55, 25), wxNO_BORDER | wxTE_READONLY | wxTE_LEFT);
    label5->SetBackgroundColour("WHITE");
    label5->SetFont(wxFont(wxFontInfo(12)));

    ncNickname = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(235, 90), wxSize(100, 25));
    ncNickname->SetBackgroundColour(wxColour(200, 200, 200));
    ncNickname->SetFont(wxFont(wxFontInfo(10)));
    wxTextCtrl* label6 = new wxTextCtrl(ncPanel1, wxID_ANY, "Nickname:", wxPoint(145, 90), wxSize(90, 25), wxNO_BORDER | wxTE_READONLY | wxTE_LEFT);
    label6->SetBackgroundColour("WHITE");
    label6->SetFont(wxFont(wxFontInfo(12)));

    main = new wxRadioButton(ncPanel1, wxID_ANY, "Main", wxPoint(395, 80), wxSize(60, 20), wxRB_GROUP);
    main->SetFont(wxFont(wxFontInfo(10)));
    secon = new wxRadioButton(ncPanel1, wxID_ANY, "Secondary", wxPoint(395, 100), wxSize(100, 20));
    secon->SetFont(wxFont(wxFontInfo(10)));

    wxTextCtrl* label7 = new wxTextCtrl(ncPanel1, wxID_ANY, "Role:", wxPoint(345, 90), wxSize(50, 25), wxNO_BORDER | wxTE_READONLY | wxTE_LEFT);
    label7->SetBackgroundColour("WHITE");
    label7->SetFont(wxFont(wxFontInfo(12)));

    wxTextCtrl* label8 = new wxTextCtrl(ncPanel1, wxID_ANY, "Appearance", wxPoint(15, 145), wxSize(475, 25), wxTE_READONLY | wxTE_LEFT);
    label8->SetBackgroundColour(wxColor(230, 0, 20, 0));
    label8->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncAppearance = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(15, 170), wxSize(475, 85), wxTE_MULTILINE);

    wxTextCtrl* label9 = new wxTextCtrl(ncPanel1, wxID_ANY, "Personality", wxPoint(15, 265), wxSize(475, 25), wxTE_READONLY | wxTE_LEFT);
    label9->SetBackgroundColour(wxColor(230, 0, 20, 0));
    label9->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncPersonality = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(15, 290), wxSize(475, 85), wxTE_MULTILINE);

    wxTextCtrl* label10 = new wxTextCtrl(ncPanel1, wxID_ANY, "Backstory", wxPoint(15, 385), wxSize(475, 25), wxTE_READONLY | wxTE_LEFT);
    label10->SetBackgroundColour(wxColor(230, 0, 20, 0));
    label10->SetFont(wxFont(wxFontInfo(13).Bold()));
    ncBackstory = new wxTextCtrl(ncPanel1, wxID_ANY, wxEmptyString, wxPoint(15, 410), wxSize(475, 85), wxTE_MULTILINE);

    ncNext = new wxButton(ncPanel1, BUTTON_NextChar, "Next", wxPoint(300, 505), wxSize(90, 30));
    ncCancel = new wxButton(ncPanel1, BUTTON_CancelChar, "Cancel", wxPoint(400, 505), wxSize(90, 30));

    SetIcon(wxICON(characterIcon));
    this->Layout();

    ncPanel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(520, 585));
    ncPanel2->Hide();
    ncPanel2->SetBackgroundColour("WHITE");

    ncBack = new wxButton(ncPanel2, BUTTON_BackChar, "Back", wxPoint(15, 505), wxSize(90, 30));

    ncChooseImage = new wxButton(ncPanel2, BUTTON_CharImage, "Choose image file", wxPoint(285, 415), wxSize(150, 30));
    ncRemoveImage = new wxButton(ncPanel2, BUTTON_CharRemoveImage, "Remove", wxPoint(75, 415), wxSize(100, 30));
    ncRemoveImage->Hide();
    ncImagePanel = new ImagePanel(ncPanel2, wxPoint(75, 40), wxSize(360, 360));
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
    ncNext->Reparent(ncPanel2);
    ncNext->SetLabel("Ok");
    ncBack->SetId(BUTTON_BackEdit);
    ncCancel->Reparent(ncPanel2);
    ncPanel1->Hide();
    ncPanel2->Show();
}

void CharacterCreator::backEdit(wxCommandEvent& event) {
    ncCancel->Reparent(ncPanel1);
    ncNext->Reparent(ncPanel1);
    ncNext->SetLabel("Next");
    ncNext->SetId(BUTTON_NextEdit);
    ncPanel2->Hide();
    ncPanel1->Show();
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
}

void CharacterCreator::cancel(wxCommandEvent& event) {
    mainFrame->Enable();
    this->Destroy();
}

void CharacterCreator::next(wxCommandEvent& event) {
    ncNext->Reparent(ncPanel2);
    ncNext->SetLabel("Create");
    ncNext->SetId(BUTTON_CreateChar);
    ncCancel->Reparent(ncPanel2);
    ncPanel1->Hide();
    ncPanel2->Show();
}

void CharacterCreator::back(wxCommandEvent& event) {
    ncCancel->Reparent(ncPanel1);
    ncNext->Reparent(ncPanel1);
    ncNext->SetLabel("Next");
    ncNext->SetId(BUTTON_NextChar);
    ncPanel2->Hide();
    ncPanel1->Show();
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
}

void CharacterCreator::removeImage(wxCommandEvent& event) {
    ncImagePanel->ClearBackground();
    ncImage.Destroy();
    ncRemoveImage->Hide();
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
            check->Destroy();
            break;
        }

    } else {
        mainFrame->Enable();
        this->Destroy();
    }
}
