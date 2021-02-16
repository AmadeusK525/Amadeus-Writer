#include "TimelineShapes.h"

XS_IMPLEMENT_CLONABLE_CLASS(TimelineCard, wxSFRoundRectShape);

TimelineCard::TimelineCard() {
	SetRadius(0.3);
	SetRectSize(400, 280);

	RemoveStyle(sfsPOSITION_CHANGE);
	RemoveStyle(sfsSHOW_HANDLES);
	AddStyle(sfsHIGHLIGHTING);
	RemoveStyle(sfsSIZE_CHANGE);

	AddStyle(sfsSHOW_SHADOW);

	MarkSerializableDataMembers();
}

TimelineCard::TimelineCard(int row, int col) : TimelineCard() {
	m_row = row;
	m_col = col;
}

TimelineCard::TimelineCard(const TimelineCard& other) : wxSFRoundRectShape(other) {

}

void TimelineCard::RecalculatePosition() {
	MoveTo((60 * (m_col)) + 30 + (400 * m_col), (30 * (m_row + 1)) + (280 * m_row));
}

void TimelineCard::MarkSerializableDataMembers() {
	XS_SERIALIZE_EX(m_row, wxT("row"), -1);
	XS_SERIALIZE_EX(m_col, wxT("column"), -1);
}
