#include "ChapterWriter.h"

#include "ElementsNotebook.h"

#include <wx\popupwin.h>
#include <wx\listbox.h>
#include <wx\richtext\richtextxml.h>

#include "wxmemdbg.h"

///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// amChapterWriter //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(amChapterWriter, wxFrame)

EVT_BUTTON(BUTTON_NoteClear, amChapterWriter::ClearNote)
EVT_BUTTON(BUTTON_NoteAdd, amChapterWriter::AddNote)

EVT_BUTTON(BUTTON_AddChar, amChapterWriter::OnAddCharacter)
EVT_BUTTON(BUTTON_AddLoc, amChapterWriter::OnAddLocation)
EVT_BUTTON(BUTTON_AddItem, amChapterWriter::OnAddItem)
EVT_BUTTON(BUTTON_RemChar, amChapterWriter::OnRemoveCharacter)
EVT_BUTTON(BUTTON_RemLoc, amChapterWriter::OnRemoveLocation)
EVT_BUTTON(BUTTON_RemItem, amChapterWriter::OnRemoveItem)

EVT_BUTTON(BUTTON_NextChap, amChapterWriter::OnNextChapter)
EVT_BUTTON(BUTTON_PreviousChap, amChapterWriter::OnPreviousChapter)

EVT_TIMER(TIMER_Save, amChapterWriter::OnTimerEvent)

EVT_CLOSE(amChapterWriter::OnClose)

END_EVENT_TABLE()

