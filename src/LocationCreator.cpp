#include "LocationCreator.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(LocationCreator, wxFrame)

EVT_CLOSE(LocationCreator::checkClose)

EVT_BUTTON(BUTTON_NextLoc, LocationCreator::next)
EVT_BUTTON(BUTTON_BackLoc, LocationCreator::back)
EVT_BUTTON(BUTTON_CreateLoc, LocationCreator::create)
EVT_BUTTON(BUTTON_CancelLoc, LocationCreator::cancel)

EVT_BUTTON(BUTTON_NextEditLoc, LocationCreator::nextEdit)
EVT_BUTTON(BUTTON_BackEditLoc, LocationCreator::backEdit)
EVT_BUTTON(BUTTON_CreateEditLoc, LocationCreator::edit)

EVT_BUTTON(BUTTON_ChooseImageLoc, LocationCreator::setImage)
EVT_BUTTON(BUTTON_RemoveImageLoc, LocationCreator::removeImage)

END_EVENT_TABLE()

using std::vector;
using std::string;

LocationCreator::LocationCreator(wxWindow* parent, ElementsNotebook* notebook) :
    wxFrame(parent, wxID_ANY, "Create location", wxDefaultPosition, wxSize(655, 630),
    wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_SHAPED | wxFRAME_FLOAT_ON_PARENT) {

    this->notebook = notebook;
    this->CenterOnParent();

    nlPanel1 = new wxPanel(this, wxID_ANY);
    nlPanel1->SetBackgroundColour("WHITE");

    wxPanel* panel = new wxPanel(nlPanel1, wxID_ANY, wxPoint(80, 20), wxSize(130, 35));
    panel->SetBackgroundColour(wxColour(230, 0, 20));

    wxTextCtrl* label1 = new wxTextCtrl(nlPanel1, wxID_ANY, "Name of location:", wxPoint(80, 25), wxSize(130, 25), wxTE_CENTER | wxTE_READONLY | wxNO_BORDER);
    label1->SetBackgroundColour("WHITE");
    label1->SetFont(wxFont(wxFontInfo(12)));

    nlName = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(220, 25), wxSize(330, 25));
    nlName->SetBackgroundColour(wxColour(200, 200, 200));
    nlName->SetFont(wxFont(wxFontInfo(10)));

    wxTextCtrl* label2 = new wxTextCtrl(nlPanel1, wxID_ANY, "Historical background", wxPoint(10, 70), wxSize(200, 25), wxTE_READONLY | wxTE_CENTER);
    label2->SetBackgroundColour(wxColour(230, 0, 20));
    label2->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlHBack = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(10, 95), wxSize(200, 205), wxTE_MULTILINE);

    wxTextCtrl* label3 = new wxTextCtrl(nlPanel1, wxID_ANY, "Natural characteristics", wxPoint(220, 70), wxSize(200, 25), wxTE_READONLY | wxTE_CENTER);
    label3->SetBackgroundColour(wxColour(200, 200, 200));
    label3->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlNatural = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(220, 95), wxSize(200, 205), wxTE_MULTILINE);

    wxTextCtrl* label4 = new wxTextCtrl(nlPanel1, wxID_ANY, "Architecture", wxPoint(430, 70), wxSize(200, 25), wxTE_READONLY | wxTE_CENTER);
    label4->SetBackgroundColour(wxColor(230, 0, 20, 0));
    label4->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlArquitecture = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(430, 95), wxSize(200, 205), wxTE_MULTILINE);

    wxTextCtrl* label5 = new wxTextCtrl(nlPanel1, wxID_ANY, "Space type", wxPoint(10, 310), wxSize(200, 25), wxTE_READONLY | wxTE_CENTER);
    label5->SetBackgroundColour(wxColour(200, 200, 200));
    label5->SetFont(wxFont(wxFontInfo(12).Bold()));

    nlPrivate = new wxRadioButton(nlPanel1, wxID_ANY, "Private", wxPoint(10, 340), wxSize(100, 30), wxRB_GROUP);
    nlPrivate->SetFont(wxFont(wxFontInfo(11).Bold()));
    nlPublic = new wxRadioButton(nlPanel1, wxID_ANY, "Public", wxPoint(10, 370), wxSize(100, 30));
    nlPublic->SetFont(wxFont(wxFontInfo(11).Bold()));

    wxTextCtrl* label6 = new wxTextCtrl(nlPanel1, wxID_ANY, "Economy", wxPoint(220, 310), wxSize(200, 25), wxTE_READONLY | wxTE_CENTER);
    label6->SetBackgroundColour(wxColor(230, 0, 20, 0));
    label6->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlEconomy = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(220, 335), wxSize(200, 205), wxTE_MULTILINE);

    wxTextCtrl* label7 = new wxTextCtrl(nlPanel1, wxID_ANY, "Culture", wxPoint(430, 310), wxSize(200, 25), wxTE_READONLY | wxTE_CENTER);
    label7->SetBackgroundColour(wxColour(200, 200, 200));
    label7->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlCulture = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(430, 335), wxSize(200, 205), wxTE_MULTILINE);

    wxTextCtrl* label8 = new wxTextCtrl(nlPanel1, wxID_ANY, "Importance:", wxPoint(10, 555), wxSize(100, 25), wxTE_READONLY | wxTE_LEFT | wxNO_BORDER);
    label8->SetBackgroundColour("WHITE");
    label8->SetFont(wxFont(wxFontInfo(12)));

    nlHigh = new wxRadioButton(nlPanel1, wxID_ANY, "High", wxPoint(110, 545), wxSize(60, 20), wxRB_GROUP);
    nlHigh->SetFont(wxFont(wxFontInfo(10)));
    nlLow = new wxRadioButton(nlPanel1, wxID_ANY, "Low", wxPoint(110, 565), wxSize(60, 20));
    nlLow->SetFont(wxFont(wxFontInfo(10)));

    nlNext = new wxButton(nlPanel1, BUTTON_NextLoc, "Next", wxPoint(440, 550), wxSize(90, 30));
    nlCancel = new wxButton(nlPanel1, BUTTON_CancelLoc, "Cancel", wxPoint(540, 550), wxSize(90, 30));

    SetIcon(wxICON(locationIcon));

    this->Layout();
    this->Show();

    nlPanel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(655, 630));
    nlPanel2->Hide();
    nlPanel2->SetBackgroundColour("WHITE");

    nlBack = new wxButton(nlPanel2, BUTTON_BackLoc, "Back", wxPoint(15, 550), wxSize(90, 30));

    nlChooseImage = new wxButton(nlPanel2, BUTTON_ChooseImageLoc, "Choose image file", wxPoint(355, 430), wxSize(150, 30));
    nlRemoveImage = new wxButton(nlPanel2, BUTTON_RemoveImageLoc, "Remove", wxPoint(135, 430), wxSize(100, 30));
    nlRemoveImage->Hide();
    nlImagePanel = new ImagePanel(nlPanel2, wxPoint(135, 40), wxSize(370, 370));
}

