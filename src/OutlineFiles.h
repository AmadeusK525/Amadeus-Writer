#ifndef OUTLINEFILES_H_
#define OUTLINEFILES_H_
#pragma once

#include <wx\splitter.h>
#include <wx\dataview.h>
#include <wx\toolbar.h>
#include <wx\richtext\richtextctrl.h>
#include <wx\sstream.h>

#include "StoryElements.h"

class OutlineTreeModelNode;
WX_DEFINE_ARRAY_PTR(OutlineTreeModelNode*, OulineTreeModelNodePtrArray);

class OutlineTreeModelNode {
private:
    OutlineTreeModelNode* m_parent = nullptr;
    OulineTreeModelNodePtrArray m_children{};

    wxDataViewItemAttr m_attr{};

    wxRichTextBuffer* m_buffer = nullptr;
    wxString m_title{};
    bool m_isContainer = true;

public:
    static wxVector<wxIcon> m_icons;

public:
    inline OutlineTreeModelNode(OutlineTreeModelNode* parent,
        const wxString& title, const wxRichTextBuffer& buffer) {
        m_parent = parent;
        m_title = title;
        m_buffer = new wxRichTextBuffer(buffer);

        if (m_parent)
            m_parent->Append(this);

        m_isContainer = false;

        m_attr.SetBackgroundColour(wxColour(45, 45, 45));
        m_attr.SetColour(wxColour(255, 255, 255));
    }

    inline OutlineTreeModelNode(OutlineTreeModelNode* parent,
        const wxString& branch) {
        m_parent = parent;
        m_title = branch;

        if (m_parent)
            m_parent->Append(this);

        m_isContainer = true;

        m_attr.SetBackgroundColour(wxColour(45, 45, 45));
        m_attr.SetColour(wxColour(255, 255, 255));
    }

    inline ~OutlineTreeModelNode() {
        for (size_t i = 0; i < m_children.GetCount(); i++) {
            OutlineTreeModelNode* child = m_children[i];

            if (child)
                delete child;
        }

        if (m_buffer)
            delete m_buffer;
    }

    inline static void InitAllIcons() {
        if (m_icons.empty()) {
            wxIcon research(wxICON(researchIcon));
            research.SetSize(14, 14);

            wxIcon folder(wxICON(folderIcon));
            folder.SetSize(14, 14);

            wxIcon file(wxICON(fileIcon));
            file.SetSize(14, 14);

            m_icons.push_back(research);
            m_icons.push_back(folder);
            m_icons.push_back(file);
        }
    }

    inline wxString& GetTitle() { return m_title; }
    inline wxRichTextBuffer& GetBuffer() { return *m_buffer; }

    inline void SetTitle(wxString& title) { m_title = title; }

    inline bool IsContainer() const {
        return m_isContainer;
    }

    inline void Reparent(OutlineTreeModelNode* newParent) {
        if (m_parent)
            m_parent->GetChildren().Remove(this);
        
        m_parent = newParent;
        
        if (m_parent)
            m_parent->Append(this);
    }

    inline void Reparent(OutlineTreeModelNode* newParent, int n) {
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

    inline OutlineTreeModelNode* GetParent() {
        return m_parent;
    }

    inline OulineTreeModelNodePtrArray& GetChildren() {
        return m_children;
    }

    inline OutlineTreeModelNode* GetChild(unsigned int n) {
        return m_children.Item(n);
    }

    inline wxDataViewItemAttr& GetAttr() {
        return m_attr;
    }

    inline void Insert(OutlineTreeModelNode* child, unsigned int n) {
        m_children.Insert(child, n);
    }
    
    inline void Append(OutlineTreeModelNode* child) {
        m_children.push_back(child);
    }
    
    inline int GetChildCount() const {
        return m_children.GetCount();
    }
};

class OutlineTreeModel : public wxDataViewModel {
private:
    // pointers to some "special" nodes of the tree:
    OutlineTreeModelNode* m_research;
    OutlineTreeModelNode* m_characters;
    OutlineTreeModelNode* m_locations;
    OutlineTreeModelNode* m_items;

    OulineTreeModelNodePtrArray m_otherRoots{};

public:
    OutlineTreeModel();
    ~OutlineTreeModel() {
        if (m_research)
            delete m_research;
        
        if (m_characters)
            delete m_characters;

        if (m_locations)
            delete m_locations;

        if (m_items)
            delete m_items;

        for (unsigned int i = 0; i < m_otherRoots.GetCount(); i++) {
            if (m_otherRoots.at(i))
                delete m_otherRoots.at(i);
        }
    }

    wxString GetTitle(const wxDataViewItem& item) const;
    wxRichTextBuffer& GetBuffer(const wxDataViewItem& item) const;

    OulineTreeModelNodePtrArray& GetCharacters() {
        return m_characters->GetChildren();
    }

    OulineTreeModelNodePtrArray& GetLocations() {
        return m_locations->GetChildren();
    }

