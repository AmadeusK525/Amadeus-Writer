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

    manager.AcceptShape("All");
    canvas = new CorkboardCanvas(&manager, this);

    corkboardSizer = new wxBoxSizer(wxVERTICAL);
    corkboardSizer->Add(toolBar, wxSizerFlags(0).Expand().Border(wxALL, 1));
    corkboardSizer->Add(canvas, wxSizerFlags(1).Expand());

    SetSizer(corkboardSizer);
}

void Corkboard::addNote(wxCommandEvent& event) {
    NoteShape* pShape = (NoteShape*)manager.AddShape(CLASSINFO(NoteShape),
        wxPoint(wxGetMousePosition().x + 50, wxGetMousePosition().y + 50));

    pShape->AcceptConnection("All");
    pShape->AcceptSrcNeighbour("All");
    pShape->AcceptTrgNeighbour("All");
    pShape->AcceptChild("All");

    // Show shadows only on the topmost shapes.
    canvas->ShowShadows(true, wxSFShapeCanvas::shadowTOPMOST);

    // ... and then perform standard operations provided by the shape canvas:
    Refresh(false);
}

void Corkboard::addImage(wxCommandEvent& event) {
    wxFileDialog dlg(this, wxT("Load bitmap image..."), wxGetCwd(), wxT(""),
        wxT("BMP Files, JPEG Files, JPG Files,  PNG Files (*.bmp;*.jpeg;*.jpg;*.png)|*.bmp;*.jpeg;*.jpg;*.png"),
        wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_OK) {
        ImageShape* pShape = (ImageShape*)(manager.AddShape(CLASSINFO(ImageShape),
            wxPoint(wxGetMousePosition().x + 50, wxGetMousePosition().y + 50), sfDONT_SAVE_STATE));

        if (pShape) {
            // create image from BMP file
            pShape->CreateFromFile(dlg.GetPath(), wxBITMAP_TYPE_ANY);

            // set shape policy
            pShape->AcceptConnection("All");
            pShape->AcceptSrcNeighbour("All");
            pShape->AcceptTrgNeighbour("All");
            pShape->AcceptChild("All");
        }
    }

    // Show shadows only on the topmost shapes.
    canvas->ShowShadows(true, wxSFShapeCanvas::shadowTOPMOST);

    // ... and then perform standard operations provided by the shape canvas:
    Refresh(false);
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