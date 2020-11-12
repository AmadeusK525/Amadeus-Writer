#include "Corkboard.h"

#include "MyApp.h"
#include "MainFrame.h"
#include "ImagePanel.h"
#include "Outline.h"
#include "CorkboardCanvas.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(Corkboard, wxPanel)

EVT_TOOL(TOOL_NewNote, Corkboard::addNote)
EVT_TOOL(TOOL_NewImage, Corkboard::addImage)

EVT_TOOL(TOOL_CorkboardFullScreen, Corkboard::callFullScreen)

END_EVENT_TABLE()

Corkboard::Corkboard(wxWindow* parent) : wxPanel(parent) {
    this->parent = (Outline*)(parent->GetParent());

    toolBar = new wxToolBar(this, -1);
    toolBar->AddTool(TOOL_NewNote, "", wxBITMAP_PNG(newNote), "New note");
    toolBar->AddTool(TOOL_NewImage, "", wxBITMAP_PNG(newImage), "New image");
    toolBar->AddSeparator();
    toolBar->AddTool(TOOL_ResetCenter, "", wxBITMAP_PNG(resetCenter), "Reset to center");
    toolBar->AddSeparator();
    toolBar->AddSeparator();
    toolBar->AddCheckTool(TOOL_CorkboardFullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Full screen");
    toolBar->Realize();
    toolBar->SetBackgroundColour(wxColour(140, 140, 140));

    SetBackgroundColour(wxColour(0, 0, 0));

    manager.AcceptShape("wxSFAllShapes");
    canvas = new CorkboardCanvas(&manager, this);

    canvas->AddStyle(wxSFShapeCanvas::sfsGRID_USE);
    canvas->AddStyle(wxSFShapeCanvas::sfsGRID_SHOW);

    corkboardSizer = new wxBoxSizer(wxVERTICAL);
    corkboardSizer->Add(toolBar, wxSizerFlags(0).Expand().Border(wxALL, 1));
    corkboardSizer->Add(canvas, wxSizerFlags(1).Expand());

    SetSizer(corkboardSizer);
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

void Corkboard::callFullScreen(wxCommandEvent& event) {
    wxKeyEvent keyEvent;
    keyEvent.m_keyCode = WXK_F11;

    canvas->OnKeyDown(keyEvent);
}

void Corkboard::fullScreen(bool fs) {
    if (fs) {
        parent->corkHolderSizer->Remove(0);
        Reparent(wxGetApp().GetTopWindow());
    } else {
        Reparent(parent->corkHolder);
        parent->corkHolderSizer->Add(this, wxSizerFlags(1).Expand());
    }

    corkboardSizer->Layout();
}