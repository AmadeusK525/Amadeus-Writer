#include "Timeline.h"

TimelineCanvas::TimelineCanvas(wxSFDiagramManager* manager, wxWindow* parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
	wxSFShapeCanvas(manager, parent, id, pos, size, style) {

	RemoveStyle(sfsGRID_SHOW);
	RemoveStyle(sfsGRID_USE);
	RemoveStyle(sfsMULTI_SIZE_CHANGE);

	AddStyle(sfsGRADIENT_BACKGROUND);
	SetGradientFrom(wxColour(255, 250, 220));
	SetGradientTo(wxColour(240, 150, 70));

	AddStyle(sfsPRINT_BACKGROUND);

	SetScrollRate(15, 15);

	AddStyle(sfsPROCESS_MOUSEWHEEL);
	SetMinScale(0.1);
	SetMaxScale(2);
	SetScale(0.75);

	GetDiagramManager()->ClearAcceptedShapes();
	GetDiagramManager()->AcceptShape("TimelineCard");

	wxSFShapeCanvas::EnableGC(true);

	TimelineCard* shape = nullptr;
	TimelineCard* first = nullptr;
	for (int i = 0; i < 6; i++) {
		AddThread(i, wxColour(rand() % 200, rand() % 200, rand() % 200), false);
		shape = AddCard(i, i);

		if (i == 1)
			first = shape;
	}
	
	RepositionThreads();

	SetCardToColumn(2, shape);
	SetCardToColumn(3, first);

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

	ShapeList temp;
	GetDiagramManager()->GetShapes(nullptr, temp);

	m_separators.push_back(460 * temp.GetCount());
	return shape;
}

void TimelineCanvas::RepositionThreads() {
	ShapeList temp;
	GetDiagramManager()->GetShapes(nullptr, temp);

	int width = m_separators.back();

	for (int i = 0; i < m_threads.size(); i++) {
			m_threads[i].y = (310 * i) + 165;
			m_threads[i].rect = wxRect(0, m_threads[i].y - 150, width, 325);
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
			int xCell = (pos.x / 460);

			if (m_curDragCell.first.y != i) {
				m_curDragCell.first.y = i;
				m_curDragCell.second.y = (30 * (i + 1)) + (280 * i);
				changed = true;
			}

			if (m_curDragCell.first.x != xCell) {
				m_curDragCell.first.x = xCell;
				m_curDragCell.second.x = (460 * xCell) + 30;
				changed = true;
			}

			break;
		}

	return changed;
}

void TimelineCanvas::DrawBackground(wxDC& dc, bool fromPaint) {
	wxSFShapeCanvas::DrawBackground(dc, fromPaint);
	wxSize size = GetVirtualSize();
	double curScale = GetScale();

	for (auto& it : m_threads) {
		dc.SetPen(wxPen(it.colour));
		dc.SetBrush(wxBrush(it.colour));
		dc.DrawRectangle(wxPoint(0, it.y), wxSize(size.x / curScale, 10));
	}
}

void TimelineCanvas::DrawForeground(wxDC& dc, bool fromPaint) {
	if (m_drawSeparators) {
		int height = m_threads.back().y + 145;

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
		}
	}

	m_curDragCell.first = wxPoint(-1, -1);
	m_curDragCell.second.SetTopLeft(m_curDragCell.first);

	wxSFShapeCanvas::OnLeftUp(event);
}