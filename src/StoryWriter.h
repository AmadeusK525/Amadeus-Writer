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

class StoryTreeModelNode : public amTreeModelNode {
private:
    StoryTreeModelNode* m_trash = nullptr;

    STMN_Type m_type = STMN_Null;
    int m_index = -1;
    int m_dbID = -1;

    bool m_isInTrash = false;

public:
    static wxVector<wxIcon> m_icons;

public:
    inline StoryTreeModelNode(StoryTreeModelNode* parent, StoryTreeModelNode* trash,
        const wxString& title, int index, int id, STMN_Type type) :
        amTreeModelNode(parent, title) {
        m_index = index;
        m_dbID = id;
        m_type = type;

        if (m_parent) {
            if (trash) {
                trash->Append(this);
                m_isInTrash = true;
            } else {
                if (index >= m_parent->GetChildCount())
                    m_parent->Append(this);
                else
                    m_parent->Insert(this, index);
            }
        }

        if (m_type == STMN_Chapter) {
            m_attr.SetBackgroundColour(wxColour(45, 45, 45));
            m_attr.SetColour(wxColour(255, 255, 255));
        } else {
            m_attr.SetBackgroundColour(wxColour(55, 55, 55));
            m_attr.SetColour(wxColour(255, 255, 255));
        }
    }

    inline static void InitAllIcons() {
        if (m_icons.empty()) {
            int x = 14, y = 14;

            wxIcon book(wxICON(bookIcon));
            book.SetSize(x, y);

            wxIcon section(wxICON(sectionIcon));
            section.SetSize(x, y);

            wxIcon chapter(wxICON(fileIcon));
            chapter.SetSize(x, y);

            wxIcon trash(wxICON(trashIcon));
            trash.SetSize(x, y);

            m_icons.push_back(book);
            m_icons.push_back(section);
            m_icons.push_back(chapter);
            m_icons.push_back(trash);
    
#ifdef __WXMSW__
            m_hoverAttr.SetBackgroundColour(wxColour(110, 110, 110));
            m_hoverAttr.SetColour(wxColour(255, 255, 255));
#endif
        }
    }

    inline int GetID() { return m_dbID; }

    inline int GetIndex() { return m_index; }
    inline void SetIndex(int index) { m_index = index; }

    inline void SetTitle(wxString& title) { m_title = title; }

    inline bool IsBook() { return m_type == STMN_Book; }
    inline bool IsSection() { return m_type == STMN_Section; }
    inline bool IsChapter() { return m_type == STMN_Chapter; }
    inline bool IsScene() { return m_type == STMN_Scene; }
    inline bool IsTrash() { return m_type == STMN_Trash; }

    inline bool IsInTrash() { return m_isInTrash; }
    inline void SetIsInTrash(bool is) { m_isInTrash = is; }

    inline STMN_Type GetType() { return m_type; }

    inline virtual void Reparent(StoryTreeModelNode* newParent) {
        if (m_parent && !m_isInTrash)
            RemoveSelfFromParentList();

        m_parent = newParent;

        if (m_parent)
            m_parent->Append(this);
    }

    inline virtual void Reparent(StoryTreeModelNode* newParent, int n) {
        if (m_parent && !m_isInTrash)
            RemoveSelfFromParentList();

        m_parent = newParent;

        if (m_parent)
            m_parent->Insert(this, n);
    }

    inline virtual void Reposition(int n) {
        /*if (m_parent)
            m_parent->getChildren().Remove(this);
        else
            return;

        m_parent->getChildren().Insert(this, n);*/
    }
};


class StoryTreeModel : public amDataViewModel {
private:
    // pointers to some "special" nodes of the tree:
    StoryTreeModelNode* m_book = nullptr;
    StoryTreeModelNode* m_trash = nullptr;

public:
    StoryTreeModel();
    ~StoryTreeModel() {
        if (m_book)
            delete m_book;

        if (m_trash)
            delete m_trash;
    }

    bool Load();
    bool Save();
    void CreateFromScratch(Book& book);

    wxDataViewItem AddSection(const wxString& name, int id, int index, bool isInTrash);
    wxDataViewItem AddChapter(const wxDataViewItem& section, const wxString& name, int id, int index, bool isInTrash);
    wxDataViewItem AddScene(const wxDataViewItem& chapter, const wxString& name, int id, int index, bool isInTrash);

    wxDataViewItem GetChapterItem(int chapterIndex, int sectionIndex);

    void RedeclareChapterIndexes(const Section& section);
    int GetIndex(const wxDataViewItem& item);

    STMN_Type MoveToTrash(const wxDataViewItem& item);
    STMN_Type RestoreFromTrash(const wxDataViewItem& item);
    void DeleteItem(const wxDataViewItem& item);

    bool Reposition(const wxDataViewItem& item, int n);

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

    virtual wxDataViewItem GetParent(const wxDataViewItem& item) const;
    virtual unsigned int GetChildren(const wxDataViewItem& parent,
        wxDataViewItemArray& array) const;

    ////////////////////////////// DragEventHandlers ////////////////////////////

