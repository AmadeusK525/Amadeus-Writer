#ifndef OUTLINEFILES_H_
#define OUTLINEFILES_H_
#pragma once

#include <wx\splitter.h>
#include <wx\dataview.h>
#include <wx\toolbar.h>
#include <wx\richtext\richtextctrl.h>
#include <wx\sstream.h>

#include "StoryElements.h"
#include "amUtility.h"
#include "ProjectManager.h"

#include <atomic>

class OutlineTreeModelNode : public amTreeModelNode
{
private:
	wxRichTextBuffer* m_buffer = nullptr;

public:
	static wxVector<wxIcon> m_icons;

public:
	inline OutlineTreeModelNode(OutlineTreeModelNode* parent,
		const wxString& title, bool isContainer) : amTreeModelNode(parent, title)
	{

		if ( m_parent )
			m_parent->Append(this);

		m_isContainer = isContainer;
		if ( !isContainer )
		{
			m_buffer = new wxRichTextBuffer;
			m_attr.SetBackgroundColour(wxColour(55, 55, 55));
			m_attr.SetColour(wxColour(255, 255, 255));
		}
		else
		{
			m_attr.SetBackgroundColour(wxColour(65, 65, 65));
			m_attr.SetColour(wxColour(255, 255, 255));
		}
	}

	inline virtual ~OutlineTreeModelNode()
	{
		if ( m_buffer )
			delete m_buffer;
	}

	inline static void InitAllIcons()
	{
		if ( m_icons.empty() )
		{
			int x = 14, y = 14;

			wxIcon research(wxICON(researchIcon));
			research.SetSize(x, y);

			wxIcon folder(wxICON(folderIcon));
			folder.SetSize(x, y);

			wxIcon file(wxICON(fileIcon));
			file.SetSize(x, y);

			m_icons.push_back(research);
			m_icons.push_back(folder);
			m_icons.push_back(file);

#ifdef __WXMSW__
			m_hoverAttr.SetBackgroundColour(wxColour(110, 110, 110));
			m_hoverAttr.SetColour(wxColour(255, 255, 255));
#endif
		}
	}

	inline wxRichTextBuffer* GetBuffer() { return m_buffer; }
};

class OutlineTreeModel : public amDataViewModel
{
private:
	// pointers to some "special" nodes of the tree:
	OutlineTreeModelNode* m_research;
	OutlineTreeModelNode* m_characters;
	OutlineTreeModelNode* m_locations;
	OutlineTreeModelNode* m_items;

public:
	OutlineTreeModel();
	inline virtual ~OutlineTreeModel()
	{
		if ( m_research )
			delete m_research;

		if ( m_characters )
			delete m_characters;

		if ( m_locations )
			delete m_locations;

		if ( m_items )
			delete m_items;
	}

	wxRichTextBuffer* GetBuffer(const wxDataViewItem& item) const;

	inline wxVector<amTreeModelNode*>& GetCharacters()
	{
		return m_characters->GetChildren();
	}

	inline wxVector<amTreeModelNode*>& GetLocations()
	{
		return m_locations->GetChildren();
	}

	inline wxVector<amTreeModelNode*>& GetItems()
	{
		return m_items->GetChildren();
	}

	inline OutlineTreeModelNode* GetResearchNode()
	{
		return m_research;
	}

	inline OutlineTreeModelNode* GetCharactersNode()
	{
		return m_characters;
	}

	inline OutlineTreeModelNode* GetLocationsNode()
	{
		return m_locations;
	}

	inline OutlineTreeModelNode* GetItemsNode()
	{
		return m_items;
	}

	// helper methods to change the model

	wxDataViewItem AddToResearch(const wxString& title);
	wxDataViewItem AddToCharacters(const wxString& title);
	wxDataViewItem AddToLocations(const wxString& title);
	wxDataViewItem AddToItems(const wxString& title);

	wxDataViewItem AppendFile(const wxDataViewItem& parent, const wxString& name);
	wxDataViewItem AppendFolder(const wxDataViewItem& parent, const wxString& name);

	bool IsResearch(const wxDataViewItem& item);
	bool IsCharacters(const wxDataViewItem& item);
	bool IsLocations(const wxDataViewItem& item);
	bool IsItems(const wxDataViewItem& item);

