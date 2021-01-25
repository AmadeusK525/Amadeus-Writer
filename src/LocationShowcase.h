#ifndef LOCATIONSHOWCASE_H_
#define LOCATIONSHOWCASE_H_
#pragma once

#include <string>
#include <vector>

#include <wx/wx.h>

#include "ImagePanel.h"
#include "Elements.h"

using std::string;
using std::vector;
using std::pair;

class LocationShowcase : public wxScrolledWindow {
private:
    ImagePanel* m_image = nullptr;

    wxStaticText* m_generalLabel = nullptr,
        * m_natLabel = nullptr,
        * m_archLabel = nullptr,
        * m_poliLabel = nullptr,
        * m_ecoLabel = nullptr,
        * m_culLabel = nullptr;

    wxStaticText* m_name = nullptr,
        * m_importance = nullptr;

    wxTextCtrl* m_general = nullptr,
        * m_natural = nullptr,
        * m_architecture = nullptr,
        * m_politics = nullptr,
        * m_economy = nullptr,
        * m_culture = nullptr;

    wxBoxSizer* m_vertical = nullptr,
        * m_firstColumn = nullptr,
        * m_secondColumn = nullptr;

    vector<pair<wxStaticText*, wxTextCtrl*>> m_custom{};

    bool m_first = true;

public:
    LocationShowcase(wxWindow* parent);

    void setData(Location& location);
};

#endif