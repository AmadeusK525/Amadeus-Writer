#ifndef CHAPTERSNOTEBOOK_H_
#define CHAPTERSNOTEBOOK_H_

#pragma once

#include <wx/aui/auibook.h>
#include <wx/listctrl.h>
#include <wx/wrapsizer.h>

#include "Views/MainFrame.h"

#include <list>

class StoryGrid;
struct Document;

class amStoryNotebook : public wxAuiNotebook
{
private:
	StoryGrid* m_grid = nullptr;
	wxListView* m_list = nullptr;

public:
	amStoryNotebook(wxWindow* parent);

	void SetBookData(am::Book* book);

	void AddDocument(am::Document* document, int pos = -1);
	void AddToList(am::Document* document, int pos = -1);

	void DeleteDocument(am::Document* document);
	void RemoveFromList(am::Document* document);

	void LayoutGrid();

	StoryGrid* GetGrid() { return m_grid; }
	wxListView* GetList() { return m_list; }

	void ClearAll();
};


//////////////////////////////////////////////////////////////////
//////////////////////////// StoryGrid /////////////////////////
//////////////////////////////////////////////////////////////////


class StoryGrid : public wxScrolledWindow
{
private:
	wxVector<wxButton*> m_buttons{};
	wxWrapSizer* m_btnSizer = nullptr;

public:
	StoryGrid(wxWindow* parent);

	void AddButton();
	void DeleteButton();

	void OpenDocument(unsigned int documentNumber);

	void OnButtonPressed(wxCommandEvent& event);

	void ClearAll();
};

#endif
