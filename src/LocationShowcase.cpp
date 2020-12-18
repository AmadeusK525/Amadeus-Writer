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
    m_firstColumn->Add(m_culture, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));

    m_secondColumn = new wxBoxSizer(wxVERTICAL);
    m_secondColumn->Add(label2, wxSizerFlags(0));
    m_secondColumn->Add(m_natural, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_secondColumn->Add(label4, wxSizerFlags(0));
    m_secondColumn->Add(m_economy, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_secondColumn->Add(label6, wxSizerFlags(0).Expand().Border(wxBOTTOM, 5));
    m_secondColumn->Add(m_type, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 72));
    //m_secondColumn->AddStretchSpacer(1);

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

void LocationShowcase::setData(Location& location) {
    Freeze();
    m_name->SetLabel(location.name);
    
    if (location.importance == "High") {
        m_importance->SetBackgroundColour(wxColour(230, 60, 60));
        m_importance->SetLabel("Main");
    } else {
        m_importance->SetBackgroundColour(wxColour(220, 220, 220));

        if (location.importance == "Low")
            m_importance->SetLabel("Secondary");
        else
            m_importance->SetLabel("");
    }

    m_background->SetValue(location.background);
    m_natural->SetValue(location.natural);
    m_architecture->SetValue(location.architecture);
    m_economy->SetValue(location.economy);
    m_culture->SetValue(location.culture);

    m_type->SetLabel(location.type);
    if (location.type == "Private")
        m_type->SetBackgroundColour(wxColour(0, 0, 100));
    else if (location.type == "Public")
        m_type->SetBackgroundColour(wxColour(100, 0, 0));
    else
        m_type->SetBackgroundColour(wxColour(50, 50, 50));

    m_type->Show(!location.type.empty());

    int tcsize = m_custom.size();
    int ccsize = location.custom.size();

    if (tcsize > ccsize) {
        auto it = m_custom.end() - 1;

        for (int i = (tcsize - 1); i > (ccsize - 1); i--) {
            m_custom[i].first->Destroy();
            m_custom[i].second->Destroy();
            m_custom.erase(it--);

            m_first = !m_first;
        }
    }

    wxSize size(-1, 80);

    wxWindowList list;
    list.Append(m_background);
    list.Append(m_natural);
    list.Append(m_architecture);
    list.Append(m_economy);
    list.Append(m_culture);

    for (int i = 0; i < location.custom.size(); i++) {
        if (i >= tcsize) {
            wxStaticText* label = new wxStaticText(this, -1, "", wxDefaultPosition,
                wxDefaultSize, wxBORDER_DOUBLE);
            wxTextCtrl* content = new wxTextCtrl(this, -1, "", wxDefaultPosition,
                size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);

            if (m_first) {
                m_firstColumn->Add(label, wxSizerFlags(0).Left());
                m_firstColumn->Add(content, wxSizerFlags(0).Border(wxBOTTOM, 20).Expand());
            } else {
                m_secondColumn->Add(label, wxSizerFlags(0).Left());
                m_secondColumn->Add(content, wxSizerFlags(0).Border(wxBOTTOM, 20).Expand());
            }

            label->SetFont(wxFontInfo(12).Bold());
            label->SetBackgroundColour(wxColour(180, 180, 180));

            content->SetFont(wxFontInfo(9));
            content->SetBackgroundColour(wxColour(225, 225, 225));
            content->SetCursor(wxCURSOR_DEFAULT);

            m_first = !m_first;
            m_custom.push_back(pair<wxStaticText*, wxTextCtrl*>(label, content));
        }

        m_custom[i].first->SetLabel(location.custom[i].first);
        m_custom[i].second->SetLabel(location.custom[i].second);

        list.Append(m_custom[i].second);
    }

    m_vertical->FitInside(this);

    int nol;
    for (auto it : list) {
        nol = ((wxTextCtrl*)it)->GetNumberOfLines();

        if (nol > 5)
            it->SetMinSize(wxSize(-1, nol * it->GetCharHeight() + 5));
        else
            it->SetMinSize(size);
    }

    m_image->Show(m_image->setImage(location.image));
    m_vertical->FitInside(this);
    Thaw();
}
