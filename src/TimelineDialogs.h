#ifndef TIMELINEDIALOGS_H_
#define TIMELINEDIALOGS_H_
#pragma once

#include <wx\frame.h>
#include <wx\listctrl.h>
#include <wx\clrpicker.h>

#include "Timeline.h"

enum {
	BUTTON_AddThreadDone,
	BUTTON_AddSectionDone
};

enum amTLThreadDlgMode {
	MODE_THREAD_Add,
	MODE_THREAD_Change
};

class amTLAddThreadDlg : public wxFrame {
private:
	amTLTimeline* m_timeline = nullptr;
	amProjectManager* m_manager = nullptr;

	wxListView* m_characterList = nullptr;
	wxImageList* m_characterImages = nullptr;

	wxColourPickerCtrl* m_colourPicker = nullptr;

	amTLThreadDlgMode m_mode = MODE_THREAD_Add;

public:
	amTLAddThreadDlg(wxWindow* parent, amTLTimeline* timeline, amProjectManager* manager,
		amTLThreadDlgMode mode = MODE_THREAD_Add, const wxSize& size = wxDefaultSize);
	
	void OnDone(wxCommandEvent& event);
	void OnUpdateDone(wxUpdateUIEvent& event);

	void SetCharacters(wxVector<Character>& characters);
	void OnListResize(wxSizeEvent& event);

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};

enum amTLSectionDlgMode {
	MODE_SECTION_Add,
	MODE_SECTION_Change
};

class amTLAddSectionDlg : public wxFrame {
private:
	amTLTimeline* m_timeline = nullptr;

	wxTextCtrl* m_titleTc = nullptr;
	wxColourPickerCtrl* m_colourPicker = nullptr;

	amTLSectionDlgMode m_mode = MODE_SECTION_Add;

public:
	amTLAddSectionDlg(wxWindow* parent, amTLTimeline* timeline,
		amTLSectionDlgMode mode = MODE_SECTION_Add, const wxSize& size = wxDefaultSize);

	void OnDone(wxCommandEvent& event);
	void OnUpdateDone(wxUpdateUIEvent& event);

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif