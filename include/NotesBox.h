#pragma once

#include "wx/panel.h"
#include "wx/textctrl.h"
#include "wx/menu.h"
#include "wx/dcbuffer.h"
#include "wx/sizer.h"

#include <string>
#include <fstream>

#include "Corkboard.h"

class Corkboard;

class NotesBox : public wxPanel {
private:
    ImagePanel* parent = nullptr;
    Corkboard* corkboard = nullptr;
    wxTextCtrl* label = nullptr;

    wxPanel* arrow = nullptr;
    wxPanel* area = nullptr;

    std::string text = "New field";

    bool dragging = false;
    bool resizing = false;
    int x;
    int y;

public:
    int id;
    wxSize size;
    wxPoint pos;

    float xRatio;
    float yRatio;

    wxBoxSizer* s = nullptr;

public:
    NotesBox(wxWindow* parent, const wxPoint& pos, const wxSize& size, int num);
    NotesBox(const NotesBox& other);

    ~NotesBox();

    void initBox();

    void onLeftDown(wxMouseEvent& event);
    void onLeftUp(wxMouseEvent& event);
    void onRightUp(wxMouseEvent& event);
    void onMotion(wxMouseEvent& event);
    void onMouseCaptureLost(wxMouseCaptureLostEvent& event);

    void arrowDown(wxMouseEvent& event);

    void editLabel(wxCommandEvent& event);
    void doEditLabel(wxCommandEvent& event);
    void loseFocus(wxFocusEvent& event) { doEditLabel(wxCommandEvent()); };
    void deleteThis(wxCommandEvent& event);

    void paintBox(wxPaintEvent& event);
    void paintArrow(wxPaintEvent& event);

    void save(std::ofstream& out);
    void load(std::ifstream& in);

    void operator=(NotesBox& other);

    DECLARE_EVENT_TABLE()
};

enum {
    MENU_EditBoxLabel,
    MENU_DeleteBox
};
