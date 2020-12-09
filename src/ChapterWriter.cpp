#include "ChapterWriter.h"
#include "Chapter.h"

#include <wx/popupwin.h>
#include <wx/listbox.h>
#include <wx/richtext/richtextxml.h>

#include "wxmemdbg.h"

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// ChapterWriter //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(ChapterWriter, wxFrame)

EVT_BUTTON(BUTTON_NoteClear, ChapterWriter::clearNote)
EVT_BUTTON(BUTTON_NoteAdd, ChapterWriter::addNote)

EVT_BUTTON(BUTTON_AddChar, ChapterWriter::addCharButtonPressed)
EVT_BUTTON(BUTTON_RemChar, ChapterWriter::removeChar)
EVT_BUTTON(BUTTON_AddLoc, ChapterWriter::addLocButtonPressed)
EVT_BUTTON(BUTTON_RemLoc, ChapterWriter::removeLocButtonPressed)

EVT_BUTTON(BUTTON_NextChap, ChapterWriter::nextChap)
EVT_BUTTON(BUTTON_PreviousChap, ChapterWriter::prevChap)

EVT_TIMER(TIMER_Save, ChapterWriter::timerEvent)

EVT_CLOSE(ChapterWriter::onClose)

END_EVENT_TABLE()

namespace fs = boost::filesystem;

