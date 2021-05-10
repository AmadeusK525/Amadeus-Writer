/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/switchctrlg.cpp
// Purpose:     Generic wxSwitchCtrl class implementation
// Author:      Leonardo Bronstein Franceschetti <bronstein.franceschetti@protonmail.com>
// Created:     2019-02-07
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "SwitchCtrl.h"
#include <wx/wx.h>
#include <wx/dcgraph.h>

wxColour wxMixColours(const wxColour& firstColour, const wxColour& secondColour, int percent)
{
	int newRed = (double)((secondColour.Red() * percent) + (firstColour.Red() * (100 - percent))) / 100;
	int newGreen = (double)((secondColour.Green() * percent) + (firstColour.Green() * (100 - percent))) / 100;
	int newBlue = (double)((secondColour.Blue() * percent) + (firstColour.Blue() * (100 - percent))) / 100;

	return wxColour((unsigned char)newRed, (unsigned char)newGreen, (unsigned char)newBlue);
}

// Implementing events
wxDEFINE_EVENT(wxEVT_SWITCH, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SWITCHING, wxCommandEvent);


wxBEGIN_EVENT_TABLE(wxSwitchCtrl, wxControl)

EVT_PAINT(wxSwitchCtrl::OnPaint)
EVT_SIZE(wxSwitchCtrl::OnSize)

EVT_LEFT_DOWN(wxSwitchCtrl::OnLeftDown)
EVT_LEFT_UP(wxSwitchCtrl::OnLeftUp)
EVT_MOTION(wxSwitchCtrl::OnMouseMove)
EVT_MOUSE_CAPTURE_LOST(wxSwitchCtrl::OnMouseCaptureLost)

EVT_TIMER(12345, wxSwitchCtrl::OnAnimationTimer)

wxEND_EVENT_TABLE()

wxSwitchCtrl::wxSwitchCtrl(wxWindow* parent,
	wxWindowID id,
	bool value,
	const wxString& label,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxValidator& validator,
	const wxString& name)
	: wxControl(parent, id, pos, size, style, validator, name), m_tAnimationTimer(this, 12345)
{
	m_sizer = new wxBoxSizer(wxVERTICAL);
	m_sizer->AddStretchSpacer(1);
	SetSizer(m_sizer);

	if ( !label.IsEmpty() )
		SetLabel(label);

	SetCursor(wxCURSOR_CLOSED_HAND);
	DoSwitch(value, false);
}

void wxSwitchCtrl::SetLabel(const wxString& label)
{
	if ( !m_labelST )
	{
		m_labelST = new wxStaticText(this, -1, label, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
		m_labelST->SetCursor(wxCURSOR_DEFAULT);
		m_labelST->SetFont(GetFont());
		m_labelST->SetForegroundColour(GetForegroundColour());

		m_sizer->Insert(0, m_labelST, wxSizerFlags(0).Expand().Border(wxBOTTOM, 1));
		m_sizer->AddSpacer(m_labelST->GetSize().y);
	}
	else
	{
		m_labelST->SetLabel(label);
	}

	wxControl::SetLabel(label);
	Layout();
}

bool wxSwitchCtrl::SetFont(const wxFont& font)
{
	if ( m_labelST )
		m_labelST->SetFont(font);

	wxControl::SetFont(font);
	return true;
}

bool wxSwitchCtrl::SetForegroundColour(const wxColour& colour)
{
	if ( m_labelST )
		m_labelST->SetForegroundColour(colour);

	wxControl::SetForegroundColour(colour);
	return true;
}

wxSize wxSwitchCtrl::DoGetBestClientSize() const
{
	if ( m_labelST )
	{
		wxSize labelSize = m_labelST->GetSize();
		return labelSize + wxSize(labelSize.x < 15 ? 20 : labelSize.x, labelSize.y);
	}
	else
	{
		return FromDIP(wxSize(m_radius * 4, m_radius * 2));
	}
}

void wxSwitchCtrl::DoSwitch(bool state, bool sendEvent)
{
	if ( state == m_bIsOn )
		sendEvent = false;

	if ( sendEvent )
	{
		wxCommandEvent event(wxEVT_SWITCHING, GetId());
		event.SetInt(state);
		event.Skip();
		ProcessEvent(event);

		if ( !event.GetSkipped() )
			return;
	}

	m_bIsOn = state;
	m_szCacheSize = GetClientSize();
	m_tAnimationTimer.Start(1);

	if ( sendEvent )
	{
		wxCommandEvent* event = new wxCommandEvent(wxEVT_SWITCH, GetId());
		event->SetInt(state);
		QueueEvent(event);
	}
}

void wxSwitchCtrl::OnAnimationTimer(wxTimerEvent& event)
{
	if ( m_bIsOn )
	{
		if ( m_nCurrentUnit < m_nUnitCount )
		{
			m_nCurrentUnit++;
		}
		else
		{
			m_nCurrentUnit = m_nUnitCount;
			m_tAnimationTimer.Stop();
		}
	}
	else
	{
		if ( m_nCurrentUnit > 0 )
		{
			m_nCurrentUnit--;
		}
		else
		{
			m_nCurrentUnit = 0;
			m_tAnimationTimer.Stop();
		}
	}

	m_nCurrentButtonPos = ((double)(m_nCurrentUnit * (m_szCacheSize.x - (m_radius * 2))) / m_nUnitCount);
	Refresh(true);
	Update();
}

void wxSwitchCtrl::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	wxGCDC gdc(dc);
	wxSize clientSize = GetClientSize();

	wxColour slideColour = wxMixColours(m_disabledColour, m_enabledColour, (m_nCurrentUnit * 100) / m_nUnitCount);

	gdc.SetBrush(wxBrush(slideColour));
	gdc.SetPen(*wxTRANSPARENT_PEN);
	gdc.DrawRoundedRectangle(wxPoint(0, m_yOrigin), clientSize - wxSize(0, m_yOrigin), m_radius);

	gdc.SetBrush(wxBrush(m_buttonColour));
	gdc.SetPen(*wxTRANSPARENT_PEN);
	gdc.DrawCircle(wxPoint(m_radius + m_nCurrentButtonPos, clientSize.y - m_radius), m_radius - 1);
}

