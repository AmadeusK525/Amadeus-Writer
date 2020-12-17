#include "CharacterShowcase.h"

#include "wxmemdbg.h"

CharacterShowcase::CharacterShowcase(wxWindow* parent):
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL) {

    wxFont font(wxFontInfo(12).Bold());
    wxFont font2(wxFontInfo(11));

    role = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 25), wxALIGN_CENTER | wxBORDER_RAISED | wxST_NO_AUTORESIZE);
    role->SetBackgroundColour(wxColour(220, 220, 220));
    role->SetFont(font);

    name = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxALIGN_CENTER | wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    name->SetBackgroundColour(wxColour(250, 250, 250));
    name->SetFont(font);

    wxStaticText* label1 = new wxStaticText(this, -1, "Age:");
    label1->SetFont(font);
    label1->SetBackgroundColour(wxColour(190, 190, 190));
    age = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(45, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    age->SetFont(font2);
    age->SetBackgroundColour(wxColour(225, 225, 225));
    
    wxStaticText* label2 = new wxStaticText(this, -1, "Sex:");
    label2->SetFont(font);
    label2->SetBackgroundColour(wxColour(190, 190, 190));
    sex = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    sex->SetFont(font2);
    sex->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label3 = new wxStaticText(this, -1, "Height:");
    label3->SetFont(font);
    label3->SetBackgroundColour(wxColour(190, 190, 190));
    height = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    height->SetFont(font2);
    height->SetBackgroundColour(wxColour(225, 225, 225));

    wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
    firstLine->Add(label1, wxSizerFlags(0).Border(wxLEFT, 10));
    firstLine->Add(age, wxSizerFlags(0));
    firstLine->Add(label2, wxSizerFlags(0).Border(wxLEFT, 20));
    firstLine->Add(sex, wxSizerFlags(0));
    firstLine->Add(label3, wxSizerFlags(0).Border(wxLEFT, 20));
    firstLine->Add(height, wxSizerFlags(0).Border(wxRIGHT, 5));

    wxStaticText* label4 = new wxStaticText(this, -1, "Nickname:");
    label4->SetFont(font);
    label4->SetBackgroundColour(wxColour(190, 190, 190));
    nick = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    nick->SetFont(font2);
    nick->SetBackgroundColour(wxColour(225, 225, 225));
    
    wxStaticText* label5 = new wxStaticText(this, -1, "Nationality:");
    label5->SetFont(font);
    label5->SetBackgroundColour(wxColour(190, 190, 190));
    nat = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(120, 20), wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    nat->SetFont(font2);
    nat->SetBackgroundColour(wxColour(225, 225, 225));

    wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
    secondLine->Add(label4, wxSizerFlags(0).Border(wxLEFT, 10));
    secondLine->Add(nick, wxSizerFlags(0));
    secondLine->Add(label5, wxSizerFlags(0).Border(wxLEFT, 20));
    secondLine->Add(nat, wxSizerFlags(0).Border(wxRIGHT, 5));

    wxStaticText* label6 = new wxStaticText(this, -1, "Appearance", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE | wxST_NO_AUTORESIZE);
    label6->SetFont(font);
    label6->SetBackgroundColour(wxColour(180, 180, 180));
    appearance = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    appearance->SetFont(wxFontInfo(9));
    appearance->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label7 = new wxStaticText(this, -1, "Personality", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE | wxST_NO_AUTORESIZE);
    label7->SetFont(font);
    label7->SetBackgroundColour(wxColour(180, 180, 180));
    personality = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    personality->SetFont(wxFontInfo(9));
    personality->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label8 = new wxStaticText(this, -1, "Backstory", wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE | wxST_NO_AUTORESIZE);
    label8->SetFont(font);
    label8->SetBackgroundColour(wxColour(180, 180, 180));
    backstory = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    backstory->SetFont(wxFontInfo(9));
    backstory->SetBackgroundColour(wxColour(225, 225, 225));

    image = new ImagePanel(this, wxDefaultPosition, wxSize(200, 200));
    image->SetBackgroundColour(wxColour(150, 150, 150));
    image->setBorderColour(wxColour(20, 20, 20));
    image->Hide();

    vertical = new  wxBoxSizer(wxVERTICAL);
    vertical->Add(role, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    vertical->Add(image, wxSizerFlags(0).CenterHorizontal().Shaped());
    vertical->SetItemMinSize(1, wxSize(200, 200));
    vertical->Add(name, wxSizerFlags(0).Expand().Border(wxALL, 15));
    vertical->Add(firstLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    vertical->Add(secondLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    vertical->Add(label6, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    vertical->Add(appearance, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    vertical->Add(label7, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    vertical->Add(personality, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    vertical->Add(label8, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
    vertical->Add(backstory, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    
    SetSizer(vertical);
    vertical->FitInside(this);
    this->SetScrollRate(20, 20);

    appearance->SetCursor(wxCURSOR_DEFAULT);
    personality->SetCursor(wxCURSOR_DEFAULT);
    backstory->SetCursor(wxCURSOR_DEFAULT);
}

void CharacterShowcase::setData(Character& character) {
    name->SetLabel(character.name);
    sex->SetLabel(character.sex);

    if (character.sex == "Female")
        sex->SetBackgroundColour(wxColour(255, 182, 193));
    else if (character.sex == "Male")
        sex->SetBackgroundColour(wxColour(139, 186, 255));
    else
        sex->SetBackgroundColour(wxColour(220, 220, 220));
    
    age->SetLabel(character.age);
    height->SetLabel(character.height);
    nat->SetLabel(character.nat);
    nick->SetLabel(character.nick);
    role->SetLabel(character.role);

    if (character.role == "Main")
        role->SetBackgroundColour(wxColour(230, 60, 60));
    else
        role->SetBackgroundColour(wxColour(220, 220, 220));

    appearance->SetValue(character.appearance);
    personality->SetValue(character.personality);
    backstory->SetValue(character.backstory);

    int nol;
    wxSize size(-1, 80);

    nol = appearance->GetNumberOfLines();
    if (nol > 5)
        appearance->SetMinSize(wxSize(-1, nol * appearance->GetCharHeight() + 5));
    else
        appearance->SetMinSize(size);

    nol = personality->GetNumberOfLines();
    if (nol > 5)
        personality->SetMinSize(wxSize(-1, nol * personality->GetCharHeight() + 5));
    else
        personality->SetMinSize(size);

    nol = backstory->GetNumberOfLines();
    if (nol > 5)
        backstory->SetMinSize(wxSize(-1, nol * backstory->GetCharHeight() + 5));
    else
        backstory->SetMinSize(size);

    image->Show(image->setImage(character.image));

    int tcsize = custom.size();
    int ccsize = character.custom.size();

    if (tcsize > ccsize) {
        auto it = custom.end() - 1;

        for (int i = (tcsize - 1); i > (ccsize - 1); i--) {
            custom[i].first->Destroy();
            custom[i].second->Destroy();
            custom.erase(it--);
        }
    }

    for (int i = 0; i < character.custom.size(); i++) {
        if (i >= tcsize) {
            wxStaticText* label = new wxStaticText(this, -1, "", wxDefaultPosition,
                wxDefaultSize, wxBORDER_DOUBLE | wxST_NO_AUTORESIZE);
            label->SetFont(wxFontInfo(12).Bold());
            label->SetBackgroundColour(wxColour(180, 180, 180));

            wxTextCtrl* content = new wxTextCtrl(this, -1, "", wxDefaultPosition,
                size, wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
            content->SetFont(wxFontInfo(9));
            content->SetBackgroundColour(wxColour(225, 225, 225));
            content->SetCursor(wxCURSOR_DEFAULT);

            vertical->Add(label, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10).Left());
            vertical->Add(content, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());

            custom.push_back(pair<wxStaticText*, wxTextCtrl*>(label, content));
        }

        custom[i].first->SetLabel(character.custom[i].first);
        custom[i].second->SetLabel(character.custom[i].second);
    }

    vertical->FitInside(this);

    for (auto it : custom) {
        nol = it.second->GetNumberOfLines();
        if (nol > 5) {
            it.second->SetMinSize(wxSize(-1, nol * it.second->GetCharHeight() + 5));
        } else {
            it.second->SetMinSize(wxSize(-1, 80));
        }
    }

    vertical->FitInside(this);
    Refresh();
}
