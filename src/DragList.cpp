#include "DragList.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(DragList, wxListView)

EVT_LEFT_DOWN(DragList::onLeftDown)
EVT_LEFT_UP(DragList::onLeftUp)
EVT_LEAVE_WINDOW(DragList::onLeaveWindow)
EVT_ENTER_WINDOW(DragList::selectItem)
EVT_MOTION(DragList::onMouseMotion)

END_EVENT_TABLE()

DragList::DragList(wxWindow* parent, const wxSize& size) :
    wxListView(parent, -1, wxDefaultPosition, size, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxBORDER_DOUBLE) {
    this->parent = parent;

    posMarker = new wxPanel(this, -1, wxPoint(0, GetSize().y / 2), wxSize(size.x, 2));
    posMarker->Hide();
    posMarker->SetBackgroundColour(wxColour(10, 10, 10, 100));

    SetDoubleBuffered(true);
}

void DragList::onLeftDown(wxMouseEvent& evt) {
    state = DRAG_START;
}

void DragList::onLeftUp(wxMouseEvent& evt) {
    wxPoint evtPos = evt.GetPosition();
    int listFlags = wxLIST_HITTEST_ONITEM;
    int itemUnderMouse = HitTest(evtPos, listFlags), insert;

    if (state == DRAGGING) {
        wxRect rect;
        GetItemRect(itemUnderMouse, rect, wxLIST_RECT_BOUNDS);

        if (itemUnderMouse == -1) {
            if (evtPos.y > GetPosition().y) {
                insert = GetItemCount();
            } else {
                insert = 0;
            }

        } else {
            if (evtPos.y > (rect.GetTop() + rect.GetHeight() / 2)) {
                // below
                insert = itemUnderMouse + 1;
            } else {
                // above
                insert = itemUnderMouse;
            }
        }
        InsertItem(insert, tempName);
        selectItem(evt);
    }
    endDragging();
}

void DragList::onLeaveWindow(wxMouseEvent& evt) {
    if (dragImage) {
        endDragging();
        InsertItem(previous, tempName);
        selectItem(evt);
    }
}

void DragList::onMouseMotion(wxMouseEvent& evt) {
    this;
    wxPoint evtPos = evt.GetPosition();
    int listFlags = wxLIST_HITTEST_ONITEMICON | wxLIST_HITTEST_ONITEMLABEL | wxLIST_HITTEST_ONITEMSTATEICON;

    // switch to dragging mode if mouseDown was on item
    if (state == DRAG_START) {
        itemDragging = HitTest(evtPos, listFlags);
        if (itemDragging == FindItem(-1, tempName)) {
            previous = itemDragging;
            // create dragImage
            if (dragImage) {
                delete dragImage;
            }

            dragImage = new wxDragImage(GetItemText(itemDragging));

            // position the dragImage right on top of the dragged item and
            // start dragging
            wxRect rect;
            GetItemRect(itemDragging, rect, wxLIST_RECT_BOUNDS);
            dragImage->BeginDrag(evtPos - rect.GetLeftTop(), this);
            dragImage->Show();

            posMarker->SetPosition(rect.GetTopLeft());

            state = DRAGGING;
            SetCursor(wxCursor(wxCURSOR_CLOSED_HAND));
            DeleteItem(previous);
        } else {
            state = DRAG_NONE;
        }
    }

    // show markers only when dragging over an item
    bool showMarkers = state == DRAGGING;

    if (showMarkers) {
        wxRect rect;
        int itemHovered = HitTest(evtPos, listFlags);

        if (itemHovered != -1)
            GetItemRect(itemHovered, rect, wxLIST_RECT_BOUNDS);

        if (evtPos.y > rect.GetTop() + rect.GetHeight() / 2) {
            // below
            posMarker->SetPosition(rect.GetBottomLeft());
        } else {
            // above
            posMarker->SetPosition(rect.GetTopLeft());
        }

        dragImage->Hide();

        if (posMarker->GetPosition().y > 20) {
            posMarker->Show();
        } else {
            posMarker->Hide();
        }

        Refresh(false);
        Update();

        dragImage->Show();
        dragImage->Move(evtPos);
    }
}


void DragList::endDragging() {
    state = DRAG_NONE;

    if (dragImage) {
        dragImage->EndDrag();
        delete dragImage;
        dragImage = nullptr;
    }

    posMarker->Hide();
    posMarker->SetPosition(wxPoint(0, GetSize().y / 2));
    SetCursor(wxCursor(wxCURSOR_DEFAULT));

    Refresh(false);
    Update();

    selectItem(wxMouseEvent());
    SetFocus();
}

void DragList::selectItem(wxMouseEvent& evt) {
    for (int i = 0; i < GetItemCount(); i++) {
        Select(i, false);
    }

    Select(FindItem(-1, tempName));
    EnsureVisible(FindItem(-1, tempName));
}