amChapterWriter::amChapterWriter(wxWindow* parent, amProjectManager* manager, int numb) :
    m_saveTimer(this, TIMER_Save),
    wxFrame(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
    wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE) {

    m_manager = manager;
    m_chapterPos = numb;

    Hide();
    SetBackgroundColour(wxColour(10, 10, 10));

    m_cwNotebook = new amChapterWriterNotebook(this);
    //cwNotebook->SetBackgroundColour(wxColour(20, 20, 20));
    //cwNotebook->SetForegroundColour(wxColour(20, 20, 20));

    wxNotebook* leftNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

    m_leftPanel = new wxPanel(leftNotebook, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_leftPanel->SetBackgroundColour(wxColour(60, 60, 60));

    m_characterPanel = new wxPanel(m_leftPanel, -1);
    m_characterPanel->SetBackgroundColour(wxColour(80, 80, 80));

    wxStaticText* charInChapLabel = new wxStaticText(m_characterPanel, -1, "Characters present in chapter",
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
    charInChapLabel->SetBackgroundColour(wxColour(25, 25, 25));
    charInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
    charInChapLabel->SetFont(wxFontInfo(10).Bold());

    m_charInChap = new wxListView(m_characterPanel, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_HRULES | wxLC_REPORT | wxLC_NO_HEADER | wxBORDER_NONE);
    m_charInChap->SetMinSize(FromDIP(wxSize(150, 150)));

    m_charInChap->SetBackgroundColour(wxColour(35, 35, 35));
    m_charInChap->SetForegroundColour(wxColour(245, 245, 245));
    m_charInChap->InsertColumn(0, "Characters present in chapter", wxLIST_ALIGN_LEFT, FromDIP(10));

    wxButton* addCharButton = new wxButton(m_characterPanel, BUTTON_AddChar, "Add");
    addCharButton->SetBackgroundColour(wxColour(240, 240, 240));
    wxButton* remCharButton = new wxButton(m_characterPanel, BUTTON_RemChar, "Remove");
    remCharButton->SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* charBSizer = new wxBoxSizer(wxHORIZONTAL);
    charBSizer->Add(addCharButton);
    charBSizer->AddStretchSpacer();
    charBSizer->Add(remCharButton);

    wxBoxSizer* charactersSizer = new wxBoxSizer(wxVERTICAL);
    charactersSizer->Add(charInChapLabel, wxSizerFlags(0).Expand());
    charactersSizer->Add(m_charInChap, wxSizerFlags(1).Expand());
    charactersSizer->Add(charBSizer, wxSizerFlags(0).Expand());
    m_characterPanel->SetSizer(charactersSizer);

    m_locationPanel = new wxPanel(m_leftPanel, -1);
    m_locationPanel->SetBackgroundColour(wxColour(80, 80, 80));

    wxStaticText* locInChapLabel = new wxStaticText(m_locationPanel, -1, "Locations present in chapter",
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
    locInChapLabel->SetBackgroundColour(wxColour(25, 25, 25));
    locInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
    locInChapLabel->SetFont(wxFontInfo(10).Bold());
    m_locInChap = new wxListView(m_locationPanel, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_HRULES | wxLC_REPORT | wxLC_NO_HEADER | wxBORDER_NONE);

    m_locInChap->SetMinSize(FromDIP(wxSize(150, 150)));
    m_locInChap->SetBackgroundColour(wxColour(35, 35, 35));
    m_locInChap->SetForegroundColour(wxColour(245, 245, 245));
    m_locInChap->InsertColumn(0, "Locations present in chapter", wxLIST_ALIGN_LEFT, FromDIP(155));

    wxButton* addLocButton = new wxButton(m_locationPanel, BUTTON_AddLoc, "Add");
    addLocButton->SetBackgroundColour(wxColour(240, 240, 240));
    wxButton* remLocButton = new wxButton(m_locationPanel, BUTTON_RemLoc, "Remove");
    remLocButton->SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* locBSizer = new wxBoxSizer(wxHORIZONTAL);
    locBSizer->Add(addLocButton);
    locBSizer->AddStretchSpacer();
    locBSizer->Add(remLocButton);

    wxBoxSizer* locationsSizer = new wxBoxSizer(wxVERTICAL);
    locationsSizer->Add(locInChapLabel, wxSizerFlags(0).Expand());
    locationsSizer->Add(m_locInChap, wxSizerFlags(1).Expand());
    locationsSizer->Add(locBSizer, wxSizerFlags(0).Expand());
    m_locationPanel->SetSizer(locationsSizer);

    m_itemPanel = new wxPanel(m_leftPanel, -1);
    m_itemPanel->SetBackgroundColour(wxColour(80, 80, 80));

    wxStaticText* itemsInChapLabel = new wxStaticText(m_itemPanel, -1, "Items present in chapter",
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
    itemsInChapLabel->SetBackgroundColour(wxColour(25, 25, 25));
    itemsInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
    itemsInChapLabel->SetFont(wxFontInfo(10).Bold());
    m_itemsInChap = new wxListView(m_itemPanel, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_HRULES | wxLC_REPORT | wxLC_NO_HEADER | wxBORDER_NONE);
    m_itemsInChap->SetMinSize(FromDIP(wxSize(150, 150)));

    m_itemsInChap->SetBackgroundColour(wxColour(35, 35, 35));
    m_itemsInChap->SetForegroundColour(wxColour(245, 245, 245));
    m_itemsInChap->InsertColumn(0, "Items present in chapter", wxLIST_ALIGN_LEFT, FromDIP(155));

    wxButton* addItemButton = new wxButton(m_itemPanel, BUTTON_AddItem, "Add");
    addLocButton->SetBackgroundColour(wxColour(240, 240, 240));
    wxButton* remItemButton = new wxButton(m_itemPanel, BUTTON_RemItem, "Remove");
    remLocButton->SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* itemBSizer = new wxBoxSizer(wxHORIZONTAL);
    itemBSizer->Add(addItemButton);
    itemBSizer->AddStretchSpacer();
    itemBSizer->Add(remItemButton);

    wxBoxSizer* itemsSizer = new wxBoxSizer(wxVERTICAL);
    itemsSizer->Add(itemsInChapLabel, wxSizerFlags(0).Expand());
    itemsSizer->Add(m_itemsInChap, wxSizerFlags(1).Expand());
    itemsSizer->Add(itemBSizer, wxSizerFlags(0).Expand());
    m_itemPanel->SetSizer(itemsSizer);

    wxButton* leftButton = new wxButton(m_leftPanel, BUTTON_PreviousChap, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
    leftButton->SetBitmap(wxBITMAP_PNG(arrowLeft));

    m_leftSizer = new wxBoxSizer(wxVERTICAL);
    m_leftSizer->Add(m_characterPanel, wxSizerFlags(1).Expand().Border(wxALL, 7));
    m_leftSizer->Add(m_locationPanel, wxSizerFlags(1).Expand().Border(wxALL, 7));
    m_leftSizer->Add(m_itemPanel, wxSizerFlags(1).Expand().Border(wxALL, 7));
    m_leftSizer->Add(leftButton, wxSizerFlags(0).Right().Border(wxRIGHT | wxBOTTOM, 8));

    m_leftPanel->SetSizer(m_leftSizer);
    leftNotebook->AddPage(m_leftPanel,"Elements");

    wxPanel* rightPanel = new wxPanel(this, -1);
    rightPanel->SetBackgroundColour(wxColour(60, 60, 60));

    wxPanel* sumPanel = new wxPanel(rightPanel);
    sumPanel->SetBackgroundColour(wxColour(255, 255, 255));

    m_summary = new wxTextCtrl(sumPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxBORDER_SIMPLE);
    m_summary->SetBackgroundColour(wxColour(35, 35, 35));
    m_summary->SetForegroundColour(wxColour(245, 245, 245));

    wxStaticText* sumLabel = new wxStaticText(sumPanel, -1, "Synopsys", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    sumLabel->SetBackgroundColour(wxColour(150, 0, 0));
    sumLabel->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased()));
    sumLabel->SetForegroundColour(wxColour(255, 255, 255));

    wxBoxSizer* sumSizer = new wxBoxSizer(wxVERTICAL);
    sumSizer->Add(sumLabel, wxSizerFlags(0).Expand());
    sumSizer->Add(m_summary, wxSizerFlags(1).Expand());
    sumPanel->SetSizer(sumSizer);

    m_noteChecker = new wxStaticText(rightPanel, -1, "Nothing to show.", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    m_noteChecker->SetBackgroundColour(wxColour(20, 20, 20));
    m_noteChecker->SetForegroundColour(wxColour(255, 255, 255));
    m_noteChecker->SetFont(wxFontInfo(10).Bold());

    m_noteLabel = new wxTextCtrl(rightPanel, -1, "New note", wxDefaultPosition, wxDefaultSize);
    m_noteLabel->SetBackgroundColour(wxColour(245, 245, 245));
    m_noteLabel->SetFont(wxFont(wxFontInfo(9)));

    m_note = new wxTextCtrl(rightPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_note->SetBackgroundColour(wxColour(255, 250, 205));
    m_note->SetFont(wxFont(wxFontInfo(9)));

    wxPanel* nbHolder = new wxPanel(rightPanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_DEFAULT);
    nbHolder->SetBackgroundColour(wxColour(255, 250, 205));

    m_noteClear = new wxButton(nbHolder, BUTTON_NoteClear, "Clear");
    m_noteClear->SetBackgroundColour(wxColour(240, 240, 240));

    m_noteAdd = new wxButton(nbHolder, BUTTON_NoteAdd, "Add");
    m_noteAdd->SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* nbSizer = new wxBoxSizer(wxHORIZONTAL);
    nbSizer->Add(m_noteClear, wxSizerFlags(0).Border(wxALL, 2));
    nbSizer->AddStretchSpacer(1);
    nbSizer->Add(m_noteAdd, wxSizerFlags(0).Border(wxALL, 2));

    nbHolder->SetSizer(nbSizer);

    wxButton* rightButton = new wxButton(rightPanel, BUTTON_NextChap, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
    rightButton->SetBitmap(wxBITMAP_PNG(arrowRight));

    m_rightSizer = new wxBoxSizer(wxVERTICAL);
    m_rightSizer->Add(sumPanel, wxSizerFlags(2).Expand().Border(wxALL, 8));
    m_rightSizer->AddStretchSpacer(1);
    m_rightSizer->Add(m_noteChecker, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 8));
    m_rightSizer->Add(m_noteLabel, wxSizerFlags(0).Expand().Border(wxTOP | wxRIGHT | wxLEFT, 8));
    m_rightSizer->Add(m_note, wxSizerFlags(2).Expand().Border(wxRIGHT | wxLEFT, 8));
    m_rightSizer->Add(nbHolder, wxSizerFlags(0).Expand().Border(wxBOTTOM | wxRIGHT | wxLEFT, 9));
    m_rightSizer->Add(rightButton, wxSizerFlags(0).Left().Border(wxLEFT | wxBOTTOM, 8));

    rightPanel->SetSizer(m_rightSizer);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(leftNotebook, wxSizerFlags(1).Expand());
    mainSizer->Add(m_cwNotebook, wxSizerFlags(4).Expand());
    mainSizer->Add(rightPanel, wxSizerFlags(1).Expand());

    SetSizer(mainSizer);

    SetSize(1100, 700);
    Maximize();

    m_charInChap->SetColumnWidth(0, m_leftPanel->GetSize().x);
    m_locInChap->SetColumnWidth(0, m_leftPanel->GetSize().x);

    wxMenuBar* menu = new wxMenuBar();
    wxMenu* menu1 = new wxMenu();
    menu->Append(menu1, "File");

    SetMenuBar(menu);

    m_statusBar = CreateStatusBar(3);
    m_statusBar->SetStatusText("Chapter up-to-date", 0);
    m_statusBar->SetStatusText("Number of words: 0", 1);
    m_statusBar->SetStatusText("Zoom: 100%", 2);
    m_statusBar->SetBackgroundColour(wxColour(100, 100, 100));

    m_cwNotebook->notesSize.x = (m_cwNotebook->corkBoard->GetSize().x / 3) - 30;
    m_cwNotebook->notesSize.y = (m_cwNotebook->corkBoard->GetSize().y / 4) - 10;

    m_cwNotebook->corkBoard->size = m_cwNotebook->corkBoard->GetSize();

    SetIcon(wxICON(amadeus));

    LoadChapter();

    Show();
    SetFocus();

    m_saveTimer.Start(10000);
}

void amChapterWriter::ClearNote(wxCommandEvent& event) {
    m_note->Clear();
    //m_noteLabel->SetValue("New note");

    event.Skip();
}

void amChapterWriter::AddNote(wxCommandEvent& event) {
    if (!m_note->IsEmpty()) {
        m_cwNotebook->AddNote((wxString)m_note->GetValue(), (wxString)m_noteLabel->GetValue(), m_doGreenNote);
        m_note->Clear();
        m_noteLabel->SetValue("New note");
        m_doGreenNote = false;

        CheckNotes();
    }

    event.Skip();
}

void amChapterWriter::CheckNotes() {
    if (m_cwNotebook->notes.size()) {
        if (m_cwNotebook->HasRedNote()) {
            m_noteChecker->SetBackgroundColour(wxColour(120, 0, 0));
            m_noteChecker->SetLabel("Unresolved notes!");
        } else {
            m_noteChecker->SetBackgroundColour(wxColour(0, 100, 0));
            m_noteChecker->SetLabel("All notes resolved!");
        }
    } else {
        m_noteChecker->SetBackgroundColour(wxColour(20, 20, 20));
        m_noteChecker->SetLabel("No notes yet.");
    }

    m_rightSizer->Layout();
}

void amChapterWriter::OnAddCharacter(wxCommandEvent& event) {
    wxPoint point(m_characterPanel->GetScreenPosition());

    int x = point.x;
    int y = point.y + m_charInChap->GetSize().y;
    
    wxPopupTransientWindow* win = new wxPopupTransientWindow(m_leftPanel, wxBORDER_RAISED | wxPU_CONTAINS_CONTROLS);

    wxListBox* list = new wxListBox();
    list->Create(win, LIST_AddChar, wxDefaultPosition, wxDefaultSize,
        m_manager->GetCharacterNames(), wxLB_NEEDED_SB | wxLB_SINGLE);
    list->Bind(wxEVT_LISTBOX_DCLICK, &amChapterWriter::AddCharacter, this);

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(list, wxSizerFlags(1).Expand());
    win->SetSizer(siz);

    win->SetPosition(wxPoint(x, y));
    win->SetSize(m_characterPanel->GetSize());

    win->Popup(list);
    event.Skip();
}

void amChapterWriter::OnAddLocation(wxCommandEvent& event) {
    wxPoint point(m_locationPanel->GetScreenPosition());

    int x = point.x;
    int y = point.y + m_locInChap->GetSize().y;

    wxPopupTransientWindow* win = new wxPopupTransientWindow(m_locationPanel, wxBORDER_RAISED | wxPU_CONTAINS_CONTROLS);

    wxListBox* list = new wxListBox(win, -1, wxDefaultPosition, wxDefaultSize,
        m_manager->GetLocationNames(), wxLB_NEEDED_SB | wxLB_SINGLE);
    list->Bind(wxEVT_LISTBOX_DCLICK, &amChapterWriter::AddLocation, this);

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(list, 1, wxGROW);
    win->SetSizer(siz);

    win->SetPosition(wxPoint(x, y));
    win->SetSize(m_locationPanel->GetSize());

    win->Popup();
    event.Skip();
}

void amChapterWriter::OnAddItem(wxCommandEvent& event) {
    wxPoint point(m_itemPanel->GetScreenPosition());

    int x = point.x;
    int y = point.y + m_itemsInChap->GetSize().y;

    wxPopupTransientWindow* win = new wxPopupTransientWindow(m_leftPanel, wxBORDER_RAISED | wxPU_CONTAINS_CONTROLS);

    wxListBox* list = new wxListBox();
    list->Create(win, LIST_AddItem, wxDefaultPosition, wxDefaultSize,
        m_manager->GetItemNames(), wxLB_NEEDED_SB | wxLB_SINGLE);
    list->Bind(wxEVT_LISTBOX_DCLICK, &amChapterWriter::AddItem, this);

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(list, wxSizerFlags(1).Expand());
    win->SetSizer(siz);

    win->SetPosition(wxPoint(x, y));
    win->SetSize(m_itemPanel->GetSize());

    win->Popup(list);
    event.Skip();
}

void amChapterWriter::AddCharacter(wxCommandEvent& event) {
    wxString name = event.GetString();
    if (m_charInChap->FindItem(-1, name) == -1) {
        CheckChapterValidity();
        m_manager->AddChapterToCharacter(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);
        UpdateCharacterList();
        m_manager->GetElementsNotebook()->UpdateCharacterList();
    }
}

void amChapterWriter::AddLocation(wxCommandEvent& event) {
    wxString name = event.GetString();
   
    if (m_locInChap->FindItem(-1, name) == -1) {
        CheckChapterValidity();
        m_manager->AddChapterToLocation(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);
        UpdateLocationList();
        m_manager->GetElementsNotebook()->UpdateLocationList();
    }
}

void amChapterWriter::AddItem(wxCommandEvent& event) {
    wxString name = event.GetString();

    if (m_itemsInChap->FindItem(-1, name) == -1) {
        CheckChapterValidity();
        m_manager->AddChapterToItem(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);
        UpdateItemList();
        m_manager->GetElementsNotebook()->UpdateItemList();
    }
}

void amChapterWriter::UpdateCharacterList() {
    m_charInChap->DeleteAllItems();
    
    int i = 0;
    for (auto& it : m_manager->GetChapters(1, 1)[m_chapterPos - 1].characters) {
        m_charInChap->InsertItem(i++, it);
    }
}

void amChapterWriter::UpdateLocationList() {
    m_locInChap->DeleteAllItems();

    int i = 0;
    for (auto& it : m_manager->GetChapters(1, 1)[m_chapterPos - 1].locations) {
        m_locInChap->InsertItem(i++, it);
    }
}

void amChapterWriter::UpdateItemList() {
    m_itemsInChap->DeleteAllItems();

    int i = 0;
    for (auto& it : m_manager->GetChapters(1, 1)[m_chapterPos - 1].items) {
        m_itemsInChap->InsertItem(i++, it);
    }
}

void amChapterWriter::OnRemoveCharacter(wxCommandEvent& event) {
    int sel = m_charInChap->GetFirstSelected();
    int nos = m_charInChap->GetSelectedItemCount();

    CheckChapterValidity();

    for (int i = 0; i < nos; i++) {
        wxString name = m_charInChap->GetItemText(sel);
        m_charInChap->DeleteItem(sel);

        m_manager->RemoveChapterFromCharacter(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);

        sel = m_charInChap->GetNextSelected(sel - 1);
    }

    UpdateCharacterList();
    m_manager->GetElementsNotebook()->UpdateCharacterList();
    event.Skip();
}

void amChapterWriter::OnRemoveLocation(wxCommandEvent& event) {
    int sel = m_locInChap->GetFirstSelected();
    int nos = m_locInChap->GetSelectedItemCount();

    CheckChapterValidity();

    for (int i = 0; i < nos; i++) {
        wxString name = m_locInChap->GetItemText(sel);
        m_locInChap->DeleteItem(sel);

        m_manager->RemoveChapterFromLocation(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);

        sel = m_locInChap->GetNextSelected(sel + 1);
    }

    m_manager->GetElementsNotebook()->UpdateLocationList();
    event.Skip();
}

void amChapterWriter::OnRemoveItem(wxCommandEvent& event) {
    int sel = m_itemsInChap->GetFirstSelected();
    int nos = m_itemsInChap->GetSelectedItemCount();

    CheckChapterValidity();

    for (int i = 0; i < nos; i++) {
        wxString name = m_itemsInChap->GetItemText(sel);
        m_itemsInChap->DeleteItem(sel);

        m_manager->RemoveChapterFromItem(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);

        sel = m_itemsInChap->GetNextSelected(sel + 1);
    }

    m_manager->GetElementsNotebook()->UpdateItemList();
    event.Skip();
}

void amChapterWriter::OnNextChapter(wxCommandEvent& event) {
    if (m_chapterPos < m_manager->GetChapterCount(1)) {
        SaveChapter();
        m_chapterPos++;
        m_cwNotebook->m_textCtrl->EndAllStyles();

        m_cwNotebook->notes.clear();
        if (m_cwNotebook->notesSizer->GetItemCount() > 0)
            m_cwNotebook->notesSizer->Clear(true);

        LoadChapter();

        m_cwNotebook->corkBoard->FitInside();
    }
    event.Skip();
}

void amChapterWriter::OnPreviousChapter(wxCommandEvent& event) {
    if (m_chapterPos > 1) {
        SaveChapter();
        m_chapterPos--;
        m_cwNotebook->notes.clear();

        if (m_cwNotebook->notesSizer->GetItemCount() > 0) {
            m_cwNotebook->notesSizer->Clear(true);
        }

        LoadChapter();

        m_cwNotebook->corkBoard->FitInside();
    }
    event.Skip();
}

void amChapterWriter::CheckChapterValidity() {
    // For now, if the store chapter position is bigger than the chapter count,
    // it simply makes it so the current chapter is the last one. I'll make it something
    // else later.
    while (m_chapterPos > m_manager->GetChapterCount(1))
        m_chapterPos--;
}

void amChapterWriter::LoadChapter() {
    wxBusyCursor busy;

    Chapter& chapter = m_manager->GetChapters(1, 1)[m_chapterPos - 1];

    SetTitle(chapter.name);

    m_cwNotebook->Freeze();
    
    chapter.scenes[0].content.SetBasicStyle(m_cwNotebook->m_textCtrl->GetBasicStyle());
    
    m_cwNotebook->m_textCtrl->GetBuffer() = chapter.scenes[0].content;
    m_cwNotebook->m_textCtrl->GetBuffer().Invalidate(wxRICHTEXT_ALL);
    m_cwNotebook->m_textCtrl->RecreateBuffer();
    m_cwNotebook->m_textCtrl->Refresh();

    m_summary->SetValue(chapter.synopsys);

    for (auto& noteIt : chapter.notes) {
        m_note->SetValue(noteIt.content);
        m_noteLabel->SetValue(noteIt.name);
        m_doGreenNote = noteIt.isDone;

        AddNote(wxCommandEvent());
    }

    m_cwNotebook->corkBoard->Layout();
    m_cwNotebook->Thaw();

    UpdateCharacterList();
    UpdateLocationList();
    UpdateItemList();

    CheckNotes();
    CountWords();
    m_statusBar->SetStatusText("Chapter up-to-date", 0);
}

void amChapterWriter::SaveChapter() {
    CheckChapterValidity();
    Chapter& chapter = m_manager->GetChapters(1, 1)[m_chapterPos - 1];

    chapter.scenes[0].content = m_cwNotebook->m_textCtrl->GetBuffer();
    chapter.scenes[0].content.SetBasicStyle(m_cwNotebook->m_textCtrl->GetBasicStyle());
    chapter.synopsys = (wxString)m_summary->GetValue();

    chapter.notes = m_cwNotebook->notes;

    chapter.Update(m_manager->GetStorage(), true, true);

    UpdateCharacterList();
    UpdateLocationList();
    UpdateItemList();
}

void amChapterWriter::CountWords() {
    wxString count = m_cwNotebook->m_textCtrl->GetValue();

    if (count != "") {
        int words = std::count(count.begin(), count.end(), ' ');
        m_statusBar->SetStatusText("Number of words: " + std::to_string(words + 1), 1);
    } else {
        m_statusBar->SetStatusText("Number of words: 0", 1);
    }
}

void amChapterWriter::OnTimerEvent(wxTimerEvent& event) {
    m_statusBar->SetStatusText("Autosaving chapter...", 0);

    SaveChapter();
    CountWords();

    if (IsShown())
        m_manager->GetMainFrame()->Show();

    m_statusBar->SetStatusText("Chapter up-to-date", 0);

    event.Skip();
}

void amChapterWriter::OnClose(wxCloseEvent& event) {
    SaveChapter();

    Destroy();
    event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// amChapterWriterNotebook //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(amChapterWriterNotebook, wxAuiNotebook)

EVT_TOOL(TOOL_Bold, amChapterWriterNotebook::OnBold)
EVT_TOOL(TOOL_Italic, amChapterWriterNotebook::OnItalic)
EVT_TOOL(TOOL_Underline, amChapterWriterNotebook::OnUnderline)
EVT_TOOL(TOOL_AlignLeft, amChapterWriterNotebook::OnAlignLeft)
EVT_TOOL(TOOL_AlignCenter, amChapterWriterNotebook::OnAlignCenter)
EVT_TOOL(TOOL_AlignCenterJust, amChapterWriterNotebook::OnAlignCenterJust)
EVT_TOOL(TOOL_AlignRight, amChapterWriterNotebook::OnAlignRight)

EVT_SLIDER(TOOL_ContentScale, amChapterWriterNotebook::OnZoom)
EVT_TOOL(TOOL_ChapterFullScreen, amChapterWriterNotebook::OnFullScreen)
EVT_TOOL(TOOL_PageView, amChapterWriterNotebook::OnPageView)

EVT_UPDATE_UI(TOOL_Bold, amChapterWriterNotebook::OnUpdateBold)
EVT_UPDATE_UI(TOOL_Italic, amChapterWriterNotebook::OnUpdateItalic)
EVT_UPDATE_UI(TOOL_Underline, amChapterWriterNotebook::OnUpdateUnderline)
EVT_UPDATE_UI(TOOL_AlignLeft, amChapterWriterNotebook::OnUpdateAlignLeft)
EVT_UPDATE_UI(TOOL_AlignCenter, amChapterWriterNotebook::OnUpdateAlignCenter)
EVT_UPDATE_UI(TOOL_AlignCenterJust, amChapterWriterNotebook::OnUpdateAlignCenterJust)
EVT_UPDATE_UI(TOOL_AlignRight, amChapterWriterNotebook::OnUpdateAlignRight)
EVT_UPDATE_UI(TOOL_FontSize, amChapterWriterNotebook::OnUpdateFontSize)

EVT_COMBOBOX(TOOL_FontSize, amChapterWriterNotebook::OnFontSize)

EVT_TEXT(TEXT_Content, amChapterWriterNotebook::OnText)
EVT_RICHTEXT_CONSUMING_CHARACTER(TEXT_Content, amChapterWriterNotebook::OnKeyDown)

END_EVENT_TABLE()

amChapterWriterNotebook::amChapterWriterNotebook(wxWindow* parent) :
    wxAuiNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT |
    wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxBORDER_NONE) {

    this->parent = reinterpret_cast<amChapterWriter*>(parent);

    wxPanel* mainPanel = new wxPanel(this, 1, wxDefaultPosition, wxDefaultSize);
    mainPanel->SetBackgroundColour(wxColour(100, 100, 100));

    m_textCtrl = new wxRichTextCtrl(mainPanel, TEXT_Content, "", wxDefaultPosition, wxDefaultSize,
        wxRE_MULTILINE | wxBORDER_NONE);
    wxRichTextBuffer::AddHandler(new wxRichTextPlainTextHandler);

    wxRichTextAttr attr;
    attr.SetFont(wxFontInfo(10));
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    attr.SetLeftIndent(63, -63);
    attr.SetTextColour(wxColour(250, 250, 250));
    m_textCtrl->SetBasicStyle(attr);
    m_textCtrl->SetBackgroundColour(wxColour(35, 35, 35));

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

    contentScale = new wxSlider(contentTool, TOOL_ContentScale, 100, 50, 300,
        wxDefaultPosition, wxDefaultSize, wxSL_MIN_MAX_LABELS);

    contentTool->AddControl(fontSize);
    contentTool->AddStretchableSpace();
    contentTool->AddControl(contentScale);
    contentTool->AddSeparator();
    contentTool->AddCheckTool(TOOL_ChapterFullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Toggle Full Screen");
    //contentTool->AddCheckTool()

    contentTool->Realize();

    wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(contentTool, wxSizerFlags(0).Expand().Border(wxBOTTOM, 3));
    pageSizer->Add(m_textCtrl, wxSizerFlags(1).Expand());

    mainPanel->SetSizer(pageSizer);

    corkBoard = new ImagePanel(this, wxDefaultPosition, wxDefaultSize);
    corkBoard->SetBackgroundColour(wxColour(45, 45, 45));
    corkBoard->EnableScrolling(false, true);
    corkBoard->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

    notesSizer = new wxWrapSizer();
    corkBoard->SetSizer(notesSizer);
    corkBoard->SetScrollRate(15, 15);

    AddPage(mainPanel, "Main");
    AddPage(corkBoard, "Notes");

    wxTimer m_timer(this, 12345);
    m_timer.Start(10000);
}

void amChapterWriterNotebook::OnText(wxCommandEvent& WXUNUSED(event)) {
    parent->m_statusBar->SetStatusText("Chapter modified. Autosaving soon...", 0);
}

void amChapterWriterNotebook::OnKeyDown(wxRichTextEvent& event) {
    if (wxGetKeyState(WXK_CONTROL)) {
        switch (event.GetCharacter()) {
        case 'n':
        case 'N':
        case 'b':
        case 'B':
            OnBold(event);
            break;

        case 'i':
        case 'I':
            OnItalic(event);
            break;

        case 's':
        case 'S':
        case 'u':
        case 'U':
            OnUnderline(event);
            break;
        }
    }
}

void amChapterWriterNotebook::OnBold(wxCommandEvent& event) {
    m_textCtrl->ApplyBoldToSelection();
    OnText(event);
}

void amChapterWriterNotebook::OnItalic(wxCommandEvent& event) {
    m_textCtrl->ApplyItalicToSelection();
    OnText(event);
}

void amChapterWriterNotebook::OnUnderline(wxCommandEvent& event) {
    m_textCtrl->ApplyUnderlineToSelection();
    OnText(event);
}

void amChapterWriterNotebook::OnAlignLeft(wxCommandEvent& event) {
    m_textCtrl->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_LEFT));
    OnText(event);
}

void amChapterWriterNotebook::OnAlignCenter(wxCommandEvent& event) {
    m_textCtrl->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_CENTER));
    OnText(event);
}

void amChapterWriterNotebook::OnAlignCenterJust(wxCommandEvent& event) {
    m_textCtrl->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_JUSTIFIED));
    OnText(event);
}

void amChapterWriterNotebook::OnAlignRight(wxCommandEvent& event) {
    m_textCtrl->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_RIGHT));
    OnText(event);
}

