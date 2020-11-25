#ifndef CHAPTERWRITER_H_
#define CHAPTERWRITER_H_
#pragma once

#include "wx/frame.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/panel.h"
#include "wx/listctrl.h"
#include "wx/textctrl.h"

#include <string>
#include <list>

#include "ImagePanel.h"
#include "ChaptersNotebook.h"

using std::string;
using std::list;

struct ChapterWriterNotebook;

class ChapterWriter : wxFrame {
private:
    MainFrame* mainFrame = nullptr;
    Chapter* thisChap = nullptr;

    ChapterWriterNotebook* chapWriterNotebook = nullptr;

    wxTextCtrl* summary = nullptr;
    wxTextCtrl* note = nullptr;
    wxTextCtrl* noteLabel = nullptr;

    wxPanel* leftPanel = nullptr;
    wxPanel* locPanel = nullptr;
    wxPanel* charPanel = nullptr;

    wxBoxSizer* leftSizer = nullptr;

    wxListView* charInChap = nullptr;
    wxListView* locInChap = nullptr;
    wxArrayString charNames{ true };
    wxArrayString locNames{ true };

    wxBoxSizer* pageSizer = nullptr;

    wxButton* noteClear = nullptr;
    wxButton* noteAdd = nullptr;

    ChaptersNotebook* chapNote = nullptr;
    unsigned int chapterPos;

    wxTimer saveTimer;
    wxTimer wordsTimer;

    bool green = false;

public:
    wxStatusBar* statusBar = nullptr;

public:
    ChapterWriter(wxWindow* parent, ChaptersNotebook* notebook, int numb);

    void clearNote(wxCommandEvent& event);
    void addNote(wxCommandEvent& event);

    void test(wxMouseEvent& pos);

    void addCharButtonPressed(wxCommandEvent& event);
    void addLocButtonPressed(wxCommandEvent& event);
    void removeChar(wxCommandEvent& event);
    void removeLocButtonPressed(wxCommandEvent& event);

    void addChar(wxCommandEvent& event);
    void addLoc(wxCommandEvent& event);

    void nextChap(wxCommandEvent& event);
    void prevChap(wxCommandEvent& event);

    void loadChapter();
    void saveChapter();

    void countWords();

    void timerEvent(wxTimerEvent& event);

    void toggleFullScreen() { ShowFullScreen(!IsFullScreen()); }
    void onClose(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    BUTTON_AddChar,
    BUTTON_AddLoc,
    BUTTON_RemChar,
    BUTTON_RemLoc,

    LIST_AddChar,
    LIST_AddLoc,

    BUTTON_NoteAdd,
    BUTTON_NoteClear,

    BUTTON_PreviousChap,
    BUTTON_NextChap,

    TIMER_Save,
    TIMER_Words
};

#endif