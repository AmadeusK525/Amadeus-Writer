#include "NotesBox.h"

#include "ImagePanel.h"
#include "MainFrame.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(NotesBox, wxStaticBox)

EVT_LEFT_UP(NotesBox::onLeftUp)
EVT_MOTION(NotesBox::onMotion)

EVT_MENU(MENU_EditBoxLabel, NotesBox::editLabel)
EVT_MENU(MENU_DeleteBox, NotesBox::deleteThis)

EVT_MOUSE_CAPTURE_LOST(NotesBox::onMouseCaptureLost)

END_EVENT_TABLE()


NotesBox::NotesBox(wxWindow* parent, const wxPoint& pos, const wxSize& size, int num) :
    wxPanel(parent, num, pos, size) {
    this->parent = dynamic_cast<ImagePanel*>(parent);
    this->corkboard = dynamic_cast<Corkboard*>(parent->GetParent()->GetParent());
    this->id = num;
    x = 0;
    y = 0;
    this->size = size;
    this->pos = pos;
    xRatio = parent->GetSize().GetX() / pos.x;
    yRatio = parent->GetSize().GetY() / pos.y;

    //SetBackgroundColour(wxColour(220, 220, 220));
    SetFont(wxFont(wxFontInfo(11).Bold()));
    SetForegroundColour(wxColour(20, 20, 20));

    area = new wxPanel(this);
    area->Bind(wxEVT_PAINT, &NotesBox::paintBox, this);
    area->Bind(wxEVT_LEFT_DOWN, &NotesBox::onLeftDown, this);
    area->Bind(wxEVT_RIGHT_UP, &NotesBox::onRightUp, this);

    arrow = new wxPanel(this, -1, wxDefaultPosition, wxSize(9, 9));
    arrow->Bind(wxEVT_PAINT, &NotesBox::paintArrow, this);
    arrow->Bind(wxEVT_LEFT_DOWN, &NotesBox::arrowDown, this);
    arrow->SetCursor(wxCURSOR_SIZENWSE);

    s = new wxBoxSizer(wxVERTICAL);
    s->Add(area, wxSizerFlags(1).Expand());
    s->Add(arrow, wxSizerFlags(0).Right());

    SetSizer(s);

    arrow->Refresh();
    arrow->Update();
    Layout();
    s->Layout();
    Refresh();
    Update();
}

NotesBox::NotesBox(const NotesBox& other) :
    NotesBox(other.parent, other.GetPosition(), other.GetSize(), other.id) {
    this->x = other.x;
    this->y = other.y;
    this->text = other.text;
    xRatio = other.xRatio;
    yRatio = other.yRatio;
}

NotesBox::~NotesBox() {
    if (parent)
        parent = nullptr;

    if (label != nullptr)
        label = nullptr;
}

void NotesBox::initBox() {
    SetSize(size);
    SetPosition(pos);
}

void NotesBox::onLeftDown(wxMouseEvent& event) {
    CaptureMouse();
    x = event.GetX();
    y = event.GetY();
    dragging = true;
}

void NotesBox::onLeftUp(wxMouseEvent& event) {
    if (dragging) {
        dragging = false;
        pos = GetPosition();
    }

    if (resizing) {
        resizing = false;
        size = GetSize();
    }

    if (HasCapture())
        ReleaseMouse();

    parent->Refresh();
    parent->Update();
    SetFocus();
}

void NotesBox::onRightUp(wxMouseEvent& event) {
    wxMenu menu;

    menu.Append(MENU_EditBoxLabel, "Edit label");
    menu.Append(MENU_DeleteBox, "Delete");

    PopupMenu(&menu);
}

void NotesBox::onMotion(wxMouseEvent& event) {
    wxPoint mouseOnScreen = wxGetMousePosition();

    if (dragging) {
        int newx = mouseOnScreen.x - x;
        int newy = mouseOnScreen.y - y;
        this->Move(GetParent()->ScreenToClient(wxPoint(newx, newy)));
        pos = GetPosition();
        parent->Refresh();
        parent->Update();
        MainFrame::isSaved = false;
    }

    if (resizing) {
        wxPoint pos = parent->ClientToScreen(GetPosition());
        wxSize s(mouseOnScreen.x - pos.x, mouseOnScreen.y - pos.y);
        SetSize(s);
        size = s;

        if (GetSize().x < 50 || GetSize().y < 50) {
            SetSize(50, 50);
            size = wxSize(50, 50);
        }

        Refresh();
        Update();
        MainFrame::isSaved = false;
    }
}

