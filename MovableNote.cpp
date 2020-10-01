#include "MovableNote.h"

#include "CorkboardImage.h"
#include "MainFrame.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(MovableNote, wxPanel)

EVT_LEFT_UP(MovableNote::onMouseUp)
EVT_MOTION(MovableNote::onMove)

EVT_MOUSE_CAPTURE_LOST(MovableNote::onLoseMouseCapture)

EVT_MENU(MENU_NoteDefault, MovableNote::changeColour)
EVT_MENU(MENU_NoteBlack, MovableNote::changeColour)
EVT_MENU(MENU_NoteWhite, MovableNote::changeColour)
EVT_MENU(MENU_NoteRed, MovableNote::changeColour)
EVT_MENU(MENU_NoteGreen, MovableNote::changeColour)
EVT_MENU(MENU_NoteBlue, MovableNote::changeColour)
EVT_MENU(MENU_NoteYellow, MovableNote::changeColour)
EVT_MENU(MENU_NotePink, MovableNote::changeColour)

EVT_MENU(MENU_NoteDelete, MovableNote::deleteThis)

EVT_CHECKBOX(CHECK_In, MovableNote::cancelIn)
EVT_CHECKBOX(CHECK_Out, MovableNote::cancelOut)

EVT_MENU(MENU_ClearConnections, MovableNote::clearConnections)
EVT_MENU(MENU_ClearBackConnections, MovableNote::clearBackConnections)

END_EVENT_TABLE()

MovableNote::MovableNote(wxWindow* parent, const wxPoint& pos, const wxSize& size, int id) :
    wxPanel(parent, id, pos, size, wxBORDER_RAISED | wxID_RESIZE_FRAME) {
    this->parent = dynamic_cast<ImagePanel*>(parent);
    this->corkboard = dynamic_cast<Corkboard*>(parent->GetParent()->GetParent());
    this->pos = pos;
    this->size = size;

    handle = new wxPanel(this, -1, wxDefaultPosition, wxSize(-1, 17));
    handle->SetBackgroundColour(wxColour(80, 80, 80));
    handle->Bind(wxEVT_LEFT_DOWN, &MovableNote::onMouseDown, this);
    handle->Bind(wxEVT_RIGHT_UP, &MovableNote::onRightUp, this);

    content = new wxRichTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxTE_MULTILINE);

    arrow = new wxPanel(this, -1, wxDefaultPosition, wxSize(7, 7));
    arrow->SetCursor(wxCURSOR_SIZENWSE);
    arrow->Bind(wxEVT_PAINT, &MovableNote::paintArrow, this);
    arrow->Bind(wxEVT_LEFT_DOWN, &MovableNote::arrowDown, this);
    arrow->SetBackgroundColour((wxColour(80, 80, 80)));

    inBut = new wxCheckBox(this, CHECK_In, "");
    outBut = new wxCheckBox(this, CHECK_Out, "");

    inBut->Bind(wxEVT_RIGHT_UP, &MovableNote::rightIn, this);
    outBut->Bind(wxEVT_RIGHT_UP, &MovableNote::rightOut, this);
    outBut->Bind(wxEVT_LEFT_DOWN, &MovableNote::outDown, this);

    wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(inBut, wxSizerFlags(0).CenterVertical());
    buttonsSizer->Add(handle, wxSizerFlags(1));
    buttonsSizer->Add(outBut, wxSizerFlags(0).CenterVertical());

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(buttonsSizer, wxSizerFlags(0).Expand());
    siz->Add(content, wxSizerFlags(1).Expand());
    siz->Add(arrow, wxSizerFlags(0).Right());

    SetSizer(siz);

    curColour = MENU_NoteDefault;
    arrow->Refresh();
    arrow->Update();
    SetBackgroundColour(wxColour(80, 80, 80));
    Refresh();
    Update();
}

