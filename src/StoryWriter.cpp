#include "StoryWriter.h"
#include "ElementsNotebook.h"
#include "Outline.h"
#include "UtilityClasses.h"

#include <wx\popupwin.h>
#include <wx\listbox.h>
#include <wx\richtext\richtextxml.h>
#include <wx\dcbuffer.h>
#include <wx\dcgraph.h>
#include <wx\splitter.h>
#include <wx\wfstream.h>

#include <sstream>

#include "wxmemdbg.h"


StoryTreeModel::StoryTreeModel() {
    StoryTreeModelNode::InitAllIcons();
}

bool StoryTreeModel::Load() {
    return false;
}

bool StoryTreeModel::Save() {
    return false;
}

void StoryTreeModel::CreateFromScratch(Book& book) {}

wxString StoryTreeModel::GetTitle(const wxDataViewItem& item) const {
    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
    if (!node)
        return wxEmptyString;

    return node->GetTitle();
}

wxDataViewItem StoryTreeModel::AddSection(const wxString& name, int id, int pos) {
    StoryTreeModelNode* node = new StoryTreeModelNode(m_book, name, pos, id, STMN_Section);
    wxDataViewItem item(node);

    ItemAdded(wxDataViewItem(m_book), item);

    return item;
}

wxDataViewItem StoryTreeModel::AddChapter(wxDataViewItem& section, const wxString& name, int id, int pos) {
    StoryTreeModelNode* node = new StoryTreeModelNode((StoryTreeModelNode*)section.GetID(), name, pos, id, STMN_Chapter);
    wxDataViewItem item(node);

    ItemAdded(section, item);

    return item;
}

wxDataViewItem StoryTreeModel::AddScene(wxDataViewItem& chapter, const wxString& name, int id, int pos) {
    StoryTreeModelNode* node = new StoryTreeModelNode((StoryTreeModelNode*)chapter.GetID(), name, pos, id, STMN_Scene);
    wxDataViewItem item(node);

    ItemAdded(chapter, item);

    return item;
}

bool StoryTreeModel::IsDescendant(wxDataViewItem& item, wxDataViewItem& descendant) {
    wxDataViewItem& parent = GetParent(descendant);

    bool is = false;

    while (parent.IsOk()) {
        if (parent.GetID() == item.GetID())
            is = true;

        parent = GetParent(parent);
    }

    return is;
}

void StoryTreeModel::MoveToTrash(const wxDataViewItem& item) {
}

void StoryTreeModel::DeleteItem(const wxDataViewItem& item) {
    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
    if (!node)      // happens if item.IsOk()==false
        return;

    if (node == m_book || node == m_trash) {
        wxMessageBox(_("Cannot remove special folders!"));
        return;
    }
    // first remove the node from the parent's array of children;
    // NOTE: StoryTreeModelNode is only an array of _pointers_
    //       thus removing the node from it doesn't result in freeing it

    wxDataViewItemArray children;
    GetChildren(item, children);

    for (unsigned int i = 0; i < children.GetCount(); i++)
        DeleteItem(children[i]);

    StoryTreeModelNode* parentNode = node->GetParent();
    if (parentNode)
        parentNode->GetChildren().Remove(node);
    else
        m_otherRoots.Remove(node);

    wxDataViewItem parent(parentNode);

    // free the node
    delete node;

    ItemDeleted(parent, item);
}

void StoryTreeModel::SetItemBackgroundColour(wxDataViewItem& item, wxColour& colour) {
    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
    wxDataViewItemAttr& attr = node->GetAttr();

    attr.SetBackgroundColour(colour);
}

void StoryTreeModel::SetItemForegroundColour(wxDataViewItem& item, wxColour& colour) {
    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
    wxDataViewItemAttr& attr = node->GetAttr();

    attr.SetColour(colour);
}

void StoryTreeModel::SetItemFont(wxDataViewItem& item, wxFont& font) {
    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
    wxDataViewItemAttr attr = node->GetAttr();

    attr.SetBold(font.GetWeight() >= 400);
    attr.SetItalic(font.GetStyle() == wxFONTSTYLE_ITALIC);
    attr.SetStrikethrough(font.GetStrikethrough());
}

bool StoryTreeModel::Reparent(StoryTreeModelNode* itemNode, StoryTreeModelNode* newParentNode) {
    StoryTreeModelNode* oldParentNode(itemNode->GetParent());

    wxDataViewItem item((void*)itemNode);
    wxDataViewItem newParent((void*)newParentNode);
    wxDataViewItem oldParent((void*)oldParentNode);


    if (!itemNode)
        return false;

    itemNode->Reparent(newParentNode);

    if (!oldParentNode)
        m_otherRoots.Remove(itemNode);

    if (!newParentNode)
        m_otherRoots.Add(itemNode);

    ItemAdded(newParent, item);
    ItemDeleted(oldParent, item);
    return true;
}

bool StoryTreeModel::Reparent(StoryTreeModelNode* itemNode, StoryTreeModelNode* newParentNode, int n) {
    StoryTreeModelNode* oldParentNode(itemNode->GetParent());

    wxDataViewItem item((void*)itemNode);
    wxDataViewItem newParent((void*)newParentNode);
    wxDataViewItem oldParent((void*)oldParentNode);


    if (!itemNode)
        return false;

    itemNode->Reparent(newParentNode, n);

    if (!oldParentNode)
        m_otherRoots.Remove(itemNode);

    if (!newParentNode)
        m_otherRoots.Insert(itemNode, n);

    ItemAdded(newParent, item);
    ItemDeleted(oldParent, item);
    return true;
}

bool StoryTreeModel::Reposition(wxDataViewItem& item, int n) {
    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

    if (node) {
        node->Reposition(n);
        ItemChanged(wxDataViewItem(node->GetParent()));
    } else
        return false;

    return true;
}

void StoryTreeModel::Clear() {
    StoryTreeModelNodePtrArray array;

    array = m_book->GetChildren();
    for (unsigned int i = 0; i < array.GetCount(); i++) {
        DeleteItem(wxDataViewItem(array[i]));
    }

    array = m_trash->GetChildren();
    for (unsigned int i = 0; i < array.GetCount(); i++) {
        DeleteItem(wxDataViewItem(array[i]));
    }
}

void StoryTreeModel::GetValue(wxVariant& variant,
    const wxDataViewItem& item, unsigned int col) const {
    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

    wxDataViewIconText it;
    it.SetText(node->GetTitle());

    /*if (node->IsBook())
        it.SetIcon(node->m_icons[0]);
    else if (node->IsSection())
        it.SetIcon(node->m_icons[1]);
    else if (node->IsChapter())
        it.SetIcon(node->m_icons[2]);*/

    switch (col) {
    case 0:
        variant << it;
        break;
    default:
        break;
    }
}

bool StoryTreeModel::SetValue(const wxVariant& variant,
    const wxDataViewItem& item, unsigned int col) {

    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
    wxDataViewIconText it;
    it << variant;

    switch (col) {
    case 0:
        node->SetTitle(it.GetText());
        return true;

    default:
        break;
    }
    return false;
}

wxDataViewItem StoryTreeModel::GetParent(const wxDataViewItem& item) const {
    // the invisible root node has no parent
    if (!item.IsOk())
        return wxDataViewItem(0);

    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
    return wxDataViewItem(node->GetParent());
}

bool StoryTreeModel::IsContainer(const wxDataViewItem& item) const {
    // the invisible root node can have children
    if (!item.IsOk())
        return true;

    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

    if (node)
        return node->IsContainer();
    else
        return false;
}

bool StoryTreeModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const {
    StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

    if (node)
        attr = node->GetAttr();

    return true;
}

