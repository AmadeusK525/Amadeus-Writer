#ifndef CHAPTERWRITER_H_
#define CHAPTERWRITER_H_
#pragma once

#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/dataview.h>
#include <wx/aui/aui.h>
#include <wx/wrapsizer.h>
#include <wx/scrolwin.h>

#include "ProjectManaging.h"
#include "Style/Style.hpp"
#include "Views/Story/StoryNotebook.h"
#include "Views/Outline/OutlineFiles.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../Assets/OSX/Book.xpm"
#include "../Assets/OSX/Section.xpm"
#include "../Assets/OSX/Trash.xpm"
#endif

struct amStoryWriterTab;
class amStoryWriter;
class amStoryWriterNotebook;


///////////////////////////////////////////////////////////////////
//////////////////////// StoryTreeModelNode ///////////////////////
///////////////////////////////////////////////////////////////////


enum STMN_Type
{
	STMN_Book,
	STMN_Document,

	STMN_Trash,
	STMN_Null
};

class StoryTreeModelNode : public amTreeModelNode
{
private:
	StoryTreeModelNode* m_trashNode = nullptr;

	STMN_Type m_type = STMN_Null;
	am::Document* m_document = nullptr;
	am::Book* m_book = nullptr;
	int m_index = -1;

	inline StoryTreeModelNode(StoryTreeModelNode* parent, StoryTreeModelNode* trash,
		const wxString& title, int index, STMN_Type type) :
		amTreeModelNode(parent, title)
	{
		m_index = index;
		m_type = type;

		if ( m_parent )
		{
			if ( trash )
			{
				trash->Append(this);
			}
			else
			{
				if ( index >= m_parent->GetChildCount() || index < 0 )
					m_parent->Append(this);
				else
					m_parent->Insert(this, index);
			}
		}

		if ( m_type == STMN_Document )
		{
			m_attr.SetBackgroundColour(wxColour(45, 45, 45));
			m_attr.SetColour(wxColour(255, 255, 255));
		}
		else
		{
			m_attr.SetBackgroundColour(wxColour(55, 55, 55));
			m_attr.SetColour(wxColour(255, 255, 255));
		}
	}

public:
	static wxVector<wxBitmapBundle> m_icons;

	inline StoryTreeModelNode(StoryTreeModelNode* parent, StoryTreeModelNode* trash, am::Document* document, int position) :
		StoryTreeModelNode(parent, trash, document->name, position, STMN_Document)
	{
		m_document = document;
	}

	inline StoryTreeModelNode(StoryTreeModelNode* parent, StoryTreeModelNode* trash, am::Document* document) :
		StoryTreeModelNode(parent, trash, document, document->position)
	{}

	inline StoryTreeModelNode(am::Book* book) :
		StoryTreeModelNode(nullptr, nullptr, book->title, -1, STMN_Book)
	{
		m_book = book;
	}

	inline StoryTreeModelNode(const wxString& trashTitle) :
		StoryTreeModelNode(nullptr, nullptr, trashTitle, -1, STMN_Trash)
	{}

	inline static void InitAllIcons()
	{
		if ( m_icons.empty() )
		{
            const wxSize iconSize(14, 14);

            wxBitmapBundle bookBitmapBundle = Style::Icon::GetBmp(IconName::Book, iconSize);
            wxBitmapBundle sectionBitmapBundle = Style::Icon::GetBmp(IconName::Book, iconSize);
            wxBitmapBundle fileBitmapBundle = Style::Icon::GetBmp(IconName::AddFile, iconSize);
            wxBitmapBundle trashBitmapBundle = Style::Icon::GetBmp(IconName::Trash, iconSize);

            m_icons.push_back(bookBitmapBundle);
			m_icons.push_back(sectionBitmapBundle);
			m_icons.push_back(fileBitmapBundle);
			m_icons.push_back(trashBitmapBundle);

#ifdef __WXMSW__
			m_hoverAttr.SetBackgroundColour(wxColour(110, 110, 110));
			m_hoverAttr.SetColour(wxColour(255, 255, 255));
#endif
		}
	}

	inline int GetID() {
		if ( m_document) return m_document->id;
		if ( m_book ) return m_book->id;

		return -1;
	}

	inline int GetIndex() { return m_index; }
	inline void SetIndex(int index) { m_index = index; }

	inline virtual wxString GetTitle() const override
	{
		if ( m_document )
			return m_document->name;

		if ( m_book )
			return m_book->title;

		if ( IsTrash() )
			return _("Trash");

		return "";
	}

