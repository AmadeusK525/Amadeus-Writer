#ifndef CORKBOARD_H_
#define CORKBOARD_H_

#pragma once

#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/dcclient.h>

#include <wx\wxsf\wxShapeFramework.h>

#include <vector>
#include <memory>

class CorkboardCanvas;

class Corkboard;
class Outline;

using std::vector;

enum ToolMode {
    modeDESIGN,
    modeNOTE,
    modeIMAGE,
    modeTEXT,
    modeCONNECTION
};

class Corkboard : public wxPanel {
private:
    Outline* parent = nullptr;
    wxToolBar* toolBar = nullptr;

    wxBoxSizer* corkboardSizer = nullptr;

    CorkboardCanvas* canvas = nullptr;
    wxSFDiagramManager manager;

    ToolMode toolMode = modeDESIGN;
    bool isDraggingRight = false;

    int currentImage = 1;

public:
    Corkboard(wxWindow* parent);

    void onTool(wxCommandEvent& event);

    void callFullScreen(wxCommandEvent& event);
    void fullScreen(bool fs);

    void setToolMode(ToolMode mode);
    ToolMode getToolMode() { return toolMode; }
    wxToolBar* getToolbar() { return toolBar; }

    void exportToImage(wxBitmapType type);

    void save(std::ofstream& out);
    void load(std::ifstream& in);
    
    //friend class CorkboardCanvas;

    enum {
        TOOL_Cursor,
        TOOL_NewNote,
        TOOL_NewImage,
        TOOL_NewText,
        TOOL_NewConnection,
        TOOL_CorkboardFullScreen
    };

    DECLARE_EVENT_TABLE()
};
#endif