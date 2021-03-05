#include "Timeline.h"
#include "Document.h"
#include "MyApp.h"

#include <wx\sstream.h>


///////////////////////////////////////////////////////////////
//////////////////////// TimelineThread ///////////////////////
///////////////////////////////////////////////////////////////


int TimelineThread::m_height = 10;
int TimelineThread::m_width = 1000;
int TimelineThread::m_titleOffset = 250;
wxPen TimelineThread::m_hoverPen{ wxColour(160,160,245), 4 };
wxPen TimelineThread::m_selectedPen{ wxColour(110,110,200), 5 };

void TimelineThread::Draw(wxDC& dc) {
	if (m_isSelected && m_drawSelected)
		DrawSelected(dc);
	else if (m_isHovering)
		DrawHover(dc);
	else
		DrawNormal(dc);
}

void TimelineThread::DrawNormal(wxDC& dc) {
	dc.SetPen(wxPen(m_colour));
	dc.SetBrush(wxBrush(m_colour));

	dc.DrawRectangle(wxPoint(0, m_y), wxSize(m_width, m_height));
}

void TimelineThread::DrawHover(wxDC& dc) {
	dc.SetPen(m_hoverPen);
	dc.SetBrush(wxBrush(m_colour));

	dc.DrawRectangle(wxPoint(0, m_y), wxSize(m_width, m_height));
}

void TimelineThread::DrawSelected(wxDC& dc) {
	dc.SetPen(m_selectedPen);
	dc.SetBrush(wxBrush(m_colour));

	dc.DrawRectangle(wxPoint(0, m_y), wxSize(m_width, m_height));
}

void TimelineThread::Refresh(bool delayed) {
	wxRect rect(m_boundingRect);
	rect.width = m_canvas->GetVirtualSize().x / m_canvas->GetScale();

	if (delayed)
		m_canvas->InvalidateRect(rect);
	else
		m_canvas->RefreshCanvas(true, rect);
}

void TimelineThread::OnLeftDown(const wxPoint& pos) {
	m_isSelected = true;
	m_drawSelected = true;
	Refresh();

	if (m_canvas)
		m_canvas->OnThreadSelected(this);
}

void TimelineThread::KillFocus() {
	m_isSelected = false;
	m_drawSelected = false;
	Refresh();

	if (m_canvas)
		m_canvas->OnThreadUnselected(this);
}

void TimelineThread::OnMouseMove(const wxPoint& pos) {
	m_isHovering = true;

	if (!m_isSelected)
		Refresh();
}

void TimelineThread::OnMouseLeave() {
	m_isHovering = false;
	Refresh();
}


///////////////////////////////////////////////////////////////
//////////////////////// TimelineSection //////////////////////
///////////////////////////////////////////////////////////////


int TimelineSection::m_horSpacing = 400;
int TimelineSection::m_markerWidth = 20;
int TimelineSection::m_titleOffset = 200;

wxBrush TimelineSection::m_fill{ wxColour(250,250,250) };
wxBrush TimelineSection::m_textBackground{ wxColour(0, 0, 0, 80) };
wxPen TimelineSection::m_border{ wxColour(0,0,0), 4 };
wxPen TimelineSection::m_separatorPen{ wxColour(130, 130, 130), 2, wxPENSTYLE_LONG_DASH };
wxColour TimelineSection::m_textShadow{ 0, 0, 0 };

void TimelineSection::AppendCard() {
	m_last++;

	if (m_last - m_first >= 1)
		m_separators.push_back(m_insideRect.x + m_insideRect.width);

	m_isEmpty = false;
	RecalculatePosition();
}

void TimelineSection::PrependCard() {
	m_first--;

	if (m_last - m_first >= 1)
		m_separators.insert(m_separators.begin(), m_insideRect.x);

	m_isEmpty = false;
	RecalculatePosition();
}

void TimelineSection::RemoveCardFromEnd() {
	m_last--;

	if (!m_separators.empty())
		m_separators.pop_back();

	m_isEmpty = m_last < m_first;
	RecalculatePosition();
}

