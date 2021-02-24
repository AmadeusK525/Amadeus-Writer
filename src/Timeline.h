#ifndef TIMELINE_H_
#define TIMELINE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include <wx\splitter.h>
#include <wx\notebook.h>
#include <wx\timer.h>

#include "TimelineShapes.h"
#include "ProjectManager.h"

using std::pair;


///////////////////////////////////////////////////////////////
//////////////////////// TimelineThread ///////////////////////
///////////////////////////////////////////////////////////////


class TimelineThread {
private:
	wxSFShapeCanvas* m_canvas = nullptr;
	wxRect m_boundingRect{ -1,-1,-1,-1 };

	int m_y = -1;
	wxColour m_colour{ 0,0,0 };

	static wxPen m_hoverPen;
	static wxPen m_selectedPen;

	wxString m_character{ "" };

	static int m_height, m_width;
	static int m_titleOffset;

	bool m_isHovering = false, m_isSelected = false, m_drawSelected = false;

public:
	TimelineThread() = default;
	TimelineThread(int y, wxColour& colour, wxSFShapeCanvas* canvas) :
		m_y(y), m_colour(colour), m_canvas(canvas) {}

	void Draw(wxDC& dc);
	void DrawNormal(wxDC& dc);
	void DrawHover(wxDC& dc);
	void DrawSelected(wxDC& dc);

	inline bool Contains(wxPoint& pos) { return m_boundingRect.Contains(pos); }
	void Refresh(bool delayed = true);

	inline int GetY() { return m_y; }
	inline void SetY(int y) { m_y = y; }

	inline static int GetHeight() { return m_height; }
	inline static int GetWidth() { return m_width; }
	inline static void SetHeight(int height) { m_height = height; }
	inline static void SetWidth(int width) { m_width = width; }

	inline wxColour& GetColour() { return m_colour; }
	inline void SetColour(wxColour& colour) { m_colour = colour; }

	inline wxString& GetCharacter() { return m_character; }

	inline wxRect& GetRect() { return m_boundingRect; }
	inline void SetRect(wxRect& rect) { m_boundingRect = rect; }

	inline static int GetTitleOffset() { return m_titleOffset; }

	void OnLeftDown(const wxPoint& pos);
	void KillFocus();
	void SetDrawSelected(bool draw) { m_drawSelected = draw; }
	bool GetDrawSelected() { return m_drawSelected; }

	void OnMouseMove(const wxPoint& pos);
	void OnMouseLeave();
};


///////////////////////////////////////////////////////////////
//////////////////////// TimelineSection //////////////////////
///////////////////////////////////////////////////////////////


class TimelineSection {
private:
	wxSFShapeCanvas* m_canvas = nullptr;

	wxString m_title{ "Section" };
	wxString m_titleToDraw{ "Section" };
	wxFont m_titleFont{ wxFontInfo(45).Bold().AntiAliased() };

	int m_pos = -1;
	int m_first = -1, m_last = -1;

	bool m_isEmpty = true;

	static int m_markerWidth, m_horSpacing;
	static int m_titleOffset;

	int m_bottom1, m_bottom2;

	wxRect m_insideRect{ -1,0,-1,-1 };
	wxVector<int> m_separators{};
	int m_separatorY;

public:
	TimelineSection(int pos, int first, int last, wxSFShapeCanvas* canvas, const wxString& title = wxEmptyString) {
		m_pos = pos;

		m_first = first;
		m_last = last;

		m_canvas = canvas;

		m_separatorY = GetTitleOffset();

		if (title == wxEmptyString)
			m_title = "Section " + std::to_string(pos + 1) + " - Very, very big title";
		else
			m_title = title;

		m_titleToDraw = m_title;
		RecalculatePosition();
	}

	void AppendCard();
	void PrependCard();
	void RemoveCardFromEnd();
	void RemoveCardFromBeggining();

	void ShiftLeft();
	void ShiftRight();

	inline int GetFirst() { return m_first; }
	inline int GetLast() { return m_last; }
	inline int GetPos() { return m_pos; }

	inline static int GetMarkerWidth() { return m_markerWidth; }
	inline static int GetHorizontalSpcaing() { return m_horSpacing; }
	inline static int GetTitleOffset() { return m_titleOffset; }

	inline wxRect& GetRect() { return m_insideRect; }
	inline void SetHeight(int height) { m_insideRect.height = height; }

	wxPoint GetCellInPosition(wxPoint& pos);

	void DrawNormal(wxDC& dc, int virtualHeight, bool drawSeparators);

	bool Contains(wxPoint& pos) { return m_insideRect.Contains(pos); }
	bool EmptyContains(wxPoint& pos);

	void CalculateTitleWrap();
	void RecalculatePosition();
};


///////////////////////////////////////////////////////////////
//////////////////////// TimelineCanvas ///////////////////////
///////////////////////////////////////////////////////////////


class TimelineCanvas : public wxSFShapeCanvas {
private:
	wxVector<TimelineThread> m_threads{};
	wxVector<TimelineSection> m_sections{};

	pair<wxPoint, wxRect> m_curDragCell{ wxPoint(-1, -1), wxRect(-1,-1, 300, 200) };
	int m_curDragSection = 0;
	bool m_isDragOnEmptySection = false;

	bool m_drawSeparators = true;
	bool m_propagateColour = true;

	TimelineThread* m_threadUnderMouse = nullptr;
	TimelineThread* m_selectedThread = nullptr;

	wxTimer m_threadSelectionTimer{};

public:
	TimelineCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL | wxVSCROLL);

	void InitSidebar();

	void AddThread(int pos, wxColour& colour, bool refresh = true);
	TimelineCard* AddCard(int row, int col, int section);

	void AppendSection();

	pair<int, int> SetCardToSection(int section, TimelineCard* shape);
	bool SetCardToColumn(int column, TimelineCard* shape);
	bool SetCardToRow(int row, TimelineCard* shape);
	
	bool CalculateCellDrag(wxPoint& pos);

	wxColour GetThreadColour(int thread);
	int GetBottom();

	void OnThreadSelected(TimelineThread* thread);
	void OnThreadUnselected(TimelineThread* thread);
	void OnThreadSelectionTimer(wxTimerEvent& event);

	virtual void DrawBackground(wxDC& dc, bool fromPaint);
	virtual void DrawForeground(wxDC& dc, bool fromPaint);

	virtual void OnUpdateVirtualSize(wxRect& rect);

	virtual void OnMouseMove(wxMouseEvent& event);

	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnLeftUp(wxMouseEvent& event);
	virtual void OnLeftDoubleClick(wxMouseEvent& event);

	virtual void OnKeyDown(wxKeyEvent& event);

	virtual void OnTextChange(wxSFEditTextShape* shape);

	void OnScroll(wxScrollWinEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void RepositionThreads();
};


////////////////////////////////////////////////////////////////////
//////////////////////////// Timeline //////////////////////////////
////////////////////////////////////////////////////////////////////


class amTimeline : public wxSplitterWindow {
private:
	amProjectManager* m_manager = nullptr;
	amOutline* m_parent = nullptr;

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

	void Save();
	void Load(wxStringInputStream& stream);

	void SaveThreads(wxStringOutputStream& stream);
	void LoadThreads(wxStringInputStream& stream);
};
#endif