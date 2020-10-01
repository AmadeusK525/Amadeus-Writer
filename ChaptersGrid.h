#pragma once

#include "wx/scrolwin.h"
#include "wx/wrapsizer.h"
#include "wx/button.h"

#include "Chapter.h"
#include "ChapterWriter.h"

#include "vector"
#include "list"
#include "boost/filesystem.hpp"

using std::vector;
using std::list;

class MainFrame;

class ChaptersGrid: public wxScrolledWindow {
private:
    MainFrame* parent = nullptr;
    vector<wxButton*> chapterButtons{};

public:
    list<Chapter> chapters{};
    int current = 1;

    wxWrapSizer* sizer = nullptr;

public:
    ChaptersGrid(wxWindow* parent);

    void addChapter(Chapter& chapter, int& pos);
    void addButton();
    void openChapter(wxCommandEvent& event);

    void clearAll();

    list<Chapter> getChapters();
};

