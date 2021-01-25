#ifndef OUTLINEFILES_H_
#define OUTLINEFILES_H_
#pragma once

#include <wx\splitter.h>
#include <wx\dataview.h>
#include <wx\toolbar.h>
#include <wx\richtext\richtextctrl.h>

#include "Elements.h"

using std::vector;
using std::string;

class OutlineTreeModelNode;
WX_DEFINE_ARRAY_PTR(OutlineTreeModelNode*, OulineTreeModelNodePtrArray);

class OutlineTreeModelNode {
private:
    OutlineTreeModelNode* m_parent = nullptr;
    OulineTreeModelNodePtrArray m_children{};

    wxDataViewItemAttr m_attr{};

public:     // public to avoid getters/setters
    wxRichTextBuffer m_buffer{};
    string m_title{};
    bool m_isContainer = true;

public:
    OutlineTreeModelNode(OutlineTreeModelNode* parent,
        const wxString& title, const wxRichTextBuffer& buffer) {
        m_parent = parent;
        m_title = title;
        m_buffer = buffer;

        if (m_parent)
            m_parent->Append(this);

        m_isContainer = false;

        m_attr.SetBackgroundColour(wxColour(250, 250, 250));
        m_attr.SetColour(wxColour(20, 20, 20));
        m_attr.SetBold(false);
        m_attr.SetItalic(false);
        m_attr.SetStrikethrough(false);
    }

    OutlineTreeModelNode(OutlineTreeModelNode* parent,
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

    ~OutlineTreeModelNode() {
        for (size_t i = 0; i < m_children.GetCount(); i++) {
            OutlineTreeModelNode* child = m_children[i];

            if (child)
                delete child;
        }
    }

    bool IsContainer() const {
        return m_isContainer;
    }

    void Reparent(OutlineTreeModelNode* newParent) {
        if (m_parent)
            m_parent->GetChildren().Remove(this);
        
        m_parent = newParent;
        
        if (m_parent)
            m_parent->Append(this);
    }

    void Reparent(OutlineTreeModelNode* newParent, int n) {
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

    OutlineTreeModelNode* GetParent() {
        return m_parent;
    }

    OulineTreeModelNodePtrArray& GetChildren() {
        return m_children;
    }

    OutlineTreeModelNode* GetChild(unsigned int n) {
        return m_children.Item(n);
    }

    wxDataViewItemAttr& GetAttr() {
        return m_attr;
    }

    void Insert(OutlineTreeModelNode* child, unsigned int n) {
        m_children.Insert(child, n);
    }
    
    void Append(OutlineTreeModelNode* child) {
        m_children.push_back(child);
    }
    
    int GetChildCount() const {
        return m_children.GetCount();
    }
};

class OutlineTreeModel : public wxDataViewModel {
private:
    // pointers to some "special" nodes of the tree:
    OutlineTreeModelNode* m_research;
    OutlineTreeModelNode* m_characters;
    OutlineTreeModelNode* m_locations;

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

        for (int i = 0; i < m_otherRoots.GetCount(); i++) {
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

    OutlineTreeModelNode* GetResearchNode() {
        return m_research;
    }

    OutlineTreeModelNode* GetCharactersNode() {
        return m_characters;
    }

    OutlineTreeModelNode* GetLocationsNode() {
        return m_locations;
    }

    // helper methods to change the model

    wxDataViewItem AddToResearch(const string& title);
    wxDataViewItem AddToCharacters(const string& title);
    wxDataViewItem AddToLocations(const string& title);

    wxDataViewItem AppendFile(wxDataViewItem& parent, const string& name, const wxRichTextBuffer& buffer);
    wxDataViewItem AppendFolder(wxDataViewItem& parent, const string& name);

    bool IsResearch(wxDataViewItem& item);
    bool IsCharacters(wxDataViewItem& item);
    bool IsLocations(wxDataViewItem& item);

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
        return "string";
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

class amdOutlineFilesPanel : public wxSplitterWindow {
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

public:
    amdOutlineFilesPanel(wxWindow* parent);
    void Init();

    void GenerateCharacterBuffer(Character& character, wxRichTextBuffer& buffer);
    void GenerateLocationBuffer(Location& location, wxRichTextBuffer& buffer);
    void AppendCharacter(Character& character);
    void AppendLocation(Location& location);

    void DeleteCharacter(Character& character);
    void DeleteLocation(Location& location);

    void NewFile(wxCommandEvent& event);
    void NewFolder(wxCommandEvent& event);

    void DeleteItem(wxDataViewItem& item);

    void OnKeyDownDataView(wxKeyEvent& event);
    void OnRightDownDataView(wxMouseEvent& event);
    void OnMenuDataView(wxCommandEvent& event);

    void OnSelectionChanged(wxDataViewEvent& event);
    void OnEditingStart(wxDataViewEvent& event);
    void OnEditingEnd(wxDataViewEvent& event);
    
    void OnBeginDrag(wxDataViewEvent& event);
    void OnDropPossible(wxDataViewEvent& event);
    void OnDrop(wxDataViewEvent& event);

    virtual void OnUnsplit(wxWindow* window);

    void OnTimerEvent(wxTimerEvent& event);
    void SaveCurrentBuffer();

    wxXmlNode* SerializeFolder(wxDataViewItem& item);
    wxXmlNode* SerializeFile(wxDataViewItem& item);
    void DeserializeNode(wxXmlNode* node, wxDataViewItem& parent);
    bool Save();
    bool Load();

    void ClearAll();

    DECLARE_EVENT_TABLE()
};

#endif