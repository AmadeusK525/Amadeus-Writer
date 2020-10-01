#pragma once

#include "wx/wx.h"
#include "wx/bitmap.h"
#include "wx/custombgwin.h"

class ImagePanel : public wxCustomBackgroundWindow<wxScrolledWindow> {
private:
    wxImage image{};

public:
    wxColour colour{150, 150, 150};
    wxSize size{};

public:
    ImagePanel(wxWindow* parent,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize);

    void adjustSize(const wxSize& size);
    void setBorderColour(const wxColour& colour);

    void setImage(wxImage& image);
    void newScale();

    void setImageAsIs(wxImage& image);
};

