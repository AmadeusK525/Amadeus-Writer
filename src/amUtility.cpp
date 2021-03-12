#include "amUtility.h"

wxImage amGetScaledImage(int maxWidth, int maxHeight, wxImage& image) {
    int width = image.GetWidth();
    int height = image.GetHeight();

    double ratio;

    int neww, xoffset;
    int newh, yoffset;

    ratio = (double)width / height;

    if (width > height) {
        neww = maxWidth;
        newh = maxWidth / ratio;
        xoffset = 0;
        yoffset = (maxHeight / 2) - (newh / 2);
    } else {
        newh = maxHeight;
        neww = maxHeight * ratio;
        yoffset = 0;
        xoffset = (maxWidth / 2) - (neww / 2);
    }

    return image.Scale(neww, newh, wxIMAGE_QUALITY_HIGH).Size(wxSize(maxWidth, maxHeight), wxPoint(xoffset, yoffset));
}