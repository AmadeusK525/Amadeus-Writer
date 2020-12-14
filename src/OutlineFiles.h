#ifndef OUTLINEFILES_H_
#define OUTLINEFILES_H_
#pragma once

#include <wx\splitter.h>
#include <wx\dataview.h>
#include <wx\toolbar.h>
#include <wx\richtext\richtextctrl.h>

#include "Character.h"
#include "Location.h"

using std::vector;
using std::string;

class OutlineTreeModelNode;
WX_DEFINE_ARRAY_PTR(OutlineTreeModelNode*, OulineTreeModelNodePtrArray);

class OutlineTreeModelNode {
private:
    OutlineTreeModelNode* m_parent = nullptr;
    OulineTreeModelNodePtrArray   m_children{};

    wxDataViewItemAttr attr{};

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
            m_parent->append(this);

        m_isContainer = false;

        attr.SetBackgroundColour(wxColour(250, 250, 250));
        attr.SetColour(wxColour(20, 20, 20));
        attr.SetBold(false);
        attr.SetItalic(false);
        attr.SetStrikethrough(false);
    }

    OutlineTreeModelNode(OutlineTreeModelNode* parent,
        const wxString& branch) {
        m_parent = parent;
        m_title = branch;

        if (m_parent)
            m_parent->append(this);

        m_isContainer = true;

        attr.SetBackgroundColour(wxColour(250, 250, 250));
        attr.SetColour(wxColour(20, 20, 20));
        attr.SetBold(false);
        attr.SetItalic(false);
        attr.SetStrikethrough(false);
    }

    ~OutlineTreeModelNode() {
        for (size_t i = 0; i < m_children.GetCount(); i++) {
            OutlineTreeModelNode* child = m_children[i];

            if (child)
                delete child;
        }
    }

    bool isContainer() const {
        return m_isContainer;
    }

    void reparent(OutlineTreeModelNode* newParent) {
        if (m_parent)
            m_parent->getChildren().Remove(this);
        
        m_parent = newParent;
        
        if (m_parent)
            m_parent->append(this);
    }

    void reparent(OutlineTreeModelNode* newParent, int n) {
        if (m_parent)
            m_parent->getChildren().Remove(this);

        m_parent = newParent;
        
        if (m_parent)
            m_parent->insert(this, n);
    }

    void reposition(int n) {
        if (m_parent)
            m_parent->getChildren().Remove(this);
        else
            return;

        m_parent->getChildren().Insert(this, n);
    }

    OutlineTreeModelNode* getParent() {
        return m_parent;
    }

    OulineTreeModelNodePtrArray& getChildren() {
        return m_children;
    }

    OutlineTreeModelNode* getChild(unsigned int n) {
        return m_children.Item(n);
    }

    wxDataViewItemAttr& getAttr() {
        return attr;
    }

    void insert(OutlineTreeModelNode* child, unsigned int n) {
        m_children.Insert(child, n);
    }
    
    void append(OutlineTreeModelNode* child) {
        m_children.Add(child);
    }
    
    unsigned int getChildCount() const {
        return m_children.GetCount();
    }
};

class OutlineTreeModel : public wxDataViewModel {
private:
    // pointers to some "special" nodes of the tree:
    OutlineTreeModelNode* m_research;
    OutlineTreeModelNode* m_characters;
    OutlineTreeModelNode* m_locations;

    OulineTreeModelNodePtrArray otherRoots{};

public:
    OutlineTreeModel();
    ~OutlineTreeModel() {
        if (m_research)
            delete m_research;
        
        if (m_characters)
            delete m_characters;

        if (m_locations)
            delete m_locations;

        for (int i = 0; i < otherRoots.GetCount(); i++) {
            if (otherRoots.at(i))
                delete otherRoots.at(i);
        }
    }

    wxString getTitle(const wxDataViewItem& item) const;
    wxRichTextBuffer& getBuffer(const wxDataViewItem& item) const;

    OulineTreeModelNodePtrArray& getCharacters() {
        return m_characters->getChildren();
    }

    OulineTreeModelNodePtrArray& getLocations() {
        return m_locations->getChildren();
    }

    OutlineTreeModelNode* getResearchNode() {
        return m_research;
    }

    OutlineTreeModelNode* getCharactersNode() {
        return m_characters;
    }

    OutlineTreeModelNode* getLocationsNode() {
        return m_locations;
    }

    // helper methods to change the model

    wxDataViewItem addToResearch(const string& title);
    wxDataViewItem addToCharacters(const string& title);
    wxDataViewItem addToLocations(const string& title);

    wxDataViewItem appendFile(wxDataViewItem& parent, const string& name, const wxRichTextBuffer& buffer);
    wxDataViewItem appendFolder(wxDataViewItem& parent, const string& name);

    bool isResearch(wxDataViewItem& item);
    bool isCharacters(wxDataViewItem& item);
    bool isLocations(wxDataViewItem& item);

    void deleteItem(const wxDataViewItem& item);

    void setItemBackgroundColour(wxDataViewItem& item, wxColour& colour);
    void setItemForegroundColour(wxDataViewItem& item, wxColour& colour);
    void setItemFont(wxDataViewItem& item, wxFont& font);
        
    bool reparent(OutlineTreeModelNode* item, OutlineTreeModelNode* newParent);
    bool reparent(OutlineTreeModelNode* item, OutlineTreeModelNode* newParent, int n);

    bool reposition(wxDataViewItem& item, int n);

    void clear();

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

class OutlineFilesPanel : public wxSplitterWindow {
private:
    wxPanel* leftPanel = nullptr;
    wxDataViewCtrl* files = nullptr;
    wxRichTextCtrl* content = nullptr;
    wxRichTextAttr basicAttr;

    wxToolBar* filesTB = nullptr, * contentTB = nullptr;

    wxObjectDataPtr<OutlineTreeModel> outlineTreeModel;
    OutlineTreeModelNode* nodeForDnD = nullptr;
    wxDataViewItem itemForDnD{};

    std::string current{ "" };

public:
    OutlineFilesPanel(wxWindow* parent);
    void init();

    void appendCharacter(Character& character);
    void appendLocation(Location& location);

    void deleteCharacter(Character& character);
    void deleteLocation(Location& location);

    void newFile(wxCommandEvent& event);
    void newFolder(wxCommandEvent& event);

    void deleteItem(wxDataViewItem& item);

    void onKeyDownDataView(wxKeyEvent& event);
    void onRightDownDataView(wxMouseEvent& event);
    void onMenuDataView(wxCommandEvent& event);

    void onSelectionChanged(wxDataViewEvent& event);
    void onEditingStart(wxDataViewEvent& event);
    void onEditingEnd(wxDataViewEvent& event);
    
    void onBeginDrag(wxDataViewEvent& event);
    void onDropPossible(wxDataViewEvent& event);
    void onDrop(wxDataViewEvent& event);

    virtual void OnUnsplit(wxWindow* window);

    wxXmlNode* getNodeWithChildren(wxDataViewItem& item);
    void deserializeNode(wxXmlNode* node, wxDataViewItem& parent);
    bool save();
    bool load();

    void clearAll();

    DECLARE_EVENT_TABLE()
};

enum {
    TREE_Files,

    TOOL_NewFile,
    TOOL_NewFolder,

    MENU_DeleteItem
};

#endif