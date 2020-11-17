#include "ImageShape.h"

XS_IMPLEMENT_CLONABLE_CLASS(ImageShape, wxSFBitmapShape)

ImageShape::ImageShape(): wxSFBitmapShape() {
	
}

ImageShape::ImageShape(const ImageShape& other): wxSFBitmapShape(other) {
	
}

ImageShape::~ImageShape() {}

void ImageShape::Scale(double x, double y, bool children) {
	if (m_fCanScale) {
		m_nRectSize.x *= x;
		m_nRectSize.y *= x;

		if (!m_fRescaleInProgress) RescaleImage(m_nRectSize);

		// call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, x, children);
	}
}