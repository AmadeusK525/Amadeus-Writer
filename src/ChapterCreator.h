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
    amdProjectManager* m_manager = nullptr;

    wxPanel* m_nchapPanel1 = nullptr,
        * m_nchapPanel2 = nullptr;

    wxPanel* m_btnPanel = nullptr;

    wxTextCtrl* m_nchapName = nullptr;
    wxTextCtrl* m_nchapSummary = nullptr;

    amdDragList* m_nchapList = nullptr;

    wxButton* m_nchapNext = nullptr,
        * m_nchapBack = nullptr,
        * m_nchapCancel = nullptr;

    wxBoxSizer* m_mainHor = nullptr;

    string m_tempName{};
    bool m_firstNext = true;

public:
    ChapterCreator(wxWindow* parent, amdProjectManager* manager);

    void Next(wxCommandEvent& event);
    void Back(wxCommandEvent& event);
    void Cancel(wxCommandEvent& event);
    void Create(wxCommandEvent& event);

    void CheckClose(wxCloseEvent& event);

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
