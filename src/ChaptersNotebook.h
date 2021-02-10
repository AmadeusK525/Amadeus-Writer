#ifndef CHAPTERSNOTEBOOK_H_
#define CHAPTERSNOTEBOOK_H_

#pragma once

#include <wx/notebook.h>
#include <wx/listctrl.h>

#include "MainFrame.h"

#include <list>

class ChapterGrid;
struct Chapter;

class amChaptersNotebook : public wxNotebook {
private:
    amProjectManager* m_manager = nullptr;
    
    ChapterGrid* m_grid = nullptr;
    wxListView* m_list = nullptr;

public:
    amChaptersNotebook(wxWindow* parent, amProjectManager* manager);

    void AddChapter(Chapter& chapter, int pos = -1);
    void AddToList(Chapter& chapter, int pos = -1);

    ChapterGrid* GetGrid() { return m_grid; }
    wxListView* GetList() { return m_list; }

    void ClearAll();
};

#endif