#ifndef UTILITY_AM_H_
#define UTILITY_AM_H_
#pragma once

#include <wx\window.h>
#include <wx\splitter.h>
#include <wx\image.h>
#include <wx\dataview.h>

#include <wx\wxsf\wxShapeFramework.h>

class amSplitterWindow : public wxSplitterWindow {
public:
    amSplitterWindow(wxWindow* parent,
        wxWindowID 	id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxSP_NOBORDER | wxSP_NO_XP_THEME | wxSP_THIN_SASH,
        const wxString& name = "splitterWindow") : wxSplitterWindow(parent, id, pos, size, style, name) {}

    inline virtual void OnDoubleClickSash(int x, int y) {}
};


inline wxImage amGetScaledImage(int maxWidth, int maxHeight, wxImage& image) {
    int width = image.GetWidth();
    int height = image.GetHeight();

    double ratio;

    int neww, xoffset;
    int newh, yoffset;

    ratio = (double)width / height;

    if (width > height) {
        neww = maxWidth;
        newh = maxWidth / ratio;
        xoffset = 0;
        yoffset = (maxHeight / 2) - (newh / 2);
    } else {
        newh = maxHeight;
        neww = maxHeight * ratio;
        yoffset = 0;
        xoffset = (maxWidth / 2) - (neww / 2);
    }

    return image.Scale(neww, newh, wxIMAGE_QUALITY_HIGH).Size(wxSize(maxWidth, maxHeight), wxPoint(xoffset, yoffset));
}

void amDoSubWindowFullscreen(wxWindow* window, wxWindow* originalParent, bool fs,
    wxBoxSizer* originalSizer = nullptr, wxSizerFlags flags = { 0 }, int sizerInex = -1);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////// wxDVC Classes ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////


class amTreeModelNode {
protected:
    amTreeModelNode* m_parent = nullptr;
    wxVector<amTreeModelNode*> m_children{};

    wxString m_title{ "" };

    wxDataViewItemAttr m_attr{};
    bool m_isContainer = false;

public:
    amTreeModelNode(amTreeModelNode* parent, const wxString& title) {
        m_parent = parent;
        m_title = title;
    }

    inline virtual ~amTreeModelNode() {
        for (amTreeModelNode*& child : m_children) {
            if (child)
                delete child;
        }
    }

    inline const wxString& GetTitle() const { return m_title; }
    inline void SetTitle(const wxString& title) { m_title = title; }

    inline bool IsContainer() const { return m_isContainer; }
    inline virtual void SetContainer(bool isContainer) { m_isContainer = isContainer; }

    inline virtual void Reparent(amTreeModelNode* newParent) {
        if (m_parent)
            RemoveSelfFromParentList();

        m_parent = newParent;

        if (m_parent)
            m_parent->Append(this);
    }

    inline virtual void Reparent(amTreeModelNode* newParent, int n) {
        if (m_parent)
            RemoveSelfFromParentList();

        m_parent = newParent;

        if (m_parent)
            m_parent->Insert(this, n);
    }

    inline virtual void RemoveSelfFromParentList() {
        if (m_parent) {
            for (amTreeModelNode*& node : m_parent->GetChildren()) {
                if (node == this)
                    m_parent->GetChildren().erase(&node);
            }
        }

    }

    inline virtual void Reposition(unsigned int n) {}

    inline virtual amTreeModelNode* GetParent() {
        return m_parent;
    }

    inline virtual wxVector<amTreeModelNode*>& GetChildren() {
        return m_children;
    }

    inline virtual amTreeModelNode* GetChild(unsigned int n) {
        return m_children.at(n);
    }

    inline virtual wxDataViewItemAttr& GetAttr() { return m_attr; }

    inline virtual void Insert(amTreeModelNode* child, unsigned int n) {
        int i = 0;
        for (amTreeModelNode*& node : m_children) {
            if (i++ == n) {
                m_children.insert(&node, child);
            }
        }
    }

    inline virtual void Append(amTreeModelNode* child) {
        m_children.push_back(child);
    }

    inline virtual int GetChildCount() const {
        return m_children.size();
    }

#ifdef __WXMSW__
protected:
    bool m_isHovering = false;
    static wxDataViewItemAttr m_hoverAttr;

public:
    inline void SetHovering(bool isHovering) { m_isHovering = isHovering; }
    inline virtual wxDataViewItemAttr& GetHoverAttr() { return m_hoverAttr; }

    inline virtual bool IsHovering() const { return m_isHovering; }
#endif
};


