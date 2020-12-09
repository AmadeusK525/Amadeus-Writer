#include "LocationShowcase.h"

#include "wxmemdbg.h"

LocationShowcase::LocationShowcase(wxWindow* parent) :
    wxScrolledWindow(parent, wxID_ANY) {

    wxFont font(wxFontInfo(12).Bold());
    wxFont font2(wxFontInfo(9));

    importance = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 25), wxALIGN_CENTER | wxBORDER_RAISED | wxST_NO_AUTORESIZE);
    importance->SetBackgroundColour(wxColour(220, 220, 220));
    importance->SetFont(font);
    
    image = new ImagePanel(this, wxDefaultPosition, wxSize(200, 200));
    image->SetBackgroundColour(wxColour(150, 150, 150));
    image->setBorderColour(wxColour(20, 20, 20));
    image->Hide();

    name = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxALIGN_CENTER | wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    name->SetBackgroundColour(wxColour(255, 255, 255));
    name->SetFont(font);

    wxStaticText* label1 = new wxStaticText(this, -1, "Historical background", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label1->SetFont(font);
    label1->SetBackgroundColour(wxColour(180, 180, 180));
    background = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    background->SetFont(font2);
    background->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label2 = new wxStaticText(this, -1, "Natural Aspects", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label2->SetFont(font);
    label2->SetBackgroundColour(wxColour(180, 180, 180));
    natural = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    natural->SetFont(font2);
    natural->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label3 = new wxStaticText(this, -1, "Architecture", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label3->SetFont(font);
    label3->SetBackgroundColour(wxColour(180, 180, 180));
    architecture = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    architecture->SetFont(font2);
    architecture->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label4 = new wxStaticText(this, -1, "Economy", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label4->SetFont(font);
    label4->SetBackgroundColour(wxColour(180, 180, 180));
    economy = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    economy->SetFont(font2);
    economy->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label5 = new wxStaticText(this, -1, "Culture", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label5->SetFont(font);
    label5->SetBackgroundColour(wxColour(180, 180, 180));
    culture = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    culture->SetFont(font2);
    culture->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label6 = new wxStaticText(this, -1, "Space Type", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_DOUBLE);
    label6->SetFont(font);
    label6->SetBackgroundColour(wxColour(180, 180, 180));
    type = new wxStaticText(this, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    type->SetFont(wxFontInfo(11).Bold());
    type->SetBackgroundColour(wxColour(50, 50, 50));
    type->SetForegroundColour(wxColour(255, 255, 255));
    
    type->Hide();

    firstColumn = new wxBoxSizer(wxVERTICAL);
    firstColumn->Add(label1, wxSizerFlags(0));
    firstColumn->Add(background, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    firstColumn->Add(label3, wxSizerFlags(0));
    firstColumn->Add(architecture, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    firstColumn->Add(label5, wxSizerFlags(0));
    firstColumn->Add(culture, wxSizerFlags(0).Expand());

    secondColumn = new wxBoxSizer(wxVERTICAL);
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

    background->SetCursor(wxCURSOR_DEFAULT);
    natural->SetCursor(wxCURSOR_DEFAULT);
    architecture->SetCursor(wxCURSOR_DEFAULT);
    economy->SetCursor(wxCURSOR_DEFAULT);
    culture->SetCursor(wxCURSOR_DEFAULT);

    SetSizer(vertical);

    this->FitInside();
    this->SetScrollRate(15, 15);
}

void LocationShowcase::setData(wxImage& set, vector<string> locData) {
    image->Show(image->setImage(set));
    name->SetLabel(locData[0]);
    
    if (locData[7] == "High") {
        importance->SetBackgroundColour(wxColour(230, 60, 60));
        importance->SetLabel("Main");
    } else {
        importance->SetBackgroundColour(wxColour(220, 220, 220));

        if (locData[7] == "Low")
            importance->SetLabel("Secondary");
        else
            importance->SetLabel("");
    }

    background->SetValue(locData[1]);
    natural->SetValue(locData[2]);
    architecture->SetValue(locData[3]);
    economy->SetValue(locData[5]);
    culture->SetValue(locData[6]);

    type->SetLabel(locData[4]);
    if (locData[4] == "Private")
        type->SetBackgroundColour(wxColour(0, 0, 100));
    else if (locData[4] == "Public")
        type->SetBackgroundColour(wxColour(100, 0, 0));
    else
        type->SetBackgroundColour(wxColour(50, 50, 50));

    type->Show(!locData[4].empty());

    int nol;

    nol = background->GetNumberOfLines();
    if (nol > 5)
        firstColumn->SetItemMinSize(size_t(1), wxSize(-1, nol * 16));
    else
        firstColumn->SetItemMinSize(size_t(1), wxSize(-1, 80));

    nol = natural->GetNumberOfLines();
    if (nol > 5)
        secondColumn->SetItemMinSize(size_t(1), wxSize(-1, nol * 16));
    else
        secondColumn->SetItemMinSize(size_t(1), wxSize(-1, 80));

    nol = architecture->GetNumberOfLines();
    if (nol > 5)
        firstColumn->SetItemMinSize(size_t(3), wxSize(-1, nol * 16));
    else
        firstColumn->SetItemMinSize(size_t(3), wxSize(-1, 80));

    nol = economy->GetNumberOfLines();
    if (nol > 5)
        secondColumn->SetItemMinSize(size_t(3), wxSize(-1, nol * 16));
    else
        secondColumn->SetItemMinSize(size_t(3), wxSize(-1, 80));

    nol = culture->GetNumberOfLines();
    if (nol > 5)
        firstColumn->SetItemMinSize(size_t(5), wxSize(-1, nol * 16));
    else
        firstColumn->SetItemMinSize(size_t(5), wxSize(-1, 80));

    vertical->Layout();
    FitInside();
    Refresh();
}
