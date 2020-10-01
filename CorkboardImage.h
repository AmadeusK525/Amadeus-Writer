#pragma once

#include "wx/dcbuffer.h"
#include "wx/panel.h"

#include <fstream>

#include "ImagePanel.h"
#include "Corkboard.h"

class CorkboardImage : public wxPanel {
private:
    ImagePanel* parent = nullptr;
    Corkboard* corkboard = nullptr;

    ImagePanel* image = nullptr;
    wxImage picture;
    wxPanel* arrow = nullptr;

    wxBoxSizer* siz = nullptr;

    bool resizing = false;
    bool dragging = false;
    int x;
    int y;

public:
    wxSize size;
    wxPoint pos;

    float ratio;
    int orientation;

    wxCheckBox* outBut = nullptr;
    wxCheckBox* inBut = nullptr;

    wxArrayInt conAIds;
    wxArrayInt conBIds;
    wxArrayInt backConAIds;
    wxArrayInt backConBIds;

    vector<MovableNote*> conA;
    vector<MovableNote*> backConA;
    vector<CorkboardImage*> conB;
    vector<CorkboardImage*> backConB;

public:
    CorkboardImage(wxWindow* parent,
        wxImage& im,
        const wxPoint& pos,
        const wxSize& size,
        int orientation,
        float ratio,
        int id);

    void initImage();

    void onLeftDown(wxMouseEvent& event);
    void onLeftUp(wxMouseEvent& event);
    void onRightUp(wxMouseEvent& event);
    void onMotion(wxMouseEvent& event);
    void onLoseMouseCapture(wxMouseCaptureLostEvent& event) {};

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

    void paintArrow(wxPaintEvent& event);
    void arrowDown(wxMouseEvent& event);

    void deleteThis(wxCommandEvent& event); 

    void save(std::ofstream& out);
    void load(std::ifstream& in);

    DECLARE_EVENT_TABLE()
};

enum {
    MENU_DeleteImage,
    MENU_ClearImConnections,
    MENU_ClearImBackConnections
};
