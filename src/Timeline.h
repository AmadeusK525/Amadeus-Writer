#ifndef TIMELINE_H_
#define TIMELINE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include <wx\notebook.h>
#include <wx\timer.h>

#include "TimelineShapes.h"
#include "ProjectManager.h"
#include "amUtility.h"

using std::pair;

class amTLThread;
class amTLSection;
class amTLTimelineCanvas;
class amTLTimeline;
class amTLTimelineSidebar;
class amTLThreadThumbnail;
class amTLCardThumbnail;
class amTLSectionThumbnail;


/////////////////////////////////////////////////////////////////////
/////////////////////////// TimelineThread //////////////////////////
/////////////////////////////////////////////////////////////////////


class amTLThread : public xsSerializable {
private:
	amTLTimelineCanvas* m_canvas = nullptr;
	wxRect m_boundingRect{ -1,-1,-1,-1 };
	wxFont m_titleFont{ wxFontInfo(15).Bold().AntiAliased() };

	int m_y = -1;
	int m_index = -1;
	wxColour m_colour{ 0,0,0 };

	static wxPen m_hoverPen;
	static wxPen m_selectedPen;

	wxString m_character{ "" };
	wxString m_titleToDraw{ "" };
	int m_titleHeight = -1;

	static int m_height, m_width;
	static int m_titleOffset;

	bool m_isHovering = false, m_isSelected = false, m_drawSelected = false;

public:
	amTLThread() = default;
	amTLThread(const wxString& character, int index, int y, const wxColour& colour, amTLTimelineCanvas* canvas) :
		m_character(character), m_index(index), m_y(y), m_colour(colour), m_canvas(canvas) {
		MarkSerializableDataMembers();
		CalculateTitleWrap();
	}

	void Draw(wxDC& dc);
	void DrawNormal(wxDC& dc);
	void DrawHover(wxDC& dc);
	void DrawSelected(wxDC& dc);

	void DrawTitle(wxDC& dc);

	inline bool Contains(wxPoint& pos) {
		m_boundingRect.width = m_width;
		return m_boundingRect.Contains(pos);
	}

	void Refresh(bool delayed = true);

	inline int GetY() { return m_y; }
	inline void SetY(int y) { m_y = y; }

	inline int GetIndex() { return m_index; }
	inline void SetIndex(int index) { m_index = index; }

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

	void CalculateTitleWrap();

	inline void MarkSerializableDataMembers() {
		XS_SERIALIZE_INT(m_y, "y");
		XS_SERIALIZE_INT(m_index, "index");
		XS_SERIALIZE_INT(m_height, "height");
		XS_SERIALIZE_INT(m_width, "width");
		XS_SERIALIZE_INT(m_titleOffset, "titleOffset");
	}
};


///////////////////////////////////////////////////////////////////////
//////////////////////////// TimelineSection //////////////////////////
///////////////////////////////////////////////////////////////////////


class amTLSection : public xsSerializable {
private:
	amTLTimelineCanvas* m_canvas = nullptr;

	wxString m_title{ "Section" };
	wxString m_titleToDraw{ "Section" };
	wxFont m_titleFont{ wxFontInfo(45).Bold().AntiAliased() };

	wxColour m_colour{ 255,255,255 };

	static wxPen m_border;
	static wxPen m_separatorPen;

	int m_titleHeight = -1;

	int m_index = -1;
	int m_first = -1, m_last = -1;

	bool m_isEmpty = true;

	static int m_markerWidth, m_horSpacing;
	static int m_titleOffset;

	wxRect m_insideRect{ -1,0,-1,-1 };
	wxVector<int> m_separators{};
	int m_separatorY;

public:
	inline amTLSection(int index, int first, int last, amTLTimelineCanvas* canvas,
		const wxString& title = wxEmptyString, const wxColour& colour = { 255,255,255 }) {
		m_index = index;

		m_first = first;
		m_last = last;

		m_canvas = canvas;

		m_separatorY = GetTitleOffset();

		if (title == wxEmptyString)
			m_title = "Section " + std::to_string(index + 1);
		else
			m_title = title;

		m_colour = colour;
		m_titleToDraw = m_title;
		RecalculatePosition();
		MarkSerializableDataMembers();
	}

	inline const wxString& GetTitle() const { return m_title; }

	void AppendCard();
	void PrependCard();
	void RemoveCardFromEnd();
	void RemoveCardFromBeggining();

	void ShiftLeft();
	void ShiftRight();

	inline int GetFirst() { return m_first; }
	inline int GetLast() { return m_last; }
	inline int GetIndex() { return m_index; }

	inline int SetIndex(int i) { m_index = i; }

	inline static int GetMarkerWidth() { return m_markerWidth; }
	inline static int GetHorizontalSpcaing() { return m_horSpacing; }
	inline static int GetTitleOffset() { return m_titleOffset; }

