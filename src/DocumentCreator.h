#ifndef CHAPTERCREATOR_H_
#define CHAPTERCREATOR_H_

#pragma once

#include "wx/editlbox.h"

#include "DragList.h"
#include "StoryNotebook.h"

class DocumentCreator : public wxFrame
{
private:
	wxPanel* m_ndocPanel1 = nullptr,
		* m_ndocPanel2 = nullptr;

	wxPanel* m_btnPanel = nullptr;

	wxTextCtrl* m_ndocName = nullptr;
	wxTextCtrl* m_ndocSummary = nullptr;

	amDragList* m_ndocList = nullptr;

	wxButton* m_ndocNext = nullptr,
		* m_ndocBack = nullptr,
		* m_ndocCancel = nullptr;

	wxBoxSizer* m_mainHor = nullptr;

	wxString m_tempName{};
	bool m_firstNext = true;

public:
	DocumentCreator(wxWindow* parent);

	void Next(wxCommandEvent& event);
	void Back(wxCommandEvent& event);
	void Cancel(wxCommandEvent& event);
	void Create(wxCommandEvent& event);

	void CheckClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};

enum
{
	LIST_EditableDocument,

	BUTTON_NextDocument,
	BUTTON_CancelDocument,
	BUTTON_BackDocument,
	BUTTON_CreateDocument,
};

#endif
