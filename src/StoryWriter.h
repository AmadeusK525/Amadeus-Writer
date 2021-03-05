#ifndef CHAPTERWRITER_H_
#define CHAPTERWRITER_H_
#pragma once

#include <wx\wx.h>
#include <wx\richtext\richtextctrl.h>
#include <wx\dataview.h>
#include <wx\aui\aui.h>
#include <wx\wrapsizer.h>
#include <wx\scrolwin.h>

#include "ProjectManager.h"
#include "StoryNotebook.h"
#include "OutlineFiles.h"
#include "Note.h"

enum STMD_Type {
    STMD_Section,
    STMD_Chapter,
    STMD_Scene,

    STMD_Null
};

class StoryTreeModelNode;
WX_DEFINE_ARRAY_PTR(StoryTreeModelNode*, StoryTreeModelNodePtrArray);

class StoryTreeModelNode {
private:
    StoryTreeModelNode* m_parent = nullptr;
    StoryTreeModelNodePtrArray m_children{};

    wxDataViewItemAttr m_attr{};

    STMD_Type m_type = STMD_Null;
    int m_pos = -1;

    wxString m_title{};
    bool m_isContainer = true;

public:
    StoryTreeModelNode(StoryTreeModelNode* parent,
        const wxString& title, int pos, STMD_Type type) {
        m_parent = parent;
        m_title = title;
        m_pos = pos;
        m_type = type;

        if (m_parent)
            m_parent->Append(this);

        void* pp = this;
        StoryTreeModelNode* pp2 = static_cast<StoryTreeModelNode*>(pp);

        m_isContainer = false;

        m_attr.SetBackgroundColour(wxColour(250, 250, 250));
        m_attr.SetColour(wxColour(20, 20, 20));
        m_attr.SetBold(false);
        m_attr.SetItalic(false);
        m_attr.SetStrikethrough(false);
    }

    StoryTreeModelNode(StoryTreeModelNode* parent,
        const wxString& branch) {
        m_parent = parent;
        m_title = branch;

        if (m_parent)
            m_parent->Append(this);

        m_isContainer = true;

        m_attr.SetBackgroundColour(wxColour(250, 250, 250));
        m_attr.SetColour(wxColour(20, 20, 20));
        m_attr.SetBold(false);
        m_attr.SetItalic(false);
        m_attr.SetStrikethrough(false);
    }

    ~StoryTreeModelNode() {
        for (size_t i = 0; i < m_children.GetCount(); i++) {
            StoryTreeModelNode* child = m_children[i];

            if (child)
                delete child;
        }
    }

    bool IsContainer() const {
        return m_isContainer;
    }

    void Reparent(StoryTreeModelNode* newParent) {
        if (m_parent)
            m_parent->GetChildren().Remove(this);

        m_parent = newParent;

        if (m_parent)
            m_parent->Append(this);
    }

    void Reparent(StoryTreeModelNode* newParent, int n) {
        if (m_parent)
            m_parent->GetChildren().Remove(this);

        m_parent = newParent;

        if (m_parent)
            m_parent->Insert(this, n);
    }

    void Reposition(int n) {
        /*if (m_parent)
            m_parent->getChildren().Remove(this);
        else
            return;

        m_parent->getChildren().Insert(this, n);*/
    }

    StoryTreeModelNode* GetParent() {
        return m_parent;
    }

    StoryTreeModelNodePtrArray& GetChildren() {
        return m_children;
    }

    StoryTreeModelNode* GetChild(unsigned int n) {
        return m_children.Item(n);
    }

    wxDataViewItemAttr& GetAttr() {
        return m_attr;
    }

    void Insert(StoryTreeModelNode* child, unsigned int n) {
        m_children.Insert(child, n);
    }

    void Append(StoryTreeModelNode* child) {
        m_children.push_back(child);
    }

    int GetChildCount() const {
        return m_children.GetCount();
    }
};

class amStoryWriterNotebook;

class amStoryWriter : public wxFrame {
private:
    amProjectManager* m_manager = nullptr;
    amStoryWriterNotebook* m_cwNotebook = nullptr;

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

    wxDataViewCtrl* m_outlineView = nullptr;
    wxObjectDataPtr<OutlineTreeModel> m_outlineTreeModel;

    wxBoxSizer* m_pageSizer = nullptr;

    wxButton* m_noteClear = nullptr,
        * m_noteAdd = nullptr;

    unsigned int m_chapterPos;

    wxTimer m_saveTimer;

    bool m_doGreenNote = false;

public:
    wxStatusBar* m_statusBar = nullptr;

public:
    amStoryWriter(wxWindow* parent, amProjectManager* manager, int numb);

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

    void OnListResize(wxSizeEvent& event);
    void OnLeftSplitterChanged(wxSplitterEvent& event);

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


class amStoryWriterNotebook : public wxPanel {
private:
    amStoryWriter* m_parent = nullptr;
    wxAuiNotebook* m_notebook = nullptr;
    
    wxToolBar* m_contentToolbar = nullptr;
    wxComboBox* m_fontSize = nullptr;
    wxSlider* m_contentScale = nullptr;

    wxRichTextCtrl* m_curTextCtrl = nullptr;
    wxRichTextStyleSheet* m_styleSheet = nullptr;
    wxVector<Note> m_notes;
    wxScrolledWindow* m_curCorkboard = nullptr;

    wxWrapSizer* m_notesSizer = nullptr;
    wxPanel* m_selNote = nullptr;
    wxSize m_noteSize{};

    wxVector<pair<wxRichTextCtrl*, wxScrolledWindow*>> m_writerPages{};

public:
    amStoryWriterNotebook::amStoryWriterNotebook(wxWindow* parent, amStoryWriter* chapterWriter);

    wxRichTextCtrl* GetTextCtrl() { return m_curTextCtrl; }
    wxPanel* GetCorkboard() { return m_curCorkboard; }
    
    wxVector<Note>& GetNotes() { return m_notes; }
    wxWrapSizer* GetNotesSizer() { return m_notesSizer; }

    void SetNoteSize(wxSize& size) { m_noteSize = size; }

    void OnText(wxCommandEvent& event);

    void OnBold(wxCommandEvent& event);
    void OnItalic(wxCommandEvent& event);
    void OnUnderline(wxCommandEvent& event);
    void OnAlignLeft(wxCommandEvent& event);
    void OnAlignCenter(wxCommandEvent& event);
    void OnAlignCenterJust(wxCommandEvent& event);
    void OnAlignRight(wxCommandEvent& event);
    void OnNoteView(wxCommandEvent& event);
    void OnTestCircle(wxCommandEvent& event);

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
    void OnUpdateNoteView(wxUpdateUIEvent& event);

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
    TOOL_NoteView,
    TOOL_PageView,
    TOOL_ContentScale,
    TOOL_ChapterFullScreen,
    TOOL_TestCircle,

    MENU_Delete,
    TEXT_Content
};


#endif