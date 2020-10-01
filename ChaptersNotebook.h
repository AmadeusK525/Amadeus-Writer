#pragma once

#include "wx/notebook.h"
#include "wx/listctrl.h"

#include "ChaptersGrid.h"

class ChaptersGrid;

class ChaptersNotebook : public wxNotebook {
private:
    ChaptersGrid* grid = nullptr;
    wxListView* list = nullptr;

public:
    ChaptersNotebook(wxWindow* parent);
    ChaptersGrid* getGrid() { return grid; }
    wxListView* getList() { return list; }
};

