#ifndef CHAPTERSNOTEBOOK_H_
#define CHAPTERSNOTEBOOK_H_

#pragma once

#include <wx\notebook.h>
#include <wx\listctrl.h>
#include <wx\wrapsizer.h>

#include "MainFrame.h"

#include <list>

class StoryGrid;
struct Document;

class amStoryNotebook : public wxNotebook
{
private:
	amProjectManager* m_manager = nullptr;

	StoryGrid* m_grid = nullptr;
	wxListView* m_list = nullptr;

public:
	amStoryNotebook(wxWindow* parent, amProjectManager* manager);

	void SetBookData(Book* book);

	void AddDocument(Document* document, int pos = -1);
	void AddToList(Document* document, int pos = -1);

	void DeleteDocument(Document* document);
	void RemoveFromList(Document* document);

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
	amProjectManager* m_manager = nullptr;

	wxVector<wxButton*> m_buttons{};
	wxWrapSizer* m_btnSizer = nullptr;

public:
	StoryGrid(wxWindow* parent, amProjectManager* manager);

	void AddButton();
	void DeleteButton();

	void OpenDocument(unsigned int documentNumber);

	void OnButtonPressed(wxCommandEvent& event);

	void ClearAll();
};

#endif