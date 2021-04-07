#include "MyApp.h"
#include "Document.h"
#include "Timeline.h"
#include "TimelineDialogs.h"
#include "MainFrame.h"

#include <wx\sstream.h>
#include <wx\dcbuffer.h>
#include <wx\dcgraph.h>
#include <wx\spinctrl.h>

#include "wxmemdbg.h"


///////////////////////////////////////////////////////////////
////////////////////////// amTLThread /////////////////////////
///////////////////////////////////////////////////////////////


int amTLThread::m_height = 10;
int amTLThread::m_width = 1000;
int amTLThread::m_titleOffset = 300;
wxPen amTLThread::m_hoverPen{ wxColour(160,160,245), 4 };
wxPen amTLThread::m_selectedPen{ wxColour(110,110,200), 5 };

void amTLThread::Draw(wxDC& dc, float titleOffset) {
	if (m_isSelected && m_drawSelected)
		DrawSelected(dc);
	else if (m_isHovering)
		DrawHover(dc);
	else
		DrawNormal(dc);

	DrawTitle(dc, titleOffset);
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

void amTLThread::DrawTitle(wxDC& dc, float titleOffset) {
	dc.SetTextForeground(m_colour);
	dc.SetFont(m_titleFont);
	dc.DrawText(m_titleToDraw, wxPoint(titleOffset, m_y - m_titleHeight));
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

void amTLThread::CalculateTitleWrap() {
		if (m_character.IsEmpty()) {
			m_titleToDraw.Empty();
			return;
		}

		m_titleToDraw = m_character;

		wxClientDC dc(m_canvas);
		wxGraphicsContext* pGC = wxGraphicsContext::Create(dc);
		pGC->SetFont(m_titleFont, wxColour(0, 0, 0));

		int index = 0;
		int strLen = m_titleToDraw.Length();

		double wd = -1, hd = -1;

		int maxWidth = m_titleOffset - 10;
		bool hit = false;

		while (index <= strLen + 1) {
			index += 2;

			if (index == strLen)
				index = strLen;

			pGC->GetTextExtent(m_titleToDraw.SubString(0, index), &wd, &hd);

			if (wd >= maxWidth) {
				m_titleToDraw.Remove(index - 3);
				m_titleToDraw << "...";
				m_titleHeight = hd + 10;
				hit = true;
				break;
			}
			m_titleHeight = hd + 10;
		}

		if (pGC)
			delete pGC;
}

void amTLThread::RecalculatePosition() {
	int cardVerSpacing = TimelineCard::GetVerticalSpacing();
	int cardHeight = TimelineCard::GetHeight();
	int threadHeight = amTLThread::GetHeight();
	int sectionTitleOffset = amTLSection::GetTitleOffset();

	m_y = ((cardVerSpacing + cardHeight) * (m_index + 1))
		- (cardHeight / 2)
		- (threadHeight / 2)
		+ sectionTitleOffset;

	m_boundingRect = { 0,
		m_y + (threadHeight / 2) - (cardHeight / 2) - (cardVerSpacing / 2),
		m_width,
		cardVerSpacing + cardHeight };
}


///////////////////////////////////////////////////////////////
//////////////////////// TimelineSection //////////////////////
///////////////////////////////////////////////////////////////


int amTLSection::m_horSpacing = 400;
int amTLSection::m_markerWidth = 20;
int amTLSection::m_titleOffset = 200;

wxPen amTLSection::m_border{ wxColour(0,0,0), 4 };
wxPen amTLSection::m_separatorPen{ wxColour(130, 130, 130), 2, wxPENSTYLE_LONG_DASH };

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

void amTLSection::DrawNormal(wxDC& dc, float curCanvasLeft, bool drawSeparators) {
	dc.SetPen(m_border);
	dc.SetBrush(wxBrush(m_colour));

	dc.DrawRectangle(m_insideRect.GetTopLeft() - wxPoint(m_markerWidth, 0), wxSize(m_markerWidth, m_insideRect.height + 10));
	dc.DrawRectangle(m_insideRect.GetTopRight(), wxSize(m_markerWidth, m_insideRect.height + 10));

	if (!m_isEmpty) {
		dc.SetFont(m_titleFont);

		dc.SetBrush({ wxColour(0,0,0) });
		dc.SetPen(wxNullPen);
		dc.DrawRectangle(wxPoint(m_insideRect.GetLeft(), 30), wxSize(m_insideRect.GetWidth(), m_titleHeight));

		dc.SetTextForeground({ 255,255,255 });
		int textPos = m_insideRect.GetLeft() + 20;

		if (curCanvasLeft > textPos) {
			if (curCanvasLeft + m_titleWidth >= m_insideRect.GetRight())
				dc.DrawText(m_titleToDraw, wxPoint(m_insideRect.GetRight() - m_titleWidth, 40));
			else
				dc.DrawText(m_titleToDraw, wxPoint(curCanvasLeft, 40));
		} else
			dc.DrawText(m_titleToDraw, wxPoint(textPos, 40));
	}
	
	if (drawSeparators) {
		dc.SetPen(m_separatorPen);
	
		for (int& it : m_separators)
			dc.DrawLine(wxPoint(it, m_titleOffset), wxPoint(it, m_insideRect.height));
	
		int height2 = m_insideRect.GetHeight() + 10;

		dc.DrawLine(wxPoint(m_insideRect.GetLeft(), m_titleOffset), wxPoint(m_insideRect.GetRight(), m_titleOffset));
		dc.DrawLine(m_insideRect.GetBottomLeft(), m_insideRect.GetBottomRight());
		dc.DrawLine(wxPoint(m_insideRect.GetLeft(), height2),
			wxPoint(m_insideRect.GetRight(), height2));
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
		m_titleHeight = hd + 20;
		m_titleWidth = wd;

		if (wd >= maxWidth) {
			m_titleToDraw.Remove(index - 3);
			m_titleToDraw << "...";
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

	CalculateTitleWrap();
}


///////////////////////////////////////////////////////////////
//////////////////////// TimelineCanvas ///////////////////////
///////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(amTLTimelineCanvas, amSFShapeCanvas)

EVT_MENU(MENU_DeleteTimelineCard, amTLTimelineCanvas::OnDeleteCard)
EVT_MENU(MENU_DeleteTimelineThread, amTLTimelineCanvas::OnDeleteThread)
EVT_MENU(MENU_DeleteTimelineSection, amTLTimelineCanvas::OnDeleteSection)

END_EVENT_TABLE()

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
	EnableScrolling(false, true);

	m_threadSelectionTimer.Bind(wxEVT_TIMER, &amTLTimelineCanvas::OnThreadSelectionTimer, this);
	Bind(wxEVT_LEAVE_WINDOW, &amTLTimelineCanvas::OnLeaveWindow, this);

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

void amTLTimelineCanvas::AddThread(int index, const wxString& character, const wxColour& colour, bool refresh) {
	if (index > m_threads.size())
		index = m_threads.size();

	amTLThread* newThread = new amTLThread(character, index, -1, colour, this);
	m_threads.push_back(newThread);

	if (refresh) {
		RepositionThreads();
		Refresh(true);
		
		m_selectedThread = newThread;
		OnThreadSelected(newThread);
	}
}

TimelineCard* amTLTimelineCanvas::AddCard(int rowIndex, int colIndex, int sectionIndex) {
	if (!CanAddCards()) {
		wxMessageBox("You need at least one thread and one section to be able to place cards on the Timeline!");
		return nullptr;
	}
	
	TimelineCard* card = (TimelineCard*)GetDiagramManager()->AddShape(CLASSINFO(TimelineCard), wxPoint());
	if (sectionIndex == -1)
		sectionIndex = m_sections.size() - 1;

	SetCardToRow(rowIndex, card);
	SetCardToSection(sectionIndex, card);
	SetCardToColumn(colIndex, card);
	
	RepositionThreads();
	UpdateSelectedThreadSBData();
	UpdateSelectedSectionSBData();
	SaveCanvasState();
	return card;
}

TimelineCard* amTLTimelineCanvas::AppendCard(int rowIndex, int sectionIndex) {
	if (!CanAddCards()) {
		wxMessageBox("You need at least one thread and one section to be able to place cards on the Timeline!");
		return nullptr;
	}
	
	TimelineCard* card = (TimelineCard*)GetDiagramManager()->AddShape(CLASSINFO(TimelineCard), wxPoint());
	if (sectionIndex == -1)
		sectionIndex = m_sections.size() - 1;

	SetCardToRow(rowIndex, card);
	SetCardToSection(sectionIndex, card);
	SetCardToColumn(GetSection(sectionIndex)->GetLast(), card);

	RepositionThreads();
	UpdateSelectedThreadSBData();
	UpdateSelectedSectionSBData(); 
	SaveCanvasState();
	return card;
}

void amTLTimelineCanvas::AppendSection(const wxString& title, const wxColour& colour) {
	ShapeList shapes;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), shapes);

	amTLSection* section = new amTLSection(m_sections.size(), shapes.size(), shapes.size() - 1, this, title, colour);
	m_sections.push_back(section);

	section->RecalculatePosition();
	m_selectedSection = section;
	UpdateSelectedSectionSBData();
	Refresh();
}

/// <summary>
/// 
/// </summary>
/// <param name="section"></param>
/// <param name="shape"></param>
/// <returns>std::pair, in which first is section's first card and second is section's last card.</returns>
pair<int, int> amTLTimelineCanvas::SetCardToSection(int section, TimelineCard* shape) {
	if (m_sections.empty())
		return pair<int, int>(-1, -1);

	int prevSection = shape->GetSectionIndex();
	shape->SetSection(section);
	
	if (prevSection == section)
		return pair<int, int>(0, 0);

	if (prevSection == -1)
		prevSection = m_sections.size();

	amTLSection* focusSection = nullptr;

	if (prevSection > section) {
		for (amTLSection*& sectionIt : m_sections) {
			int sectionItIndex = sectionIt->GetIndex();

			if (sectionItIndex > section && sectionItIndex < prevSection) {
				sectionIt->ShiftRight();
			} else if (sectionItIndex == prevSection) {
				if (prevSection < m_sections.size())
					sectionIt->RemoveCardFromBeggining();
			} else if (sectionItIndex == section) {
				sectionIt->AppendCard();
				focusSection = sectionIt;
			}
		}
	} else {
		for (amTLSection*& sectionIt : m_sections) {
			int sectionItIndex = sectionIt->GetIndex();

			if (sectionItIndex > prevSection && sectionItIndex < section) {
				sectionIt->ShiftLeft();
			} else if (sectionItIndex == prevSection) {
				if (prevSection < m_sections.size())
					sectionIt->RemoveCardFromEnd();
			} else if (sectionItIndex == section) {
				sectionIt->PrependCard();
				focusSection = sectionIt;
			}
		}
	}

	if (focusSection)
		return pair<int, int>(focusSection->GetFirst(), focusSection->GetLast());
	else
		return pair<int, int>(-1, -1);
}

bool amTLTimelineCanvas::SetCardToColumn(int column, TimelineCard* shape) {
	ShapeList list;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), list);

	TimelineCard* card = shape;
	int prevCol = card->GetColumn();

	if (prevCol == column)
		return false;

	if (prevCol == -1)
		prevCol = 9999;

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
	if (shape->GetThreadIndex() == row)
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

			if (cell.y != m_curDragCell.yCell || m_isDragOnEmptySection) {
				m_curDragCell.yCell = cell.y;
				m_curDragCell.pos.y = (TimelineCard::GetVerticalSpacing() * (cell.y + 1)) +
					(TimelineCard::GetHeight() * cell.y) + amTLSection::GetTitleOffset();
				changed = true;
			}

			if (cell.x != m_curDragCell.xCell || m_isDragOnEmptySection) {
				int cardHorSpacing = TimelineCard::GetHorizontalSpacing();
				int sectionMarkerWidth = amTLSection::GetMarkerWidth();

				m_curDragCell.xCell = cell.x;
				m_curDragCell.pos.x = ((TimelineCard::GetWidth() + cardHorSpacing) * cell.x) 
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
							m_curDragCell.yCell = j;
							m_curDragCell.pos.y = m_threads[j]->GetRect().GetTop();
							break;
						}
					}

					m_curDragCell.xCell = m_sections[i]->GetFirst();
					m_curDragCell.pos.x = m_sections[i]->GetRect().GetLeft();

					changed = true;
					
					m_isDragOnEmptySection = true;
					break;
				}
			}
		}
	}

	return changed;
}