unsigned int StoryTreeModel::GetChildren(const wxDataViewItem& parent,
    wxDataViewItemArray& array) const {
    StoryTreeModelNode* node = (StoryTreeModelNode*)parent.GetID();
    if (!node) {
        int n = 0;

        if (m_book) {
            array.Add(wxDataViewItem(m_book));
            n++;
        }

        if (m_trash) {
            array.Add(wxDataViewItem(m_trash));
            n++;
        }

        for (unsigned int i = 0; i < m_otherRoots.GetCount(); i++) {
            array.Add(wxDataViewItem(m_otherRoots.at(i)));
        }

        return n + m_otherRoots.GetCount();
    }

    int count = node->GetChildCount();

    if (count == 0) {
        return 0;
    }

    for (int pos = 0; pos < count; pos++) {
        StoryTreeModelNode* child = node->GetChildren().Item(pos);
        array.Add(wxDataViewItem(child));
    }

    return count;
}


///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// amStoryWriter ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(amStoryWriter, wxFrame)

EVT_BUTTON(BUTTON_NoteClear, amStoryWriter::ClearNote)
EVT_BUTTON(BUTTON_NoteAdd, amStoryWriter::AddNote)

EVT_BUTTON(BUTTON_AddChar, amStoryWriter::OnAddCharacter)
EVT_BUTTON(BUTTON_AddLoc, amStoryWriter::OnAddLocation)
EVT_BUTTON(BUTTON_AddItem, amStoryWriter::OnAddItem)
EVT_BUTTON(BUTTON_RemChar, amStoryWriter::OnRemoveCharacter)
EVT_BUTTON(BUTTON_RemLoc, amStoryWriter::OnRemoveLocation)
EVT_BUTTON(BUTTON_RemItem, amStoryWriter::OnRemoveItem)

EVT_BUTTON(BUTTON_NextChap, amStoryWriter::OnNextChapter)
EVT_BUTTON(BUTTON_PreviousChap, amStoryWriter::OnPreviousChapter)

EVT_TIMER(TIMER_Save, amStoryWriter::OnTimerEvent)

EVT_CLOSE(amStoryWriter::OnClose)

END_EVENT_TABLE()