vector<string> LocationCreator::getValues() {
    vector<string> vec;

    vec.push_back((string)nlName->GetValue());
    vec.push_back((string)nlHBack->GetValue());
    vec.push_back((string)nlNatural->GetValue());
    vec.push_back((string)nlArquitecture->GetValue());

    string type;
    if (nlPrivate->GetValue()) {
        vec.push_back((string)nlPrivate->GetLabel());
    } else {
        vec.push_back((string)nlPublic->GetLabel());
    }

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

void LocationCreator::setEdit(Location* location) {
    nlNext->SetId(BUTTON_NextEditLoc);

    nlName->SetValue(location->name);
    nlHBack->SetValue(location->background);
    nlNatural->SetValue(location->natural);
    nlArquitecture->SetValue(location->architecture);

    if (location->type == "Public") {
        nlPublic->SetValue(true);
    }

    nlEconomy->SetValue(location->economy);
    nlCulture->SetValue(location->culture);

    if (location->importance == "Low") {
        nlLow->SetValue(true);
    }

    nlImage = location->image;
    nlImagePanel->setImage(nlImage);

    if (nlImage.IsOk()) {
        nlRemoveImage->Show();
    }

    locationEdit = location;
}

void LocationCreator::nextEdit(wxCommandEvent& event) {
    nlNext->SetId(BUTTON_CreateEditLoc);
    nlNext->Reparent(nlPanel2);
    nlCancel->Reparent(nlPanel2);
    nlBack->SetId(BUTTON_BackEditLoc);
    nlNext->SetLabel("Ok");
    nlPanel1->Hide();
    nlPanel2->Show();
}

void LocationCreator::backEdit(wxCommandEvent& event) {
    nlNext->Reparent(nlPanel1);
    nlNext->SetId(BUTTON_NextEditLoc);
    nlCancel->Reparent(nlPanel1);
    nlNext->SetLabel("Next");
    nlPanel2->Hide();
    nlPanel1->Show();
}

void LocationCreator::edit(wxCommandEvent& event) {
    vector<string> vec = getValues();
    string temp;

    if (nlName->IsModified() || locationEdit->importance != vec[7]) {
        notebook->removeLocName(locationEdit->name);
        temp = locationEdit->importance;
        MainFrame::locations.erase(locationEdit->importance + locationEdit->name);
        Location dummy;
        locationEdit = &dummy;
    }

    locationEdit->name = vec[0];
    locationEdit->background = vec[1];
    locationEdit->natural = vec[2];
    locationEdit->architecture = vec[3];
    locationEdit->type = vec[4];
    locationEdit->economy = vec[5];
    locationEdit->culture = vec[6];
    locationEdit->importance = vec[7];

    locationEdit->image = nlImage;

    if (nlName->IsModified() || locationEdit->importance != temp) {
        notebook->addLocName(locationEdit->name);
        MainFrame::locations[locationEdit->importance + locationEdit->name] = *locationEdit;
        notebook->setSearchAC(wxBookCtrlEvent());
    }

    ElementsNotebook::updateLB();

    MainFrame::isSaved = false;

    this->Destroy();
}

void LocationCreator::next(wxCommandEvent& event) {
    nlNext->SetId(BUTTON_CreateLoc);
    nlNext->Reparent(nlPanel2);
    nlCancel->Reparent(nlPanel2);
    nlNext->SetLabel("Create");
    nlPanel1->Hide();
    nlPanel2->Show();
}

void LocationCreator::back(wxCommandEvent& event) {
    nlNext->SetId(BUTTON_NextLoc);
    nlNext->Reparent(nlPanel1);
    nlCancel->Reparent(nlPanel1);
    nlNext->SetLabel("Next");
    nlPanel2->Hide();
    nlPanel1->Show();
}

void LocationCreator::setImage(wxCommandEvent& event) {
    wxFileDialog* choose = new wxFileDialog(this, "Select a Image", wxEmptyString, wxEmptyString,
        "JPG (*.jpg)|*.jpg",
        wxFD_OPEN, wxDefaultPosition);

    if (choose->ShowModal() == wxID_OK) {
        nlImage.LoadFile(choose->GetPath(), wxBITMAP_TYPE_ANY);
        nlImagePanel->setImage(nlImage);
        nlRemoveImage->Show();
    }
}

void LocationCreator::removeImage(wxCommandEvent& event) {
    nlImagePanel->ClearBackground();
    nlImage.Destroy();
    nlRemoveImage->Hide();
}

void LocationCreator::create(wxCommandEvent& event) {
    if (nlArquitecture->IsModified() || nlName->IsModified() || nlHBack->IsModified() ||
        nlNatural->IsModified() || nlEconomy->IsModified() || nlCulture->IsModified()) {

        std::vector<std::string> vec = getValues();

        Location location;

        location.name = vec[0];
        location.background = vec[1];
        location.natural = vec[2];
        location.architecture = vec[3];
        location.type = vec[4];
        location.economy = vec[5];
        location.culture = vec[6];
        location.importance = vec[7];

        location.image = nlImage;

        MainFrame::locations[vec [7] + vec[0]] = location;

        MainFrame::saved[1] = MainFrame::locations.size();

        ElementsNotebook::updateLB();

        MainFrame::isSaved = false;
    }

    Destroy();
}

void LocationCreator::checkClose(wxCloseEvent& event) {

    if (nlArquitecture->IsModified() || nlName->IsModified() || nlHBack->IsModified() ||
        nlNatural->IsModified() || nlEconomy->IsModified() || nlCulture->IsModified()) {

        wxMessageDialog* check = new wxMessageDialog(this, "Are you sure you want to close?", "Close", wxYES_NO | wxNO_DEFAULT);

        if (check->ShowModal() == wxID_YES) {
            this->Destroy();
        } else {
            check->Destroy();
        }
    } else {
        this->Destroy();
    }
}

void LocationCreator::cancel(wxCommandEvent& event) {
    this->Destroy();
}