void amTLTimelineCanvas::MoveThread(amTLThread* thread, bool moveUp, bool repositionAll) {
	if (moveUp) {
		int previousIndex = thread->GetIndex();
		if (previousIndex <= 0)
			return;

		ShapeList cards;
		GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), cards);

		ShapeList::compatibility_iterator node = cards.GetFirst();
		TimelineCard* card = nullptr;

		while (node) {
			card = (TimelineCard*)node->GetData();
			int prevCardRow = card->GetThreadIndex();

			if (prevCardRow == previousIndex) {
				card->SetRow(prevCardRow - 1);
				card->RecalculatePosition();
			} else if (prevCardRow == previousIndex - 1) {
				card->SetRow(prevCardRow + 1);
				card->RecalculatePosition();
			}

			node = node->GetNext();
		}

		for (amTLThread*& otherThread : m_threads) {
			int prevOtherThreadIndex = otherThread->GetIndex();

			if (prevOtherThreadIndex == previousIndex - 1) {
				otherThread->SetIndex(prevOtherThreadIndex + 1);
				otherThread->RecalculatePosition();
				break;
			}
		}

		thread->SetIndex(previousIndex - 1);
	} else {
		int previousIndex = thread->GetIndex();
		if (previousIndex >= m_threads.size() - 1)
			return;

		ShapeList cards;
		GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), cards);

		ShapeList::compatibility_iterator node = cards.GetFirst();
		TimelineCard* card = nullptr;

		while (node) {
			card = (TimelineCard*)node->GetData();
			int prevCardRow = card->GetThreadIndex();

			if (prevCardRow == previousIndex) {
				card->SetRow(prevCardRow + 1);
				card->RecalculatePosition();
			} else if (prevCardRow == previousIndex + 1) {
				card->SetRow(prevCardRow - 1);
				card->RecalculatePosition();
			}

			node = node->GetNext();
		}

		for (amTLThread*& otherThread : m_threads) {
			int prevOtherThreadIndex = otherThread->GetIndex();

			if (prevOtherThreadIndex == previousIndex + 1) {
				otherThread->SetIndex(prevOtherThreadIndex - 1);
				otherThread->RecalculatePosition();
				break;
			}
		}

		thread->SetIndex(previousIndex + 1);
	}
	thread->RecalculatePosition();
}

void amTLTimelineCanvas::OnDeleteCard(wxCommandEvent& event) {
	wxMessageDialog message(amGetManager()->GetMainFrame(), _("Are you sure you want to delete this card?"
		" This action cannot be undone!"), _("Confirmation"),
		wxYES_NO | wxCANCEL | wxICON_WARNING);

	switch (message.ShowModal()) {
	case wxID_OK:
	case wxID_YES:
		DeleteSelectedCard();
		break;

	default:
		break;
	}
}

void amTLTimelineCanvas::OnDeleteThread(wxCommandEvent& event) {
	wxMessageDialog message(amGetManager()->GetMainFrame(), _("Are you sure you want to delete this thread?"
		" This action cannot be undone!"), _("Confirmation"),
		wxYES_NO | wxCANCEL | wxICON_WARNING);

	switch (message.ShowModal()) {
	case wxID_OK:
	case wxID_YES:
		DeleteSelectedThread();
		break;

	default:
		break;
	}
}

void amTLTimelineCanvas::OnDeleteSection(wxCommandEvent& event) {
	wxMessageDialog message(amGetManager()->GetMainFrame(), _("Are you sure you want to delete this section?"
		" This action cannot be undone!"), _("Confirmation"),
		wxYES_NO | wxCANCEL | wxICON_WARNING);

	switch (message.ShowModal()) {
	case wxID_OK:
	case wxID_YES:
		DeleteSelectedSection();
		break;

	default:
		break;
	}
}

void amTLTimelineCanvas::DeleteCard(TimelineCard* card, bool refresh, bool UpdateThumbnails) {
	if (!card)
		return;

	bool isSelected = card == m_selectedCard;

	SetCardToColumn(9999, card);
	SetCardToSection(9999, card);

	GetDiagramManager()->RemoveShape(card, refresh);

	if (isSelected) {
		m_selectedCard = nullptr;
		m_parent->SetCardData(m_selectedCard);
	}

	if (UpdateThumbnails) {
		UpdateSelectedThreadSBData();
		UpdateSelectedSectionSBData();
	}
}

void amTLTimelineCanvas::DeleteSelectedThread() {
	if (!m_selectedThread)
		return;

	ShapeList cards;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), cards);
	ShapeList::compatibility_iterator node = cards.GetFirst();

	int threadIndex = m_selectedThread->GetIndex();
	TimelineCard* card = nullptr;

	while (node) {
		card = (TimelineCard*)node->GetData();
		if (card->GetThreadIndex() == threadIndex)
			DeleteCard(card, false, false);

		node = node->GetNext();
	}
	
	for (amTLThread*& thread : m_threads) {
		if (thread == m_selectedThread)
			m_threads.erase(&thread);
	}

	int newBottom = 0;
	int curBottom = 0;
	for (amTLThread*& thread : m_threads) {
		if (thread->GetIndex() > threadIndex)
			MoveThread(thread, true);

		curBottom = thread->GetRect().GetBottom();
		if (curBottom > newBottom)
			newBottom = curBottom;
	}

	SetBottom(newBottom);
	for (amTLSection*& section : m_sections)
		section->SetHeight(newBottom);

	delete m_selectedThread;
	m_selectedThread = nullptr;
	
	UpdateSelectedThreadSBData();
	UpdateSelectedSectionSBData();

	Refresh(true);
}

