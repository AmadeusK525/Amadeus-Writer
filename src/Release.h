#pragma once

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextprint.h>

#include "ProjectManaging.h"


class amRelease : public wxPanel
{
private:
	//wxRichTextPrinting* print = nullptr;
	wxPanel* m_panel = nullptr;
	wxBoxSizer* m_pagesSizer = nullptr;

public:
	amRelease(wxWindow* parent);
	virtual ~amRelease();

	void ClearAll();

	void SetBookData(am::Book* book);
	void UpdateContent();

	void OnNext(wxCommandEvent& event);
	void OnPrevious(wxCommandEvent& event);
	void OnKeyPressed(wxKeyEvent& event);

	DECLARE_EVENT_TABLE()
};

enum
{
	BUTTON_NextPage,
	BUTTON_PreviousPage
};