class amDataViewModel : public wxDataViewModel {
protected:
    wxVector<amTreeModelNode*> m_otherRoots{};
    wxDataViewItem m_itemForDnD{ nullptr };

public:
    inline amDataViewModel() = default;
    inline virtual ~amDataViewModel() {
        for (amTreeModelNode*& node : m_otherRoots) {
            if (node)
                delete node;
        }
    }

    inline virtual const wxString& GetTitle(const wxDataViewItem& item) const {
        if (!item.IsOk())
            return wxEmptyString;

        return ((amTreeModelNode*)item.GetID())->GetTitle();
    }
    
    inline virtual void SetItemBackgroundColour(const wxDataViewItem& item, const wxColour& colour) {
        amTreeModelNode* node = (amTreeModelNode*)item.GetID();
        wxDataViewItemAttr& attr = node->GetAttr();

        attr.SetBackgroundColour(colour);
    }
    
    inline virtual void SetItemForegroundColour(const wxDataViewItem& item, const wxColour& colour) {
        amTreeModelNode* node = (amTreeModelNode*)item.GetID();
        wxDataViewItemAttr& attr = node->GetAttr();

        attr.SetColour(colour);
    }
    
    inline virtual void SetItemFont(const wxDataViewItem& item, const wxFont& font) {
        amTreeModelNode* node = (amTreeModelNode*)item.GetID();
        wxDataViewItemAttr& attr = node->GetAttr();

        attr.SetBold(font.GetWeight() >= 400);
        attr.SetItalic(font.GetStyle() == wxFONTSTYLE_ITALIC);
        attr.SetStrikethrough(font.GetStrikethrough());
    }

    inline virtual bool IsDescendant(const wxDataViewItem& item, const wxDataViewItem& descendant) const {
        wxDataViewItem& parent = GetParent(descendant);

        bool is = false;

        while (parent.IsOk()) {
            if (parent.GetID() == item.GetID())
                is = true;

            parent = GetParent(parent);
        }

        return is;
    }

    inline virtual bool Reparent(const wxDataViewItem item, const wxDataViewItem newParent) {
        amTreeModelNode* oldParentNode = (amTreeModelNode*)GetParent(item).GetID();
        amTreeModelNode* itemNode = (amTreeModelNode*)item.GetID();
        amTreeModelNode* newParentNode = (amTreeModelNode*)newParent.GetID();

        wxDataViewItem oldParent((void*)oldParentNode);


        if (!item.IsOk())
            return false;

        itemNode->Reparent(newParentNode);

        if (!oldParentNode) {
            for (amTreeModelNode*& node : m_otherRoots) {
                if (node == itemNode)
                    m_otherRoots.erase(&node);
            }
        }

        if (!newParentNode)
            m_otherRoots.push_back(itemNode);

        ItemAdded(newParent, item);
        ItemDeleted(oldParent, item);
        return true;
    }

    inline virtual bool Reparent(const wxDataViewItem item, const wxDataViewItem newParent, int n) {
        amTreeModelNode* oldParentNode = (amTreeModelNode*)GetParent(item).GetID();
        amTreeModelNode* itemNode = (amTreeModelNode*)item.GetID();
        amTreeModelNode* newParentNode = (amTreeModelNode*)newParent.GetID();

        wxDataViewItem oldParent((void*)oldParentNode);


        if (!item.IsOk())
            return false;

        itemNode->Reparent(newParentNode);

        if (!oldParentNode) {
            for (amTreeModelNode*& node : m_otherRoots) {
                if (node == itemNode)
                    m_otherRoots.erase(&node);
            }
        }

        if (!newParentNode) {
            int i = 0;
            for (amTreeModelNode*& node : m_otherRoots) {
                if (i++ == n)
                    m_otherRoots.insert(&node, itemNode);
            }
        }

        ItemAdded(newParent, item);
        ItemDeleted(oldParent, item);
        return true;
    }

    inline virtual bool SetValue(const wxVariant& variant,
        const wxDataViewItem& item, unsigned int col) {

        amTreeModelNode* node = (amTreeModelNode*)item.GetID();
        wxDataViewIconText it;

        switch (col) {
        case 0:
            it << variant;
            node->SetTitle(it.GetText());
            return true;

        default:
            break;
        }
        return false;
    }

    inline virtual wxDataViewItem GetParent(const wxDataViewItem& item) const {
        // The invisible root node has no parent
        if (!item.IsOk())
            return wxDataViewItem(0);

        amTreeModelNode* node = (amTreeModelNode*)item.GetID();
        return wxDataViewItem(node->GetParent());
    }