void amChapterWriterNotebook::OnZoom(wxCommandEvent& event) {
    int zoom = event.GetInt();

    m_textCtrl->SetScale((double)zoom / 100.0, true);
    parent->m_statusBar->SetStatusText(wxString("Zoom: ") << zoom << "%", 2);
}

void amChapterWriterNotebook::OnFullScreen(wxCommandEvent& WXUNUSED(event)) {
    parent->ToggleFullScreen();
}

void amChapterWriterNotebook::OnPageView(wxCommandEvent& WXUNUSED(event)) {}

void amChapterWriterNotebook::OnUpdateBold(wxUpdateUIEvent& event) {
    event.Check(m_textCtrl->IsSelectionBold());
}

void amChapterWriterNotebook::OnUpdateItalic(wxUpdateUIEvent& event) {
    event.Check(m_textCtrl->IsSelectionItalics());
}

void amChapterWriterNotebook::OnUpdateUnderline(wxUpdateUIEvent& event) {
    event.Check(m_textCtrl->IsSelectionUnderlined());
}

void amChapterWriterNotebook::OnUpdateAlignLeft(wxUpdateUIEvent& event) {
    event.Check(m_textCtrl->IsSelectionAligned(wxTEXT_ALIGNMENT_LEFT));
}

void amChapterWriterNotebook::OnUpdateAlignCenter(wxUpdateUIEvent& event) {
    event.Check(m_textCtrl->IsSelectionAligned(wxTEXT_ALIGNMENT_CENTER));
}

