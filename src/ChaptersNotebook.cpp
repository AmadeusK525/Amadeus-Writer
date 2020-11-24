#include "ChaptersNotebook.h"

#include "ChaptersGrid.h"
#include "Chapter.h"

ChaptersNotebook::ChaptersNotebook(wxWindow* parent) : wxNotebook(parent, -1) {
    grid = new ChaptersGrid(this);
    grid->SetBackgroundColour(wxColour(150, 0, 0));

    list = new wxListView(this);

    AddPage(grid, "Grid");
    AddPage(list, "List");
}

void ChaptersNotebook::addChapter(Chapter& chapter, int& pos) {

    if (pos < current) {
        auto it = chapters.begin();
        for (int i = 0; i < pos; i++) {
            it++;
        }
        chapters.insert(it, chapter);
    } else {
        chapters.push_back(chapter);
    }

    // Redeclare all chapter positions  
    int i = 1;
    for (auto it : chapters) {
        it.position = i++;
    }

    MainFrame::saved[2]++;
    grid->addButton();
    MainFrame::isSaved = false;
}

void ChaptersNotebook::clearAll() {
    grid->clearAll();
}