void amTLTimelineCanvas::DeleteSelectedSection() {
	if (!m_selectedSection) 
		return;

	ShapeList cards;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), cards);
	ShapeList::compatibility_iterator node = cards.GetFirst();

	int sectionIndex = m_selectedSection->GetIndex();
	TimelineCard* card = nullptr;

	int curCardSectionIndex = -1;
	while (node) {
		card = (TimelineCard*)node->GetData();
		curCardSectionIndex = card->GetSectionIndex();

		if (curCardSectionIndex == sectionIndex)
			DeleteCard(card, false, false);
		else if (curCardSectionIndex > sectionIndex) {
			card->SetSection(curCardSectionIndex - 1);
			card->RecalculatePosition();
		}

		node = node->GetNext();
	}

	for (amTLSection*& section : m_sections) {
		if (section == m_selectedSection) {
			m_sections.erase(&section);

			delete m_selectedSection;
			m_selectedSection = nullptr;
			break;
		}
	}

	int curSectionIndex = 0;
	for (amTLSection*& section : m_sections) {
		curSectionIndex = section->GetIndex();
		if (curSectionIndex > sectionIndex) {
			section->SetIndex(curSectionIndex - 1);
			section->RecalculatePosition();
		}
	}

	UpdateSelectedThreadSBData();
	UpdateSelectedSectionSBData();

	Refresh();
}

wxColour amTLTimelineCanvas::GetThreadColour(int threadIndex) {
	if (threadIndex >= m_threads.size() || threadIndex < 0)
		return wxColour(255, 255, 255);

	for (amTLThread*& thread : m_threads) {
		if (thread->GetIndex() == threadIndex)
			return thread->GetColour();
	}
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
				if (card->GetThreadIndex() != m_selectedThread->GetIndex())
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

amTLSection* amTLTimelineCanvas::GetSection(int index) {
	for (amTLSection*& section : m_sections) {
		if (section->GetIndex() == index)
			return section;
	}

	return nullptr;
}

void amTLTimelineCanvas::ResetCardColours() {
	ShapeList cards;
	GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), cards);

	ShapeList::compatibility_iterator node = cards.GetFirst();
	TimelineCard* card = nullptr;
	wxColour textColour(0, 0, 0);
	if (m_propagateColour) {
		while (node) {
			card = (TimelineCard*)node->GetData();
			card->SetTextColour(textColour);
			for (amTLThread*& thread : m_threads) {
				if (thread->GetIndex() == card->GetThreadIndex())
					card->SetColour(thread->GetColour());
			}

			node = node->GetNext();
		}
	} else {
		wxColour colour(40, 40, 40);
		textColour = { 255,255,255 };
		while (node) {
			card = (TimelineCard*)node->GetData();
			card->SetColour(colour);
			card->SetTextColour(textColour);
			node = node->GetNext();
		}
	}
}

void amTLTimelineCanvas::DrawBackground(wxDC& dc, bool fromPaint) {
	amSFShapeCanvas::DrawBackground(dc, fromPaint);

	int xs, ys;
	CalcUnscrolledPosition(0, 0, &xs, &ys);

	float xo = (float)xs / GetScale();
	m_curLeft = xo;

	for (amTLThread*& thread : m_threads)
		thread->Draw(dc, xo);
}

void amTLTimelineCanvas::DrawForeground(wxDC& dc, bool fromPaint) {
	for (amTLSection*& section : m_sections)
		section->DrawNormal(dc, m_curLeft, m_drawSeparators);

	dc.SetPen(wxPen(wxColour(180, 180, 255, 128), 2));
	dc.SetBrush(wxBrush(wxColour(200, 200, 255, 128)));

	if (m_nWorkingMode == modeSHAPEMOVE && m_curDragCell.xCell != -1) {
		dc.DrawRectangle(wxRect(m_curDragCell.pos, wxSize(TimelineCard::GetWidth(), TimelineCard::GetHeight())));
	}

	dc.SetPen(wxNullPen);
	dc.SetBrush(wxNullBrush);
}

void amTLTimelineCanvas::OnUpdateVirtualSize(wxRect& rect) {
	amTLThread::SetWidth((double)GetVirtualSize().x / m_Settings.m_nScale);

	if (!m_sections.empty())
		rect.SetSize(GetGoodSize());

}

