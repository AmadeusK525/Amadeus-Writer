#include "LocationShowcase.h"

#include "wxmemdbg.h"

LocationShowcase::LocationShowcase(wxWindow* parent) :
    wxScrolledWindow(parent, wxID_ANY) {

    wxFont font(wxFontInfo(12).Bold());
    wxFont font2(wxFontInfo(9));

    m_importance = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 25), wxALIGN_CENTER | wxBORDER_RAISED | wxST_NO_AUTORESIZE);
    m_importance->SetBackgroundColour(wxColour(220, 220, 220));
    m_importance->SetFont(font);
    
    m_image = new ImagePanel(this, wxDefaultPosition, wxSize(180, 180));
    m_image->SetBackgroundColour(wxColour(150, 150, 150));
    m_image->SetBorderColour(wxColour(20, 20, 20));
    m_image->Hide();

    m_name = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxALIGN_CENTER | wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    m_name->SetBackgroundColour(wxColour(255, 255, 255));
    m_name->SetFont(font);

    m_generalLabel = new wxStaticText(this, -1, "General", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    m_generalLabel->SetFont(font);
    m_generalLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_general = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_general->SetFont(font2);
    m_general->SetBackgroundColour(wxColour(225, 225, 225));

    m_natLabel = new wxStaticText(this, -1, "Natural Aspects", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    m_natLabel->SetFont(font);
    m_natLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_natural = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_natural->SetFont(font2);
    m_natural->SetBackgroundColour(wxColour(225, 225, 225));

    m_archLabel = new wxStaticText(this, -1, "Architecture", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    m_archLabel->SetFont(font);
    m_archLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_architecture = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_architecture->SetFont(font2);
    m_architecture->SetBackgroundColour(wxColour(225, 225, 225));

    m_ecoLabel = new wxStaticText(this, -1, "Economy", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    m_ecoLabel->SetFont(font);
    m_ecoLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_economy = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_economy->SetFont(font2);
    m_economy->SetBackgroundColour(wxColour(225, 225, 225));

    m_culLabel = new wxStaticText(this, -1, "Culture", wxDefaultPosition, wxDefaultSize, 0L | wxBORDER_DOUBLE);
    m_culLabel->SetFont(font);
    m_culLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_culture = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_culture->SetFont(font2);
    m_culture->SetBackgroundColour(wxColour(225, 225, 225));

    m_poliLabel = new wxStaticText(this, -1, "Politics", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_DOUBLE);
    m_poliLabel->SetFont(font);
    m_poliLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_politics = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_politics->SetFont(font2);
    m_politics->SetBackgroundColour(wxColour(225, 225, 225));

    m_firstColumn = new wxBoxSizer(wxVERTICAL);
    m_firstColumn->Add(m_generalLabel, wxSizerFlags(0));
    m_firstColumn->Add(m_general, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_firstColumn->Add(m_archLabel, wxSizerFlags(0));
    m_firstColumn->Add(m_architecture, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_firstColumn->Add(m_culLabel, wxSizerFlags(0));
    m_firstColumn->Add(m_culture, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));

    m_secondColumn = new wxBoxSizer(wxVERTICAL);
    m_secondColumn->Add(m_natLabel, wxSizerFlags(0));
    m_secondColumn->Add(m_natural, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_secondColumn->Add(m_ecoLabel, wxSizerFlags(0));
    m_secondColumn->Add(m_economy, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));
    m_secondColumn->Add(m_poliLabel, wxSizerFlags(0));
    m_secondColumn->Add(m_politics, wxSizerFlags(0).Expand().Border(wxBOTTOM, 20));

    wxBoxSizer* horSizer = new wxBoxSizer(wxHORIZONTAL);
    horSizer->Add(m_firstColumn, wxSizerFlags(1).Expand().Border(wxRIGHT, 10));
    horSizer->Add(m_secondColumn, wxSizerFlags(1).Expand().Border(wxLEFT, 10));

    m_vertical = new  wxBoxSizer(wxVERTICAL);
    m_vertical->Add(m_importance, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    m_vertical->Add(m_image, wxSizerFlags(0).CenterHorizontal());
    m_vertical->SetItemMinSize(size_t(1), wxSize(200, 200));
    m_vertical->Add(m_name, wxSizerFlags(0).Expand().Border(wxALL, 15));
    m_vertical->Add(horSizer, wxSizerFlags(1).Expand().Border(wxALL, 15));

    m_general->SetCursor(wxCURSOR_DEFAULT);
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

    m_general->SetValue(location.general);
    m_natural->SetValue(location.natural);
    m_architecture->SetValue(location.architecture);
    m_politics->SetValue(location.politics);
    m_economy->SetValue(location.economy);
    m_culture->SetValue(location.culture);

    m_general->Show(location.general != "");
    m_generalLabel->Show(location.general != "");
    m_natural->Show(location.natural!= "");
    m_natLabel->Show(location.natural != "");
    m_architecture->Show(location.architecture != "");
    m_archLabel->Show(location.architecture != "");
    m_politics->Show(location.politics != "");
    m_poliLabel->Show(location.politics != "");
    m_economy->Show(location.economy != "");
    m_ecoLabel->Show(location.economy != "");
    m_culture->Show(location.culture != "");
    m_culLabel->Show(location.culture != "");

    m_image->Show(m_image->SetImage(location.image));

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
    list.Append(m_general);
    list.Append(m_natural);
    list.Append(m_architecture);
    list.Append(m_economy);
    list.Append(m_culture);

    for (int i = 0; i < location.custom.size(); i++) {
        if (i >= tcsize) {
            wxStaticText* label = new wxStaticText(this, -1, "", wxDefaultPosition,
                wxDefaultSize, wxBORDER_DOUBLE);
            wxTextCtrl* content = new wxTextCtrl(this, -1, "", wxDefaultPosition,
                size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);

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

        m_custom[i].first->Show(location.custom[i].second != "");
        m_custom[i].second->Show(location.custom[i].second != "");

        list.Append(m_custom[i].second);
    }

    m_vertical->FitInside(this);

    int nol;
    for (auto it : list) {
        if (it->IsShown()) {
            nol = ((wxTextCtrl*)it)->GetNumberOfLines();

            if (nol > 5)
                it->SetMinSize(wxSize(-1, nol * it->GetCharHeight() + 5));
            else
                it->SetMinSize(size);
        }
    }

    m_vertical->FitInside(this);
    Thaw();
}
