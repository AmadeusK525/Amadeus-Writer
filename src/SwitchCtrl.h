#ifndef _WX_SWITCHCTRL_H_
#define _WX_SWITCHCTRL_H_
#pragma once

#include <wx/control.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/timer.h>

wxColour WXDLLIMPEXP_CORE wxMixColours(const wxColour& firstColour, const wxColour& secondColour, int percent);

class WXDLLIMPEXP_CORE wxSwitchCtrl : public wxControl
{
public:
	wxSwitchCtrl() = default;
	wxSwitchCtrl(wxWindow* parent,
		wxWindowID id,
		bool value = false,
		const wxString& label = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(30, 15),
		long style = wxBORDER_NONE,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxControlNameStr);

	virtual void SetLabel(const wxString& label) wxOVERRIDE;
	virtual bool SetFont(const wxFont& font) wxOVERRIDE;
	virtual bool SetForegroundColour(const wxColour& colour) wxOVERRIDE;
	virtual wxSize DoGetBestClientSize() const wxOVERRIDE;

	inline void Switch() { DoSwitch(!m_bIsOn, true); }

	inline virtual void SetValue(bool state) { DoSwitch(state, false); }
	inline bool GetValue() { return m_bIsOn; }

	inline void SetEnabledColour(const wxColour& colour) { m_enabledColour = colour; }
	inline void SetDisabledColour(const  wxColour& colour) { m_disabledColour = colour; }

	/*!
	* \brief Set the number of units the buttons needs to travel when activaded.
	* The higher the number, the slower the animation on activate/deactivate
	* will be. Default is 10.
	* \param units Number of units
	*/
	inline void SetUnitsToScroll(int units) { m_nUnitCount = units; }

	wxDECLARE_EVENT_TABLE();

protected:
	void DoSwitch(bool state, bool sendEvent = true);
	void OnAnimationTimer(wxTimerEvent& event);

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);

	void OnLeftDown(wxMouseEvent& event);
	void OnLeftUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

	wxStaticText* m_labelST = nullptr;

	wxBoxSizer* m_sizer = nullptr;

	wxColour m_enabledColour{ 50, 50, 255 };
	wxColour m_disabledColour{ 100,100,100 };
	wxColour m_buttonColour{ 30,30,30 };

	bool m_bIsOn = false;

private:
	int m_nCurrentUnit = 0;
	int m_nCurrentButtonPos = 0;
	int m_yOrigin = 0;

	int m_nUnitCount = 10;
	double m_radius = 2.0;

	bool m_bWillDrag = false;
	bool m_bIsDragging = false;

	wxTimer m_tAnimationTimer;
	wxSize m_szCacheSize;
};

wxDECLARE_EVENT(wxEVT_SWITCH, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_SWITCHING, wxCommandEvent);
#define EVT_SWITCH(winid, func) wx__DECLARE_EVT1(wxEVT_SWITCH, winid, wxCommandEventHandler(func))
#define EVT_SWITCHING(winid, func) wx__DECLARE_EVT1(wxEVT_SWITCHING, winid, wxCommandEventHandler(func))

#endif // _WX_SWITCHCTRL_H_