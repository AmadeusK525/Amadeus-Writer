#ifndef TIMELINESHAPES_H_
#define TIMELINESHAPES_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include "CorkboardShapes.h"

class TimelineCard : public wxSFRoundRectShape {
private:
	int m_row = -1;
	int m_col = -1;

	AutoWrapTextShape* m_content = nullptr;

public:
	XS_DECLARE_CLONABLE_CLASS(TimelineCard);

	TimelineCard();
	TimelineCard(int row, int col);
	TimelineCard(const TimelineCard& other);

	void RecalculatePosition();

	int GetRow() { return m_row; }
	int GetColumn() { return m_col; }

	void SetRow(int row) { m_row = row; }
	void SetColumn(int col) { m_col = col; }

	void IncrementColumn(int incrementBy = 1) { m_col += incrementBy; }
	void DecrementColumn(int decrementBy = 1) { m_col -= decrementBy; }

	void SetColour(wxColour& colour);

	void MarkSerializableDataMembers();
};

#endif