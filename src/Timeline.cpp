#include "MyApp.h"
#include "Document.h"
#include "Timeline.h"
#include "wxmemdbg.h"

#include <wx\sstream.h>
#include <wx\dcbuffer.h>
#include <wx\dcgraph.h>



///////////////////////////////////////////////////////////////
////////////////////////// amTLThread /////////////////////////
///////////////////////////////////////////////////////////////


int amTLThread::m_height = 10;
int amTLThread::m_width = 1000;
int amTLThread::m_titleOffset = 250;
wxPen amTLThread::m_hoverPen{ wxColour(160,160,245), 4 };
wxPen amTLThread::m_selectedPen{ wxColour(110,110,200), 5 };

void amTLThread::Draw(wxDC& dc) {
	if (m_isSelected && m_drawSelected)
		DrawSelected(dc);
	else if (m_isHovering)
		DrawHover(dc);
	else
		DrawNormal(dc);
}

void amTLThread::DrawNormal(wxDC& dc) {
	dc.SetPen(wxPen(m_colour));
	dc.SetBrush(wxBrush(m_colour));

	dc.DrawRectangle(wxPoint(0, m_y), wxSize(m_width, m_height));
}

void amTLThread::DrawHover(wxDC& dc) {
	dc.SetPen(m_hoverPen);
	dc.SetBrush(wxBrush(m_colour));

	dc.DrawRectangle(wxPoint(0, m_y), wxSize(m_width, m_height));
}

void amTLThread::DrawSelected(wxDC& dc) {
	dc.SetPen(m_selectedPen);
	dc.SetBrush(wxBrush(m_colour));

	dc.DrawRectangle(wxPoint(0, m_y), wxSize(m_width, m_height));
}

void amTLThread::Refresh(bool delayed) {
	wxRect rect(m_boundingRect);
	rect.width = m_canvas->GetVirtualSize().x / m_canvas->GetScale();

	if (delayed)
		m_canvas->InvalidateRect(rect);
	else
		m_canvas->RefreshCanvas(true, rect);
}

void amTLThread::OnLeftDown(const wxPoint& pos) {
	m_isSelected = true;
	m_drawSelected = true;
	Refresh();
}

void amTLThread::KillFocus() {
	m_isSelected = false;
	m_drawSelected = false;
	Refresh();
}

void amTLThread::OnMouseMove(const wxPoint& pos) {
	m_isHovering = true;

	if (!m_isSelected)
		Refresh();
}

void amTLThread::OnMouseLeave() {
	m_isHovering = false;
	Refresh();
}


///////////////////////////////////////////////////////////////
//////////////////////// TimelineSection //////////////////////
///////////////////////////////////////////////////////////////


int amTLSection::m_horSpacing = 400;
int amTLSection::m_markerWidth = 20;
int amTLSection::m_titleOffset = 200;

wxBrush amTLSection::m_fill{ wxColour(250,250,250) };
wxBrush amTLSection::m_textBackground{ wxColour(0, 0, 0, 80) };
wxPen amTLSection::m_border{ wxColour(0,0,0), 4 };
wxPen amTLSection::m_separatorPen{ wxColour(130, 130, 130), 2, wxPENSTYLE_LONG_DASH };
wxColour amTLSection::m_textShadow{ 0, 0, 0 };

void amTLSection::AppendCard() {
	m_last++;

	if (m_last - m_first >= 1)
		m_separators.push_back(m_insideRect.x + m_insideRect.width);

	m_isEmpty = false;
	RecalculatePosition();
}

void amTLSection::PrependCard() {
	m_first--;

	if (m_last - m_first >= 1)
		m_separators.insert(m_separators.begin(), m_insideRect.x);

	m_isEmpty = false;
	RecalculatePosition();
}

void amTLSection::RemoveCardFromEnd() {
	m_last--;

	if (!m_separators.empty())
		m_separators.pop_back();

	m_isEmpty = m_last < m_first;
	RecalculatePosition();
}

void amTLSection::RemoveCardFromBeggining() {
	m_first++;

	if (!m_separators.empty())
		m_separators.erase(m_separators.begin());

	m_isEmpty = m_last < m_first;
	RecalculatePosition();
}

void amTLSection::ShiftLeft() {
	PrependCard();
	RemoveCardFromEnd();
	RecalculatePosition();
}

void amTLSection::ShiftRight() {
	AppendCard();
	RemoveCardFromBeggining();
	RecalculatePosition();
}

