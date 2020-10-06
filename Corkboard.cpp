#include "Corkboard.h"

#include "MainFrame.h"
#include "ImagePanel.h"
#include "MovableNote.h"
#include "NotesBox.h"
#include "CorkboardImage.h"
#include "Outline.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(Corkboard, wxPanel)

EVT_TOOL(TOOL_NewNote, Corkboard::addNote)
EVT_TOOL(TOOL_NewImage, Corkboard::addImage)
EVT_TOOL(TOOL_ResetCenter, Corkboard::resetCenter)

EVT_LEFT_UP(Corkboard::onMouseUp)
EVT_RIGHT_UP(Corkboard::onMouseUp)
EVT_MOTION(Corkboard::onMove)
EVT_MOUSE_CAPTURE_LOST(Corkboard::onMouseCaptureLost)

END_EVENT_TABLE()

Corkboard::Corkboard(wxWindow* parent): wxPanel(parent) {
    this->parent = dynamic_cast<Outline*>(parent);

    toolBar = new wxToolBar(this, -1);
    toolBar->AddTool(TOOL_NewNote, "", wxBITMAP_PNG(newNote), "New note");
    toolBar->AddTool(TOOL_NewImage, "", wxBITMAP_PNG(newImage), "New image");
    toolBar->AddSeparator();
    toolBar->AddTool(TOOL_ResetCenter, "", wxBITMAP_PNG(resetCenter), "Reset to center");
    toolBar->Realize();
    toolBar->SetBackgroundColour(wxColour(140, 140, 140));

    SetBackgroundColour(wxColour(0, 0, 0));

    pan = new wxPanel(this);

    main = new ImagePanel(pan, wxDefaultPosition, wxSize(200000, 200000));
    //main->SetDoubleBuffered(true);
    main->Bind(wxEVT_RIGHT_DOWN, &Corkboard::onRightDown, this);
    main->Bind(wxEVT_LEFT_DOWN, &Corkboard::onLeftDown, this);
    main->Bind(wxEVT_LEFT_DCLICK, &Corkboard::onDoubleClick, this);
    main->Bind(wxEVT_MOUSE_CAPTURE_LOST, &Corkboard::onMouseCaptureLost, this);
    main->Bind(wxEVT_PAINT, &Corkboard::paintMain, this);
    //main->Bind(wxEVT_ERASE_BACKGROUND, &Corkboard::onEraseBackground, this);

    //main->Bind(wxEVT_SCROLLWIN_LINEDOWN, &Corkboard::onScrollDown, this);
    //main->Bind(wxEVT_SCROLLWIN_LINEUP, &Corkboard::onScrollUp, this);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(toolBar, wxSizerFlags(0).Expand().Border(wxALL, 1));
    sizer->Add(pan, wxSizerFlags(1).Expand());
    
    SetSizer(sizer);
}

void Corkboard::initMain(const wxSize& size, wxImage& image) {
    main->CenterOnParent();
    main->size = size;
    main->setImageAsIs(image);
}

void Corkboard::addNote(wxCommandEvent& event) {
    wxPoint pos = wxGetMousePosition();
    pos.x += 200;
    pos.y += 200;

    MovableNote* note = new MovableNote(main, main->ScreenToClient(pos),wxSize(230, 150), curNote++);
    note->Layout();

    notes.push_back(note);

    vector<NotesBox*> subNotes;

    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        NotesBox* subNote = new NotesBox(**it);
        (*it)->Destroy();
        subNotes.push_back(subNote);
    }

    boxes.clear();

    for (auto it : subNotes) {
        boxes.push_back(it);
    }

    main->Refresh();
    main->Update();

    note->size = note->GetSize();
    parent->setNon(curNote);
}

