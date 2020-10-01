#pragma once

#include "wx/listctrl.h"
#include "wx/panel.h"
#include "wx/dragimag.h"

#include <string>

class DragList : public wxListView {
private:
    int itemDragging{};
    int previous{};
    wxPanel* posMarker = nullptr;
    wxWindow* parent = nullptr;

public:
    wxDragImage* dragImage = nullptr;
    std::string tempName{};

    typedef enum {
        DRAG_NONE,
        DRAG_START,
        DRAGGING
    } LDragState;

private:
    LDragState state = DRAG_NONE;

    void onLeftDown(wxMouseEvent& evt);
    void onLeftUp(wxMouseEvent& evt);
    void onLeaveWindow(wxMouseEvent& evt);
    void onMouseMotion(wxMouseEvent& evt);
    void endDragging();

    void selectItem(wxMouseEvent& evt);

public:
    DragList(wxWindow* parent, const wxSize& size);

    DECLARE_EVENT_TABLE()
};

