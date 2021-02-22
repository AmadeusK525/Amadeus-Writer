#include "CorkboardShapes.h"

#include "Corkboard.h"

XS_IMPLEMENT_CLONABLE_CLASS(AutoWrapTextShape,
	wxSFEditTextShape);

bool AutoWrapTextShape::m_countLines = true;

AutoWrapTextShape::AutoWrapTextShape() : wxSFEditTextShape() {
	SetFill(m_bgColour);
	m_sText = "";
}

AutoWrapTextShape::AutoWrapTextShape(const AutoWrapTextShape& other) : wxSFEditTextShape(other) {
	SetFill(m_bgColour);
	m_sText = other.m_sText;
	m_bgColour = other.m_bgColour;
}

void AutoWrapTextShape::UpdateRectSize() {
	wxSFShapeBase* parent = GetParentShape();

	if (!parent) {
		wxSFEditTextShape::UpdateRectSize();
		return;
	}

	wxSize tsize = parent->GetBoundingBox().GetSize();
	tsize.y -= m_topSpace;

	if (m_height == -1) {
		if (tsize.y <= 1) tsize.y = 10;
		m_nRectSize.y = (double)tsize.y;
	} else
		m_nRectSize.y = m_height - m_topSpace;

	if (tsize.x <= 1) tsize.x = 10;
	m_nRectSize.x = (double)tsize.x;
}

void AutoWrapTextShape::DrawTextContent(wxDC& dc) {
	dc.SetBrush(m_Fill);

	dc.SetTextForeground(m_TextColor);
	dc.SetFont(m_Font);

	wxRect rect(GetBoundingBox());

	if (m_clipRegion)
		m_textToDraw.Empty();

	// Get wrapped text and draw all text lines
	if (m_countLines && rect.width > 30) {
		int numberOfLines = 1;

		CalcWrappedText(rect.width, rect.height);
	}

	dc.DrawText(m_textToDraw, rect.x, rect.y);
}

void AutoWrapTextShape::SetHeight(int height) {
	m_height = height;
	m_nRectSize.y = height;
}

void AutoWrapTextShape::OnLeftDoubleClick(const wxPoint& pos) {
	if (GetParentCanvas()) {
		int dx, dy;
		wxRealPoint shpPos = GetAbsolutePosition();
		double scale = GetParentCanvas()->GetScale();
		GetParentCanvas()->CalcUnscrolledPosition(0, 0, &dx, &dy);

		wxRect shpBB = GetBoundingBox();

		m_nCurrentState = GetStyle();
		RemoveStyle(sfsSIZE_CHANGE);

		int style = wxTE_BESTWRAP;
		if (m_forceMultiline)
			style = wxTE_MULTILINE;

		m_pTextCtrl = new wxSFContentCtrl(GetParentCanvas(), wxID_ANY,
			this, m_sText, wxPoint(int((shpPos.x * scale) - dx),
			int((shpPos.y * scale) - dy)),
			wxSize(int(shpBB.GetWidth() * scale), int(shpBB.GetHeight() * scale)), style);

		m_pTextCtrl->SetBackgroundColour(m_Fill.GetColour());
		m_pTextCtrl->SetForegroundColour(m_TextColor);
	}
}

void AutoWrapTextShape::CalcWrappedText(int length, int height) {
	m_textToDraw = m_sText;

	if (m_textToDraw.IsEmpty())
		return;

	wxSize textSize;

	int begin = 0;
	int end = 0;
	int strLen = m_sText.Length();

	int numberOfLines = 1;
	if (m_nLineHeight > 0)
		numberOfLines = height / m_nLineHeight;

	bool first = true;
	int maxChar = 0;

	while (numberOfLines >= 0) {
		if (end >= begin + maxChar || first)
			end += 2;
		else
			end += maxChar;

		if (end > strLen)
			end = strLen;

		textSize = GetTextExtent(m_textToDraw.SubString(begin, end));

		if (textSize.x >= length - 2 && numberOfLines != 0) {
			size_t found = m_textToDraw.rfind(" ", end);

			if (found != std::string::npos)
				m_textToDraw.replace(found, 1, "\n");
			else
				m_textToDraw.replace(end, 1, "-");

			begin = found;
			end = found;

			numberOfLines--;
		} else if (numberOfLines == 0 || end >= strLen ||
			GetTextExtent(m_textToDraw.Left(end)).y > height) {

			if (end < strLen) {
				m_textToDraw.Remove(end - 3);
				m_textToDraw << "...";
			} else
				m_textToDraw.Remove(end);

			break;
		}

	}
}


