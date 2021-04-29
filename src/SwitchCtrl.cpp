///////////////////////////////////////////////////////////////////////////////
// Name:        SiwtchCtrl.h
// Purpose:     Implementing a custom wxSwitchCtrl
// Author:      AmadeusK525
// Created:     2020-04-13
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "SwitchCtrl.h"

#include <wx\dcbuffer.h>
#include <wx\dcgraph.h>

wxColour wxMixColours(const wxColour& firstColour, const wxColour& secondColour, int percent)
{
	int newRed = (double)((secondColour.Red() * percent) + (firstColour.Red() * (100 - percent))) / 100;
	int newGreen = (double)((secondColour.Green() * percent) + (firstColour.Green() * (100 - percent))) / 100;
	int newBlue = (double)((secondColour.Blue() * percent) + (firstColour.Blue() * (100 - percent))) / 100;

	return wxColour((unsigned char)newRed, (unsigned char)newGreen, (unsigned char)newBlue);
}

wxDEFINE_EVENT(wxEVT_SWITCH, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_SWITCHING, wxCommandEvent);

BEGIN_EVENT_TABLE(wxSwitchCtrl, wxControl)

EVT_PAINT(wxSwitchCtrl::OnPaint)
EVT_SIZE(wxSwitchCtrl::OnSize)

EVT_LEFT_UP(wxSwitchCtrl::OnLeftUp)

EVT_TIMER(12345, wxSwitchCtrl::OnTimer)

END_EVENT_TABLE()

wxSwitchCtrl::wxSwitchCtrl(wxWindow* parent,
	wxWindowID id,
	bool value,
	const wxString& label,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxValidator& validator,
	const wxString& name) : wxControl(parent, id, pos, size, style, validator, name), m_animationTimer(this, 12345)
{
	SetBackgroundColour(m_parent->GetBackgroundColour());
	SetDoubleBuffered(true);

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
		return FromDIP(wxSize(m_radius * 4, m_radius * 2));
}

void wxSwitchCtrl::DoSwitch(bool state, bool sendEvent)
{
	if ( state == m_isOn )
		return;

	if ( sendEvent )
	{
		wxCommandEvent event(wxEVT_SWITCHING, GetId());
		event.SetInt(state);
		event.Skip();
		ProcessEvent(event);

		if ( !event.GetSkipped() )
			return;
	}

	m_isOn = state;
	m_cacheSize = GetClientSize();
	m_animationTimer.Start(1);

	if ( sendEvent )
	{
		wxCommandEvent* event = new wxCommandEvent(wxEVT_SWITCH, GetId());
		event->SetInt(state);
		QueueEvent(event);
	}
}

void wxSwitchCtrl::OnTimer(wxTimerEvent& event)
{
	if ( m_isOn )
		if ( m_currentUnit < m_units ) m_currentUnit++;
		else
		{
			m_currentUnit = m_units;
			m_animationTimer.Stop();
		}
	else
		if ( m_currentUnit > 0 ) m_currentUnit--;
		else
		{
			m_currentUnit = 0;
			m_animationTimer.Stop();
		}

	m_currentCenterPos = ((double)(m_currentUnit * (m_cacheSize.x - (m_radius * 2))) / m_units);
	Refresh(true);
	Update();
}

void wxSwitchCtrl::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	wxGCDC gdc(dc);
	wxSize clientSize = GetClientSize();

	wxColour slideColour = wxMixColours(m_disabledColour, m_enabledColour, (m_currentUnit * 100) / m_units);

	gdc.SetBrush(wxBrush(slideColour));
	gdc.SetPen(*wxTRANSPARENT_PEN);
	gdc.DrawRoundedRectangle(wxPoint(0, m_yOrigin), clientSize - wxSize(0, m_yOrigin), m_radius);

	gdc.SetBrush(wxBrush(m_buttonColour));
	gdc.SetPen(*wxTRANSPARENT_PEN);
	gdc.DrawCircle(wxPoint(m_radius + m_currentCenterPos, clientSize.y - m_radius), m_radius - 1);
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
	m_currentCenterPos = ((double)(m_currentUnit * (size.x - (m_radius * 2))) / m_units);

	event.Skip();
}

void wxSwitchCtrl::OnLeftUp(wxMouseEvent& event)
{
	DoSwitch(!m_isOn);
}