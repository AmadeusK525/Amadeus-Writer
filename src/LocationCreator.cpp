#include "LocationCreator.h"
#include "MyApp.h"

#include "ElementsNotebook.h"
#include "LocationShowcase.h"

#include "Outline.h"
#include "OutlineFiles.h"

#include <wx\scrolwin.h>

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(LocationCreator, wxFrame)

EVT_CLOSE(LocationCreator::checkClose)

EVT_BUTTON(BUTTON_ChooseImageLoc, LocationCreator::setImage)
EVT_BUTTON(BUTTON_RemoveImageLoc, LocationCreator::removeImage)

END_EVENT_TABLE()

using std::vector;
using std::string;

LocationCreator::LocationCreator(wxWindow* parent, ElementsNotebook* notebook, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style) :
    wxFrame(parent, id, "Create location", pos, size, style) {

    this->mainFrame = (MainFrame*)wxGetApp().GetTopWindow();
    this->notebook = notebook;
    this->CenterOnParent();

    wxColour dark(50, 50, 50);
    wxColour darker(30, 30, 30);
    wxColour grey(200, 200, 200);
    wxColour red(230, 0, 20);
    wxColour txtCol(255, 255, 255);

    wxFont font(wxFontInfo(9));

    SetBackgroundColour(darker);

    nlPanel1 = new wxPanel(this, wxID_ANY);
    nlPanel1->SetBackgroundColour(darker);
    nlPanel1->Show();

    wxPanel* panel = new wxPanel(nlPanel1, wxID_ANY, wxPoint(80, 20), wxSize(130, 35));
    panel->SetBackgroundColour(red);

    wxStaticText* label1 = new wxStaticText(panel, wxID_ANY, "Name of location:",
        wxDefaultPosition, wxSize(200, 25), wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxNO_BORDER);
    label1->SetBackgroundColour(darker);
    label1->SetForegroundColour(txtCol);
    label1->SetFont(wxFont(wxFontInfo(12)));

    nlName = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(220, 25),
        wxSize(330, 25), wxBORDER_SIMPLE);
    nlName->SetBackgroundColour(dark);
    nlName->SetForegroundColour(txtCol);

    nlName->SetFont(wxFont(wxFontInfo(10)));

    wxStaticText* label8 = new wxStaticText(nlPanel1, wxID_ANY, "Importance:",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_SIMPLE);
    label8->SetBackgroundColour(grey);
    label8->SetFont(wxFont(wxFontInfo(12).Bold()));

    nlHigh = new wxRadioButton(nlPanel1, wxID_ANY, "High", wxPoint(115, 545), wxSize(60, 20), wxRB_GROUP);
    nlHigh->SetFont(wxFont(wxFontInfo(10)));
    nlHigh->SetForegroundColour(txtCol);

    nlLow = new wxRadioButton(nlPanel1, wxID_ANY, "Low", wxPoint(115, 565), wxSize(60, 20));
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

    wxStaticText* label2 = new wxStaticText(nlPanel1, wxID_ANY, "General",
        wxPoint(10, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label2->SetBackgroundColour(red);
    label2->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlGeneral = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(10, 95),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlGeneral->SetBackgroundColour(dark);
    nlGeneral->SetForegroundColour(txtCol);
    nlGeneral->SetFont(font);

    wxStaticText* label3 = new wxStaticText(nlPanel1, wxID_ANY, "Natural characteristics",
        wxPoint(220, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label3->SetBackgroundColour(grey);
    label3->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlNatural = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(220, 95),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlNatural->SetBackgroundColour(dark);
    nlNatural->SetForegroundColour(txtCol);
    nlNatural->SetFont(font);

    wxStaticText* label4 = new wxStaticText(nlPanel1, wxID_ANY, "Architecture",
        wxPoint(430, 70), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label4->SetBackgroundColour(wxColor(230, 0, 20, 0));
    label4->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlArchitecture = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(430, 95),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlArchitecture->SetBackgroundColour(dark);
    nlArchitecture->SetForegroundColour(txtCol);
    nlArchitecture->SetFont(font);

    wxStaticText* label5 = new wxStaticText(nlPanel1, wxID_ANY, "Politics",
        wxPoint(10, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label5->SetBackgroundColour(grey);
    label5->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlPolitics = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlPolitics->SetBackgroundColour(dark);
    nlPolitics->SetForegroundColour(txtCol);
    nlPolitics->SetFont(font);

    label6 = new wxStaticText(nlPanel1, wxID_ANY, "Economy",
        wxPoint(220, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label6->SetBackgroundColour(red);
    label6->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlEconomy = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(220, 335),
        wxSize(200, 205), wxTE_MULTILINE | wxBORDER_SIMPLE);
    nlEconomy->SetBackgroundColour(dark);
    nlEconomy->SetForegroundColour(txtCol);
    nlEconomy->SetFont(font);

    wxStaticText* label7 = new wxStaticText(nlPanel1, wxID_ANY, "Culture",
        wxPoint(430, 310), wxSize(200, 25), wxALIGN_CENTER | wxBORDER_SIMPLE);
    label7->SetBackgroundColour(grey);
    label7->SetFont(wxFont(wxFontInfo(12).Bold()));
    nlCulture = new wxTextCtrl(nlPanel1, wxID_ANY, wxEmptyString, wxPoint(430, 335),
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

    nlPanel1->SetSizer(verSizer1);

    nlPanel2 = new wxScrolledWindow(this);
    nlPanel2->Hide();
    nlPanel2->SetBackgroundColour(darker);
    nlPanel2->SetScrollRate(20, 20);

    wxStaticText* customLabel = new wxStaticText(nlPanel2, -1, "Custom attributes");
    customLabel->SetBackgroundColour(darker);
    customLabel->SetForegroundColour(wxColour(245, 245, 245));
    customLabel->SetFont(wxFontInfo(15).Bold());

    wxButton* addCustom = new wxButton(nlPanel2, -1, "", wxDefaultPosition, wxSize(25, 25));
    addCustom->SetBitmap(wxBITMAP_PNG(plus));
    addCustom->Bind(wxEVT_BUTTON, &LocationCreator::addCustomAttr, this);

    wxBoxSizer* p2line = new wxBoxSizer(wxHORIZONTAL);
    p2line->Add(customLabel, wxSizerFlags());
    p2line->AddStretchSpacer();
    p2line->Add(addCustom, wxSizerFlags());

    m_customSizer = new wxWrapSizer(wxHORIZONTAL);

    wxBoxSizer* p2ver = new wxBoxSizer(wxVERTICAL);
    p2ver->Add(p2line, wxSizerFlags().Expand().Border(wxALL, 10));
    p2ver->AddSpacer(10);
    p2ver->Add(m_customSizer, wxSizerFlags(1).Expand().Border(wxLEFT, 5));

    nlPanel2->SetSizer(p2ver);
    p2ver->FitInside(nlPanel2);

    nlPanel3 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(655, 630));
    nlPanel3->Hide();
    nlPanel3->SetBackgroundColour(darker);

    nlChooseImage = new wxButton(nlPanel3, BUTTON_ChooseImageLoc, "Choose image file", wxPoint(355, 430), wxSize(150, 30));
    nlRemoveImage = new wxButton(nlPanel3, BUTTON_RemoveImageLoc, "Remove", wxPoint(135, 430), wxSize(100, 30));
    nlRemoveImage->Hide();
    nlImagePanel = new ImagePanel(nlPanel3, wxPoint(135, 40), wxSize(370, 370));
    nlImagePanel->setBorderColour(darker);

    wxBoxSizer* btnSizer2 = new wxBoxSizer(wxHORIZONTAL);
    btnSizer2->AddStretchSpacer(1);
    btnSizer2->Add(nlRemoveImage, wxSizerFlags(0));
    btnSizer2->AddStretchSpacer(5);
    btnSizer2->Add(nlChooseImage, wxSizerFlags(0));
    btnSizer2->AddStretchSpacer(1);

    wxBoxSizer* verSizer3 = new wxBoxSizer(wxVERTICAL);
    verSizer3->Add(nlImagePanel, wxSizerFlags(0).Border(wxTOP, 40).CenterHorizontal());
    verSizer3->AddStretchSpacer(1);
    verSizer3->Add(btnSizer2, wxSizerFlags(0).Expand());
    verSizer3->AddStretchSpacer(3);

    nlPanel3->SetSizer(verSizer3);

    wxPanel* btnPan = new wxPanel(this);

    nlNext = new wxButton(btnPan, BUTTON_NextLoc1, "Next", wxPoint(440, 550), wxSize(90, 30));
    nlCancel = new wxButton(btnPan, BUTTON_CancelLoc, "Cancel", wxPoint(540, 550), wxSize(90, 30));
    nlBack = new wxButton(btnPan, BUTTON_BackLoc2, "Back", wxPoint(15, 550), wxSize(90, 30));

    nlNext->Bind(wxEVT_BUTTON, &LocationCreator::next, this);
    nlBack->Bind(wxEVT_BUTTON, &LocationCreator::back, this);
    nlCancel->Bind(wxEVT_BUTTON, &LocationCreator::cancel, this);

    nlBack->Hide();

    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(nlBack, wxSizerFlags());
    btnSizer->AddStretchSpacer();
    btnSizer->Add(nlNext, wxSizerFlags());
    btnSizer->AddSpacer(10);
    btnSizer->Add(nlCancel, wxSizerFlags());

    btnPan->SetSizer(btnSizer);

    m_mainSiz = new wxBoxSizer(wxVERTICAL);
    m_mainSiz->Add(nlPanel1, wxSizerFlags(1).Expand());
    m_mainSiz->Add(nlPanel2, wxSizerFlags(1).Expand());
    m_mainSiz->Add(nlPanel3, wxSizerFlags(1).Expand());
    m_mainSiz->Add(btnPan, wxSizerFlags(0).Expand().Border(wxALL, 10));

    SetSizer(m_mainSiz);
    SetIcon(wxICON(locationIcon));

    m_mainSiz->Layout();
    this->Show();
}

vector<string> LocationCreator::getValues() {
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

vector<pair<string, string>> LocationCreator::getCustom() {
    vector<pair<string, string>> vec;
    
    string content;
    string label;
    for (auto it : nlCustom) {
        label = it.first->GetValue();
        content = it.second->GetValue();
     
        vec.push_back(pair<string, string>(label, content));
    }

    return vec;
}

void LocationCreator::setEdit(Location* location) {
    nlNext->SetId(BUTTON_NextEditLoc1);
    nlBack->SetId(BUTTON_BackEditLoc1);

    nlName->SetValue(location->name);
    nlGeneral->SetValue(location->general);
    nlNatural->SetValue(location->natural);
    nlArchitecture->SetValue(location->architecture);
    nlPolitics->SetValue(location->politics);
    nlEconomy->SetValue(location->economy);
    nlCulture->SetValue(location->culture);

    if (location->importance == "Low") {
        nlLow->SetValue(true);
    }

    int i = 0;
    for (auto it : location->custom) {
        addCustomAttr(wxCommandEvent());
        nlCustom[i].first->SetValue(it.first);
        nlCustom[i].second->SetValue(it.second);

        i++;
    }

    nlImage = location->image;
    if (nlImagePanel->setImage(nlImage)) {
        nlRemoveImage->Show();
    }

    m_locationEdit = location;
}

void LocationCreator::doEdit(wxCommandEvent& WXUNUSED(event)) {
    mainFrame->getOutline()->getOutlineFiles()->deleteLocation(*m_locationEdit);

    vector<string> vec = getValues();
    string temp;

    if (nlName->IsModified() || m_locationEdit->importance != vec[7]) {
        notebook->removeLocName(m_locationEdit->name);
        temp = m_locationEdit->importance;
        mainFrame->locations.erase(m_locationEdit->importance + m_locationEdit->name);
        Location dummy;
        m_locationEdit = &dummy;
    }

    m_locationEdit->name = vec[0];
    m_locationEdit->general = vec[1];
    m_locationEdit->natural = vec[2];
    m_locationEdit->architecture = vec[3];
    m_locationEdit->politics = vec[4];
    m_locationEdit->economy = vec[5];
    m_locationEdit->culture = vec[6];
    m_locationEdit->importance = vec[7];

    m_locationEdit->custom = getCustom();

    m_locationEdit->image = nlImage;

    if (nlName->IsModified() || m_locationEdit->importance != temp) {
        notebook->addLocName(m_locationEdit->name);
        MainFrame::locations[m_locationEdit->importance + m_locationEdit->name] = *m_locationEdit;
        notebook->setSearchAC(wxBookCtrlEvent());
    }

    notebook->updateLB();

    int n = 0;
    for (auto it : mainFrame->locations) {
        if (m_locationEdit->name == it.second.name)
            break;

        n++;
    }

    notebook->charList->Select(n);

    mainFrame->isSaved = false;
    mainFrame->Enable();
    mainFrame->getOutline()->getOutlineFiles()->appendLocation(*m_locationEdit);

    this->Destroy();
}

void LocationCreator::next(wxCommandEvent& event) {
    bool show1 = false, show2 = false, show3 = false;

    switch (event.GetId()) {
    case BUTTON_NextLoc1:
        nlNext->SetId(BUTTON_NextLoc2);
        nlBack->SetId(BUTTON_BackLoc1);
        nlBack->Show();
        show2 = true;
        break;

    case BUTTON_NextLoc2:
        nlNext->SetId(BUTTON_CreateLoc);
        nlNext->SetLabel("Create");
        nlBack->SetId(BUTTON_BackLoc2);
        show3 = true;
        break;

    case BUTTON_NextEditLoc1:
        nlNext->SetId(BUTTON_NextEditLoc2);
        nlBack->Show();
        show2 = true;
        break;

    case BUTTON_NextEditLoc2:
        nlNext->SetId(BUTTON_CreateEditLoc);
        nlNext->SetLabel("Ok");
        nlBack->SetId(BUTTON_BackEditLoc2);
        show3 = true;
        break;

    case BUTTON_CreateLoc:
        create(event);
        return;

    case BUTTON_CreateEditLoc:
        doEdit(event);
        return;
    }

    nlPanel1->Show(show1);
    nlPanel2->Show(show2);
    nlPanel3->Show(show3);

    m_mainSiz->Layout();
    event.Skip();
}

void LocationCreator::back(wxCommandEvent& event) {
    bool show1 = false, show2 = false, show3 = false;

    switch (event.GetId()) {
    case BUTTON_BackLoc1:
        nlNext->SetId(BUTTON_NextLoc1);
        nlBack->Hide();
        show1 = true;
        break;

    case BUTTON_BackLoc2:
        nlNext->SetLabel("Next");
        nlNext->SetId(BUTTON_NextLoc2);
        nlBack->SetId(BUTTON_BackLoc1);
        show2 = true;
        break;

    case BUTTON_BackEditLoc1:
        nlNext->SetId(BUTTON_NextEditLoc1);
        nlBack->Hide();
        show1 = true;
        break;

    case BUTTON_BackEditLoc2:
        nlNext->SetId(BUTTON_NextEditLoc2);
        nlNext->SetLabel("Next");
        nlBack->SetId(BUTTON_BackEditLoc1);
        show2 = true;
        break;
    }

    nlPanel1->Show(show1);
    nlPanel2->Show(show2);
    nlPanel3->Show(show3);

    m_mainSiz->Layout();
    event.Skip();
}

void LocationCreator::setImage(wxCommandEvent& WXUNUSED(event)) {
    wxFileDialog choose(this, "Select a Image", wxEmptyString, wxEmptyString,
        "JPG, JPEG and PNG(*.jpg;*.jpeg;*.png)|*.jpg;*.jpeg;*.png",
        wxFD_OPEN, wxDefaultPosition);

    if (choose.ShowModal() == wxID_OK) {
        nlImage.LoadFile(choose.GetPath(), wxBITMAP_TYPE_ANY);
        nlImagePanel->setImage(nlImage);
        nlRemoveImage->Show();
    }

    m_mainSiz->Layout();
}

void LocationCreator::removeImage(wxCommandEvent& WXUNUSED(event)) {
    nlImagePanel->ClearBackground();
    nlImage.Destroy();
    nlRemoveImage->Hide();
}

void LocationCreator::addCustomAttr(wxCommandEvent& WXUNUSED(event)) {
    wxPanel* panel = new wxPanel(nlPanel2);
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
    minus->Bind(wxEVT_BUTTON, &LocationCreator::removeCustomAttr, this);

    wxBoxSizer* hor = new wxBoxSizer(wxHORIZONTAL);
    hor->Add(label, wxSizerFlags(1));
    hor->Add(minus, wxSizerFlags(0).Expand());

    wxBoxSizer* ver = new wxBoxSizer(wxVERTICAL);
    ver->Add(hor, wxSizerFlags(0).Expand());
    ver->Add(content, wxSizerFlags(1).Expand());

    panel->SetSizer(ver);

    m_customSizer->Add(panel, wxSizerFlags(0).Border(wxALL, 5));
    m_mainSiz->Layout();

    nlCustom.push_back(pair<wxTextCtrl*, wxTextCtrl*>(label, content));
    m_minusButtons.push_back(minus);

    recolorCustoms();
    Refresh();
}

void LocationCreator::removeCustomAttr(wxCommandEvent& event) {
    Freeze();
    wxButton* minus = (wxButton*)event.GetEventObject();

    auto it1 = nlCustom.begin();
    auto it2 = m_minusButtons.begin();

    for (it2 = m_minusButtons.begin(); it2 != m_minusButtons.end(); it2++) {
        if (*it2 == minus)
            break;

        it1++;
    }

    nlCustom.erase(it1);
    m_minusButtons.erase(it2);

    minus->GetParent()->Destroy();
    m_mainSiz->Layout();

    recolorCustoms();
    Thaw();
}

void LocationCreator::recolorCustoms() {
    wxColour red(230, 0, 20);
    wxColour grey(200, 200, 200);

    bool doRed = true;
    for (auto it : nlCustom) {
        if (doRed)
            it.first->SetBackgroundColour(red);
        else
            it.first->SetBackgroundColour(grey);

        doRed = !doRed;
    }
}

void LocationCreator::create(wxCommandEvent& WXUNUSED(event)) {
    if (!nlName->IsEmpty()) {

        std::vector<std::string> vec = getValues();

        Location location;

        location.name = vec[0];
        location.general = vec[1];
        location.natural = vec[2];
        location.architecture = vec[3];
        location.politics = vec[4];
        location.economy = vec[5];
        location.culture = vec[6];
        location.importance = vec[7];

        location.image = nlImage;

        location.custom = getCustom();

        mainFrame->locations[vec[7] + vec[0]] = location;
        mainFrame->saved[1] = MainFrame::locations.size();

        ElementsNotebook::updateLB();

        mainFrame->isSaved = false;
        mainFrame->getOutline()->getOutlineFiles()->appendLocation(location);
    } else {
        wxMessageBox("You can't create a location with no name!");
        return;
    }

    mainFrame->Enable();
    Destroy();
}

void LocationCreator::checkClose(wxCloseEvent& WXUNUSED(event)) {

    if (nlArchitecture->IsModified() || nlName->IsModified() || nlGeneral->IsModified() ||
        nlNatural->IsModified() || nlEconomy->IsModified() || nlCulture->IsModified()) {

        wxMessageDialog* check = new wxMessageDialog(this, "Are you sure you want to close?", "Close", wxYES_NO | wxNO_DEFAULT);

        if (check->ShowModal() == wxID_YES) {
            mainFrame->Enable();
            this->Destroy();
        } else {
            check->Destroy();
        }
    } else {
        mainFrame->Enable();
        this->Destroy();
    }
}

void LocationCreator::cancel(wxCommandEvent& WXUNUSED(event)) {
    mainFrame->Enable();
    this->Destroy();
}