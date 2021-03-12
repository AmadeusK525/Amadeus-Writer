#ifndef OVERVIEW_H_
#define OVERVIEW_H_
#pragma once

#include <wx\scrolwin.h>
#include <wx\tglbtn.h>

#include "BookElements.h"

#include "MainFrame.h"

class amOverview;
class amBookPicker;

class amBookPicker : public wxScrolledWindow {
private:
	wxSize m_bookCoverSize{};
	wxSize m_buttonSize{};

	amOverview* m_parent = nullptr;

	wxBoxSizer* m_mainSizer = nullptr;
	wxVector<wxBitmapToggleButton*> m_bookButtons{};

public:
	amBookPicker(amOverview* parent);

	void AddButton(Book& book, int index);
	void OnButtonPress(wxCommandEvent& event);

	void SetSelection(int bookPos);
};


/////////////////////////////////////////////////////////////////
/////////////////////////// Overview ////////////////////////////
/////////////////////////////////////////////////////////////////


class amOverview : public wxPanel {
private:
	amProjectManager* m_manager = nullptr;
	amBookPicker* m_bookPicker = nullptr;

public:
	amOverview(wxWindow* parent, amProjectManager* manager);

	void LoadOverview();
};

#endif