wxPoint amTLSection::GetCellInPosition(wxPoint& pos) {
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

void amTLSection::DrawNormal(wxDC& dc, bool drawSeparators) {
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
	
	if (drawSeparators) {
		dc.SetPen(m_separatorPen);
	
		for (int& it : m_separators)
			dc.DrawLine(wxPoint(it, m_separatorY), wxPoint(it, m_insideRect.height));
	
		dc.DrawLine(wxPoint(m_insideRect.GetLeft(), m_separatorY), wxPoint(m_insideRect.GetRight(), m_separatorY));
		dc.DrawLine(m_insideRect.GetBottomLeft(), m_insideRect.GetBottomRight());
		dc.DrawLine(wxPoint(m_insideRect.GetLeft(), m_bottom1), wxPoint(m_insideRect.GetRight(), m_bottom2));
	}
}

bool amTLSection::EmptyContains(wxPoint& pos) {
	wxRect rect(m_insideRect);

	rect.x -= m_markerWidth;
	rect.width += (2 * m_markerWidth);

	return rect.Contains(pos);
}

void amTLSection::CalculateTitleWrap() {
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

	if (pGC)
		delete pGC;
}

void amTLSection::RecalculatePosition() {
	int cardHorizontalSpacing = TimelineCard::GetHorizontalSpacing();
	int cardWidth = TimelineCard::GetWidth();

	int cardSpace = cardHorizontalSpacing + cardWidth;

	m_insideRect.x = (cardSpace * m_first) 
		+ (m_index * m_horSpacing)
		+ (m_index * 2 * m_markerWidth) 
		+ m_markerWidth
		+ amTLThread::GetTitleOffset();
	
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


amTLTimelineCanvas::amTLTimelineCanvas(wxSFDiagramManager* manager, wxWindow* parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
	amSFShapeCanvas(manager, parent, id, pos, size, style) {
	m_parent = (amTLTimeline*)parent;

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

	GetDiagramManager()->ClearAcceptedShapes();
	GetDiagramManager()->AcceptShape("TimelineCard");

	wxSFShapeCanvas::EnableGC(true);

	AppendSection();
	AppendSection();

	TimelineCard* shape = nullptr;
	for (int i = 0; i < 6; i++) {
		AddThread(i, wxColour(rand() % 255, rand() % 255, rand() % 255), false);
		shape = AddCard(i, i, 0);
	}

	AppendSection();
	RepositionThreads();

	EnableScrolling(true, true);

	m_curDragCell.second.height = TimelineCard::GetHeight();
	m_curDragCell.second.width = TimelineCard::GetWidth();

	m_threadSelectionTimer.Bind(wxEVT_TIMER, &amTLTimelineCanvas::OnThreadSelectionTimer, this);

	UpdateVirtualSize();
	Refresh(true);
}

amTLTimelineCanvas::~amTLTimelineCanvas() {
	CleanUp();
}

void amTLTimelineCanvas::CleanUp() {
	for (amTLThread* thread : m_threads) {
		if (thread)
			delete thread;
	}

	for (amTLSection* section : m_sections) {
		if (section)
			delete section;
	}
}

void amTLTimelineCanvas::AddThread(int pos, wxColour& colour, bool refresh) {
	if (pos >= m_threads.size())
		m_threads.push_back(new amTLThread(m_threads.size(), -1, colour, this));
	else {
		int i = 0;

		for (amTLThread*& thread : m_threads) {
			if (i++ >= pos)
				m_threads.insert(&thread, new amTLThread(pos, -1, colour, this));
		}
	}

	if (refresh) {
		RepositionThreads();
		Refresh(true);
	}
}

TimelineCard* amTLTimelineCanvas::AddCard(int rowIndex, int colIndex, int sectionIndex) {
	if (m_threads.empty() || m_sections.empty()) {
		wxMessageBox("You need at least one thread and one section to be able to place cards on the Timeline!");
		return nullptr;
	}
	
	TimelineCard* shape = (TimelineCard*)GetDiagramManager()->AddShape(CLASSINFO(TimelineCard), wxPoint());

	SetCardToRow(rowIndex, shape);
	SetCardToSection(sectionIndex, shape);
	SetCardToColumn(colIndex, shape);
	
	RepositionThreads();
	UpdateSelectedThreadSBData();
	UpdateSelectedSectionSBData();
	SaveCanvasState();
	return shape;
}

TimelineCard* amTLTimelineCanvas::AppendCard(int rowIndex) {
	TimelineCard* shape = (TimelineCard*)GetDiagramManager()->AddShape(CLASSINFO(TimelineCard), wxPoint());

	SetCardToRow(rowIndex, shape);
	SetCardToSection(m_sections.size() - 1, shape);
	SetCardToColumn(m_sections.back()->GetLast(), shape);

	RepositionThreads();
	UpdateSelectedThreadSBData();
	UpdateSelectedSectionSBData(); 
	SaveCanvasState();
	return shape;
}

void amTLTimelineCanvas::AppendSection() {
	ShapeList shapes;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), shapes);

	amTLSection* section = new amTLSection(m_sections.size(), shapes.size(), shapes.size() - 1, this);
	m_sections.push_back(section);
}

/// <summary>
/// 
/// </summary>
/// <param name="section"></param>
/// <param name="shape"></param>
/// <returns>std::pair, in which first is section's first card and second is section's last card.</returns>
pair<int, int> amTLTimelineCanvas::SetCardToSection(int section, TimelineCard* shape) {
	if (section >= m_sections.size() || m_sections.empty())
		return pair<int, int>(-1, -1);

	int prevSection = shape->GetSection();
	shape->SetSection(section);
	
	if (prevSection == section)
		return pair<int, int>(0, 0);

	if (prevSection == -1)
		prevSection = m_sections.size();

	if (prevSection > section) {
		if (prevSection - section > 1)
			for (int i = section + 1; i < prevSection; i++)
				m_sections[i]->ShiftRight();

		if (prevSection < m_sections.size())
			m_sections[prevSection]->RemoveCardFromBeggining();
		

		m_sections[section]->AppendCard();
	} else {
		if (section - prevSection > 1)
			for (int i = prevSection + 1; i < section; i++)
				m_sections[i]->ShiftLeft();

		if (prevSection < m_sections.size())
			m_sections[prevSection]->RemoveCardFromEnd();

		m_sections[section]->PrependCard();
	}

	return pair<int, int>(m_sections[section]->GetFirst(), m_sections[section]->GetLast());
}

bool amTLTimelineCanvas::SetCardToColumn(int column, TimelineCard* shape) {
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

bool amTLTimelineCanvas::SetCardToRow(int row, TimelineCard* shape) {
	if (shape->GetRow() == row)
		return false;

	shape->SetRow(row);

	if (m_propagateColour)
		shape->SetColour(GetThreadColour(row));

	return true;
}

bool amTLTimelineCanvas::CalculateCellDrag(wxPoint& pos) {
	bool changed = false;
	bool hitPopulatedSection = false;

	for (int i = 0; i < m_sections.size(); i++) {
		if (m_sections[i]->Contains(pos)) {
			m_curDragSection = i;
			wxPoint cell = m_sections[i]->GetCellInPosition(pos);

			if (cell.y != m_curDragCell.first.y || m_isDragOnEmptySection) {
				m_curDragCell.first.y = cell.y;
				m_curDragCell.second.y = (TimelineCard::GetVerticalSpacing() * (cell.y + 1)) +
					(TimelineCard::GetHeight() * cell.y) + amTLSection::GetTitleOffset();
				changed = true;
			}

			if (cell.x != m_curDragCell.first.x || m_isDragOnEmptySection) {
				int cardHorSpacing = TimelineCard::GetHorizontalSpacing();
				int sectionMarkerWidth = amTLSection::GetMarkerWidth();

				m_curDragCell.first.x = cell.x;
				m_curDragCell.second.x = ((TimelineCard::GetWidth() + cardHorSpacing) * cell.x) 
					+ (sectionMarkerWidth + (cardHorSpacing / 2)) 
					+ (i * (amTLSection::GetHorizontalSpcaing() + (sectionMarkerWidth * 2))
					+ amTLThread::GetTitleOffset());

				changed = true;
			}

			hitPopulatedSection = true;
			m_isDragOnEmptySection = false;
			break;
		}
	}

	if (!hitPopulatedSection) {
		for (int i = 0; i < m_sections.size(); i++) {
			if (m_sections[i]->GetFirst() > m_sections[i]->GetLast()) {
				if (m_sections[i]->EmptyContains(pos)) {
					m_curDragSection = i;
					
					for (int j = 0; j < m_threads.size(); j++) {
						if (m_threads[j]->GetRect().Contains(pos)) {
							m_curDragCell.first.y = j;
							m_curDragCell.second.y = m_threads[j]->GetRect().GetTop();
							break;
						}
					}

					m_curDragCell.first.x = m_sections[i]->GetFirst();
					m_curDragCell.second.x = m_sections[i]->GetRect().GetLeft();

					changed = true;
					
					m_isDragOnEmptySection = true;
					break;
				}
			}
		}
	}

	return changed;
}

wxColour amTLTimelineCanvas::GetThreadColour(int thread) {
	if (thread >= m_threads.size())
		return wxColour(255, 255, 255);

	return m_threads[thread]->GetColour();
}

int amTLTimelineCanvas::GetBottom() {
	int bottom = 200;
	int current;

	for (amTLThread*& thread : m_threads) {
		current = thread->GetRect().GetBottom();
		if (current > bottom)
			bottom = current;
	}

	return bottom;
}

void amTLTimelineCanvas::OnThreadSelected(amTLThread* thread) {
	m_threadSelectionTimer.Start(700);
	UpdateSelectedThreadSBData();
}

void amTLTimelineCanvas::OnThreadUnselected(amTLThread* thread) {
	m_threadSelectionTimer.Stop();
}

void amTLTimelineCanvas::OnThreadSelectionTimer(wxTimerEvent& event) {
	if (m_selectedThread) {
		m_selectedThread->SetDrawSelected(!m_selectedThread->GetDrawSelected());
		
		if (IsShownOnScreen())
			m_selectedThread->Refresh(false);
		else
			m_selectedThread->Refresh(true);
	}
}

void amTLTimelineCanvas::UpdateSelectedThreadSBData() {
	ShapeList shapes;
	ShapeList filtered;

	if (m_selectedThread) {
		GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), shapes);

		ShapeList::compatibility_iterator node = shapes.GetFirst();
		filtered = shapes;
		TimelineCard* card;

		while (node) {
			card = (TimelineCard*)node->GetData();

			if (card) {
				if (card->GetRow() != m_selectedThread->GetIndex())
					filtered.remove(card);
			}

			node = node->GetNext();
		}
	}

	m_parent->SetThreadData(m_selectedThread, filtered);
}