ChapterWriter::ChapterWriter(wxWindow* parent, ChaptersNotebook* notebook, int numb) :
    saveTimer(this, TIMER_Save), wordsTimer(this, TIMER_Words),
    wxFrame(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE) {

    this->mainFrame = (MainFrame*)(parent);
    this->chapNote = notebook;
    chapterPos = numb;

    Hide();
    SetBackgroundColour(wxColour(100, 100, 100));

    chapWriterNotebook = new ChapterWriterNotebook(this);
    //chapWriterNotebook->SetBackgroundColour(wxColour(20, 20, 20));
    //chapWriterNotebook->SetForegroundColour(wxColour(20, 20, 20));

    wxNotebook* leftNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

    leftPanel = new wxPanel(leftNotebook, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    leftPanel->SetBackgroundColour(wxColour(60, 60, 60));

    wxPanel* sumPanel = new wxPanel(leftPanel);
    sumPanel->SetBackgroundColour(wxColour(255, 255, 255));

    summary = new wxTextCtrl(sumPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    summary->SetBackgroundColour(wxColour(35, 35, 35));
    summary->SetForegroundColour(wxColour(245, 245, 245));

    wxStaticText* sumLabel = new wxStaticText(sumPanel, -1, "Synopsys", wxDefaultPosition, wxDefaultSize, wxBORDER_DEFAULT);
    sumLabel->SetBackgroundColour(wxColour(150, 0, 0));
    sumLabel->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased()));
    sumLabel->SetForegroundColour(wxColour(255, 255, 255));

    wxBoxSizer* sumSizer = new wxBoxSizer(wxVERTICAL);
    sumSizer->Add(sumLabel, wxSizerFlags(0).Expand());
    sumSizer->Add(summary, wxSizerFlags(1).Expand());
    sumPanel->SetSizer(sumSizer);

    charPanel = new wxPanel(leftPanel, -1);
    charPanel->SetBackgroundColour(wxColour(210, 210, 210));
    charInChap = new wxListView(charPanel, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_HRULES | wxLC_REPORT | wxBORDER_NONE);

    charInChap->SetBackgroundColour(wxColour(35, 35, 35));
    charInChap->SetForegroundColour(wxColour(245, 245, 245));
    charInChap->InsertColumn(0, "Characters present in chapter");

    wxButton* addCharButton = new wxButton(charPanel, BUTTON_AddChar, "Add");
    addCharButton->SetBackgroundColour(wxColour(240, 240, 240));
    wxButton* remCharButton = new wxButton(charPanel, BUTTON_RemChar, "Remove");
    remCharButton->SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* charBSizer = new wxBoxSizer(wxHORIZONTAL);
    charBSizer->Add(addCharButton);
    charBSizer->AddStretchSpacer();
    charBSizer->Add(remCharButton);

    wxBoxSizer* charactersSizer = new wxBoxSizer(wxVERTICAL);
    charactersSizer->Add(charInChap, wxSizerFlags(1).Expand());
    charactersSizer->Add(charBSizer, wxSizerFlags(0).Expand());
    charPanel->SetSizer(charactersSizer);

    locPanel = new wxPanel(leftPanel, -1);
    locPanel->SetBackgroundColour(wxColour(210, 210, 210));
    locInChap = new wxListView(locPanel, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_HRULES | wxLC_REPORT | wxBORDER_NONE);

    locInChap->SetBackgroundColour(wxColour(35, 35, 35));
    locInChap->SetForegroundColour(wxColour(245, 245, 245));
    locInChap->InsertColumn(0, "Locations present in chapter");

    wxButton* addLocButton = new wxButton(locPanel, BUTTON_AddLoc, "Add");
    addLocButton->SetBackgroundColour(wxColour(240, 240, 240));
    wxButton* remLocButton = new wxButton(locPanel, BUTTON_RemLoc, "Remove");
    remLocButton->SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* locBSizer = new wxBoxSizer(wxHORIZONTAL);
    locBSizer->Add(addLocButton);
    locBSizer->AddStretchSpacer();
    locBSizer->Add(remLocButton);

    wxBoxSizer* locationsSizer = new wxBoxSizer(wxVERTICAL);
    locationsSizer->Add(locInChap, wxSizerFlags(1).Expand());
    locationsSizer->Add(locBSizer, wxSizerFlags(0).Expand());
    locPanel->SetSizer(locationsSizer);

    wxButton* leftButton = new wxButton(leftPanel, BUTTON_PreviousChap, "", wxDefaultPosition, wxSize(25, 25));
    leftButton->SetBitmap(wxBITMAP_PNG(arrowLeft));

    leftSizer = new wxBoxSizer(wxVERTICAL);
    leftSizer->Add(sumPanel, wxSizerFlags(1).Expand().Border(wxALL, 8));
    leftSizer->Add(charPanel, wxSizerFlags(1).Expand().Border(wxALL, 8));
    leftSizer->Add(locPanel, wxSizerFlags(1).Expand().Border(wxALL, 8));
    leftSizer->Add(leftButton, wxSizerFlags(0).Right().Border(wxRIGHT | wxBOTTOM, 8));

    leftPanel->SetSizer(leftSizer);
    leftNotebook->AddPage(leftPanel,"Left");

    wxPanel* rightPanel = new wxPanel(this, -1);
    rightPanel->SetBackgroundColour(wxColour(60, 60, 60));

    noteCheck = new wxStaticText(rightPanel, -1, "Nothing to show.", wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED);
    noteCheck->SetBackgroundColour(wxColour(20, 20, 20));
    noteCheck->SetForegroundColour(wxColour(255, 255, 255));
    noteCheck->SetFont(wxFontInfo(10).Bold());

    noteLabel = new wxTextCtrl(rightPanel, -1, "New note", wxDefaultPosition, wxDefaultSize);
    noteLabel->SetBackgroundColour(wxColour(245, 245, 245));
    noteLabel->SetFont(wxFont(wxFontInfo(9)));

    note = new wxTextCtrl(rightPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    note->SetBackgroundColour(wxColour(255, 250, 205));
    note->SetFont(wxFont(wxFontInfo(9)));

    wxPanel* nbHolder = new wxPanel(rightPanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_DEFAULT);
    nbHolder->SetBackgroundColour(wxColour(255, 250, 205));

    noteClear = new wxButton(nbHolder, BUTTON_NoteClear, "Clear");
    noteClear->SetBackgroundColour(wxColour(240, 240, 240));

    noteAdd = new wxButton(nbHolder, BUTTON_NoteAdd, "Add");
    noteAdd->SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* nbSizer = new wxBoxSizer(wxHORIZONTAL);
    nbSizer->Add(noteClear, wxSizerFlags(0).Border(wxALL, 2));
    nbSizer->AddStretchSpacer(1);
    nbSizer->Add(noteAdd, wxSizerFlags(0).Border(wxALL, 2));

    nbHolder->SetSizer(nbSizer);

    wxButton* rightButton = new wxButton(rightPanel, BUTTON_NextChap, "", wxDefaultPosition, wxSize(25, 25));
    rightButton->SetBitmap(wxBITMAP_PNG(arrowRight));

    rightSizer = new wxBoxSizer(wxVERTICAL);
    rightSizer->AddStretchSpacer(2);
    rightSizer->Add(noteCheck, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 8));
    rightSizer->Add(noteLabel, wxSizerFlags(0).Expand().Border(wxTOP | wxRIGHT | wxLEFT, 8));
    rightSizer->Add(note, wxSizerFlags(1).Expand().Border(wxRIGHT | wxLEFT, 8));
    rightSizer->Add(nbHolder, wxSizerFlags(0).Expand().Border(wxBOTTOM | wxRIGHT | wxLEFT, 9));
    rightSizer->Add(rightButton, wxSizerFlags(0).Left().Border(wxLEFT | wxBOTTOM, 8));

    rightPanel->SetSizer(rightSizer);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(leftNotebook, wxSizerFlags(1).Expand());
    mainSizer->Add(chapWriterNotebook, wxSizerFlags(4).Expand());
    mainSizer->Add(rightPanel, wxSizerFlags(1).Expand());

    SetSizer(mainSizer);

    SetSize(1100, 700);
    Maximize();

    charInChap->SetColumnWidth(0, leftPanel->GetSize().x);
    locInChap->SetColumnWidth(0, leftPanel->GetSize().x);

    wxMenuBar* menu = new wxMenuBar();
    wxMenu* menu1 = new wxMenu();
    menu->Append(menu1, "File");

    SetMenuBar(menu);

    statusBar = CreateStatusBar(3);
    statusBar->SetStatusText("Chapter up-to-date", 0);
    statusBar->SetStatusText("Number of words: 0", 1);

    chapWriterNotebook->notesSize.x = (chapWriterNotebook->corkBoard->GetSize().x / 3) - 30;
    chapWriterNotebook->notesSize.y = (chapWriterNotebook->corkBoard->GetSize().y / 4) - 10;

    chapWriterNotebook->corkBoard->size = chapWriterNotebook->corkBoard->GetSize();
    chapWriterNotebook->corkBoard->setImageAsIs(wxBitmap(wxBITMAP_PNG(corkBoard)).ConvertToImage());

    SetIcon(wxICON(amadeus));

    loadChapter();

    Show();
    SetFocus();

    saveTimer.Start(10000);
}

