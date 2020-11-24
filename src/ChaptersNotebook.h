#ifndef CHAPTERSNOTEBOOK_H_
#define CHAPTERSNOTEBOOK_H_

#pragma once

#include <wx/notebook.h>
#include <wx/listctrl.h>

#include "MainFrame.h"

#include <list>

class ChaptersGrid;
struct Chapter;

class ChaptersNotebook : public wxNotebook {
private:
    ChaptersGrid* grid = nullptr;
    wxListView* list = nullptr;

public:
    std::list<Chapter> chapters{};
    int current = 1;

public:
    ChaptersNotebook(wxWindow* parent);

    void addChapter(Chapter& chapter, int& pos);

    ChaptersGrid* getGrid() { return grid; }
    wxListView* getList() { return list; }

    void clearAll();
};

#endif