void amTLTimelineCanvas::UpdateSelectedSectionSBData() {
	ShapeList shapes;
	ShapeList filtered;

	if (m_selectedSection) {
		GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), shapes);

		ShapeList::compatibility_iterator node = shapes.GetFirst();
		filtered = shapes;
		TimelineCard* card;

		while (node) {
			card = (TimelineCard*)node->GetData();

			if (card) {
				if (card->GetColumn() < m_selectedSection->GetFirst() 
					|| card->GetColumn() > m_selectedSection->GetLast())
					filtered.remove(card);
			}

			node = node->GetNext();
		}
	}

	m_parent->SetSectionData(m_selectedSection, filtered);
}

void amTLTimelineCanvas::DrawBackground(wxDC& dc, bool fromPaint) {
	amSFShapeCanvas::DrawBackground(dc, fromPaint);

	int threadWidth = GetVirtualSize().x / GetScale();

	for (amTLThread*& thread : m_threads)
		thread->Draw(dc);
}

void amTLTimelineCanvas::DrawForeground(wxDC& dc, bool fromPaint) {
	for (amTLSection*& section : m_sections)
		section->DrawNormal(dc, m_drawSeparators);

	dc.SetPen(wxPen(wxColour(180, 180, 255, 128), 2));
	dc.SetBrush(wxBrush(wxColour(200, 200, 255, 128)));

	if (m_nWorkingMode == modeSHAPEMOVE && m_curDragCell.first.x != -1) {
		dc.DrawRectangle(m_curDragCell.second);
	}

	dc.SetPen(wxNullPen);
	dc.SetBrush(wxNullBrush);
}

