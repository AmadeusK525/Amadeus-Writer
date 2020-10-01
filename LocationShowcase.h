#pragma once

#include "string"
#include "vector"

#include "wx/wx.h"
#include "ImagePanel.h"

using std::string;
using std::vector;

class LocationShowcase : public wxScrolledWindow {
private:
    ImagePanel* image = nullptr;
    wxTextCtrl* name = nullptr;
    wxTextCtrl* importance = nullptr;

public:
    LocationShowcase(wxWindow* parent);

    void setData(wxImage& set, vector<string> locData);
    void resizeImage(wxSizeEvent& event);

    DECLARE_EVENT_TABLE()
};