void TimelineSection::RemoveCardFromBeggining() {
	m_first++;

	if (!m_separators.empty())
		m_separators.erase(m_separators.begin());

	m_isEmpty = m_last < m_first;
	RecalculatePosition();
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

	int yCell;
	
	if (pos.y <= m_titleOffset)
		yCell = 0;
	else
		yCell = (((pos.y - (TimelineCard::GetVerticalSpacing() / 2)) - GetTitleOffset()) /
		(TimelineCard::GetVerticalSpacing() + TimelineCard::GetHeight()));

	return wxPoint(xCell, yCell);
}

void TimelineSection::DrawNormal(wxDC& dc, int virtualHeight, bool drawSeparators) {
	dc.SetPen(m_border);
	dc.SetBrush(m_fill);

	dc.DrawRectangle(m_insideRect.GetTopLeft() - wxPoint(m_markerWidth, 0), wxSize(m_markerWidth, m_insideRect.height + 10));
	dc.DrawRectangle(m_insideRect.GetTopRight(), wxSize(m_markerWidth, m_insideRect.height + 10));

	if (!m_isEmpty) {
		dc.SetFont(m_titleFont);

		//dc.SetTextForeground(m_textShadow);
		//dc.DrawText(m_titleToDraw, wxPoint(m_insideRect.GetLeft() + 23, 43));
		dc.SetBrush(m_textBackground);
		dc.SetPen(wxNullPen);
		dc.DrawRectangle(wxPoint(m_insideRect.GetLeft(), 30), wxSize(m_insideRect.GetWidth(), m_titleHeight));

		dc.SetTextForeground(m_fill.GetColour());
		dc.DrawText(m_titleToDraw, wxPoint(m_insideRect.GetLeft() + 20, 40));
	}
	
	dc.SetPen(m_separatorPen);
	if (drawSeparators) {
		for (auto& it : m_separators)
			dc.DrawLine(wxPoint(it, m_separatorY), wxPoint(it, m_insideRect.height));
	}

	dc.DrawLine(wxPoint(m_insideRect.GetLeft(), m_separatorY), wxPoint(m_insideRect.GetRight(), m_separatorY));

	dc.DrawLine(m_insideRect.GetBottomLeft(), m_insideRect.GetBottomRight());
	dc.DrawLine(wxPoint(m_insideRect.GetLeft(), m_bottom1), wxPoint(m_insideRect.GetRight(), m_bottom2));
}

bool TimelineSection::EmptyContains(wxPoint& pos) {
	wxRect rect(m_insideRect);

	rect.x -= m_markerWidth;
	rect.width += (2 * m_markerWidth);

	return rect.Contains(pos);
}

void TimelineSection::CalculateTitleWrap() {
	if (m_title.IsEmpty() || m_isEmpty) {
		m_titleToDraw.Empty();
		return;
	}

	m_titleToDraw = m_title;

	wxClientDC dc(m_canvas);
	wxGraphicsContext* pGC = wxGraphicsContext::Create(dc);
	pGC->SetFont(m_titleFont, wxColour(0,0,0));

	int index = 0;
	int strLen = m_titleToDraw.Length();

	double wd = -1, hd = -1;

	int maxWidth = m_insideRect.GetWidth() - 20;

	while (index <= strLen + 1) {
		index += 2;

		if (index == strLen)
			index = strLen;

		pGC->GetTextExtent(m_titleToDraw.SubString(0, index), &wd, &hd);

		if (wd >= maxWidth) {
			m_titleToDraw.Remove(index - 3);
			m_titleToDraw << "...";
			m_titleHeight = hd + 20;
			break;
		}
	}
}

void TimelineSection::RecalculatePosition() {
	int cardHorizontalSpacing = TimelineCard::GetHorizontalSpacing();
	int cardWidth = TimelineCard::GetWidth();

	int cardSpace = cardHorizontalSpacing + cardWidth;

	m_insideRect.x = (cardSpace * m_first) 
		+ (m_pos * m_horSpacing)
		+ (m_pos * 2 * m_markerWidth) 
		+ m_markerWidth
		+ TimelineThread::GetTitleOffset();
	
	m_insideRect.width = (cardSpace * (m_last - m_first + 1));
	m_insideRect.height = m_canvas->GetBottom();

	for (int i = 0; i < m_separators.size(); i++)
		m_separators[i] = m_insideRect.x + (cardSpace * (i + 1));

	m_bottom2 = (m_insideRect.GetBottomLeft() + wxPoint(0, 10)).y;
	m_bottom1 = (m_insideRect.GetBottomRight() + wxPoint(0, 10)).y;

	CalculateTitleWrap();
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
	SetGradientFrom(wxColour(20, 20, 20));
	SetGradientTo(wxColour(30, 30, 30));

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
	RepositionThreads();

	m_curDragCell.second.height = TimelineCard::GetHeight();
	m_curDragCell.second.width = TimelineCard::GetWidth();

	m_threadSelectionTimer.Bind(wxEVT_TIMER, &TimelineCanvas::OnThreadSelectionTimer, this);

	UpdateVirtualSize();
	Refresh(true);
}

