#pragma once

#include "wx/panel.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/sizer.h"
#include "wx/dcclient.h"
#include "wx/menu.h"

#include <vector>
#include <fstream>

#include "ImagePanel.h"
#include "Corkboard.h"

using std::vector;

class MovableNote : public wxPanel {
private:
    ImagePanel* parent = nullptr;
    Corkboard* corkboard = nullptr;

    wxPanel* handle = nullptr;
    wxPanel* arrow = nullptr;
    wxRichTextCtrl* content = nullptr;

    std::string text{ "" };
    int curColour;

    bool dragging = false;
    bool resizing = false;
    int x;
    int y;

public:
    wxSize size;
    wxPoint pos;

    wxArrayInt conAIds;
    wxArrayInt conBIds;
    wxArrayInt backConAIds;
    wxArrayInt backConBIds;

    vector<MovableNote*> conA;
    vector<MovableNote*> backConA;
    vector<CorkboardImage*> conB;
    vector<CorkboardImage*> backConB;

    wxCheckBox* outBut = nullptr;
    wxCheckBox* inBut = nullptr;

public:
    MovableNote(wxWindow* parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int id = 0);

    void initNote();

    void onMouseDown(wxMouseEvent& event);
    void onMouseUp(wxMouseEvent& event);
    void onMove(wxMouseEvent& event);
    void onLoseMouseCapture(wxMouseCaptureLostEvent& event) {};

    void onRightUp(wxMouseEvent& event);

    void changeColour(wxCommandEvent& event);
    void deleteThis(wxCommandEvent& event);

    void paintArrow(wxPaintEvent& event);
    void arrowDown(wxMouseEvent& event);

    void rightIn(wxMouseEvent& event);
    void rightOut(wxMouseEvent& event);
    void outDown(wxMouseEvent& event);

    void cancelIn(wxCommandEvent& event);
    void cancelOut(wxCommandEvent& event);
    void clearConnections(wxCommandEvent& event);
    void clearBackConnections(wxCommandEvent& event);

    wxPoint& getOutConPos() { return wxPoint(pos.x + size.x, pos.y + 8); }
    wxPoint& getInConPos() { return wxPoint(pos.x, pos.y + 8); }
    void connectNote(MovableNote* connection);
    void connectImage(CorkboardImage* connection);
    void connectBackNote(MovableNote* connection);
    void connectBackImage(CorkboardImage* connection);

    void save(std::ofstream& out);
    void load(std::ifstream& in);

    DECLARE_EVENT_TABLE()
};

enum {
    MENU_NoteRed,
    MENU_NoteGreen,
    MENU_NoteBlue,
    MENU_NotePink,
    MENU_NoteYellow,
    MENU_NoteWhite,
    MENU_NoteBlack,
    MENU_NoteDefault,
    MENU_NoteDelete,

    CHECK_In,
    CHECK_Out,

    MENU_ClearConnections,
    MENU_ClearBackConnections
};