	inline bool IsBook() const { return m_type == STMN_Book; }
	inline bool IsDocument() const { return m_type == STMN_Document; }
	inline bool IsTrash() const { return m_type == STMN_Trash; }

	inline am::Book* GetBook() { return m_book; }
	inline am::Document* GetDocument() { return m_document; }

	inline bool IsInTrash() { return (m_document && m_document->isInTrash); }
	inline void SetIsInTrash(bool is) { if ( m_document ) m_document->isInTrash = is; }

	inline STMN_Type GetType() { return m_type; }

	inline virtual void Reparent(StoryTreeModelNode* newParent)
	{
		if ( m_parent && !IsInTrash() )
			RemoveSelfFromParentList();

		m_parent = newParent;

		if ( m_parent )
			m_parent->Append(this);
	}

	inline virtual void Reparent(StoryTreeModelNode* newParent, int n)
	{
		if ( m_parent && !IsInTrash() )
			RemoveSelfFromParentList();

		m_parent = newParent;

		if ( m_parent )
			m_parent->Insert(this, n);
	}

	inline virtual void Reposition(int n)
	{
		/*if (m_parent)
			m_parent->getChildren().Remove(this);
		else
			return;

		m_parent->getChildren().Insert(this, n);*/
	}
};


class StoryTreeModel : public amDataViewModel
{
protected:
	// pointers to some "special" nodes of the tree:
	wxVector<StoryTreeModelNode*> m_vBooks;
	StoryTreeModelNode* m_trashNode = nullptr;

public:
	StoryTreeModel();
	~StoryTreeModel()
	{
		for ( StoryTreeModelNode*& bookNode : m_vBooks )
		{
			if ( bookNode )
				delete bookNode;
		}

		if ( m_trashNode )
			delete m_trashNode;
	}

	bool Load();
	bool Save();
	void CreateFromScratch(am::Project* project);

	inline wxVector<StoryTreeModelNode*>& GetBooks() { return m_vBooks; }
	inline StoryTreeModelNode* GetTrash() { return m_trashNode; }

	wxDataViewItem AddDocument(am::Document* document, const wxDataViewItem& parentItem);

	wxDataViewItem GetBookItem(am::Book* book);
	wxDataViewItem GetDocumentItem(am::Document* document);
	wxDataViewItem ScanForDocumentRecursive(StoryTreeModelNode* node, am::Document* document);

	int GetIndex(const wxDataViewItem& item);

	STMN_Type MoveToTrash(const wxDataViewItem& item);
	STMN_Type RestoreFromTrash(const wxDataViewItem& item);
	void DeleteItem(const wxDataViewItem& item);

	wxDataViewItem SelectDocument(am::Document* document);

	bool Reposition(const wxDataViewItem& item, int n);

	void ClearContent();
	void ClearAll();

	// implementation of base class virtuals to define model
	virtual unsigned int GetColumnCount() const
	{
		return 1;
	}