void amTLTimelineCanvas::OnUpdateVirtualSize(wxRect& rect) {
	if (!m_sections.empty()) {
		rect.SetSize(GetGoodSize());
	}
	
	amTLThread::SetWidth(rect.width / m_Settings.m_nScale);
}

void amTLTimelineCanvas::OnMouseMove(wxMouseEvent& event) {
	wxPoint pos = DP2LP(event.GetPosition());

	if (m_nWorkingMode == modeSHAPEMOVE) {
		if (CalculateCellDrag(pos))
			Refresh(false);
	}

	bool hasThreadHover = false;

	for (amTLThread*& thread : m_threads) {
		if (thread->Contains(pos)) {
			if (m_threadUnderMouse && m_threadUnderMouse != thread)
				m_threadUnderMouse->OnMouseLeave();

			m_threadUnderMouse = thread;
			thread->OnMouseMove(pos);
			
			hasThreadHover = true;
			break;
		}
	}

	if (!hasThreadHover && m_threadUnderMouse) {
		m_threadUnderMouse->OnMouseLeave();
		m_threadUnderMouse = nullptr;
	}
	
	amSFShapeCanvas::OnMouseMove(event);
}

void amTLTimelineCanvas::OnLeftDown(wxMouseEvent& event) {
	wxPoint pos = DP2LP(event.GetPosition());
	wxSFShapeBase* pShape = GetShapeUnderCursor();

	if (pShape) {
		CalculateCellDrag(pos);
		
		TimelineCard* card = dynamic_cast<TimelineCard*>(pShape->GetGrandParentShape());
		m_parent->SetCardData(card);
	} else {
		bool hitThread = false;
		for (amTLThread*& thread : m_threads) {
			if (thread->Contains(pos)) {
				if (m_selectedThread && m_selectedThread != thread) {
					m_selectedThread->KillFocus();
					OnThreadUnselected(m_selectedThread);
				}

				thread->OnLeftDown(pos);
				m_selectedThread = thread;
				OnThreadSelected(thread);

				hitThread = true;
				Refresh(true);
				break;
			}
		}

		bool hitSection = false;
		for (amTLSection*& section : m_sections) {
			if (section->EmptyContains(pos)) {
				m_selectedSection = section;

				hitSection = true;
				Refresh(true);
				UpdateSelectedSectionSBData();
				break;
			}
		}

		if (!hitThread && m_selectedThread) {
			m_selectedThread->KillFocus();
			m_selectedThread = nullptr;
			UpdateSelectedThreadSBData();
		}

		if (!hitSection && m_selectedSection) {
			m_selectedThread = nullptr;
			UpdateSelectedSectionSBData();
		}

		m_parent->SetCardData(nullptr);
	}

	amSFShapeCanvas::OnLeftDown(event);
}

void amTLTimelineCanvas::OnLeftUp(wxMouseEvent& event) {
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
		
			UpdateSelectedThreadSBData();
			UpdateSelectedSectionSBData();
		}
	}

	amSFShapeCanvas::OnLeftUp(event);
}

void amTLTimelineCanvas::OnLeftDoubleClick(wxMouseEvent& event) {
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
		amSFShapeCanvas::OnLeftDoubleClick(event);
}