    virtual void OnBeginDrag(wxDataViewEvent& event, wxDataViewCtrl* dvc) {}
    virtual void OnDropPossible(wxDataViewEvent& event, wxDataViewCtrl* dvc) {}
    virtual void OnDrop(wxDataViewEvent& event, wxDataViewCtrl* dvc) {}
};


///////////////////////////////////////////////////////////////////
//////////////////////////// StoryWriter //////////////////////////
///////////////////////////////////////////////////////////////////


class amStoryWriterNotebook;

class amStoryWriter : public wxFrame {
private:
    amProjectManager* m_manager = nullptr;
    amStoryWriterNotebook* m_swNotebook = nullptr;

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

    wxDataViewCtrl* m_storyView = nullptr;
    wxObjectDataPtr<StoryTreeModel> m_storyTreeModel;

    wxDataViewCtrl* m_outlineView = nullptr;
    wxObjectDataPtr<OutlineTreeModel> m_outlineTreeModel;

#ifdef __WXMSW__
    amHotTrackingDVCHandler m_storyViewHTHandler;
    amHotTrackingDVCHandler m_outlineViewHTHandler;
#endif

    wxBoxSizer* m_pageSizer = nullptr;

    wxButton* m_noteClear = nullptr,
        * m_noteAdd = nullptr;

    int m_bookPos = -1;

    int m_prevChapterIndex = -1;
    int m_chapterIndex = -1;

    int m_prevSectionIndex = -1;
    int m_sectionIndex = -1;

    wxTimer m_saveTimer;

    bool m_doGreenNote = false;

    wxDataViewItem m_itemForTrash{ nullptr };
    wxDataViewItem m_itemForRestore{ nullptr };

public:
    wxStatusBar* m_statusBar = nullptr;

public:
    amStoryWriter(wxWindow* parent, amProjectManager* manager, int chapterPos, int sectionIndex);

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

    void OnStoryItemActivated(wxDataViewEvent& event);
    void OnStoryItemSelected(wxDataViewEvent& event);
    void OnStoryViewRightClick(wxMouseEvent& event);

    void OnMoveToTrash(wxCommandEvent& event);
    void OnRestoreFromTrash(wxCommandEvent& event);

    void SetCurrentChapter(int chapterIndex, int sectionIndex, bool load);
    void OnNextChapter(wxCommandEvent& event);
    void OnPreviousChapter(wxCommandEvent& event);
    void CheckChapterValidity();

    void LoadChapter();
    void SaveChapter(int chapterIndex, int sectionIndex);
    void UnloadChapter(int chapterIndex, int sectionIndex);

    void CountWords();

    void OnTimerEvent(wxTimerEvent& event);
    void OnLeftSplitterChanged(wxSplitterEvent& event);

    void ToggleFullScreen(bool fs);
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
    TIMER_Words,
    
    MENU_MoveToTrash,
    MENU_RestoreFromTrash
};


///////////////////////////////////////////////////////////////////////
//////////////////////// StoryWriterNotebookPage //////////////////////
///////////////////////////////////////////////////////////////////////


struct amStoryWriterNotebookPage {
    wxRichTextCtrl* rtc = nullptr;
    wxScrolledWindow* notePanel = nullptr;

    int chapterIndex = -1;
    int sectionIndex = -1;
    int dbID = -1;

    amStoryWriterNotebookPage() = default;
    amStoryWriterNotebookPage(wxRichTextCtrl* rtc, wxScrolledWindow* notePanel,
        int chapterIndex, int sectionIndex, int dbID) :
        rtc(rtc), notePanel(notePanel),
        chapterIndex(chapterIndex), sectionIndex(sectionIndex), dbID(dbID) {}
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
        TOOL_StoryFullScreen,
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
        long style = wxTB_DEFAULT_STYLE | wxTB_FLAT);

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
    void OnUpdateZoom(wxUpdateUIEvent& event);

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
    inline amStoryWriterToolbar* GetToolbar() { return m_contentToolbar; }

    inline wxVector<Note>& GetNotes() { return m_notes; }
    inline wxWrapSizer* GetNotesSizer() { return (wxWrapSizer*)m_curCorkboard->GetSizer(); }
    inline void SetNotes(wxVector<Note>& notes) { m_notes = notes; }

    inline wxWindow* GetCurrentPage() { return m_notebook->GetCurrentPage(); }
    inline int GetSelection() { return m_notebook->GetSelection(); }
    inline wxVector<amStoryWriterNotebookPage>& GetAllWriterPages() { return m_writerPages; }
    inline wxAuiNotebook* GetNotebook() { return m_notebook; }

    void AddPage(amStoryWriterNotebookPage& page, const wxString& title);
    void SetCurrentPage(amStoryWriterNotebookPage& page, bool load);

    inline void SetNoteSize(wxSize& size) { m_noteSize = size; }

    void OnSelectionChanged(wxAuiNotebookEvent& event);
    void OnPageClosing(wxAuiNotebookEvent& event);

    void OnText(wxCommandEvent& event);
    void OnZoomChange(int zoom);

    inline void ToggleFullScreen(bool fs) { m_parent->ToggleFullScreen(fs); }

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
    TOOL_StoryFullScreen,
    TOOL_TestCircle,

    MENU_Delete,
    TEXT_Content
};

#endif