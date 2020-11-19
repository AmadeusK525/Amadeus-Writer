#include "NoteShape.h"

// Implement RTTI information and Clone() functions.
XS_IMPLEMENT_CLONABLE_CLASS(NoteShape,
	wxSFRoundRectShape);

NoteShape::NoteShape() : wxSFRoundRectShape() {
	// Set accepted connections for the new shape.
	AcceptConnection("All");
	AcceptSrcNeighbour("NoteShape");
	AcceptTrgNeighbour("NoteShape");
	AcceptChild("None");

	SetRadius(6.0);
	SetFill(wxBrush(wxColour(80, 80, 80)));

	AddStyle(sfsNO_FIT_TO_CHILDREN);

	content = new AutoWrapTextShape();

	if (content) {
		content->SetVAlign(wxSFShapeBase::valignBOTTOM);
		content->SetHAlign(wxSFShapeBase::halignCENTER);

		content->SetVBorder(10.0);

		// Set required shape style(s)
		content->SetStyle(sfsALWAYS_INSIDE | sfsHOVERING | sfsPROCESS_DEL |
			sfsPROPAGATE_SELECTION | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_INTERACTIVE_CONNECTION);

		// Components of composite shapes created at runtime in parent
		// shape constructor cannot be re-created by the serializer so
		// it is important to disable their automatic serialization ...
		content->EnableSerialization(false);

		// ... but their properties can be serialized in the standard way:
		XS_SERIALIZE_DYNAMIC_OBJECT_NO_CREATE(content, "content");

		// Assign the text shape to the parent shape.
		AddChild(content);
	}

	Scale(2.3, 2.6);
	content->SetText("New note");
}

NoteShape::NoteShape(const NoteShape& other) : wxSFRoundRectShape(other) {
	// Clone source child text object..
	content = (AutoWrapTextShape*)other.content->Clone();

	if (content) {
		// .. and append it to this shapes as its child.
		AddChild(content);

		// This object is created by the parent class constructor and
		// not by the serializer (only its properties are deserialized.
		XS_SERIALIZE_DYNAMIC_OBJECT_NO_CREATE(content, "content");
	}
}

void NoteShape::changeColour(wxWindowID id) {
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
	content->SetFill(textBgColour);
	content->SetTextColour(textFgColour);

	curColour = id;
}

void NoteShape::OnBeginHandle(wxSFShapeHandle& handle) {
	willCountLines(false);
	willClip(true);
	wxSFRoundRectShape::OnBeginHandle(handle);
}

void NoteShape::OnHandle(wxSFShapeHandle& handle) {
	content->UpdateRectSize();
	wxSFRoundRectShape::OnHandle(handle);
}

void NoteShape::OnEndHandle(wxSFShapeHandle& handle) {
	willCountLines(true);
	willClip(false);
	wxSFRoundRectShape::OnEndHandle(handle);
}

void NoteShape::OnBeginDrag(const wxPoint& pos) {
	willCountLines(false);
	wxSFRoundRectShape::OnBeginDrag(pos);
}

void NoteShape::OnEndDrag(const wxPoint& pos) {
	willCountLines(true);
	wxSFRoundRectShape::OnEndDrag(pos);
}
