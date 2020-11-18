#include "Corkboard.h"

#include "MyApp.h"
#include "MainFrame.h"
#include "ImagePanel.h"
#include "Outline.h"
#include "CorkboardCanvas.h"
#include "NoteShape.h"
#include "ImageShape.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(Corkboard, wxPanel)

EVT_TOOL_RANGE(TOOL_Cursor, TOOL_CorkboardFullScreen, Corkboard::onTool)

END_EVENT_TABLE()

Corkboard::Corkboard(wxWindow* parent) : wxPanel(parent) {
    this->parent = (Outline*)(parent->GetParent());

    toolBar = new wxToolBar(this, -1);
    toolBar->AddRadioTool(TOOL_Cursor, "", wxBITMAP_PNG(cursor), wxNullBitmap, "Default");
    toolBar->AddRadioTool(TOOL_NewNote, "", wxBITMAP_PNG(newNote), wxNullBitmap, "New note");
    toolBar->AddRadioTool(TOOL_NewImage, "", wxBITMAP_PNG(newImage), wxNullBitmap, "New image");
    toolBar->AddRadioTool(TOOL_NewConnection, "", wxBITMAP_PNG(connectionLine), wxNullBitmap, "New connection");
    //toolBar->AddSeparator();
    //toolBar->AddTool(TOOL_ResetCenter, "", wxBITMAP_PNG(resetCenter), "Reset to center");
    toolBar->AddSeparator();
    toolBar->AddSeparator();
    toolBar->AddCheckTool(TOOL_CorkboardFullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Full screen");
    toolBar->Realize();
    toolBar->SetBackgroundColour(wxColour(140, 140, 140));

    SetBackgroundColour(wxColour(0, 0, 0));

    manager.AcceptShape("All");
    canvas = new CorkboardCanvas(&manager, this);

    corkboardSizer = new wxBoxSizer(wxVERTICAL);
    corkboardSizer->Add(toolBar, wxSizerFlags(0).Expand().Border(wxALL, 1));
    corkboardSizer->Add(canvas, wxSizerFlags(1).Expand());

    SetSizer(corkboardSizer);
}

void Corkboard::onTool(wxCommandEvent& event) {
    switch (event.GetId()) {
    case TOOL_Cursor:
        toolMode = modeDEFAULT;
        break;

    case TOOL_NewNote:
        toolMode = modeNOTE;
        break;

    case TOOL_NewImage:
        toolMode = modeIMAGE;
        break;
    case TOOL_NewConnection:
        toolMode = modeCONNECTION;
        break;
    case TOOL_FullScreen:
        callFullScreen(event);
        break;
    }
}

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

    toolBar->ToggleTool(TOOL_CorkboardFullScreen, fs);
    corkboardSizer->Layout();
}

void Corkboard::setToolMode(ToolMode mode) {
    switch (mode) {
    case modeDEFAULT:
        toolBar->ToggleTool(TOOL_Cursor, true);
        break;
    case modeNOTE:
        toolBar->ToggleTool(TOOL_NewNote, true);
        break;
    case modeIMAGE:
        toolBar->ToggleTool(TOOL_NewImage, true);
        break;
    case modeCONNECTION:
        toolBar->ToggleTool(TOOL_NewConnection, true);
        break;
    }

    toolMode = mode;
}
