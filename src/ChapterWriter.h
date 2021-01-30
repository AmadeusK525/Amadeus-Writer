#ifndef CHAPTERWRITER_H_
#define CHAPTERWRITER_H_
#pragma once

#include <wx\wx.h>
#include <wx\richtext\richtextctrl.h>
#include <wx\aui\aui.h>
#include <wx\wrapsizer.h>


#include <list>

#include "ProjectManager.h"
#include "ImagePanel.h"
#include "ChaptersNotebook.h"
#include "Note.h"


using std::list;

struct amdChapterWriterNotebook;

class amdChapterWriter : public wxFrame {
private:
    amdProjectManager* m_manager = nullptr;
    amdChapterWriterNotebook* m_cwNotebook = nullptr;

    wxTextCtrl* m_summary = nullptr,
        * m_note = nullptr,
        * m_noteLabel = nullptr;

    wxStaticText* m_noteChecker = nullptr;

    wxPanel* m_leftPanel = nullptr,
        * m_characterPanel = nullptr,
        * m_locationPanel = nullptr,
        * m_itemPanel = nullptr;

    wxBoxSizer* m_leftSizer = nullptr,
        * m_rightSizer = nullptr;

    wxListView* m_charInChap = nullptr,
        * m_locInChap = nullptr,
        * m_itemsInChap = nullptr;

    wxBoxSizer* m_pageSizer = nullptr;

    wxButton* m_noteClear = nullptr,
        * m_noteAdd = nullptr;

    unsigned int m_chapterPos;

    wxTimer m_saveTimer;

    bool m_doGreenNote = false;

public:
    wxStatusBar* m_statusBar = nullptr;

public:
    amdChapterWriter(wxWindow* parent, amdProjectManager* manager, int numb);

    void ClearNote(wxCommandEvent& event);
    void AddNote(wxCommandEvent& event);

    void CheckNotes();

    void OnAddCharacter(wxCommandEvent& event);
    void OnAddLocation(wxCommandEvent& event);
    void OnAddItem(wxCommandEvent& event);
    void OnRemoveCharacter(wxCommandEvent& event);
    void OnRemoveLocation(wxCommandEvent& event);
    void OnRemoveItem(wxCommandEvent& event);

    void AddCharacter(wxCommandEvent& event);
    void AddLocation(wxCommandEvent& event);
    void AddItem(wxCommandEvent& event);

    void UpdateCharacterList();
    void UpdateLocationList();
    void UpdateItemList();

    void OnNextChapter(wxCommandEvent& event);
    void OnPreviousChapter(wxCommandEvent& event);
    void CheckChapterValidity();

    void LoadChapter();
    void SaveChapter();

    void CountWords();

    void OnTimerEvent(wxTimerEvent& event);

    void ToggleFullScreen() { ShowFullScreen(!IsFullScreen()); }
    void OnClose(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    BUTTON_AddChar,
    BUTTON_AddLoc,
    BUTTON_AddItem,
    BUTTON_RemChar,
    BUTTON_RemLoc,
    BUTTON_RemItem,

    LIST_AddChar,
    LIST_AddLoc,
    LIST_AddItem,

    BUTTON_NoteAdd,
    BUTTON_NoteClear,

    BUTTON_PreviousChap,
    BUTTON_NextChap,

    TIMER_Save,
    TIMER_Words
};


struct amdChapterWriterNotebook : public wxAuiNotebook {
    amdChapterWriter* parent = nullptr;

    wxToolBar* contentTool = nullptr;
    wxComboBox* fontSize = nullptr;
    wxSlider* contentScale = nullptr;

    wxRichTextCtrl* m_textCtrl = nullptr;
    wxRichTextStyleSheet* styleSheet = nullptr;
    wxVector<Note> notes;
    ImagePanel* corkBoard = nullptr;

    wxWrapSizer* notesSizer = nullptr;
    wxPanel* selNote = nullptr;
    wxSize notesSize{};

    amdChapterWriterNotebook::amdChapterWriterNotebook(wxWindow* parent);

    void OnText(wxCommandEvent& event);
    void OnKeyDown(wxRichTextEvent& event);

    void OnBold(wxCommandEvent& event);
    void OnItalic(wxCommandEvent& event);
    void OnUnderline(wxCommandEvent& event);
    void OnAlignLeft(wxCommandEvent& event);
    void OnAlignCenter(wxCommandEvent& event);
    void OnAlignCenterJust(wxCommandEvent& event);
    void OnAlignRight(wxCommandEvent& event);

    void OnZoom(wxCommandEvent& event);

    void OnFullScreen(wxCommandEvent& event);
    void OnPageView(wxCommandEvent& event);

    void OnUpdateBold(wxUpdateUIEvent& event);
    void OnUpdateItalic(wxUpdateUIEvent& event);
    void OnUpdateUnderline(wxUpdateUIEvent& event);
    void OnUpdateAlignLeft(wxUpdateUIEvent& event);
    void OnUpdateAlignCenter(wxUpdateUIEvent& event);
    void OnUpdateAlignCenterJust(wxUpdateUIEvent& event);
    void OnUpdateAlignRight(wxUpdateUIEvent& event);
    void OnUpdateFontSize(wxUpdateUIEvent& event);

    void OnFontSize(wxCommandEvent& event);

    bool HasRedNote();

    void AddNote(wxString& note, wxString& noteName, bool isDone);
    void PaintDots(wxPaintEvent& event);
    void SetRed(wxCommandEvent& event);
    void SetGreen(wxCommandEvent& event);
    void OnDeleteNote(wxCommandEvent& event);

    void OnNoteClick(wxMouseEvent& event);

    void UpdateNoteLabel(wxCommandEvent& event);
    void UpdateNote(wxCommandEvent& event);

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
    TOOL_ContentScale,
    TOOL_ChapterFullScreen,
    TOOL_PageView,

    MENU_Delete,
    TEXT_Content
};


#endif