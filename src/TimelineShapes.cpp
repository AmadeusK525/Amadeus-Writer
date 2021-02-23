#include "TimelineShapes.h"

#include "Timeline.h"

XS_IMPLEMENT_CLONABLE_CLASS(TimelineCard, wxSFRoundRectShape);

int TimelineCard::m_verSpacing = 10;
int TimelineCard::m_horSpacing = 20;
int TimelineCard::m_width = 250;
int TimelineCard::m_height = 150;

TimelineCard::TimelineCard() {
	SetRadius(0.3);
	SetRectSize(m_width, m_height);

	RemoveStyle(sfsPOSITION_CHANGE);
	RemoveStyle(sfsSHOW_HANDLES);
	RemoveStyle(sfsSIZE_CHANGE);
	AddStyle(sfsHIGHLIGHTING);
	AddStyle(sfsHOVERING);

	AddStyle(sfsSHOW_SHADOW);

	m_title = new AutoWrapTextShape();

	if (m_title) {
		m_title->SetVAlign(wxSFShapeBase::valignTOP);
		m_title->SetHAlign(wxSFShapeBase::halignCENTER);

		m_title->SetVBorder(5.0);
		m_title->SetHeight(20);
		m_title->ForceMultiline(false);

		// Set required shape style(s)
		m_title->SetStyle(sfsALWAYS_INSIDE | sfsHOVERING | sfsPROCESS_DEL |
			sfsPROPAGATE_SELECTION | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_INTERACTIVE_CONNECTION);

		// Components of composite shapes created at runtime in parent
		// shape constructor cannot be re-created by the serializer so
		// it is important to disable their automatic serialization ...
		m_title->EnableSerialization(false);

		// ... but their properties can be serialized in the standard way:
		XS_SERIALIZE_DYNAMIC_OBJECT_NO_CREATE(m_title, "title");

		// Assign the text shape to the parent shape.
		AddChild(m_title);
		m_title->SetFont(wxFontInfo(12).Bold());
		m_title->SetFill(wxBrush(wxColour(255, 255, 255, 128)));
	}

	m_content = new AutoWrapTextShape();

	if (m_content) {
		m_content->SetVAlign(wxSFShapeBase::valignBOTTOM);
		m_content->SetHAlign(wxSFShapeBase::halignCENTER);

		m_content->SetVBorder(10.0);
		m_content->SetTopSpace(40);

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

	MarkSerializableDataMembers();
}

TimelineCard::TimelineCard(int row, int col) : TimelineCard() {
	m_row = row;
	m_col = col;
}

TimelineCard::TimelineCard(const TimelineCard& other) : wxSFRoundRectShape(other) {

}

void TimelineCard::RecalculatePosition() {
	int sectionMarkerWidth = TimelineSection::GetMarkerWidth();
	int sectionHorSpacing = TimelineSection::GetHorizontalSpcaing();

	int x = ((m_width + m_horSpacing) * m_col) +
		(sectionMarkerWidth + (m_horSpacing / 2)) +
		(m_section * (sectionHorSpacing + (sectionMarkerWidth * 2)));
	
	int y = (m_verSpacing * (m_row + 1)) +
		(m_height * m_row);

	MoveTo(x, y);
}

void TimelineCard::SetColour(wxColour& colour) {
	SetFill(colour);

	unsigned char r = colour.Red();
	unsigned char g = colour.Green();
	unsigned char b = colour.Blue();

	if (r >= 215)
		r = 255;
	else
		r += 40;

	if (g >= 215)
		g = 255;
	else
		g += 40;

	if (b >= 215)
		b = 255;
	else
		b += 40;

	m_content->SetFill(wxColour(r, g, b));
}

void TimelineCard::MarkSerializableDataMembers() {
	XS_SERIALIZE_EX(m_row, wxT("row"), -1);
	XS_SERIALIZE_EX(m_col, wxT("column"), -1);
}

bool TimelineCard::operator<(const TimelineCard& other) {
	return m_col < other.m_col;
}