void ChapterWriter::clearNote(wxCommandEvent& event) {
    note->Clear();
    //noteLabel->SetValue("New note");

    event.Skip();
}

void ChapterWriter::addNote(wxCommandEvent& event) {
    if (!note->IsEmpty()) {
        chapWriterNotebook->addNote((string)note->GetValue(), (string)noteLabel->GetValue(), green);
        note->Clear();
        noteLabel->SetValue("New note");
        green = false;

        checkNotes();
    }

    event.Skip();
}

void ChapterWriter::checkNotes() {
    if (chapWriterNotebook->notes.size()) {
        if (chapWriterNotebook->hasRedNote()) {
            noteCheck->SetBackgroundColour(wxColour(120, 0, 0));
            noteCheck->SetLabel("Unresolved notes!");
        } else {
            noteCheck->SetBackgroundColour(wxColour(0, 100, 0));
            noteCheck->SetLabel("All notes resolved!");
        }
    } else {
        noteCheck->SetBackgroundColour(wxColour(20, 20, 20));
        noteCheck->SetLabel("No notes yet.");
    }

    rightSizer->Layout();
}

void ChapterWriter::addCharButtonPressed(wxCommandEvent& event) {
    wxPoint point(charPanel->GetScreenPosition());

    int x = point.x;
    int y = point.y + charInChap->GetSize().y;
    
    wxPopupTransientWindow* win = new wxPopupTransientWindow(leftPanel, wxBORDER_RAISED | wxPU_CONTAINS_CONTROLS);

    wxArrayString array(true);
    for (auto it = MainFrame::characters.begin(); it != MainFrame::characters.end(); it++) {
        array.Add(it->second.name);
    }

    wxListBox* list = new wxListBox();
    list->Create(win, LIST_AddChar, wxDefaultPosition, wxDefaultSize, array, wxLB_NEEDED_SB | wxLB_SINGLE);
    list->Bind(wxEVT_LISTBOX_DCLICK, &ChapterWriter::addChar, this);

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(list, wxSizerFlags(1).Expand());
    win->SetSizer(siz);

    win->SetPosition(wxPoint(x, y));
    win->SetSize(charPanel->GetSize());

    win->Popup(list);
    event.Skip();
}

