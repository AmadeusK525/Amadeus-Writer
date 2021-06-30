#ifndef OVERVIEW_H_
#define OVERVIEW_H_
#pragma once

#include <wx\scrolwin.h>
#include <wx\tglbtn.h>

#include "BookElements.h"
#include "StoryWriter.h"
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
		am::Book* book = nullptr;

		amBookButton(wxWindow* parent, am::Book* book, const wxBitmap& bmp,
			const wxPoint& pos, const wxSize& size) :
			wxBitmapToggleButton(parent, -1, bmp, pos, size), book(book)
		{
			wxASSERT(book);
		}
	};

	wxVector<amBookButton*> m_bookButtons{};

public:
	amBookPicker(amOverview* parent);

	void AddButton(am::Book* book, size_t index);
	void OnBookClicked(wxCommandEvent& event);

	void SetSelectionByBook(am::Book* book);

	void OnCreateBook(wxCommandEvent& event);
	void OnDeleteBook(wxCommandEvent& event);

	void DeleteAllButtons();
};


/////////////////////////////////////////////////////////////////
/////////////////////////// Overview ////////////////////////////
/////////////////////////////////////////////////////////////////


class amOverview : public wxPanel
{
private:
	amBookPicker* m_bookPicker = nullptr;

	wxStaticText* m_stBookTitle = nullptr;
	wxStaticText* m_stBookAuthor = nullptr;

#ifdef __WXMSW__
	amHotTrackingDVCHandler m_recentDocumentsHandler;
#endif
	wxDataViewCtrl* m_recentDocumentsDVC = nullptr;
	wxObjectDataPtr<StoryTreeModel> m_recentDocumentsModel;

	wxBoxSizer* m_mainSizer = nullptr;

public:
	amOverview(wxWindow* parent);

	void ClearAll();

	void SetBookData(am::Book* book);
	void LoadRecentDocuments(am::Book* book);

	void LoadBookContainer();

	void OnRecentDocument(wxDataViewEvent& event);
};

#endif