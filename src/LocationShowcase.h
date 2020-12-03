#ifndef LOCATIONSHOWCASE_H_
#define LOCATIONSHOWCASE_H_
#pragma once

#include <string>
#include <vector>

#include <wx/wx.h>

#include "ImagePanel.h"

using std::string;
using std::vector;

class LocationShowcase : public wxScrolledWindow {
private:
    ImagePanel* image = nullptr;

    wxStaticText* name = nullptr,
        * importance = nullptr,
        * type = nullptr;

    wxTextCtrl * background = nullptr,
        * natural = nullptr,
        * architecture = nullptr,
        * economy = nullptr,
        * culture = nullptr;

    wxBoxSizer* vertical = nullptr;

public:
    LocationShowcase(wxWindow* parent);

    void setData(wxImage& set, vector<string> locData);
};

#endif