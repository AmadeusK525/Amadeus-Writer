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

    void AdjustSize(const wxSize& size);
    void SetBorderColour(const wxColour& colour);

    bool SetImage(const wxImage& image);
    void NewScale();

    void SetImageAsIs(wxImage& image);

    void Clear();
    wxImage& GetImage() { return image; }
};

