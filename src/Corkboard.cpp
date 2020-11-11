#include "Corkboard.h"

#include "MainFrame.h"
#include "ImagePanel.h"
#include "Outline.h"

#include "CorkboardCanvas.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(Corkboard, wxPanel)

EVT_TOOL(TOOL_NewNote, Corkboard::addNote)
EVT_TOOL(TOOL_NewImage, Corkboard::addImage)

END_EVENT_TABLE()

Corkboard::Corkboard(wxWindow* parent) : wxPanel(parent) {
    this->parent = dynamic_cast<Outline*>(parent);

    toolBar = new wxToolBar(this, -1);
    toolBar->AddTool(TOOL_NewNote, "", wxBITMAP_PNG(newNote), "New note");
    toolBar->AddTool(TOOL_NewImage, "", wxBITMAP_PNG(newImage), "New image");
    toolBar->AddSeparator();
    toolBar->AddTool(TOOL_ResetCenter, "", wxBITMAP_PNG(resetCenter), "Reset to center");
    toolBar->Realize();
    toolBar->SetBackgroundColour(wxColour(140, 140, 140));

    SetBackgroundColour(wxColour(0, 0, 0));

    manager.AcceptShape("wxSFAllShapes");
    canvas = new CorkboardCanvas(&manager, this);

    canvas->AddStyle(wxSFShapeCanvas::sfsGRID_USE);
    canvas->AddStyle(wxSFShapeCanvas::sfsGRID_SHOW);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(toolBar, wxSizerFlags(0).Expand().Border(wxALL, 1));
    sizer->Add(canvas, wxSizerFlags(1).Expand());

    SetSizer(sizer);
}

void Corkboard::addNote(wxCommandEvent& event) {
    wxSFShapeBase* pShape =
        manager.AddShape(CLASSINFO(wxSFRectShape),
        wxPoint(wxGetMousePosition().x + 50, wxGetMousePosition().y + 50));

    if (pShape) {
        // set accepted child shapes for the
        // new shape ...
        pShape->AcceptChild
        ("wxSFRectShape");
    }
    // ... and then perform standard
    // operations provided by the shape
    // canvas:
    Refresh(false);
    event.Skip();
}

void Corkboard::addImage(wxCommandEvent& event) {}
