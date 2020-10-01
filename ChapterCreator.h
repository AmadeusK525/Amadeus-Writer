#pragma once

#include "wx/editlbox.h"

#include <string>

#include "DragList.h"
#include "MainFrame.h"
#include "ChaptersGrid.h"

#ifndef CHAPTERCREATOR_H_
#define CHAPTERCREATOR_H_

class ChapterCreator: public wxFrame {
private:
    MainFrame* parent = nullptr;
    ChaptersGrid* grid = nullptr;

    wxPanel* nchapPanel1 = nullptr;
    wxPanel* nchapPanel2 = nullptr;

    wxStaticText* mainLabel = nullptr;

    wxTextCtrl* nchapName = nullptr;
    wxTextCtrl* nchapSummary = nullptr;

    DragList* nchapList = nullptr;

    wxButton* nchapNext = nullptr;
    wxButton* nchapBack = nullptr;
    wxButton* nchapCancel = nullptr;

    std::string tempName{};

    bool firstNext = true; 

public:
    ChapterCreator(wxWindow* parent, ChaptersGrid* grid);

    void next(wxCommandEvent& event);
    void back(wxCommandEvent& event);
    void cancel(wxCommandEvent& event);
    void create(wxCommandEvent& event);

    void checkClose(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    LIST_EditableChapter,

    BUTTON_NextChapter,
    BUTTON_CancelChapter,
    BUTTON_BackChapter,
    BUTTON_CreateChapter,
};



#endif