void amTLTimelineCanvas::OnKeyDown(wxKeyEvent& event) {
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

void amTLTimelineCanvas::OnTextChange(wxSFEditTextShape* shape) {
	AutoWrapTextShape* pShape = dynamic_cast<AutoWrapTextShape*>(shape);

	if (pShape)
		pShape->CalcWrappedText();

	amSFShapeCanvas::OnTextChange(shape);
}

wxSize amTLTimelineCanvas::GetGoodSize() {
	if (!m_sections.empty())
		return wxSize(m_sections.back()->GetRight() + 300, GetBottom() + 300);
	else
		return wxSize(300, 150);
}

void amTLTimelineCanvas::RepositionThreads() {
	int width;
	if (!m_sections.empty())
		width = m_sections.back()->GetRight();
	else
		width = (double)GetVirtualSize().x / GetScale();
	
	int cardVerSpacing = TimelineCard::GetVerticalSpacing();
	int cardHeight = TimelineCard::GetHeight();
	int threadHeight = amTLThread::GetHeight();

	int bottom = -1;

	for (int i = 0; i < m_threads.size(); i++) {
		m_threads[i]->SetY(((cardVerSpacing + cardHeight) * (i + 1))
			- (cardHeight / 2)
			- (threadHeight / 2)
			+ amTLSection::GetTitleOffset());

		m_threads[i]->SetRect(wxRect(0,
			m_threads[i]->GetY() + (threadHeight / 2) - (cardHeight / 2) - (cardVerSpacing / 2),
			width,
			cardVerSpacing + cardHeight));

		m_threads[i]->SetIndex(i);

		int current = m_threads[i]->GetRect().GetBottom();
		if (current > bottom)
			bottom = current;
	}

	for (amTLSection*& section : m_sections)
		section->SetHeight(bottom);
}


////////////////////////////////////////////////////////////////////
//////////////////////////// Timeline //////////////////////////////
////////////////////////////////////////////////////////////////////


amTLTimeline::amTLTimeline(wxWindow* parent) : amSplitterWindow(parent) {
	this->m_parent = (amOutline*)(parent->GetParent());
	m_manager = amGetManager();
	
	m_canvas = new amTLTimelineCanvas(&m_canvasManager, this);
	m_sidebar = new amTLTimelineSidebar(this, m_canvas);

	SetMinimumPaneSize(40);
	SplitVertically(m_canvas, m_sidebar, 99999);
	SetSashGravity(1.0);
	SetSashInvisible(true);
}

void amTLTimeline::AddCardToThread(amTLThread* thread) {
	if (thread)
		m_canvas->AppendCard(thread->GetIndex());
}

void amTLTimeline::AddCardToSection(amTLSection* section) {
	if (section)
		m_canvas->AddCard(0, section->GetLast() + 1, section->GetIndex());
}

void amTLTimeline::AddCardBefore(TimelineCard* card) {
	int sectionIndex = card->GetSection();
	m_canvas->AddCard(card->GetRow(), card->GetColumn(), sectionIndex);
}

void amTLTimeline::AddCardAfter(TimelineCard* card) {
	int sectionIndex = card->GetSection();
	m_canvas->AddCard(card->GetRow(), card->GetColumn() + 1, sectionIndex);
}

void amTLTimeline::ShowSidebar() {
	int i = 1;
	int j = GetSashPosition();

	wxSize canvasSize = m_canvas->GetClientSize();

	bool show = !m_isSidebarShown;

	if (show) {
		while (i <= 220) {
			SetSashPosition(j - i);
			i += 20;
		}
		m_sidebar->ShowContent(show);
	} else {
		m_canvas->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_NEVER);
		m_sidebar->ShowContent(show);

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

void amTLTimeline::SetThreadData(amTLThread* thread, ShapeList& shapes) {
	if (!m_sidebar)
		return;

	m_sidebar->SetThreadData(thread, shapes);
}

void amTLTimeline::SetSectionData(amTLSection* section, ShapeList& shapes) {
	if (!m_sidebar)
		return;
	
	m_sidebar->SetSectionData(section, shapes);
}

void amTLTimeline::SetCardData(TimelineCard* card) {
	if (!m_sidebar)
		return;

	m_sidebar->SetCardData(card);
}

void amTLTimeline::Save() {
	amDocument document;
	document.tableName = "outline_timelines";
	document.name = "Timeline Canvas";

	document.strings.push_back(pair<wxString, wxString>("content", wxString()));

	wxStringOutputStream sstream(&document.strings[0].second);
	m_canvas->SaveCanvas(sstream);

	m_manager->SaveDocument(document, document);
}

void amTLTimeline::Load(wxStringInputStream& stream) {
//	if (stream.CanRead() && stream.IsOk() && !stream.Eof())
//		m_canvas->LoadCanvas(stream);
}

void amTLTimeline::SaveThreads(wxStringOutputStream& stream) {

}

void amTLTimeline::LoadThreads(wxStringInputStream& stream) {

}


////////////////////////////////////////////////////////////////////
///////////////////////// Timeline Sidebar /////////////////////////
////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(amTLTimelineSidebar, wxPanel)

EVT_BUTTON(BUTTON_AddCardToThread, amTLTimelineSidebar::OnAddCardToThread)

EVT_BUTTON(BUTTON_AddCardBefore, amTLTimelineSidebar::OnAddCardBefore)
EVT_BUTTON(BUTTON_AddCardAfter, amTLTimelineSidebar::OnAddCardAfter)

EVT_BUTTON(BUTTON_AddCardToSection, amTLTimelineSidebar::OnAddCardToSection)

END_EVENT_TABLE()

amTLTimelineSidebar::amTLTimelineSidebar(wxWindow* parent, amTLTimelineCanvas* canvas) : wxPanel(parent) {
	m_parent = (amTLTimeline*)parent;

	SetBackgroundColour(wxColour(60, 60, 60));
	SetDoubleBuffered(true);

	m_content = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_content->SetBackgroundColour(wxColour(60, 60, 60));

	m_threadPanel = new wxScrolledWindow(m_content);
	m_threadPanel->SetBackgroundColour(wxColour(60, 60, 60));

	wxButton* addThread = new wxButton(m_threadPanel, -1, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
	addThread->SetBitmap(wxBITMAP_PNG(plus));

	wxStaticText* curThreadLabel = new wxStaticText(m_threadPanel, -1, "Currently selected:");
	curThreadLabel->SetForegroundColour(wxColour(255, 255, 255));
	curThreadLabel->SetFont(wxFontInfo(12).Bold());

	m_threadThumbnail = new amTLThreadThumbnail(m_threadPanel);
	m_threadThumbnail->SetCanvas(canvas);

	m_curThreadName = new wxStaticText(m_threadPanel, -1, "");
	m_curThreadName->SetBackgroundColour(wxColour(10, 10, 10));
	m_curThreadName->SetForegroundColour(wxColour(255, 255, 255));
	m_curThreadName->SetFont(wxFontInfo(13).Bold());

	wxButton* addCardToThread = new wxButton(m_threadPanel, BUTTON_AddCardToThread, "Add card");
	addCardToThread->SetBackgroundColour(wxColour(30, 30, 30));
	addCardToThread->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* threadPanelSizer = new wxBoxSizer(wxVERTICAL);
	threadPanelSizer->Add(addThread, wxSizerFlags(0).Left().Border(wxALL, 5));
	threadPanelSizer->AddSpacer(10);
	threadPanelSizer->Add(curThreadLabel, wxSizerFlags(0).Left().Border(wxLEFT, 5));
	threadPanelSizer->Add(m_threadThumbnail, wxSizerFlags(0).Expand().Border(wxALL, 5));
	threadPanelSizer->Add(m_curThreadName, wxSizerFlags(0).CenterHorizontal());
	threadPanelSizer->Add(addCardToThread, wxSizerFlags(0).Expand().Border(wxALL, 5));

	m_threadPanel->SetSizer(threadPanelSizer);

	m_cardPanel = new wxScrolledWindow(m_content);

	wxStaticText* curCardLabel = new wxStaticText(m_cardPanel, -1, "Currently selected:");
	curCardLabel->SetForegroundColour(wxColour(255, 255, 255));
	curCardLabel->SetFont(wxFontInfo(12).Bold());

	m_cardThumbnail = new amTLCardThumbnail(m_cardPanel);
	m_cardThumbnail->SetCanvas(canvas);

	wxButton* addCardBefore = new wxButton(m_cardPanel, BUTTON_AddCardBefore, "Add before");
	wxButton* addCardAfter = new wxButton(m_cardPanel, BUTTON_AddCardAfter, "Add after");

	wxBoxSizer* addCardsSizer = new wxBoxSizer(wxHORIZONTAL);
	addCardsSizer->Add(addCardBefore, wxSizerFlags(0).Expand());
	addCardsSizer->AddStretchSpacer(1);
	addCardsSizer->Add(addCardAfter, wxSizerFlags(0).Expand());

	wxBoxSizer* cardPanelSizer = new wxBoxSizer(wxVERTICAL);
	cardPanelSizer->Add(curCardLabel, wxSizerFlags(0).Left().Border(wxLEFT, 5));
	cardPanelSizer->Add(m_cardThumbnail, wxSizerFlags(0).Expand().Border(wxALL, 5));
	cardPanelSizer->Add(addCardsSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 5));

	m_cardPanel->SetSizer(cardPanelSizer);

	m_sectionPanel = new wxScrolledWindow(m_content);

	wxButton* addSection = new wxButton(m_sectionPanel, -1, "", wxDefaultPosition, FromDIP(wxSize(25,25)));
	addSection->SetBitmap(wxBITMAP_PNG(plus));

	wxStaticText* curSectionLabel = new wxStaticText(m_sectionPanel, -1, "Currently selected:");
	curSectionLabel->SetForegroundColour(wxColour(255, 255, 255));
	curSectionLabel->SetFont(wxFontInfo(12).Bold());

	m_sectionThumbnail = new amTLSectionThumbnail(m_sectionPanel);
	m_sectionThumbnail->SetCanvas(canvas);

	m_curSectionName = new wxStaticText(m_sectionPanel, -1, "");
	m_curSectionName->SetBackgroundColour(wxColour(10, 10, 10));
	m_curSectionName->SetForegroundColour(wxColour(255, 255, 255));
	m_curSectionName->SetFont(wxFontInfo(13).Bold());

	wxButton* addCardToSection = new wxButton(m_sectionPanel, BUTTON_AddCardToSection, "Add card");

	wxBoxSizer* sectionPanelSizer = new wxBoxSizer(wxVERTICAL);
	sectionPanelSizer->Add(addSection, wxSizerFlags(0).Left().Border(wxALL, 5));
	sectionPanelSizer->AddSpacer(10);
	sectionPanelSizer->Add(curSectionLabel, wxSizerFlags(0).Left().Border(wxLEFT, 5));
	sectionPanelSizer->Add(m_sectionThumbnail, wxSizerFlags(0).Expand().Border(wxALL, 5));
	sectionPanelSizer->Add(m_curSectionName, wxSizerFlags(0).CenterHorizontal());
	sectionPanelSizer->Add(addCardToSection, wxSizerFlags(0).Expand().Border(wxALL, 5));

	m_sectionPanel->SetSizer(sectionPanelSizer);
		
	m_content->AddPage(m_threadPanel, "Threads");
	m_content->AddPage(m_cardPanel, "Cards");
	m_content->AddPage(m_sectionPanel, "Sections");

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_content, wxSizerFlags(1).Expand().Border(wxTOP, 40));

	SetSizer(sizer);
	m_content->Hide();

	Bind(wxEVT_PAINT, &amTLTimelineSidebar::OnPaint, this);
	Bind(wxEVT_MOTION, &amTLTimelineSidebar::OnMouseMove, this);
	Bind(wxEVT_LEFT_DOWN, &amTLTimelineSidebar::OnLeftDown, this);
}

void amTLTimelineSidebar::ShowContent(bool show) {
	m_content->Show(show);
}

void amTLTimelineSidebar::OnAddCardToThread(wxCommandEvent& event) {
	amTLThread* thread = m_threadThumbnail->GetThread();

	if (thread)
		m_parent->AddCardToThread(thread);
}

void amTLTimelineSidebar::OnAddCardToSection(wxCommandEvent& event) {
	amTLSection* section = m_sectionThumbnail->GetSection();

	if (section)
		m_parent->AddCardToSection(section);
}

void amTLTimelineSidebar::OnAddCardBefore(wxCommandEvent& event) {
	TimelineCard* card = m_cardThumbnail->GetCard();

	if (card)
		m_parent->AddCardBefore(card);
}

void amTLTimelineSidebar::OnAddCardAfter(wxCommandEvent& event) {
	TimelineCard* card = m_cardThumbnail->GetCard();

	if (card)
		m_parent->AddCardAfter(card);
}

void amTLTimelineSidebar::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);

	dc.SetPen(wxPen(wxColour(140, 140, 140), 4));

	int x1 = m_pullRect.x, x2 = m_pullRect.x + m_pullRect.width - 2;
	int y = m_pullRect.height / 4 + 2;

	dc.DrawLine(wxPoint(x1, y), wxPoint(x2, y));
	dc.DrawLine(wxPoint(x1, y * 2), wxPoint(x2, y * 2));
	dc.DrawLine(wxPoint(x1, y * 3), wxPoint(x2, y * 3));
}