void Corkboard::addImage(wxCommandEvent& event) {
    wxPoint pos = wxGetMousePosition();

    pos.x += 200;
    pos.y += 200;

    wxFileDialog* choose = new wxFileDialog(this, "Select an image", wxEmptyString, wxEmptyString,
        "JPG, JPEG(*.jpg;*.jpeg)|*.jpg;*.jpeg",
        wxFD_OPEN, wxDefaultPosition);

    if (choose->ShowModal() == wxID_OK) {
        std::string path = choose->GetPath();

        wxImage image(path);

        if (image.IsOk()) {
            wxSize imageSize(image.GetWidth(), image.GetHeight());
            int orientation;
            float ratio;

            if (imageSize.x > imageSize.y) {
                orientation = 0;
                ratio = (float)imageSize.x / (float)imageSize.y;
            } else {
                orientation = 1;
                ratio = (float)imageSize.y / imageSize.x;
            }
            CorkboardImage* frame = new CorkboardImage(main,
                image,
                main->ScreenToClient(pos),
                wxDefaultSize,
                orientation,
                ratio,
                curImage++);

            images.push_back(frame);

            vector<NotesBox*> subNotes;

            for (auto it = boxes.begin(); it != boxes.end(); it++) {
                NotesBox* subNote = new NotesBox(**it);
                (*it)->Destroy();
                subNotes.push_back(subNote);
            }

            boxes.clear();

            for (auto it : subNotes) {
                boxes.push_back(it);
            }
        }
    }

    if (choose)
        choose->Destroy();

    main->Refresh();
    main->Update();

    parent->setNoi(curImage);
}

void Corkboard::doDeleteNote(wxWindowID id) {
    for (auto it = notes.begin(); it != notes.end(); it++) {
        if ((*it)->GetId() == id) {
            (*it)->clearBackConnections(wxCommandEvent());
            (*it)->clearConnections(wxCommandEvent());
            (*it)->Destroy();
            notes.erase(it);
            break;
        }
    }

    curNote = 0;

    if (notes.size() > 0) {
        for (auto it = notes.begin(); it != notes.end(); it++) {
            (*it)->SetId(curNote++);
            (*it)->conAIds.Clear();
            (*it)->conBIds.Clear();
            (*it)->backConAIds.Clear();
            (*it)->backConBIds.Clear();
        }

        for (auto it = notes.begin(); it != notes.end(); it++) {
            for (unsigned int i = 0; i < (*it)->conA.size(); i++)
                (*it)->conAIds.Add((*it)->conA[i]->GetId());

            for (unsigned int i = 0; i < (*it)->conB.size(); i++)
                (*it)->conBIds.Add((*it)->conB[i]->GetId());
            
            for (unsigned int i = 0; i < (*it)->backConA.size(); i++)
                (*it)->backConAIds.Add((*it)->backConA[i]->GetId());

            for (unsigned int i = 0; i < (*it)->backConB.size(); i++)
                (*it)->backConBIds.Add((*it)->backConB[i]->GetId());
        }
    }

    main->Refresh();
    main->Update();

    parent->setNon(curNote);
}

void Corkboard::doDeleteBox(wxWindowID id) {
    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        if ((*it)->GetId() == id) {
            (*it)->Destroy();
            boxes.erase(it);
            break;
        }
    }

    curBox = 0;

    if (boxes.size() > 0) {
        for (auto it = boxes.begin(); it != boxes.end(); it++) {
            (*it)->id = curBox;
            (*it)->SetId(curBox++);
        }
    }

    parent->setNob(curBox);
}

void Corkboard::doDeleteImage(wxWindowID id) {
    for (auto it = images.begin(); it != images.end(); it++) {
        if ((*it)->GetId() == id) {
            (*it)->clearBackConnections(wxCommandEvent());
            (*it)->clearConnections(wxCommandEvent());
            (*it)->Destroy();
            images.erase(it);
            break;
        }
    }

    curImage = 0;

    if (images.size() > 0) {
        for (auto it = images.begin(); it != images.end(); it++) {
            (*it)->SetId(curImage++);
            (*it)->conAIds.Clear();
            (*it)->conBIds.Clear();
            (*it)->backConAIds.Clear();
            (*it)->backConBIds.Clear();
        }

        for (auto it = images.begin(); it != images.end(); it++) {
            for (unsigned int i = 0; i < (*it)->conA.size(); i++)
                (*it)->conAIds.Add((*it)->conA[i]->GetId());

            for (unsigned int i = 0; i < (*it)->conB.size(); i++)
                (*it)->conBIds.Add((*it)->conB[i]->GetId());

            for (unsigned int i = 0; i < (*it)->backConA.size(); i++)
                (*it)->backConAIds.Add((*it)->backConA[i]->GetId());

            for (unsigned int i = 0; i < (*it)->backConB.size(); i++)
                (*it)->backConBIds.Add((*it)->backConB[i]->GetId());
        }
    }

    parent->setNoi(curImage);
}

