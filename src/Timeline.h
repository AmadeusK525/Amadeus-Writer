#ifndef TIMELINE_H_
#define TIMELINE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include <wx\splitter.h>
#include <wx\notebook.h>

#include "TimelineShapes.h"

using std::pair;


///////////////////////////////////////////////////////////////
//////////////////////// TimelineThread ///////////////////////
///////////////////////////////////////////////////////////////


struct TimelineThread {
	wxRect rect{ -1,-1,-1,-1 };

	wxVector<pair<int, wxColour>> stats;
 
	TimelineThread() = default;
	TimelineThread(int y, wxColour& colour) {
		stats.push_back(pair<int, wxColour>(y, colour));
	};

	void Draw(wxDC& dc, int width);
};


///////////////////////////////////////////////////////////////
//////////////////////// TimelineCanvas ///////////////////////
///////////////////////////////////////////////////////////////


class TimelineCanvas : public wxSFShapeCanvas {
private:
	wxVector<TimelineThread> m_threads{};
	wxVector<int> m_separators;

	std::pair<wxPoint, wxRect> m_curDragCell{ wxPoint(-1, -1), wxRect(-1,-1, 300, 200) };

	bool m_drawSeparators = true;
	bool m_propagateColour = true;

	wxBitmap m_dragShapeBmp{};

	wxRealPoint m_cacheZoomPoint{ -1,-1 };
	wxTimer m_zoomTimer{};

public:
	TimelineCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL | wxVSCROLL);

	void InitSidebar();

	void AddThread(int pos, wxColour& colour, bool refresh = true);
	TimelineCard* AddCard(int row, int col, bool recalcPos = true);

	void SetCardToColumn(int column, TimelineCard* shape);
	
	bool CalculateCellDrag(wxPoint& pos);

	wxColour GetThreadColour(int thread);

	virtual void DrawBackground(wxDC& dc, bool fromPaint);
	virtual void DrawForeground(wxDC& dc, bool fromPaint);

	virtual void OnUpdateVirtualSize(wxRect& rect);

	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseWheel(wxMouseEvent& event);

	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnLeftUp(wxMouseEvent& event);
	virtual void OnLeftDoubleClick(wxMouseEvent& event);

	virtual void OnKeyDown(wxKeyEvent& event);

	void OnScroll(wxScrollWinEvent& event);
	void OnTimer(wxTimerEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void RepositionThreads();
};


////////////////////////////////////////////////////////////////////
//////////////////////////// Timeline //////////////////////////////
////////////////////////////////////////////////////////////////////


class amTimeline : public wxSplitterWindow {
private:
	TimelineCanvas* m_canvas = nullptr;
	wxSFDiagramManager m_canvasManager;

	wxPanel* m_sideHolder = nullptr;
	wxNotebook* m_sidebar = nullptr;

	wxRect m_pullRect{ 5, 5, 30, 30 };

	bool m_isSidebarShown = false;

public:
	amTimeline(wxWindow* parent);

	void ShowSidebar(bool show);

	virtual void OnDoubleClickSash(int x, int y) {}

	void OnSidebarMouseMove(wxMouseEvent& event);
	void OnSidebarLeftDown(wxMouseEvent& event);

	void OnSidebarPaint(wxPaintEvent& event);
};


#endif