void amTLTimelineSidebar::OnMouseMove(wxMouseEvent& event) {
	if (m_pullRect.Contains(event.GetPosition()))
		wxSetCursor(wxCURSOR_CLOSED_HAND);
	else
		wxSetCursor(wxCURSOR_DEFAULT);
}

void amTLTimelineSidebar::OnLeftDown(wxMouseEvent& event) {
	if (m_pullRect.Contains(event.GetPosition()))
		m_parent->ShowSidebar();
}

void amTLTimelineSidebar::SetThreadData(amTLThread* thread, ShapeList& shapes) {
	if (!m_threadThumbnail)
		return;

	m_threadThumbnail->SetData(thread, shapes);

	if (thread)
		m_curThreadName->SetLabel(thread->GetCharacter());
}

void amTLTimelineSidebar::SetSectionData(amTLSection* section, ShapeList& shapes) {
	if (!m_sectionThumbnail)
		return;

	m_sectionThumbnail->SetData(section, shapes);

	if (section)
		m_curSectionName->SetLabel(section->GetTitle());
}

void amTLTimelineSidebar::SetCardData(TimelineCard* card) {
	if (!m_cardThumbnail)
		return;

	m_cardThumbnail->SetData(card);
}


////////////////////////////////////////////////////////////////////
////////////////////////// ThreadThumbnails ////////////////////////
////////////////////////////////////////////////////////////////////


