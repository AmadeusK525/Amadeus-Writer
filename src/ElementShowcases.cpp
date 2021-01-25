#include "ElementShowcases.h"

ElementShowcase::ElementShowcase(wxWindow* parent):
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL) {
    wxFont font(wxFontInfo(12).Bold());

    m_role = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(150, 25), wxALIGN_CENTER | wxBORDER_RAISED);
    m_role->SetBackgroundColour(wxColour(220, 220, 220));
    m_role->SetFont(font);

    m_name = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxALIGN_CENTER | wxBORDER_SIMPLE);
    m_name->SetBackgroundColour(wxColour(250, 250, 250));
    m_name->SetFont(font);

    m_image = new ImagePanel(this, wxDefaultPosition, wxSize(180, 180));
    m_image->SetBackgroundColour(wxColour(150, 150, 150));
    m_image->SetBorderColour(wxColour(20, 20, 20));
    m_image->Hide();

    m_vertical = new  wxBoxSizer(wxVERTICAL);
    m_vertical->Add(m_role, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    m_vertical->Add(m_image, wxSizerFlags(0).CenterHorizontal());
    m_vertical->SetItemMinSize(1, wxSize(200, 200));
    m_vertical->Add(m_name, wxSizerFlags(0).Expand().Border(wxALL, 15));

    SetSizer(m_vertical);
    m_vertical->FitInside(this);
    this->SetScrollRate(20, 20);
}

void ElementShowcase::SetData(const Element& element) {
    Freeze();
    m_name->SetLabel(element.name);
}


///////////////////////////////////////////////////////////////////
///////////////////////// CharacterShowcase ///////////////////////
///////////////////////////////////////////////////////////////////


