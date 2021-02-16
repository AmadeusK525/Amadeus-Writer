#ifndef TIMELINE_H_
#define TIMELINE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include "TimelineShapes.h"

struct TimelineThread {
	wxRect rect{ -1,-1,-1,-1 };
	int y = -1;
	wxColour colour{ 0,0,0 };

	TimelineThread() = default;
	TimelineThread(int y, wxColour& colour) : y(y), colour(colour) {};
};

class TimelineCanvas : public wxSFShapeCanvas {
private:
	wxVector<TimelineThread> m_threads{};
	wxVector<int> m_separators;

	std::pair<wxPoint, wxRect> m_curDragCell{ wxPoint(-1, -1), wxRect(-1,-1, 400, 280) };

	bool m_drawSeparators = true;

	wxBitmap m_dragShapeBmp{};

public:
	TimelineCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL | wxVSCROLL);

	void AddThread(int pos, wxColour& colour, bool refresh = true);
	TimelineCard* AddCard(int row, int col, bool recalcPos = true);

	void SetCardToColumn(int column, TimelineCard* shape);

	bool CalculateCellDrag(wxPoint& pos);

	virtual void DrawBackground(wxDC& dc, bool fromPaint);
	virtual void DrawForeground(wxDC& dc, bool fromPaint);

	virtual void OnUpdateVirtualSize(wxRect& rect);

	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnLeftUp(wxMouseEvent& event);

private:
	void RepositionThreads();
};

#endif