#ifndef IMAGESHAPE_H_
#define IMAGESHAPE_H_

#pragma once

#include "wx\wxsf\wxShapeFramework.h"

class ImageShape: public wxSFBitmapShape {
private:
	double ratio;

public:
	ImageShape();
	ImageShape(const ImageShape& other);
	virtual ~ImageShape();

	void create(const wxString& path, wxBitmapType type = wxBITMAP_TYPE_ANY);

	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	//virtual void OnEndHandle(wxSFShapeHandle& handle);

	XS_DECLARE_CLONABLE_CLASS(ImageShape)
};

#endif