void NotesBox::onMouseCaptureLost(wxMouseCaptureLostEvent& event) {
    if (resizing)
        resizing = false;

    if (dragging)
        dragging = false;
}

void NotesBox::arrowDown(wxMouseEvent& event) {
    CaptureMouse();
    x = event.GetX();
    y = event.GetY();
    resizing = true;
}

void NotesBox::editLabel(wxCommandEvent& event) {
    if (label)
        delete label;

    label = new wxTextCtrl(area, -1, text, wxPoint(0, 0), wxSize(GetSize().x, 30), wxTE_PROCESS_ENTER);
    label->SetFont(wxFont(wxFontInfo(11).Bold()));
    label->Bind(wxEVT_KILL_FOCUS, &NotesBox::loseFocus, this);
    label->Bind(wxEVT_TEXT_ENTER, &NotesBox::doEditLabel, this);
}

void NotesBox::doEditLabel(wxCommandEvent& event) {
    if (label) {
        text = label->GetValue();
        label->Destroy();

        parent->Refresh();
        parent->Update();

        MainFrame::isSaved = false;
    }
}

void NotesBox::deleteThis(wxCommandEvent& event) {
    corkboard->doDeleteBox(GetId());
    MainFrame::isSaved = false;
}

void NotesBox::paintBox(wxPaintEvent& event) {
    wxPaintDC dc(area);

    dc.SetFont(wxFont(wxFontInfo(11).Bold()));
    dc.DrawText(text, wxPoint(15, 2));

    corkboard->paintBoxes(dc, this);

    event.Skip();
}

void NotesBox::paintArrow(wxPaintEvent& event) {
    wxPaintDC dc(arrow);

    wxPoint points[3];

    points[0] = wxPoint(0, 9);
    points[1] = wxPoint(9, 9);
    points[2] = wxPoint(9, 0);

    dc.SetBrush(wxBrush(wxColour(0, 0, 0)));
    dc.DrawPolygon(3, points);

    dc.SetPen(wxPen(wxColour(180, 180, 180), 1.5));
    dc.DrawLine(wxPoint(2, 9), wxPoint(9, 2));
    dc.DrawLine(wxPoint(5, 9), wxPoint(9, 5));
}

void NotesBox::save(std::ofstream& out) {
    if (out.is_open()) {
        int size;

        size = text.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(text.c_str(), size);

        out.write((char*)&id, sizeof(int));
        out.write((char*)&xRatio, sizeof(int));
        out.write((char*)&yRatio, sizeof(int));

        int sx = this->size.GetX();
        int sy = this->size.GetY();

        out.write((char*)&sx, sizeof(int));
        out.write((char*)&sy, sizeof(int));
        out.write((char*)&this->pos.x, sizeof(int));
        out.write((char*)&this->pos.y, sizeof(int));
    }
}

void NotesBox::load(std::ifstream& in) {
    if (in.is_open()) {
        int size;
        char* data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        text = data;
        delete data;

        in.read((char*)&id, sizeof(int));
        in.read((char*)&xRatio, sizeof(int));
        in.read((char*)&yRatio, sizeof(int));

        int sx, sy, px, py;

        in.read((char*)&sx, sizeof(int));
        in.read((char*)&sy, sizeof(int));
        in.read((char*)&px, sizeof(int));
        in.read((char*)&py, sizeof(int));

        this->size = wxSize(sx, sy);
        this->pos = wxPoint(px, py);
    }
}

void NotesBox::operator=(NotesBox& other) {
    this->parent = other.parent;
    this->id = other.id;
    this->x = other.x;
    this->y = other.y;
    this->size = other.size;
    this->pos = other.pos;
    this->text = other.text;
    this->SetPosition(other.GetPosition());
    this->SetSize(other.GetSize());
}
