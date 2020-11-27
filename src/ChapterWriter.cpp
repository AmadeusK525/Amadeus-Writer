#include "ChapterWriter.h"
#include "Chapter.h"
#include "ChapterWriterNotebook.h"

#include <wx/popupwin.h>
#include <wx/listbox.h>
#include <wx/richtext/richtextxml.h>

#include "wxmemdbg.h"


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

    this->mainFrame = dynamic_cast<MainFrame*>(parent);
    this->chapNote = notebook;
    chapterPos = numb;

    Hide();
    SetBackgroundColour(wxColour(100, 100, 100));

    chapWriterNotebook = new ChapterWriterNotebook(this);

    wxNotebook* leftNotebook = new wxNotebook(this, -1);

    leftPanel = new wxPanel(leftNotebook, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED);
    leftPanel->SetBackgroundColour(wxColour(60, 60, 60));

    wxPanel* sumPanel = new wxPanel(leftPanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    sumPanel->SetBackgroundColour(wxColour(255, 255, 255));

    summary = new wxTextCtrl(sumPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    wxStaticText* sumLabel = new wxStaticText(sumPanel, -1, "Synopsys", wxDefaultPosition, wxDefaultSize);
    sumLabel->SetBackgroundColour(wxColour(150, 0, 0));
    sumLabel->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased()));
    sumLabel->SetForegroundColour(wxColour(230, 230, 230));

    wxBoxSizer* sumSizer = new wxBoxSizer(wxVERTICAL);
    sumSizer->Add(sumLabel, wxSizerFlags(0).Expand());
    sumSizer->Add(summary, wxSizerFlags(1).Expand());
    sumPanel->SetSizer(sumSizer);

    charPanel = new wxPanel(leftPanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    charPanel->SetBackgroundColour(wxColour(210, 210, 210));
    charInChap = new wxListView(charPanel, -1, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_REPORT);
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

    locPanel = new wxPanel(leftPanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    locPanel->SetBackgroundColour(wxColour(210, 210, 210));
    locInChap = new wxListView(locPanel, -1, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_REPORT);
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

    wxPanel* rightPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED);
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

        MainNotebook::updateLB();
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

    MainNotebook::updateLB();
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

        MainNotebook::updateLB();
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

    MainNotebook::updateLB();
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
        chapWriterNotebook->content->Clear();
    }

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
}