void MovableNote::initNote() {
    for (unsigned int i = 0; i < conAIds.GetCount(); i++) {
        for (auto it = corkboard->notes.begin(); it != corkboard->notes.end(); it++) {
            if ((*it)->GetId() == conAIds[i])
                conA.push_back((*it));
        }
    }

    for (unsigned int i = 0; i < conBIds.GetCount(); i++) {
        for (auto it = corkboard->images.begin(); it != corkboard->images.end(); it++) {
            if ((*it)->GetId() == conBIds[i])
                conB.push_back((*it));
        }
    }

    for (unsigned int i = 0; i < backConAIds.GetCount(); i++) {
        for (auto it = corkboard->notes.begin(); it != corkboard->notes.end(); it++) {
            if ((*it)->GetId() == backConAIds[i])
                backConA.push_back((*it));
        }
    }

    for (unsigned int i = 0; i < backConBIds.GetCount(); i++) {
        for (auto it = corkboard->images.begin(); it != corkboard->images.end(); it++) {
            if ((*it)->GetId() == backConBIds[i])
                backConB.push_back((*it));
        }
    }

    SetSize(size);
    SetPosition(pos);

    content->SetValue(text);

    wxCommandEvent ev;
    ev.SetId(curColour);
    changeColour(ev);

    if (conA.size() > 0 || conB.size() > 0)
        outBut->SetValue(true);

    if (backConA.size() > 0 || backConB.size() > 0)
        inBut->SetValue(true);
}

void MovableNote::onMouseDown(wxMouseEvent& event) {
    CaptureMouse();
    x = event.GetX();
    y = event.GetY();
    dragging = true;
}

void MovableNote::onMouseUp(wxMouseEvent& event) {
    if (dragging) {
        dragging = false;
        pos = GetPosition();
    }

    if (resizing) {
        resizing = false;
        size = GetSize();
    }

    corkboard->isConnectingA = false;

    if (HasCapture())
        ReleaseMouse();

    if (corkboard->HasCapture())
        corkboard->ReleaseMouse();

    SetFocus();
    parent->Refresh();
    parent->Update();
    arrow->Refresh();
    arrow->Update();

    event.Skip();
}