amStoryWriter::amStoryWriter(wxWindow* parent, amProjectManager* manager, int numb) :
    m_saveTimer(this, TIMER_Save),
    wxFrame(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
    wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {

    m_manager = manager;
    m_chapterPos = numb;

    Hide();

    amSplitterWindow* rightSplitter = new amSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize,
        1024L | wxSP_LIVE_UPDATE | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);
    amSplitterWindow* leftSplitter = new amSplitterWindow(rightSplitter, -1, wxDefaultPosition, wxDefaultSize,
        1024L | wxSP_LIVE_UPDATE | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);
    leftSplitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGED, &amStoryWriter::OnLeftSplitterChanged, this);

    leftSplitter->SetBackgroundColour(wxColour(20, 20, 20));
    rightSplitter->SetBackgroundColour(wxColour(20, 20, 20));

    leftSplitter->SetMinimumPaneSize(30);
    rightSplitter->SetMinimumPaneSize(30);

    m_cwNotebook = new amStoryWriterNotebook(leftSplitter, this);

    wxNotebook* leftNotebook = new wxNotebook(leftSplitter, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_leftPanel = new wxPanel(leftNotebook, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_leftPanel->SetBackgroundColour(wxColour(50, 50, 50));

    m_characterPanel = new wxPanel(m_leftPanel);
    m_characterPanel->SetBackgroundColour(wxColour(80, 80, 80));
    
    wxStaticText* charInChapLabel = new wxStaticText(m_characterPanel, -1, "Characters present in chapter",
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
    charInChapLabel->SetBackgroundColour(wxColour(25, 25, 25));
    charInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
    charInChapLabel->SetFont(wxFontInfo(10).Bold());
    charInChapLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_charInChap = new wxListView(m_characterPanel, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_HRULES | wxLC_REPORT | wxLC_NO_HEADER | wxBORDER_NONE);
    m_charInChap->SetMinSize(FromDIP(wxSize(150, 150)));

    m_charInChap->SetBackgroundColour(wxColour(35, 35, 35));
    m_charInChap->SetForegroundColour(wxColour(230, 230, 230));
    m_charInChap->InsertColumn(0, "Characters present in chapter", wxLIST_ALIGN_LEFT, FromDIP(10));
    m_charInChap->SetMinSize(wxSize(15, 15));

    wxButton* addCharButton = new wxButton(m_characterPanel, BUTTON_AddChar, "Add", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    addCharButton->SetBackgroundColour(wxColour(60, 60, 60));
    addCharButton->SetForegroundColour(wxColour(255, 255, 255));
    wxButton* remCharButton = new wxButton(m_characterPanel, BUTTON_RemChar, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    remCharButton->SetBackgroundColour(wxColour(60, 60, 60));
    remCharButton->SetForegroundColour(wxColour(255, 255, 255));

    wxBoxSizer* charBSizer = new wxBoxSizer(wxHORIZONTAL);
    charBSizer->Add(addCharButton);
    charBSizer->AddStretchSpacer();
    charBSizer->Add(remCharButton);

    wxBoxSizer* charactersSizer = new wxBoxSizer(wxVERTICAL);
    charactersSizer->Add(charInChapLabel, wxSizerFlags(0).Expand());
    charactersSizer->Add(m_charInChap, wxSizerFlags(1).Expand());
    charactersSizer->Add(charBSizer, wxSizerFlags(0).Expand());
    m_characterPanel->SetSizer(charactersSizer);

    m_locationPanel = new wxPanel(m_leftPanel, -1);
    m_locationPanel->SetBackgroundColour(wxColour(80, 80, 80));

    wxStaticText* locInChapLabel = new wxStaticText(m_locationPanel, -1, "Locations present in chapter",
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
    locInChapLabel->SetBackgroundColour(wxColour(25, 25, 25));
    locInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
    locInChapLabel->SetFont(wxFontInfo(10).Bold());
    locInChapLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_locInChap = new wxListView(m_locationPanel, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_HRULES | wxLC_REPORT | wxLC_NO_HEADER | wxBORDER_NONE);

    m_locInChap->SetMinSize(FromDIP(wxSize(150, 150)));
    m_locInChap->SetBackgroundColour(wxColour(35, 35, 35));
    m_locInChap->SetForegroundColour(wxColour(230, 230, 230));
    m_locInChap->InsertColumn(0, "Locations present in chapter", wxLIST_ALIGN_LEFT, FromDIP(155));

    wxButton* addLocButton = new wxButton(m_locationPanel, BUTTON_AddLoc, "Add", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    addLocButton->SetBackgroundColour(wxColour(60, 60, 60));
    addLocButton->SetForegroundColour(wxColour(255, 255, 255));
    wxButton* remLocButton = new wxButton(m_locationPanel, BUTTON_RemLoc, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    remLocButton->SetBackgroundColour(wxColour(60, 60, 60));
    remLocButton->SetForegroundColour(wxColour(255, 255, 255));

    wxBoxSizer* locBSizer = new wxBoxSizer(wxHORIZONTAL);
    locBSizer->Add(addLocButton);
    locBSizer->AddStretchSpacer();
    locBSizer->Add(remLocButton);

    wxBoxSizer* locationsSizer = new wxBoxSizer(wxVERTICAL);
    locationsSizer->Add(locInChapLabel, wxSizerFlags(0).Expand());
    locationsSizer->Add(m_locInChap, wxSizerFlags(1).Expand());
    locationsSizer->Add(locBSizer, wxSizerFlags(0).Expand());
    m_locationPanel->SetSizer(locationsSizer);

    m_itemPanel = new wxPanel(m_leftPanel, -1);
    m_itemPanel->SetBackgroundColour(wxColour(80, 80, 80));

    wxStaticText* itemsInChapLabel = new wxStaticText(m_itemPanel, -1, "Items present in chapter",
        wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
    itemsInChapLabel->SetBackgroundColour(wxColour(25, 25, 25));
    itemsInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
    itemsInChapLabel->SetFont(wxFontInfo(10).Bold());
    itemsInChapLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_itemsInChap = new wxListView(m_itemPanel, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_HRULES | wxLC_REPORT | wxLC_NO_HEADER | wxBORDER_NONE);
    m_itemsInChap->SetMinSize(FromDIP(wxSize(150, 150)));

    m_itemsInChap->SetBackgroundColour(wxColour(35, 35, 35));
    m_itemsInChap->SetForegroundColour(wxColour(230, 230, 230));
    m_itemsInChap->InsertColumn(0, "Items present in chapter", wxLIST_ALIGN_LEFT, FromDIP(155));

    wxButton* addItemButton = new wxButton(m_itemPanel, BUTTON_AddItem, "Add", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    addItemButton->SetBackgroundColour(wxColour(60, 60, 60));
    addItemButton->SetForegroundColour(wxColour(255, 255, 255));
    wxButton* remItemButton = new wxButton(m_itemPanel, BUTTON_RemItem, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    remItemButton->SetBackgroundColour(wxColour(60, 60, 60));
    remItemButton->SetForegroundColour(wxColour(255, 255, 255));

    wxBoxSizer* itemBSizer = new wxBoxSizer(wxHORIZONTAL);
    itemBSizer->Add(addItemButton);
    itemBSizer->AddStretchSpacer();
    itemBSizer->Add(remItemButton);

    wxBoxSizer* itemsSizer = new wxBoxSizer(wxVERTICAL);
    itemsSizer->Add(itemsInChapLabel, wxSizerFlags(0).Expand());
    itemsSizer->Add(m_itemsInChap, wxSizerFlags(1).Expand());
    itemsSizer->Add(itemBSizer, wxSizerFlags(0).Expand());
    m_itemPanel->SetSizer(itemsSizer);

    wxButton* leftButton = new wxButton(m_leftPanel, BUTTON_PreviousChap, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
    leftButton->SetBitmap(wxBITMAP_PNG(arrowLeft));

    m_leftSizer = new wxBoxSizer(wxVERTICAL);
    m_leftSizer->Add(m_characterPanel, wxSizerFlags(1).Expand().Border(wxALL, 7));
    m_leftSizer->Add(m_locationPanel, wxSizerFlags(1).Expand().Border(wxALL, 7));
    m_leftSizer->Add(m_itemPanel, wxSizerFlags(1).Expand().Border(wxALL, 7));
    m_leftSizer->Add(leftButton, wxSizerFlags(0).Right().Border(wxRIGHT | wxBOTTOM, 8));

    m_leftPanel->SetSizer(m_leftSizer);

    m_outlineView = new wxDataViewCtrl(leftNotebook, -1,wxDefaultPosition, wxDefaultSize,
		wxDV_NO_HEADER | wxDV_SINGLE | wxBORDER_NONE);
    
    wxDataViewColumn* columnF = new wxDataViewColumn(_("Files"), new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType(),
        wxDATAVIEW_CELL_EDITABLE), 0, FromDIP(200), wxALIGN_LEFT);
    m_outlineView->AppendColumn(columnF);

    m_outlineTreeModel = m_manager->GetOutline()->GetOutlineFiles()->GetOutlineTreeModel();
    m_outlineView->AssociateModel(m_outlineTreeModel.get());
    m_outlineView->SetBackgroundColour(wxColour(255, 255, 255));

    m_storyView = new wxDataViewCtrl(leftNotebook, -1, wxDefaultPosition, wxDefaultSize,
        wxDV_NO_HEADER | wxDV_SINGLE | wxBORDER_NONE);

    wxDataViewColumn* columnS = new wxDataViewColumn(_("Story"), new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType(),
        wxDATAVIEW_CELL_EDITABLE), 0, FromDIP(200), wxALIGN_LEFT);
    m_storyView->AppendColumn(columnS);

    m_storyTreeModel = new StoryTreeModel();
    m_storyView->AssociateModel(m_storyTreeModel.get());

    if (!m_storyTreeModel->Load())
        m_storyTreeModel->CreateFromScratch(Book());

    //m_outlineView->GetMainWindow()->SetBackgroundColour(wxColour(255, 255, 255));
    //m_outlineView->GetMainWindow()->SetForegroundColour(wxColour(255, 255, 255));
    m_outlineView->SetBackgroundColour(wxColour(255, 255, 255));

    leftNotebook->AddPage(m_leftPanel,"Elements");
    leftNotebook->AddPage(m_outlineView, "Outline");
    leftNotebook->AddPage(m_storyView, "Story");

    wxPanel* rightPanel = new wxPanel(rightSplitter, -1);
    rightPanel->SetBackgroundColour(wxColour(60, 60, 60));

    m_summary = new wxTextCtrl(rightPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxBORDER_NONE);
    m_summary->SetBackgroundColour(wxColour(35, 35, 35));
    m_summary->SetForegroundColour(wxColour(245, 245, 245));
    m_summary->SetBackgroundStyle(wxBG_STYLE_PAINT);

    wxStaticText* sumLabel = new wxStaticText(rightPanel, -1, "Synopsys", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    sumLabel->SetBackgroundColour(wxColour(150, 0, 0));
    sumLabel->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased()));
    sumLabel->SetForegroundColour(wxColour(255, 255, 255));
    sumLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_noteChecker = new wxStaticText(rightPanel, -1, "Nothing to show.", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    m_noteChecker->SetBackgroundColour(wxColour(20, 20, 20));
    m_noteChecker->SetForegroundColour(wxColour(255, 255, 255));
    m_noteChecker->SetFont(wxFontInfo(10).Bold());
    m_noteChecker->SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_noteLabel = new wxTextCtrl(rightPanel, -1, "New note", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    m_noteLabel->SetBackgroundColour(wxColour(245, 245, 245));
    m_noteLabel->SetFont(wxFont(wxFontInfo(9)));
    m_noteLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_note = new wxTextCtrl(rightPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxBORDER_NONE);
    m_note->SetBackgroundColour(wxColour(255, 250, 205));
    m_note->SetFont(wxFont(wxFontInfo(9)));
    m_note->SetBackgroundStyle(wxBG_STYLE_PAINT);

    wxPanel* nbHolder = new wxPanel(rightPanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    nbHolder->SetBackgroundColour(wxColour(255, 250, 205));

    m_noteClear = new wxButton(nbHolder, BUTTON_NoteClear, "Clear");
    m_noteClear->SetBackgroundColour(wxColour(240, 240, 240));

    m_noteAdd = new wxButton(nbHolder, BUTTON_NoteAdd, "Add");
    m_noteAdd->SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* nbSizer = new wxBoxSizer(wxHORIZONTAL);
    nbSizer->Add(m_noteClear, wxSizerFlags(0).Border(wxALL, 2));
    nbSizer->AddStretchSpacer(1);
    nbSizer->Add(m_noteAdd, wxSizerFlags(0).Border(wxALL, 2));

    nbHolder->SetSizer(nbSizer);

    wxButton* rightButton = new wxButton(rightPanel, BUTTON_NextChap, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
    rightButton->SetBitmap(wxBITMAP_PNG(arrowRight));

    m_rightSizer = new wxBoxSizer(wxVERTICAL);
    m_rightSizer->Add(sumLabel, wxSizerFlags(0).Expand().Border(wxLEFT | wxTOP | wxRIGHT, 8));
    m_rightSizer->Add(m_summary, wxSizerFlags(2).Expand().Border(wxLEFT | wxRIGHT, 8));
    m_rightSizer->AddStretchSpacer(1);
    m_rightSizer->Add(m_noteChecker, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 8));
    m_rightSizer->Add(m_noteLabel, wxSizerFlags(0).Expand().Border(wxTOP | wxRIGHT | wxLEFT, 8));
    m_rightSizer->Add(m_note, wxSizerFlags(2).Expand().Border(wxRIGHT | wxLEFT, 8));
    m_rightSizer->Add(nbHolder, wxSizerFlags(0).Expand().Border(wxBOTTOM | wxRIGHT | wxLEFT, 8));
    m_rightSizer->Add(rightButton, wxSizerFlags(0).Left().Border(wxLEFT | wxBOTTOM, 8));

    rightPanel->SetSizer(m_rightSizer);

    //wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    //mainSizer->Add(leftNotebook, wxSizerFlags(1).Expand());
    //mainSizer->Add(m_cwNotebook, wxSizerFlags(4).Expand());
    //mainSizer->Add(rightPanel, wxSizerFlags(1).Expand());

    //SetSizer(mainSizer);

    SetSize(FromDIP(wxSize(1100, 700)));
    Maximize();

    leftSplitter->SplitVertically(leftNotebook, m_cwNotebook, FromDIP(230));
    rightSplitter->SplitVertically(leftSplitter, rightPanel, FromDIP(1140));
    leftSplitter->SetSashGravity(0.0);
    rightSplitter->SetSashGravity(1.0);

    wxMenuBar* menu = new wxMenuBar();
    wxMenu* menu1 = new wxMenu();
    menu->Append(menu1, "File");

    SetMenuBar(menu);

    m_statusBar = CreateStatusBar(3);
    m_statusBar->SetStatusText("Chapter up-to-date", 0);
    m_statusBar->SetStatusText("Number of words: 0", 1);
    m_statusBar->SetStatusText("Zoom: 100%", 2);
    m_statusBar->SetBackgroundColour(wxColour(120, 120, 120));

    wxSize noteSize((m_cwNotebook->GetTextCtrl()->GetSize()));
    m_cwNotebook->SetNoteSize(FromDIP(wxSize((noteSize.x / 3) - 30, (noteSize.y / 4) - 10)));

    SetIcon(wxICON(amadeus));

    LoadChapter();

    Show();
    SetFocus();

    m_saveTimer.Start(10000);
    OnLeftSplitterChanged(wxSplitterEvent());
}

void amStoryWriter::ClearNote(wxCommandEvent& event) {
    m_note->Clear();
    //m_noteLabel->SetValue("New note");

    event.Skip();
}

void amStoryWriter::AddNote(wxCommandEvent& event) {
    if (!m_note->IsEmpty()) {
        m_cwNotebook->AddNote(m_note->GetValue(), m_noteLabel->GetValue(), m_doGreenNote);
        m_note->Clear();
        m_noteLabel->SetValue("New note");
        m_doGreenNote = false;

        CheckNotes();
    }

    event.Skip();
}

void amStoryWriter::CheckNotes() {
    if (m_cwNotebook->GetNotes().size()) {
        if (m_cwNotebook->HasRedNote()) {
            m_noteChecker->SetBackgroundColour(wxColour(120, 0, 0));
            m_noteChecker->SetLabel("Unresolved notes!");
        } else {
            m_noteChecker->SetBackgroundColour(wxColour(0, 100, 0));
            m_noteChecker->SetLabel("All notes resolved!");
        }
    } else {
        m_noteChecker->SetBackgroundColour(wxColour(20, 20, 20));
        m_noteChecker->SetLabel("No notes yet.");
    }

    m_rightSizer->Layout();
}

void amStoryWriter::OnAddCharacter(wxCommandEvent& event) {
    wxPoint point(m_characterPanel->GetScreenPosition());

    int x = point.x;
    int y = point.y + m_charInChap->GetSize().y;
    
    wxPopupTransientWindow* win = new wxPopupTransientWindow(m_characterPanel, wxBORDER_NONE | wxPU_CONTAINS_CONTROLS);

    wxListBox* list = new wxListBox(win, -1, wxDefaultPosition, wxDefaultSize,
        m_manager->GetCharacterNames(), wxBORDER_SIMPLE | wxLB_NEEDED_SB | wxLB_SINGLE);
    list->Bind(wxEVT_LISTBOX_DCLICK, &amStoryWriter::AddCharacter, this);
    list->SetBackgroundColour(wxColour(55, 55, 55));
    list->SetForegroundColour(wxColour(255, 255, 255));

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(list, wxSizerFlags(1).Expand());
    win->SetSizer(siz);

    win->SetPosition(wxPoint(x, y));
    win->SetSize(m_characterPanel->GetSize());

    win->Popup(list);
    event.Skip();
}

void amStoryWriter::OnAddLocation(wxCommandEvent& event) {
    wxPoint point(m_locationPanel->GetScreenPosition());

    int x = point.x;
    int y = point.y + m_locInChap->GetSize().y;

    wxPopupTransientWindow* win = new wxPopupTransientWindow(m_locationPanel, wxBORDER_NONE | wxPU_CONTAINS_CONTROLS);

    wxListBox* list = new wxListBox(win, -1, wxDefaultPosition, wxDefaultSize,
        m_manager->GetLocationNames(), wxBORDER_SIMPLE | wxLB_NEEDED_SB | wxLB_SINGLE);
    list->Bind(wxEVT_LISTBOX_DCLICK, &amStoryWriter::AddLocation, this);
    list->SetBackgroundColour(wxColour(55, 55, 55));
    list->SetForegroundColour(wxColour(255, 255, 255));

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(list, 1, wxGROW);
    win->SetSizer(siz);

    win->SetPosition(wxPoint(x, y));
    win->SetSize(m_locationPanel->GetSize());

    win->Popup();
    event.Skip();
}

void amStoryWriter::OnAddItem(wxCommandEvent& event) {
    wxPoint point(m_itemPanel->GetScreenPosition());

    int x = point.x;
    int y = point.y + m_itemsInChap->GetSize().y;

    wxPopupTransientWindow* win = new wxPopupTransientWindow(m_itemPanel, wxBORDER_NONE | wxPU_CONTAINS_CONTROLS);

    wxListBox* list = new wxListBox(win, -1, wxDefaultPosition, wxDefaultSize,
        m_manager->GetItemNames(), wxBORDER_SIMPLE | wxLB_NEEDED_SB | wxLB_SINGLE);
    list->Bind(wxEVT_LISTBOX_DCLICK, &amStoryWriter::AddItem, this);
    list->SetBackgroundColour(wxColour(55, 55, 55));
    list->SetForegroundColour(wxColour(255, 255, 255));

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(list, wxSizerFlags(1).Expand());
    win->SetSizer(siz);

    win->SetPosition(wxPoint(x, y));
    win->SetSize(m_itemPanel->GetSize());

    win->Popup(list);
    event.Skip();
}

void amStoryWriter::AddCharacter(wxCommandEvent& event) {
    wxString name = event.GetString();
    if (m_charInChap->FindItem(-1, name) == -1) {
        CheckChapterValidity();
        m_manager->AddChapterToCharacter(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);
        UpdateCharacterList();
        m_manager->GetElementsNotebook()->UpdateCharacterList();
    }
}

void amStoryWriter::AddLocation(wxCommandEvent& event) {
    wxString name = event.GetString();
   
    if (m_locInChap->FindItem(-1, name) == -1) {
        CheckChapterValidity();
        m_manager->AddChapterToLocation(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);
        UpdateLocationList();
        m_manager->GetElementsNotebook()->UpdateLocationList();
    }
}

void amStoryWriter::AddItem(wxCommandEvent& event) {
    wxString name = event.GetString();

    if (m_itemsInChap->FindItem(-1, name) == -1) {
        CheckChapterValidity();
        m_manager->AddChapterToItem(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);
        UpdateItemList();
        m_manager->GetElementsNotebook()->UpdateItemList();
    }
}

void amStoryWriter::UpdateCharacterList() {
    m_charInChap->Freeze();
    m_charInChap->DeleteAllItems();
    
    int i = 0;
    for (wxString& character : m_manager->GetChapters(1, 1)[m_chapterPos - 1].characters)
        m_charInChap->InsertItem(i++, character);
  
    m_charInChap->Thaw();
}

void amStoryWriter::UpdateLocationList() {
    m_locInChap->Freeze();
    m_locInChap->DeleteAllItems();

    int i = 0;
    for (wxString& location : m_manager->GetChapters(1, 1)[m_chapterPos - 1].locations)
        m_locInChap->InsertItem(i++, location);
  
    m_locInChap->Thaw();
}

void amStoryWriter::UpdateItemList() {
    m_itemsInChap->Freeze();
    m_itemsInChap->DeleteAllItems();

    int i = 0;
    for (wxString& item : m_manager->GetChapters(1, 1)[m_chapterPos - 1].items)
        m_itemsInChap->InsertItem(i++, item);
    
    m_itemsInChap->Thaw();
}

void amStoryWriter::OnRemoveCharacter(wxCommandEvent& event) {
    int sel = m_charInChap->GetFirstSelected();
    int nos = m_charInChap->GetSelectedItemCount();

    CheckChapterValidity();

    for (int i = 0; i < nos; i++) {
        wxString name = m_charInChap->GetItemText(sel);
        m_charInChap->DeleteItem(sel);

        m_manager->RemoveChapterFromCharacter(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);

        sel = m_charInChap->GetNextSelected(sel - 1);
    }

    m_manager->GetElementsNotebook()->UpdateCharacterList();
    event.Skip();
}

void amStoryWriter::OnRemoveLocation(wxCommandEvent& event) {
    int sel = m_locInChap->GetFirstSelected();
    int nos = m_locInChap->GetSelectedItemCount();

    CheckChapterValidity();

    for (int i = 0; i < nos; i++) {
        wxString name = m_locInChap->GetItemText(sel);
        m_locInChap->DeleteItem(sel);

        m_manager->RemoveChapterFromLocation(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);

        sel = m_locInChap->GetNextSelected(sel + 1);
    }

    m_manager->GetElementsNotebook()->UpdateLocationList();
    event.Skip();
}

void amStoryWriter::OnRemoveItem(wxCommandEvent& event) {
    int sel = m_itemsInChap->GetFirstSelected();
    int nos = m_itemsInChap->GetSelectedItemCount();

    CheckChapterValidity();

    for (int i = 0; i < nos; i++) {
        wxString name = m_itemsInChap->GetItemText(sel);
        m_itemsInChap->DeleteItem(sel);

        m_manager->RemoveChapterFromItem(name, m_manager->GetChapters(1, 1)[m_chapterPos - 1]);

        sel = m_itemsInChap->GetNextSelected(sel + 1);
    }

    m_manager->GetElementsNotebook()->UpdateItemList();
    event.Skip();
}

void amStoryWriter::OnNextChapter(wxCommandEvent& event) {
    if (m_chapterPos < m_manager->GetChapterCount(1)) {
        SaveChapter();
        m_chapterPos++;
        m_cwNotebook->GetTextCtrl()->EndAllStyles();

        m_cwNotebook->GetNotes().clear();
        if (m_cwNotebook->GetNotesSizer()->GetItemCount() > 0)
            m_cwNotebook->GetNotesSizer()->Clear(true);

        LoadChapter();

        m_cwNotebook->GetCorkboard()->FitInside();
    }
    event.Skip();
}

void amStoryWriter::OnPreviousChapter(wxCommandEvent& event) {
    if (m_chapterPos > 1) {
        SaveChapter();
        m_chapterPos--;
        m_cwNotebook->GetNotes().clear();

        if (m_cwNotebook->GetNotesSizer()->GetItemCount() > 0) {
            m_cwNotebook->GetNotesSizer()->Clear(true);
        }

        LoadChapter();

        m_cwNotebook->GetCorkboard()->FitInside();
    }
    event.Skip();
}

void amStoryWriter::CheckChapterValidity() {
    // For now, if the stored chapter position is bigger than the chapter count,
    // it simply makes it so the current chapter is the last one. I'll make it something
    // else later.
    while (m_chapterPos > m_manager->GetChapterCount(1))
        m_chapterPos--;
}

void amStoryWriter::LoadChapter() {
    wxBusyCursor busy;

    Chapter& chapter = m_manager->GetChapters(1, 1)[m_chapterPos - 1];

    SetTitle(chapter.name);

    m_cwNotebook->Freeze();
    
    wxRichTextCtrl* textCtrl = m_cwNotebook->GetTextCtrl();
    chapter.scenes[0].content.SetBasicStyle(textCtrl->GetBasicStyle());
    
    textCtrl->GetBuffer() = chapter.scenes[0].content;
    textCtrl->GetBuffer().Invalidate(wxRICHTEXT_ALL);
    textCtrl->Refresh();

    m_summary->SetValue(chapter.synopsys);

    for (Note& note : chapter.notes) {
        m_note->SetValue(note.content);
        m_noteLabel->SetValue(note.name);
        m_doGreenNote = note.isDone;

        AddNote(wxCommandEvent());
    }

    m_cwNotebook->GetCorkboard()->Layout();
    m_cwNotebook->Thaw();

    UpdateCharacterList();
    UpdateLocationList();
    UpdateItemList();

    CheckNotes();
    CountWords();
    m_statusBar->SetStatusText("Chapter up-to-date", 0);
}

void amStoryWriter::SaveChapter() {
    CheckChapterValidity();
    Chapter& chapter = m_manager->GetChapters(1, 1)[m_chapterPos - 1];

    chapter.scenes[0].content = m_cwNotebook->GetTextCtrl()->GetBuffer();
    chapter.scenes[0].content.SetBasicStyle(m_cwNotebook->GetTextCtrl()->GetBasicStyle());
    chapter.synopsys = (wxString)m_summary->GetValue();

    chapter.notes = m_cwNotebook->GetNotes();

    chapter.Update(m_manager->GetStorage(), true, true);

    UpdateCharacterList();
    UpdateLocationList();
    UpdateItemList();
}

void amStoryWriter::CountWords() {
    int result = 0;

    if (!m_cwNotebook->GetTextCtrl()->IsEmpty()) {
        std::stringstream stream(m_cwNotebook->GetTextCtrl()->GetValue().ToStdString());
        result = std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
    }
    
    
    m_statusBar->SetStatusText("Number of words: " + std::to_string(result), 1);
}

void amStoryWriter::OnTimerEvent(wxTimerEvent& event) {
    m_statusBar->SetStatusText("Autosaving chapter...", 0);

    SaveChapter();
    CountWords();

    if (IsShown())
        m_manager->GetMainFrame()->Show();

    m_statusBar->SetStatusText("Chapter up-to-date", 0);

    event.Skip();
}

void amStoryWriter::OnListResize(wxSizeEvent& event) {
    ((wxListView*)event.GetEventObject())->SetColumnWidth(0, event.GetSize().x - 2);
}

void amStoryWriter::OnLeftSplitterChanged(wxSplitterEvent& event) {
    int x = m_charInChap->GetSize().x - 1;

    m_charInChap->SetColumnWidth(0, x);
    m_locInChap->SetColumnWidth(0, x);
    m_itemsInChap->SetColumnWidth(0, x);
}

void amStoryWriter::OnClose(wxCloseEvent& event) {
    SaveChapter();

    Destroy();
    event.Skip();
}

void amStoryWriter::CreateDVCTree() {

}


////////////////////////////////////////////////////////////////////////////////
//////////////////////////// amStoryWriterToolbar //////////////////////////////
////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(amStoryWriterToolbar, wxToolBar)

EVT_TOOL(TOOL_Bold, amStoryWriterToolbar::OnBold)
EVT_TOOL(TOOL_Italic, amStoryWriterToolbar::OnItalic)
EVT_TOOL(TOOL_Underline, amStoryWriterToolbar::OnUnderline)
EVT_TOOL(TOOL_AlignLeft, amStoryWriterToolbar::OnAlignLeft)
EVT_TOOL(TOOL_AlignCenter, amStoryWriterToolbar::OnAlignCenter)
EVT_TOOL(TOOL_AlignCenterJust, amStoryWriterToolbar::OnAlignCenterJust)
EVT_TOOL(TOOL_AlignRight, amStoryWriterToolbar::OnAlignRight)
EVT_TOOL(TOOL_TestCircle, amStoryWriterToolbar::OnTestCircle)
EVT_TOOL(TOOL_PageView, amStoryWriterToolbar::OnPageView)
EVT_TOOL(TOOL_NoteView, amStoryWriterToolbar::OnNoteView)

EVT_SLIDER(TOOL_ContentScale, amStoryWriterToolbar::OnZoom)
EVT_TOOL(TOOL_ChapterFullScreen, amStoryWriterToolbar::OnFullScreen)

EVT_UPDATE_UI(TOOL_Bold, amStoryWriterToolbar::OnUpdateBold)
EVT_UPDATE_UI(TOOL_Italic, amStoryWriterToolbar::OnUpdateItalic)
EVT_UPDATE_UI(TOOL_Underline, amStoryWriterToolbar::OnUpdateUnderline)
EVT_UPDATE_UI(TOOL_AlignLeft, amStoryWriterToolbar::OnUpdateAlignLeft)
EVT_UPDATE_UI(TOOL_AlignCenter, amStoryWriterToolbar::OnUpdateAlignCenter)
EVT_UPDATE_UI(TOOL_AlignCenterJust, amStoryWriterToolbar::OnUpdateAlignCenterJust)
EVT_UPDATE_UI(TOOL_AlignRight, amStoryWriterToolbar::OnUpdateAlignRight)
EVT_UPDATE_UI(TOOL_FontSize, amStoryWriterToolbar::OnUpdateFontSize)
EVT_UPDATE_UI(TOOL_NoteView, amStoryWriterToolbar::OnUpdateNoteView)

EVT_COMBOBOX(TOOL_FontSize, amStoryWriterToolbar::OnFontSize)

END_EVENT_TABLE()

amStoryWriterToolbar::amStoryWriterToolbar(wxWindow* parent,
    amStoryWriterNotebookPage& currentPage,
    long amStyle,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style) : wxToolBar(parent, id, pos, size, style) {

    m_currentPage = currentPage;

    AddCheckTool(TOOL_Bold, "", wxBITMAP_PNG(bold), wxBITMAP_PNG(bold), "Bold");
    AddCheckTool(TOOL_Italic, "", wxBITMAP_PNG(italic), wxBITMAP_PNG(italic), "italic");
    AddCheckTool(TOOL_Underline, "", wxBITMAP_PNG(underline), wxBITMAP_PNG(underline), "Underline");
    AddSeparator();
    AddRadioTool(TOOL_AlignLeft, "", wxBITMAP_PNG(leftAlign), wxBITMAP_PNG(leftAlign), "Align left");
    AddRadioTool(TOOL_AlignCenter, "", wxBITMAP_PNG(centerAlign), wxBITMAP_PNG(centerAlign), "Align center");
    AddRadioTool(TOOL_AlignCenterJust, "", wxBITMAP_PNG(centerJustAlign), wxBITMAP_PNG(centerJustAlign), "Align center and fit");
    AddRadioTool(TOOL_AlignRight, "", wxBITMAP_PNG(rightAlign), wxBITMAP_PNG(rightAlign), "Align right");
    AddSeparator();

    wxArrayString sizes;
    for (int i = 8; i < 28; i++) {
        if (i > 12)
            i++;

        sizes.Add(std::to_string(i));
    }
    sizes.Add("36");
    sizes.Add("48");
    sizes.Add("72");

    m_fontSize = new wxComboBox(this, TOOL_FontSize, "10", wxDefaultPosition, wxDefaultSize,
        sizes, wxCB_READONLY | wxCB_SIMPLE);
    m_fontSize->SetBackgroundColour(wxColour(30, 30, 30));
    m_fontSize->SetForegroundColour(wxColour(230, 230, 230));

    m_contentScale = new wxSlider(this, TOOL_ContentScale, 100, 50, 300,
        wxDefaultPosition, wxDefaultSize, wxSL_MIN_MAX_LABELS);
    m_contentScale->SetToolTip("100");

    AddControl(m_fontSize);
    AddStretchableSpace();

    amStoryWriterNotebook* pSWN = dynamic_cast<amStoryWriterNotebook*>(parent);
    if (amStyle |= amTB_NOTE_VIEW && pSWN) {
        m_parent = pSWN;

        AddCheckTool(TOOL_NoteView, "", wxBITMAP_PNG(noteView), wxNullBitmap, "Toggle note view");
        AddSeparator();
    }

    if (amStyle |= amTB_ZOOM) 
        AddControl(m_contentScale);

    if (amStyle |= amTB_FULLSCREEN && pSWN) {
        AddSeparator();
        AddCheckTool(TOOL_ChapterFullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Toggle Full Screen");
    }

    Realize();
}

void amStoryWriterToolbar::OnBold(wxCommandEvent& event) {
    m_currentPage.rtc->ApplyBoldToSelection();

    if (m_parent)
        m_parent->OnText(event);
}

void amStoryWriterToolbar::OnItalic(wxCommandEvent& event) {
    m_currentPage.rtc->ApplyItalicToSelection();

    if (m_parent)
        m_parent->OnText(event);
}

void amStoryWriterToolbar::OnUnderline(wxCommandEvent& event) {
    m_currentPage.rtc->ApplyUnderlineToSelection();

    if (m_parent)
        m_parent->OnText(event);
}

void amStoryWriterToolbar::OnAlignLeft(wxCommandEvent& event) {
    m_currentPage.rtc->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_LEFT));

    if (m_parent)
        m_parent->OnText(event);
}

void amStoryWriterToolbar::OnAlignCenter(wxCommandEvent& event) {
    m_currentPage.rtc->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_CENTER));

    if (m_parent)
        m_parent->OnText(event);
}

void amStoryWriterToolbar::OnAlignCenterJust(wxCommandEvent& event) {
    m_currentPage.rtc->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_JUSTIFIED));

    if (m_parent)
        m_parent->OnText(event);
}

void amStoryWriterToolbar::OnAlignRight(wxCommandEvent& event) {
    m_currentPage.rtc->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_RIGHT));

    if (m_parent)
        m_parent->OnText(event);
}

void amStoryWriterToolbar::OnNoteView(wxCommandEvent& event) {
    if (GetToolState(TOOL_NoteView)) {
        wxRichTextCtrl* current = dynamic_cast<wxRichTextCtrl*>(m_parent->GetCurrentPage());
        Freeze();

        if (current) {
            for (amStoryWriterNotebookPage& it : m_parent->GetAllWriterPages()) {
                if (current == it.rtc) {
                    wxAuiNotebook* notebook = m_parent->GetNotebook();

                    int index = notebook->GetSelection();
                    wxString title = notebook->GetPageText(index);

                    notebook->RemovePage(index);
                    notebook->InsertPage(index, it.notePanel, title, true);

                    it.rtc->Hide();
                    it.notePanel->Show();
                    it.notePanel->GetSizer()->FitInside(it.notePanel);
                    break;
                }
            }
        }
        Thaw();
    } else {
        wxScrolledWindow* current = dynamic_cast<wxScrolledWindow*>(m_parent->GetCurrentPage());
        Freeze();

        if (current) {
            for (amStoryWriterNotebookPage& it : m_parent->GetAllWriterPages()) {
                if (current == it.notePanel) {
                    wxAuiNotebook* notebook = m_parent->GetNotebook();

                    int index = notebook->GetSelection();
                    wxString title = notebook->GetPageText(index);

                    notebook->RemovePage(index);
                    notebook->InsertPage(index, it.rtc, title, true);

                    it.notePanel->Hide();
                    it.rtc->Show();
                    it.rtc->Layout();
                    break;
                }
            }
        }
        Layout();
        Thaw();
    }
}

void amStoryWriterToolbar::OnTestCircle(wxCommandEvent& event) {
    wxRichTextRange sel = m_currentPage.rtc->GetSelectionRange();
    wxRichTextBuffer& buf = m_currentPage.rtc->GetBuffer();


    wxRichTextProperties prop;
    prop.SetProperty("commentStart", true);

    buf.InsertFieldWithUndo(&buf, sel.GetStart(), "commentTag", prop, m_currentPage.rtc, 0, m_currentPage.rtc->GetBasicStyle());

    prop.SetProperty("commentStart", false);
    buf.InsertFieldWithUndo(&buf, sel.GetEnd() + 1, "commentTag", prop, m_currentPage.rtc, 0, m_currentPage.rtc->GetBasicStyle());

    for (wxRichTextObject* it : buf.GetChildren()) {
        //it.spli
    }

    wxFFileOutputStream fileStream("F:\\RTCdump.txt");
    wxTextOutputStream textStream(fileStream);

    m_currentPage.rtc->GetBuffer().Dump(textStream);

    fileStream.Close();
}

void amStoryWriterToolbar::OnZoom(wxCommandEvent& event) {
    int zoom = event.GetInt();

    m_currentPage.rtc->SetScale((double)zoom / 100.0, true);
    m_parent->OnZoomChange(zoom);
    m_contentScale->SetToolTip(std::to_string(zoom));
}

void amStoryWriterToolbar::OnFullScreen(wxCommandEvent& WXUNUSED(event)) {
    m_parent->ToggleFullScreen();
}

void amStoryWriterToolbar::OnPageView(wxCommandEvent& WXUNUSED(event)) {}

void amStoryWriterToolbar::OnUpdateBold(wxUpdateUIEvent& event) {
    event.Check(m_currentPage.rtc->IsSelectionBold());
}

void amStoryWriterToolbar::OnUpdateItalic(wxUpdateUIEvent& event) {
    event.Check(m_currentPage.rtc->IsSelectionItalics());
}

void amStoryWriterToolbar::OnUpdateUnderline(wxUpdateUIEvent& event) {
    event.Check(m_currentPage.rtc->IsSelectionUnderlined());
}

void amStoryWriterToolbar::OnUpdateAlignLeft(wxUpdateUIEvent& event) {
    event.Check(m_currentPage.rtc->IsSelectionAligned(wxTEXT_ALIGNMENT_LEFT));
}

void amStoryWriterToolbar::OnUpdateAlignCenter(wxUpdateUIEvent& event) {
    event.Check(m_currentPage.rtc->IsSelectionAligned(wxTEXT_ALIGNMENT_CENTER));
}

void amStoryWriterToolbar::OnUpdateAlignCenterJust(wxUpdateUIEvent& event) {
    event.Check(m_currentPage.rtc->IsSelectionAligned(wxTEXT_ALIGNMENT_JUSTIFIED));
}

void amStoryWriterToolbar::OnUpdateAlignRight(wxUpdateUIEvent& event) {
    event.Check(m_currentPage.rtc->IsSelectionAligned(wxTEXT_ALIGNMENT_RIGHT));
}

void amStoryWriterToolbar::OnUpdateFontSize(wxUpdateUIEvent& WXUNUSED(event)) {
    wxRichTextAttr attr;
    m_currentPage.rtc->GetStyle(m_currentPage.rtc->GetInsertionPoint() - 1, attr);
    wxString size(std::to_string(attr.GetFontSize()));

    if (m_fontSize->GetValue() != size)
        m_fontSize->SetValue(size);
}

void amStoryWriterToolbar::OnUpdateNoteView(wxUpdateUIEvent& event) {
    wxScrolledWindow* currentCorkboard = dynamic_cast<wxScrolledWindow*>(m_parent->GetCurrentPage());
    event.Check(currentCorkboard);
}

void amStoryWriterToolbar::OnFontSize(wxCommandEvent& event) {
    m_currentPage.rtc->SetFocus();
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT_SIZE);
    long size;
    event.GetString().ToLong(&size);
    attr.SetFontSize(size);

    if (m_currentPage.rtc->HasSelection()) {
        m_currentPage.rtc->SetStyleEx(m_currentPage.rtc->GetSelectionRange(), attr,
            wxRICHTEXT_SETSTYLE_WITH_UNDO | wxRICHTEXT_SETSTYLE_OPTIMIZE | wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
    } else {
        wxRichTextAttr current = m_currentPage.rtc->GetDefaultStyle();
        current.Apply(attr);
        m_currentPage.rtc->SetDefaultStyle(current);
    }
}


////////////////////////////////////////////////////////////////////////////////
/////////////////////////// amStoryWriterNotebook //////////////////////////////
////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(amStoryWriterNotebook, wxPanel)
EVT_TEXT(TEXT_Content, amStoryWriterNotebook::OnText)
END_EVENT_TABLE()

amStoryWriterNotebook::amStoryWriterNotebook(wxWindow* parent, amStoryWriter* chapterWriter) :
    wxPanel(parent) {
    m_parent = chapterWriter;
    
    m_notebook = new wxAuiNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT |
        wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_BOTTOM | wxBORDER_NONE);

    m_curTextCtrl = new wxRichTextCtrl(m_notebook, TEXT_Content, "", wxDefaultPosition, wxDefaultSize,
        wxRE_MULTILINE | wxBORDER_NONE);
    wxRichTextBuffer::AddHandler(new wxRichTextPlainTextHandler);
    //wxRichTextBuffer::AddFieldType(new amWPCommentTag("commentTag"));

    wxRichTextAttr attr;
    attr.SetFont(wxFontInfo(10));
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
    attr.SetLeftIndent(63, -63);
    attr.SetTextColour(wxColour(250, 250, 250));
    m_curTextCtrl->SetBasicStyle(attr);
    m_curTextCtrl->SetBackgroundColour(wxColour(35, 35, 35));

    m_curCorkboard = new wxScrolledWindow(m_notebook, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_curCorkboard->SetBackgroundColour(wxColour(45, 45, 45));
    m_curCorkboard->EnableScrolling(false, true);
    m_curCorkboard->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
    m_curCorkboard->Hide();

    m_notesSizer = new wxWrapSizer();
    m_curCorkboard->SetSizer(m_notesSizer);
    m_curCorkboard->SetScrollRate(15, 15);

    m_notebook->AddPage(m_curTextCtrl, "Main");
    //m_notebook->AddPage(m_curCorkboard, "Notes");

    m_writerPages.push_back(amStoryWriterNotebookPage(m_curTextCtrl, m_curCorkboard));
    m_contentToolbar = new amStoryWriterToolbar(this, m_writerPages.front());

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_contentToolbar, wxSizerFlags(0).Expand());
    sizer->Add(m_notebook, wxSizerFlags(1).Expand());

    SetSizer(sizer);

    wxTimer m_timer(this, 12345);
    m_timer.Start(10000);
}

void amStoryWriterNotebook::OnText(wxCommandEvent& WXUNUSED(event)) {
    m_parent->m_statusBar->SetStatusText("Chapter modified. Autosaving soon...", 0);
}

void amStoryWriterNotebook::OnZoomChange(int zoom) {
    m_parent->PushStatusText("Zoom: " + std::to_string(zoom) + "%", 2);
}

bool amStoryWriterNotebook::HasRedNote() {
    for (unsigned int i = 0; i < m_notes.size(); i++) {
        if (m_notes[i].isDone == false)
            return true;
    }

    return false;
}

void amStoryWriterNotebook::AddNote(wxString& noteContent, wxString& noteName, bool isDone) {

    wxPanel* notePanel = new wxPanel(m_curCorkboard);

    wxTextCtrl* noteLabel = new wxTextCtrl(notePanel, -1, noteName);
    noteLabel->SetValue(noteName);
    noteLabel->SetBackgroundColour(wxColour(240, 240, 240));
    wxPanel* options = new wxPanel(notePanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    options->SetBackgroundColour(wxColour(200, 200, 200));
    wxTextCtrl* note = new wxTextCtrl(notePanel, -1, noteContent, wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE);
    note->SetBackgroundColour(wxColour(255, 250, 205));

    noteLabel->Bind(wxEVT_TEXT, &amStoryWriterNotebook::UpdateNoteLabel, this);
    note->Bind(wxEVT_TEXT, &amStoryWriterNotebook::UpdateNote, this);

    options->Bind(wxEVT_RIGHT_DOWN, &amStoryWriterNotebook::OnNoteClick, this);
    options->Bind(wxEVT_LEFT_DOWN, &amStoryWriterNotebook::OnNoteClick, this);
    options->Bind(wxEVT_PAINT, &amStoryWriterNotebook::PaintDots, this);

    wxRadioButton* b1 = new wxRadioButton(notePanel, -1, "", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    wxRadioButton* b2 = new wxRadioButton(notePanel, -1, "", wxDefaultPosition, wxDefaultSize);

    b1->Bind(wxEVT_RADIOBUTTON, &amStoryWriterNotebook::SetRed, this);
    b2->Bind(wxEVT_RADIOBUTTON, &amStoryWriterNotebook::SetGreen, this);

    b1->SetBackgroundColour(wxColour(120, 0, 0));
    b2->SetBackgroundColour(wxColour(0, 140, 0));

    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(noteLabel, wxSizerFlags(8).Expand());
    topSizer->Add(options, wxSizerFlags(1).Expand());

    wxBoxSizer* bsiz = new wxBoxSizer(wxHORIZONTAL);
    bsiz->Add(b1, wxSizerFlags(0));
    bsiz->AddStretchSpacer(1);
    bsiz->Add(b2, wxSizerFlags(0));

    wxBoxSizer* siz = new wxBoxSizer(wxVERTICAL);
    siz->Add(topSizer, wxSizerFlags(0).Expand());
    siz->Add(note, wxSizerFlags(1).Expand());
    siz->Add(bsiz, wxSizerFlags(0).Expand());
    notePanel->SetSizer(siz);

    if (isDone) {
        notePanel->SetBackgroundColour(wxColour(0, 140, 0));
        b2->SetValue(true);
    } else {
        notePanel->SetBackgroundColour(wxColour(120, 0, 0));
        b1->SetValue(true);
    }

    m_notesSizer->Add(notePanel, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP, 22));
    m_notesSizer->SetItemMinSize(m_notesSizer->GetItemCount() - 1, m_noteSize);
    m_notesSizer->Layout();

    m_curCorkboard->GetSizer()->FitInside(m_curCorkboard);

    Note thisNote(noteContent, noteName);
    thisNote.isDone = isDone;

    m_notes.push_back(thisNote);
}

void amStoryWriterNotebook::PaintDots(wxPaintEvent& event) {
    wxPanel* pan = (wxPanel*)event.GetEventObject();

    wxPaintDC dc(pan);
    wxGCDC gc(dc);

    pan->PrepareDC(gc);

    wxSize opSize(pan->GetSize());

    gc.SetBrush(wxBrush(wxColour(140, 140, 140)));
    gc.SetPen(wxPen(wxColour(140, 140, 140)));

    gc.DrawCircle(wxPoint((opSize.x / 4) - 1, opSize.y / 2), 2);
    gc.DrawCircle(wxPoint((opSize.x / 2) - 1, opSize.y / 2), 2);
    gc.DrawCircle(wxPoint((((opSize.x / 4) * 3)), opSize.y / 2), 2);
}

void amStoryWriterNotebook::SetRed(wxCommandEvent& event) {
    wxRadioButton* rb = (wxRadioButton*)event.GetEventObject();
    wxPanel* np = (wxPanel*)rb->GetParent();

    wxPanel* sp;
    int i = 0;
    for (Note& note : m_notes) {
        sp = (wxPanel*)m_notesSizer->GetItem(i)->GetWindow();

        if (sp == np) {
            note.isDone = false;
            np->SetBackgroundColour(wxColour(120, 0, 0));
            np->Refresh();
            break;
        }

        i++;
    }

    m_parent->CheckNotes();
}

void amStoryWriterNotebook::SetGreen(wxCommandEvent& event) {
    wxRadioButton* gb = (wxRadioButton*)event.GetEventObject();
    wxPanel* np = (wxPanel*)gb->GetParent();

    wxPanel* sp;
    int i = 0;
    for (Note& note : m_notes) {
        sp = (wxPanel*)m_notesSizer->GetItem(i)->GetWindow();

        if (sp == np) {
            note.isDone = true;
            np->SetBackgroundColour(wxColour(0, 140, 0));
            np->Refresh();
            break;
        }

        i++;
    }

    m_parent->CheckNotes();
}

void amStoryWriterNotebook::OnDeleteNote(wxCommandEvent& WXUNUSED(event)) {
    if (m_selNote) {
        wxPanel* sp;

        int i = 0;
        for (Note& note : m_notes) {
            sp = (wxPanel*)m_notesSizer->GetItem(i)->GetWindow();

            if (sp == m_selNote) {
                m_notes.erase(&note);
                m_selNote->Destroy();
                m_notesSizer->Layout();
                m_curCorkboard->FitInside();
                m_notes.shrink_to_fit();
                break;
            }

            i++;
        }


        m_selNote = nullptr;
    }

    m_parent->CheckNotes();
}

void amStoryWriterNotebook::OnNoteClick(wxMouseEvent& event) {
    wxPanel* pan = (wxPanel*)event.GetEventObject();

    m_selNote = (wxPanel*)pan->GetParent();

    wxMenu menu;
    menu.Append(MENU_Delete, "Delete");
    menu.Bind(wxEVT_MENU, &amStoryWriterNotebook::OnDeleteNote, this, MENU_Delete);
    pan->PopupMenu(&menu, wxPoint(-1, pan->GetSize().y));
}

void amStoryWriterNotebook::UpdateNoteLabel(wxCommandEvent& event) {
    wxTextCtrl* ttc = (wxTextCtrl*)event.GetEventObject();
    wxPanel* pan = (wxPanel*)ttc->GetParent();

    wxPanel* sp;
    int i = 0;
    for (Note& note : m_notes) {
        sp = (wxPanel*)m_notesSizer->GetItem(i)->GetWindow();
        if (sp == pan) {
            note.name = ttc->GetValue();
        }

        i++;
    }
}

void amStoryWriterNotebook::UpdateNote(wxCommandEvent& event) {
    wxTextCtrl* trtc = (wxTextCtrl*)event.GetEventObject();
    wxPanel* pan = (wxPanel*)trtc->GetParent();

    wxPanel* sp;
    int i = 0;
    for (Note& note : m_notes) {
        sp = (wxPanel*)m_notesSizer->GetItem(i)->GetWindow();
        if (sp == pan) {
            note.content = trtc->GetValue();
        }

        i++;
    }
}