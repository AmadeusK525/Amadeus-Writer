#include "ImageShape.h"
#include "Corkboard.h"
#include "MyApp.h"

XS_IMPLEMENT_CLONABLE_CLASS(ImageShape, wxSFBitmapShape)

ImageShape::ImageShape(): wxSFBitmapShape() {
	
}

ImageShape::ImageShape(const ImageShape& other): wxSFBitmapShape(other) {
	ratio = other.ratio;
}

ImageShape::~ImageShape() {}

void ImageShape::Create(const wxString& path, wxBitmapType type) {
	CreateFromFile(path, type);
	wxImage image(path);
	
	ratio = (double)m_Bitmap.GetWidth() / (double)m_Bitmap.GetHeight();
}

void ImageShape::OnBeginHandle(wxSFShapeHandle& handle){
	if (ratio == 99999.9) {
		ratio = (double)m_Bitmap.GetWidth() / (double)m_Bitmap.GetHeight();
	}
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

void ImageShape::OnEndHandle(wxSFShapeHandle& handle) {
	amGetManager()->SetSaved(false);
	wxSFBitmapShape::OnEndHandle(handle);
}

void ImageShape::OnEndDrag(wxPoint& pos) {
	amGetManager()->SetSaved(false);
	wxSFBitmapShape::OnEndDrag(pos);
}