    inline virtual bool IsContainer(const wxDataViewItem& item) const {
        // the invisible root node can have children
        if (!item.IsOk())
            return true;

        amTreeModelNode* node = (amTreeModelNode*)item.GetID();
        return node->IsContainer();
    }

    inline virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const {
        amTreeModelNode* node = (amTreeModelNode*)item.GetID();

        if (node) {
            if (m_itemForDnD == item) {
                attr.SetColour(wxColour(0, 0, 0));
            } else {
                if (node->IsHovering())
                    attr = node->GetHoverAttr();
                else
                    attr = node->GetAttr();
            }

            return true;
        } else {
            return false;
        }
    }

    ////////////////////////////// DragEventHandlers ////////////////////////////

    inline virtual void OnBeginDrag(wxDataViewEvent& event, wxDataViewCtrl* dvc) = 0;
    inline virtual void OnDropPossible(wxDataViewEvent& event, wxDataViewCtrl* dvc) = 0;
    inline virtual void OnDrop(wxDataViewEvent& event, wxDataViewCtrl* dvc) = 0;

    inline void UnsetItemForDnD() { if (m_itemForDnD.IsOk()) m_itemForDnD.Unset(); }
};


#ifdef __WXMSW__
class amHotTrackingDVCHandler : public wxEvtHandler {
private:
    wxDataViewCtrl* m_dvc = nullptr;
    amDataViewModel* m_model;

    wxDataViewItem m_itemUnderMouse{ nullptr };

public:
    inline amHotTrackingDVCHandler() = default;
    inline amHotTrackingDVCHandler(wxWindow* parent, wxWindowID id, amDataViewModel* model, long style = wxDV_NO_HEADER | wxDV_SINGLE | wxBORDER_NONE) {
        Create(parent, id, model, style);
    }

    inline wxDataViewCtrl* Create(wxWindow* parent, wxWindowID id, amDataViewModel* model, long style = wxDV_NO_HEADER | wxDV_SINGLE | wxBORDER_NONE) {
        m_dvc = new wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize,
            style);
        m_dvc->AssociateModel(model);
        BindEvents();

        m_model = model;
        return m_dvc;
    }

    inline void BindEvents() {
        wxWindow* mainWindow = m_dvc->GetMainWindow();

        mainWindow->Bind(wxEVT_MOTION, &amHotTrackingDVCHandler::OnDVCMouseMove, this);
        mainWindow->Bind(wxEVT_LEAVE_WINDOW, &amHotTrackingDVCHandler::OnDVCLeaveWindow, this);
        mainWindow->Bind(wxEVT_LEFT_DOWN, &amHotTrackingDVCHandler::OnDVCClick, this);
        mainWindow->Bind(wxEVT_RIGHT_DOWN, &amHotTrackingDVCHandler::OnDVCClick, this);

        m_dvc->Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &amHotTrackingDVCHandler::OnBeginDrag, this);
        m_dvc->Bind(wxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, &amHotTrackingDVCHandler::OnDropPossible, this);
        m_dvc->Bind(wxEVT_DATAVIEW_ITEM_DROP, &amHotTrackingDVCHandler::OnDrop, this);
    }

    inline wxDataViewCtrl* GetDVC() { return m_dvc; }

    inline void CalculateItemRect(wxDataViewItem& item, wxRect& rect) {
        rect = m_dvc->GetItemRect(item);
        rect.width += rect.GetLeft();
        rect.SetLeft(0);
    }

    inline void OnDVCMouseMove(wxMouseEvent& event) {
        wxDataViewItem item;
        wxDataViewColumn* column;
        m_dvc->HitTest(event.GetPosition(), item, column);

        wxRect rect;

        if (item.IsOk()) {
            if (item.GetID() != m_itemUnderMouse.GetID()) {
                if (m_itemUnderMouse.IsOk()) {
                    ((amTreeModelNode*)m_itemUnderMouse.GetID())->SetHovering(false);
                    CalculateItemRect(m_itemUnderMouse, rect);
                    m_dvc->GetMainWindow()->RefreshRect(rect);
                }

                ((amTreeModelNode*)item.GetID())->SetHovering(true);
                CalculateItemRect(item, rect);
                m_dvc->GetMainWindow()->RefreshRect(rect);
                m_itemUnderMouse = item;
            }
        } else {
            if (m_itemUnderMouse.IsOk()) {
                ((amTreeModelNode*)m_itemUnderMouse.GetID())->SetHovering(false);
                CalculateItemRect(m_itemUnderMouse, rect);
                m_dvc->GetMainWindow()->RefreshRect(rect);
            }

            m_itemUnderMouse = item;
        }

        event.Skip();
    }

    inline void OnDVCLeaveWindow(wxMouseEvent& event) {
        if (m_itemUnderMouse.IsOk()) {
            ((amTreeModelNode*)m_itemUnderMouse.GetID())->SetHovering(false);
            wxRect rect;
            CalculateItemRect(m_itemUnderMouse, rect);
            m_dvc->GetMainWindow()->RefreshRect(rect);
        }

        event.Skip();
    }

    inline void OnDVCClick(wxMouseEvent& event) {
        OnDVCMouseMove(event);
        event.Skip();
    }

    inline void OnBeginDrag(wxDataViewEvent& event) {
        m_model->OnBeginDrag(event, m_dvc);
    }
    inline void OnDropPossible(wxDataViewEvent& event) {
        m_model->OnDropPossible(event, m_dvc);
    }
    inline void OnDrop(wxDataViewEvent& event) {
        m_model->OnDrop(event, m_dvc);
    }
};
#endif /*__WXMSW__*/

