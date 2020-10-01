#include "CorkboardImage.h"

#include "Corkboard.h"
#include "MovableNote.h"
#include "MainFrame.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(CorkboardImage, wxPanel)

EVT_MOTION(CorkboardImage::onMotion)
EVT_LEFT_UP(CorkboardImage::onLeftUp)

EVT_MENU(MENU_DeleteImage, CorkboardImage::deleteThis)
EVT_MENU(MENU_ClearImConnections, CorkboardImage::clearConnections)
EVT_MENU(MENU_ClearImBackConnections, CorkboardImage::clearBackConnections)

END_EVENT_TABLE()

CorkboardImage::CorkboardImage(wxWindow* parent, wxImage& im,
    const wxPoint& pos, const wxSize& size, int orientation, float ratio, int id) :
    wxPanel(parent, id, pos, size, wxBORDER_RAISED) {

    this->parent = dynamic_cast<ImagePanel*>(parent);
    this->corkboard = dynamic_cast<Corkboard*>(parent->GetParent()->GetParent());

    this->pos = pos;
    this->ratio = ratio;
    this->orientation = orientation;
    this->picture = im;
    wxSize newSize;

    if (orientation == 0) {
        newSize.Set(220, 220 / ratio);
    } else if (orientation == 1) {
        newSize.Set(220 / ratio, 220);
    }

    SetSize(newSize);
    this->size = newSize;

    image = new ImagePanel(this, wxDefaultPosition, wxSize(GetSize().GetX(), GetSize().GetY()));
    image->size = wxSize(newSize.x, newSize.y);
    image->setImage(im);
    image->Bind(wxEVT_LEFT_DOWN, &CorkboardImage::onLeftDown, this);
    image->Bind(wxEVT_RIGHT_UP, &CorkboardImage::onRightUp, this);
    image->SetCursor(wxCURSOR_SIZING);

    arrow = new wxPanel(this, -1, wxDefaultPosition, wxSize(9, 9));
    arrow->Bind(wxEVT_PAINT, &CorkboardImage::paintArrow, this);
    arrow->Bind(wxEVT_LEFT_DOWN, &CorkboardImage::arrowDown, this);
    //arrow->Bind(wxEVT_LEFT_UP, &CorkboardImage::arrowUp, this);
    arrow->SetCursor(wxCURSOR_SIZENWSE);

    siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(image, wxSizerFlags(1).Expand());
    siz->Add(arrow, wxSizerFlags(0).Right());

    SetSizer(siz);

    outBut = new wxCheckBox(image, -1, "");
    inBut = new wxCheckBox(image, -1, "");

    inBut->Bind(wxEVT_RIGHT_UP, &CorkboardImage::rightIn, this);
    inBut->SetCursor(wxCURSOR_DEFAULT);
    outBut->Bind(wxEVT_RIGHT_UP, &CorkboardImage::rightOut, this);
    outBut->Bind(wxEVT_LEFT_DOWN, &CorkboardImage::outDown, this);
    outBut->SetCursor(wxCURSOR_DEFAULT);

    wxBoxSizer* siz2 = new wxBoxSizer(wxHORIZONTAL);
    siz2->Add(inBut, wxSizerFlags(0).Top());
    siz2->AddStretchSpacer(1);
    siz2->Add(outBut, wxSizerFlags(0).Top());

    image->SetSizer(siz2);

    SetBackgroundColour(wxColour(160, 160, 160));

    arrow->Refresh();
    arrow->Update();
    Layout();
    siz->Layout();
    Refresh();
    Update();
}

void CorkboardImage::initImage() {
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

    image->size = size;
    image->setImage(picture);

    if (conA.size() > 0 || conB.size() > 0)
        outBut->SetValue(true);

    if (backConA.size() > 0 || backConB.size() > 0)
        inBut->SetValue(true);
}

void CorkboardImage::onLeftDown(wxMouseEvent& event) {
    CaptureMouse();
    x = event.GetX();
    y = event.GetY();
    dragging = true;
}

