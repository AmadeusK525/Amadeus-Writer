#include "Timeline.h"
#include "Document.h"
#include "MyApp.h"

#include <wx\sstream.h>

///////////////////////////////////////////////////////////////
//////////////////////// TimelineThread ///////////////////////
///////////////////////////////////////////////////////////////

int TimelineThread::m_height = 10;

void TimelineThread::Draw(wxDC& dc, int width) {
	dc.SetPen(wxPen(m_colour));
	dc.SetBrush(wxBrush(m_colour));

	dc.DrawRectangle(wxPoint(0, m_y), wxSize(width, m_height));
}


///////////////////////////////////////////////////////////////
//////////////////////// TimelineSection //////////////////////
///////////////////////////////////////////////////////////////


int TimelineSection::m_horSpacing = 400;
int TimelineSection::m_markerWidth = 20;
int TimelineSection::m_titleOffset = 100;

void TimelineSection::AppendCard() {
	m_last++;

	if (m_last - m_first >= 2)
		m_separators.push_back(m_insideRect.x + m_insideRect.width);

	m_insideRect.width += 340;
}

void TimelineSection::PrependCard() {
	m_first--;

	if (m_last - m_first >= 2)
		m_separators.insert(m_separators.begin(), m_insideRect.x);

	m_insideRect.x -= TimelineCard::GetWidth() + TimelineCard::GetHorizontalSpacing();
}

void TimelineSection::RemoveCardFromEnd() {
	m_last--;

	if (!m_separators.empty())
		m_separators.pop_back();

	m_insideRect.width -= TimelineCard::GetWidth() + TimelineCard::GetHorizontalSpacing();
}

void TimelineSection::RemoveCardFromBeggining() {
	m_first++;

	if (!m_separators.empty())
		m_separators.erase(m_separators.begin());

	m_insideRect.x += TimelineCard::GetWidth() + TimelineCard::GetHorizontalSpacing();
}

void TimelineSection::ShiftLeft() {
	PrependCard();
	RemoveCardFromEnd();
	RecalculatePosition();
}

void TimelineSection::ShiftRight() {
	AppendCard();
	RemoveCardFromBeggining();
	RecalculatePosition();
}

wxPoint TimelineSection::GetCellInPosition(wxPoint& pos) {
	int xCell = ((pos.x - m_insideRect.x) /
		(TimelineCard::GetHorizontalSpacing() + TimelineCard::GetWidth())) + m_first;

	int yCell = ((pos.y - (TimelineCard::GetVerticalSpacing() / 2)) / (TimelineCard::GetVerticalSpacing() + TimelineCard::GetHeight()));

	return wxPoint(xCell, yCell);
}

void TimelineSection::Draw(wxDC& dc, int virtualHeight, bool drawSeparators) {
	dc.SetPen(wxPen(wxColour(255, 255, 255), 2));
	dc.SetBrush(wxBrush(wxColour(0,0,0)));

	dc.DrawRectangle(m_insideRect.GetTopLeft() - wxPoint(m_markerWidth, 0), wxSize(m_markerWidth, m_insideRect.height + 10));
	dc.DrawRectangle(m_insideRect.GetTopRight(), wxSize(m_markerWidth, m_insideRect.height + 10));

	dc.SetPen(wxPen(wxColour(0, 0, 0, 128), 2, wxPENSTYLE_LONG_DASH));
	
	if (drawSeparators) {
		for (auto& it : m_separators) {
			dc.DrawLine(wxPoint(it, 0), wxPoint(it, m_insideRect.height));
		}
	}

	dc.DrawLine(m_insideRect.GetBottomLeft(), m_insideRect.GetBottomRight());
	dc.DrawLine(m_insideRect.GetBottomLeft() + wxPoint(0, 10), m_insideRect.GetBottomRight() + wxPoint(0, 10));
}