class amSFShapeCanvas : public wxSFShapeCanvas {
protected:
    bool m_beginDraggingRight = false;
    bool m_isDraggingRight = false;
    wxPoint m_rightDownPos{};
    wxPoint m_scrollbarPos{};

public:
    inline amSFShapeCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxHSCROLL | wxVSCROLL) : wxSFShapeCanvas(manager, parent, id, pos, size, style) {
        Bind(wxEVT_MOUSE_CAPTURE_LOST, &amSFShapeCanvas::OnMouseCaptureLost, this);
        Bind(wxEVT_SCROLLWIN_BOTTOM, &amSFShapeCanvas::OnScroll, this);
        Bind(wxEVT_SCROLLWIN_LINEDOWN, &amSFShapeCanvas::OnScroll, this);
        Bind(wxEVT_SCROLLWIN_LINEUP, &amSFShapeCanvas::OnScroll, this);
        Bind(wxEVT_SCROLLWIN_PAGEDOWN, &amSFShapeCanvas::OnScroll, this);
        Bind(wxEVT_SCROLLWIN_PAGEUP, &amSFShapeCanvas::OnScroll, this);
        Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &amSFShapeCanvas::OnScroll, this);
        Bind(wxEVT_SCROLLWIN_THUMBTRACK, &amSFShapeCanvas::OnScroll, this);
        Bind(wxEVT_SCROLLWIN_TOP, &amSFShapeCanvas::OnScroll, this);

    }

    inline virtual void OnMouseMove(wxMouseEvent& event) {
        if (m_beginDraggingRight) {
            m_isDraggingRight = true;
            m_beginDraggingRight = false;
            SetCursor(wxCursor(wxCURSOR_CLOSED_HAND));
        }

        if (m_isDraggingRight) {
            wxPoint toMove = m_rightDownPos - wxGetMousePosition();

            int px, py;
            GetScrollPixelsPerUnit(&px, &py);
            Scroll((m_scrollbarPos.x + toMove.x) / px, (m_scrollbarPos.y + toMove.y) / py);
        }
    
        wxSFShapeCanvas::OnMouseMove(event);
    }

    inline virtual void OnMouseCaptureLost(wxMouseCaptureLostEvent& event) {
        m_isDraggingRight = false;
        m_beginDraggingRight = false;
    }

    inline virtual void OnRightDown(wxMouseEvent& event) {
        // Set begin dragging state as true and calculate current mouse position and current
        // scrollbar position.
        m_beginDraggingRight = true;
        m_rightDownPos = wxGetMousePosition();
        CalcUnscrolledPosition(0, 0, &m_scrollbarPos.x, &m_scrollbarPos.y);

        CaptureMouse();
        event.Skip();
    }

    inline virtual void OnRightUp(wxMouseEvent& event) {
        // Reset everything.
        if (m_isDraggingRight) {
            m_isDraggingRight = false;
            SetCursor(wxCursor(wxCURSOR_DEFAULT));
        }

        if (HasCapture())
            ReleaseCapture();

        m_beginDraggingRight = false;
        event.Skip();
    }

    inline virtual void OnScroll(wxScrollWinEvent& event) {    
        if (wxGetKeyState(WXK_COMMAND))
            return;

        event.Skip();
    }
};


#endif; /*UTILITY_AM_H_*/