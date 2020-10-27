#include "ChapterWriterNotebook.h"
#include "ChapterWriter.h"

#include "wx/richtext/richtextbuffer.h"
#include "wx/spinctrl.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(ChapterWriterNotebook, wxAuiNotebook)

EVT_TOOL(TOOL_Bold, ChapterWriterNotebook::setBold)
EVT_TOOL(TOOL_Italic, ChapterWriterNotebook::setItalic)
EVT_TOOL(TOOL_Underline, ChapterWriterNotebook::setUnderlined)
EVT_TOOL(TOOL_AlignLeft, ChapterWriterNotebook::setAlignLeft)
EVT_TOOL(TOOL_AlignCenter, ChapterWriterNotebook::setAlignCenter)
EVT_TOOL(TOOL_AlignCenterJust, ChapterWriterNotebook::setAlignCenterJust)
EVT_TOOL(TOOL_AlignRight, ChapterWriterNotebook::setAlignRight)

EVT_UPDATE_UI(TOOL_Bold, ChapterWriterNotebook::onUpdateBold)
EVT_UPDATE_UI(TOOL_Italic, ChapterWriterNotebook::onUpdateItalic)
EVT_UPDATE_UI(TOOL_Underline, ChapterWriterNotebook::onUpdateUnderline)
EVT_UPDATE_UI(TOOL_AlignLeft, ChapterWriterNotebook::onUpdateAlignLeft)
EVT_UPDATE_UI(TOOL_AlignCenter, ChapterWriterNotebook::onUpdateAlignCenter)
EVT_UPDATE_UI(TOOL_AlignCenterJust, ChapterWriterNotebook::onUpdateAlignCenterJust)
EVT_UPDATE_UI(TOOL_AlignRight, ChapterWriterNotebook::onUpdateAlignRight)

EVT_COMBOBOX(TOOL_FontSize, ChapterWriterNotebook::setFontSize)

EVT_TEXT(TEXT_Content, ChapterWriterNotebook::setModified)

END_EVENT_TABLE()

ChapterWriterNotebook::ChapterWriterNotebook(wxWindow* parent) :
    wxAuiNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT |
    wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP) {

    this->parent = reinterpret_cast<ChapterWriter*>(parent);

    wxPanel* mainPanel = new wxPanel(this, 1, wxDefaultPosition, wxDefaultSize);
    mainPanel->SetBackgroundColour(wxColour(100, 100, 100));

    content = new wxRichTextCtrl(mainPanel, TEXT_Content, "", wxDefaultPosition, wxDefaultSize,
        wxRE_MULTILINE | wxBORDER_NONE);
    content->SetScrollRate(50, 50);
    wxRichTextBuffer::AddHandler(new wxRichTextPlainTextHandler);

    contentTool = new wxToolBar(mainPanel, -1, wxDefaultPosition, wxSize(-1, -1));
    //contentTool->SetBackgroundColour(wxColour(180, 40, 40));
    contentTool->AddCheckTool(TOOL_Bold, "", wxBITMAP_PNG(bold), wxBITMAP_PNG(bold), "Bold");
    contentTool->AddCheckTool(TOOL_Italic, "", wxBITMAP_PNG(italic), wxBITMAP_PNG(italic), "italic");
    contentTool->AddCheckTool(TOOL_Underline, "", wxBITMAP_PNG(underline), wxBITMAP_PNG(underline), "Underline");
    contentTool->AddSeparator();
    contentTool->AddRadioTool(TOOL_AlignLeft, "", wxBITMAP_PNG(leftAlign), wxBITMAP_PNG(leftAlign), "Align left");
    contentTool->AddRadioTool(TOOL_AlignCenter, "", wxBITMAP_PNG(centerAlign), wxBITMAP_PNG(centerAlign), "Align center");
    contentTool->AddRadioTool(TOOL_AlignCenterJust, "", wxBITMAP_PNG(centerJustAlign), wxBITMAP_PNG(centerJustAlign), "Align center and fit");
    contentTool->AddRadioTool(TOOL_AlignRight, "", wxBITMAP_PNG(rightAlign), wxBITMAP_PNG(rightAlign), "Align right");
    contentTool->AddSeparator();

    wxArrayString sizes;
    for (int i = 8; i < 28; i++) {
        if (i > 12)
            i++;

        sizes.Add(std::to_string(i));
    }
    sizes.Add("36");
    sizes.Add("48");
    sizes.Add("72");

    fontSize = new wxComboBox(contentTool, TOOL_FontSize, "10", wxDefaultPosition, wxDefaultSize,
        sizes, wxCB_READONLY | wxCB_SIMPLE);

    contentTool->AddControl(fontSize);

    contentTool->Realize();

    wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(contentTool, wxSizerFlags(0).Expand().Border(wxBOTTOM, 3));
    pageSizer->Add(content, wxSizerFlags(1).Expand());

    mainPanel->SetSizer(pageSizer);

    corkBoard = new ImagePanel(this, wxDefaultPosition, wxDefaultSize);
    corkBoard->SetWindowStyle(wxBORDER_SUNKEN);

    notesSizer = new wxWrapSizer();
    corkBoard->SetSizer(notesSizer);
    corkBoard->SetScrollRate(15, 15);

    AddPage(mainPanel, "First View");
    AddPage(corkBoard, "Chapter notes");

    //content->BeginParagraphSpacing(30, 30);

    wxTimer timer(this, 12345);
    timer.Start(10000);
}