	virtual wxString GetColumnType(unsigned int col) const
	{
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


///////////////////////////////////////////////////////////////////////
//////////////////////////// StoryWriterTab ///////////////////////////
///////////////////////////////////////////////////////////////////////


struct amStoryWriterTab
{
	wxPanel* mainPanel = nullptr;

	wxRichTextCtrl* rtc = nullptr;
	wxScrolledWindow* notePanel = nullptr;

	am::Document* document = nullptr;

	amStoryWriterTab() = default;
	amStoryWriterTab(wxPanel* mainPanel, wxRichTextCtrl* rtc, wxScrolledWindow* notePanel, am::Document* document) :
		mainPanel(mainPanel), rtc(rtc), notePanel(notePanel), document(document)
	{}
};


///////////////////////////////////////////////////////////////////
//////////////////////////// StoryWriter //////////////////////////
///////////////////////////////////////////////////////////////////


class amStoryWriterNotebook;

class amStoryWriter : public wxFrame
{
private:
	amStoryWriterNotebook* m_swNotebook = nullptr;

	wxTextCtrl* m_noteLabel = nullptr;
	wxRichTextCtrl* m_synopsys = nullptr,
		* m_note = nullptr;

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

	amStoryWriterTab m_activeTab;
	wxTimer m_saveTimer;

	wxDataViewItem m_itemForTrash{ nullptr };
	wxDataViewItem m_itemForRestore{ nullptr };

public:
	wxStatusBar* m_statusBar = nullptr;

public:
	amStoryWriter(wxWindow* parent, am::Document* document);

	void ClearNote(wxCommandEvent& event);
	void OnAddNote(wxCommandEvent& event);
	void AddNote(am::Note* note, bool addToDocument);

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

	void OnElementActivated(wxListEvent& event);

	void OnStoryItemActivated(wxDataViewEvent& event);
	void OnStoryItemSelected(wxDataViewEvent& event);
	void OnStoryViewRightClick(wxMouseEvent& event);

	void OnMoveToTrash(wxCommandEvent& event);
	void OnRestoreFromTrash(wxCommandEvent& event);

	void SetActiveTab(const amStoryWriterTab& tab, bool saveBefore, bool load);

	void OnNextDocument(wxCommandEvent& event);
	void OnPreviousDocument(wxCommandEvent& event);

	void LoadDocument(am::Document* document);
	void SaveActiveTab();

	inline amStoryWriterTab GetActiveTab() { return m_activeTab; }

	void CountWords();

	void OnCharRTC(wxRichTextEvent& event);

	void OnTimerEvent(wxTimerEvent& event);
	void OnLeftSplitterChanged(const wxSplitterEvent& event);

	void ToggleFullScreen(bool fs);
	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};

enum
{
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


//////////////////////////////////////////////////////////////////
//////////////////////// StoryWriterToolbar //////////////////////
//////////////////////////////////////////////////////////////////


enum
{
	amTB_FULLSCREEN = 1,
	amTB_NOTE_VIEW = 2,
	amTB_ZOOM = 4,

	amTB_DEFAULT_STYLE = amTB_FULLSCREEN | amTB_NOTE_VIEW | amTB_ZOOM
};

class amStoryWriterToolbar : public wxToolBar
{
private:
	enum
	{
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

	amStoryWriterTab m_currentTab;

public:
	amStoryWriterToolbar(wxWindow* parent,
		long amStyle = amTB_DEFAULT_STYLE,
		wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTB_DEFAULT_STYLE | wxTB_FLAT);

	inline void SetCurrentTab(const amStoryWriterTab& currentPage) { m_currentTab = currentPage; }

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


class amStoryWriterNotebook : public wxPanel
{
private:
	amStoryWriter* m_storyWriter = nullptr;
	wxAuiNotebook* m_notebook = nullptr;

	amStoryWriterToolbar* m_contentToolbar = nullptr;

	wxRichTextCtrl* m_curTextCtrl = nullptr;
	wxRichTextStyleSheet* m_styleSheet = nullptr;
	wxScrolledWindow* m_curCorkboard = nullptr;

	wxPanel* m_selNote = nullptr;
	wxSize m_noteSize{};

	wxVector<amStoryWriterTab> m_swTabs{};
	amStoryWriterTab m_closingTab;

public:
	amStoryWriterNotebook(wxWindow* parent, amStoryWriter* documentWriter);

	inline wxRichTextCtrl* GetTextCtrl() { return m_curTextCtrl; }
	inline wxPanel* GetCorkboard() { return m_curCorkboard; }
	inline wxVector<amStoryWriterTab>& GetTabs() { return m_swTabs; }
	inline amStoryWriterToolbar* GetToolbar() { return m_contentToolbar; }

	inline wxWrapSizer* GetNotesSizer() { return (wxWrapSizer*)m_curCorkboard->GetSizer(); }
	//inline void SetNotes(wxVector<Note*>& notes) { m_notes = notes; }

	inline wxWindow* GetCurrentPage() { return m_notebook->GetCurrentPage(); }
	inline int GetSelection() { return m_notebook->GetSelection(); }
	inline wxVector<amStoryWriterTab>& GetAllWriterPages() { return m_swTabs; }
	inline wxAuiNotebook* GetNotebook() { return m_notebook; }

	void AddTab(const amStoryWriterTab& tab, const wxString& title);
	void SetCurrentTab(const amStoryWriterTab& tab, bool load);

	inline void SetNoteSize(const wxSize& size) { m_noteSize = size; }

	void OnSelectionChanged(wxAuiNotebookEvent& event);
	void OnPageClosing(wxAuiNotebookEvent& event);
	void OnPageClosed(wxAuiNotebookEvent& event);

	void OnText(wxCommandEvent& event);
	void OnZoomChange(int zoom);

	inline void ToggleFullScreen(bool fs) { m_storyWriter->ToggleFullScreen(fs); }

	bool HasRedNote();

	void AddNote(am::Note* note);
	void PaintDots(wxPaintEvent& event);
	void SetRed(wxCommandEvent& event);
	void SetGreen(wxCommandEvent& event);
	void OnDeleteNote(wxCommandEvent& event);

	void OnNoteClick(wxMouseEvent& event);

	void UpdateNoteLabel(wxCommandEvent& event);
	void UpdateNote(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

enum
{
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
