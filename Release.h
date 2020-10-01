#pragma once

#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextprint.h"

#include "MainFrame.h"

class MainFrame;

class Release : public wxPanel {
private:
    MainFrame* parent;

    wxRichTextParagraph* para = nullptr;
    wxRichTextPlainText* text = nullptr;
    wxRichTextPrinting* print = nullptr;
    wxPanel* pan = nullptr;

    wxBoxSizer* pagesSizer = nullptr;

public:
    Release(wxWindow* parent);
    ~Release();

    void updateContent();

    void next(wxCommandEvent& event);
    void previous(wxCommandEvent& event);
    void keyPressed(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    BUTTON_NextPage,
    BUTTON_PreviousPage
};
