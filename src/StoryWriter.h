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


///////////////////////////////////////////////////////////////////
//////////////////////// StoryTreeModelNode ///////////////////////
///////////////////////////////////////////////////////////////////


enum STMN_Type {
    STMN_Book,
    STMN_Section,
    STMN_Chapter,
    STMN_Scene,

    STMN_Trash,
    STMN_Null
};

class StoryTreeModelNode;
WX_DEFINE_ARRAY_PTR(StoryTreeModelNode*, StoryTreeModelNodePtrArray);

class StoryTreeModelNode {
private:
    StoryTreeModelNode* m_parent = nullptr;
    StoryTreeModelNodePtrArray m_children{};

    wxDataViewItemAttr m_attr{};

    STMN_Type m_type = STMN_Null;
    int m_pos = -1;
    int m_dbID = -1;

    wxString m_title{};
    bool m_isContainer = false;

public:
    static wxVector<wxIcon> m_icons;

public:
    inline StoryTreeModelNode(StoryTreeModelNode* parent,
        const wxString& title, int pos, int id, STMN_Type type) {
        m_parent = parent;
        m_title = title;
        m_pos = pos;
        m_dbID = id;
        m_type = type;

        if (m_parent)
            m_parent->Insert(this, pos);

        if (m_type == STMN_Chapter) {
            m_attr.SetBackgroundColour(wxColour(45, 45, 45));
            m_attr.SetColour(wxColour(255, 255, 255));
        } else {
            m_attr.SetBackgroundColour(wxColour(55, 55, 55));
            m_attr.SetColour(wxColour(255, 255, 255));
        }
    }

    inline ~StoryTreeModelNode() {
        for (size_t i = 0; i < m_children.GetCount(); i++) {
            StoryTreeModelNode* child = m_children[i];

            if (child)
                delete child;
        }
    }

    inline static void InitAllIcons() {
        if (m_icons.empty()) {
            int x = 14, y = 14;

            wxIcon book(wxICON(bookIcon));
            book.SetHeight(x);
            book.SetWidth(y);

            wxIcon section(wxICON(sectionIcon));
            section.SetHeight(x);
            section.SetWidth(y);

            wxIcon chapter(wxICON(fileIcon));
            chapter.SetHeight(x);
            chapter.SetWidth(y);

            wxIcon trash(wxICON(trashIcon));
            trash.SetHeight(x);
            trash.SetWidth(y);

            m_icons.push_back(book);
            m_icons.push_back(section);
            m_icons.push_back(chapter);
            m_icons.push_back(trash);
        }
    }

    inline wxString& GetTitle() { return m_title; }
    inline int GetID() { return m_dbID; }

    inline void SetTitle(wxString& title) { m_title = title; }

    inline bool IsBook() { return m_type == STMN_Book; }
    inline bool IsSection() { return m_type == STMN_Section; }
    inline bool IsChapter() { return m_type == STMN_Chapter; }
    inline bool IsScene() { return m_type == STMN_Scene; }

    inline bool IsTrash() { return m_type == STMN_Trash; }

    inline STMN_Type GetType() { return m_type; }

    inline bool IsContainer() const { 
        return m_isContainer;
    }
    inline void SetContainer(bool is) { m_isContainer = is; }

    inline void Reparent(StoryTreeModelNode* newParent) {
        if (m_parent)
            m_parent->GetChildren().Remove(this);

        m_parent = newParent;

        if (m_parent)
            m_parent->Append(this);
    }

    inline void Reparent(StoryTreeModelNode* newParent, int n) {
        if (m_parent)
            m_parent->GetChildren().Remove(this);

        m_parent = newParent;

        if (m_parent)
            m_parent->Insert(this, n);
    }

    inline void Reposition(int n) {
        /*if (m_parent)
            m_parent->getChildren().Remove(this);
        else
            return;

        m_parent->getChildren().Insert(this, n);*/
    }

    inline StoryTreeModelNode* GetParent() {
        return m_parent;
    }