void Corkboard::resetCenter(wxCommandEvent& event) {
    main->Move(wxPoint(-main->GetSize().GetX() / 2, -main->GetSize().GetY() / 2));
}

void Corkboard::onLeftDown(wxMouseEvent& event) {
    CaptureMouse();
    startDragging = true;
}

void Corkboard::onRightDown(wxMouseEvent& event) {
    CaptureMouse();
    x = event.GetX();
    y = event.GetY();
    wxSetCursor(wxCURSOR_CLOSED_HAND);
    draggingRight = true;
}

void Corkboard::onDoubleClick(wxMouseEvent& event) {
    for (unsigned int i = 0; i < boxes.size(); i++) {
        wxRect rect = boxes[i]->GetRect();

        if (rect.Contains(event.GetPosition())) {
            boxes[i]->editLabel(wxCommandEvent());
            break;
        }
    }
}

void Corkboard::onMouseUp(wxMouseEvent& event) {
    if (HasCapture())
        ReleaseMouse();

    if (draggingRight) {
        draggingRight = false;


        for (unsigned int i = 0; i < boxes.size(); i++) {
            wxRect rect = boxes[i]->GetRect();

            if (rect.Contains(wxPoint(x, y))) {
                //boxes[i]->onRightUp(event);
                break;
            }
        }
    }

    if (draggingLeft) {
        if (boxes[curBox]->GetSize().x > 80 && boxes[curBox]->GetSize().y > 80) {
            boxes[curBox]->size = boxes[curBox]->GetSize();
            curBox++;
            parent->setNob(curBox);
            MainFrame::isSaved = false;
        } else
            doDeleteBox(boxes[curBox]->GetId());

        draggingLeft = false;
        startDragging = false;
    }

    if (startDragging) {
        startDragging = false;
    }

    if (isConnectingA) {
        if (connectedA) {
            bool already = false;

            for (auto it = connectingA->conA.begin(); it != connectingA->conA.end(); it++) {
                if ((*it) == connectedA)
                    already = true;
            }

            if (!already) {
                connectingA->connectNote(connectedA);
                connectedA->connectBackNote(connectingA);
            }
        }

        if (connectedB) {
            bool already = false;

            for (auto it = connectingA->conB.begin(); it != connectingA->conB.end(); it++) {
                if ((*it) == connectedB)
                    already = true;
            }

            if (!already) {
                connectingA->connectImage(connectedB);
                connectedB->connectBackNote(connectingA);
            }
        }

        isConnectingA = false;
        isConnectingB = false;
        connectingA = nullptr;
        connectingB = nullptr;
        connectedA = nullptr;
        connectedB = nullptr;
    }

    if (isConnectingB) {
        if (connectedA) {
            bool already = false;

            for (auto it = connectingB->conA.begin(); it != connectingB->conA.end(); it++) {
                if ((*it) == connectedA)
                    already = true;
            }

            if (!already) {
                connectingB->connectNote(connectedA);
                connectedA->connectBackImage(connectingB);
            }
        }

        if (connectedB) {
            bool already = false;

            for (auto it = connectingB->conB.begin(); it != connectingB->conB.end(); it++) {
                if ((*it) == connectedB)
                    already = true;
            }

            if (!already) {
                connectingB->connectImage(connectedB);
                connectedB->connectBackImage(connectingB);
            }
        }

        isConnectingA = false;
        isConnectingB = false;
        connectingA = nullptr;
        connectingB = nullptr;
        connectedA = nullptr;
        connectedB = nullptr;
    }

    main->Refresh();
    main->Update();
    wxSetCursor(wxCURSOR_DEFAULT);
    main->SetFocus();
}