void ChapterWriterNotebook::setModified(wxCommandEvent& event) {
    parent->statusBar->SetStatusText("Chapter modified. Autosaving soon...", 0);
}

void ChapterWriterNotebook::setBold(wxCommandEvent& event) {
    content->ApplyBoldToSelection();
    setModified(event);
}

void ChapterWriterNotebook::setItalic(wxCommandEvent& event) {
    content->ApplyItalicToSelection();
    setModified(event);
}

void ChapterWriterNotebook::setUnderlined(wxCommandEvent& event) {
    content->ApplyUnderlineToSelection();
    setModified(event);
}

void ChapterWriterNotebook::setAlignLeft(wxCommandEvent& event) {
    content->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_LEFT));
}

void ChapterWriterNotebook::setAlignCenter(wxCommandEvent& event) {
    content->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_CENTER));
}

void ChapterWriterNotebook::setAlignCenterJust(wxCommandEvent& event) {
    content->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_JUSTIFIED));
}

void ChapterWriterNotebook::setAlignRight(wxCommandEvent& event) {
    content->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_RIGHT));
}

void ChapterWriterNotebook::onUpdateBold(wxUpdateUIEvent& event) {
    event.Check(content->IsSelectionBold());
}

void ChapterWriterNotebook::onUpdateItalic(wxUpdateUIEvent& event) {
    event.Check(content->IsSelectionItalics());
}

void ChapterWriterNotebook::onUpdateUnderline(wxUpdateUIEvent& event) {
    event.Check(content->IsSelectionUnderlined());
}

void ChapterWriterNotebook::onUpdateAlignLeft(wxUpdateUIEvent& event) {
    event.Check(content->IsSelectionAligned(wxTEXT_ALIGNMENT_LEFT));
}

void ChapterWriterNotebook::onUpdateAlignCenter(wxUpdateUIEvent& event) {
    event.Check(content->IsSelectionAligned(wxTEXT_ALIGNMENT_CENTER));
}

void ChapterWriterNotebook::onUpdateAlignCenterJust(wxUpdateUIEvent& event) {
    event.Check(content->IsSelectionAligned(wxTEXT_ALIGNMENT_JUSTIFIED));
}

void ChapterWriterNotebook::onUpdateAlignRight(wxUpdateUIEvent& event) {
    event.Check(content->IsSelectionAligned(wxTEXT_ALIGNMENT_RIGHT));
}

void ChapterWriterNotebook::setFontSize(wxCommandEvent& event) {
    content->BeginFontSize(event.GetInt());
}