CharacterShowcase::CharacterShowcase(wxWindow* parent) : ElementShowcase(parent) {

    wxFont font(wxFontInfo(12).Bold());
    wxFont font2(wxFontInfo(11));

    m_ageLabel = new wxStaticText(this, -1, "Age:");
    m_ageLabel->SetFont(font);
    m_ageLabel->SetBackgroundColour(wxColour(190, 190, 190));
    m_age = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(45, 20), wxBORDER_SIMPLE);
    m_age->SetFont(font2);
    m_age->SetBackgroundColour(wxColour(225, 225, 225));

    m_sexLabel = new wxStaticText(this, -1, "Sex:");
    m_sexLabel->SetFont(font);
    m_sexLabel->SetBackgroundColour(wxColour(190, 190, 190));
    m_sex = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_SIMPLE);
    m_sex->SetFont(font2);
    m_sex->SetBackgroundColour(wxColour(225, 225, 225));

    m_heightLabel = new wxStaticText(this, -1, "Height:");
    m_heightLabel->SetFont(font);
    m_heightLabel->SetBackgroundColour(wxColour(190, 190, 190));
    m_height = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_SIMPLE);
    m_height->SetFont(font2);
    m_height->SetBackgroundColour(wxColour(225, 225, 225));

    wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
    firstLine->Add(m_ageLabel, wxSizerFlags(0).Border(wxLEFT, 10));
    firstLine->Add(m_age, wxSizerFlags(1));
    firstLine->Add(m_sexLabel, wxSizerFlags(0).Border(wxLEFT, 20));
    firstLine->Add(m_sex, wxSizerFlags(2));
    firstLine->Add(m_heightLabel, wxSizerFlags(0).Border(wxLEFT, 20));
    firstLine->Add(m_height, wxSizerFlags(2).Border(wxRIGHT, 5));

    m_nickLabel = new wxStaticText(this, -1, "Nickname:");
    m_nickLabel->SetFont(font);
    m_nickLabel->SetBackgroundColour(wxColour(190, 190, 190));
    m_nick = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 20), wxBORDER_SIMPLE);
    m_nick->SetFont(font2);
    m_nick->SetBackgroundColour(wxColour(225, 225, 225));

    m_natLabel = new wxStaticText(this, -1, "Nationality:");
    m_natLabel->SetFont(font);
    m_natLabel->SetBackgroundColour(wxColour(190, 190, 190));
    m_nat = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(120, 20), wxBORDER_SIMPLE);
    m_nat->SetFont(font2);
    m_nat->SetBackgroundColour(wxColour(225, 225, 225));

    wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
    secondLine->Add(m_nickLabel, wxSizerFlags(0));
    secondLine->Add(m_nick, wxSizerFlags(1));
    secondLine->Add(m_natLabel, wxSizerFlags(0).Border(wxLEFT, 20));
    secondLine->Add(m_nat, wxSizerFlags(1));

    m_appLabel = new wxStaticText(this, -1, "Appearance", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE);
    m_appLabel->SetFont(font);
    m_appLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_appearance = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_appearance->SetFont(wxFontInfo(9));
    m_appearance->SetBackgroundColour(wxColour(225, 225, 225));

    m_perLabel = new wxStaticText(this, -1, "Personality", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE);
    m_perLabel->SetFont(font);
    m_perLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_personality = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_personality->SetFont(wxFontInfo(9));
    m_personality->SetBackgroundColour(wxColour(225, 225, 225));

    m_bsLabel = new wxStaticText(this, -1, "Backstory", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE);
    m_bsLabel->SetFont(font);
    m_bsLabel->SetBackgroundColour(wxColour(180, 180, 180));
    m_backstory = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80),
        wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
    m_backstory->SetFont(wxFontInfo(9));
    m_backstory->SetBackgroundColour(wxColour(225, 225, 225));

    m_vertical->Add(firstLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM | wxLEFT | wxRIGHT, 10));
    m_vertical->Add(secondLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM | wxLEFT | wxRIGHT, 10));
    m_vertical->Add(m_appLabel, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    m_vertical->Add(m_appearance, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    m_vertical->Add(m_perLabel, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    m_vertical->Add(m_personality, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    m_vertical->Add(m_bsLabel, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    m_vertical->Add(m_backstory, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());

    m_appearance->SetCursor(wxCURSOR_DEFAULT);
    m_personality->SetCursor(wxCURSOR_DEFAULT);
    m_backstory->SetCursor(wxCURSOR_DEFAULT);
}

void CharacterShowcase::SetData(const Element& charToSet) {
    const Character* character = dynamic_cast<const Character*>(&charToSet);
    if (!character) {
        wxMessageBox("There was an unexpected problem when loading the Character to the panel.");
        SetData(Character());
        return;
    }

    Freeze();
    m_name->SetLabel(character->name);
    m_sex->SetLabel(character->sex);

    if (character->sex == "Female")
        m_sex->SetBackgroundColour(wxColour(255, 182, 193));
    else if (character->sex == "Male")
        m_sex->SetBackgroundColour(wxColour(139, 186, 255));
    else
        m_sex->SetBackgroundColour(wxColour(220, 220, 220));

    m_age->SetLabel(character->age);
    m_height->SetLabel(character->height);
    m_nat->SetLabel(character->nat);
    m_nick->SetLabel(character->nick);

    string role("");
    wxColour rolebg(220, 220, 220);
    wxColour rolefg(10, 10, 10);
    switch (character->role) {
    case cProtagonist:
        role = "Protagonist";
        rolebg = wxColour(230, 60, 60);
        rolefg = wxColour(10, 10, 10);
        break;

    case cSupporting:
        role = "Supporting";
        rolebg = wxColour(130, 230, 180);
        rolefg = wxColour(10, 10, 10);
        break;

    case cVillian:
        role = "Villian";
        rolebg = wxColour(100, 20, 20);
        rolefg = wxColour(255, 255, 255);
        break;

    case cSecondary:
        role = "Secondary";
        rolebg = wxColour(220, 220, 220);
        rolefg = wxColour(10, 10, 10);
        break;
    }

    m_role->SetLabel(role);
    m_role->SetBackgroundColour(rolebg);
    m_role->SetForegroundColour(rolefg);

    m_appearance->SetValue(character->appearance);
    m_personality->SetValue(character->personality);
    m_backstory->SetValue(character->backstory);

    m_age->Show(character->age != "");
    m_ageLabel->Show(character->age != "");
    m_sex->Show(character->sex != "");
    m_sexLabel->Show(character->sex != "");
    m_height->Show(character->height != "");
    m_heightLabel->Show(character->height != "");
    m_nat->Show(character->nat != "");
    m_natLabel->Show(character->nat != "");
    m_nick->Show(character->nick != "");
    m_nickLabel->Show(character->nick != "");
    m_appearance->Show(character->appearance != "");
    m_appLabel->Show(character->appearance != "");
    m_personality->Show(character->personality != "");
    m_perLabel->Show(character->personality != "");
    m_backstory->Show(character->backstory != "");
    m_bsLabel->Show(character->backstory != "");

    m_image->Show(m_image->SetImage(character->image));

    int tcsize = m_custom.size();
    int ccsize = character->custom.size();

    if (tcsize > ccsize) {
        auto it = m_custom.end() - 1;

        for (int i = (tcsize - 1); i > (ccsize - 1); i--) {
            m_custom[i].first->Destroy();
            m_custom[i].second->Destroy();
            m_custom.erase(it--);
        }
    }

    wxSize size(-1, 80);

    wxWindowList list;
    list.Append(m_appearance);
    list.Append(m_personality);
    list.Append(m_backstory);

    for (int i = 0; i < character->custom.size(); i++) {
        if (i >= tcsize) {
            wxStaticText* label = new wxStaticText(this, -1, "", wxDefaultPosition,
                wxDefaultSize, wxBORDER_DOUBLE);
            label->SetFont(wxFontInfo(12).Bold());
            label->SetBackgroundColour(wxColour(180, 180, 180));

            wxTextCtrl* content = new wxTextCtrl(this, -1, "", wxDefaultPosition,
                size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL | wxBORDER_SIMPLE);
            content->SetFont(wxFontInfo(9));
            content->SetBackgroundColour(wxColour(225, 225, 225));
            content->SetCursor(wxCURSOR_DEFAULT);

            m_vertical->Add(label, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
            m_vertical->Add(content, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());

            m_custom.push_back(pair<wxStaticText*, wxTextCtrl*>(label, content));
        }

        list.Append(m_custom[i].second);
        m_custom[i].first->SetLabel(character->custom[i].first);
        m_custom[i].second->SetLabel(character->custom[i].second);

        m_custom[i].first->Show(character->custom[i].second != "");
        m_custom[i].second->Show(character->custom[i].second != "");
    }

    m_vertical->Layout();
    m_vertical->FitInside(this);

    int nol;
    for (auto& it : list) {
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


///////////////////////////////////////////////////////////////////
///////////////////////// LocationShowcase ////////////////////////
///////////////////////////////////////////////////////////////////


LocationShowcase::LocationShowcase(wxWindow* parent) :ElementShowcase(parent) {
    wxFont font(wxFontInfo(12).Bold());
    wxFont font2(wxFontInfo(9));

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

    m_vertical->Add(horSizer, wxSizerFlags(1).Expand().Border(wxALL, 15));

    m_general->SetCursor(wxCURSOR_DEFAULT);
    m_natural->SetCursor(wxCURSOR_DEFAULT);
    m_architecture->SetCursor(wxCURSOR_DEFAULT);
    m_economy->SetCursor(wxCURSOR_DEFAULT);
    m_culture->SetCursor(wxCURSOR_DEFAULT);
}

void LocationShowcase::SetData(const Element& locToSet) {
    const Location* location = dynamic_cast<const Location*>(&locToSet);

    if (!location) {
        wxMessageBox("There was an unexpected problem when loading the Lcoation to the panel.");
        SetData(Location());
        return;
    }

    Freeze();
    m_name->SetLabel(location->name);

    switch (location->role) {
    case lHigh:
        m_role->SetBackgroundColour(wxColour(230, 60, 60));
        m_role->SetLabel("Main");
        break;

    case lLow:
        m_role->SetBackgroundColour(wxColour(220, 220, 220));
        m_role->SetLabel("Secondary");
        break;

    default:
        m_role->SetBackgroundColour(wxColour(220, 220, 220));
        m_role->SetLabel("");
        break;
    }

    m_general->SetValue(location->general);
    m_natural->SetValue(location->natural);
    m_architecture->SetValue(location->architecture);
    m_politics->SetValue(location->politics);
    m_economy->SetValue(location->economy);
    m_culture->SetValue(location->culture);

    m_general->Show(location->general != "");
    m_generalLabel->Show(location->general != "");
    m_natural->Show(location->natural != "");
    m_natLabel->Show(location->natural != "");
    m_architecture->Show(location->architecture != "");
    m_archLabel->Show(location->architecture != "");
    m_politics->Show(location->politics != "");
    m_poliLabel->Show(location->politics != "");
    m_economy->Show(location->economy != "");
    m_ecoLabel->Show(location->economy != "");
    m_culture->Show(location->culture != "");
    m_culLabel->Show(location->culture != "");

    m_image->Show(m_image->SetImage(location->image));

    int tcsize = m_custom.size();
    int ccsize = location->custom.size();

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

    for (int i = 0; i < location->custom.size(); i++) {
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

        m_custom[i].first->SetLabel(location->custom[i].first);
        m_custom[i].second->SetLabel(location->custom[i].second);

        m_custom[i].first->Show(location->custom[i].second != "");
        m_custom[i].second->Show(location->custom[i].second != "");

        list.Append(m_custom[i].second);
    }

    m_vertical->FitInside(this);

    int nol;
    for (auto& it : list) {
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
