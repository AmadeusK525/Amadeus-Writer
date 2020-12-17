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
    ImagePanel* m_image = nullptr;
    wxStaticText* m_name = nullptr,
        * m_importance = nullptr,
        * m_type = nullptr;

    wxTextCtrl* m_background = nullptr,
        * m_natural = nullptr,
        * m_architecture = nullptr,
        * m_economy = nullptr,
        * m_culture = nullptr;

    wxBoxSizer* m_vertical = nullptr,
        * m_firstColumn = nullptr,
        * m_secondColumn = nullptr;

public:
    LocationShowcase(wxWindow* parent);

    void setData(wxImage& set, vector<string> locData);
};

#endif