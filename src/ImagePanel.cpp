#include "ImagePanel.h"

#include "wxmemdbg.h"

ImagePanel::ImagePanel(wxWindow* parent, const wxPoint& pos, const wxSize& size)
{
	Create(parent, wxID_ANY, pos, size);
	SetPosition(pos);
	SetSize(size);
	SetBackgroundColour(wxColour(150, 150, 150));
	this->size = size;
}

void ImagePanel::AdjustSize(const wxSize& size)
{
	this->size = size;
	SetImage(image);
}

void ImagePanel::SetBorderColour(const wxColour& colour)
{
	this->colour = colour;
}


bool ImagePanel::SetImage(const wxImage& im)
{
	image = im;
	bool ok = image.IsOk();

	if ( ok )
		NewScale();

	SetBackgroundBitmap(image);
	Update();
	Refresh();

	return ok;
}

void ImagePanel::NewScale()
{
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

void ImagePanel::SetImageAsIs(wxImage& im)
{
	image = im;
	image.Rescale(size.x, size.y);
	SetBackgroundBitmap(image);
	Refresh();
}

void ImagePanel::Clear()
{
	ClearBackground();
	image.Destroy();
}
