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

	inline int GetThreadIndex() { return m_row; }
	inline int GetColumnIndex() { return m_col; }
	inline int GetSectionIndex() { return m_section; }

	inline wxString GetTitle() { return m_title->GetText(); }
	inline wxString GetContent() { return m_content->GetText(); }

	void SetTitle(const wxString& title);
	void SetContent(const wxString& content);

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

	void SetColour(const wxColour& colour);
	void SetTextColour(const wxColour& colour);

	void MarkSerializableDataMembers();

	bool operator<(const TimelineCard& other);
};

#endif