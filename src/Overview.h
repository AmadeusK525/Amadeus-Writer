#ifndef OVERVIEW_H_
#define OVERVIEW_H_
#pragma once

#include <wx\scrolwin.h>
#include <wx\tglbtn.h>

#include "BookElements.h"

#include "MainFrame.h"

class amOverview;
class amBookPicker;

class amBookPicker : public wxPanel
{
private:
	wxSize m_bookCoverSize{};
	wxSize m_buttonSize{};

	amOverview* m_overview = nullptr;

	wxScrolledWindow* m_mainWindow = nullptr;
	wxBoxSizer* m_mainSizer = nullptr;

	struct amBookButton : public wxBitmapToggleButton
	{
		Book* book = nullptr;

		amBookButton(wxWindow* parent, Book* book, const wxBitmap& bmp,
			const wxPoint& pos, const wxSize& size) :
			wxBitmapToggleButton(parent, -1, bmp, pos, size), book(book)
		{
			wxASSERT(book);
		}
	};

	wxVector<amBookButton*> m_bookButtons{};

public:
	amBookPicker(amOverview* parent);

	void AddButton(Book* book, int index);
	void OnBookClicked(wxCommandEvent& event);

	void SetSelection(int bookPos);
};


/////////////////////////////////////////////////////////////////
/////////////////////////// Overview ////////////////////////////
/////////////////////////////////////////////////////////////////


class amOverview : public wxPanel
{
private:
	amProjectManager* m_manager = nullptr;
	amBookPicker* m_bookPicker = nullptr;

	wxStaticText* m_stBookTitle = nullptr;
	wxStaticText* m_stBookAuthor = nullptr;

	wxBoxSizer* m_mainSizer = nullptr;

public:
	amOverview(wxWindow* parent, amProjectManager* manager);

	void SetBookData(Book* book);

	void LoadBookContainer();
};

#endif