#include "ImagePanel.h"

#include "wxmemdbg.h"

ImagePanel::ImagePanel(wxWindow* parent, const wxPoint& pos, const wxSize& size) {
    Create(parent, wxID_ANY, pos, size);
    SetPosition(pos);
    SetSize(size);
    SetBackgroundColour(wxColour(150, 150, 150));
    this->size = size;
}

void ImagePanel::adjustSize(const wxSize& size) {
    this->size = size;
    setImage(image);
}

void ImagePanel::setBorderColour(const wxColour& colour) {
    this->colour = colour;
}


bool ImagePanel::setImage(wxImage& im) {
    image = im;
    bool ok = image.IsOk();

    if (ok)
        newScale();

    SetBackgroundBitmap(image);

    if (!ok)
        return false;

    Update();
    Refresh();
    return true;
}

void ImagePanel::newScale() {
    int width = image.GetWidth();
    int height = image.GetHeight();

    float scale;

    int neww;
    int newh;

    scale = (float)width / height;
    newh = size.y;
    neww = size.y * scale;

    image.Rescale(neww, newh, wxIMAGE_QUALITY_HIGH);

    SetSize(wxSize(neww, newh));
    SetMinSize(wxSize(neww, newh));
}

void ImagePanel::setImageAsIs(wxImage& im) {
    image = im;
    image.Rescale(size.x, size.y);
    SetBackgroundBitmap(image);
    Refresh();
}

void ImagePanel::clear() {
    ClearBackground();
    image.Destroy();
}