void ChapterWriter::addChar(wxCommandEvent& event) {
    string name = event.GetString();
    if (charInChap->FindItem(-1, name) == -1) {
        charNames.Add(name);
        charInChap->DeleteAllItems();

        for (unsigned int i = 0; i < charNames.GetCount(); i++) {
            charInChap->InsertItem(i, charNames[i]);
        }

        thisChap->characters.push_back(name);

        auto mapIt = MainFrame::characters.find("Main" + name);
        if (mapIt == MainFrame::characters.end())
            mapIt = MainFrame::characters.find("Secondary" + name);

        mapIt->second.chapters++;
        mapIt->second.hasAppeared = true;

        if (mapIt->second.chapters == 1)
            mapIt->second.firstChap = chapterPos;

        ElementsNotebook::updateLB();
    }
}

void ChapterWriter::removeChar(wxCommandEvent& event) {
    int sel = charInChap->GetFirstSelected();
    int nos = charInChap->GetSelectedItemCount();

    for (int i = 0; i < nos; i++) {
        string name = charInChap->GetItemText(sel);
        charInChap->DeleteItem(sel);
        charNames.Remove(name);

        auto it = thisChap->characters.begin();
        while (true) {
            if (*it == name) {
                thisChap->characters.erase(it);
                break;
            }
            it++;
        }

        thisChap->characters.shrink_to_fit();

        auto mapIt = MainFrame::characters.find("Main" + name);
        if (mapIt == MainFrame::characters.end())
            mapIt = MainFrame::characters.find("Secondary" + name);

        if (mapIt != MainFrame::characters.end()) {
            if ((--mapIt->second.chapters) == 0)
                mapIt->second.hasAppeared = false;
        }

        sel = charInChap->GetNextSelected(sel - 1);
    }

    ElementsNotebook::updateLB();
    event.Skip();
}

void ChapterWriter::addLocButtonPressed(wxCommandEvent& event) {
    wxPoint point(locPanel->GetScreenPosition());

    int x = point.x;
    int y = point.y + locInChap->GetSize().y;

    wxPopupTransientWindow* win = new wxPopupTransientWindow(locPanel, wxBORDER_RAISED | wxPU_CONTAINS_CONTROLS);

    wxArrayString array(true);
    for (auto it = MainFrame::locations.begin(); it != MainFrame::locations.end(); it++) {
        array.Add(it->second.name);
    }

    wxListBox* list = new wxListBox(win, -1, wxDefaultPosition, wxDefaultSize, array, wxLB_NEEDED_SB);
    list->Bind(wxEVT_LISTBOX_DCLICK, &ChapterWriter::addLoc, this);

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(list, 1, wxGROW);
    win->SetSizer(siz);

    win->SetPosition(wxPoint(x, y));
    win->SetSize(locPanel->GetSize());

    win->Popup();
    event.Skip();
}

void ChapterWriter::addLoc(wxCommandEvent& event) {
    string name = event.GetString();
    if (locInChap->FindItem(-1, name) == -1) {
        locNames.Add(name);
        locInChap->DeleteAllItems();

        for (unsigned int i = 0; i < locNames.GetCount(); i++) {
            locInChap->InsertItem(i, locNames[i]);
        }

        thisChap->locations.push_back(name);

        auto mapIt = MainFrame::locations.find("High" + name);
        if (mapIt == MainFrame::locations.end())
            mapIt = MainFrame::locations.find("Low" + name);

        mapIt->second.chapters++;
        mapIt->second.hasAppeared = true;

        if (mapIt->second.chapters == 1)
            mapIt->second.firstChap = chapterPos;

        ElementsNotebook::updateLB();
    }
}