    OulineTreeModelNodePtrArray& GetItems() {
        return m_items->GetChildren();
    }

    OutlineTreeModelNode* GetResearchNode() {
        return m_research;
    }

    OutlineTreeModelNode* GetCharactersNode() {
        return m_characters;
    }

    OutlineTreeModelNode* GetLocationsNode() {
        return m_locations;
    }

    OutlineTreeModelNode* GetItemsNode() {
        return m_items;
    }

    // helper methods to change the model

    wxDataViewItem AddToResearch(const wxString& title);
    wxDataViewItem AddToCharacters(const wxString& title);
    wxDataViewItem AddToLocations(const wxString& title);
    wxDataViewItem AddToItems(const wxString& title);

    wxDataViewItem AppendFile(wxDataViewItem& parent, const wxString& name, const wxRichTextBuffer& buffer);
    wxDataViewItem AppendFolder(wxDataViewItem& parent, const wxString& name);

    bool IsResearch(wxDataViewItem& item);
    bool IsCharacters(wxDataViewItem& item);
    bool IsLocations(wxDataViewItem& item);
    bool IsItems(wxDataViewItem& item);

    bool IsSpecial(wxDataViewItem& item);

    bool IsDescendant(wxDataViewItem& item, wxDataViewItem& descendant);

    void DeleteItem(const wxDataViewItem& item);

    void SetItemBackgroundColour(wxDataViewItem& item, wxColour& colour);
    void SetItemForegroundColour(wxDataViewItem& item, wxColour& colour);
    void SetItemFont(wxDataViewItem& item, wxFont& font);
        
    bool Reparent(OutlineTreeModelNode* item, OutlineTreeModelNode* newParent);
    bool Reparent(OutlineTreeModelNode* item, OutlineTreeModelNode* newParent, int n);

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

enum {
    TREE_Files,

    TOOL_NewFile,
    TOOL_NewFolder,

    MENU_ChangeItemFgColour,
    MENU_ChangeItemBgColour,
    MENU_DeleteItem,

    TIMER_OutlineFiles
};

class amOutlineFilesPanel : public wxSplitterWindow {
private:
    wxPanel* m_leftPanel = nullptr;
    wxDataViewCtrl* m_files = nullptr;
    wxRichTextCtrl* m_textCtrl = nullptr;
    wxRichTextAttr m_basicAttr{};

    wxToolBar* m_filesTB = nullptr, * m_contentTB = nullptr;

    wxObjectDataPtr<OutlineTreeModel> m_outlineTreeModel;

    OutlineTreeModelNode* m_currentNode = nullptr;

    OutlineTreeModelNode* m_nodeForDnD = nullptr;
    wxDataViewItem m_itemForDnD{};

    wxTimer m_timer{ this, TIMER_OutlineFiles };
    bool m_isSaving = false;

public:
    amOutlineFilesPanel(wxWindow* parent);
    void Init();

    wxObjectDataPtr<OutlineTreeModel> GetOutlineTreeModel() { return m_outlineTreeModel; }

    void GenerateCharacterBuffer(Character& character, wxRichTextBuffer& buffer);
    void GenerateLocationBuffer(Location& location, wxRichTextBuffer& buffer);
    void GenerateItemBuffer(Item& item, wxRichTextBuffer& buffer);
    void AppendCharacter(Character& character);
    void AppendLocation(Location& location);
    void AppendItem(Item& item);

    void DeleteCharacter(Character& character);
    void DeleteLocation(Location& location);
    void DeleteItem(Item& item);

    void NewFile(wxCommandEvent& event);
    void NewFolder(wxCommandEvent& event);

    void DeleteItem(wxDataViewItem& item);

    void OnKeyDownDataView(wxKeyEvent& event);
    void OnRightDownDataView(wxMouseEvent& event);
    void OnMenuDataView(wxCommandEvent& event);

    void OnSelectionChanged(wxDataViewEvent& event);
    void OnEditingStart(wxDataViewEvent& event);
    void OnEditingEnd(wxDataViewEvent& event);
    
    void OnItemExpanded(wxDataViewEvent& event);
    void OnItemCollapsed(wxDataViewEvent& event);

    void OnBeginDrag(wxDataViewEvent& event);
    void OnDropPossible(wxDataViewEvent& event);
    void OnDrop(wxDataViewEvent& event);

    // Prevent wxWidgets from unsplitting sidebar
    virtual void OnDoubleClickSash(int x, int y) {}

    void OnTimerEvent(wxTimerEvent& event);
    void SaveCurrentBuffer();

    wxXmlNode* SerializeFolder(wxDataViewItem& item);
    wxXmlNode* SerializeFile(wxDataViewItem& item);
    void DeserializeNode(wxXmlNode* node, wxDataViewItem& parent);
    bool Save();
    bool Load(wxStringInputStream& stream);

    void ClearAll();

    DECLARE_EVENT_TABLE()
};

#endif