amTLThumbnail::amTLThumbnail(wxWindow* parent) : wxSFThumbnail(parent) {
	Bind(wxEVT_PAINT, &amTLThumbnail::OnPaint, this);
	SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void amTLThumbnail::OnPaint(wxPaintEvent& event) {
	wxBufferedPaintDC dc(this);

	// clear background
	dc.SetBackground(wxBrush(wxColour(150, 150, 150)));
	dc.Clear();

	if (m_pCanvas) {
		wxSize szCanvas = m_pCanvas->GetSize();
		wxSize szGoodCanvas = ((amTLTimelineCanvas*)m_pCanvas)->GetGoodSize();
		wxSize szCanvasOffset = GetCanvasOffset();
		wxSize szThumb = GetClientSize();

		// scale and copy bitmap to DC
		double cgx = szGoodCanvas.x, tx = szThumb.x;
		double cx = szCanvas.x, cy = szCanvas.y;

		m_nScale = tx / cgx;
		double clientScale = tx / (cgx * m_pCanvas->GetScale());

		float ratio = (float)szThumb.x / szThumb.y;
		float canvasRatio = (float)szGoodCanvas.x / szGoodCanvas.y;

		if (canvasRatio != ratio) {
			SetMinSize(wxSize(szThumb.x, szThumb.x / canvasRatio));
			GetParent()->Layout();
		}
		
		// draw virtual canvas area
		dc.SetPen(*wxWHITE_PEN);
		dc.SetBrush(wxBrush(wxColour(40, 40, 40)));
		dc.DrawRectangle(0, 0, double(szGoodCanvas.x) * m_nScale, double(szGoodCanvas.y) * m_nScale);

		wxSFScaledDC sdc((wxWindowDC*)&dc, m_nScale);
		// draw top level shapes
		this->DrawContent(sdc);

		// draw canvas client area
		dc.SetPen(*wxRED_PEN);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle(double(szCanvasOffset.x) * clientScale, double(szCanvasOffset.y) * clientScale,
			cx * clientScale, cy * clientScale);

		dc.SetBrush(wxNullBrush);
		dc.SetPen(wxNullPen);
	}

	dc.SetBackground(wxNullBrush);
}

amTLThreadThumbnail::amTLThreadThumbnail(wxWindow* parent) : amTLThumbnail(parent) { }

void amTLThreadThumbnail::DrawContent(wxDC& dc) {
	if (!m_thread)
		return;

	m_thread->DrawNormal(dc);

	ShapeList::compatibility_iterator node = m_shapes.GetFirst();
	wxSFShapeBase* pShape;

	while (node) 	{
		pShape = node->GetData();

		if (pShape) {
			if (m_nThumbStyle & tsSHOW_ELEMENTS)
				pShape->Draw(dc, sfWITHOUTCHILDREN);
		}
		node = node->GetNext();
	}
}

void amTLThreadThumbnail::SetData(amTLThread* thread, ShapeList& shapes) {
	m_thread = thread;
	m_shapes = shapes;
}

amTLCardThumbnail::amTLCardThumbnail(wxWindow* parent) : wxSFThumbnail(parent) {
	Bind(wxEVT_PAINT, &amTLCardThumbnail::OnPaint, this);
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetBackgroundColour(wxColour(40, 40, 40));
}

void amTLCardThumbnail::DrawContent(wxDC& dc) {
	wxSize szThumb = GetClientSize();

	int cardX = TimelineCard::GetWidth(), cardY = TimelineCard::GetHeight();
	double scale = (double)szThumb.x / (double)cardX;

	int y = scale * cardY;

	if (szThumb.y != y) {
		SetMinSize(wxSize(szThumb.x, y));
		GetParent()->Layout();
	}

	if (m_card) {
		// draw top level shapes
		m_card->Draw(dc);
	}
}

void amTLCardThumbnail::OnPaint(wxPaintEvent& event) {
	wxAutoBufferedPaintDC dc(this);
	wxSize szThumb = GetClientSize();

	dc.Clear();

	int cardX = TimelineCard::GetWidth(), cardY = TimelineCard::GetHeight();
	double scale = (double)szThumb.x / (double)cardX;

	int y = scale * cardY;

	if (szThumb.y != y) {
		SetMinSize(wxSize(szThumb.x, y));
		GetParent()->Layout();
	}

	if (m_card) {
		wxGCDC gdc(dc);
		wxGraphicsContext* pGC = gdc.GetGraphicsContext();
		pGC->Scale(scale, scale);

		m_card->DrawOnOrigin(gdc, sfWITHCHILDREN);
	}
}

amTLSectionThumbnail::amTLSectionThumbnail(wxWindow* parent) : amTLThumbnail(parent) {}

void amTLSectionThumbnail::SetData(amTLSection* section, ShapeList& shapes) {
	m_section = section;
	m_shapes = shapes;
}

void amTLSectionThumbnail::DrawContent(wxDC& dc) {
	if (!m_section)
		return;

	m_section->DrawNormal(dc, false);

	ShapeList::compatibility_iterator node = m_shapes.GetFirst();
	wxSFShapeBase* pShape;

	while (node) {
		pShape = node->GetData();

		if (pShape) {
			if (m_nThumbStyle & tsSHOW_ELEMENTS)
				pShape->Draw(dc, sfWITHOUTCHILDREN);
		}
		node = node->GetNext();
	}
}
