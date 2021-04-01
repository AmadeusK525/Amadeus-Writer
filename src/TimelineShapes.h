#ifndef TIMELINESHAPES_H_
#define TIMELINESHAPES_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include "CorkboardShapes.h"

class TimelineCard : public wxSFRoundRectShape {
private:
	int m_row = -1, m_col = -1;
	int m_section = -1;

	static int m_horSpacing, m_verSpacing;
	static int m_width, m_height;

	AutoWrapTextShape* m_title = nullptr, * m_content = nullptr;

public:
	XS_DECLARE_CLONABLE_CLASS(TimelineCard);

	TimelineCard();
	TimelineCard(int row, int col);
	TimelineCard(const TimelineCard& other);

	void RecalculatePosition();

	void DrawOnOrigin(wxDC& dc, bool children = true);

	inline int GetRow() { return m_row; }
	inline int GetColumn() { return m_col; }
	inline int GetSection() { return m_section; }

	inline static int GetHorizontalSpacing() { return m_horSpacing; }
	inline static int GetVerticalSpacing() { return m_verSpacing; }
	inline static int GetWidth() { return m_width; }
	inline static int GetHeight() { return m_height; }

	inline static void SetHorizontalSpacing(int spacing) { m_horSpacing = spacing; }
	inline static void SetVerticalSpacing(int spacing) { m_verSpacing = spacing; }
	inline static void SetWidth(int width) { m_width = width; }
	inline static void SetHeight(int height) { m_height = height; }

	inline void SetRow(int row) { m_row = row; }
	inline void SetColumn(int col) { m_col = col; }
	inline void SetSection(int section) { m_section = section; }

	inline void IncrementColumn(int incrementBy = 1) { m_col += incrementBy; }
	inline void DecrementColumn(int decrementBy = 1) { m_col -= decrementBy; }

	void SetColour(wxColour& colour);

	void MarkSerializableDataMembers();

	bool operator<(const TimelineCard& other);
};

#endif