void amTLTimelineCanvas::OnMouseMove(wxMouseEvent& event) {
	wxPoint pos = DP2LP(event.GetPosition());

	bool isRefreshed = false;

	if (m_nWorkingMode == modeSHAPEMOVE) {
		if (CalculateCellDrag(pos)) {
			Refresh(false);
			isRefreshed = true;
		}
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
	if (!isRefreshed && m_isDraggingRight) {
		InvalidateVisibleRect();
	}
}

void amTLTimelineCanvas::OnLeftDown(wxMouseEvent& event) {
	wxPoint pos = DP2LP(event.GetPosition());
	wxSFShapeBase* pShape = GetShapeUnderCursor();

	if (pShape) {
		CalculateCellDrag(pos);
		
		TimelineCard* card = dynamic_cast<TimelineCard*>(pShape->GetGrandParentShape());
		m_selectedCard = card;
	} else {
		bool hitThread = false;
		for (amTLThread*& thread : m_threads) {
			if (thread->Contains(pos)) {
				SelectThread(thread, pos);
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
			OnThreadUnselected(m_selectedThread);
			m_selectedThread = nullptr;
			UpdateSelectedThreadSBData();
		}

		if (!hitSection && m_selectedSection) {
			m_selectedSection = nullptr;
			UpdateSelectedSectionSBData();
		}

		m_selectedCard = nullptr;
	}
	m_parent->SetCardData(m_selectedCard);

	if (!wxGetKeyState(WXK_COMMAND))
		amSFShapeCanvas::OnLeftDown(event);
}

void amTLTimelineCanvas::OnLeftUp(wxMouseEvent& event) {
	if (m_nWorkingMode == modeSHAPEMOVE) {
		ShapeList sel;
		GetSelectedShapes(sel);

		TimelineCard* pSel = nullptr;
		if (!sel.IsEmpty()) {
			pSel = (TimelineCard*)sel[0];
			SetCardToRow(m_curDragCell.yCell, pSel);

			int prevSection = pSel->GetSectionIndex();
			int column = m_curDragCell.xCell;

			pair<int, int> sectionSpan = SetCardToSection(m_curDragSection, pSel);

			int curSection = pSel->GetSectionIndex();

			if (prevSection > curSection && column + 1 == sectionSpan.second)
				column += 1;
			else if (curSection > prevSection && column - 1 == sectionSpan.first)
				column -= 1;
			
			if (!SetCardToColumn(column, pSel))
				pSel->RecalculatePosition();

			m_curDragCell.xCell = -1;
			m_curDragCell.yCell = -1;
			m_curDragCell.pos = { -1,-1 };

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

void amTLTimelineCanvas::OnRightUp(wxMouseEvent& event) {
	bool isDraggingRightCache = m_isDraggingRight;
	amSFShapeCanvas::OnRightUp(event);

	if (!isDraggingRightCache) {
		bool doPopup = false;
		wxMenu menu;

		wxSFShapeBase* shape = GetShapeUnderCursor();
		if (shape) {
			TimelineCard* card = dynamic_cast<TimelineCard*>(shape->GetGrandParentShape());
			if (card) {
				m_selectedCard = card;
				m_parent->SetCardData(card);

				menu.Append(MENU_DeleteTimelineCard, "Delete card");
				doPopup = true;
			}
		} else {
			wxPoint pos = DP2LP(event.GetPosition());
			
			for (amTLThread*& thread : m_threads) {
				if (thread->Contains(pos)) {
					SelectThread(thread, pos);
					menu.Append(MENU_DeleteTimelineThread, "Delete thread '" + thread->GetCharacter() + "'");
					doPopup = true;
					break;
				}
			}

			for (amTLSection*& section : m_sections) {
				if (section->EmptyContains(pos)) {
					m_selectedSection = section;
					UpdateSelectedSectionSBData();
					menu.Append(MENU_DeleteTimelineSection, "Delete section '" + section->GetTitle() + "'");
					doPopup = true;
					break;
				}
			}
		}

		if (doPopup)
			PopupMenu(&menu);
	}
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

void amTLTimelineCanvas::OnLeaveWindow(wxMouseEvent& event) {
	if (m_threadUnderMouse) {
		m_threadUnderMouse->OnMouseLeave();
		m_threadUnderMouse = nullptr;
		RefreshInvalidatedRect();
	}
}

wxSize amTLTimelineCanvas::GetGoodSize() {
	if (!m_sections.empty())
		return wxSize(m_sections.back()->GetRight() + 300, GetBottom() + 300);
	else
		return wxSize(500, 250);
}

void amTLTimelineCanvas::RepositionThreads() {
	amTLThread::SetWidth((double)GetVirtualSize().x / GetScale());

	int bottom = -1;
	for (amTLThread*& thread : m_threads) {
		thread->RecalculatePosition();

		int current = thread->GetRect().GetBottom();
		if (current > bottom)
			bottom = current;
	}

	SetBottom(bottom);
	for (amTLSection*& section : m_sections)
		section->SetHeight(bottom);
}

void amTLTimelineCanvas::SelectThread(amTLThread* thread, const wxPoint& pos) {
	if (m_selectedThread && m_selectedThread != thread) {
		m_selectedThread->KillFocus();
		OnThreadUnselected(m_selectedThread);
	}

	thread->OnLeftDown(pos);
	m_selectedThread = thread;
	OnThreadSelected(thread);
}


////////////////////////////////////////////////////////////////////
//////////////////////////// Timeline //////////////////////////////
////////////////////////////////////////////////////////////////////


amTLTimeline::amTLTimeline(wxWindow* parent) : amSplitterWindow(parent) {
	m_parent = (amOutline*)(parent->GetParent());
	m_manager = amGetManager();
	
	m_canvas = new amTLTimelineCanvas(&m_canvasManager, this);
	m_sidebar = new amTLTimelineSidebar(this, m_canvas);

	SetMinimumPaneSize(40);
	SplitVertically(m_canvas, m_sidebar, 99999);
	SetSashGravity(1.0);
	SetSashInvisible(true);
}

bool amTLTimeline::IsCharacterPresent(const wxString& character) {
	bool is = false;

	for (amTLThread*& thread : m_canvas->GetThreads()) {
		if (thread->GetCharacter().IsSameAs(character)) {
			is = true;
			break;
		}
	}

	return is;
}

void amTLTimeline::RecalculateCanvas(bool doThreads, bool doCards, bool doSections) {
	if (doThreads)
		m_canvas->RepositionThreads();

	if (doCards) {
		ShapeList cards;
		m_canvas->GetDiagramManager()->GetShapes(CLASSINFO(TimelineCard), cards);

		ShapeList::compatibility_iterator node = cards.GetFirst();
		while (node) {
			TimelineCard* card = (TimelineCard*)node->GetData();
			card->RecalculatePosition();

			node = node->GetNext();
		}
	}

	if (doSections) {
		for (amTLSection*& section : m_canvas->GetSections())
			section->RecalculatePosition();
	}

	Refresh();
}

void amTLTimeline::AppendThread(const wxString& character, const wxColour& colour) {
	m_canvas->AppendThread(character, colour);
}

void amTLTimeline::AppendSection(const wxString& title, const wxColour& colour) {
	m_canvas->AppendSection(title, colour);
}

void amTLTimeline::AddCardToThread(amTLThread* thread, int sectionIndex) {
	if (thread)
		m_canvas->AppendCard(thread->GetIndex(), sectionIndex);
}

void amTLTimeline::AddCardToSection(amTLSection* section, int threadIndex) {
	if (section)
		m_canvas->AddCard(threadIndex, section->GetLast() + 1, section->GetIndex());
}

void amTLTimeline::AddCardBefore(TimelineCard* card) {
	int sectionIndex = card->GetSectionIndex();
	m_canvas->AddCard(card->GetThreadIndex(), card->GetColumn(), sectionIndex);
}

void amTLTimeline::AddCardAfter(TimelineCard* card) {
	int sectionIndex = card->GetSectionIndex();
	m_canvas->AddCard(card->GetThreadIndex(), card->GetColumn() + 1, sectionIndex);
}

void amTLTimeline::EditCurrentThread(const wxString& newCharacter) {
	amTLThread* thread = m_canvas->GetSelectedThread();
	if (thread)
		thread->SetCharacter(newCharacter);

	m_canvas->Refresh();
	m_sidebar->GetThreadStaticText()->SetLabel(newCharacter);
}

void amTLTimeline::EditCurrentCardTitle(const wxString& newTitle) {
	TimelineCard* card = m_canvas->GetSelectedCard();
	if (card)
		card->SetTitle(newTitle);

	m_canvas->Refresh();
}

void amTLTimeline::EditCurrentCardContent(const wxString& newContent) {
	TimelineCard* card = m_canvas->GetSelectedCard();
	if (card)
		card->SetContent(newContent);

	m_canvas->Refresh();
}

void amTLTimeline::EditCurrentSection(const wxString& newTitle) {
	amTLSection* section = m_canvas->GetSelectedSection();
	if (section)
		section->SetTitle(newTitle);

	m_canvas->Refresh();
	m_sidebar->GetSectionStaticText()->SetLabel(newTitle);
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

	m_sidebar->GetSizer()->Layout();
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

EVT_BUTTON(BUTTON_AddThread, amTLTimelineSidebar::OnAddThread)
EVT_BUTTON(BUTTON_AddSection, amTLTimelineSidebar::OnAddSection)
EVT_BUTTON(BUTTON_AddCardToThread, amTLTimelineSidebar::OnAddCardToThread)

EVT_BUTTON(BUTTON_AddCardBefore, amTLTimelineSidebar::OnAddCardBefore)
EVT_BUTTON(BUTTON_AddCardAfter, amTLTimelineSidebar::OnAddCardAfter)

EVT_BUTTON(BUTTON_AddCardToSection, amTLTimelineSidebar::OnAddCardToSection)

EVT_BUTTON(BUTTON_ChangeCharacterThread, amTLTimelineSidebar::OnChangeCharacterThread)
EVT_BUTTON(BUTTON_ChangeNameCard, amTLTimelineSidebar::OnChangeNameCard)
EVT_BUTTON(BUTTON_ChangeContentCard, amTLTimelineSidebar::OnChangeContentCard)
EVT_BUTTON(BUTTON_ChangeNameSection, amTLTimelineSidebar::OnChangeNameSection)

EVT_COLOURPICKER_CHANGED(COLOUR_ThreadPicker, amTLTimelineSidebar::OnThreadColourChanged)
EVT_COLOURPICKER_CHANGED(COLOUR_SectionPicker, amTLTimelineSidebar::OnSectionColourChanged)

EVT_BUTTON(BUTTON_MoveUpThread, amTLTimelineSidebar::OnMoveThread)
EVT_BUTTON(BUTTON_MoveDownThread, amTLTimelineSidebar::OnMoveThread)

EVT_BUTTON(BUTTON_DeleteThreadSidebar, amTLTimelineSidebar::OnDeleteThread)
EVT_BUTTON(BUTTON_DeleteCardSidebar, amTLTimelineSidebar::OnDeleteCard)
EVT_BUTTON(BUTTON_DeleteSectionSidebar, amTLTimelineSidebar::OnDeleteSection)

EVT_SPINCTRL(SPINBUTTON_ThreadHeight, amTLTimelineSidebar::OnPreferencesSpin)
EVT_SPINCTRL(SPINBUTTON_ThreadTitleOffset, amTLTimelineSidebar::OnPreferencesSpin)
EVT_SPINCTRL(SPINBUTTON_CardWidth, amTLTimelineSidebar::OnPreferencesSpin)
EVT_SPINCTRL(SPINBUTTON_CardHeight, amTLTimelineSidebar::OnPreferencesSpin)
EVT_SPINCTRL(SPINBUTTON_CardXSpacing, amTLTimelineSidebar::OnPreferencesSpin)
EVT_SPINCTRL(SPINBUTTON_CardYSpacing, amTLTimelineSidebar::OnPreferencesSpin)
EVT_SPINCTRL(SPINBUTTON_SectionTitleOffset, amTLTimelineSidebar::OnPreferencesSpin)
EVT_SPINCTRL(SPINBUTTON_SectionMarkerWidth, amTLTimelineSidebar::OnPreferencesSpin)
EVT_SPINCTRL(SPINBUTTON_SectionSpacing, amTLTimelineSidebar::OnPreferencesSpin)

EVT_CHECKBOX(CHECKBOX_ThreadPropagateColour, amTLTimelineSidebar::OnPreferencesCheck)
EVT_CHECKBOX(CHECKBOX_SectionDrawSeparators, amTLTimelineSidebar::OnPreferencesCheck)

EVT_BUTTON(BUTTON_ResetPreferences, amTLTimelineSidebar::OnResetPreferences)

END_EVENT_TABLE()

amTLTimelineSidebar::amTLTimelineSidebar(wxWindow* parent, amTLTimelineCanvas* canvas) : wxPanel(parent) {
	m_parent = (amTLTimeline*)parent;

	SetBackgroundColour(wxColour(50, 50, 50));
	SetDoubleBuffered(true);

	m_notebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_notebook->SetBackgroundColour(wxColour(60, 60, 60));

	m_threadPanel = new wxScrolledWindow(m_notebook);
	m_threadPanel->SetBackgroundColour(wxColour(60, 60, 60));

	wxButton* addThread = new wxButton(m_threadPanel, BUTTON_AddThread, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
	addThread->SetBitmap(wxBITMAP_PNG(plus));

	wxStaticText* curThreadLabel = new wxStaticText(m_threadPanel, -1, _("Currently selected:"));
	curThreadLabel->SetForegroundColour(wxColour(255, 255, 255));
	curThreadLabel->SetFont(wxFontInfo(12).Bold());

	m_threadThumbnail = new amTLThreadThumbnail(m_threadPanel);
	m_threadThumbnail->SetCanvas(canvas);

	m_threadName = new wxStaticText(m_threadPanel, -1, "");
	m_threadName->SetBackgroundColour(wxColour(10, 10, 10));
	m_threadName->SetForegroundColour(wxColour(255, 255, 255));
	m_threadName->SetFont(wxFontInfo(13).Bold());

	wxButton* addCardToThread = new wxButton(m_threadPanel, BUTTON_AddCardToThread, _("Add card"));
	addCardToThread->SetBackgroundColour(wxColour(30, 30, 30));
	addCardToThread->SetForegroundColour(wxColour(255, 255, 255));
	addCardToThread->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_threadThumbnail->GetThread() && m_parent->CanAddCards()); });

	wxButton* changeNameThread = new wxButton(m_threadPanel, BUTTON_ChangeCharacterThread, _("Change character"));
	changeNameThread->SetBackgroundColour(wxColour(30, 30, 30));
	changeNameThread->SetForegroundColour(wxColour(255, 255, 255));
	changeNameThread->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_threadThumbnail->GetThread()); });

	wxStaticText* numberOfCardsThread = new wxStaticText(m_threadPanel, -1, _("Cards:"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	numberOfCardsThread->SetBackgroundColour(wxColour(60, 60, 60));
	numberOfCardsThread->SetForegroundColour(wxColour(255, 255, 255));
	numberOfCardsThread->SetFont(wxFontInfo(9).Bold());
	m_threadCardCount = new wxStaticText(m_threadPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_threadCardCount->SetBackgroundColour(wxColour(30, 30, 30));
	m_threadCardCount->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* cardCountLineThread = new wxBoxSizer(wxHORIZONTAL);
	cardCountLineThread->Add(numberOfCardsThread, wxSizerFlags(0).Expand());
	cardCountLineThread->AddSpacer(5);
	cardCountLineThread->Add(m_threadCardCount, wxSizerFlags(1).Expand());

	wxStaticText* colourLabelThread = new wxStaticText(m_threadPanel, -1, _("Color:"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	colourLabelThread->SetBackgroundColour(wxColour(60, 60, 60));
	colourLabelThread->SetForegroundColour(wxColour(255, 255, 255));
	colourLabelThread->SetFont(wxFontInfo(9).Bold());

	m_threadColourPicker = new wxColourPickerCtrl(m_threadPanel, COLOUR_ThreadPicker);
	m_threadColourPicker->GetPickerCtrl()->SetBackgroundColour(*wxStockGDI::GetColour(wxStockGDI::COLOUR_BLACK));
	m_threadColourPicker->Enable(false);

	wxBoxSizer* colourLineThread = new wxBoxSizer(wxHORIZONTAL);
	colourLineThread->Add(colourLabelThread, wxSizerFlags(0).CenterVertical());
	colourLineThread->AddSpacer(5);
	colourLineThread->Add(m_threadColourPicker, wxSizerFlags(1).Expand());

	wxButton* deleteThread = new wxButton(m_threadPanel, BUTTON_DeleteThreadSidebar, "Delete thread");
	deleteThread->SetBackgroundColour(wxColour(30, 30, 30));
	deleteThread->SetForegroundColour(wxColour(255, 255, 255));
	deleteThread->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_threadThumbnail->GetThread());
		});

	wxButton* moveUpThread = new wxButton(m_threadPanel, BUTTON_MoveUpThread, "", wxDefaultPosition, wxSize(25, 25));
	moveUpThread->SetBitmap(wxBITMAP_PNG(arrowUp));
	moveUpThread->SetToolTip(_("Move thread position up"));
	moveUpThread->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_threadThumbnail->GetThread());
		});

	wxButton* moveDownThread = new wxButton(m_threadPanel, BUTTON_MoveDownThread, "", wxDefaultPosition, wxSize(25, 25));
	moveDownThread->SetBitmap(wxBITMAP_PNG(arrowDown));
	moveDownThread->SetToolTip(_("Move thread position down"));
	moveDownThread->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_threadThumbnail->GetThread());
		});

	wxBoxSizer* moveThreadLine = new wxBoxSizer(wxHORIZONTAL);
	moveThreadLine->Add(moveUpThread);
	moveThreadLine->AddStretchSpacer(1);
	moveThreadLine->Add(moveDownThread);

	wxBoxSizer* threadPanelSizer = new wxBoxSizer(wxVERTICAL);
	threadPanelSizer->Add(addThread, wxSizerFlags(0).Left().Border(wxALL, 5));
	threadPanelSizer->AddSpacer(10);
	threadPanelSizer->Add(curThreadLabel, wxSizerFlags(0).Left().Border(wxLEFT, 5));
	threadPanelSizer->Add(m_threadThumbnail, wxSizerFlags(0).Expand().Border(wxALL, 5));
	threadPanelSizer->Add(m_threadName, wxSizerFlags(0).CenterHorizontal());
	threadPanelSizer->Add(addCardToThread, wxSizerFlags(0).Expand().Border(wxALL, 5));
	threadPanelSizer->Add(changeNameThread, wxSizerFlags(0).Border(wxLEFT | wxBOTTOM, 5));
	threadPanelSizer->AddSpacer(10);
	threadPanelSizer->Add(cardCountLineThread, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 5));
	threadPanelSizer->AddSpacer(5);
	threadPanelSizer->Add(colourLineThread, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 5));
	threadPanelSizer->AddStretchSpacer(1);
	threadPanelSizer->Add(deleteThread, wxSizerFlags(0).Right().Border(wxRIGHT, 5));
	threadPanelSizer->Add(moveThreadLine, wxSizerFlags(0).Expand().Border(wxALL, 5));

	m_threadPanel->SetSizer(threadPanelSizer);

	m_cardPanel = new wxScrolledWindow(m_notebook);

	wxStaticText* curCardLabel = new wxStaticText(m_cardPanel, -1, _("Currently selected:"));
	curCardLabel->SetForegroundColour(wxColour(255, 255, 255));
	curCardLabel->SetFont(wxFontInfo(12).Bold());

	m_cardThumbnail = new amTLCardThumbnail(m_cardPanel);
	m_cardThumbnail->SetCanvas(canvas);

	wxButton* addCardBefore = new wxButton(m_cardPanel, BUTTON_AddCardBefore, _("Add before"));
	wxButton* addCardAfter = new wxButton(m_cardPanel, BUTTON_AddCardAfter, _("Add after"));
	
	addCardBefore->SetBackgroundColour(wxColour(30, 30, 30));
	addCardBefore->SetForegroundColour(wxColour(255, 255, 255));
	addCardAfter->SetBackgroundColour(wxColour(30, 30, 30));
	addCardAfter->SetForegroundColour(wxColour(255, 255, 255));

	addCardBefore->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(m_cardThumbnail->GetCard()); });
	addCardAfter->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(m_cardThumbnail->GetCard()); });

	wxBoxSizer* addCardsSizer = new wxBoxSizer(wxHORIZONTAL);
	addCardsSizer->Add(addCardBefore, wxSizerFlags(0).Expand());
	addCardsSizer->AddStretchSpacer(1);
	addCardsSizer->Add(addCardAfter, wxSizerFlags(0).Expand());

	wxButton* changeNameCard = new wxButton(m_cardPanel, BUTTON_ChangeNameCard, "Edit title");
	changeNameCard->SetBackgroundColour(wxColour(30, 30, 30));
	changeNameCard->SetForegroundColour(wxColour(255, 255, 255));
	changeNameCard->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_cardThumbnail->GetCard());
		});

	wxButton* changeContentCard = new wxButton(m_cardPanel, BUTTON_ChangeContentCard, "Edit content");
	changeContentCard->SetBackgroundColour(wxColour(30, 30, 30));
	changeContentCard->SetForegroundColour(wxColour(255, 255, 255));
	changeContentCard->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_cardThumbnail->GetCard());
		});

	wxButton* moveCardUp = new wxButton(m_cardPanel, -1, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
	wxButton* moveCardDown = new wxButton(m_cardPanel, -1, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
	wxButton* moveCardLeft = new wxButton(m_cardPanel, -1, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
	wxButton* moveCardRight = new wxButton(m_cardPanel, -1, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));

	moveCardUp->SetBitmap(wxBITMAP_PNG(arrowUp));
	moveCardDown->SetBitmap(wxBITMAP_PNG(arrowDown));
	moveCardLeft->SetBitmap(wxBITMAP_PNG(arrowLeft));
	moveCardRight->SetBitmap(wxBITMAP_PNG(arrowRight));

	wxBoxSizer* moveCardHorSizer = new wxBoxSizer(wxHORIZONTAL);
	moveCardHorSizer->AddStretchSpacer(1);
	moveCardHorSizer->Add(moveCardLeft, wxSizerFlags(0).Border(wxALL, 5));
	moveCardHorSizer->AddSpacer(FromDIP(25));
	moveCardHorSizer->Add(moveCardRight, wxSizerFlags(0).Border(wxALL, 5));
	moveCardHorSizer->AddStretchSpacer(1);

	wxButton* deleteCard = new wxButton(m_cardPanel, BUTTON_DeleteCardSidebar, "Delete card");
	deleteCard->SetBackgroundColour(wxColour(30, 30, 30));
	deleteCard->SetForegroundColour(wxColour(255, 255, 255));
	deleteCard->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_cardThumbnail->GetCard());
		});

	wxBoxSizer* cardPanelSizer = new wxBoxSizer(wxVERTICAL);
	cardPanelSizer->Add(curCardLabel, wxSizerFlags(0).Left().Border(wxLEFT, 5));
	cardPanelSizer->Add(m_cardThumbnail, wxSizerFlags(0).Expand().Border(wxALL, 5));
	cardPanelSizer->Add(addCardsSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 5));
	cardPanelSizer->AddSpacer(15);
	cardPanelSizer->Add(changeNameCard, wxSizerFlags(0).Expand().Border(wxALL, 5));
	cardPanelSizer->Add(changeContentCard, wxSizerFlags(0).Expand().Border(wxALL, 5));
	cardPanelSizer->AddStretchSpacer(1);
	cardPanelSizer->Add(moveCardUp, wxSizerFlags(0).CenterHorizontal());
	cardPanelSizer->Add(moveCardHorSizer, wxSizerFlags(0).Expand());
	cardPanelSizer->Add(moveCardDown, wxSizerFlags(0).CenterHorizontal());
	cardPanelSizer->AddStretchSpacer(1);
	cardPanelSizer->Add(deleteCard, wxSizerFlags(0).Right().Border(wxALL, 5));

	m_cardPanel->SetSizer(cardPanelSizer);

	m_sectionPanel = new wxScrolledWindow(m_notebook);

	wxButton* addSection = new wxButton(m_sectionPanel, BUTTON_AddSection, "", wxDefaultPosition, FromDIP(wxSize(25,25)));
	addSection->SetBitmap(wxBITMAP_PNG(plus));

	wxStaticText* curSectionLabel = new wxStaticText(m_sectionPanel, -1, "Currently selected:");
	curSectionLabel->SetForegroundColour(wxColour(255, 255, 255));
	curSectionLabel->SetFont(wxFontInfo(12).Bold());

	m_sectionThumbnail = new amTLSectionThumbnail(m_sectionPanel);
	m_sectionThumbnail->SetCanvas(canvas);

	m_sectionName = new wxStaticText(m_sectionPanel, -1, "");
	m_sectionName->SetBackgroundColour(wxColour(10, 10, 10));
	m_sectionName->SetForegroundColour(wxColour(255, 255, 255));
	m_sectionName->SetFont(wxFontInfo(13).Bold());

	wxButton* addCardToSection = new wxButton(m_sectionPanel, BUTTON_AddCardToSection, _("Add card"));
	addCardToSection->SetBackgroundColour(wxColour(30, 30, 30));
	addCardToSection->SetForegroundColour(wxColour(255, 255, 255));
	addCardToSection->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_sectionThumbnail->GetSection() && m_parent->CanAddCards()); });

	wxButton* changeNameSection = new wxButton(m_sectionPanel, BUTTON_ChangeNameSection, _("Change title"));
	changeNameSection->SetBackgroundColour(wxColour(30, 30, 30));
	changeNameSection->SetForegroundColour(wxColour(255, 255, 255));
	changeNameSection->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_sectionThumbnail->GetSection()); });

	wxStaticText* numberOfCardsSection = new wxStaticText(m_sectionPanel, -1, _("Cards:"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	numberOfCardsSection->SetBackgroundColour(wxColour(60, 60, 60));
	numberOfCardsSection->SetForegroundColour(wxColour(255, 255, 255));
	numberOfCardsSection->SetFont(wxFontInfo(9).Bold());
	m_sectionCardCount = new wxStaticText(m_sectionPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_sectionCardCount->SetBackgroundColour(wxColour(30, 30, 30));
	m_sectionCardCount->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* cardCountLineSection = new wxBoxSizer(wxHORIZONTAL);
	cardCountLineSection->Add(numberOfCardsSection, wxSizerFlags(0).Expand());
	cardCountLineSection->AddSpacer(5);
	cardCountLineSection->Add(m_sectionCardCount, wxSizerFlags(1).Expand());

	wxStaticText* colourLabelSection = new wxStaticText(m_sectionPanel, -1, _("Color:"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	colourLabelSection->SetBackgroundColour(wxColour(60, 60, 60));
	colourLabelSection->SetForegroundColour(wxColour(255, 255, 255));
	colourLabelSection->SetFont(wxFontInfo(9).Bold());

	m_sectionColourPicker = new wxColourPickerCtrl(m_sectionPanel, COLOUR_SectionPicker);
	m_sectionColourPicker->GetPickerCtrl()->SetBackgroundColour(*wxStockGDI::GetColour(wxStockGDI::COLOUR_BLACK));
	m_sectionColourPicker->Enable(false);

	wxBoxSizer* colourLineSection = new wxBoxSizer(wxHORIZONTAL);
	colourLineSection->Add(colourLabelSection, wxSizerFlags(0).CenterVertical());
	colourLineSection->AddSpacer(5);
	colourLineSection->Add(m_sectionColourPicker, wxSizerFlags(1).Expand());

	wxButton* deleteSection = new wxButton(m_sectionPanel, BUTTON_DeleteSectionSidebar, "Delete section");
	deleteSection->SetBackgroundColour(wxColour(30, 30, 30));
	deleteSection->SetForegroundColour(wxColour(255, 255, 255));
	deleteSection->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {
		event.Enable(m_sectionThumbnail->GetSection());
		});

	wxBoxSizer* sectionPanelSizer = new wxBoxSizer(wxVERTICAL);
	sectionPanelSizer->Add(addSection, wxSizerFlags(0).Left().Border(wxALL, 5));
	sectionPanelSizer->AddSpacer(10);
	sectionPanelSizer->Add(curSectionLabel, wxSizerFlags(0).Left().Border(wxLEFT, 5));
	sectionPanelSizer->Add(m_sectionThumbnail, wxSizerFlags(0).Expand().Border(wxALL, 5));
	sectionPanelSizer->Add(m_sectionName, wxSizerFlags(0).CenterHorizontal());
	sectionPanelSizer->Add(addCardToSection, wxSizerFlags(0).Expand().Border(wxALL, 5));
	sectionPanelSizer->Add(changeNameSection, wxSizerFlags(0).Border(wxLEFT | wxBOTTOM, 5));
	sectionPanelSizer->AddSpacer(10);
	sectionPanelSizer->Add(cardCountLineSection, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 5));
	sectionPanelSizer->AddSpacer(5);
	sectionPanelSizer->Add(colourLineSection, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 5));
	sectionPanelSizer->AddStretchSpacer(1);
	sectionPanelSizer->Add(deleteSection, wxSizerFlags(0).Right().Border(wxALL, 5));
	
	m_sectionPanel->SetSizer(sectionPanelSizer);
	
	m_preferencesPanel = new wxScrolledWindow(m_notebook);

	wxStaticText* threadLabel = new wxStaticText(m_preferencesPanel, -1, _("Threads"));
	threadLabel->SetForegroundColour(wxColour(255, 255, 255));
	threadLabel->SetFont(wxFontInfo(12).Bold());

	wxStaticText* threadTitleOffsetLabel = new wxStaticText(m_preferencesPanel, -1, _("Title offset:"));
	threadTitleOffsetLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_threadTitleOffset = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_ThreadTitleOffset, "300", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 10, 400, 300);
	m_threadTitleOffset->SetBackgroundColour(wxColour(20, 20, 20));
	m_threadTitleOffset->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* threadTitleOffsetSizer = new wxBoxSizer(wxHORIZONTAL);
	threadTitleOffsetSizer->Add(threadTitleOffsetLabel, wxSizerFlags(0).CenterVertical());
	threadTitleOffsetSizer->AddSpacer(5);
	threadTitleOffsetSizer->Add(m_threadTitleOffset, wxSizerFlags(0).Expand());

	wxStaticText* threadHeightLabel = new wxStaticText(m_preferencesPanel, -1, _("Height:"));
	threadHeightLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_threadHeight = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_ThreadHeight, "10", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 1, 30, 10);
	m_threadHeight->SetBackgroundColour(wxColour(20, 20, 20));
	m_threadHeight->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* threadHeightSizer = new wxBoxSizer(wxHORIZONTAL);
	threadHeightSizer->Add(threadHeightLabel, wxSizerFlags(0).CenterVertical());
	threadHeightSizer->AddSpacer(5);
	threadHeightSizer->Add(m_threadHeight, wxSizerFlags(0).Expand());

	m_threadPropagateColour = new wxCheckBox(m_preferencesPanel, CHECKBOX_ThreadPropagateColour, "Propagate colour");
	m_threadPropagateColour->SetForegroundColour(wxColour(255, 255, 255));
	m_threadPropagateColour->SetValue(true);

	wxStaticText* cardLabel = new wxStaticText(m_preferencesPanel, -1, _("Cards"));
	cardLabel->SetForegroundColour(wxColour(255, 255, 255));
	cardLabel->SetFont(wxFontInfo(12).Bold());

	wxStaticText* cardWidthLabel = new wxStaticText(m_preferencesPanel, -1, _("Width:"));
	cardWidthLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_cardWidth = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_CardWidth, "250", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 50, 500, 250);
	m_cardWidth->SetBackgroundColour(wxColour(20, 20, 20));
	m_cardWidth->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* cardWidthSizer = new wxBoxSizer(wxHORIZONTAL);
	cardWidthSizer->Add(cardWidthLabel, wxSizerFlags(0).CenterVertical());
	cardWidthSizer->AddSpacer(5);
	cardWidthSizer->Add(m_cardWidth, wxSizerFlags(0).Expand());

	wxStaticText* cardHeightLabel = new wxStaticText(m_preferencesPanel, -1, _("Height:"));
	cardHeightLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_cardHeight = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_CardHeight, "150", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 50, 300, 150);
	m_cardHeight->SetBackgroundColour(wxColour(20, 20, 20));
	m_cardHeight->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* cardHeightSizer = new wxBoxSizer(wxHORIZONTAL);
	cardHeightSizer->Add(cardHeightLabel, wxSizerFlags(0).CenterVertical());
	cardHeightSizer->AddSpacer(5);
	cardHeightSizer->Add(m_cardHeight, wxSizerFlags(0).Expand());

	wxStaticText* cardXLabel = new wxStaticText(m_preferencesPanel, -1, _("Horizontal spacing:"));
	cardXLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_cardXSpacing = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_CardXSpacing, "40", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 10, 100, 40);
	m_cardXSpacing->SetBackgroundColour(wxColour(20, 20, 20));
	m_cardXSpacing->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* cardXSizer = new wxBoxSizer(wxHORIZONTAL);
	cardXSizer->Add(cardXLabel, wxSizerFlags(0).CenterVertical());
	cardXSizer->AddSpacer(5);
	cardXSizer->Add(m_cardXSpacing, wxSizerFlags(0).Expand());

	wxStaticText* cardYLabel = new wxStaticText(m_preferencesPanel, -1, _("Vertical spacing:"));
	cardYLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_cardYSpacing = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_CardYSpacing, "10", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 10, 100, 10);
	m_cardYSpacing->SetBackgroundColour(wxColour(20, 20, 20));
	m_cardYSpacing->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* cardYSizer = new wxBoxSizer(wxHORIZONTAL);
	cardYSizer->Add(cardYLabel, wxSizerFlags(0).CenterVertical());
	cardYSizer->AddSpacer(5);
	cardYSizer->Add(m_cardYSpacing, wxSizerFlags(0).Expand());

	wxStaticText* sectionLabel = new wxStaticText(m_preferencesPanel, -1, _("Sections"));
	sectionLabel->SetForegroundColour(wxColour(255, 255, 255));
	sectionLabel->SetFont(wxFontInfo(12).Bold());

	wxStaticText* sectionTitleOffsetLabel = new wxStaticText(m_preferencesPanel, -1, _("Title Offset"));
	sectionTitleOffsetLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_sectionTitleOffset = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_SectionTitleOffset, "200", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 50, 300, 10);
	m_sectionTitleOffset->SetBackgroundColour(wxColour(20, 20, 20));
	m_sectionTitleOffset->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* sectionTitleOffsetSizer = new wxBoxSizer(wxHORIZONTAL);
	sectionTitleOffsetSizer->Add(sectionTitleOffsetLabel, wxSizerFlags(0).CenterVertical());
	sectionTitleOffsetSizer->AddSpacer(5);
	sectionTitleOffsetSizer->Add(m_sectionTitleOffset, wxSizerFlags(0).Expand());

	wxStaticText* sectionMarkerLabel = new wxStaticText(m_preferencesPanel, -1, _("Marker width:"));
	sectionMarkerLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_sectionMarkerWidth = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_SectionMarkerWidth, "20", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 5, 40, 20);
	m_sectionMarkerWidth->SetBackgroundColour(wxColour(20, 20, 20));
	m_sectionMarkerWidth->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* sectionMarkerSizer = new wxBoxSizer(wxHORIZONTAL);
	sectionMarkerSizer->Add(sectionMarkerLabel, wxSizerFlags(0).CenterVertical());
	sectionMarkerSizer->AddSpacer(5);
	sectionMarkerSizer->Add(m_sectionMarkerWidth, wxSizerFlags(0).Expand());

	wxStaticText* sectionSpacingLabel = new wxStaticText(m_preferencesPanel, -1, _("Spacing:"));
	sectionSpacingLabel->SetForegroundColour(wxColour(255, 255, 255));

	m_sectionSpacing = new wxSpinCtrl(m_preferencesPanel, SPINBUTTON_SectionSpacing, "400", wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS | wxSP_WRAP, 50, 800, 400);
	m_sectionSpacing->SetBackgroundColour(wxColour(20, 20, 20));
	m_sectionSpacing->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* sectionSpacingSizer = new wxBoxSizer(wxHORIZONTAL);
	sectionSpacingSizer->Add(sectionSpacingLabel, wxSizerFlags(0).CenterVertical());
	sectionSpacingSizer->AddSpacer(5);
	sectionSpacingSizer->Add(m_sectionSpacing, wxSizerFlags(0).Expand());

	m_sectionSeparators = new wxCheckBox(m_preferencesPanel, CHECKBOX_SectionDrawSeparators, "Draw separators");
	m_sectionSeparators->SetForegroundColour(wxColour(255, 255, 255));
	m_sectionSeparators->SetValue(true);

	wxButton* resetPreferences = new wxButton(m_preferencesPanel, BUTTON_ResetPreferences, "Reset preferences");
	
	wxBoxSizer* preferencesSizer = new wxBoxSizer(wxVERTICAL);
	preferencesSizer->Add(threadLabel, wxSizerFlags(0).Left().Border(wxALL, 5));
	preferencesSizer->Add(threadTitleOffsetSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->Add(threadHeightSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->Add(m_threadPropagateColour, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->AddSpacer(25);
	preferencesSizer->Add(cardLabel, wxSizerFlags(0).Expand().Border(wxALL, 5));
	preferencesSizer->Add(cardWidthSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->Add(cardHeightSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->Add(cardXSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->Add(cardYSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->AddSpacer(25);
	preferencesSizer->Add(sectionLabel, wxSizerFlags(0).Expand().Border(wxALL, 5));
	preferencesSizer->Add(sectionTitleOffsetSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->Add(sectionMarkerSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->Add(sectionSpacingSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->Add(m_sectionSeparators, wxSizerFlags(0).Expand().Border(wxLEFT | wxBOTTOM, 5));
	preferencesSizer->AddSpacer(25);
	preferencesSizer->Add(resetPreferences, wxSizerFlags(0).Left().Border(wxALL, 5));

	m_preferencesPanel->SetSizer(preferencesSizer);

	m_notebook->AddPage(m_threadPanel, _("Threads"));
	m_notebook->AddPage(m_cardPanel, _("Cards"));
	m_notebook->AddPage(m_sectionPanel, _("Sections"));
	m_notebook->AddPage(m_preferencesPanel, _("Preferences"));

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_notebook, wxSizerFlags(1).Expand().Border(wxTOP, 40));

	SetSizer(sizer);
	m_notebook->Hide();

	Bind(wxEVT_PAINT, &amTLTimelineSidebar::OnPaint, this);
	Bind(wxEVT_MOTION, &amTLTimelineSidebar::OnMouseMove, this);
	Bind(wxEVT_LEFT_DOWN, &amTLTimelineSidebar::OnLeftDown, this);
}

void amTLTimelineSidebar::ShowContent(bool show) {
	m_notebook->Show(show);
}

void amTLTimelineSidebar::OnAddThread(wxCommandEvent& event) {
	wxBusyCursor busy;
	amProjectManager* pManager = amGetManager();

	amTLAddThreadDlg* addThread = new amTLAddThreadDlg(pManager->GetMainFrame(), m_parent, pManager,
		MODE_THREAD_Add, FromDIP(wxSize(500, 500)));
	addThread->CenterOnScreen();
	addThread->Show();

	pManager->GetMainFrame()->Enable(false);
}

void amTLTimelineSidebar::OnAddSection(wxCommandEvent& event) {
	wxBusyCursor busy;
	amProjectManager* pManager = amGetManager();
		amTLAddSectionDlg* addSection = new amTLAddSectionDlg(pManager->GetMainFrame(), m_parent,
		MODE_SECTION_Add, FromDIP(wxSize(400, 400)));
	addSection->CenterOnScreen();
	addSection->Show();

	pManager->GetMainFrame()->Enable(false);
}

void amTLTimelineSidebar::OnAddCardToThread(wxCommandEvent& event) {
	amTLThread* thread = m_threadThumbnail->GetThread();
	if (thread) {
		amTLSection* section = m_sectionThumbnail->GetSection();
		int sectionIndex = -1;

		if (section)
			sectionIndex = section->GetIndex();

		m_parent->AddCardToThread(thread, sectionIndex);
	}
}

void amTLTimelineSidebar::OnAddCardToSection(wxCommandEvent& event) {
	amTLSection* section = m_sectionThumbnail->GetSection();
	
	if (section) {
		amTLThread* thread = m_threadThumbnail->GetThread();
		int threadIndex = 0;

		if (thread)
			threadIndex = thread->GetIndex();

		m_parent->AddCardToSection(section, threadIndex);
	}
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

void amTLTimelineSidebar::OnChangeCharacterThread(wxCommandEvent& event) {
	wxBusyCursor busy;
	amProjectManager* pManager = amGetManager();

	amTLAddThreadDlg* addThread = new amTLAddThreadDlg(pManager->GetMainFrame(), m_parent,
		pManager, MODE_THREAD_Change, FromDIP(wxSize(400, 400)));
	addThread->CenterOnScreen();
	addThread->Show();

	pManager->GetMainFrame()->Enable(false);
}

void amTLTimelineSidebar::OnChangeNameCard(wxCommandEvent& event) {
	amProjectManager* manager = amGetManager();

	amTLCardDlg* change = new amTLCardDlg(manager->GetMainFrame(), m_parent, m_cardThumbnail->GetCard()->GetTitle(),
		MODE_CARD_Title, FromDIP(wxSize(250, 150)));
	change->CenterOnScreen();
	change->Show();

	manager->GetMainFrame()->Enable(false);
}

void amTLTimelineSidebar::OnChangeContentCard(wxCommandEvent& event) {
	amProjectManager* manager = amGetManager();

	amTLCardDlg* change = new amTLCardDlg(manager->GetMainFrame(), m_parent, m_cardThumbnail->GetCard()->GetContent(),
		MODE_CARD_Content, FromDIP(wxSize(300, 250)));
	change->CenterOnScreen();
	change->Show();

	manager->GetMainFrame()->Enable(false);
}

void amTLTimelineSidebar::OnChangeNameSection(wxCommandEvent& event) {
	amProjectManager* manager = amGetManager();

	amTLAddSectionDlg* change = new amTLAddSectionDlg(manager->GetMainFrame(), m_parent,
		MODE_SECTION_Change, FromDIP(wxSize(300, 150)));
	change->CenterOnScreen();
	change->Show();

	manager->GetMainFrame()->Enable(false);
}

void amTLTimelineSidebar::OnThreadColourChanged(wxColourPickerEvent& event) {
	m_threadColourPicker->GetPickerCtrl()->SetBackgroundColour(event.GetColour());
	m_threadThumbnail->GetThread()->SetColour(event.GetColour());

	ShapeList::compatibility_iterator node = m_threadThumbnail->GetCards().GetFirst();

	while (node) {
		TimelineCard* card = dynamic_cast<TimelineCard*>(node->GetData());
		if (card)
			card->SetColour(event.GetColour());

		node = node->GetNext();
	}

	m_parent->GetCanvas()->Refresh();
}

void amTLTimelineSidebar::OnSectionColourChanged(wxColourPickerEvent& event) {
	m_sectionColourPicker->GetPickerCtrl()->SetBackgroundColour(event.GetColour());
	m_sectionThumbnail->GetSection()->SetColour(event.GetColour());

	m_parent->GetCanvas()->Refresh();
}

void amTLTimelineSidebar::OnMoveThread(wxCommandEvent& event) {
	amTLTimelineCanvas* canvas = m_parent->GetCanvas();
	
	canvas->MoveThread(m_threadThumbnail->GetThread(), event.GetId() == BUTTON_MoveUpThread);
	canvas->Refresh();
}

void amTLTimelineSidebar::OnDeleteThread(wxCommandEvent& event) {
	m_parent->DeleteCurrentThread();
}

void amTLTimelineSidebar::OnDeleteCard(wxCommandEvent& event) {
	m_parent->DeleteCurrentCard();
}

void amTLTimelineSidebar::OnDeleteSection(wxCommandEvent& event) {
	m_parent->DeleteCurrentSection();
}

void amTLTimelineSidebar::OnPreferencesSpin(wxSpinEvent& event) {
	int value = event.GetInt();
	bool doThreads = false, doCards = false, doSections = false;

	switch (event.GetId()) {
	case SPINBUTTON_ThreadTitleOffset:
		amTLThread::SetTitleOffset(value);
		doCards = true;
		doSections = true;
		break;
	
	case SPINBUTTON_ThreadHeight:
		amTLThread::SetHeight(value);
		doThreads = true;
		break;

	case SPINBUTTON_CardWidth:
		TimelineCard::SetWidth(value);
		doCards = true;
		doSections = true;
		break;

	case SPINBUTTON_CardHeight:
		TimelineCard::SetHeight(value);
		doThreads = true;
		doCards = true;
		break;

	case SPINBUTTON_CardXSpacing:
		TimelineCard::SetHorizontalSpacing(value);
		doCards = true;
		doSections = true;
		break;

	case SPINBUTTON_CardYSpacing:
		TimelineCard::SetVerticalSpacing(value);
		doThreads = true;
		doCards = true;
		break;

	case SPINBUTTON_SectionTitleOffset:
		amTLSection::SetTitleOffset(value);
		doThreads = true;
		doCards = true;
		doSections = true;
		break;

	case SPINBUTTON_SectionMarkerWidth:
		amTLSection::SetMarkerWidth(value);
		doCards = true;
		doSections = true;
		break;

	case SPINBUTTON_SectionSpacing:
		amTLSection::SetHorizontalSpacing(value);
		doCards = true;
		doSections = true;
		break;
	}

	m_parent->RecalculateCanvas(doThreads, doCards, doSections);
}

void amTLTimelineSidebar::OnPreferencesCheck(wxCommandEvent& event) {
	bool value = event.GetInt();
	amTLTimelineCanvas* canvas = m_parent->GetCanvas();

	switch (event.GetId()) {
	case CHECKBOX_ThreadPropagateColour:
		canvas->SetPropagateColour(value);
		canvas->ResetCardColours();
		break;

	case  CHECKBOX_SectionDrawSeparators:
		canvas->SetDrawSeparators(value);
		break;
	}

	canvas->Refresh();
}

void amTLTimelineSidebar::OnResetPreferences(wxCommandEvent& event) {
	amProjectManager* manager = amGetManager();

	wxMessageDialog message(manager->GetMainFrame(), _("Are you sure you want to reset all preferences?"), _("Confirmation"),
		wxYES_NO | wxCANCEL | wxICON_WARNING);

	switch (message.ShowModal()) {
	case wxID_OK:
	case wxID_YES:
		amTLThread::SetHeight(10);
		amTLThread::SetTitleOffset(300);

		TimelineCard::SetWidth(250);
		TimelineCard::SetHeight(150);
		TimelineCard::SetHorizontalSpacing(40);
		TimelineCard::SetVerticalSpacing(10);

		amTLSection::SetTitleOffset(200);
		amTLSection::SetMarkerWidth(20);
		amTLSection::SetHorizontalSpacing(400);

		m_threadHeight->SetValue(10);
		m_threadTitleOffset->SetValue(300);

		m_cardWidth->SetValue(250);
		m_cardHeight->SetValue(150);
		m_cardXSpacing->SetValue(40);
		m_cardYSpacing->SetValue(10);

		m_sectionTitleOffset->SetValue(200);
		m_sectionMarkerWidth->SetValue(20);
		m_sectionSpacing->SetValue(400);

		m_parent->RecalculateCanvas(true, true, true);
		break;

	default:
		break;
	}
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

	m_threadPanel->Freeze();
	if (thread) {
		m_threadName->SetLabel(thread->GetCharacter());
		m_threadCardCount->SetLabel(std::to_string(shapes.GetCount()));
		m_threadColourPicker->SetColour(thread->GetColour());
		m_threadColourPicker->GetPickerCtrl()->SetBackgroundColour(thread->GetColour());
		m_threadColourPicker->Enable(true);
	} else {
		m_threadName->SetLabel(" ------------------ ");
		m_threadCardCount->SetLabel("");
		m_threadColourPicker->SetColour(wxColour(0, 0, 0));
		m_threadColourPicker->GetPickerCtrl()->SetBackgroundColour(wxColour(0, 0, 0));
		m_threadColourPicker->Enable(false);
	}

	m_threadPanel->Thaw();
}

void amTLTimelineSidebar::SetSectionData(amTLSection* section, ShapeList& shapes) {
	if (!m_sectionThumbnail)
		return;

	m_sectionThumbnail->SetData(section, shapes);

	m_sectionPanel->Freeze();
	if (section) {
		m_sectionName->SetLabel(section->GetTitle());
		m_sectionCardCount->SetLabel(std::to_string(shapes.GetCount()));
		m_sectionColourPicker->SetColour(section->GetColour());
		m_sectionColourPicker->GetPickerCtrl()->SetBackgroundColour(section->GetColour());
		m_sectionColourPicker->Enable(true);
	} else {
		m_sectionName->SetLabel(" ------------------ ");
		m_sectionCardCount->SetLabel("");
		m_sectionColourPicker->SetColour(wxColour(0, 0, 0));
		m_sectionColourPicker->GetPickerCtrl()->SetBackgroundColour(wxColour(0, 0, 0));
		m_sectionColourPicker->Enable(false);
	}
	m_sectionPanel->Thaw();
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

	m_section->DrawNormal(dc, 0.0, false);

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