////////////////////////////////////////////////////////////////////
//////////////////////////// NoteShape /////////////////////////////
////////////////////////////////////////////////////////////////////


// Implement RTTI information and Clone() functions.
XS_IMPLEMENT_CLONABLE_CLASS(NoteShape,
	wxSFRoundRectShape);

NoteShape::NoteShape() : wxSFRoundRectShape() {
	m_currentColour = CorkboardCanvas::MENU_NoteDefault;

	// Set accepted connections for the new shape.
	AcceptConnection("All");
	AcceptSrcNeighbour("NoteShape");
	AcceptTrgNeighbour("NoteShape");
	AcceptChild("None");

	SetRadius(4.0);
	SetFill(wxBrush(wxColour(80, 80, 80)));

	AddStyle(sfsNO_FIT_TO_CHILDREN);

	m_content = new AutoWrapTextShape();

	if (m_content) {
		m_content->SetVAlign(wxSFShapeBase::valignBOTTOM);
		m_content->SetHAlign(wxSFShapeBase::halignCENTER);

		m_content->SetVBorder(10.0);
		m_content->SetTopSpace(30.0);

		// Set required shape style(s)
		m_content->SetStyle(sfsALWAYS_INSIDE | sfsHOVERING | sfsPROCESS_DEL |
			sfsPROPAGATE_SELECTION | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_INTERACTIVE_CONNECTION);

		// Components of composite shapes created at runtime in parent
		// shape constructor cannot be re-created by the serializer so
		// it is important to disable their automatic serialization ...
		m_content->EnableSerialization(false);

		// ... but their properties can be serialized in the standard way:
		XS_SERIALIZE_DYNAMIC_OBJECT_NO_CREATE(m_content, "content");

		// Assign the text shape to the parent shape.
		AddChild(m_content);
	}

	Scale(2.3, 2.6);

	XS_SERIALIZE_INT_EX(m_currentColour, "currentColour", CorkboardCanvas::MENU_NoteDefault);
}

NoteShape::NoteShape(const NoteShape& other) : wxSFRoundRectShape(other) {
	m_currentColour = other.m_currentColour;

	// Clone source child text object..
	m_content = (AutoWrapTextShape*)other.m_content->Clone();

	if (m_content) {
		// .. and append it to this shapes as its child.
		AddChild(m_content);

		// This object is created by the parent class constructor and
		// not by the serializer (only its properties are deserialized.
		XS_SERIALIZE_DYNAMIC_OBJECT_NO_CREATE(m_content, "content");
	}
}

