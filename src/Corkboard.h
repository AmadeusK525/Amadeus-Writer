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

class Corkboard : public wxPanel {
private:
    Outline* parent = nullptr;
    wxToolBar* toolBar = nullptr;

    wxBoxSizer* corkboardSizer = nullptr;

    CorkboardCanvas* canvas = nullptr;
    wxSFDiagramManager manager;

    bool isDraggingRight = false;

public:
    Corkboard(wxWindow* parent);

    void addNote(wxCommandEvent& event);
    void addImage(wxCommandEvent& event);
    void callFullScreen(wxCommandEvent& event);

    void fullScreen(bool fs);
    wxToolBar* getToolbar() { return toolBar; }

    DECLARE_EVENT_TABLE()
};

enum {
    TOOL_NewNote,
    TOOL_NewImage,
    TOOL_ResetCenter,
    TOOL_CorkboardFullScreen
};

#endif