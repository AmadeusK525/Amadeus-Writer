#include "Timeline.h"


///////////////////////////////////////////////////////////////
//////////////////////// TimelineThread ///////////////////////
///////////////////////////////////////////////////////////////


void TimelineThread::Draw(wxDC& dc, int width) {
	for (auto& it : stats) {
		dc.SetPen(wxPen(it.second));
		dc.SetBrush(wxBrush(it.second));

		dc.DrawRectangle(wxPoint(0, it.first), wxSize(width, 10));
	}
}


///////////////////////////////////////////////////////////////
//////////////////////// TimelineCanvas ///////////////////////
///////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(TimelineCanvas, wxSFShapeCanvas)
EVT_SCROLLWIN(TimelineCanvas::OnScroll)
END_EVENT_TABLE()

TimelineCanvas::TimelineCanvas(wxSFDiagramManager* manager, wxWindow* parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
	wxSFShapeCanvas(manager, parent, id, pos, size, style) {

	RemoveStyle(sfsGRID_SHOW);
	RemoveStyle(sfsGRID_USE);
	RemoveStyle(sfsMULTI_SELECTION);
	RemoveStyle(sfsMULTI_SIZE_CHANGE);

	AddStyle(sfsGRADIENT_BACKGROUND);
	SetGradientFrom(wxColour(255, 250, 220));
	SetGradientTo(wxColour(240, 150, 70));

	AddStyle(sfsPRINT_BACKGROUND);

	AddStyle(sfsPROCESS_MOUSEWHEEL);
	SetMinScale(0.1);
	SetMaxScale(2);
	SetScale(0.75);

	SetScrollRate(25, 25);

	GetDiagramManager()->ClearAcceptedShapes();
	GetDiagramManager()->AcceptShape("TimelineCard");

	wxSFShapeCanvas::EnableGC(true);
	m_zoomTimer.Bind(wxEVT_TIMER, &TimelineCanvas::OnTimer, this);

	TimelineCard* shape = nullptr;
	TimelineCard* first = nullptr;
	for (int i = 0; i < 6; i++) {
		AddThread(i, wxColour(rand() % 255, rand() % 255, rand() % 255), false);
		shape = AddCard(i, i);

		if (i == 1)
			first = shape;
	}

	/*for (int i = 6; i < 56; i++) {
		shape = AddCard(rand() % 6, i);
	}*/
	
	RepositionThreads();

	UpdateVirtualSize();
	Refresh(true);
}

void TimelineCanvas::AddThread(int pos, wxColour& colour, bool refresh) {
	if (pos >= m_threads.size())
		m_threads.push_back(TimelineThread(-1, colour));
	else {
		int i = 0;
		for (auto& it : m_threads) {
			if (i++ == pos)
				m_threads.insert(&it, TimelineThread(-1, colour));
		}
	}

	if (refresh) {
		RepositionThreads();
		Refresh(true);
	}
}

TimelineCard* TimelineCanvas::AddCard(int row, int col, bool recalcPos) {
	TimelineCard* shape = (TimelineCard*)GetDiagramManager()->AddShape(CLASSINFO(TimelineCard), wxPoint());
	shape->SetRow(row);

	if (recalcPos)
		shape->RecalculatePosition();

	SetCardToColumn(col, shape);

	if (m_propagateColour)
		shape->SetColour(GetThreadColour(row));
	
	ShapeList temp;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), temp);

	m_separators.push_back(340 * temp.GetCount());
	SaveCanvasState();
	return shape;
}

void TimelineCanvas::RepositionThreads() {
	ShapeList temp;
	GetDiagramManager()->GetShapes(nullptr, temp);

	int width = m_separators.back();

	for (int i = 0; i < m_threads.size(); i++) {
		switch (m_threads[i].stats.size()) {
		case 1:
			m_threads[i].stats[0].first = (220 * i) + 115;
			m_threads[i].rect = wxRect(0, m_threads[i].stats[0].first - 105, width, 220);
			break;

		case 2:
			m_threads[i].stats[0].first = (220 * i) + 110;
			m_threads[i].stats[1].first = (220 * i) + 120;
			m_threads[i].rect = wxRect(0, m_threads[i].stats[0].first - 100, width, 220);
			break;

		case 3:
			for (int j = 0; j < m_threads[i].stats.size(); j++)
				m_threads[j].stats[j].first = (220 * i) + 105 + (j * 10);

			m_threads[i].rect = wxRect(0, m_threads[i].stats[1].first - 100, width, 220);
			break;

		default:
			continue;
		}
	}
}