void NoteShape::ChangeColour(wxWindowID id) {
	wxColour frameColour, textBgColour, textFgColour;
	textFgColour = wxColour(20, 20, 20);

	switch (id) {
	case CorkboardCanvas::MENU_NoteDefault:
		frameColour = wxColour(80, 80, 80);
		textBgColour = wxColour(255, 255, 255);

		break;

	case CorkboardCanvas::MENU_NoteBlack:
		frameColour = wxColour(20, 20, 20);
		textBgColour = wxColour(80, 80, 80);
		textFgColour = wxColour(230, 230, 230);

		break;

	case CorkboardCanvas::MENU_NoteWhite:
		frameColour = wxColour(230, 230, 230);
		textBgColour = wxColour(255, 255, 255);

		break;

	case CorkboardCanvas::MENU_NoteRed:
		frameColour = wxColour(200, 0, 0);
		textBgColour = wxColour(200, 100, 100);
		textFgColour = wxColour(20, 20, 20);

		break;

	case CorkboardCanvas::MENU_NoteGreen:
		frameColour = wxColour(0, 180, 0);
		textBgColour = wxColour(130, 200, 130);
		textFgColour = wxColour(10, 10, 10);

		break;

	case CorkboardCanvas::MENU_NoteBlue:
		frameColour = wxColour(0, 0, 180);
		textBgColour = wxColour(130, 130, 200);

		break;

	case CorkboardCanvas::MENU_NoteYellow:
		frameColour = wxColour(245, 245, 0);
		textBgColour = wxColour(255, 255, 130);

		break;

	case CorkboardCanvas::MENU_NotePink:
		frameColour = wxColour(255, 20, 147);
		textBgColour = wxColour(255, 182, 193);

		break;
	}

	SetFill(frameColour);
	m_content->SetFill(textBgColour);
	m_content->SetTextColour(textFgColour);

	m_currentColour = id;
	((amCorkboard*)((CorkboardCanvas*)GetParentCanvas())->GetParent())->Save();
}

void NoteShape::OnBeginHandle(wxSFShapeHandle& handle) {
	ShouldCountLines(false);
	ShouldClip(true);
	wxSFRoundRectShape::OnBeginHandle(handle);
}

void NoteShape::OnHandle(wxSFShapeHandle& handle) {
	m_content->UpdateRectSize();
	wxSFRoundRectShape::OnHandle(handle);
}

void NoteShape::OnEndHandle(wxSFShapeHandle& handle) {
	ShouldCountLines(true);
	ShouldClip(false);

	wxSFRoundRectShape::OnEndHandle(handle);
	((amCorkboard*)((CorkboardCanvas*)GetParentCanvas())->GetParent())->Save();
}

void NoteShape::OnBeginDrag(const wxPoint& pos) {
	ShouldCountLines(false);
	wxSFRoundRectShape::OnBeginDrag(pos);
}

void NoteShape::OnEndDrag(const wxPoint& pos) {
	ShouldCountLines(true);

	wxSFRoundRectShape::OnEndDrag(pos);
	((amCorkboard*)((CorkboardCanvas*)GetParentCanvas())->GetParent())->Save();
}


////////////////////////////////////////////////////////////////////
//////////////////////////// ImageShape ////////////////////////////
////////////////////////////////////////////////////////////////////


XS_IMPLEMENT_CLONABLE_CLASS(ImageShape, wxSFBitmapShape)

ImageShape::ImageShape() : wxSFBitmapShape() {

}

ImageShape::ImageShape(const ImageShape& other) : wxSFBitmapShape(other) {
	ratio = other.ratio;
}

ImageShape::~ImageShape() {}

void ImageShape::Create(const wxString& path, wxBitmapType type) {
	CreateFromFile(path, type);
	wxImage image(path);

	ratio = (double)m_Bitmap.GetWidth() / (double)m_Bitmap.GetHeight();
}

void ImageShape::OnBeginHandle(wxSFShapeHandle& handle) {
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
	wxSFBitmapShape::OnEndHandle(handle);
	((amCorkboard*)((CorkboardCanvas*)GetParentCanvas())->GetParent())->Save();
}

void ImageShape::OnEndDrag(const wxPoint& pos) {
	wxSFBitmapShape::OnEndDrag(pos);
	((amCorkboard*)((CorkboardCanvas*)GetParentCanvas())->GetParent())->Save();
}

TextShape::TextShape() {}

TextShape::TextShape(const TextShape& other) : wxSFEditTextShape(other) {}

void TextShape::OnEndHandle(wxSFShapeHandle& handle) {
	wxSFEditTextShape::OnEndHandle(handle);
	((amCorkboard*)((CorkboardCanvas*)GetParentCanvas())->GetParent())->Save();
}

void TextShape::OnEndDrag(const wxPoint& pos) {
	wxSFEditTextShape::OnEndDrag(pos);
	((amCorkboard*)((CorkboardCanvas*)GetParentCanvas())->GetParent())->Save();
}
