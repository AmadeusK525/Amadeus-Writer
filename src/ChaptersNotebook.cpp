#include "ChaptersNotebook.h"

#include "ChaptersGrid.h"
#include "Chapter.h"

ChaptersNotebook::ChaptersNotebook(wxWindow* parent) : wxNotebook(parent, -1) {
    grid = new ChaptersGrid(this);
    grid->SetBackgroundColour(wxColour(150, 0, 0));

    list = new wxListView(this);
    list->InsertColumn(0, "Name");
    list->InsertColumn(1, "Characters");
    list->InsertColumn(2, "Locations");
    list->InsertColumn(3, "Point of View");

    AddPage(grid, "Grid");
    AddPage(list, "List");
}

void ChaptersNotebook::addChapter(Chapter& chapter, int pos) {

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
    addToList(chapter, pos);
    MainFrame::isSaved = false;
}

void ChaptersNotebook::addToList(Chapter& chapter, int pos) {
    list->InsertItem(pos, chapter.name);
    list->SetItem(pos, 1, std::to_string(chapter.characters.size()));
    list->SetItem(pos, 2, std::to_string(chapter.locations.size()));
    list->SetItem(pos, 3, chapter.pointOfView);
}

void ChaptersNotebook::clearAll() {
    grid->clearAll();
}