void Corkboard::onMove(wxMouseEvent& event) {
    wxPoint mouseOnScreen = wxGetMousePosition();

    if (draggingRight) {
        int newx = mouseOnScreen.x - x;
        int newy = mouseOnScreen.y - y;
        main->Move(pan->ScreenToClient(wxPoint(newx, newy)));
        main->Refresh();
        main->Update();
    }

    /*
    if (startDragging) {
        NotesBox* box = new NotesBox(main, main->ScreenToClient(mouseOnScreen), wxDefaultSize, curBox);

        boxes.push_back(box);
        draggingLeft = true;
        startDragging = false;
    }

    if (draggingLeft) {
        wxPoint pos = main->ClientToScreen(boxes[curBox]->GetPosition());
        boxes[curBox]->SetSize(mouseOnScreen.x - pos.x, mouseOnScreen.y - pos.y);

        main->Refresh();
        main->Update();
    }
    */

    if (isConnectingA) {
        wxRect rect;
        for (auto it = notes.begin(); it != notes.end(); it++) {
            rect = (*it)->inBut->GetScreenRect();

            if (rect.Contains(mouseOnScreen)) {
                if (connectingA != *it) {
                    (*it)->inBut->SetValue(true);
                    connectedA = *it;
                    break;
                }
            } else {
                if ((*it)->backConA.empty() && (*it)->backConB.empty())
                    (*it)->inBut->SetValue(false);

                connectedA = nullptr;
            }
        }

        for (auto it = images.begin(); it != images.end(); it++) {
            rect = (*it)->inBut->GetScreenRect();

            if (rect.Contains(mouseOnScreen)) {
                    (*it)->inBut->SetValue(true);
                    connectedB = *it;
                    break;
            } else {
                if ((*it)->backConA.empty() && (*it)->backConB.empty())
                    (*it)->inBut->SetValue(false);

                connectedB = nullptr;
            }
        }

        main->Refresh();
        main->Update();
    }

    if (isConnectingB) {
        wxRect rect;
        for (auto it = notes.begin(); it != notes.end(); it++) {
            rect = (*it)->inBut->GetScreenRect();

            if (rect.Contains(mouseOnScreen)) {
                    (*it)->inBut->SetValue(true);
                    connectedA = *it;
                    break;
            } else {
                if ((*it)->backConA.empty() && (*it)->backConB.empty())
                    (*it)->inBut->SetValue(false);

                connectedA = nullptr;
            }
        }

        for (auto it = images.begin(); it != images.end(); it++) {
            rect = (*it)->inBut->GetScreenRect();

            if (rect.Contains(mouseOnScreen)) {
                if (connectingB != (*it)) {
                    (*it)->inBut->SetValue(true);
                    connectedB = *it;
                    break;
                }
            } else {
                if ((*it)->backConA.empty() && (*it)->backConB.empty())
                    (*it)->inBut->SetValue(false);

                connectedB = nullptr;
            }
        }

        main->Refresh();
        main->Update();
    }
}

void Corkboard::onScrollDown(wxScrollWinEvent& event) {
    if (zoom > 40) {
        zoom--;
        doZoom();
    }
}

void Corkboard::onScrollUp(wxScrollWinEvent& event) {
    if (zoom < 100) {
        zoom++;
        doZoom();
    }
}