void ChapterWriterNotebook::addNote(std::string& noteContent, std::string& noteName, bool isDone) {

    wxPanel* notePanel = new wxPanel(corkBoard);

    wxTextCtrl* noteLabel = new wxTextCtrl(notePanel, -1, noteName);
    noteLabel->SetValue(noteName);
    noteLabel->SetBackgroundColour(wxColour(240, 240, 240));
    wxPanel* options = new wxPanel(notePanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    options->SetBackgroundColour(wxColour(200, 200, 200));
    wxRichTextCtrl* note = new wxRichTextCtrl(notePanel, -1, noteContent, wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE);
    note->SetBackgroundColour(wxColour(255, 250, 205));

    noteLabel->Bind(wxEVT_TEXT, &ChapterWriterNotebook::updateNoteLabel, this);
    note->Bind(wxEVT_TEXT, &ChapterWriterNotebook::updateNote, this);

    options->Bind(wxEVT_RIGHT_DOWN, &ChapterWriterNotebook::onNoteClick, this);
    options->Bind(wxEVT_LEFT_DOWN, &ChapterWriterNotebook::onNoteClick, this);
    options->Bind(wxEVT_PAINT, &ChapterWriterNotebook::paintDots, this);

    wxRadioButton* b1 = new wxRadioButton(notePanel, -1, "", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    wxRadioButton* b2 = new wxRadioButton(notePanel, -1, "", wxDefaultPosition, wxDefaultSize);

    b1->Bind(wxEVT_RADIOBUTTON, &ChapterWriterNotebook::setRed, this);
    b2->Bind(wxEVT_RADIOBUTTON, &ChapterWriterNotebook::setGreen, this);

    b1->SetBackgroundColour(wxColour(220, 0, 0));
    b2->SetBackgroundColour(wxColour(0, 210, 0));

    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(noteLabel, wxSizerFlags(8).Expand());
    topSizer->Add(options, wxSizerFlags(1).Expand());

    wxBoxSizer* bsiz = new wxBoxSizer(wxHORIZONTAL);
    bsiz->Add(b1, wxSizerFlags(0));
    bsiz->AddStretchSpacer(1);
    bsiz->Add(b2, wxSizerFlags(0));

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(topSizer, wxSizerFlags(0).Expand());
    siz->Add(note, wxSizerFlags(1).Expand());
    siz->Add(bsiz, wxSizerFlags(0).Expand());
    notePanel->SetSizer(siz);

    if (isDone) {
        notePanel->SetBackgroundColour(wxColour(0, 210, 0));
        b2->SetValue(true);
    } else {
        notePanel->SetBackgroundColour(wxColour(220, 0, 0));
        b1->SetValue(true);
    }

    notesSizer->Add(notePanel, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP, 22));
    notesSizer->SetItemMinSize(notesSizer->GetItemCount() - 1, notesSize);
    notesSizer->Layout();

    corkBoard->FitInside();

    Note thisNote(noteContent, noteName);
    thisNote.isDone = isDone;

    notes.push_back(thisNote);
}

void ChapterWriterNotebook::paintDots(wxPaintEvent& event) {
    wxPanel* pan = dynamic_cast<wxPanel*>(event.GetEventObject());
    wxPaintDC dc(pan);

    wxSize opSize(pan->GetSize());

    dc.SetBrush(wxBrush(wxColour(140, 140, 140)));
    dc.SetPen(wxPen(wxColour(140, 140, 140)));

    dc.DrawCircle(wxPoint(opSize.x / 5 - 1, opSize.y / 2), 2);
    dc.DrawCircle(wxPoint(opSize.x / 2 - 1, opSize.y / 2), 2);
    dc.DrawCircle(wxPoint(((opSize.x / 5) * 4) - 1, opSize.y / 2), 2);
}

void ChapterWriterNotebook::setRed(wxCommandEvent& event) {
    wxRadioButton* rb = dynamic_cast<wxRadioButton*>(event.GetEventObject());
    wxPanel* np = dynamic_cast<wxPanel*>(rb->GetParent());

    wxPanel* sp;
    int i = 0;
    for (auto it = notes.begin(); it != notes.end(); it++) {
        sp = dynamic_cast<wxPanel*>(notesSizer->GetItem(i)->GetWindow());

        if (sp == np) {
            it->isDone = false;
            np->SetBackgroundColour(wxColour(220, 0, 0));
            np->Refresh();
            break;
        }

        i++;
    }
}

void ChapterWriterNotebook::setGreen(wxCommandEvent& event) {
    wxRadioButton* gb = dynamic_cast<wxRadioButton*>(event.GetEventObject());
    wxPanel* np = dynamic_cast<wxPanel*>(gb->GetParent());

    wxPanel* sp;
    int i = 0;
    for (auto it = notes.begin(); it != notes.end(); it++) {
        sp = dynamic_cast<wxPanel*>(notesSizer->GetItem(i)->GetWindow());

        if (sp == np) {
            it->isDone = true;
            np->SetBackgroundColour(wxColour(0, 210, 0));
            np->Refresh();
            break;
        }

        i++;
    }
}

void ChapterWriterNotebook::deleteNote(wxCommandEvent& event) {
    if (selNote) {
        wxPanel* sp;

        int i = 0;
        for (auto it = notes.begin(); it != notes.end(); it++) {
            sp = dynamic_cast<wxPanel*>(notesSizer->GetItem(i)->GetWindow());

            if (sp == selNote) {
                notes.erase(it);
                selNote->Destroy();
                notesSizer->Layout();
                corkBoard->FitInside();
                notes.shrink_to_fit();
                break;
            }

            i++;
        }
        

        selNote = nullptr;
    }
}

void ChapterWriterNotebook::onNoteClick(wxMouseEvent& event) {
    wxPanel* pan = dynamic_cast<wxPanel*>(event.GetEventObject());

    selNote = dynamic_cast<wxPanel*>(pan->GetParent());

    wxMenu menu;
    menu.Append(MENU_Delete, "Delete");
    menu.Bind(wxEVT_MENU, &ChapterWriterNotebook::deleteNote, this, MENU_Delete);
    pan->PopupMenu(&menu, wxPoint(-1, pan->GetSize().y));
}

void ChapterWriterNotebook::updateNoteLabel(wxCommandEvent& event) {
    wxTextCtrl* ttc = (wxTextCtrl*)event.GetEventObject();
    wxPanel* pan = dynamic_cast<wxPanel*>(ttc->GetParent());

    wxPanel* sp;
    int i = 0;
    for (auto it = notes.begin(); it != notes.end(); it++) {
        sp = dynamic_cast<wxPanel*>(notesSizer->GetItem(i)->GetWindow());
        if (sp == pan) {
            it->name = ttc->GetValue();
        }

        i++;
    }
}

void ChapterWriterNotebook::updateNote(wxCommandEvent& event) {
    wxRichTextCtrl* trtc = (wxRichTextCtrl*)event.GetEventObject();
    wxPanel* pan = dynamic_cast<wxPanel*>(trtc->GetParent());

    wxPanel* sp;
    int i = 0;
    for (auto it = notes.begin(); it != notes.end(); it++) {
        sp = dynamic_cast<wxPanel*>(notesSizer->GetItem(i)->GetWindow());
        if (sp == pan) {
            it->content = trtc->GetValue();
        }

        i++;
    }
}