void CorkboardImage::onLeftUp(wxMouseEvent& event) {
    if (dragging) {
        dragging = false;
        pos = GetPosition();
    }

    if (resizing) {
        resizing = false;
        size = GetSize();
    }

    corkboard->isConnectingB = false;

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

void CorkboardImage::onRightUp(wxMouseEvent& event) {
    wxMenu menu;
    menu.Append(MENU_DeleteImage, "Delete");

    PopupMenu(&menu);
}

void CorkboardImage::onMotion(wxMouseEvent& event) {
    wxPoint mouseOnScreen = wxGetMousePosition();

    if (dragging) {
        int newx = mouseOnScreen.x - x;
        int newy = mouseOnScreen.y - y;
        this->Move(parent->ScreenToClient(wxPoint(newx, newy)));
        pos = GetPosition();
        parent->Refresh();
        parent->Update();

        MainFrame::isSaved = false;
    }

    if (resizing) {
        wxPoint pos = parent->ClientToScreen(GetPosition());
        wxSize size(mouseOnScreen.x - x, mouseOnScreen.y - y);

        int newx;
        int newy;

        if (size.x > size.y) {
            switch (orientation) {
            case 0:
                newx = size.x;
                newy = newx / ratio;
                break;

            case 1:
                newx = size.x;
                newy = newx * ratio;
                break;
            }
        } else {
            switch (orientation) {
            case 0:
                newy = size.y;
                newx = newy * ratio;
                break;

            case 1:
                newy = size.y;
                newx = newy / ratio;
                break;
            }
        }

        SetSize(newx, newy);
        if (GetSize().x < 70 || GetSize().y < 70) {
            switch (orientation) {
            case 0:
                SetSize(70, 70 / ratio);
                break;

            case 1:
                SetSize(70 / ratio, 70);
                break;
            }

            return;
        }

        this->size = GetSize();

        image->size = wxSize(newx, newy);
        image->setImage(picture);
        parent->Refresh();
        parent->Update();

        MainFrame::isSaved = false;
    }
}

void CorkboardImage::rightIn(wxMouseEvent& event) {
    wxMenu menu;

    menu.Append(MENU_ClearImBackConnections, "Clear connections");

    PopupMenu(&menu);
}

void CorkboardImage::rightOut(wxMouseEvent& event) {
    wxMenu menu;

    menu.Append(MENU_ClearImConnections, "Clear connections");

    PopupMenu(&menu);
}

void CorkboardImage::outDown(wxMouseEvent& event) {
    corkboard->CaptureMouse();
    corkboard->isConnectingB = true;
    corkboard->connectingB = this;
    x = wxGetMousePosition().x;
    y = wxGetMousePosition().y;
}

void CorkboardImage::cancelIn(wxCommandEvent& event) {
    if (event.IsChecked())
        inBut->SetValue(false);
    else
        inBut->SetValue(true);
}

void CorkboardImage::cancelOut(wxCommandEvent& event) {
    if (event.IsChecked())
        outBut->SetValue(false);
    else
        outBut->SetValue(true);
}

void CorkboardImage::clearConnections(wxCommandEvent& event) {
    for (auto a = conA.begin(); a != conA.end(); a++) {
        for (auto otherCon = (*a)->backConB.begin(); otherCon != (*a)->backConB.end(); otherCon++) {
            if ((*otherCon)->GetId() == GetId()) {
                (*a)->backConB.erase(otherCon);
                (*a)->backConBIds.Remove(GetId());

                if ((*a)->backConA.empty() && (*a)->backConB.empty())
                    (*a)->inBut->SetValue(false);

                conAIds.Remove((*a)->GetId());
                break;
            }
        }
    }

    for (auto b = conB.begin(); b != conB.end(); b++) {
        for (auto otherCon = (*b)->backConB.begin(); otherCon != (*b)->backConB.end(); otherCon++) {
            if ((*otherCon)->GetId() == GetId()) {
                (*b)->backConB.erase(otherCon);
                (*b)->backConBIds.Remove(GetId());

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

void CorkboardImage::clearBackConnections(wxCommandEvent& event) {
    for (auto a = backConA.begin(); a != backConA.end(); a++) {
        for (auto otherCon = (*a)->conB.begin(); otherCon != (*a)->conB.end(); otherCon++) {
            if ((*otherCon)->GetId() == GetId()) {
                (*a)->conB.erase(otherCon);
                (*a)->conBIds.Remove(GetId());

                if ((*a)->conA.empty() && (*a)->conB.empty())
                    (*a)->outBut->SetValue(false);

                backConAIds.Remove((*a)->GetId());
                break;
            }
        }
    }

    for (auto b = backConB.begin(); b != backConB.end(); b++) {
        for (auto otherCon = (*b)->conB.begin(); otherCon != (*b)->conB.end(); otherCon++) {
            if ((*otherCon)->GetId() == GetId()) {
                (*b)->conB.erase(otherCon);
                (*b)->conBIds.Remove(GetId());

                if ((*b)->conA.empty() && (*b)->conB.empty())
                    (*b)->outBut->SetValue(false);

                break;
            }
        }
    }

    backConA.clear();
    backConB.clear();
    inBut->SetValue(false);

    parent->Refresh();
    parent->UpdateWindowUI();

    MainFrame::isSaved = false;
}

void CorkboardImage::connectNote(MovableNote* connection) {
    conA.push_back(connection);
    conAIds.Add(connection->GetId());
    outBut->SetValue(true);

    MainFrame::isSaved = false;
}

void CorkboardImage::connectImage(CorkboardImage* connection) {
    conB.push_back(connection);
    conBIds.Add(connection->GetId());
    outBut->SetValue(true);

    MainFrame::isSaved = false;
}

void CorkboardImage::connectBackNote(MovableNote* connection) {
    backConA.push_back(connection);
    backConAIds.Add(connection->GetId());
    inBut->SetValue(true);

    MainFrame::isSaved = false;
}

void CorkboardImage::connectBackImage(CorkboardImage* connection) {
    backConB.push_back(connection);
    backConBIds.Add(connection->GetId());
    inBut->SetValue(true);

    MainFrame::isSaved = false;
}

void CorkboardImage::arrowDown(wxMouseEvent& event) {
    CaptureMouse();
    wxPoint pos = ClientToScreen(wxPoint(event.GetX(), event.GetY()));
    x = pos.x;
    y = pos.y;
    resizing = true;
}

void CorkboardImage::deleteThis(wxCommandEvent& event) {
    corkboard->doDeleteImage(GetId());
    MainFrame::isSaved = false;
}

void CorkboardImage::save(std::ofstream& out) {
    if (out.is_open()) {
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

        out.write((char*)&ratio, sizeof(float));
        out.write((char*)&orientation, sizeof(int));

        int sx = this->size.GetX();
        int sy = this->size.GetY();

        out.write((char*)&sx, sizeof(int));
        out.write((char*)&sy, sizeof(int));
        out.write((char*)&this->pos.x, sizeof(int));
        out.write((char*)&this->pos.y, sizeof(int));

        string path(MainFrame::currentDocFolder + "\\Images\\Outline\\" + std::to_string(GetId()) + ".jpg");
        picture.SaveFile(path);
    }
}

void CorkboardImage::load(std::ifstream& in) {
    if (in.is_open()) {
        int size;
        int number;

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

        in.read((char*)&ratio, sizeof(float));
        in.read((char*)&orientation, sizeof(int));

        int sx, sy, px, py;

        in.read((char*)&sx, sizeof(int));
        in.read((char*)&sy, sizeof(int));
        in.read((char*)&px, sizeof(int));
        in.read((char*)&py, sizeof(int));

        this->size = wxSize(sx, sy);
        this->pos = wxPoint(px, py);

        string path(MainFrame::currentDocFolder + "\\Images\\Outline\\" + std::to_string(GetId()) + ".jpg");
        picture.LoadFile(path);

        if (!picture.IsOk()) {
            picture.Clear();
            picture.Resize(wxSize(200, 200), wxDefaultPosition);
        }
    }
}

void CorkboardImage::paintArrow(wxPaintEvent& event) {
    wxPaintDC dc(arrow);

    wxPoint points[3];

    points[0] = wxPoint(0, 9);
    points[1] = wxPoint(9, 9);
    points[2] = wxPoint(9, 0);

    dc.SetBrush(wxBrush(wxColour(50, 50, 50)));
    dc.SetPen(wxPen(wxColour(150, 150, 150), 1));

    dc.DrawPolygon(3, points);

    event.Skip();
}