void Corkboard::doZoom() {
    /*wxSize mainSize = main->GetSize();
    wxPoint mainPos = main->GetPosition();

    if (mainPos.x == 0 || mainPos.y == 0)
        return;

    float mainRatioX = (mainSize.x / (mainPos.x));
    float mainRatioY = (mainSize.y / (mainPos.y));

    main->SetSize(5000 / 100 * zoom, 5000 / 100 * zoom);
    mainSize = main->GetSize();
    main->SetPosition(wxPoint((mainSize.GetX() / mainRatioX), (mainSize.GetY() / mainRatioY)));
    */
    for (auto it = boxes.begin(); it != boxes.end(); it++) {
        wxSize s = (*it)->size;
        (*it)->SetSize(s.x / 100 * zoom, s.y / 100 * zoom);

        //(*it)->SetPosition(wxPoint(mainSize.x / (*it)->xRatio, mainSize.y / (*it)->yRatio));
    }

    for (auto it = notes.begin(); it != notes.end(); it++) {
        wxSize s = (*it)->size;
        //wxPoint p = (*it)->pos;
        (*it)->SetSize((s.x / 100 * zoom) + 35, (s.y / 100 * zoom) + 50);
    }

    Refresh();
    Update();
}

void Corkboard::onMouseCaptureLost(wxMouseCaptureLostEvent& event) {
    draggingLeft = false;
    startDragging = false;

    if (&boxes[curBox])
        boxes[curBox]->deleteThis(wxCommandEvent());

    if (isConnectingA) {
        connectingA = nullptr;
        connectingB = nullptr;

        isConnectingA = false;
        Refresh();
        Update();
    }
}

void Corkboard::paintMain(wxPaintEvent& event) {
    wxPaintDC dc(main);
    
    dc.SetPen(wxPen(wxColour(240, 240, 240), 4));
    wxPoint c1;
    wxPoint c2;

    if (isConnectingA) {
        if (connectingA) {
            c1 = connectingA->getOutConPos();
            c2 = main->ScreenToClient(wxGetMousePosition());

            int x = c2.x - c1.x;
            int y = c2.y - c1.y;

            wxPoint points[6];
            points[0] = c1;
            points[1] = wxPoint(c1.x + 14, c1.y);
            points[2] = wxPoint(c1.x + (x / 4), c2.y - (y * 0.9));
            points[3] = wxPoint(c1.x + ((x / 4) * 3), c1.y + (y * 0.9));
            points[4] = wxPoint(c2.x - 14, c2.y);
            points[5] = c2;

            dc.DrawSpline(6, points);
        }
    }

    if (isConnectingB) {
        if (connectingB) {
            c1 = connectingB->getOutConPos();
            c2 = main->ScreenToClient(wxGetMousePosition());

            int x = c2.x - c1.x;
            int y = c2.y - c1.y;

            wxPoint points[6];
            points[0] = c1;
            points[1] = wxPoint(c1.x + 14, c1.y);
            points[2] = wxPoint(c1.x + (x / 4), c2.y - (y * 0.9));
            points[3] = wxPoint(c1.x + ((x / 4) * 3), c1.y + (y * 0.9));
            points[4] = wxPoint(c2.x - 14, c2.y);
            points[5] = c2;

            dc.DrawSpline(6, points);
        }
    }

    for (auto it = notes.begin(); it != notes.end(); it++) {
        c1 = (*it)->getOutConPos();

        for (auto aVec = (*it)->conA.begin(); aVec != (*it)->conA.end(); aVec++) {
            c2 = (*aVec)->getInConPos();

            int x = c2.x - c1.x;
            int y = c2.y - c1.y;

            wxPoint points[6];
            points[0] = c1;
            points[1] = wxPoint(c1.x + 14, c1.y);
            points[2] = wxPoint(c1.x + (x / 4), c2.y - (y * 0.9));
            points[3] = wxPoint(c1.x + ((x / 4) * 3), c1.y + (y * 0.9));
            points[4] = wxPoint(c2.x - 14, c2.y);
            points[5] = c2;

            dc.DrawSpline(6, points);
        }

        for (auto bVec = (*it)->conB.begin(); bVec != (*it)->conB.end(); bVec++) {
            c2 = (*bVec)->getInConPos();

            int x = c2.x - c1.x;
            int y = c2.y - c1.y;

            wxPoint points[6];
            points[0] = c1;
            points[1] = wxPoint(c1.x + 14, c1.y);
            points[2] = wxPoint(c1.x + (x / 4), c2.y - (y * 0.9));
            points[3] = wxPoint(c1.x + ((x / 4) * 3), c1.y + (y * 0.9));
            points[4] = wxPoint(c2.x - 14, c2.y);
            points[5] = c2;

            dc.DrawSpline(6, points);
        }
    }

    for (auto it = images.begin(); it != images.end(); it++) {
        c1 = (*it)->getOutConPos();

        for (auto aVec = (*it)->conA.begin(); aVec != (*it)->conA.end(); aVec++) {
            c2 = (*aVec)->getInConPos();

            int x = c2.x - c1.x;
            int y = c2.y - c1.y;

            wxPoint points[6];
            points[0] = c1;
            points[1] = wxPoint(c1.x + 14, c1.y);
            points[2] = wxPoint(c1.x + (x / 4), c2.y - (y * 0.9));
            points[3] = wxPoint(c1.x + ((x / 4) * 3), c1.y + (y * 0.9));
            points[4] = wxPoint(c2.x - 14, c2.y);
            points[5] = c2;

            dc.DrawSpline(6, points);
        }

        for (auto bVec = (*it)->conB.begin(); bVec != (*it)->conB.end(); bVec++) {
            c2 = (*bVec)->getInConPos();

            int x = c2.x - c1.x;
            int y = c2.y - c1.y;

            wxPoint points[6];
            points[0] = c1;
            points[1] = wxPoint(c1.x + 14, c1.y);
            points[2] = wxPoint(c1.x + (x / 4), c2.y - (y * 0.9));
            points[3] = wxPoint(c1.x + ((x / 4) * 3), c1.y + (y * 0.9));
            points[4] = wxPoint(c2.x - 14, c2.y);
            points[5] = c2;

            dc.DrawSpline(6, points);
        }
    }

    /*
    for (auto it : boxes) {
        it->Refresh();
        it->Update();
    }
    */
    
    event.Skip();
}

