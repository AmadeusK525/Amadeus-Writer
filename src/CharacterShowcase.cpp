#include "CharacterShowcase.h"

#include "wxmemdbg.h"

CharacterShowcase::CharacterShowcase(wxWindow* parent):
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL) {

    wxFont font(wxFontInfo(12).Bold());
    wxFont font2(wxFontInfo(11));

    m_role = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 25), wxALIGN_CENTER | wxBORDER_RAISED | wxST_NO_AUTORESIZE);
    m_role->SetBackgroundColour(wxColour(220, 220, 220));
    m_role->SetFont(font);

    m_name = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxALIGN_CENTER | wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    m_name->SetBackgroundColour(wxColour(250, 250, 250));
    m_name->SetFont(font);

    wxStaticText* label1 = new wxStaticText(this, -1, "Age:");
    label1->SetFont(font);
    label1->SetBackgroundColour(wxColour(190, 190, 190));
    m_age = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(45, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    m_age->SetFont(font2);
    m_age->SetBackgroundColour(wxColour(225, 225, 225));
    
    wxStaticText* label2 = new wxStaticText(this, -1, "Sex:");
    label2->SetFont(font);
    label2->SetBackgroundColour(wxColour(190, 190, 190));
    m_sex = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    m_sex->SetFont(font2);
    m_sex->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label3 = new wxStaticText(this, -1, "Height:");
    label3->SetFont(font);
    label3->SetBackgroundColour(wxColour(190, 190, 190));
    m_height = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    m_height->SetFont(font2);
    m_height->SetBackgroundColour(wxColour(225, 225, 225));

    wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
    firstLine->Add(label1, wxSizerFlags(0).Border(wxLEFT, 10));
    firstLine->Add(m_age, wxSizerFlags(1));
    firstLine->Add(label2, wxSizerFlags(0).Border(wxLEFT, 20));
    firstLine->Add(m_sex, wxSizerFlags(2));
    firstLine->Add(label3, wxSizerFlags(0).Border(wxLEFT, 20));
    firstLine->Add(m_height, wxSizerFlags(2).Border(wxRIGHT, 5));

    wxStaticText* label4 = new wxStaticText(this, -1, "Nickname:");
    label4->SetFont(font);
    label4->SetBackgroundColour(wxColour(190, 190, 190));
    m_nick = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    m_nick->SetFont(font2);
    m_nick->SetBackgroundColour(wxColour(225, 225, 225));
    
    wxStaticText* label5 = new wxStaticText(this, -1, "Nationality:");
    label5->SetFont(font);
    label5->SetBackgroundColour(wxColour(190, 190, 190));
    m_nat = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(120, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    m_nat->SetFont(font2);
    m_nat->SetBackgroundColour(wxColour(225, 225, 225));

    wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
    secondLine->Add(label4, wxSizerFlags(0).Border(wxLEFT, 10));
    secondLine->Add(m_nick, wxSizerFlags(1));
    secondLine->Add(label5, wxSizerFlags(0).Border(wxLEFT, 20));
    secondLine->Add(m_nat, wxSizerFlags(1).Border(wxRIGHT, 5));

    wxStaticText* label6 = new wxStaticText(this, -1, "Appearance", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE | wxST_NO_AUTORESIZE);
    label6->SetFont(font);
    label6->SetBackgroundColour(wxColour(180, 180, 180));
    m_appearance = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    m_appearance->SetFont(wxFontInfo(9));
    m_appearance->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label7 = new wxStaticText(this, -1, "Personality", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE | wxST_NO_AUTORESIZE);
    label7->SetFont(font);
    label7->SetBackgroundColour(wxColour(180, 180, 180));
    m_personality = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    m_personality->SetFont(wxFontInfo(9));
    m_personality->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label8 = new wxStaticText(this, -1, "Backstory", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE | wxST_NO_AUTORESIZE);
    label8->SetFont(font);
    label8->SetBackgroundColour(wxColour(180, 180, 180));
    m_backstory = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    m_backstory->SetFont(wxFontInfo(9));
    m_backstory->SetBackgroundColour(wxColour(225, 225, 225));

    m_image = new ImagePanel(this, wxDefaultPosition, wxSize(200, 200));
    m_image->SetBackgroundColour(wxColour(150, 150, 150));
    m_image->setBorderColour(wxColour(20, 20, 20));
    m_image->Hide();

    m_vertical = new  wxBoxSizer(wxVERTICAL);
    m_vertical->Add(m_role, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    m_vertical->Add(m_image, wxSizerFlags(0).CenterHorizontal().Shaped());
    m_vertical->SetItemMinSize(1, wxSize(200, 200));
    m_vertical->Add(m_name, wxSizerFlags(0).Expand().Border(wxALL, 15));
    m_vertical->Add(firstLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    m_vertical->Add(secondLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    m_vertical->Add(label6, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    m_vertical->Add(m_appearance, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    m_vertical->Add(label7, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    m_vertical->Add(m_personality, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    m_vertical->Add(label8, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    m_vertical->Add(m_backstory, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    
    SetSizer(m_vertical);
    m_vertical->FitInside(this);
    this->SetScrollRate(20, 20);

    m_appearance->SetCursor(wxCURSOR_DEFAULT);
    m_personality->SetCursor(wxCURSOR_DEFAULT);
    m_backstory->SetCursor(wxCURSOR_DEFAULT);
}

void CharacterShowcase::setData(Character& character) {
    Freeze();
    m_name->SetLabel(character.name);
    m_sex->SetLabel(character.sex);

    if (character.sex == "Female")
        m_sex->SetBackgroundColour(wxColour(255, 182, 193));
    else if (character.sex == "Male")
        m_sex->SetBackgroundColour(wxColour(139, 186, 255));
    else
        m_sex->SetBackgroundColour(wxColour(220, 220, 220));
    
    m_age->SetLabel(character.age);
    m_height->SetLabel(character.height);
    m_nat->SetLabel(character.nat);
    m_nick->SetLabel(character.nick);
    m_role->SetLabel(character.role);

    if (character.role == "Main")
        m_role->SetBackgroundColour(wxColour(230, 60, 60));
    else
        m_role->SetBackgroundColour(wxColour(220, 220, 220));

    m_appearance->SetValue(character.appearance);
    m_personality->SetValue(character.personality);
    m_backstory->SetValue(character.backstory);

    m_image->Show(m_image->setImage(character.image));

    int tcsize = m_custom.size();
    int ccsize = character.custom.size();

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

    for (int i = 0; i < character.custom.size(); i++) {
        if (i >= tcsize) {
            wxStaticText* label = new wxStaticText(this, -1, "", wxDefaultPosition,
                wxDefaultSize, wxBORDER_DOUBLE);
            label->SetFont(wxFontInfo(12).Bold());
            label->SetBackgroundColour(wxColour(180, 180, 180));

            wxTextCtrl* content = new wxTextCtrl(this, -1, "", wxDefaultPosition,
                size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
            content->SetFont(wxFontInfo(9));
            content->SetBackgroundColour(wxColour(225, 225, 225));
            content->SetCursor(wxCURSOR_DEFAULT);

            m_vertical->Add(label, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
            m_vertical->Add(content, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());

            m_custom.push_back(pair<wxStaticText*, wxTextCtrl*>(label, content));
        }

        list.Append(m_custom[i].second);
        m_custom[i].first->SetLabel(character.custom[i].first);
        m_custom[i].second->SetLabel(character.custom[i].second);
    }

    m_vertical->Layout();
    m_vertical->FitInside(this);

    int nol;
    for (auto it : list) {
        nol = ((wxTextCtrl*)it)->GetNumberOfLines();

        if (nol > 5)
            it->SetMinSize(wxSize(-1, nol * it->GetCharHeight() + 5));
        else
            it->SetMinSize(size);
    }

    m_vertical->FitInside(this);
    Thaw();
}