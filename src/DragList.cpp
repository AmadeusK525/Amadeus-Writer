#include "DragList.h"

#include <wx\dcbuffer.h>
#include <wx\wx.h>

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(amDragList, wxListView)

EVT_LEFT_DOWN(amDragList::OnLeftDown)
EVT_LEFT_UP(amDragList::OnLeftUp)
EVT_LEAVE_WINDOW(amDragList::OnLeaveWindow)
EVT_ENTER_WINDOW(amDragList::SelectItem)
EVT_MOTION(amDragList::OnMouseMotion)

END_EVENT_TABLE()

amDragList::amDragList(wxWindow* parent, const wxSize& size) :
	wxListView(parent, -1, wxDefaultPosition, size, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxBORDER_SIMPLE)
{
	m_posMarker = new wxPanel(this, -1, wxPoint(0, GetSize().y / 2));
	m_posMarker->Hide();
	m_posMarker->Enable(false);
	m_posMarker->SetBackgroundColour(wxColour(10, 10, 10, 100));

	SetDoubleBuffered(true);
}

void amDragList::OnLeftDown(wxMouseEvent& evt)
{
	state = DRAG_START;
}

void amDragList::OnLeftUp(wxMouseEvent& evt)
{
	wxPoint evtPos = evt.GetPosition();
	int listFlags = wxLIST_HITTEST_ONITEM;
	int itemUnderMouse = HitTest(evtPos, listFlags), insert;

	if ( state == DRAGGING )
	{
		if ( itemUnderMouse == -1 )
		{
			if ( evtPos.y > GetPosition().y )
			{
				insert = GetItemCount();
			}
			else
			{
				insert = 0;
			}

		}
		else
		{
			wxRect rect;
			GetItemRect(itemUnderMouse, rect, wxLIST_RECT_BOUNDS);

			if ( evtPos.y > (rect.GetTop() + rect.GetHeight() / 2) )
			{
				// below
				insert = itemUnderMouse + 1;
			}
			else
			{
				// above
				insert = itemUnderMouse;
			}
		}
		InsertItem(insert, m_tempName);
		SelectItem(evt);
	}

	EndDragging();
}

void amDragList::OnLeaveWindow(wxMouseEvent& evt)
{
	if ( state == DRAGGING )
	{
		EndDragging();
		InsertItem(m_previous, m_tempName);
		SelectItem(evt);
	}
}

void amDragList::OnMouseMotion(wxMouseEvent& evt)
{
	wxPoint evtPos = evt.GetPosition();
	int listFlags = wxLIST_HITTEST_ONITEMICON | wxLIST_HITTEST_ONITEMLABEL | wxLIST_HITTEST_ONITEMSTATEICON;

	// switch to dragging mode if mouseDown was on item
	if ( state == DRAG_START )
	{
		m_itemDragging = HitTest(evtPos, listFlags);
		if ( m_itemDragging == FindItem(-1, m_tempName) )
		{
			m_previous = m_itemDragging;

			wxRect rect;
			GetItemRect(m_previous, rect, wxLIST_RECT_BOUNDS);

			m_posMarker->SetPosition(rect.GetTopLeft());
			m_posMarker->SetSize(GetSize().x, 2);

			state = DRAGGING;
			SetCursor(wxCursor(wxCURSOR_CLOSED_HAND));
			DeleteItem(m_previous);
		}
		else
		{
			state = DRAG_NONE;
		}
	}

	// show markers only when dragging over an item
	bool showMarkers = state == DRAGGING;

	if ( showMarkers )
	{
		wxRect rect;
		int itemHovered = HitTest(evtPos, listFlags);

		if ( itemHovered != -1 )
			GetItemRect(itemHovered, rect, wxLIST_RECT_BOUNDS);

		if ( evtPos.y > rect.GetTop() + rect.GetHeight() / 2 )
		{
			// below
			m_posMarker->SetPosition(rect.GetBottomLeft());
		}
		else
		{
			// above
			m_posMarker->SetPosition(rect.GetTopLeft());
		}

		if ( m_posMarker->GetPosition().y > 20 )
		{
			m_posMarker->Show();
		}
		else
		{
			m_posMarker->Hide();
		}

		Refresh(false);
		Update();
	}
}

void amDragList::EndDragging()
{
	state = DRAG_NONE;

	m_posMarker->Hide();
	m_posMarker->SetPosition(wxPoint(0, GetSize().y / 2));
	SetCursor(wxCursor(wxCURSOR_DEFAULT));

	Refresh(false);
	Update();

	SelectItem(wxMouseEvent());
	SetFocus();
}

void amDragList::SelectItem(wxMouseEvent& evt)
{
	for ( int i = 0; i < GetItemCount(); i++ )
	{
		Select(i, false);
	}

	Select(FindItem(-1, m_tempName));
	EnsureVisible(FindItem(-1, m_tempName));
}