void amChapterWriterNotebook::OnUpdateAlignCenterJust(wxUpdateUIEvent& event) {
    event.Check(m_textCtrl->IsSelectionAligned(wxTEXT_ALIGNMENT_JUSTIFIED));
}

void amChapterWriterNotebook::OnUpdateAlignRight(wxUpdateUIEvent& event) {
    event.Check(m_textCtrl->IsSelectionAligned(wxTEXT_ALIGNMENT_RIGHT));
}

void amChapterWriterNotebook::OnUpdateFontSize(wxUpdateUIEvent& WXUNUSED(event)) {
    wxRichTextAttr attr;
    m_textCtrl->GetStyle(m_textCtrl->GetInsertionPoint() - 1, attr);
    wxString size(std::to_string(attr.GetFontSize()));

    if (fontSize->GetValue() != size)
        fontSize->SetValue(size);
}

void amChapterWriterNotebook::OnFontSize(wxCommandEvent& event) {
    m_textCtrl->SetFocus();
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_SIZE);
    long size;
    event.GetString().ToLong(&size);
    attr.SetFontSize(size);

    if (m_textCtrl->HasSelection()) {
        m_textCtrl->SetStyleEx(m_textCtrl->GetSelectionRange(), attr,
            wxRICHTEXT_SETSTYLE_WITH_UNDO | wxRICHTEXT_SETSTYLE_OPTIMIZE | wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
    } else {
        wxRichTextAttr current = m_textCtrl->GetDefaultStyle();
        current.Apply(attr);
        m_textCtrl->SetDefaultStyle(current);
    }
}

