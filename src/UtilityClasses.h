#ifndef UTILITYCLASSES_AM_H_
#define UTILITYCLASSES_AM_H_
#pragma once

#include <wx\splitter.h>

class amSplitterWindow : public wxSplitterWindow {
public:
    amSplitterWindow(wxWindow* parent,
        wxWindowID 	id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxSP_NOBORDER | wxSP_NO_XP_THEME | wxSP_THIN_SASH,
        const wxString& name = "splitterWindow") : wxSplitterWindow(parent, id, pos, size, style, name) {}

    virtual void OnDoubleClickSash(int x, int y) {}
};
#endif;