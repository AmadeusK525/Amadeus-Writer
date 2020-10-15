#pragma once

#include "wx/panel.h"
#include "wx/toolbar.h"
#include "wx/dcclient.h"

#include <vector>
#include <memory>

class NotesBox;
class MovableNote;
class CorkboardImage;
class ImagePanel;

class Corkboard;
class Outline;

using std::vector;

class Corkboard : public wxPanel {
private:
    Outline* parent = nullptr;

    wxToolBar* toolBar = nullptr;
    wxPanel* pan = nullptr;

    bool startDragging = false;
    bool draggingLeft = false;
    bool draggingRight = false;
    int x;
    int y;

    int zoom = 100;
    int prevZoom = 100;


public:
    ImagePanel* main = nullptr;

    bool isConnectingA = false;
    bool isConnectingB = false;

    int curNote = 0;
    int curBox = 0;
    int curImage = 0;

    vector<MovableNote*> notes;
    //vector<std::pair<wxPoint, wxPoint>> points;
    vector<NotesBox*> boxes;
    vector<CorkboardImage*> images;

    MovableNote* connectingA = nullptr;
    MovableNote* connectedA = nullptr;
    CorkboardImage* connectingB = nullptr;
    CorkboardImage* connectedB = nullptr;

public:
    Corkboard(wxWindow* parent);

    void initMain(const wxSize& size, wxImage& image);

    void addNote(wxCommandEvent& event);
    void addImage(wxCommandEvent& event);
    void doDeleteNote(wxWindowID id);
    void doDeleteBox(wxWindowID id);
    void doDeleteImage(wxWindowID id);

    void resetCenter(wxCommandEvent& event);

    void onLeftDown(wxMouseEvent& event);
    void onRightDown(wxMouseEvent& event);
    void onDoubleClick(wxMouseEvent& event);
    void onMouseUp(wxMouseEvent& event);
    void onMove(wxMouseEvent& event);
    void onScrollDown(wxScrollWinEvent& event);
    void onScrollUp(wxScrollWinEvent& event);

    void doZoom();

    void onMouseCaptureLost(wxMouseCaptureLostEvent& event);

    void paintMain(wxPaintEvent& event);
    void paintBoxes(wxPaintDC& dc, NotesBox* note);

    DECLARE_EVENT_TABLE()
};

enum {
    TOOL_NewNote,
    TOOL_NewImage,
    TOOL_ResetCenter
};
