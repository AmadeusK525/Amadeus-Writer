#pragma once

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextprint.h>

#include "Project.h"


class amdRelease : public wxPanel {
private:
    amdProjectManager* m_manager = nullptr;

    //wxRichTextPrinting* print = nullptr;
    wxPanel* m_panel = nullptr;
    wxBoxSizer* m_pagesSizer = nullptr;

public:
    amdRelease(wxWindow* parent);
    ~amdRelease();

    void UpdateContent();

    void OnNext(wxCommandEvent& event);
    void OnPrevious(wxCommandEvent& event);
    void OnKeyPressed(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    BUTTON_NextPage,
    BUTTON_PreviousPage
};
