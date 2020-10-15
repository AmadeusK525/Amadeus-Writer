#pragma once

#include "string"
#include "vector"

#include "wx/wx.h"

#include "ImagePanel.h"

using std::string;
using std::vector;

class CharacterShowcase : public wxScrolledWindow {
private:
    ImagePanel* image = nullptr;
    wxTextCtrl* label1 = nullptr;
    wxTextCtrl* age = nullptr;
    wxTextCtrl* sex = nullptr;
    wxTextCtrl* nat = nullptr;
    wxTextCtrl* height = nullptr;
    wxTextCtrl* nick = nullptr;
    wxTextCtrl* role = nullptr;
    wxTextCtrl* appearance = nullptr;
    wxTextCtrl* personality = nullptr;
    wxTextCtrl* backstory = nullptr;

    wxBoxSizer* vertical = nullptr;
    wxBoxSizer* apbSizer = nullptr;

public:
    CharacterShowcase(wxWindow* parent);

    void setData(wxImage& image, vector<string>& charData);
};

