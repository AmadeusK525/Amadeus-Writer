#ifndef CHAPTERSNOTEBOOK_H_
#define CHAPTERSNOTEBOOK_H_

#pragma once

#include <wx\notebook.h>
#include <wx\listctrl.h>
#include <wx\wrapsizer.h>

#include "MainFrame.h"

#include <list>

class StoryGrid;
struct Chapter;

class amStoryNotebook : public wxNotebook {
private:
    amProjectManager* m_manager = nullptr;
    
    StoryGrid* m_grid = nullptr;
    wxListView* m_list = nullptr;

public:
    amStoryNotebook(wxWindow* parent, amProjectManager* manager);

    void AddChapter(Chapter& chapter, int pos = -1);
    void AddToList(Chapter& chapter, int pos = -1);

    void DeleteChapter(Chapter& chapter);
    void RemoveFromList(Chapter& chapter);

    void LayoutGrid();

    StoryGrid* GetGrid() { return m_grid; }
    wxListView* GetList() { return m_list; }

    void ClearAll();
};


//////////////////////////////////////////////////////////////////
//////////////////////////// StoryGrid /////////////////////////
//////////////////////////////////////////////////////////////////


class StoryGrid : public wxScrolledWindow {
private:
    amProjectManager* m_manager = nullptr;

    wxVector<wxButton*> m_buttons{};
    wxWrapSizer* m_btnSizer = nullptr;

public:
    StoryGrid(wxWindow* parent, amProjectManager* manager);

    void AddButton();
    void DeleteButton();

    void OpenChapter(unsigned int chapterNumber);

    void OnButtonPressed(wxCommandEvent& event);

    void ClearAll();
};

#endif