void wxSwitchCtrl::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();

	if ( m_labelST )
	{
		wxSize labelSize = m_labelST->GetBestSize();
		size.y -= labelSize.y;
		m_yOrigin = labelSize.y;

		if ( size.y < labelSize.y )
			SetMinClientSize(wxSize(labelSize.x, labelSize.y * 2));
	}

	m_radius = (double)size.y / 2.0;
	m_nCurrentButtonPos = ((double)(m_nCurrentUnit * (size.x - (m_radius * 2))) / m_nUnitCount);

	event.Skip();
}

void wxSwitchCtrl::OnLeftDown(wxMouseEvent& event)
{
	// If the user presses clicks on the button, prepare to
	// drag, but don't change state to dragging yet.

	int buttonWidth = m_radius * 2;
	wxRect buttonRect(m_nCurrentButtonPos, GetClientSize().y - buttonWidth, buttonWidth, buttonWidth);

	if ( buttonRect.Contains(event.GetPosition()) )
		m_bWillDrag = true;
}

void wxSwitchCtrl::OnLeftUp(wxMouseEvent& event)
{
	if ( !m_bIsDragging )
		DoSwitch(!m_bIsOn);
	else
	{
		DoSwitch(m_nCurrentUnit >= m_nUnitCount / 2 ? true : false);
		ReleaseMouse();
	}

	m_bWillDrag = false;
	m_bIsDragging = false;
}

void wxSwitchCtrl::OnMouseMove(wxMouseEvent& event)
{
	if ( m_bWillDrag )
	{
		CaptureMouse();
		m_bIsDragging = true;
		m_bWillDrag = false;
	}

	if ( m_bIsDragging )
	{
		wxSize clientSize(GetClientSize());
		wxPoint mousePos(event.GetPosition());

		// Force input to be valid
		if ( mousePos.x < 0 )
			mousePos.x = 0;
		else if ( mousePos.x > clientSize.x )
			mousePos.x = clientSize.x;

		int unitWidth = clientSize.x / m_nUnitCount;

		// Calculate current button unit and its position
		m_nCurrentUnit = mousePos.x / unitWidth;
		m_nCurrentButtonPos = ((double)(m_nCurrentUnit * (clientSize.x - (m_radius * 2))) / m_nUnitCount);

		// Update the screen for the user
		Refresh();
		Update();
	}

	event.Skip();
}

void wxSwitchCtrl::OnMouseCaptureLost(wxMouseCaptureLostEvent& event)
{
	m_bWillDrag = false;
	m_bIsDragging = false;

	DoSwitch(m_nCurrentUnit >= m_nUnitCount / 2 ? true : false);
}