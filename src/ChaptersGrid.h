#ifndef CHAPTERSGRID_H_
#define CHAPTERSGRID_H_
#pragma once

#include "wx/scrolwin.h"
#include "wx/wrapsizer.h"
#include "wx/button.h"

#include "ChaptersNotebook.h"

class ChapterGrid: public wxScrolledWindow {
private:
    amdProjectManager* m_manager = nullptr;

    wxVector<wxButton*> m_buttons{};
    wxWrapSizer* m_btnSizer = nullptr;

public:
    ChapterGrid(wxWindow* parent, amdProjectManager* manager);

    void AddButton();
    void OpenChapter(unsigned int chapterNumber);

    void OnButtonPressed(wxCommandEvent& event);

    void ClearAll();
};

#endif