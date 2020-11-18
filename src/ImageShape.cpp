#include "ImageShape.h"
#include "CorkboardCanvas.h"

XS_IMPLEMENT_CLONABLE_CLASS(ImageShape, wxSFBitmapShape)

ImageShape::ImageShape(): wxSFBitmapShape() {
	
}

ImageShape::ImageShape(const ImageShape& other): wxSFBitmapShape(other) {
	
}

ImageShape::~ImageShape() {}

void ImageShape::create(const wxString& path, wxBitmapType type) {
	CreateFromFile(path, type);
	wxImage image(path);
	
	int width = image.GetWidth();
	int height = image.GetHeight();

	ratio = (double)width / (double)height;
}

void ImageShape::OnBeginHandle(wxSFShapeHandle& handle){
	wxSFBitmapShape::OnBeginHandle(handle);
}

void ImageShape::OnHandle(wxSFShapeHandle& handle) {
	wxRect rect(GetBoundingBox());
	wxPoint dxy(handle.GetDelta());

	switch (handle.GetType()) {
	case wxSFShapeHandle::hndRIGHTBOTTOM:
	case wxSFShapeHandle::hndBOTTOM:
		m_nRectSize.y += dxy.y;
		m_nRectSize.x = m_nRectSize.y * ratio;
		break;
	case wxSFShapeHandle::hndLEFTBOTTOM:
		m_nRectSize.x -= dxy.x;
		m_nRectSize.y = m_nRectSize.x / ratio;
		m_nRelativePosition.x += dxy.x;
		break;
	case wxSFShapeHandle::hndRIGHT:
		m_nRectSize.x += dxy.x;
		m_nRectSize.y = m_nRectSize.x / ratio;
		break;
	case wxSFShapeHandle::hndLEFT:
		m_nRectSize.x -= dxy.x;
		m_nRectSize.y = m_nRectSize.x / ratio;
		m_nRelativePosition.x += dxy.x;
		break;
	case wxSFShapeHandle::hndLEFTTOP:
	{
		m_nRectSize.x -= dxy.x;
		double prevY = m_nRectSize.y;
		m_nRectSize.y = m_nRectSize.x / ratio;
		m_nRelativePosition.x += dxy.x;
		m_nRelativePosition.y += prevY - m_nRectSize.y;
		break;
	}
	case wxSFShapeHandle::hndRIGHTTOP:
	case wxSFShapeHandle::hndTOP:
		m_nRectSize.y -= dxy.y;
		m_nRectSize.x = m_nRectSize.y * ratio;
		m_nRelativePosition.y += dxy.y;
		break;
	}

	//Scale(ratio, ratio);
}