void ChapterWriter::removeLocButtonPressed(wxCommandEvent& event) {
    int sel = locInChap->GetFirstSelected();
    int nos = locInChap->GetSelectedItemCount();

    for (int i = 0; i < nos; i++) {
        string name = locInChap->GetItemText(sel);
        locInChap->DeleteItem(sel);
        locNames.Remove(name);

        auto it = thisChap->locations.begin();
        while (true) {
            if (*it == name) {
                thisChap->locations.erase(it);
                break;
            }
            it++;
        }

        auto mapIt = MainFrame::locations.find("High" + name);
        if (mapIt == MainFrame::locations.end())
            mapIt = MainFrame::locations.find("Low" + name);

        if (mapIt != MainFrame::locations.end()) {
            if ((--mapIt->second.chapters) == 0)
                mapIt->second.hasAppeared = false;
        }

    sel = locInChap->GetNextSelected(sel + 1);
    }

    ElementsNotebook::updateLB();
    event.Skip();
}

void ChapterWriter::nextChap(wxCommandEvent& event) {
    if (chapterPos < chapNote->chapters.size()) {
        saveChapter();
        chapterPos++;
        chapWriterNotebook->content->EndAllStyles();

        chapWriterNotebook->notes.clear();
        if (chapWriterNotebook->notesSizer->GetItemCount() > 0)
            chapWriterNotebook->notesSizer->Clear(true);

        loadChapter();

        chapWriterNotebook->corkBoard->FitInside();
    }
    event.Skip();
}

void ChapterWriter::prevChap(wxCommandEvent& event) {
    if (chapterPos > 1) {
        saveChapter();
        chapterPos--;
        chapWriterNotebook->notes.clear();

        if (chapWriterNotebook->notesSizer->GetItemCount() > 0) {
            chapWriterNotebook->notesSizer->Clear(true);
        }

        loadChapter();

        chapWriterNotebook->corkBoard->FitInside();
    }
    event.Skip();
}

void ChapterWriter::loadChapter() {
    wxBusyCursor busy;

    auto it = chapNote->chapters.begin();
    for (unsigned int i = 1; i < chapterPos; i++) {
        it++;
    }

    thisChap = &(*it);

    SetTitle(thisChap->name);

    if (fs::exists(MainFrame::currentDocFolder + "\\Files\\Chapters\\" +
        std::to_string(thisChap->position) + " - " + thisChap->name + ".xml")) {
        
        chapWriterNotebook->content->LoadFile(mainFrame->currentDocFolder + "\\Files\\Chapters\\" +
            std::to_string(thisChap->position) + " - " + thisChap->name + ".xml", wxRICHTEXT_TYPE_XML);
    } else {
        chapWriterNotebook->content->GetBuffer() = thisChap->content;
    }

    thisChap->content.SetBasicStyle(chapWriterNotebook->content->GetBasicStyle());

    summary->SetValue(it->summary);

    for (auto noteIt = it->notes.begin(); noteIt != it->notes.end(); noteIt++) {
        note->SetValue(noteIt->content);
        noteLabel->SetValue(noteIt->name);
        green = noteIt->isDone;

        addNote(wxCommandEvent());
    }

    chapWriterNotebook->corkBoard->Layout();

    charInChap->DeleteAllItems();
    charNames.clear();
    for (unsigned int i = 0; i < thisChap->characters.size(); i++) {
        charNames.Add(thisChap->characters[i]);
        charInChap->InsertItem(i, charNames[i]);
    }

    locInChap->DeleteAllItems();
    locNames.clear();
    for (unsigned int i = 0; i < thisChap->locations.size(); i++) {
        locNames.Add(thisChap->locations[i]);
        locInChap->InsertItem(i, locNames[i]);
    }

    checkNotes();
    countWords();
    statusBar->SetStatusText("Chapter up-to-date", 0);
}