	bool IsSpecial(const wxDataViewItem& item);

	void DeleteItem(const wxDataViewItem& item);

	bool Reposition(const wxDataViewItem& item, int n);

	void Clear();

	// implementation of base class virtuals to define model
	inline virtual unsigned int GetColumnCount() const
	{
		return 1;
	}

	inline virtual wxString GetColumnType(unsigned int col) const
	{
		return "wxString";
	}

	virtual void GetValue(wxVariant& variant,
		const wxDataViewItem& item, unsigned int col) const;

	virtual unsigned int GetChildren(const wxDataViewItem& parent,
		wxDataViewItemArray& array) const;

	////////////////////////////// DragEventHandlers ////////////////////////////

	virtual void OnBeginDrag(wxDataViewEvent& event, wxDataViewCtrl* dvc);
	virtual void OnDropPossible(wxDataViewEvent& event, wxDataViewCtrl* dvc);
	virtual void OnDrop(wxDataViewEvent& event, wxDataViewCtrl* dvc);
};

enum
{
	TREE_Files,

	TOOL_NewFile,
	TOOL_NewFolder,

	MENU_ChangeItemFgColour,
	MENU_ChangeItemBgColour,
	MENU_DeleteItem,

	TIMER_OutlineFiles
};

class amOutlineFilesPanel : public amSplitterWindow
{
private:
	wxPanel* m_leftPanel = nullptr;
	amHotTrackingDVCHandler m_filesHTHandler;
	wxDataViewCtrl* m_files = nullptr;

	wxRichTextCtrl* m_textCtrl = nullptr;
	wxRichTextAttr m_basicAttr{};

	wxToolBar* m_filesTB = nullptr, * m_contentTB = nullptr;

	wxObjectDataPtr<OutlineTreeModel> m_outlineTreeModel;

	OutlineTreeModelNode* m_currentNode = nullptr;

	wxTimer m_timer{ this, TIMER_OutlineFiles };
	std::atomic<bool> m_isSaving = false;

public:
	amOutlineFilesPanel(wxWindow* parent);
	void Init();

	wxObjectDataPtr<OutlineTreeModel>& GetOutlineTreeModel() { return m_outlineTreeModel; }

	void AppendStoryElement(StoryElement* element);
	void DeleteStoryElement(StoryElement* element);

	void GenerateElementBuffer(StoryElement* element, wxRichTextBuffer* buffer);
	/*void GenerateCharacterBuffer(Character* character, wxRichTextBuffer* buffer);
	void GenerateLocationBuffer(Location* location, wxRichTextBuffer* buffer);
	void GenerateItemBuffer(Item* item, wxRichTextBuffer* buffer);*/
	/*void AppendCharacter(Character* character);
	void AppendLocation(Location* location);
	void AppendItem(Item* item);*/

	void DeleteCharacter(Character* character);
	void DeleteLocation(Location* location);
	void DeleteItem(Item* item);

	void NewFile(wxCommandEvent& event);
	void NewFolder(wxCommandEvent& event);

	void DeleteItem(wxDataViewItem& item);

	void OnKeyDownDataView(wxKeyEvent& event);
	void OnMenuDataView(wxCommandEvent& event);

	void OnRightDownDataView(wxDataViewEvent& event);
	void OnSelectionChanged(wxDataViewEvent& event);
	void OnEditingStart(wxDataViewEvent& event);
	void OnEditingEnd(wxDataViewEvent& event);

	void OnItemExpanded(wxDataViewEvent& event);
	void OnItemCollapsed(wxDataViewEvent& event);
	void OnDrop(wxDataViewEvent& event);

	void OnTimerEvent(wxTimerEvent& event);
	void SaveCurrentBuffer();

	wxXmlNode* SerializeFolder(wxDataViewItem& item);
	wxXmlNode* SerializeFile(wxDataViewItem& item);
	void DeserializeNode(wxXmlNode* node, wxDataViewItem& parent);
	bool Save();
	bool Load(amProjectSQLDatabase* db);

	void ClearAll();

	DECLARE_EVENT_TABLE()
};

#endif