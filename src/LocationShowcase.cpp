#include "LocationShowcase.h"

#include "wxmemdbg.h"

LocationShowcase::LocationShowcase(wxWindow* parent) :
    wxScrolledWindow(parent, wxID_ANY) {

    wxFont font(wxFontInfo(12).Bold());
    wxFont font2(wxFontInfo(9));

    m_importance = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 25), wxALIGN_CENTER | wxBORDER_RAISED | wxST_NO_AUTORESIZE);
    m_importance->SetBackgroundColour(wxColour(220, 220, 220));
    m_importance->SetFont(font);
    
    m_image = new ImagePanel(this, wxDefaultPosition, wxSize(200, 200));
    m_image->SetBackgroundColour(wxColour(150, 150, 150));
    m_image->setBorderColour(wxColour(20, 20, 20));
    m_image->Hide();

    m_name = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxALIGN_CENTER | wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    m_name->SetBackgroundColour(wxColour(255, 255, 255));
    m_name->SetFont(font);

    wxStaticText* label1 = new wxStaticText(this, -1, "Historical background", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label1->SetFont(font);
    label1->SetBackgroundColour(wxColour(180, 180, 180));
    m_background = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    m_background->SetFont(font2);
    m_background->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label2 = new wxStaticText(this, -1, "Natural Aspects", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label2->SetFont(font);
    label2->SetBackgroundColour(wxColour(180, 180, 180));
    m_natural = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    m_natural->SetFont(font2);
    m_natural->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label3 = new wxStaticText(this, -1, "Architecture", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label3->SetFont(font);
    label3->SetBackgroundColour(wxColour(180, 180, 180));
    m_architecture = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    m_architecture->SetFont(font2);
    m_architecture->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label4 = new wxStaticText(this, -1, "Economy", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label4->SetFont(font);
    label4->SetBackgroundColour(wxColour(180, 180, 180));
    m_economy = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    m_economy->SetFont(font2);
    m_economy->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label5 = new wxStaticText(this, -1, "Culture", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    label5->SetFont(font);
    label5->SetBackgroundColour(wxColour(180, 180, 180));
    m_culture = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    m_culture->SetFont(font2);
    m_culture->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label6 = new wxStaticText(this, -1, "Space Type", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_DOUBLE);
    label6->SetFont(font);
    label6->SetBackgroundColour(wxColour(180, 180, 180));
    m_type = new wxStaticText(this, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    m_type->SetFont(wxFontInfo(11).Bold());
    m_type->SetBackgroundColour(wxColour(50, 50, 50));
    m_type->SetForegroundColour(wxColour(255, 255, 255));
    
    m_type->Hide();

    m_firstColumn = new wxBoxSizer(wxVERTICAL);
    m_firstColumn->Add(label1, wxSizerFlags(0));
    m_firstColumn->Add(m_background, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_firstColumn->Add(label3, wxSizerFlags(0));
    m_firstColumn->Add(m_architecture, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_firstColumn->Add(label5, wxSizerFlags(0));
    m_firstColumn->Add(m_culture, wxSizerFlags(0).Expand());

    m_secondColumn = new wxBoxSizer(wxVERTICAL);
    m_secondColumn->Add(label2, wxSizerFlags(0));
    m_secondColumn->Add(m_natural, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_secondColumn->Add(label4, wxSizerFlags(0));
    m_secondColumn->Add(m_economy, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_secondColumn->Add(label6, wxSizerFlags(0).Expand().Border(wxBOTTOM, 5));
    m_secondColumn->Add(m_type, wxSizerFlags(0).CenterHorizontal());

    wxBoxSizer* horSizer = new wxBoxSizer(wxHORIZONTAL);
    horSizer->Add(m_firstColumn, wxSizerFlags(1).Expand().Border(wxRIGHT, 10));
    horSizer->Add(m_secondColumn, wxSizerFlags(1).Expand().Border(wxLEFT, 10));

    m_vertical = new  wxBoxSizer(wxVERTICAL);
    m_vertical->Add(m_importance, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    m_vertical->Add(m_image, wxSizerFlags(0).CenterHorizontal().Shaped());
    m_vertical->SetItemMinSize(size_t(1), wxSize(200, 200));
    m_vertical->Add(m_name, wxSizerFlags(0).Expand().Border(wxALL, 15));
    m_vertical->Add(horSizer, wxSizerFlags(1).Expand().Border(wxALL, 15));

    m_background->SetCursor(wxCURSOR_DEFAULT);
    m_natural->SetCursor(wxCURSOR_DEFAULT);
    m_architecture->SetCursor(wxCURSOR_DEFAULT);
    m_economy->SetCursor(wxCURSOR_DEFAULT);
    m_culture->SetCursor(wxCURSOR_DEFAULT);

    SetSizer(m_vertical);

    m_vertical->FitInside(this);
    this->SetScrollRate(15, 15);
}

void LocationShowcase::setData(wxImage& set, vector<string> locData) {
    m_image->Show(m_image->setImage(set));
    m_name->SetLabel(locData[0]);
    
    if (locData[7] == "High") {
        m_importance->SetBackgroundColour(wxColour(230, 60, 60));
        m_importance->SetLabel("Main");
    } else {
        m_importance->SetBackgroundColour(wxColour(220, 220, 220));

        if (locData[7] == "Low")
            m_importance->SetLabel("Secondary");
        else
            m_importance->SetLabel("");
    }

    m_background->SetValue(locData[1]);
    m_natural->SetValue(locData[2]);
    m_architecture->SetValue(locData[3]);
    m_economy->SetValue(locData[5]);
    m_culture->SetValue(locData[6]);

    m_type->SetLabel(locData[4]);
    if (locData[4] == "Private")
        m_type->SetBackgroundColour(wxColour(0, 0, 100));
    else if (locData[4] == "Public")
        m_type->SetBackgroundColour(wxColour(100, 0, 0));
    else
        m_type->SetBackgroundColour(wxColour(50, 50, 50));

    m_type->Show(!locData[4].empty());

    int nol;

    nol = m_background->GetNumberOfLines();
    if (nol > 5)
        m_firstColumn->SetItemMinSize(size_t(1), wxSize(-1, nol * 16));
    else
        m_firstColumn->SetItemMinSize(size_t(1), wxSize(-1, 80));

    nol = m_natural->GetNumberOfLines();
    if (nol > 5)
        m_secondColumn->SetItemMinSize(size_t(1), wxSize(-1, nol * 16));
    else
        m_secondColumn->SetItemMinSize(size_t(1), wxSize(-1, 80));

    nol = m_architecture->GetNumberOfLines();
    if (nol > 5)
        m_firstColumn->SetItemMinSize(size_t(3), wxSize(-1, nol * 16));
    else
        m_firstColumn->SetItemMinSize(size_t(3), wxSize(-1, 80));

    nol = m_economy->GetNumberOfLines();
    if (nol > 5)
        m_secondColumn->SetItemMinSize(size_t(3), wxSize(-1, nol * 16));
    else
        m_secondColumn->SetItemMinSize(size_t(3), wxSize(-1, 80));

    nol = m_culture->GetNumberOfLines();
    if (nol > 5)
        m_firstColumn->SetItemMinSize(size_t(5), wxSize(-1, nol * 16));
    else
        m_firstColumn->SetItemMinSize(size_t(5), wxSize(-1, 80));

    m_vertical->FitInside(this);
    m_vertical->Layout();
    Refresh();
}