void TimelineSection::RecalculatePosition() {
	int cardHorizontalSpacing = TimelineCard::GetHorizontalSpacing();
	int cardWidth = TimelineCard::GetWidth();

	int cardSpace = cardHorizontalSpacing + cardWidth;

	m_insideRect.x = (cardSpace * m_first) +
		(m_pos * m_horSpacing) +
		(m_pos * 2 * m_markerWidth) + m_markerWidth;
	
	m_insideRect.width = (cardSpace * (m_last - m_first));
	m_insideRect.height = ((TimelineCanvas*)m_canvas)->GetBottom();

	for (int i = 0; i < m_separators.size(); i++)
		m_separators[i] = m_insideRect.x + (cardSpace * (i + 1));
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

	AppendSection();
	AppendSection();

	TimelineCard* shape = nullptr;
	TimelineCard* first = nullptr;
	for (int i = 0; i < 6; i++) {
		AddThread(i, wxColour(rand() % 255, rand() % 255, rand() % 255), false);
		shape = AddCard(i, i, 0);

		if (i == 1)
			first = shape;
	}

	AppendSection();

	/*for (int i = 6; i < 56; i++) {
		shape = AddCard(rand() % 6, i);
	}*/
	
	RepositionThreads();

	m_curDragCell.second.height = TimelineCard::GetHeight();
	m_curDragCell.second.width = TimelineCard::GetWidth();

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

/// <summary>
/// Column parameter is in relation to section!
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <param name="section"></param>
/// <param name="recalcPos"></param>
/// <returns></returns>
TimelineCard* TimelineCanvas::AddCard(int row, int col, int section, bool recalcPos) {
	TimelineCard* shape = (TimelineCard*)GetDiagramManager()->AddShape(CLASSINFO(TimelineCard), wxPoint());
	shape->SetRow(row);

	if (recalcPos)
		shape->RecalculatePosition();

	SetCardToSection(section, shape);
	SetCardToColumn(m_sections[section].GetFirst() + col, shape);

	if (m_propagateColour)
		shape->SetColour(GetThreadColour(row));
	
	ShapeList temp;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), temp);

	SaveCanvasState();
	return shape;
}

void TimelineCanvas::AppendSection() {
	ShapeList shapes;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), shapes);

	TimelineSection section(m_sections.size(), shapes.size(), shapes.size(), this);
	m_sections.push_back(section);
	
	AddCard(0, 0, m_sections.size() - 1);
}

void TimelineCanvas::RepositionThreads() {
	int width = m_sections.back().GetRect().GetRight();

	int cardVerSpacing = TimelineCard::GetVerticalSpacing();
	int cardHeight = TimelineCard::GetHeight();
	int threadHeight = TimelineThread::GetHeight();
	
	int bottom = -1;

	for (int i = 0; i < m_threads.size(); i++) {
		m_threads[i].SetY(((cardVerSpacing + cardHeight) * (i + 1)) 
			- (cardHeight / 2) 
			- (threadHeight / 2));

		m_threads[i].SetRect(wxRect(0,
			m_threads[i].GetY() + (threadHeight / 2) - (cardHeight / 2) - (cardVerSpacing / 2),
			width,
			cardVerSpacing + cardHeight));

		int current = m_threads[i].GetRect().GetBottom();
		if (current > bottom)
			bottom = current;
	}

	for (auto& it : m_sections)
		it.SetHeight(bottom);
}

/// <summary>
/// 
/// </summary>
/// <param name="section"></param>
/// <param name="shape"></param>
/// <returns>std::pair, in which first is section's first card and second is section's last card.</returns>
pair<int, int> TimelineCanvas::SetCardToSection(int section, TimelineCard* shape) {
	int prevSection = shape->GetSection();
	shape->SetSection(section);
	
	if (prevSection == section)
		return pair<int, int>(0, 0);

	if (prevSection == -1)
		prevSection = m_sections.size();

	if (prevSection > section) {
		if (prevSection - section > 1)
			for (int i = section + 1; i < prevSection; i++) {
				m_sections[i].ShiftRight();
				m_sections[i].RecalculatePosition();
			}

		if (prevSection < m_sections.size()) {
			m_sections[prevSection].RemoveCardFromBeggining();
			m_sections[prevSection].RecalculatePosition();
		}

		m_sections[section].AppendCard();
	} else {
		if (section - prevSection > 1)
			for (int i = prevSection + 1; i < section; i++) {
				m_sections[i].ShiftLeft();
				m_sections[i].RecalculatePosition();
			}

		if (prevSection < m_sections.size()) {
			m_sections[prevSection].RemoveCardFromEnd();
			m_sections[prevSection].RecalculatePosition();
		}

		m_sections[section].PrependCard();
	}

	m_sections[section].RecalculatePosition();
	return pair<int, int>(m_sections[section].GetFirst(), m_sections[section].GetLast());
}

bool TimelineCanvas::SetCardToColumn(int column, TimelineCard* shape) {
	ShapeList list;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), list);

	TimelineCard* card = shape;
	int prevCol = card->GetColumn();

	if (prevCol == column)
		return false;

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

	Refresh();
	return true;
}

bool TimelineCanvas::CalculateCellDrag(wxPoint& pos) {
	bool changed = false;

	for (int i = 0; i < m_sections.size(); i++) {
		if (m_sections[i].Contains(pos)) {
			m_curDragSection = i;
			wxPoint cell = m_sections[i].GetCellInPosition(pos);

			if (cell.y != m_curDragCell.first.y) {
				m_curDragCell.first.y = cell.y;
				m_curDragCell.second.y = (TimelineCard::GetVerticalSpacing() * (cell.y + 1)) +
					(TimelineCard::GetHeight() * cell.y);
				changed = true;
			}

			if (cell.x != m_curDragCell.first.x) {
				int cardHorSpacing = TimelineCard::GetHorizontalSpacing();
				int sectionMarkerWidth = TimelineSection::GetMarkerWidth();

				m_curDragCell.first.x = cell.x;
				m_curDragCell.second.x = ((TimelineCard::GetWidth() + cardHorSpacing) * cell.x) +
					(sectionMarkerWidth + (cardHorSpacing / 2)) +
					(i * (TimelineSection::GetHorizontalSpcaing() + (sectionMarkerWidth * 2)));

				changed = true;
			}
		}
	}

	return changed;
}

