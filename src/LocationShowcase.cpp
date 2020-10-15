#include "LocationShowcase.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(LocationShowcase, wxScrolledWindow)

EVT_SIZE(LocationShowcase::resizeImage)

END_EVENT_TABLE()

LocationShowcase::LocationShowcase(wxWindow* parent) :
    wxScrolledWindow(parent, wxID_ANY) {

    image = new ImagePanel(this, wxDefaultPosition, wxSize(200, 200));
    image->SetBackgroundColour(wxColour(150, 150, 150));
    image->setBorderColour(wxColour(20, 20, 20));

    name = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 22), wxTE_READONLY | wxTE_CENTER);
    name->SetBackgroundColour(wxColour(220, 220, 220));
    name->SetFont(wxFont(wxFontInfo(12).Bold()));

    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    nameSizer->AddStretchSpacer(1);
    nameSizer->Add(name, 4);
    nameSizer->AddStretchSpacer(1);

    importance = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(100, 25), wxTE_READONLY | wxTE_CENTER);
    importance->SetBackgroundColour(wxColour(220, 220, 220));
    importance->SetFont(wxFont(wxFontInfo(12).Bold()));

    wxBoxSizer* vertical = new  wxBoxSizer(wxVERTICAL);
    vertical->Add(importance, wxSizerFlags(0).CenterHorizontal().Border(wxBOTTOM, 10));
    vertical->Add(image, wxSizerFlags(1).CenterHorizontal().Shaped());
    vertical->Add(nameSizer, wxSizerFlags(0).Expand().Border(wxTOP | wxBOTTOM, 15));
    vertical->AddStretchSpacer(2);
    vertical->SetItemMinSize(size_t(1), wxSize(200, 200));

    SetSizer(vertical);

    this->FitInside();
    this->SetScrollRate(15, 15);
}

void LocationShowcase::setData(wxImage& set, vector<string> locData) {
    //image->adjustSize(mainSizer->GetItem(1)->GetSize());
    image->setImage(set);

    name->SetValue(locData[0]);
}

void LocationShowcase::resizeImage(wxSizeEvent& event) {
    //image->adjustSize(mainSizer->GetItem(1)->GetSize());
}
