#ifndef IMAGESHAPE_H_
#define IMAGESHAPE_H_

#pragma once

#include "wx\wxsf\wxShapeFramework.h"

class ImageShape: public wxSFBitmapShape {
private:
	double scale;

public:
	ImageShape();
	ImageShape(const ImageShape& other);
	virtual ~ImageShape();

	virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);

	//virtual void OnBeginHandle(wxSFShapeHandle& handle);
	//virtual void OnHandle(wxSFShapeHandle& handle);
	//virtual void OnEndHandle(wxSFShapeHandle& handle);

	XS_DECLARE_CLONABLE_CLASS(ImageShape)
};

#endif