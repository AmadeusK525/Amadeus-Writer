#include "NoteShape.h"

// Implement RTTI information and Clone() functions.
XS_IMPLEMENT_CLONABLE_CLASS(NoteShape,
	wxSFRoundRectShape);

NoteShape::NoteShape(): wxSFRoundRectShape() {
	// Set accepted connections for the new shape.
	AcceptConnection("All");
	AcceptSrcNeighbour("NoteShape");
	AcceptTrgNeighbour("NoteShape");

	SetRadius(8.0);
	SetFill(wxBrush(wxColour(80, 80, 80)));

	AddStyle(sfsNO_FIT_TO_CHILDREN);

	content = new AutoWrapTextShape();

	if (content) {
		content->SetVAlign(wxSFShapeBase::valignBOTTOM);
		content->SetHAlign(wxSFShapeBase::halignCENTER);

		content->SetVBorder(10.0);

		//content->SetFill(wxBrush(wxColour(230, 230, 230)));
		
		// Set required shape style(s)
		content->SetStyle(sfsALWAYS_INSIDE | sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_SELECTION);
		
		// Components of composite shapes created at runtime in parent
		// shape constructor cannot be re-created by the serializer so
		// it is important to disable their automatic serialization ...
		content->EnableSerialization(false);
		
		// ... but their properties can be serialized in the standard way:
		XS_SERIALIZE_DYNAMIC_OBJECT_NO_CREATE(content, "content");
		
		// Assign the text shape to the parent shape.
		AddChild(content);
	}
}

NoteShape::NoteShape(static NoteShape& other) : wxSFRoundRectShape(other) {
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
