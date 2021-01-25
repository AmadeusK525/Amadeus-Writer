#pragma once

#include "wx/listctrl.h"
#include "wx/panel.h"
#include "wx/dragimag.h"

#include <string>

using std::string;

class amdDragList : public wxListView {
private:
    int m_itemDragging{};
    int m_previous{};
    wxPanel* m_posMarker = nullptr;

public:
    string m_tempName{ "" };

    typedef enum {
        DRAG_NONE,
        DRAG_START,
        DRAGGING
    } LDragState;

private:
    LDragState state = DRAG_NONE;

    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);
    void OnMouseMotion(wxMouseEvent& evt);
    void EndDragging();

public:
    amdDragList(wxWindow* parent, const wxSize& size);

    void SelectItem(wxMouseEvent& evt);

    DECLARE_EVENT_TABLE()
};

