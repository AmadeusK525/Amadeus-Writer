#include "TimelineShapes.h"

#include "Timeline.h"

#include "wxmemdbg.h"

XS_IMPLEMENT_CLONABLE_CLASS(amTLTimelineCard, wxSFRoundRectShape);

int amTLTimelineCard::m_verSpacing = 10;
int amTLTimelineCard::m_horSpacing = 40;
int amTLTimelineCard::m_width = 250;
int amTLTimelineCard::m_height = 150;

amTLTimelineCard::amTLTimelineCard()
{
	SetRadius(0.3);
	SetRectSize(m_width, m_height);

	RemoveStyle(sfsPOSITION_CHANGE);
	RemoveStyle(sfsSHOW_HANDLES);
	RemoveStyle(sfsSIZE_CHANGE);
	AddStyle(sfsHIGHLIGHTING);
	AddStyle(sfsHOVERING);

	AddStyle(sfsSHOW_SHADOW);

	m_title = new AutoWrapTextShape();

	if ( m_title )
	{
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

	if ( m_content )
	{
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

amTLTimelineCard::amTLTimelineCard(int row, int col) : amTLTimelineCard()
{
	m_row = row;
	m_col = col;
}

amTLTimelineCard::amTLTimelineCard(const amTLTimelineCard& other) : wxSFRoundRectShape(other)
{

}

void amTLTimelineCard::RecalculatePosition()
{
	SetRectSize(m_width, m_height);
	m_title->UpdateRectSize();
	m_content->UpdateRectSize();

	int sectionMarkerWidth = amTLSection::GetMarkerWidth();
	int sectionHorSpacing = amTLSection::GetHorizontalSpcaing();

	int x = ((m_width + m_horSpacing) * m_col)
		+ (sectionMarkerWidth + (m_horSpacing / 2))
		+ (m_section * (sectionHorSpacing + (sectionMarkerWidth * 2))
		+ amTLThread::GetTitleOffset());

	int y = (m_verSpacing * (m_row + 1)) +
		(m_height * m_row) +
		amTLSection::GetTitleOffset();

	MoveTo(x, y);
}

void amTLTimelineCard::DrawOnOrigin(wxDC& dc, bool children)
{
	double tempx = m_nRelativePosition.x, tempy = m_nRelativePosition.y;
	m_nRelativePosition = { 0, 0 };

	Draw(dc, children);

	m_nRelativePosition = { tempx, tempy };
}

void amTLTimelineCard::SetTitle(const wxString& title)
{
	m_title->SetText(title);
	m_title->CalcWrappedText();
}

void amTLTimelineCard::SetContent(const wxString& content)
{
	m_content->SetText(content);
	m_content->CalcWrappedText();
}

void amTLTimelineCard::SetColour(const wxColour& colour)
{
	SetFill(colour);

	unsigned char r = colour.Red();
	unsigned char g = colour.Green();
	unsigned char b = colour.Blue();

	if ( r >= 215 )
		r = 255;
	else
		r += 40;

	if ( g >= 215 )
		g = 255;
	else
		g += 40;

	if ( b >= 215 )
		b = 255;
	else
		b += 40;

	m_content->SetFill(wxColour(r, g, b));
}

void amTLTimelineCard::SetTextColour(const wxColour& colour)
{
	m_title->SetTextColour(colour);
	m_content->SetTextColour(colour);
}

void amTLTimelineCard::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(m_row, wxT("row"), -1);
	XS_SERIALIZE_EX(m_col, wxT("column"), -1);
	XS_SERIALIZE_EX(m_section, wxT("section"), -1);
}

bool amTLTimelineCard::operator<(const amTLTimelineCard& other)
{
	return m_col < other.m_col;
}