void TimelineCanvas::SetCardToColumn(int column, TimelineCard* shape) {
	ShapeList list;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), list);

	TimelineCard* card = (TimelineCard*)shape;
	int prevCol = card->GetColumn();

	if (prevCol == -1)
		prevCol = 1000;

	if (prevCol > column) {
		for (int i = 0; i < list.GetCount(); i++) {
			card = (TimelineCard*)list[i];

			if (card == shape)
				card->SetColumn(column);
			else if (card->GetColumn() >= column && card->GetColumn() < prevCol)
				card->IncrementColumn();
			else
				continue;

			card->RecalculatePosition();
		}
	} else if (prevCol < column) {
		for (int i = 0; i < list.GetCount(); i++) {
			card = (TimelineCard*)list[i];

			if (card == shape)
				card->SetColumn(column);
			else if (card->GetColumn() > prevCol && card->GetColumn() <= column)
				card->DecrementColumn();
			else
				continue;

			card->RecalculatePosition();
		}
	} else
		card->RecalculatePosition();
}

bool TimelineCanvas::CalculateCellDrag(wxPoint& pos) {
	bool changed = false;

	for (int i = 0; i < m_threads.size(); i++)
		if (m_threads[i].rect.Contains(pos)) {
			int xCell = (pos.x / 340);

			if (m_curDragCell.first.y != i) {
				m_curDragCell.first.y = i;
				m_curDragCell.second.y = (20 * (i + 1)) + (200 * i);
				changed = true;
			}

			if (m_curDragCell.first.x != xCell) {
				m_curDragCell.first.x = xCell;
				m_curDragCell.second.x = (340 * xCell) + 20;
				changed = true;
			}

			break;
		}

	return changed;
}

wxColour TimelineCanvas::GetThreadColour(int thread) {
	return m_threads[thread].stats[0].second;
}

void TimelineCanvas::DrawBackground(wxDC& dc, bool fromPaint) {
	wxSFShapeCanvas::DrawBackground(dc, fromPaint);

	int threadWidth = GetVirtualSize().x / GetScale();

	for (auto& it : m_threads)
		it.Draw(dc, threadWidth);
}

void TimelineCanvas::DrawForeground(wxDC& dc, bool fromPaint) {
	if (m_drawSeparators) {
		int height = m_threads.size() * 220;

		dc.SetPen(wxPen(wxColour(0, 0, 0, 128), 2, wxPENSTYLE_LONG_DASH));

		for (auto& it : m_separators)
			dc.DrawLine(wxPoint(it, 0), wxPoint(it, height));
	}

	dc.SetPen(wxPen(wxColour(180, 180, 255, 128), 2));
	dc.SetBrush(wxBrush(wxColour(200, 200, 255, 128)));

	if (m_nWorkingMode == modeSHAPEMOVE && m_curDragCell.first.x != -1) {
		dc.DrawRectangle(m_curDragCell.second);
	}

	dc.SetPen(wxNullPen);
	dc.SetBrush(wxNullBrush);
}

void TimelineCanvas::OnUpdateVirtualSize(wxRect& rect) {
	rect.width += 300;
	rect.height += 300;
}

void TimelineCanvas::OnMouseMove(wxMouseEvent& event) {
	if (m_nWorkingMode == modeSHAPEMOVE) {
		if (CalculateCellDrag(DP2LP(event.GetPosition())))
			Refresh(false);
	}

	wxSFShapeCanvas::OnMouseMove(event);
}

void TimelineCanvas::OnMouseWheel(wxMouseEvent& event) {
//	Freeze();
	wxSFShapeCanvas::OnMouseWheel(event);

	/*if (event.ControlDown()) {
		wxSize szCanvas = GetVirtualSize();

		if (m_cacheZoomPoint.y == -1) {
			m_cacheZoomPoint = DP2LP(wxRealPoint((szCanvas.x/ 2), (szCanvas.y / 2)));
			m_cacheZoomPoint += wxSize(500 / m_Settings.m_nScale, 0);
		}

		int ux, uy;
		GetScrollPixelsPerUnit(&ux, &uy);

		Scroll(((m_cacheZoomPoint.x * m_Settings.m_nScale) - (double)szCanvas.x / 2) / ux,
			((m_cacheZoomPoint.y * m_Settings.m_nScale) - (double)szCanvas.y / 2) / uy);

		m_zoomTimer.StartOnce(500);
	}*/

//	Thaw();
}

void TimelineCanvas::OnLeftDown(wxMouseEvent& event) {
	CalculateCellDrag(DP2LP(event.GetPosition()));

	wxSFShapeCanvas::OnLeftDown(event);
}

void TimelineCanvas::OnLeftUp(wxMouseEvent& event) {
	if (m_nWorkingMode == modeSHAPEMOVE) {
		ShapeList sel;
		GetSelectedShapes(sel);

		TimelineCard* pSel = nullptr;
		for (int i = 0; i < sel.GetCount(); i++) {
			pSel = (TimelineCard*)sel[i];
			pSel->SetRow(m_curDragCell.first.y);
			SetCardToColumn(m_curDragCell.first.x + i, pSel);

			if (m_propagateColour)
				pSel->SetColour(GetThreadColour(m_curDragCell.first.y));
		}
	}

	m_curDragCell.first = wxPoint(-1, -1);
	m_curDragCell.second.SetTopLeft(m_curDragCell.first);

	wxSFShapeCanvas::OnLeftUp(event);
}