void ChapterWriter::saveChapter() {
    thisChap->content = chapWriterNotebook->content->GetBuffer();
    thisChap->content.SetBasicStyle(chapWriterNotebook->content->GetBasicStyle());
    thisChap->summary = (string)summary->GetValue();

    thisChap->notes.clear();
    for (unsigned int i = 0; i < chapWriterNotebook->notes.size(); i++) {
        thisChap->notes.push_back(chapWriterNotebook->notes[i]);
    }

    if (fs::exists(mainFrame->currentDocFolder + "\\Files\\Chapters\\")) {
        thisChap->content.SaveFile(mainFrame->currentDocFolder + "\\Files\\Chapters\\" +
            std::to_string(thisChap->position) + " - " + thisChap->name + ".xml", wxRICHTEXT_TYPE_XML);
    }
}

void ChapterWriter::countWords() {
    string count = chapWriterNotebook->content->GetValue();

    if (count != "") {
        int words = std::count(count.begin(), count.end(), ' ');
        statusBar->SetStatusText("Number of words: " + std::to_string(words + 1), 1);
    } else {
        statusBar->SetStatusText("Number of words: 0", 1);
    }
}

void ChapterWriter::timerEvent(wxTimerEvent& event) {
    statusBar->SetStatusText("Autosaving chapter...", 0);

    saveChapter();
    countWords();

    if (this->IsShown())
        mainFrame->Show();

    statusBar->SetStatusText("Chapter up-to-date", 0);

    event.Skip();
}

void ChapterWriter::onClose(wxCloseEvent& event) {
    saveChapter();
    mainFrame->saveFile(wxCommandEvent());

    Destroy();
    event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// ChapterWriterNotebook //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(ChapterWriterNotebook, wxAuiNotebook)

EVT_TOOL(TOOL_Bold, ChapterWriterNotebook::setBold)
EVT_TOOL(TOOL_Italic, ChapterWriterNotebook::setItalic)
EVT_TOOL(TOOL_Underline, ChapterWriterNotebook::setUnderlined)
EVT_TOOL(TOOL_AlignLeft, ChapterWriterNotebook::setAlignLeft)
EVT_TOOL(TOOL_AlignCenter, ChapterWriterNotebook::setAlignCenter)
EVT_TOOL(TOOL_AlignCenterJust, ChapterWriterNotebook::setAlignCenterJust)
EVT_TOOL(TOOL_AlignRight, ChapterWriterNotebook::setAlignRight)

EVT_TOOL(TOOL_ChapterFullScreen, ChapterWriterNotebook::onFullScreen)
EVT_TOOL(TOOL_PageView, ChapterWriterNotebook::onPageView)

EVT_UPDATE_UI(TOOL_Bold, ChapterWriterNotebook::onUpdateBold)
EVT_UPDATE_UI(TOOL_Italic, ChapterWriterNotebook::onUpdateItalic)
EVT_UPDATE_UI(TOOL_Underline, ChapterWriterNotebook::onUpdateUnderline)
EVT_UPDATE_UI(TOOL_AlignLeft, ChapterWriterNotebook::onUpdateAlignLeft)
EVT_UPDATE_UI(TOOL_AlignCenter, ChapterWriterNotebook::onUpdateAlignCenter)
EVT_UPDATE_UI(TOOL_AlignCenterJust, ChapterWriterNotebook::onUpdateAlignCenterJust)
EVT_UPDATE_UI(TOOL_AlignRight, ChapterWriterNotebook::onUpdateAlignRight)
EVT_UPDATE_UI(TOOL_FontSize, ChapterWriterNotebook::onUpdateFontSize)

EVT_COMBOBOX(TOOL_FontSize, ChapterWriterNotebook::setFontSize)

EVT_TEXT(TEXT_Content, ChapterWriterNotebook::setModified)
EVT_RICHTEXT_CONSUMING_CHARACTER(TEXT_Content, ChapterWriterNotebook::onKeyDown)

END_EVENT_TABLE()

ChapterWriterNotebook::ChapterWriterNotebook(wxWindow* parent) :
    wxAuiNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT |
    wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP) {

    this->parent = reinterpret_cast<ChapterWriter*>(parent);

    wxPanel* mainPanel = new wxPanel(this, 1, wxDefaultPosition, wxDefaultSize);
    mainPanel->SetBackgroundColour(wxColour(100, 100, 100));

    content = new wxRichTextCtrl(mainPanel, TEXT_Content, "", wxDefaultPosition, wxDefaultSize,
        wxRE_MULTILINE | wxBORDER_NONE);
    wxRichTextBuffer::AddHandler(new wxRichTextPlainTextHandler);

    wxRichTextAttr attr;
    attr.SetFont(wxFontInfo(10));
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    attr.SetLeftIndent(63, -63);
    attr.SetTextColour(wxColour(245, 245, 245));
    content->SetBasicStyle(attr);
    content->SetBackgroundColour(wxColour(35, 35, 35));

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
    contentTool->AddStretchableSpace();
    contentTool->AddCheckTool(TOOL_ChapterFullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Toggle Full Screen");
    //contentTool->AddCheckTool()

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

    AddPage(mainPanel, "Main");
    AddPage(corkBoard, "Notes");

    wxTimer timer(this, 12345);
    timer.Start(10000);
}

void ChapterWriterNotebook::setModified(wxCommandEvent& WXUNUSED(event)) {
    parent->statusBar->SetStatusText("Chapter modified. Autosaving soon...", 0);
}

void ChapterWriterNotebook::onKeyDown(wxRichTextEvent& event) {
    if (wxGetKeyState(WXK_CONTROL)) {
        switch (event.GetCharacter()) {
        case 'n':
        case 'N':
        case 'b':
        case 'B':
            setBold(event);
            break;

        case 'i':
        case 'I':
            setItalic(event);
            break;

        case 's':
        case 'S':
        case 'u':
        case 'U':
            setUnderlined(event);
            break;
        }
    }
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
    setModified(event);
}

void ChapterWriterNotebook::setAlignCenter(wxCommandEvent& event) {
    content->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_CENTER));
    setModified(event);
}

