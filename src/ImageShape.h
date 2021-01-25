#ifndef IMAGESHAPE_H_
#define IMAGESHAPE_H_

#pragma once

#include "wx\wxsf\wxShapeFramework.h"

class ImageShape: public wxSFBitmapShape {
private:
	double ratio = 99999.9;

public:
	ImageShape();
	ImageShape(const ImageShape& other);
	virtual ~ImageShape();

	void Create(const wxString& path, wxBitmapType type = wxBITMAP_TYPE_ANY);

	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	virtual void OnEndHandle(wxSFShapeHandle& handle);

	virtual void OnEndDrag(wxPoint& pos);

	XS_DECLARE_CLONABLE_CLASS(ImageShape)
};

#endif