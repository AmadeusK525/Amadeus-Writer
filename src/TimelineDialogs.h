#ifndef TIMELINEDIALOGS_H_
#define TIMELINEDIALOGS_H_
#pragma once

#include <wx\frame.h>
#include <wx\listctrl.h>
#include <wx\clrpicker.h>

#include "Timeline.h"

enum {
	BUTTON_AddThreadDone,
	BUTTON_AddSectionDone,
};

class amTLAddThreadDlg : public wxFrame {
private:
	amTLTimeline* m_timeline = nullptr;
	amProjectManager* m_manager = nullptr;

	wxListView* m_characterList = nullptr;
	wxImageList* m_characterImages = nullptr;

	wxColourPickerCtrl* m_colourPicker = nullptr;

public:
	amTLAddThreadDlg(wxWindow* parent, amTLTimeline* timeline, amProjectManager* manager, const wxSize& size = wxDefaultSize);
	
	void OnAdd(wxCommandEvent& event);
	void OnUpdateAdd(wxUpdateUIEvent& event);

	void SetCharacters(wxVector<Character>& characters);
	void OnListResize(wxSizeEvent& event);

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};


class amTLAddSectionDlg : public wxFrame {
private:
	amTLTimeline* m_timeline = nullptr;

	wxTextCtrl* m_titleTc = nullptr;
	wxColourPickerCtrl* m_colourPicker = nullptr;

public:
	amTLAddSectionDlg(wxWindow* parent, amTLTimeline* timeline, const wxSize& size);

	void OnAdd(wxCommandEvent& event);
	void OnUpdateAdd(wxUpdateUIEvent& event);

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif