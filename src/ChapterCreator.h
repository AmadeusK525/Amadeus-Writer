#ifndef CHAPTERCREATOR_H_
#define CHAPTERCREATOR_H_

#pragma once

#include "wx/editlbox.h"

#include <string>

#include "DragList.h"
#include "ChaptersGrid.h"

using std::string;

class ChapterCreator: public wxFrame {
private:
    MainFrame* m_mainFrame = nullptr;
    ChaptersNotebook* m_notebook = nullptr;

    wxPanel* m_nchapPanel1 = nullptr,
        * m_nchapPanel2 = nullptr;

    wxPanel* m_btnPanel = nullptr;

    wxTextCtrl* m_nchapName = nullptr;
    wxTextCtrl* m_nchapSummary = nullptr;

    DragList* m_nchapList = nullptr;

    wxButton* m_nchapNext = nullptr,
        * m_nchapBack = nullptr,
        * m_nchapCancel = nullptr;

    wxBoxSizer* m_mainHor = nullptr;

    string m_tempName{};

    bool m_firstNext = true;

public:
    ChapterCreator(wxWindow* parent, ChaptersNotebook* notebook);

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