wxColour TimelineCanvas::GetThreadColour(int thread) {
	if (thread >= m_threads.size())
		return wxColour(255, 255, 255);

	return m_threads[thread].GetColour();
}

int TimelineCanvas::GetBottom() {
	int bottom = -1;

	for (auto& it : m_threads) {
		int current = it.GetRect().GetBottom();
		if (current > bottom)
			bottom = current;
	}

	return bottom;
}

void TimelineCanvas::DrawBackground(wxDC& dc, bool fromPaint) {
	wxSFShapeCanvas::DrawBackground(dc, fromPaint);

	int threadWidth = GetVirtualSize().x / GetScale();

	for (auto& it : m_threads)
		it.Draw(dc, threadWidth);
}

void TimelineCanvas::DrawForeground(wxDC& dc, bool fromPaint) {
	int virtualHeight = GetVirtualSize().y / GetScale();

	for (auto& it : m_sections)
		it.Draw(dc, virtualHeight, m_drawSeparators);

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
		AutoWrapTextShape::ShouldCountLines(false);
		if (CalculateCellDrag(DP2LP(event.GetPosition())))
			Refresh(false);
	}

	wxSFShapeCanvas::OnMouseMove(event);
}

void TimelineCanvas::OnMouseWheel(wxMouseEvent& event) {
	AutoWrapTextShape::ShouldCountLines(false);
	wxSFShapeCanvas::OnMouseWheel(event);
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
		if (!sel.IsEmpty()) {
			pSel = (TimelineCard*)sel[0];
			pSel->SetRow(m_curDragCell.first.y);

			int prevSection = pSel->GetSection();
			int column = m_curDragCell.first.x;

			pair<int, int> sectionSpan = SetCardToSection(m_curDragSection, pSel);

			int curSection = pSel->GetSection();

			if (prevSection > curSection && column + 1 == sectionSpan.second)
				column += 1;
			else if (curSection > prevSection && column - 1 == sectionSpan.first)
				column -= 1;
			
			if (!SetCardToColumn(column, pSel))
				pSel->RecalculatePosition();

			if (m_propagateColour)
				pSel->SetColour(GetThreadColour(m_curDragCell.first.y));
		}
	}

	m_curDragSection = 0;
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

void TimelineCanvas::OnTextChange(wxSFEditTextShape* shape) {
	AutoWrapTextShape::ShouldCountLines(true);
	wxSFShapeCanvas::OnTextChange(shape);
	RefreshCanvas(true, GetTotalBoundingBox());
}

void TimelineCanvas::OnScroll(wxScrollWinEvent& event) {
	AutoWrapTextShape::ShouldCountLines(false);
	
	if (!wxGetKeyState(WXK_CONTROL))
		event.Skip();
}


////////////////////////////////////////////////////////////////////
//////////////////////////// Timeline //////////////////////////////
////////////////////////////////////////////////////////////////////


amTimeline::amTimeline(wxWindow* parent) : wxSplitterWindow(parent, 01, wxDefaultPosition, wxDefaultSize, 768L | wxSP_LIVE_UPDATE) {
	this->m_parent = (amOutline*)(parent->GetParent());
	m_manager = amGetManager();
	
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

	dc.SetPen(wxPen(wxColour(140, 140, 140), 4));

	int x1 = m_pullRect.x, x2 = m_pullRect.x + m_pullRect.width - 2;
	int y = m_pullRect.height / 4 + 2;

	dc.DrawLine(wxPoint(x1, y), wxPoint(x2, y));
	dc.DrawLine(wxPoint(x1, y * 2), wxPoint(x2, y * 2));
	dc.DrawLine(wxPoint(x1, y * 3), wxPoint(x2, y * 3));
}

void amTimeline::Save() {
	amDocument document;
	document.tableName = "outline_timelines";
	document.name = "Timeline Canvas";

	document.strings.push_back(pair<wxString, wxString>("content", wxString()));

	wxStringOutputStream sstream(&document.strings[0].second);
	m_canvas->SaveCanvas(sstream);

	m_manager->SaveDocument(document, document);
}

void amTimeline::Load(wxStringInputStream& stream) {
//	if (stream.CanRead() && stream.IsOk() && !stream.Eof())
//		m_canvas->LoadCanvas(stream);
}

void amTimeline::SaveThreads(wxStringOutputStream& stream) {

}

void amTimeline::LoadThreads(wxStringInputStream& stream) {

}
