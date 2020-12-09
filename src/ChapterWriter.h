#ifndef CHAPTERWRITER_H_
#define CHAPTERWRITER_H_
#pragma once

#include <wx\frame.h>
#include <wx\wrapsizer.h>
#include <wx\button.h>
#include <wx\panel.h>
#include <wx\listctrl.h>
#include <wx\textctrl.h>
#include <wx\aui\aui.h>

#include <string>
#include <list>

#include "ImagePanel.h"
#include "ChaptersNotebook.h"
#include "Note.h"

using std::string;
using std::list;

struct ChapterWriterNotebook;

class ChapterWriter : public wxFrame {
private:
    MainFrame* mainFrame = nullptr;
    Chapter* thisChap = nullptr;

    ChapterWriterNotebook* chapWriterNotebook = nullptr;

    wxTextCtrl* summary = nullptr;
    wxTextCtrl* note = nullptr;
    wxTextCtrl* noteLabel = nullptr;

    wxStaticText* noteCheck = nullptr;

    wxPanel* leftPanel = nullptr;
    wxPanel* locPanel = nullptr;
    wxPanel* charPanel = nullptr;

    wxBoxSizer* leftSizer = nullptr;
    wxBoxSizer* rightSizer = nullptr;

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

    void checkNotes();

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


struct ChapterWriterNotebook : public wxAuiNotebook {
    ChapterWriter* parent = nullptr;

    wxToolBar* contentTool = nullptr;
    wxComboBox* fontSize = nullptr;

    wxRichTextCtrl* content = nullptr;
    wxRichTextStyleSheet* styleSheet = nullptr;
    std::vector<Note> notes;
    ImagePanel* corkBoard = nullptr;

    wxWrapSizer* notesSizer = nullptr;
    wxPanel* selNote = nullptr;
    wxSize notesSize{};

    ChapterWriterNotebook::ChapterWriterNotebook(wxWindow* parent);

    void setModified(wxCommandEvent& event);
    void onKeyDown(wxRichTextEvent& event);

    void setBold(wxCommandEvent& event);
    void setItalic(wxCommandEvent& event);
    void setUnderlined(wxCommandEvent& event);
    void setAlignLeft(wxCommandEvent& event);
    void setAlignCenter(wxCommandEvent& event);
    void setAlignCenterJust(wxCommandEvent& event);
    void setAlignRight(wxCommandEvent& event);

    void onFullScreen(wxCommandEvent& event);
    void onPageView(wxCommandEvent& event);

    void onUpdateBold(wxUpdateUIEvent& event);
    void onUpdateItalic(wxUpdateUIEvent& event);
    void onUpdateUnderline(wxUpdateUIEvent& event);
    void onUpdateAlignLeft(wxUpdateUIEvent& event);
    void onUpdateAlignCenter(wxUpdateUIEvent& event);
    void onUpdateAlignCenterJust(wxUpdateUIEvent& event);
    void onUpdateAlignRight(wxUpdateUIEvent& event);
    void onUpdateFontSize(wxUpdateUIEvent& event);

    void setFontSize(wxCommandEvent& event);

    bool hasRedNote();

    void addNote(std::string& note, std::string& noteName, bool isDone);
    void paintDots(wxPaintEvent& event);
    void setRed(wxCommandEvent& event);
    void setGreen(wxCommandEvent& event);
    void deleteNote(wxCommandEvent& event);

    void onNoteClick(wxMouseEvent& event);

    void updateNoteLabel(wxCommandEvent& event);
    void updateNote(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    TOOL_Bold,
    TOOL_Italic,
    TOOL_Underline,
    TOOL_AlignLeft,
    TOOL_AlignRight,
    TOOL_AlignCenter,
    TOOL_AlignCenterJust,
    TOOL_FontSize,
    TOOL_ChapterFullScreen,
    TOOL_PageView,

    MENU_Delete,
    TEXT_Content
};


#endif