void ChapterWriterNotebook::setAlignCenterJust(wxCommandEvent& event) {
    content->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_JUSTIFIED));
    setModified(event);
}

void ChapterWriterNotebook::setAlignRight(wxCommandEvent& event) {
    content->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_RIGHT));
    setModified(event);
}

void ChapterWriterNotebook::onFullScreen(wxCommandEvent& WXUNUSED(event)) {
    parent->toggleFullScreen();
}

void ChapterWriterNotebook::onPageView(wxCommandEvent& WXUNUSED(event)) {}

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

void ChapterWriterNotebook::onUpdateFontSize(wxUpdateUIEvent& WXUNUSED(event)) {
    wxRichTextAttr attr;
    content->GetStyle(content->GetInsertionPoint() - 1, attr);
    string size(std::to_string(attr.GetFontSize()));

    if (fontSize->GetValue() != size)
        fontSize->SetValue(size);
}

void ChapterWriterNotebook::setFontSize(wxCommandEvent& event) {
    content->SetFocus();
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_SIZE);
    long size;
    event.GetString().ToLong(&size);
    attr.SetFontSize(size);

    if (content->HasSelection()) {
        content->SetStyleEx(content->GetSelectionRange(), attr,
            wxRICHTEXT_SETSTYLE_WITH_UNDO | wxRICHTEXT_SETSTYLE_OPTIMIZE | wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
    } else {
        wxRichTextAttr current = content->GetDefaultStyle();
        current.Apply(attr);
        content->SetDefaultStyle(current);
    }
}

bool ChapterWriterNotebook::hasRedNote() {
    for (int i = 0; i < notes.size(); i++) {
        if (notes[i].isDone == false)
            return true;
    }

    return false;
}

void ChapterWriterNotebook::addNote(std::string& noteContent, std::string& noteName, bool isDone) {

    wxPanel* notePanel = new wxPanel(corkBoard);

    wxTextCtrl* noteLabel = new wxTextCtrl(notePanel, -1, noteName);
    noteLabel->SetValue(noteName);
    noteLabel->SetBackgroundColour(wxColour(240, 240, 240));
    wxPanel* options = new wxPanel(notePanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    options->SetBackgroundColour(wxColour(200, 200, 200));
    wxTextCtrl* note = new wxTextCtrl(notePanel, -1, noteContent, wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE);
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

    parent->checkNotes();
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

    parent->checkNotes();
}

void ChapterWriterNotebook::deleteNote(wxCommandEvent& WXUNUSED(event)) {
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

    parent->checkNotes();
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