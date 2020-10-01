#include "ChaptersNotebook.h"

ChaptersNotebook::ChaptersNotebook(wxWindow* parent) : wxNotebook(parent, -1) {
    grid = new ChaptersGrid(this);
    grid->SetBackgroundColour(wxColour(150, 0, 0));

    list = new wxListView(this);

    AddPage(grid, "Grid");
    AddPage(list, "List");
}
