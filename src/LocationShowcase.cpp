#include "LocationShowcase.h"

#include "wxmemdbg.h"

LocationShowcase::LocationShowcase(wxWindow* parent) :
    wxScrolledWindow(parent, wxID_ANY) {

    importance = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 25), wxALIGN_CENTER | wxBORDER_RAISED);
    importance->SetBackgroundColour(wxColour(220, 220, 220));
    importance->SetFont(wxFont(wxFontInfo(12).Bold()));
    
    image = new ImagePanel(this, wxDefaultPosition, wxSize(200, 200));
    image->SetBackgroundColour(wxColour(150, 150, 150));
    image->setBorderColour(wxColour(20, 20, 20));
    image->Hide();

    name = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxALIGN_CENTER | wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    name->SetBackgroundColour(wxColour(255, 255, 255));
    name->SetFont(wxFont(wxFontInfo(12).Bold()));

    wxStaticText* label1 = new wxStaticText(this, -1, "Historical background", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label1->SetFont(wxFontInfo(12).Bold());
    label1->SetBackgroundColour(wxColour(180, 180, 180));
    background = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    background->SetFont(wxFontInfo(9));
    background->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label2 = new wxStaticText(this, -1, "Natural Aspects", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label2->SetFont(wxFontInfo(12).Bold());
    label2->SetBackgroundColour(wxColour(180, 180, 180));
    natural = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    natural->SetFont(wxFontInfo(9));
    natural->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label3 = new wxStaticText(this, -1, "Architecture", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label3->SetFont(wxFontInfo(12).Bold());
    label3->SetBackgroundColour(wxColour(180, 180, 180));
    architecture = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    architecture->SetFont(wxFontInfo(9));
    architecture->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label4 = new wxStaticText(this, -1, "Economy", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label4->SetFont(wxFontInfo(12).Bold());
    label4->SetBackgroundColour(wxColour(180, 180, 180));
    economy = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    economy->SetFont(wxFontInfo(9));
    economy->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label5 = new wxStaticText(this, -1, "Culture", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label5->SetFont(wxFontInfo(12).Bold());
    label5->SetBackgroundColour(wxColour(180, 180, 180));
    culture = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    culture->SetFont(wxFontInfo(9));
    culture->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label6 = new wxStaticText(this, -1, "Space Type", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_DOUBLE);
    label6->SetFont(wxFontInfo(12).Bold());
    label6->SetBackgroundColour(wxColour(180, 180, 180));
    type = new wxStaticText(this, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    type->SetFont(wxFontInfo(10).Bold());
    type->SetBackgroundColour(wxColour(50, 50, 50));
    type->SetForegroundColour(wxColour(255, 255, 255));
    
    type->Hide();

    wxBoxSizer* firstColumn = new wxBoxSizer(wxVERTICAL);
    firstColumn->Add(label1, wxSizerFlags(0));
    firstColumn->Add(background, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    firstColumn->Add(label3, wxSizerFlags(0));
    firstColumn->Add(architecture, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    firstColumn->Add(label5, wxSizerFlags(0));
    firstColumn->Add(culture, wxSizerFlags(0).Expand());

    wxBoxSizer* secondColumn = new wxBoxSizer(wxVERTICAL);
    secondColumn->Add(label2, wxSizerFlags(0));
    secondColumn->Add(natural, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    secondColumn->Add(label4, wxSizerFlags(0));
    secondColumn->Add(economy, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    secondColumn->Add(label6, wxSizerFlags(0).Expand().Border(wxBOTTOM, 5));
    secondColumn->Add(type, wxSizerFlags(0).CenterHorizontal());

    wxBoxSizer* horSizer = new wxBoxSizer(wxHORIZONTAL);
    horSizer->Add(firstColumn, wxSizerFlags(1).Expand().Border(wxRIGHT, 10));
    horSizer->Add(secondColumn, wxSizerFlags(1).Expand().Border(wxLEFT, 10));

    vertical = new  wxBoxSizer(wxVERTICAL);
    vertical->Add(importance, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    vertical->Add(image, wxSizerFlags(0).CenterHorizontal().Shaped());
    vertical->SetItemMinSize(size_t(1), wxSize(200, 200));
    vertical->Add(name, wxSizerFlags(0).Expand().Border(wxALL, 15));
    vertical->Add(horSizer, wxSizerFlags(1).Expand().Border(wxALL, 15));

    SetSizer(vertical);

    this->FitInside();
    this->SetScrollRate(15, 15);
}

void LocationShowcase::setData(wxImage& set, vector<string> locData) {
    image->Show(image->setImage(set));

    name->SetLabel(locData[0]);

    vertical->Layout();
    FitInside();
    Refresh();
}