    inline StoryTreeModelNodePtrArray& GetChildren() {
        return m_children;
    }

    inline StoryTreeModelNode* GetChild(unsigned int n) {
        return m_children.Item(n);
    }

    inline wxDataViewItemAttr& GetAttr() {
        return m_attr;
    }

    inline void Insert(StoryTreeModelNode* child, unsigned int n) {
        m_children.Insert(child, n);
    }

    inline void Append(StoryTreeModelNode* child) {
        m_children.push_back(child);
    }

    inline int GetChildCount() const {
        return m_children.GetCount();
    }
};


class StoryTreeModel : public wxDataViewModel {
private:
    // pointers to some "special" nodes of the tree:
    StoryTreeModelNode* m_book = nullptr;
    StoryTreeModelNode* m_trash = nullptr;

    StoryTreeModelNodePtrArray m_otherRoots{};

public:
    StoryTreeModel();
    ~StoryTreeModel() {
        for (unsigned int i = 0; i < m_otherRoots.GetCount(); i++) {
            if (m_otherRoots.at(i))
                delete m_otherRoots.at(i);
        }
    }

    bool Load();
    bool Save();
    void CreateFromScratch(Book& book);

    wxString GetTitle(const wxDataViewItem& item) const;

    wxDataViewItem AddSection(const wxString& name, int id, int pos);
    wxDataViewItem AddChapter(wxDataViewItem& section, const wxString& name, int id, int pos);
    wxDataViewItem AddScene(wxDataViewItem& chapter, const wxString& name, int id, int pos);

    bool IsDescendant(wxDataViewItem& item, wxDataViewItem& descendant);

    void MoveToTrash(const wxDataViewItem& item);
    void DeleteItem(const wxDataViewItem& item);

    void SetItemBackgroundColour(wxDataViewItem& item, wxColour& colour);
    void SetItemForegroundColour(wxDataViewItem& item, wxColour& colour);
    void SetItemFont(wxDataViewItem& item, wxFont& font);

    bool Reparent(StoryTreeModelNode* item, StoryTreeModelNode* newParent);
    bool Reparent(StoryTreeModelNode* item, StoryTreeModelNode* newParent, int n);

    bool Reposition(wxDataViewItem& item, int n);

    void Clear();

    // implementation of base class virtuals to define model
    virtual unsigned int GetColumnCount() const {
        return 1;
    }

    virtual wxString GetColumnType(unsigned int col) const {
        return "wxString";
    }

    virtual void GetValue(wxVariant& variant,
        const wxDataViewItem& item, unsigned int col) const;
    virtual bool SetValue(const wxVariant& variant,
        const wxDataViewItem& item, unsigned int col);

    virtual wxDataViewItem GetParent(const wxDataViewItem& item) const;
    virtual unsigned int GetChildren(const wxDataViewItem& parent,
        wxDataViewItemArray& array) const;
    virtual bool IsContainer(const wxDataViewItem& item) const;
    virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const;
};

///////////////////////////////////////////////////////////////////
//////////////////////////// StoryWriter //////////////////////////
///////////////////////////////////////////////////////////////////


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

    wxDataViewCtrl* m_storyView = nullptr;
    wxObjectDataPtr<StoryTreeModel> m_storyTreeModel;

    wxBoxSizer* m_pageSizer = nullptr;

    wxButton* m_noteClear = nullptr,
        * m_noteAdd = nullptr;

    int m_chapterPos = -1;

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

    void OnStoryItemSelected(wxDataViewEvent& event);

    void SetCurrentChapter(int pos, bool load);
    void OnNextChapter(wxCommandEvent& event);
    void OnPreviousChapter(wxCommandEvent& event);
    void CheckChapterValidity();

    void LoadChapter();
    void SaveChapter();

    void CountWords();

    void OnTimerEvent(wxTimerEvent& event);
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

    TREE_Outline,
    TREE_Story,

    BUTTON_NoteAdd,
    BUTTON_NoteClear,

    BUTTON_PreviousChap,
    BUTTON_NextChap,

    TIMER_Save,
    TIMER_Words
};