void Corkboard::paintBoxes(wxPaintDC& dc, NotesBox* note) {
    dc.SetPen(wxPen(wxColour(240, 240, 240), 4));
    wxPoint c1;
    wxPoint c2;

    if (isConnectingA) {
        if (connectingA) {
            c1 = note->ScreenToClient(main->ClientToScreen(connectingA->getOutConPos()));
            c2 = note->ScreenToClient(wxGetMousePosition());

            int x = c2.x - c1.x;
            int y = c2.y - c1.y;

            wxPoint points[6];
            points[0] = c1;
            points[1] = wxPoint(c1.x + 14, c1.y);
            points[2] = wxPoint(c1.x + (x / 4), c2.y - (y * 0.9));
            points[3] = wxPoint(c1.x + ((x / 4) * 3), c1.y + (y * 0.9));
            points[4] = wxPoint(c2.x - 14, c2.y);
            points[5] = c2;

            dc.DrawSpline(6, points);
        }

        if (connectingB) {

        }
    }

    for (auto it = notes.begin(); it != notes.end(); it++) {
        c1 = note->ScreenToClient(main->ClientToScreen((*it)->getOutConPos()));

        for (auto aVec = (*it)->conA.begin(); aVec != (*it)->conA.end(); aVec++) {
            c2 = note->ScreenToClient(main->ClientToScreen((*aVec)->getInConPos()));

            int x = c2.x - c1.x;
            int y = c2.y - c1.y;

            wxPoint points[6];
            points[0] = c1;
            points[1] = wxPoint(c1.x + 14, c1.y);
            points[2] = wxPoint(c1.x + (x / 4), c2.y - (y * 0.9));
            points[3] = wxPoint(c1.x + ((x / 4) * 3), c1.y + (y * 0.9));
            points[4] = wxPoint(c2.x - 14, c2.y);
            points[5] = c2;

            dc.DrawSpline(6, points);
        }
    }
}