void MovableNote::onMove(wxMouseEvent& event) {
    wxPoint mouseOnScreen = wxGetMousePosition();

    if (dragging) {
        int newx = mouseOnScreen.x - x;
        int newy = mouseOnScreen.y - y;

        wxPoint p = parent->ScreenToClient(wxPoint(newx, newy));
        this->Move(p);
        pos = p;

        Refresh();
        Update();

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

    parent->Refresh();
    parent->Update();

    event.Skip();
}

//void MovableNote::onLoseMouseCapture(wxMouseCaptureLostEvent& event) {}

void MovableNote::onRightUp(wxMouseEvent& event) {
    wxMenu menu;

    menu.AppendRadioItem(MENU_NoteDefault, "Default");
    menu.AppendRadioItem(MENU_NoteBlack, "Black");
    menu.AppendRadioItem(MENU_NoteWhite, "White");
    menu.AppendRadioItem(MENU_NoteRed, "Red");
    menu.AppendRadioItem(MENU_NoteGreen, "Green");
    menu.AppendRadioItem(MENU_NoteBlue, "Blue");
    menu.AppendRadioItem(MENU_NoteYellow, "Yellow");
    menu.AppendRadioItem(MENU_NotePink, "Pink");
    menu.AppendSeparator();
    menu.Append(MENU_NoteDelete, "Delete");

    menu.Check(curColour, true);

    PopupMenu(&menu);
}

void MovableNote::changeColour(wxCommandEvent& event) {
    wxWindowID id = event.GetId();

    switch (id) {
    case MENU_NoteDefault:
        handle->SetBackgroundColour(wxColour(80, 80, 80));
        content->SetBackgroundColour(wxColour(255, 255, 255));

        curColour = MENU_NoteDefault;
        break;

    case MENU_NoteBlack:
        handle->SetBackgroundColour(wxColour(20, 20, 20));
        content->SetBackgroundColour(wxColour(80, 80, 80));

        curColour = MENU_NoteBlack;
        break;

    case MENU_NoteWhite:
        handle->SetBackgroundColour(wxColour(230, 230, 230));
        content->SetBackgroundColour(wxColour(255, 255, 255));

        curColour = MENU_NoteWhite;
        break;

    case MENU_NoteRed:
        handle->SetBackgroundColour(wxColour(200, 0, 0));
        content->SetBackgroundColour(wxColour(200, 100, 100));

        curColour = MENU_NoteRed;
        break;

    case MENU_NoteGreen:
        handle->SetBackgroundColour(wxColour(0, 180, 0));
        content->SetBackgroundColour(wxColour(130, 200, 130));

        curColour = MENU_NoteGreen;
        break;

    case MENU_NoteBlue:
        handle->SetBackgroundColour(wxColour(0, 0, 180));
        content->SetBackgroundColour(wxColour(130, 130, 200));

        curColour = MENU_NoteBlue;
        break;

    case MENU_NoteYellow:
        handle->SetBackgroundColour(wxColour(245, 245, 0));
        content->SetBackgroundColour(wxColour(255, 255, 130));

        curColour = MENU_NoteYellow;
        break;

    case MENU_NotePink:
        handle->SetBackgroundColour(wxColour(255, 20, 147));
        content->SetBackgroundColour(wxColour(255, 182, 193));

        curColour = MENU_NotePink;
        break;
    }

    SetBackgroundColour(handle->GetBackgroundColour());
    arrow->SetBackgroundColour(handle->GetBackgroundColour());

    Refresh();
    Update();

    MainFrame::isSaved = false;
}

void MovableNote::deleteThis(wxCommandEvent& event) {
    corkboard->doDeleteNote(GetId());
    MainFrame::isSaved = false;
}

void MovableNote::paintArrow(wxPaintEvent& event) {
    wxPaintDC dc(arrow);

    wxPoint points[3];

    points[0] = wxPoint(0, 7);
    points[1] = wxPoint(7, 7);
    points[2] = wxPoint(7, 0);

    dc.SetBrush(wxBrush(wxColour(0, 0, 0)));
    dc.DrawPolygon(3, points);

    dc.SetPen(wxPen(wxColour(180, 180, 180), 1.5));
    dc.DrawLine(wxPoint(3, 7), wxPoint(7, 3));

    event.Skip(true);
}

void MovableNote::arrowDown(wxMouseEvent& event) {
    CaptureMouse();
    x = event.GetX();
    y = event.GetY();
    resizing = true;
}

void MovableNote::rightIn(wxMouseEvent& event) {
    wxMenu menu;

    menu.Append(MENU_ClearBackConnections, "Clear connections");

    PopupMenu(&menu);
}

void MovableNote::rightOut(wxMouseEvent& event) {
    wxMenu menu;

    menu.Append(MENU_ClearConnections, "Clear connections");

    PopupMenu(&menu);
}

void MovableNote::outDown(wxMouseEvent& event) {
    corkboard->CaptureMouse();
    corkboard->isConnectingA = true;
    corkboard->connectingA = this;
    x = wxGetMousePosition().x;
    y = wxGetMousePosition().y;
}

void MovableNote::cancelIn(wxCommandEvent& event) {
    if (event.IsChecked()) {
        inBut->SetValue(false);
    } else {
        inBut->SetValue(true);
    }
}

void MovableNote::cancelOut(wxCommandEvent& event) {
    if (event.IsChecked())
        outBut->SetValue(false);
    else
        outBut->SetValue(true);
}

void MovableNote::clearConnections(wxCommandEvent& event) {
    for (auto a = conA.begin(); a != conA.end(); a++) {
        for (auto otherCon = (*a)->backConA.begin(); otherCon != (*a)->backConA.end(); otherCon++) {
            if ((*otherCon)->GetId() == GetId()) {
                (*a)->backConA.erase(otherCon);
                (*a)->backConAIds.Remove(GetId());

                if ((*a)->backConA.empty() && (*a)->backConB.empty())
                    (*a)->inBut->SetValue(false);

                conAIds.Remove((*a)->GetId());
                break;
            }
        }
    }

    for (auto b = conB.begin(); b != conB.end(); b++) {
        for (auto otherCon = (*b)->backConA.begin(); otherCon != (*b)->backConA.end(); otherCon++) {
            if ((*otherCon)->GetId() == GetId()) {
                (*b)->backConA.erase(otherCon);
                (*b)->backConAIds.Remove(GetId());

                if ((*b)->backConA.empty() && (*b)->backConB.empty())
                    (*b)->inBut->SetValue(false);

                break;
            }
        }
    }

    conA.clear();
    conB.clear();
    outBut->SetValue(false);

    parent->Refresh();
    parent->UpdateWindowUI();

    MainFrame::isSaved = false;
}

void MovableNote::clearBackConnections(wxCommandEvent& event) {
    for (auto a = backConA.begin(); a != backConA.end(); a++) {
        for (auto otherCon = (*a)->conA.begin(); otherCon != (*a)->conA.end(); otherCon++) {
            if ((*otherCon)->GetId() == GetId()) {
                (*a)->conA.erase(otherCon);
                (*a)->conAIds.Remove(GetId());

                if ((*a)->conA.empty() && (*a)->conB.empty())
                    (*a)->outBut->SetValue(false);

                backConAIds.Remove((*a)->GetId());
                break;
            }
        }
    }

    for (auto b = backConB.begin(); b != backConB.end(); b++) {
        for (auto otherCon = (*b)->conA.begin(); otherCon != (*b)->conA.end(); otherCon++) {
            if ((*otherCon)->GetId() == GetId()) {
                (*b)->conA.erase(otherCon);
                (*b)->conAIds.Remove(GetId());

                if ((*b)->conA.empty() && (*b)->conB.empty()) {
                    (*b)->outBut->SetValue(false);
                }

                break;
            }
        }
    }
    
    backConA.clear();
    inBut->SetValue(false);

    parent->Refresh();
    parent->UpdateWindowUI();

    MainFrame::isSaved = false;
}

void MovableNote::connectNote(MovableNote* connection) {
    conA.push_back(connection);
    conAIds.Add(connection->GetId());
    outBut->SetValue(true);

    MainFrame::isSaved = false;
}

void MovableNote::connectImage(CorkboardImage* connection) {
    conB.push_back(connection);
    conBIds.Add(connection->GetId());
    outBut->SetValue(true);

    MainFrame::isSaved = false;
}

void MovableNote::connectBackNote(MovableNote* connection) {
    backConA.push_back(connection);
    backConAIds.Add(connection->GetId());
    inBut->SetValue(true);

    MainFrame::isSaved = false;
}

void MovableNote::connectBackImage(CorkboardImage* connection) {
    backConB.push_back(connection);
    backConBIds.Add(connection->GetId());
    inBut->SetValue(true);

    MainFrame::isSaved = false;
}

void MovableNote::save(std::ofstream& out) {
    if (out.is_open()) {
        int size;

        text = content->GetValue();
        size = text.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(text.c_str(), size);

        int writeMe;

        writeMe = conAIds.GetCount();
        out.write((char*)&writeMe, sizeof(int));

        for (int i = 0; i < writeMe; i++) {
            out.write((char*)&conAIds[i], sizeof(int));
        }

        writeMe = conBIds.GetCount();
        out.write((char*)&writeMe, sizeof(int));

        for (int i = 0; i < writeMe; i++) {
            out.write((char*)&conBIds[i], sizeof(int));
        }

        writeMe = backConAIds.GetCount();
        out.write((char*)&writeMe, sizeof(int));

        for (int i = 0; i < writeMe; i++) {
            out.write((char*)&backConAIds[i], sizeof(int));
        }

        writeMe = backConBIds.GetCount();
        out.write((char*)&writeMe, sizeof(int));

        for (int i = 0; i < writeMe; i++) {
            out.write((char*)&backConBIds[i], sizeof(int));
        }

        out.write((char*)&curColour, sizeof(int));

        int sx = this->size.GetX();
        int sy = this->size.GetY();

        out.write((char*)&sx, sizeof(int));
        out.write((char*)&sy, sizeof(int));
        out.write((char*)&this->pos.x, sizeof(int));
        out.write((char*)&this->pos.y, sizeof(int));
    }
}

void MovableNote::load(std::ifstream& in) {
    if (in.is_open()) {
        int size;
        char* data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        text = data;
        delete data;

        int number;
        std::string tempName;

        in.read((char*)&number, sizeof(int));
        for (int i = 0; i < number; i++) {
            in.read((char*)&size, sizeof(int));
            conAIds.Add(size);
        }

        in.read((char*)&number, sizeof(int));
        for (int i = 0; i < number; i++) {
            in.read((char*)&size, sizeof(int));
            conBIds.Add(size);
        }

        in.read((char*)&number, sizeof(int));
        for (int i = 0; i < number; i++) {
            in.read((char*)&size, sizeof(int));
            backConAIds.Add(size);
        }

        in.read((char*)&number, sizeof(int));
        for (int i = 0; i < number; i++) {
            in.read((char*)&size, sizeof(int));
            backConBIds.Add(size);
        }

        in.read((char*)&curColour, sizeof(int));

        int sx, sy, px, py;

        in.read((char*)&sx, sizeof(int));
        in.read((char*)&sy, sizeof(int));
        in.read((char*)&px, sizeof(int));
        in.read((char*)&py, sizeof(int));

        this->size = wxSize(sx, sy);
        this->pos = wxPoint(px, py);
    }
}