void TimelineCanvas::AddThread(int pos, wxColour& colour, bool refresh) {
	if (pos >= m_threads.size())
		m_threads.push_back(TimelineThread(-1, colour, this));
	else {
		int i = 0;
		for (auto& it : m_threads) {
			if (i++ == pos)
				m_threads.insert(&it, TimelineThread(-1, colour, this));
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
TimelineCard* TimelineCanvas::AddCard(int row, int col, int section) {
	TimelineCard* shape = (TimelineCard*)GetDiagramManager()->AddShape(CLASSINFO(TimelineCard), wxPoint());

	SetCardToRow(row, shape);
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

	TimelineSection section(m_sections.size(), shapes.size(), shapes.size() - 1, this);
	m_sections.push_back(section);
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
			- (threadHeight / 2)
			+ TimelineSection::GetTitleOffset());

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
			for (int i = section + 1; i < prevSection; i++)
				m_sections[i].ShiftRight();

		if (prevSection < m_sections.size())
			m_sections[prevSection].RemoveCardFromBeggining();
		

		m_sections[section].AppendCard();
	} else {
		if (section - prevSection > 1)
			for (int i = prevSection + 1; i < section; i++)
				m_sections[i].ShiftLeft();

		if (prevSection < m_sections.size())
			m_sections[prevSection].RemoveCardFromEnd();

		m_sections[section].PrependCard();
	}

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

bool TimelineCanvas::SetCardToRow(int row, TimelineCard* shape) {
	if (shape->GetRow() == row)
		return false;

	shape->SetRow(row);

	if (m_propagateColour)
		shape->SetColour(GetThreadColour(row));

	return true;
}

bool TimelineCanvas::CalculateCellDrag(wxPoint& pos) {
	bool changed = false;
	bool hitPopulatedSection = false;

	for (int i = 0; i < m_sections.size(); i++) {
		if (m_sections[i].Contains(pos)) {
			m_curDragSection = i;
			wxPoint cell = m_sections[i].GetCellInPosition(pos);

			if (cell.y != m_curDragCell.first.y || m_isDragOnEmptySection) {
				m_curDragCell.first.y = cell.y;
				m_curDragCell.second.y = (TimelineCard::GetVerticalSpacing() * (cell.y + 1)) +
					(TimelineCard::GetHeight() * cell.y) + TimelineSection::GetTitleOffset();
				changed = true;
			}

			if (cell.x != m_curDragCell.first.x || m_isDragOnEmptySection) {
				int cardHorSpacing = TimelineCard::GetHorizontalSpacing();
				int sectionMarkerWidth = TimelineSection::GetMarkerWidth();

				m_curDragCell.first.x = cell.x;
				m_curDragCell.second.x = ((TimelineCard::GetWidth() + cardHorSpacing) * cell.x) 
					+ (sectionMarkerWidth + (cardHorSpacing / 2)) 
					+ (i * (TimelineSection::GetHorizontalSpcaing() + (sectionMarkerWidth * 2))
					+ TimelineThread::GetTitleOffset());

				changed = true;
			}

			hitPopulatedSection = true;
			m_isDragOnEmptySection = false;
			break;
		}
	}

	if (!hitPopulatedSection) {
		for (int i = 0; i < m_sections.size(); i++) {
			if (m_sections[i].GetFirst() > m_sections[i].GetLast()) {
				if (m_sections[i].EmptyContains(pos)) {
					m_curDragSection = i;
					
					for (int j = 0; j < m_threads.size(); j++) {
						if (m_threads[j].GetRect().Contains(pos)) {
							m_curDragCell.first.y = j;
							m_curDragCell.second.y = m_threads[j].GetRect().GetTop();
							break;
						}
					}

					m_curDragCell.first.x = m_sections[i].GetFirst();
					m_curDragCell.second.x = m_sections[i].GetRect().GetLeft();

					changed = true;
					
					m_isDragOnEmptySection = true;
					break;
				}
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

void TimelineCanvas::OnThreadSelected(TimelineThread* thread) {
	m_threadSelectionTimer.Start(700);
	m_parent->SetThreadData(thread);
}

void TimelineCanvas::OnThreadUnselected(TimelineThread* thread) {
	m_threadSelectionTimer.Stop();
}

void TimelineCanvas::OnThreadSelectionTimer(wxTimerEvent& event) {
	if (m_selectedThread) {
		m_selectedThread->SetDrawSelected(!m_selectedThread->GetDrawSelected());
		
		if (IsShownOnScreen())
			m_selectedThread->Refresh(false);
		else
			m_selectedThread->Refresh(true);
	}
}

void TimelineCanvas::DrawBackground(wxDC& dc, bool fromPaint) {
	wxSFShapeCanvas::DrawBackground(dc, fromPaint);

	int threadWidth = GetVirtualSize().x / GetScale();

	for (TimelineThread& thread : m_threads)
		thread.Draw(dc);
}

void TimelineCanvas::DrawForeground(wxDC& dc, bool fromPaint) {
	int virtualHeight = GetVirtualSize().y / GetScale();

	for (auto& it : m_sections)
		it.DrawNormal(dc, virtualHeight, m_drawSeparators);

	dc.SetPen(wxPen(wxColour(180, 180, 255, 128), 2));
	dc.SetBrush(wxBrush(wxColour(200, 200, 255, 128)));

	if (m_nWorkingMode == modeSHAPEMOVE && m_curDragCell.first.x != -1) {
		dc.DrawRectangle(m_curDragCell.second);
	}

	dc.SetPen(wxNullPen);
	dc.SetBrush(wxNullBrush);
}

void TimelineCanvas::OnUpdateVirtualSize(wxRect& rect) {
	if (m_sections.empty())
		rect.width += 300;
	else
		rect.width = m_sections.back().GetRect().GetRight() + 300;

	rect.height += 300;

	TimelineThread::SetWidth(rect.width / m_Settings.m_nScale);
}

void TimelineCanvas::OnMouseMove(wxMouseEvent& event) {
	wxPoint pos = DP2LP(event.GetPosition());

	if (m_nWorkingMode == modeSHAPEMOVE) {
		if (CalculateCellDrag(pos))
			Refresh(false);
	}

	bool hasThreadHover = false;
	for (TimelineThread& thread : m_threads) {
		if (thread.Contains(pos)) {
			if (m_threadUnderMouse && m_threadUnderMouse != &thread)
				m_threadUnderMouse->OnMouseLeave();

			m_threadUnderMouse = &thread;
			thread.OnMouseMove(pos);
			
			hasThreadHover = true;
			break;
		}
	}

	if (!hasThreadHover && m_threadUnderMouse) {
		m_threadUnderMouse->OnMouseLeave();
		m_threadUnderMouse = nullptr;
	}
	
	wxSFShapeCanvas::OnMouseMove(event);
}

void TimelineCanvas::OnLeftDown(wxMouseEvent& event) {
	wxPoint pos = DP2LP(event.GetPosition());

	if (GetShapeUnderCursor())
		CalculateCellDrag(pos);
	else {
		bool hitThread = false;
		for (TimelineThread& thread : m_threads) {
			if (thread.Contains(pos)) {
				if (m_selectedThread && m_selectedThread != &thread)
					m_selectedThread->KillFocus();

				thread.OnLeftDown(pos);
				m_selectedThread = &thread;

				hitThread = true;
				break;
			}
		}

		if (!hitThread && m_selectedThread) {
			m_selectedThread->KillFocus();
			m_selectedThread = nullptr;
		}
	}

	wxSFShapeCanvas::OnLeftDown(event);
}

void TimelineCanvas::OnLeftUp(wxMouseEvent& event) {
	if (m_nWorkingMode == modeSHAPEMOVE) {
		ShapeList sel;
		GetSelectedShapes(sel);

		TimelineCard* pSel = nullptr;
		if (!sel.IsEmpty()) {
			pSel = (TimelineCard*)sel[0];
			SetCardToRow(m_curDragCell.first.y, pSel);

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
			//Undo();
			break;

		default:
			break;
		}
	}
}

void TimelineCanvas::OnTextChange(wxSFEditTextShape* shape) {
	AutoWrapTextShape* pShape = dynamic_cast<AutoWrapTextShape*>(shape);

	if (pShape)
		pShape->CalcWrappedText();

	wxSFShapeCanvas::OnTextChange(shape);
}

void TimelineCanvas::OnScroll(wxScrollWinEvent& event) {	
	if (!wxGetKeyState(WXK_CONTROL))
		event.Skip();
}


////////////////////////////////////////////////////////////////////
//////////////////////////// Timeline //////////////////////////////
////////////////////////////////////////////////////////////////////


amTimeline::amTimeline(wxWindow* parent) : amSplitterWindow(parent) {
	this->m_parent = (amOutline*)(parent->GetParent());
	m_manager = amGetManager();
	
	m_canvas = new TimelineCanvas(&m_canvasManager, this);
	m_sidebar = new amTimelineSidebar(this);

	SetMinimumPaneSize(40);
	SplitVertically(m_canvas, m_sidebar, 99999);
	SetSashGravity(1.0);
	SetSashInvisible(true);
}

void amTimeline::ShowSidebar() {
	int i = 1;
	int j = GetSashPosition();

	wxSize canvasSize = m_canvas->GetClientSize();

	bool show = !m_isSidebarShown;
	m_sidebar->ShowContent(show);

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

	m_sidebar->Layout();
	m_sidebar->Refresh();
}

void amTimeline::SetThreadData(TimelineThread* thread) {
	m_sidebar->SetThreadData(thread);
}

void amTimeline::SetSectionData(TimelineSection* section) {
	m_sidebar->SetSectionData(section);
}

void amTimeline::SetCardData(TimelineCard* card) {
	m_sidebar->SetCardData(card);
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


////////////////////////////////////////////////////////////////////
///////////////////////// Timeline Sidebar /////////////////////////
////////////////////////////////////////////////////////////////////


amTimelineSidebar::amTimelineSidebar(wxWindow* parent) : wxPanel(parent) {
	m_parent = (amTimeline*)parent;

	SetBackgroundColour(wxColour(60, 60, 60));

	m_content = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_content->SetBackgroundColour(wxColour(60, 60, 60));

	m_threadPanel = new wxScrolledWindow(m_content);
	m_threadPanel->SetBackgroundColour(wxColour(60, 60, 60));

	m_content->AddPage(m_threadPanel, "Threads");

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_content, wxSizerFlags(1).Expand().Border(wxTOP, 40));

	SetSizer(sizer);
	m_content->Hide();

	Bind(wxEVT_PAINT, &amTimelineSidebar::OnPaint, this);
	Bind(wxEVT_MOTION, &amTimelineSidebar::OnMouseMove, this);
	Bind(wxEVT_LEFT_DOWN, &amTimelineSidebar::OnLeftDown, this);
}

void amTimelineSidebar::ShowContent(bool show) {
	m_content->Show(show);
}

void amTimelineSidebar::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);

	dc.SetPen(wxPen(wxColour(140, 140, 140), 4));

	int x1 = m_pullRect.x, x2 = m_pullRect.x + m_pullRect.width - 2;
	int y = m_pullRect.height / 4 + 2;

	dc.DrawLine(wxPoint(x1, y), wxPoint(x2, y));
	dc.DrawLine(wxPoint(x1, y * 2), wxPoint(x2, y * 2));
	dc.DrawLine(wxPoint(x1, y * 3), wxPoint(x2, y * 3));
}

void amTimelineSidebar::OnMouseMove(wxMouseEvent& event) {
	if (m_pullRect.Contains(event.GetPosition()))
		wxSetCursor(wxCURSOR_CLOSED_HAND);
	else
		wxSetCursor(wxCURSOR_DEFAULT);
}

void amTimelineSidebar::OnLeftDown(wxMouseEvent& event) {
	if (m_pullRect.Contains(event.GetPosition()))
		m_parent->ShowSidebar();
}

void amTimelineSidebar::SetThreadData(TimelineThread* thread) {}

void amTimelineSidebar::SetSectionData(TimelineSection* section) {}

void amTimelineSidebar::SetCardData(TimelineCard* card) {}