bool amChapterWriterNotebook::HasRedNote() {
    for (unsigned int i = 0; i < notes.size(); i++) {
        if (notes[i].isDone == false)
            return true;
    }

    return false;
}

void amChapterWriterNotebook::AddNote(wxString& noteContent, wxString& noteName, bool isDone) {

    wxPanel* notePanel = new wxPanel(corkBoard);

    wxTextCtrl* noteLabel = new wxTextCtrl(notePanel, -1, noteName);
    noteLabel->SetValue(noteName);
    noteLabel->SetBackgroundColour(wxColour(240, 240, 240));
    wxPanel* options = new wxPanel(notePanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    options->SetBackgroundColour(wxColour(200, 200, 200));
    wxTextCtrl* note = new wxTextCtrl(notePanel, -1, noteContent, wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE);
    note->SetBackgroundColour(wxColour(255, 250, 205));

    noteLabel->Bind(wxEVT_TEXT, &amChapterWriterNotebook::UpdateNoteLabel, this);
    note->Bind(wxEVT_TEXT, &amChapterWriterNotebook::UpdateNote, this);

    options->Bind(wxEVT_RIGHT_DOWN, &amChapterWriterNotebook::OnNoteClick, this);
    options->Bind(wxEVT_LEFT_DOWN, &amChapterWriterNotebook::OnNoteClick, this);
    options->Bind(wxEVT_PAINT, &amChapterWriterNotebook::PaintDots, this);

    wxRadioButton* b1 = new wxRadioButton(notePanel, -1, "", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    wxRadioButton* b2 = new wxRadioButton(notePanel, -1, "", wxDefaultPosition, wxDefaultSize);

    b1->Bind(wxEVT_RADIOBUTTON, &amChapterWriterNotebook::SetRed, this);
    b2->Bind(wxEVT_RADIOBUTTON, &amChapterWriterNotebook::SetGreen, this);

    b1->SetBackgroundColour(wxColour(120, 0, 0));
    b2->SetBackgroundColour(wxColour(0, 140, 0));

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
        notePanel->SetBackgroundColour(wxColour(0, 140, 0));
        b2->SetValue(true);
    } else {
        notePanel->SetBackgroundColour(wxColour(120, 0, 0));
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

void amChapterWriterNotebook::PaintDots(wxPaintEvent& event) {
    wxPanel* pan = dynamic_cast<wxPanel*>(event.GetEventObject());
    wxPaintDC dc(pan);

    wxSize opSize(pan->GetSize());

    dc.SetBrush(wxBrush(wxColour(140, 140, 140)));
    dc.SetPen(wxPen(wxColour(140, 140, 140)));

    dc.DrawCircle(wxPoint(opSize.x / 5 - 1, opSize.y / 2), 2);
    dc.DrawCircle(wxPoint(opSize.x / 2 - 1, opSize.y / 2), 2);
    dc.DrawCircle(wxPoint(((opSize.x / 5) * 4) - 1, opSize.y / 2), 2);
}

void amChapterWriterNotebook::SetRed(wxCommandEvent& event) {
    wxRadioButton* rb = dynamic_cast<wxRadioButton*>(event.GetEventObject());
    wxPanel* np = dynamic_cast<wxPanel*>(rb->GetParent());

    wxPanel* sp;
    int i = 0;
    for (auto it = notes.begin(); it != notes.end(); it++) {
        sp = dynamic_cast<wxPanel*>(notesSizer->GetItem(i)->GetWindow());

        if (sp == np) {
            it->isDone = false;
            np->SetBackgroundColour(wxColour(120, 0, 0));
            np->Refresh();
            break;
        }

        i++;
    }

    parent->CheckNotes();
}

void amChapterWriterNotebook::SetGreen(wxCommandEvent& event) {
    wxRadioButton* gb = dynamic_cast<wxRadioButton*>(event.GetEventObject());
    wxPanel* np = dynamic_cast<wxPanel*>(gb->GetParent());

    wxPanel* sp;
    int i = 0;
    for (auto it = notes.begin(); it != notes.end(); it++) {
        sp = dynamic_cast<wxPanel*>(notesSizer->GetItem(i)->GetWindow());

        if (sp == np) {
            it->isDone = true;
            np->SetBackgroundColour(wxColour(0, 140, 0));
            np->Refresh();
            break;
        }

        i++;
    }

    parent->CheckNotes();
}

void amChapterWriterNotebook::OnDeleteNote(wxCommandEvent& WXUNUSED(event)) {
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

    parent->CheckNotes();
}

void amChapterWriterNotebook::OnNoteClick(wxMouseEvent& event) {
    wxPanel* pan = dynamic_cast<wxPanel*>(event.GetEventObject());

    selNote = dynamic_cast<wxPanel*>(pan->GetParent());

    wxMenu menu;
    menu.Append(MENU_Delete, "Delete");
    menu.Bind(wxEVT_MENU, &amChapterWriterNotebook::OnDeleteNote, this, MENU_Delete);
    pan->PopupMenu(&menu, wxPoint(-1, pan->GetSize().y));
}

void amChapterWriterNotebook::UpdateNoteLabel(wxCommandEvent& event) {
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

void amChapterWriterNotebook::UpdateNote(wxCommandEvent& event) {
    wxTextCtrl* trtc = (wxTextCtrl*)event.GetEventObject();
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