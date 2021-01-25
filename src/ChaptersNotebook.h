#ifndef CHAPTERSNOTEBOOK_H_
#define CHAPTERSNOTEBOOK_H_

#pragma once

#include <wx/notebook.h>
#include <wx/listctrl.h>

#include "MainFrame.h"

#include <list>

class ChapterGrid;
struct Chapter;

class amdChaptersNotebook : public wxNotebook {
private:
    amdProjectManager* m_manager = nullptr;
    
    ChapterGrid* m_grid = nullptr;
    wxListView* m_list = nullptr;

public:
    amdChaptersNotebook(wxWindow* parent, amdProjectManager* manager);

    void AddChapter(Chapter& chapter, int pos = -1, bool Reposition = true);
    void AddToList(Chapter& chapter, int pos = -1);

    void RepositionChapters();

    ChapterGrid* GetGrid() { return m_grid; }
    wxListView* GetList() { return m_list; }

    void ClearAll();
};

#endif