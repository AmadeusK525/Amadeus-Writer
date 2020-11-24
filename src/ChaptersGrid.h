#ifndef CHAPTERSGRID_H_
#define CHAPTERSGRID_H_
#pragma once

#include "wx/scrolwin.h"
#include "wx/wrapsizer.h"
#include "wx/button.h"

#include "ChaptersNotebook.h"

#include <vector>

using std::vector;

class ChaptersGrid: public wxScrolledWindow {
private:
    ChaptersNotebook* parent = nullptr;
    MainFrame* mainFrame = nullptr;

    vector<wxButton*> chapterButtons{};
    wxWrapSizer* sizer = nullptr;

public:
    ChaptersGrid(wxWindow* parent);

    void addButton();
    void openChapter(wxCommandEvent& event);

    void clearAll();
};

#endif