void TimelineCanvas::OnLeftDoubleClick(wxMouseEvent& event) {
	if (event.ControlDown()) {
		wxSFShapeBase* pShape = GetShapeUnderCursor();

		if (pShape) {
			Freeze();
			if (GetScale() >= 0.85)
				SetScale(0.3);
			else
				SetScale(0.85);

			ScrollToShape(pShape);
			Thaw();

			DeselectAll();
		}
	} else
		wxSFShapeCanvas::OnLeftDoubleClick(event);
}

void TimelineCanvas::OnKeyDown(wxKeyEvent& event) {
	if (event.ControlDown()) {
		switch (event.GetKeyCode()) {
		case 90:
		case 122:
			Undo();
			break;

		default:
			break;
		}
	}
}

void TimelineCanvas::OnScroll(wxScrollWinEvent& event) {
	if (!wxGetKeyState(WXK_CONTROL))
		event.Skip();
}

void TimelineCanvas::OnTimer(wxTimerEvent& event) {
	m_cacheZoomPoint = wxPoint(-1, -1);
}


////////////////////////////////////////////////////////////////////
//////////////////////////// Timeline //////////////////////////////
////////////////////////////////////////////////////////////////////


amTimeline::amTimeline(wxWindow* parent) : wxSplitterWindow(parent, 01, wxDefaultPosition, wxDefaultSize, 768L | wxSP_LIVE_UPDATE) {
	m_canvas = new TimelineCanvas(&m_canvasManager, this);

	m_sideHolder = new wxPanel(this);
	m_sideHolder->SetBackgroundColour(wxColour(60, 60, 60));

	m_sidebar = new wxNotebook(m_sideHolder, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_sidebar->SetBackgroundColour(wxColour(60, 60, 60));
	m_sideHolder->Bind(wxEVT_MOTION, &amTimeline::OnSidebarMouseMove, this);
	m_sideHolder->Bind(wxEVT_LEFT_DOWN, &amTimeline::OnSidebarLeftDown, this);
	m_sideHolder->Bind(wxEVT_PAINT, &amTimeline::OnSidebarPaint, this);

	wxPanel* emptyPanel = new wxPanel(m_sidebar);
	emptyPanel->SetBackgroundColour(wxColour(60, 60, 60));

	m_sidebar->AddPage(emptyPanel, _("Panel"));

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_sidebar, wxSizerFlags(1).Expand().Border(wxTOP, 40));

	m_sideHolder->SetSizer(sizer);
	m_sidebar->Hide();

	SetMinimumPaneSize(40);
	SplitVertically(m_canvas, m_sideHolder, 99999);
	SetSashGravity(1.0);
	SetSashInvisible(true);
}

void amTimeline::ShowSidebar(bool show) {
	int i = 1;
	int j = GetSashPosition();

	wxSize canvasSize = m_canvas->GetClientSize();
	m_sidebar->Show(show);

	if (show) {
		while (i <= 220) {
			SetSashPosition(j - i);
			i += 20;
		}
	} else {
		m_canvas->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_NEVER);

		while (i <= 220) {
			SetSashPosition(j + i++);
			i += 20;
		}

		m_canvas->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_DEFAULT);
	}

	m_isSidebarShown = show;

	m_sideHolder->Layout();
	m_sideHolder->Refresh();
}

void amTimeline::OnSidebarMouseMove(wxMouseEvent& event) {
	if (m_pullRect.Contains(event.GetPosition()))
		wxSetCursor(wxCURSOR_CLOSED_HAND);
	else
		wxSetCursor(wxCURSOR_DEFAULT);
}

void amTimeline::OnSidebarLeftDown(wxMouseEvent& event) {
	if (m_pullRect.Contains(event.GetPosition()))
		ShowSidebar(!m_isSidebarShown);
}

void amTimeline::OnSidebarPaint(wxPaintEvent& event) {
	wxPaintDC dc(m_sideHolder);

	dc.SetPen(wxPen(wxColour(70, 70, 70), 2));
	dc.SetBrush(wxBrush(wxColour(90, 90, 90)));

	dc.DrawRectangle(m_pullRect);

	dc.SetPen(wxPen(wxColour(130, 130, 130), 3));

	int x1 = m_pullRect.x, x2 = m_pullRect.x + m_pullRect.width - 2;
	int y = m_pullRect.height / 4 + 2;

	dc.DrawLine(wxPoint(x1, y), wxPoint(x2, y));
	dc.DrawLine(wxPoint(x1, y * 2), wxPoint(x2, y * 2));
	dc.DrawLine(wxPoint(x1, y * 3), wxPoint(x2, y * 3));
}