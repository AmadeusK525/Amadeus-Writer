#pragma once

#ifndef CHAPTERWRITERNOTEBOOK_H_
#define CHAPTERWRITERNOTEBOOK_H_

#include "wx/aui/aui.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include "wx/toolbar.h"
#include "wx/wrapsizer.h"

#include <string>
#include <vector>

#include "ImagePanel.h"
#include "Note.h"

class ChapterWriter;

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