	inline wxRect& GetRect() { return m_insideRect; }
	inline int GetRight() { return m_insideRect.GetRight() + m_markerWidth; }
	inline void SetHeight(int height) { m_insideRect.height = height; }

	wxPoint GetCellInPosition(wxPoint& pos);

	void DrawNormal(wxDC& dc, bool drawSeparators);

	bool Contains(wxPoint& pos) { return m_insideRect.Contains(pos); }
	bool EmptyContains(wxPoint& pos);

	void CalculateTitleWrap();
	void RecalculatePosition();

	inline void MarkSerializableDataMembers() {
		XS_SERIALIZE_STRING_EX(m_title, "title", "");
		XS_SERIALIZE_STRING_EX(m_titleToDraw, "titleToDraw", "");
		XS_SERIALIZE_INT_EX(m_titleHeight, "titleHeight", -1);
		XS_SERIALIZE_INT_EX(m_index, "index", -1);
		XS_SERIALIZE_INT_EX(m_first, "first", -1);
		XS_SERIALIZE_INT_EX(m_last, "last", -1);
		XS_SERIALIZE_COLOUR_EX(m_colour, "colour", wxColour(255, 255, 255));
		XS_SERIALIZE_BOOL_EX(m_isEmpty, "isEmpty", false);
		XS_SERIALIZE_INT_EX(m_markerWidth, "markerWidth", -1);
		XS_SERIALIZE_INT_EX(m_horSpacing, "horSpacing", -1);
		XS_SERIALIZE_INT_EX(m_titleOffset, "titleOffset", -1);
	}
};


///////////////////////////////////////////////////////////////////////
//////////////////////////// TimelineCanvas ///////////////////////////
///////////////////////////////////////////////////////////////////////


class amTLTimelineCanvas : public amSFShapeCanvas {
private:
	amTLTimeline* m_parent = nullptr;

	wxVector<amTLThread*> m_threads{};
	wxVector<amTLSection*> m_sections{};

	pair<wxPoint, wxRect> m_curDragCell{ wxPoint(-1, -1), wxRect(-1,-1, 300, 200) };
	int m_curDragSection = 0;
	bool m_isDragOnEmptySection = false;

	bool m_drawSeparators = true;
	bool m_propagateColour = true;

	amTLThread* m_threadUnderMouse = nullptr;
	amTLThread* m_selectedThread = nullptr;

	amTLSection* m_selectedSection = nullptr;

	bool m_beginDraggingRight = false;
	bool m_isDraggingRight = false;
	wxPoint m_rightDownPos{};
	wxPoint m_scrollbarPos{};

	wxTimer m_threadSelectionTimer{};

public:
	amTLTimelineCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL | wxVSCROLL);
	virtual ~amTLTimelineCanvas();
	void CleanUp();

	inline bool CanAddCards() { return !m_threads.empty() && !m_sections.empty(); }

	void AddThread(int pos, const wxString& character, const wxColour& colour, bool refresh = true);
	inline void AppendThread(const wxString& character, const wxColour& colour, bool refresh = true) {
		AddThread(m_threads.size(), character, colour, refresh);
	}
	TimelineCard* AddCard(int rowIndex, int colIndex, int sectionIndex);
	TimelineCard* AppendCard(int rowIndex);

	void AppendSection(const wxString& title, const wxColour& colour);

	pair<int, int> SetCardToSection(int section, TimelineCard* shape);
	bool SetCardToColumn(int column, TimelineCard* shape);
	bool SetCardToRow(int row, TimelineCard* shape);
	
	bool CalculateCellDrag(wxPoint& pos);

	inline wxVector<amTLThread*>& GetThreads() { return m_threads; }
	wxColour GetThreadColour(int thread);
	int GetBottom();

	void OnThreadSelected(amTLThread* thread);
	void OnThreadUnselected(amTLThread* thread);
	void OnThreadSelectionTimer(wxTimerEvent& event);

	void UpdateSelectedThreadSBData();
	void UpdateSelectedSectionSBData();

	amTLThread* GetSelectedThread() { return m_selectedThread; }
	amTLSection* GetSection(int index) { return m_sections[index]; }

	virtual void DrawBackground(wxDC& dc, bool fromPaint);
	virtual void DrawForeground(wxDC& dc, bool fromPaint);

	virtual void OnUpdateVirtualSize(wxRect& rect);

	virtual void OnMouseMove(wxMouseEvent& event);

	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnLeftUp(wxMouseEvent& event);
	virtual void OnLeftDoubleClick(wxMouseEvent& event);

	virtual void OnKeyDown(wxKeyEvent& event);
	virtual void OnTextChange(wxSFEditTextShape* shape);

	void OnLeaveWindow(wxMouseEvent& event);

	wxSize GetGoodSize();

private:
	void RepositionThreads();
};