///////////////////////////////////////////////////////////////////////
//////////////////////// StoryWriterNotebookPage //////////////////////
///////////////////////////////////////////////////////////////////////


struct amStoryWriterNotebookPage {
    wxRichTextCtrl* rtc = nullptr;
    wxScrolledWindow* notePanel = nullptr;

    int pos = -1;
    int dbID = -1;

    amStoryWriterNotebookPage() = default;
    amStoryWriterNotebookPage(wxRichTextCtrl* rtc, wxScrolledWindow* notePanel, int pos, int dbID):
        rtc(rtc), notePanel(notePanel), pos(pos), dbID(dbID) {}
};


//////////////////////////////////////////////////////////////////
//////////////////////// StoryWriterToolbar //////////////////////
//////////////////////////////////////////////////////////////////


enum {
    amTB_FULLSCREEN = 1,
    amTB_NOTE_VIEW = 2,
    amTB_ZOOM = 4,

    amTB_DEFAULT_STYLE = amTB_FULLSCREEN | amTB_NOTE_VIEW | amTB_ZOOM
};

class amStoryWriterToolbar : public wxToolBar {
private:
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
    };

    amStoryWriterNotebook* m_parent = nullptr;

    wxComboBox* m_fontSize = nullptr;
    wxSlider* m_contentScale = nullptr;

    amStoryWriterNotebookPage m_currentPage;

public:
    amStoryWriterToolbar(wxWindow* parent,
        long amStyle = amTB_DEFAULT_STYLE,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTB_DEFAULT_STYLE);

    inline void SetCurrentPage(amStoryWriterNotebookPage& currentPage) { m_currentPage = currentPage; }

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

    DECLARE_EVENT_TABLE()
};


////////////////////////////////////////////////////////////////////
//////////////////////// StoryWriterNotebook ///////////////////////
////////////////////////////////////////////////////////////////////


class amStoryWriterNotebook : public wxPanel {
private:
    amStoryWriter* m_parent = nullptr;
    wxAuiNotebook* m_notebook = nullptr;
    
    amStoryWriterToolbar* m_contentToolbar = nullptr;

    wxRichTextCtrl* m_curTextCtrl = nullptr;
    wxRichTextStyleSheet* m_styleSheet = nullptr;
    wxVector<Note> m_notes;
    wxScrolledWindow* m_curCorkboard = nullptr;

    wxPanel* m_selNote = nullptr;
    wxSize m_noteSize{};

    wxVector<amStoryWriterNotebookPage> m_writerPages{};

public:
    amStoryWriterNotebook::amStoryWriterNotebook(wxWindow* parent, amStoryWriter* chapterWriter);

    inline wxRichTextCtrl* GetTextCtrl() { return m_curTextCtrl; }
    inline wxPanel* GetCorkboard() { return m_curCorkboard; }
    inline wxVector<amStoryWriterNotebookPage>& GetPages() { return m_writerPages; }

    inline wxVector<Note>& GetNotes() { return m_notes; }
    inline wxWrapSizer* GetNotesSizer() { return (wxWrapSizer*)m_curCorkboard->GetSizer(); }
    inline void SetNotes(wxVector<Note>& notes) { m_notes = notes; }

    inline wxWindow* GetCurrentPage() { return m_notebook->GetCurrentPage(); }
    inline int GetSelection() { return m_notebook->GetSelection(); }
    inline wxVector<amStoryWriterNotebookPage>& GetAllWriterPages() { return m_writerPages; }
    inline wxAuiNotebook* GetNotebook() { return m_notebook; }

    void AddPage(amStoryWriterNotebookPage& page, const wxString& title);

    inline void SetNoteSize(wxSize& size) { m_noteSize = size; }

    void OnSelectionChanged(wxAuiNotebookEvent& event);

    void OnText(wxCommandEvent& event);
    void OnZoomChange(int zoom);

    inline void ToggleFullScreen() { m_parent->ToggleFullScreen(); }

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