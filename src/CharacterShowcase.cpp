#include "CharacterShowcase.h"

#include "wxmemdbg.h"

CharacterShowcase::CharacterShowcase(wxWindow* parent):
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL) {

    role = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 25), wxTE_CENTER | wxBORDER_RAISED);
    role->SetBackgroundColour(wxColour(220, 220, 220));
    role->SetFont(wxFont(wxFontInfo(12).Bold()));

    label1 = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxTE_CENTER | wxBORDER_SIMPLE);
    label1->SetBackgroundColour(wxColour(250, 250, 250));
    label1->SetFont(wxFont(wxFontInfo(12).Bold()));

    wxBoxSizer* label1Sizer = new wxBoxSizer(wxHORIZONTAL);
    label1Sizer->AddStretchSpacer(1);
    label1Sizer->Add(label1, 4);
    label1Sizer->AddStretchSpacer(1);

    wxStaticText* label2 = new wxStaticText(this, -1, "Age:");
    label2->SetFont(wxFont(wxFontInfo(12).Bold()));
    label2->SetBackgroundColour(wxColour(190, 190, 190));
    age = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(45, 20), wxTE_READONLY | wxBORDER_SIMPLE);
    age->SetFont(wxFont(wxFontInfo(11)));
    age->SetBackgroundColour(wxColour(225, 225, 225));
    wxStaticText* label3 = new wxStaticText(this, -1, "Sex:");
    label3->SetFont(wxFont(wxFontInfo(12).Bold()));
    label3->SetBackgroundColour(wxColour(190, 190, 190));
    sex = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(70, 20), wxBORDER_SIMPLE);
    sex->SetFont(wxFont(wxFontInfo(11)));
    sex->SetBackgroundColour(wxColour(225, 225, 225));
    wxStaticText* label4 = new wxStaticText(this, -1, "Height:");
    label4->SetFont(wxFont(wxFontInfo(12).Bold()));
    label4->SetBackgroundColour(wxColour(190, 190, 190));
    height = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(70, 20), wxTE_READONLY | wxBORDER_SIMPLE);
    height->SetFont(wxFont(wxFontInfo(11)));
    height->SetBackgroundColour(wxColour(225, 225, 225));

    wxBoxSizer* firstLine = new wxBoxSizer(wxHORIZONTAL);
    firstLine->Add(label2, wxSizerFlags(0).Border(wxLEFT, 10));
    firstLine->Add(age, wxSizerFlags(0));
    firstLine->Add(label3, wxSizerFlags(0).Border(wxLEFT, 20));
    firstLine->Add(sex, wxSizerFlags(0));
    firstLine->Add(label4, wxSizerFlags(0).Border(wxLEFT, 20));
    firstLine->Add(height, wxSizerFlags(0).Border(wxRIGHT, 5));

    wxStaticText* label5 = new wxStaticText(this, -1, "Nickname:");
    label5->SetFont(wxFont(wxFontInfo(12).Bold()));
    label5->SetBackgroundColour(wxColour(190, 190, 190));
    nick = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(100, 20), wxTE_READONLY | wxBORDER_SIMPLE);
    nick->SetFont(wxFont(wxFontInfo(11)));
    nick->SetBackgroundColour(wxColour(225, 225, 225));
    wxStaticText* label6 = new wxStaticText(this, -1, "Nationality:");
    label6->SetFont(wxFont(wxFontInfo(12).Bold()));
    label6->SetBackgroundColour(wxColour(190, 190, 190));
    nat = new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(120, 20), wxTE_READONLY | wxBORDER_SIMPLE);
    nat->SetFont(wxFont(wxFontInfo(11)));
    nat->SetBackgroundColour(wxColour(225, 225, 225));

    wxBoxSizer* secondLine = new wxBoxSizer(wxHORIZONTAL);
    secondLine->Add(label5, wxSizerFlags(0).Border(wxLEFT, 10));
    secondLine->Add(nick, wxSizerFlags(0));
    secondLine->Add(label6, wxSizerFlags(0).Border(wxLEFT, 20));
    secondLine->Add(nat, wxSizerFlags(0).Border(wxRIGHT, 5));

    wxStaticText* label7 = new wxStaticText(this, -1, "Appearance");
    label7->SetFont(wxFont(wxFontInfo(12).Bold()));
    label7->SetBackgroundColour(wxColour(180, 180, 180));
    appearance = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    appearance->SetFont(wxFont(wxFontInfo(9)));
    appearance->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label8 = new wxStaticText(this, -1, "Personality");
    label8->SetFont(wxFont(wxFontInfo(12).Bold()));
    label8->SetBackgroundColour(wxColour(180, 180, 180));
    personality = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    personality->SetFont(wxFont(wxFontInfo(9)));
    personality->SetBackgroundColour(wxColour(225, 225, 225));

    wxStaticText* label9 = new wxStaticText(this, -1, "Backstory");
    label9->SetFont(wxFont(wxFontInfo(12).Bold()));
    label9->SetBackgroundColour(wxColour(180, 180, 180));
    backstory = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(-1, 80), wxTE_READONLY | wxTE_MULTILINE | wxTE_NO_VSCROLL);
    backstory->SetFont(wxFont(wxFontInfo(9)));
    backstory->SetBackgroundColour(wxColour(225, 225, 225));

    apbSizer = new wxBoxSizer(wxVERTICAL);
    apbSizer->Add(label7, wxSizerFlags(0).Border(wxLEFT | wxRIGHT, 10));
    apbSizer->Add(appearance, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    apbSizer->Add(label8, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10));
    apbSizer->Add(personality, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM, 10).Expand());
    apbSizer->Add(label9, wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxTOP, 10));
    apbSizer->Add(backstory, wxSizerFlags(0).Border(wxLEFT | wxRIGHT, 10).Expand());

    image = new ImagePanel(this, wxDefaultPosition, wxSize(200, 200));
    image->SetBackgroundColour(wxColour(150, 150, 150));
    image->setBorderColour(wxColour(20, 20, 20));

    vertical = new  wxBoxSizer(wxVERTICAL);
    vertical->Add(role, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    vertical->Add(image, wxSizerFlags(1).CenterHorizontal().Shaped());
    vertical->SetItemMinSize(1, wxSize(200, 200));
    vertical->Add(label1Sizer, wxSizerFlags(0).Expand().Border(wxTOP | wxBOTTOM, 15));
    vertical->Add(firstLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    vertical->Add(secondLine, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    vertical->Add(apbSizer, wxSizerFlags(0).Border(wxTOP, 10).Expand());

    SetSizer(vertical);

    this->FitInside();

    this->SetScrollRate(20, 20);

    appearance->SetCursor(wxCURSOR_DEFAULT);
    personality->SetCursor(wxCURSOR_DEFAULT);
    backstory->SetCursor(wxCURSOR_DEFAULT);
}

void CharacterShowcase::setData(wxImage& set, vector<string>& charData) {
    if (set.IsOk())
        image->setImage(set);
    else
        image->ClearBackground();

    label1->SetLabel(charData[0]);
    sex->SetLabel(charData[1]);

    if (charData[1] == "Female")
        sex->SetBackgroundColour(wxColour(255, 182, 193));
    else if (charData[1] == "Male")
        sex->SetBackgroundColour(wxColour(139, 186, 255));
    else
        sex->SetBackgroundColour(wxColour(220, 220, 220));
    
    sex->Refresh();

    age->SetLabel(charData[2]);
    height->SetLabel(charData[4]);
    nat->SetLabel(charData[3]);
    nick->SetLabel(charData[5]);
    role->SetLabel(charData[6]);

    if (charData[6] == "Main")
        role->SetBackgroundColour(wxColour(230, 60, 60));
    else
        role->SetBackgroundColour(wxColour(220, 220, 220));

    appearance->SetValue(charData[7]);
    personality->SetValue(charData[8]);
    backstory->SetValue(charData[9]);

    int nol;

    nol = appearance->GetNumberOfLines();
    if (nol > 5)
        apbSizer->SetItemMinSize(size_t(1), wxSize(-1, nol * 16));
    else
        apbSizer->SetItemMinSize(size_t(1), wxSize(-1, 80));

    nol = personality->GetNumberOfLines();
    if (nol > 5)
        apbSizer->SetItemMinSize(size_t(3), wxSize(-1, nol * 16));
    else
        apbSizer->SetItemMinSize(size_t(3), wxSize(-1, 80));

    nol = backstory->GetNumberOfLines();
    if (nol > 5)
        apbSizer->SetItemMinSize(size_t(5), wxSize(-1, nol * 16));
    else
        apbSizer->SetItemMinSize(size_t(5), wxSize(-1, 80));

    apbSizer->Layout();
    FitInside();
}