////////////////////////////////////////////////////////////////////
//////////////////////////// Timeline /////////////////////////////
////////////////////////////////////////////////////////////////////


class amTLTimeline : public amSplitterWindow {
private:
	amProjectManager* m_manager = nullptr;
	amOutline* m_parent = nullptr;

	amTLTimelineCanvas* m_canvas = nullptr;
	wxSFDiagramManager m_canvasManager;

	amTLTimelineSidebar* m_sidebar = nullptr;

	bool m_isSidebarShown = false;

public:
	amTLTimeline(wxWindow* parent);

	bool IsCharacterPresent(const wxString& character);
	inline bool CanAddCards() { return m_canvas->CanAddCards(); }

	void AppendThread(const wxString& character, const wxColour& colour);
	void AppendSection(const wxString& title, const wxColour& colour);

	void AddCardToThread(amTLThread* thread);
	void AddCardToSection(amTLSection* section);

	void AddCardBefore(TimelineCard* card);
	void AddCardAfter(TimelineCard* card);

	void ShowSidebar();

	void SetThreadData(amTLThread* thread, ShapeList& shapes);
	void SetSectionData(amTLSection* section, ShapeList& shapes);
	void SetCardData(TimelineCard* card);

	void Save();
	void Load(wxStringInputStream& stream);

	void SaveThreads(wxStringOutputStream& stream);
	void LoadThreads(wxStringInputStream& stream);
};


////////////////////////////////////////////////////////////////////
///////////////////////// Timeline Sidebar /////////////////////////
////////////////////////////////////////////////////////////////////


enum {
	BUTTON_AddThread,
	BUTTON_AddSection,
	BUTTON_AddCardToThread,

	BUTTON_AddCardBefore,
	BUTTON_AddCardAfter,

	BUTTON_AddCardToSection
};


class amTLTimelineSidebar : public wxPanel {
	amTLTimeline* m_parent = nullptr;

	wxNotebook* m_content = nullptr;
	
	wxScrolledWindow* m_threadPanel = nullptr;
	amTLThreadThumbnail* m_threadThumbnail = nullptr;
	wxStaticText* m_curThreadName = nullptr;

	wxScrolledWindow* m_cardPanel = nullptr;
	amTLCardThumbnail* m_cardThumbnail = nullptr;

	wxScrolledWindow* m_sectionPanel = nullptr;
	amTLSectionThumbnail* m_sectionThumbnail = nullptr;
	wxStaticText* m_curSectionName = nullptr;

	wxRect m_pullRect{ 5, 5, 30, 30 };

public:
	amTLTimelineSidebar(wxWindow* parent, amTLTimelineCanvas* canvas);

	void ShowContent(bool show = true);

	void OnAddThread(wxCommandEvent& event);
	void OnAddSection(wxCommandEvent& event);
	void OnAddCardToThread(wxCommandEvent& event);
	void OnAddCardToSection(wxCommandEvent& event);

	void OnAddCardBefore(wxCommandEvent& event);
	void OnAddCardAfter(wxCommandEvent& event);

	void SetThreadData(amTLThread* thread, ShapeList& shapes);
	void SetSectionData(amTLSection* section, ShapeList& shapes);
	void SetCardData(TimelineCard* card);

	void OnPaint(wxPaintEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnLeftDown(wxMouseEvent& event);

	DECLARE_EVENT_TABLE();
};


////////////////////////////////////////////////////////////////////
////////////////////////// ThreadThumbnails ////////////////////////
////////////////////////////////////////////////////////////////////


class amTLThumbnail : public wxSFThumbnail {
protected:
	ShapeList m_shapes;

public:
	amTLThumbnail(wxWindow* parent);

	void OnPaint(wxPaintEvent& event);
};
 
class amTLThreadThumbnail : public amTLThumbnail {
private: 
	amTLThread* m_thread = nullptr;

public:
	amTLThreadThumbnail(wxWindow* parent);
	
	virtual void DrawContent(wxDC& dc);

	void SetData(amTLThread* thread, ShapeList& shapes);
	inline amTLThread* GetThread() { return m_thread; }

};

class amTLCardThumbnail : public wxSFThumbnail {
private:
	TimelineCard* m_card = nullptr;

public:
	amTLCardThumbnail(wxWindow* parent);

	inline void SetData(TimelineCard* card) { m_card = card; }
	inline TimelineCard* GetCard() { return m_card; }

	void OnPaint(wxPaintEvent& event);
	virtual void DrawContent(wxDC& dc);
};

class amTLSectionThumbnail : public amTLThumbnail {
private:
	amTLSection* m_section = nullptr;

public:
	amTLSectionThumbnail(wxWindow* parent);

	void SetData(amTLSection* section, ShapeList& shapes);
	inline amTLSection* GetSection() { return m_section; }

	virtual void DrawContent(wxDC& dc);
};

#endif