#include "StoryWriter.h"
#include "ElementsNotebook.h"
#include "Outline.h"
#include "amUtility.h"

#include <wx\popupwin.h>
#include <wx\listbox.h>
#include <wx\richtext\richtextxml.h>
#include <wx\dcbuffer.h>
#include <wx\dcgraph.h>
#include <wx\splitter.h>
#include <wx\wfstream.h>

#include <sstream>

#include "wxmemdbg.h"

wxVector<wxIcon> StoryTreeModelNode::m_icons{};

StoryTreeModel::StoryTreeModel()
{
	StoryTreeModelNode::InitAllIcons();
}

bool StoryTreeModel::Load()
{
	return false;
}

bool StoryTreeModel::Save()
{
	return false;
}

void StoryTreeModel::CreateFromScratch(amProject* project)
{
	wxString name;

	m_trashNode = new StoryTreeModelNode(_("Trash"));
	m_trashNode->SetContainer(true);
	ItemAdded(wxDataViewItem(nullptr), wxDataViewItem(m_trashNode));
	
	for ( Book*& pBook : project->books )
	{
		StoryTreeModelNode* pBookNode = new StoryTreeModelNode(pBook);
		pBookNode->SetContainer(true);

		m_vBooks.push_back(pBookNode);
		wxDataViewItem bookItem(pBookNode);

		ItemAdded(wxDataViewItem(nullptr), bookItem);

		for ( Document*& pDocument : pBook->documents )
		{
			if ( !pDocument->parent )
			{
				AddDocument(pDocument, bookItem);
			}
		}
	}
}

wxDataViewItem StoryTreeModel::AddDocument(Document* document, const wxDataViewItem& parentItem)
{
	StoryTreeModelNode* node;
	wxDataViewItem parent;
	if ( document->isInTrash )
	{
		node = new StoryTreeModelNode((StoryTreeModelNode*)parentItem.GetID(), m_trashNode, document);
		parent = wxDataViewItem(m_trashNode);
	}
	else
	{
		node = new StoryTreeModelNode((StoryTreeModelNode*)parentItem.GetID(), nullptr, document);
		parent = parentItem;
	}

	if ( !parent.IsOk() )
		m_otherRoots.push_back(node);

	wxDataViewItem item(node);
	ItemAdded(parent, item);

	for ( Document*& pChild : document->children )
	{
		AddDocument(pChild, item);
	}

	return item;
}

wxDataViewItem StoryTreeModel::GetDocumentItem(Document* document)
{
	wxDataViewItem item(nullptr);
	for ( StoryTreeModelNode*& pBookNode : m_vBooks )
	{
		for ( amTreeModelNode*& node : pBookNode->GetChildren() )
		{
			item = ScanForDocumentRecursive((StoryTreeModelNode*)node, document);
			if ( item.IsOk() )
				break;
		}

		if ( item.IsOk() )
			break;
	}

	if ( !item.IsOk() && m_trashNode )
	{
		for ( amTreeModelNode*& node : m_trashNode->GetChildren() )
		{
			item = ScanForDocumentRecursive((StoryTreeModelNode*)node, document);
			if ( item.IsOk() )
				break;
		}
	}

	return item;
}

wxDataViewItem StoryTreeModel::ScanForDocumentRecursive(StoryTreeModelNode* node, Document* document)
{
	if ( node->GetDocument() == document )
		return wxDataViewItem(node);

	wxDataViewItem item(nullptr);

	for ( amTreeModelNode*& child : node->GetChildren() )
	{
		StoryTreeModelNode* pChild = (StoryTreeModelNode*)child;
		item = ScanForDocumentRecursive(pChild, document);

		if ( item.IsOk() )
			break;
	}

	return item;
}

int StoryTreeModel::GetIndex(const wxDataViewItem& item)
{
	return ((StoryTreeModelNode*)item.GetID())->GetIndex();
}

STMN_Type StoryTreeModel::MoveToTrash(const wxDataViewItem& item)
{
	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
	if ( node->IsInTrash() || node->IsTrash() || node->IsBook() )
		return STMN_Null;

	node->RemoveSelfFromParentList();
	ItemDeleted(GetParent(item), wxDataViewItem(node));

	m_trashNode->Append(node);
	node->SetIsInTrash(true);
	ItemAdded(wxDataViewItem((void*)m_trashNode), wxDataViewItem(node));

	return node->GetType();
}

STMN_Type StoryTreeModel::RestoreFromTrash(const wxDataViewItem& item)
{
	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
	if ( !node->IsInTrash() || node->IsTrash() || node->IsBook() )
		return STMN_Null;

	amTreeModelNode* parent = node->GetParent();

	for ( amTreeModelNode*& trashChild : m_trashNode->GetChildren() )
	{
		if ( trashChild == node )
			m_trashNode->GetChildren().erase(&trashChild);
	}

	wxVector<amTreeModelNode*>& children = parent->GetChildren();

	int lastIndex = children.size() - 1;
	for ( int i = 0; i <= lastIndex; i++ )
	{
		if ( ((StoryTreeModelNode*)children[i])->GetIndex() >= node->GetIndex() + 1 )
		{
			parent->Insert(node, i);
			break;
		}
		else if ( i == lastIndex )
		{
			parent->Append(node);
		}
	}

	node->SetIsInTrash(false);

	ItemDeleted(wxDataViewItem(m_trashNode), item);
	ItemAdded(wxDataViewItem(parent), item);

	return node->GetType();
}

wxDataViewItem StoryTreeModel::GetBookItem(Book* book)
{
	for ( StoryTreeModelNode* pBookNode : m_vBooks )
	{
		if ( pBookNode->GetBook() == book )
			return wxDataViewItem(pBookNode);
	}

	return wxDataViewItem(nullptr);
}

void StoryTreeModel::DeleteItem(const wxDataViewItem& item)
{
	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();
	if ( !node )      // happens if item.IsOk()==false
		return;

	// first remove the node from the parent's array of children;
	// NOTE: StoryTreeModelNode is only an array of _pointers_
	//       thus removing the node from it doesn't result in freeing it

	wxDataViewItemArray children;
	GetChildren(item, children);

	for ( unsigned int i = 0; i < children.GetCount(); i++ )
		DeleteItem(children[i]);

	amTreeModelNode* parentNode = node->GetParent();
	if ( parentNode )
		node->RemoveSelfFromParentList();
	else
	{
		for ( amTreeModelNode*& otherRoot : m_otherRoots )
		{
			if ( otherRoot == node )
				m_otherRoots.erase(&otherRoot);
		}
	}

	if ( this->m_handler->GetItemUnderMouse() == item )
	{
		this->m_handler->SetItemUnderMouse(wxDataViewItem(nullptr));
	}

	wxDataViewItem parent(parentNode);
	ItemDeleted(parent, item);

	// free the node
	delete node;
}

wxDataViewItem StoryTreeModel::SelectDocument(Document* document)
{

	return wxDataViewItem();
}

bool StoryTreeModel::Reposition(const wxDataViewItem& item, int n)
{
	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

	if ( node )
	{
		node->Reposition(n);
		ItemChanged(wxDataViewItem(node->GetParent()));
	}
	else
		return false;

	return true;
}

void StoryTreeModel::ClearContent()
{
	wxVector<amTreeModelNode*> array;

	for ( StoryTreeModelNode* pBookNode : m_vBooks )
	{
		array = pBookNode->GetChildren();
		for ( unsigned int i = 0; i < array.size(); i++ )
			DeleteItem(wxDataViewItem(array[i]));
	}

	if ( m_trashNode )
	{
		array = m_trashNode->GetChildren();
		for ( unsigned int i = 0; i < array.size(); i++ )
			DeleteItem(wxDataViewItem(array[i]));
	}
}

void StoryTreeModel::ClearAll()
{
	for ( StoryTreeModelNode* pBookNode : m_vBooks )
	{
		DeleteItem(wxDataViewItem(pBookNode));
	}
	m_vBooks.clear();

	size_t otherRootsCount = m_otherRoots.size();
	for ( int i = 0; i < otherRootsCount; i++ )
	{
		DeleteItem(wxDataViewItem(m_otherRoots[0]));
	}
	m_otherRoots.clear();

	if ( m_trashNode )
	{
		DeleteItem(wxDataViewItem(m_trashNode));
		m_trashNode = nullptr;
	}

	m_handler->SetItemUnderMouse(wxDataViewItem(nullptr));
}

void StoryTreeModel::GetValue(wxVariant& variant,
	const wxDataViewItem& item, unsigned int col) const
{
	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

	switch ( col )
	{
	case 0:
	{
		wxDataViewIconText it;
		it.SetText(node->GetTitle());

		if ( node->IsBook() )
			it.SetIcon(node->m_icons[0]);
		else if ( node->IsDocument() )
			it.SetIcon(node->m_icons[2]);
		else if ( node->IsTrash() )
			it.SetIcon(node->m_icons[3]);

		variant << it;
		break;
	}

	case 1:
		if ( node->IsDocument() )
			variant = std::to_string(node->GetDocument()->GetWordCount());

		break;
	}
}

wxDataViewItem StoryTreeModel::GetParent(const wxDataViewItem& item) const
{
	// the invisible root node has no parent
	if ( !item.IsOk() )
		return wxDataViewItem(0);

	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

	if ( node->IsInTrash() )
		return wxDataViewItem(m_trashNode);
	else
		return wxDataViewItem(node->GetParent());
}

unsigned int StoryTreeModel::GetChildren(const wxDataViewItem& parent,
	wxDataViewItemArray& array) const
{
	StoryTreeModelNode* node = (StoryTreeModelNode*)parent.GetID();
	if ( !node )
	{
		int n = 0;

		for ( StoryTreeModelNode* pBookNode : m_vBooks )
		{
			array.Add(wxDataViewItem(pBookNode));
			n++;
		}

		if ( m_trashNode )
		{
			array.Add(wxDataViewItem(m_trashNode));
			n++;
		}

		for ( unsigned int i = 0; i < m_otherRoots.size(); i++ )
			array.Add(wxDataViewItem(m_otherRoots.at(i)));

		return n + m_otherRoots.size();
	}

	int count = node->GetChildCount();

	if ( count == 0 )
	{
		return 0;
	}

	for ( int pos = 0; pos < count; pos++ )
	{
		amTreeModelNode* child = node->GetChildren().at(pos);
		array.Add(wxDataViewItem(child));
	}

	return count;
}


///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// amStoryWriter ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(amStoryWriter, wxFrame)

EVT_BUTTON(BUTTON_NoteClear, amStoryWriter::ClearNote)
EVT_BUTTON(BUTTON_NoteAdd, amStoryWriter::OnAddNote)

EVT_BUTTON(BUTTON_AddChar, amStoryWriter::OnAddCharacter)
EVT_BUTTON(BUTTON_AddLoc, amStoryWriter::OnAddLocation)
EVT_BUTTON(BUTTON_AddItem, amStoryWriter::OnAddItem)
EVT_BUTTON(BUTTON_RemChar, amStoryWriter::OnRemoveCharacter)
EVT_BUTTON(BUTTON_RemLoc, amStoryWriter::OnRemoveLocation)
EVT_BUTTON(BUTTON_RemItem, amStoryWriter::OnRemoveItem)

EVT_DATAVIEW_ITEM_ACTIVATED(TREE_Story, amStoryWriter::OnStoryItemActivated)
EVT_DATAVIEW_SELECTION_CHANGED(TREE_Story, amStoryWriter::OnStoryItemSelected)

EVT_MENU(MENU_MoveToTrash, amStoryWriter::OnMoveToTrash)
EVT_MENU(MENU_RestoreFromTrash, amStoryWriter::OnRestoreFromTrash)

EVT_BUTTON(BUTTON_NextChap, amStoryWriter::OnNextDocument)
EVT_BUTTON(BUTTON_PreviousChap, amStoryWriter::OnPreviousDocument)

EVT_TIMER(TIMER_Save, amStoryWriter::OnTimerEvent)

EVT_CLOSE(amStoryWriter::OnClose)

END_EVENT_TABLE()

amStoryWriter::amStoryWriter(wxWindow* parent, amProjectManager* manager, Document* document) :
	m_saveTimer(this, TIMER_Save),
	wxFrame(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
	wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN)
{
	m_manager = manager;

	Hide();
	SetBackgroundColour({ 20, 20, 20 });

	amSplitterWindow* rightSplitter = new amSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize,
		1024L | wxSP_LIVE_UPDATE | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);
	amSplitterWindow* leftSplitter = new amSplitterWindow(rightSplitter, -1, wxDefaultPosition, wxDefaultSize,
		1024L | wxSP_LIVE_UPDATE | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);
	leftSplitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGED, &amStoryWriter::OnLeftSplitterChanged, this);

	leftSplitter->SetBackgroundColour(wxColour(20, 20, 20));
	rightSplitter->SetBackgroundColour(wxColour(20, 20, 20));

	leftSplitter->SetMinimumPaneSize(30);
	rightSplitter->SetMinimumPaneSize(30);

	m_swNotebook = new amStoryWriterNotebook(leftSplitter, this);

	wxAuiNotebook* leftNotebook = new wxAuiNotebook(leftSplitter, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	
	wxAuiSimpleTabArt* pArt = new wxAuiSimpleTabArt();
	pArt->SetColour(wxColour(50, 50, 50));
	pArt->SetActiveColour(wxColour(30, 30, 30));
	leftNotebook->SetArtProvider(pArt);

	leftNotebook->GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	leftNotebook->GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

	m_leftPanel = new wxPanel(leftNotebook, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_leftPanel->SetBackgroundColour(wxColour(45, 45, 45));

	m_characterPanel = new wxPanel(m_leftPanel);
	m_characterPanel->SetBackgroundColour(wxColour(80, 80, 80));

	wxStaticText* charInChapLabel = new wxStaticText(m_characterPanel, -1, "Characters present in document",
		wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
	charInChapLabel->SetBackgroundColour(wxColour(10, 10, 10));
	charInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
	charInChapLabel->SetFont(wxFontInfo(10).Bold());
	charInChapLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_charInChap = new wxListView(m_characterPanel, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_HRULES | wxLC_LIST | wxLC_NO_HEADER | wxBORDER_NONE);

	m_charInChap->SetBackgroundColour(wxColour(35, 35, 35));
	m_charInChap->SetForegroundColour(wxColour(230, 230, 230));
	m_charInChap->InsertColumn(0, "Characters present in document", wxLIST_ALIGN_LEFT, FromDIP(10));
	m_charInChap->SetMinSize(FromDIP(wxSize(150, 150)));
	m_charInChap->Bind(wxEVT_LIST_ITEM_ACTIVATED, &amStoryWriter::OnElementActivated, this);

	wxButton* addCharButton = new wxButton(m_characterPanel, BUTTON_AddChar, "Add", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	addCharButton->SetBackgroundColour(wxColour(60, 60, 60));
	addCharButton->SetForegroundColour(wxColour(255, 255, 255));
	addCharButton->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	addCharButton->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);
	wxButton* remCharButton = new wxButton(m_characterPanel, BUTTON_RemChar, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	remCharButton->SetBackgroundColour(wxColour(60, 60, 60));
	remCharButton->SetForegroundColour(wxColour(255, 255, 255));
	remCharButton->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	remCharButton->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);

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

	wxStaticText* locInChapLabel = new wxStaticText(m_locationPanel, -1, "Locations present in document",
		wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
	locInChapLabel->SetBackgroundColour(wxColour(10, 10, 10));
	locInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
	locInChapLabel->SetFont(wxFontInfo(10).Bold());
	locInChapLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_locInChap = new wxListView(m_locationPanel, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_HRULES | wxLC_LIST | wxLC_NO_HEADER | wxBORDER_NONE);

	m_locInChap->SetBackgroundColour(wxColour(35, 35, 35));
	m_locInChap->SetForegroundColour(wxColour(230, 230, 230));
	m_locInChap->InsertColumn(0, "Locations present in document", wxLIST_ALIGN_LEFT, FromDIP(155));
	m_locInChap->SetMinSize(FromDIP(wxSize(150, 150)));
	m_locInChap->Bind(wxEVT_LIST_ITEM_ACTIVATED, &amStoryWriter::OnElementActivated, this);

	wxButton* addLocButton = new wxButton(m_locationPanel, BUTTON_AddLoc, "Add", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	addLocButton->SetBackgroundColour(wxColour(60, 60, 60));
	addLocButton->SetForegroundColour(wxColour(255, 255, 255));
	addLocButton->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	addLocButton->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);
	wxButton* remLocButton = new wxButton(m_locationPanel, BUTTON_RemLoc, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	remLocButton->SetBackgroundColour(wxColour(60, 60, 60));
	remLocButton->SetForegroundColour(wxColour(255, 255, 255));
	remLocButton->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	remLocButton->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);

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

	wxStaticText* itemsInChapLabel = new wxStaticText(m_itemPanel, -1, "Items present in document",
		wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxBORDER_SIMPLE);
	itemsInChapLabel->SetBackgroundColour(wxColour(10, 10, 10));
	itemsInChapLabel->SetForegroundColour(wxColour(255, 255, 255));
	itemsInChapLabel->SetFont(wxFontInfo(10).Bold());
	itemsInChapLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_itemsInChap = new wxListView(m_itemPanel, -1, wxDefaultPosition, wxDefaultSize,
		wxLC_HRULES | wxLC_LIST | wxLC_NO_HEADER | wxBORDER_NONE);

	m_itemsInChap->SetBackgroundColour(wxColour(35, 35, 35));
	m_itemsInChap->SetForegroundColour(wxColour(230, 230, 230));
	m_itemsInChap->InsertColumn(0, "Items present in document", wxLIST_ALIGN_LEFT, FromDIP(155));
	m_itemsInChap->SetMinSize(FromDIP(wxSize(150, 150)));
	m_itemsInChap->Bind(wxEVT_LIST_ITEM_ACTIVATED, &amStoryWriter::OnElementActivated, this);

	wxButton* addItemButton = new wxButton(m_itemPanel, BUTTON_AddItem, "Add", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	addItemButton->SetBackgroundColour(wxColour(60, 60, 60));
	addItemButton->SetForegroundColour(wxColour(255, 255, 255));
	addItemButton->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	addItemButton->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);
	wxButton* remItemButton = new wxButton(m_itemPanel, BUTTON_RemItem, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	remItemButton->SetBackgroundColour(wxColour(60, 60, 60));
	remItemButton->SetForegroundColour(wxColour(255, 255, 255));
	remItemButton->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
	remItemButton->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);

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
	m_leftSizer->Add(m_characterPanel, wxSizerFlags(1).Expand());
	m_leftSizer->AddSpacer(FromDIP(15));
	m_leftSizer->Add(m_locationPanel, wxSizerFlags(1).Expand());
	m_leftSizer->AddSpacer(FromDIP(15));
	m_leftSizer->Add(m_itemPanel, wxSizerFlags(1).Expand());
	m_leftSizer->Add(leftButton, wxSizerFlags(0).Right());

	m_leftPanel->SetSizer(m_leftSizer);

	wxPanel* leftPanel2 = new wxPanel(leftNotebook);
	leftPanel2->SetBackgroundColour(wxColour(90, 90, 90));

	m_outlineTreeModel = m_manager->GetOutline()->GetOutlineFiles()->GetOutlineTreeModel();

#ifdef __WXMSW__
	m_outlineView = m_outlineViewHTHandler.Create(leftPanel2, TREE_Outline, m_outlineTreeModel.get());
#else
	m_outlineView = new wxDataViewCtrl(leftPanel2, TREE_Outline, wxDefaultPosition, wxDefaultSize,
		wxDV_NO_HEADER | wxDV_SINGLE | wxBORDER_NONE);
	m_outlineView->AssociateModel(m_outlineTreeModel.get());
#endif

	wxDataViewColumn* columnF = new wxDataViewColumn(_("Files"), new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType(),
		wxDATAVIEW_CELL_EDITABLE), 0, FromDIP(200), wxALIGN_LEFT);
	m_outlineView->AppendColumn(columnF);
	m_outlineView->SetBackgroundColour(wxColour(90, 90, 90));
	m_outlineView->EnableDragSource(wxDF_UNICODETEXT);
	m_outlineView->EnableDropTarget(wxDF_UNICODETEXT);

	wxButton* leftButton2 = new wxButton(leftPanel2, BUTTON_PreviousChap, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
	leftButton2->SetBitmap(wxBITMAP_PNG(arrowLeft));

	wxBoxSizer* outlineSizer = new wxBoxSizer(wxVERTICAL);
	outlineSizer->Add(m_outlineView, wxSizerFlags(1).Expand());
	outlineSizer->Add(leftButton2, wxSizerFlags(0).Right());

	leftPanel2->SetSizer(outlineSizer);

	wxPanel* leftPanel3 = new wxPanel(leftNotebook);
	leftPanel3->SetBackgroundColour(wxColour(90, 90, 90));

	m_storyTreeModel = new StoryTreeModel();

#ifdef __WXMSW__
	m_storyView = m_storyViewHTHandler.Create(leftPanel3, TREE_Story, m_storyTreeModel.get());
#else
	m_storyView = new wxDataViewCtrl(leftPanel3, TREE_Story, wxDefaultPosition, wxDefaultSize,
		wxDV_NO_HEADER | wxDV_SINGLE | wxBORDER_NONE);
	m_storyView->AssociateModel(m_storyView.get());
#endif

	m_storyView->SetBackgroundColour(wxColour(90, 90, 90));

	wxDataViewColumn* columnS = new wxDataViewColumn(_("Story"), new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType(),
		wxDATAVIEW_CELL_EDITABLE), 0, FromDIP(200), wxALIGN_LEFT);
	m_storyView->AppendColumn(columnS);
	m_storyView->GetMainWindow()->Bind(wxEVT_RIGHT_DOWN, &amStoryWriter::OnStoryViewRightClick, this);

	if ( !m_storyTreeModel->Load() )
	{
		m_storyTreeModel->CreateFromScratch(m_manager->GetProject());
		
		for ( StoryTreeModelNode* pBookNode : m_storyTreeModel->GetBooks() )
			m_storyView->Expand(wxDataViewItem(pBookNode));

		m_storyView->Expand(wxDataViewItem(m_storyTreeModel->GetTrash()));
	}

	m_storyView->Refresh();

	wxButton* leftButton3 = new wxButton(leftPanel3, BUTTON_PreviousChap, "", wxDefaultPosition, FromDIP(wxSize(25, 25)));
	leftButton3->SetBitmap(wxBITMAP_PNG(arrowLeft));

	wxBoxSizer* storySizer = new wxBoxSizer(wxVERTICAL);
	storySizer->Add(m_storyView, wxSizerFlags(1).Expand());
	storySizer->Add(leftButton3, wxSizerFlags(0).Right());

	leftPanel3->SetSizer(storySizer);
	
	leftNotebook->AddPage(leftPanel3, "Story");
	leftNotebook->AddPage(leftPanel2, "Outline");
	leftNotebook->AddPage(m_leftPanel, "Elements");

	wxPanel* rightPanel = new wxPanel(rightSplitter, -1);
	rightPanel->SetBackgroundColour(wxColour(60, 60, 60));

	wxRichTextAttr attr;
	attr.SetTextColour(wxColour(245, 245, 245));
	attr.SetFont(wxFontInfo(10));

	m_synopsys = new wxRichTextCtrl(rightPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_synopsys->SetBackgroundColour(wxColour(35, 35, 35));
	m_synopsys->SetBasicStyle(attr);
	m_synopsys->SetForegroundColour(wxColour(245, 245, 245));
	//m_synopsys->SetBackgroundStyle(wxBG_STYLE_PAINT);

	wxStaticText* sumLabel = new wxStaticText(rightPanel, -1, "Synopsys", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	sumLabel->SetBackgroundColour(wxColour(110, 0, 0));
	sumLabel->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased()));
	sumLabel->SetForegroundColour(wxColour(255, 255, 255));
	sumLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_noteChecker = new wxStaticText(rightPanel, -1, "Nothing to show.", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_noteChecker->SetBackgroundColour(wxColour(20, 20, 20));
	m_noteChecker->SetForegroundColour(wxColour(255, 255, 255));
	m_noteChecker->SetFont(wxFontInfo(10).Bold());
	m_noteChecker->SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_noteLabel = new wxTextCtrl(rightPanel, -1, "New note", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_noteLabel->SetBackgroundColour(wxColour(80, 80, 80));
	m_noteLabel->SetForegroundColour(wxColour(255, 255, 255));
	m_noteLabel->SetFont(wxFont(wxFontInfo(10)));
	m_noteLabel->SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_note = new wxRichTextCtrl(rightPanel, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_note->SetBackgroundColour(wxColour(50, 50, 50));
	m_note->SetBasicStyle(attr);
	//m_note->SetBackgroundStyle(wxBG_STYLE_PAINT);

	wxPanel* nbHolder = new wxPanel(rightPanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	nbHolder->SetBackgroundColour(wxColour(30, 30, 30));

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
	m_rightSizer->Add(sumLabel, wxSizerFlags(0).Expand());
	m_rightSizer->Add(m_synopsys, wxSizerFlags(1).Expand());
	m_rightSizer->AddSpacer(FromDIP(15));
	m_rightSizer->Add(m_noteChecker, wxSizerFlags(0).Expand());
	m_rightSizer->Add(m_noteLabel, wxSizerFlags(0).Expand());
	m_rightSizer->Add(m_note, wxSizerFlags(1).Expand());
	m_rightSizer->Add(nbHolder, wxSizerFlags(0).Expand());
	m_rightSizer->Add(rightButton, wxSizerFlags(0).Left());

	rightPanel->SetSizer(m_rightSizer);

	SetSize(FromDIP(wxSize(1100, 700)));
	Maximize();

	leftSplitter->SplitVertically(leftNotebook, m_swNotebook, FromDIP(230));
	rightSplitter->SplitVertically(leftSplitter, rightPanel, FromDIP(1135));
	leftSplitter->SetSashGravity(0.0);
	rightSplitter->SetSashGravity(1.0);

	wxMenuBar* menu = new wxMenuBar();
	wxMenu* menu1 = new wxMenu();
	menu->Append(menu1, "File");

	SetMenuBar(menu);

	m_statusBar = CreateStatusBar(3);
	m_statusBar->SetStatusText("Document up-to-date", 0);
	m_statusBar->SetStatusText("Number of words: 0", 1);
	m_statusBar->SetStatusText("Zoom: 100%", 2);
	m_statusBar->SetBackgroundColour(wxColour(120, 120, 120));

	wxSize noteSize((m_swNotebook->GetClientSize()));
	m_swNotebook->SetNoteSize(FromDIP(wxSize((noteSize.x / 3) - 30, (noteSize.y / 4) - 10)));

	SetIcon(wxICON(amadeus));
	LoadDocument(document);

	Show();
	SetFocus();

	m_saveTimer.Start(10000);
	OnLeftSplitterChanged(wxSplitterEvent());
}

void amStoryWriter::ClearNote(wxCommandEvent& event)
{
	m_note->Clear();
	//m_noteLabel->SetValue("New note");

	event.Skip();
}

void amStoryWriter::OnAddNote(wxCommandEvent& event)
{
	if ( !m_note->IsEmpty() )
	{
		Note* pNote = new Note(m_note->GetValue(), m_noteLabel->GetValue());
		AddNote(pNote, true);
	}

	event.Skip();
}

void amStoryWriter::AddNote(Note* note, bool addToDocument)
{
	m_swNotebook->AddNote(note);
	m_note->Clear();
	m_noteLabel->SetValue("New note");

	if ( addToDocument )
		m_activeTab.document->notes.push_back(note);

	CheckNotes();
}

void amStoryWriter::CheckNotes()
{
	if ( !m_activeTab.document->notes.empty() )
	{
		if ( m_activeTab.document->HasRedNote() )
		{
			m_noteChecker->SetBackgroundColour(wxColour(120, 0, 0));
			m_noteChecker->SetLabel("Unresolved notes!");
		}
		else
		{
			m_noteChecker->SetBackgroundColour(wxColour(0, 100, 0));
			m_noteChecker->SetLabel("All notes resolved!");
		}
	}
	else
	{
		m_noteChecker->SetBackgroundColour(wxColour(20, 20, 20));
		m_noteChecker->SetLabel("No notes yet.");
	}

	m_rightSizer->Layout();
}

void amStoryWriter::OnAddCharacter(wxCommandEvent& event)
{
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

	win->Popup();
	event.Skip();
}

void amStoryWriter::OnAddLocation(wxCommandEvent& event)
{
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

void amStoryWriter::OnAddItem(wxCommandEvent& event)
{
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

	win->Popup();
	event.Skip();
}

void amStoryWriter::AddCharacter(wxCommandEvent& event)
{
	wxString name = event.GetString();
	if ( m_charInChap->FindItem(-1, name) == -1 )
		m_manager->AddElementToDocument(m_manager->GetElementByName(name), m_activeTab.document);
}

void amStoryWriter::AddLocation(wxCommandEvent& event)
{
	wxString name = event.GetString();

	if ( m_locInChap->FindItem(-1, name) == -1 )
		m_manager->AddElementToDocument(m_manager->GetElementByName(name), m_activeTab.document);
}

void amStoryWriter::AddItem(wxCommandEvent& event)
{
	wxString name = event.GetString();

	if ( m_itemsInChap->FindItem(-1, name) == -1 )
		m_manager->AddElementToDocument(m_manager->GetElementByName(name), m_activeTab.document);
}

void amStoryWriter::UpdateCharacterList()
{
	m_charInChap->Freeze();
	m_charInChap->DeleteAllItems();

	int i = 0;
	for ( Element*& pElement : m_activeTab.document->elements )
	{
		Character* pCharacter = dynamic_cast<Character*>(pElement);

		if ( pCharacter )
			m_charInChap->InsertItem(i++, pCharacter->name);
	}

	m_charInChap->Thaw();
}

void amStoryWriter::UpdateLocationList()
{
	m_locInChap->Freeze();
	m_locInChap->DeleteAllItems();

	int i = 0;
	for ( Element*& pElement: m_activeTab.document->elements )
	{
		Location* pLocation= dynamic_cast<Location*>(pElement);

		if ( pLocation )
			m_locInChap->InsertItem(i++, pLocation->name);
	}
	m_locInChap->Thaw();
}

void amStoryWriter::UpdateItemList()
{
	m_itemsInChap->Freeze();
	m_itemsInChap->DeleteAllItems();

	int i = 0;
	for ( Element*& pElement : m_activeTab.document->elements )
	{
		Item* pItem= dynamic_cast<Item*>(pElement);

		if ( pItem )
			m_itemsInChap->InsertItem(i++, pItem->name);
	}

	m_itemsInChap->Thaw();
}

void amStoryWriter::OnElementActivated(wxListEvent& event)
{
	Element* pElement = m_manager->GetElementByName(event.GetLabel());
	if ( !pElement )
		return;

	m_manager->GoToElement(pElement);
}

void amStoryWriter::OnRemoveCharacter(wxCommandEvent& event)
{
	int sel = m_charInChap->GetFirstSelected();
	int nos = m_charInChap->GetSelectedItemCount();

	for ( int i = 0; i < nos; i++ )
	{
		wxString name = m_charInChap->GetItemText(sel);
		m_charInChap->DeleteItem(sel);

		m_manager->RemoveElementFromDocument(m_manager->GetElementByName(name), m_activeTab.document);

		sel = m_charInChap->GetNextSelected(sel - 1);
	}

	m_manager->GetElementsNotebook()->UpdateCharacterList();
	event.Skip();
}

void amStoryWriter::OnRemoveLocation(wxCommandEvent& event)
{
	int sel = m_locInChap->GetFirstSelected();
	int nos = m_locInChap->GetSelectedItemCount();

	for ( int i = 0; i < nos; i++ )
	{
		wxString name = m_locInChap->GetItemText(sel);
		m_locInChap->DeleteItem(sel);

		m_manager->RemoveElementFromDocument(m_manager->GetElementByName(name), m_activeTab.document);

		sel = m_locInChap->GetNextSelected(sel + 1);
	}

	m_manager->GetElementsNotebook()->UpdateLocationList();
	event.Skip();
}

void amStoryWriter::OnRemoveItem(wxCommandEvent& event)
{
	int sel = m_itemsInChap->GetFirstSelected();
	int nos = m_itemsInChap->GetSelectedItemCount();

	for ( int i = 0; i < nos; i++ )
	{
		wxString name = m_itemsInChap->GetItemText(sel);
		m_itemsInChap->DeleteItem(sel);

		m_manager->RemoveElementFromDocument(m_manager->GetElementByName(name), m_activeTab.document);

		sel = m_itemsInChap->GetNextSelected(sel + 1);
	}

	m_manager->GetElementsNotebook()->UpdateItemList();
	event.Skip();
}

void amStoryWriter::OnStoryItemActivated(wxDataViewEvent& event)
{
	wxDataViewItem item(event.GetItem());
	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

	int selSection, selDocument;

	switch ( node->GetType() )
	{
	case STMN_Book:
		break;

	case STMN_Document:
	{
		Document* pDocument = node->GetDocument();
		if ( pDocument != m_activeTab.document )
		{
			SaveActiveTab();
			LoadDocument(pDocument);
		}

		break;
	}
	case STMN_Trash:
		break;
	}
}

void amStoryWriter::OnStoryItemSelected(wxDataViewEvent& event)
{
	wxDataViewItem item(event.GetItem());
	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

	if ( !node )
		return;

	switch ( node->GetType() )
	{
	case STMN_Book:
		break;

	case STMN_Document:
		break;

	case STMN_Trash:
		break;
	}
}

void amStoryWriter::OnStoryViewRightClick(wxMouseEvent& event)
{
	wxDataViewItem item;
	wxDataViewColumn* col;

	m_storyView->HitTest(event.GetPosition(), item, col);

	if ( !item.IsOk() )
		return;

	StoryTreeModelNode* node = (StoryTreeModelNode*)item.GetID();

	if ( node->IsDocument() )
	{
		wxMenu menu;

		if ( !node->IsInTrash() )
		{
			m_itemForTrash = item;
			menu.Append(MENU_MoveToTrash, "Move to trash");
		}
		else
		{
			m_itemForRestore = item;
			menu.Append(MENU_RestoreFromTrash, "Restore");
		}

		m_storyView->Select(item);
		PopupMenu(&menu);
	}
}

void amStoryWriter::OnMoveToTrash(wxCommandEvent& event)
{
	if ( m_itemForTrash.IsOk() )
	{
		wxBusyCursor cursor;
		StoryTreeModelNode* node = (StoryTreeModelNode*)m_itemForTrash.GetID();

		switch ( m_storyTreeModel->MoveToTrash(m_itemForTrash) )
		{
		case STMN_Document:
		{
			Document* pDocument = node->GetDocument();
			pDocument->isInTrash = true;
			pDocument->Update(m_manager->GetStorage(), false, false);
			m_storyView->Select(m_itemForTrash);
		}
		break;
		}

		m_itemForTrash = wxDataViewItem(0);
	}
}

void amStoryWriter::OnRestoreFromTrash(wxCommandEvent& event)
{
	if ( m_itemForRestore.IsOk() )
	{
		wxBusyCursor cursor;
		StoryTreeModelNode* node = (StoryTreeModelNode*)m_itemForRestore.GetID();

		switch ( m_storyTreeModel->RestoreFromTrash(m_itemForRestore) )
		{
		case STMN_Document:
		{
			Document* pDocument = node->GetDocument();
			pDocument->isInTrash = false;
			pDocument->Update(m_manager->GetStorage(), false, false);
		}
		break;
		}

		m_storyView->Select(m_itemForRestore);
		m_itemForRestore = wxDataViewItem(0);
	}
}

void amStoryWriter::SetActiveTab(amStoryWriterTab& tab, bool saveBefore, bool load)
{
	if ( saveBefore )
		SaveActiveTab();

	if ( load )
		LoadDocument(tab.document);

	m_activeTab = tab;
}

void amStoryWriter::OnNextDocument(wxCommandEvent& event)
{
	wxVector<Document*>& documents = m_manager->GetDocumentsInCurrentBook();
	int currentIndex = m_activeTab.document->position - 1;
	int max = documents.size() - 1;

	if ( currentIndex < max )
	{
		Document* toSet;
		do
		{
			toSet = documents[++currentIndex];
		} while ( toSet->isInTrash && currentIndex < max );

		if ( !toSet->isInTrash )
		{
			SaveActiveTab();
			LoadDocument(toSet);
		}
	}
}

void amStoryWriter::OnPreviousDocument(wxCommandEvent& event)
{
	int currentIndex = m_activeTab.document->position - 1;
	if ( currentIndex > 0 )
	{
		wxVector<Document*>& documents = m_manager->GetDocumentsInCurrentBook();
		Document* toSet;
		do
		{
			toSet = documents[--currentIndex];
		} while ( toSet->isInTrash && currentIndex > 0 );

		if ( !toSet->isInTrash )
		{
			SaveActiveTab();
			LoadDocument(toSet);
		}
	}
}

void amStoryWriter::LoadDocument(Document* document)
{
	if ( m_activeTab.document == document )
		return;

	SetTitle(document->name);

	Freeze();
	m_synopsys->SetValue(document->synopsys);

	bool bIsOpen = false;

	wxAuiNotebook* notebook = m_swNotebook->GetNotebook();
	wxVector<amStoryWriterTab>& tabs = m_swNotebook->GetTabs();

	for ( amStoryWriterTab& tab : tabs )
	{
		if ( tab.document == document )
		{
			m_swNotebook->SetCurrentTab(tab, false);
			m_statusBar->SetStatusText("Zoom: " + std::to_string((int)(tab.rtc->GetFontScale() * 100)) + "%", 2);
			bIsOpen = true;

			break;
		}
	}

	if ( !bIsOpen )
	{
		wxBusyCursor busy;

		wxPanel* mainPanel = new wxPanel(notebook, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
		mainPanel->SetBackgroundColour(wxColour(20, 20, 20));

		wxRichTextCtrl* rtc = new wxRichTextCtrl(mainPanel, TEXT_Content, "", wxDefaultPosition, wxDefaultSize,
			wxRE_MULTILINE | wxBORDER_NONE);
		rtc->SetMinSize(wxSize(750, -1));
		rtc->Bind(wxEVT_RICHTEXT_CHARACTER, &amStoryWriter::OnCharRTC, this);
		rtc->Bind(wxEVT_RICHTEXT_DELETE, &amStoryWriter::OnCharRTC, this);

		wxRichTextAttr attr;
		attr.SetFont(wxFontInfo(10));
		attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);
		attr.SetLeftIndent(64, -64);
		attr.SetLineSpacing(15);
		attr.SetParagraphSpacingBefore(FromDIP(15));
		attr.SetTextColour(wxColour(250, 250, 250));

		rtc->SetBasicStyle(attr);
		rtc->SetBackgroundColour(wxColour(35, 35, 35));

		document->LoadSelf(m_manager->GetStorage(), rtc);

		rtc->GetBuffer().SetMargins(FromDIP(72), FromDIP(72), 0, 0);
		rtc->GetBuffer().Invalidate(wxRICHTEXT_ALL);

		wxScrolledWindow* notePage = new wxScrolledWindow(mainPanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
		notePage->SetBackgroundColour(wxColour(45, 45, 45));
		notePage->EnableScrolling(false, true);
		notePage->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
		notePage->Hide();

		wxWrapSizer* notesSizer = new wxWrapSizer();
		notePage->SetSizer(notesSizer);
		notePage->SetScrollRate(15, 15);

		wxBoxSizer* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
		mainPanelSizer->Add(rtc, wxSizerFlags(1).CentreHorizontal());
		mainPanelSizer->Add(notePage, wxSizerFlags(1).Expand());

		mainPanel->SetSizer(mainPanelSizer);

		m_swNotebook->AddTab(amStoryWriterTab(mainPanel, rtc, notePage, document), document->name);

		for ( Note*& pNote : document->notes )
		{
			AddNote(pNote, false);
		}
	}

	m_storyView->Select(m_storyTreeModel->GetDocumentItem(document));

	m_activeTab.notePanel->Layout();
	m_activeTab.rtc->SetFocus();

	Thaw();

	UpdateCharacterList();
	UpdateLocationList();
	UpdateItemList();

	CheckNotes();
	CountWords();
	m_statusBar->SetStatusText("Document up-to-date", 0);
}

void amStoryWriter::SaveActiveTab()
{
	if ( !m_activeTab.document || !m_activeTab.rtc )
		return;

	bool saveBuffer = false;
	if ( m_activeTab.document->buffer && m_activeTab.rtc )
	{
		//*m_activeTab.document->buffer = m_activeTab.rtc->GetBuffer();
		//m_activeTab.document->buffer->SetBasicStyle(m_activeTab.rtc->GetBasicStyle());
		saveBuffer = true;
	}

	m_activeTab.document->synopsys = m_synopsys->GetValue();
	m_activeTab.document->Update(m_manager->GetStorage(), saveBuffer, true);
}

void amStoryWriter::CountWords()
{
	int count = m_activeTab.document->GetWordCount();
	m_statusBar->SetStatusText("Number of words: " + std::to_string(count), 1);
}

void amStoryWriter::OnTimerEvent(wxTimerEvent& event)
{
	m_statusBar->SetStatusText("Autosaving document...", 0);

	SaveActiveTab();

	m_statusBar->SetStatusText("Counting words...", 0);
	CountWords();

	if ( IsShown() )
		m_manager->GetMainFrame()->Show();

	m_statusBar->SetStatusText("Document up-to-date", 0);

	event.Skip();
}

void amStoryWriter::OnLeftSplitterChanged(wxSplitterEvent& event)
{
	int x = m_charInChap->GetSize().x - 1;

	m_charInChap->SetColumnWidth(0, x);
	m_locInChap->SetColumnWidth(0, x);
	m_itemsInChap->SetColumnWidth(0, x);
}

void amStoryWriter::ToggleFullScreen(bool fs)
{
	ShowFullScreen(fs);
	m_swNotebook->GetToolbar()->ToggleTool(TOOL_StoryFullScreen, fs);
}

void amStoryWriter::OnClose(wxCloseEvent& event)
{
	SaveActiveTab();
	for ( amStoryWriterTab& tab : m_swNotebook->GetTabs() )
	{
		tab.document->CleanUp();
	}

	m_manager->NullifyStoryWriter();

	Destroy();
	event.Skip();
}

void amStoryWriter::OnCharRTC(wxRichTextEvent& event)
{
	event.Skip();

	wxRichTextCtrl* pRtc = (wxRichTextCtrl*)event.GetEventObject();
	Document* pToPush;
	if ( m_activeTab.rtc == pRtc )
	{
		pToPush = m_activeTab.document;
	}
	else
	{
		for ( amStoryWriterTab& tab : m_swNotebook->GetTabs() )
		{
			if ( tab.rtc == pRtc )
			{
				pToPush = tab.document;
				break;
			}
		}
	}

	pToPush->book->PushRecentDocument(pToPush);
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
EVT_TOOL(TOOL_StoryFullScreen, amStoryWriterToolbar::OnFullScreen)

EVT_UPDATE_UI(TOOL_Bold, amStoryWriterToolbar::OnUpdateBold)
EVT_UPDATE_UI(TOOL_Italic, amStoryWriterToolbar::OnUpdateItalic)
EVT_UPDATE_UI(TOOL_Underline, amStoryWriterToolbar::OnUpdateUnderline)
EVT_UPDATE_UI(TOOL_AlignLeft, amStoryWriterToolbar::OnUpdateAlignLeft)
EVT_UPDATE_UI(TOOL_AlignCenter, amStoryWriterToolbar::OnUpdateAlignCenter)
EVT_UPDATE_UI(TOOL_AlignCenterJust, amStoryWriterToolbar::OnUpdateAlignCenterJust)
EVT_UPDATE_UI(TOOL_AlignRight, amStoryWriterToolbar::OnUpdateAlignRight)
EVT_UPDATE_UI(TOOL_FontSize, amStoryWriterToolbar::OnUpdateFontSize)
EVT_UPDATE_UI(TOOL_NoteView, amStoryWriterToolbar::OnUpdateNoteView)
EVT_UPDATE_UI(TOOL_ContentScale, amStoryWriterToolbar::OnUpdateZoom)

EVT_COMBOBOX(TOOL_FontSize, amStoryWriterToolbar::OnFontSize)

END_EVENT_TABLE()

amStoryWriterToolbar::amStoryWriterToolbar(wxWindow* parent,
	long amStyle,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style) : wxToolBar(parent, id, pos, size, style)
{
	SetBackgroundColour(wxColour(30, 30, 30));

	AddCheckTool(TOOL_Bold, "", wxBITMAP_PNG(boldLight), wxBITMAP_PNG(boldLight).ConvertToDisabled(), _("Bold"));
	AddCheckTool(TOOL_Italic, "", wxBITMAP_PNG(italicLight), wxBITMAP_PNG(italicLight).ConvertToDisabled(), _("Italic"));
	AddCheckTool(TOOL_Underline, "", wxBITMAP_PNG(underlineLight), wxBITMAP_PNG(underlineLight).ConvertToDisabled(), _("Underline"));
	AddSeparator();
	AddRadioTool(TOOL_AlignLeft, "", wxBITMAP_PNG(leftAlignLight), wxBITMAP_PNG(leftAlignLight).ConvertToDisabled(), _("Align left"));
	AddRadioTool(TOOL_AlignCenter, "", wxBITMAP_PNG(centerAlignLight), wxBITMAP_PNG(centerAlignLight).ConvertToDisabled(), _("Align center"));
	AddRadioTool(TOOL_AlignCenterJust, "", wxBITMAP_PNG(centerJustAlignLight), wxBITMAP_PNG(centerJustAlignLight).ConvertToDisabled(), _("Align center and fit"));
	AddRadioTool(TOOL_AlignRight, "", wxBITMAP_PNG(rightAlignLight), wxBITMAP_PNG(rightAlignLight).ConvertToDisabled(), _("Align right"));
	AddSeparator();

	wxArrayString sizes;
	for ( int i = 8; i < 28; i++ )
	{
		if ( i > 12 )
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

	AddControl(m_fontSize);
	AddStretchableSpace();

	amStoryWriterNotebook* pSWN = dynamic_cast<amStoryWriterNotebook*>(parent);
	if ( amStyle |= amTB_NOTE_VIEW && pSWN )
	{
		m_parent = pSWN;

		AddCheckTool(TOOL_NoteView, "", wxBITMAP_PNG(noteViewLight), wxBITMAP_PNG(noteViewLight), "Toggle note view");
		AddSeparator();
	}

	if ( amStyle |= amTB_ZOOM )
	{
		m_contentScale = new wxSlider(this, TOOL_ContentScale, 100, 50, 300,
			wxDefaultPosition, wxDefaultSize, wxSL_MIN_MAX_LABELS);
		m_contentScale->SetToolTip("100");
		m_contentScale->SetForegroundColour(wxColour(255, 255, 255));
		AddControl(m_contentScale);
	}

	if ( amStyle |= amTB_FULLSCREEN && pSWN )
	{
		AddSeparator();
		AddCheckTool(TOOL_StoryFullScreen, "", wxBITMAP_PNG(fullScreenPngLight), wxBITMAP_PNG(fullScreenPngLight), "Toggle Full Screen");
	}

	Realize();
}

void amStoryWriterToolbar::OnBold(wxCommandEvent& event)
{
	m_currentTab.rtc->ApplyBoldToSelection();

	if ( m_parent )
		m_parent->OnText(event);
}

void amStoryWriterToolbar::OnItalic(wxCommandEvent& event)
{
	m_currentTab.rtc->ApplyItalicToSelection();

	if ( m_parent )
		m_parent->OnText(event);
}

void amStoryWriterToolbar::OnUnderline(wxCommandEvent& event)
{
	m_currentTab.rtc->ApplyUnderlineToSelection();

	if ( m_parent )
		m_parent->OnText(event);
}

void amStoryWriterToolbar::OnAlignLeft(wxCommandEvent& event)
{
	m_currentTab.rtc->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_LEFT));

	if ( m_parent )
		m_parent->OnText(event);
}

void amStoryWriterToolbar::OnAlignCenter(wxCommandEvent& event)
{
	m_currentTab.rtc->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_CENTER));

	if ( m_parent )
		m_parent->OnText(event);
}

void amStoryWriterToolbar::OnAlignCenterJust(wxCommandEvent& event)
{
	m_currentTab.rtc->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_JUSTIFIED));

	if ( m_parent )
		m_parent->OnText(event);
}

void amStoryWriterToolbar::OnAlignRight(wxCommandEvent& event)
{
	m_currentTab.rtc->ApplyAlignmentToSelection(wxTextAttrAlignment(wxTEXT_ALIGNMENT_RIGHT));

	if ( m_parent )
		m_parent->OnText(event);
}

void amStoryWriterToolbar::OnNoteView(wxCommandEvent& event)
{
	m_parent->Freeze();

	if ( GetToolState(TOOL_NoteView) )
	{
		if ( m_currentTab.rtc->IsShown() )
		{
			m_currentTab.rtc->Hide();
			m_currentTab.notePanel->Show();
			m_currentTab.notePanel->SendSizeEventToParent();
		}
	}
	else
	{
		if ( m_currentTab.notePanel->IsShown() )
		{
			m_currentTab.notePanel->Hide();
			m_currentTab.rtc->Show();
			m_currentTab.rtc->SendSizeEventToParent();
		}
	}

	m_parent->Thaw();
}

void amStoryWriterToolbar::OnTestCircle(wxCommandEvent& event)
{
	wxRichTextRange sel = m_currentTab.rtc->GetSelectionRange();
	wxRichTextBuffer& buf = m_currentTab.rtc->GetBuffer();


	wxRichTextProperties prop;
	prop.SetProperty("commentStart", true);

	buf.InsertFieldWithUndo(&buf, sel.GetStart(), "commentTag", prop, m_currentTab.rtc, 0, m_currentTab.rtc->GetBasicStyle());

	prop.SetProperty("commentStart", false);
	buf.InsertFieldWithUndo(&buf, sel.GetEnd() + 1, "commentTag", prop, m_currentTab.rtc, 0, m_currentTab.rtc->GetBasicStyle());

	for ( wxRichTextObject* it : buf.GetChildren() )
	{
		//it.spli
	}

	wxFFileOutputStream fileStream("F:\\RTCdump.txt");
	wxTextOutputStream textStream(fileStream);

	m_currentTab.rtc->GetBuffer().Dump(textStream);

	fileStream.Close();
}

void amStoryWriterToolbar::OnZoom(wxCommandEvent& event)
{
	int zoom = event.GetInt();

	m_currentTab.rtc->SetFontScale((double)zoom / 100.0, true);
	m_parent->OnZoomChange(zoom);
	m_contentScale->SetToolTip(std::to_string(zoom));
}

void amStoryWriterToolbar::OnFullScreen(wxCommandEvent& WXUNUSED(event))
{
	m_parent->ToggleFullScreen(GetToolState(TOOL_StoryFullScreen));
}

void amStoryWriterToolbar::OnPageView(wxCommandEvent& WXUNUSED(event)) {}

void amStoryWriterToolbar::OnUpdateBold(wxUpdateUIEvent& event)
{
	event.Check(m_currentTab.rtc->IsSelectionBold());
	event.Enable(m_currentTab.rtc && m_currentTab.rtc->IsShown());
}

void amStoryWriterToolbar::OnUpdateItalic(wxUpdateUIEvent& event)
{
	event.Check(m_currentTab.rtc->IsSelectionItalics());
	event.Enable(m_currentTab.rtc && m_currentTab.rtc->IsShown());
}

void amStoryWriterToolbar::OnUpdateUnderline(wxUpdateUIEvent& event)
{
	event.Check(m_currentTab.rtc->IsSelectionUnderlined());
	event.Enable(m_currentTab.rtc && m_currentTab.rtc->IsShown());
}

void amStoryWriterToolbar::OnUpdateAlignLeft(wxUpdateUIEvent& event)
{
	event.Check(m_currentTab.rtc->IsSelectionAligned(wxTEXT_ALIGNMENT_LEFT));
	event.Enable(m_currentTab.rtc && m_currentTab.rtc->IsShown());
}

void amStoryWriterToolbar::OnUpdateAlignCenter(wxUpdateUIEvent& event)
{
	event.Check(m_currentTab.rtc->IsSelectionAligned(wxTEXT_ALIGNMENT_CENTER));
	event.Enable(m_currentTab.rtc && m_currentTab.rtc->IsShown());
}

void amStoryWriterToolbar::OnUpdateAlignCenterJust(wxUpdateUIEvent& event)
{
	event.Check(m_currentTab.rtc->IsSelectionAligned(wxTEXT_ALIGNMENT_JUSTIFIED));
	event.Enable(m_currentTab.rtc && m_currentTab.rtc->IsShown());
}

void amStoryWriterToolbar::OnUpdateAlignRight(wxUpdateUIEvent& event)
{
	event.Check(m_currentTab.rtc->IsSelectionAligned(wxTEXT_ALIGNMENT_RIGHT));
	event.Enable(m_currentTab.rtc && m_currentTab.rtc->IsShown());
}

void amStoryWriterToolbar::OnUpdateFontSize(wxUpdateUIEvent& event)
{
	wxRichTextAttr attr;
	m_currentTab.rtc->GetStyle(m_currentTab.rtc->GetInsertionPoint(), attr);
	wxString size(std::to_string(attr.GetFontSize()));

	if ( m_fontSize->GetValue() != size )
		m_fontSize->SetValue(size);

	event.Enable(m_currentTab.rtc && m_currentTab.rtc->IsShown());
}

void amStoryWriterToolbar::OnUpdateNoteView(wxUpdateUIEvent& event)
{
	event.Check(m_currentTab.notePanel->IsShown());
}

void amStoryWriterToolbar::OnFontSize(wxCommandEvent& event)
{
	m_currentTab.rtc->SetFocus();
	wxRichTextAttr attr;
	attr.SetFlags(wxTEXT_ATTR_FONT_SIZE);
	long size;
	event.GetString().ToLong(&size);
	attr.SetFontSize(size);

	if ( m_currentTab.rtc->HasSelection() )
	{
		m_currentTab.rtc->SetStyleEx(m_currentTab.rtc->GetSelectionRange(), attr,
			wxRICHTEXT_SETSTYLE_WITH_UNDO | wxRICHTEXT_SETSTYLE_OPTIMIZE | wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
	}
	else
	{
		wxRichTextAttr current = m_currentTab.rtc->GetDefaultStyle();
		current.Apply(attr);
		m_currentTab.rtc->SetDefaultStyle(current);
	}
}


void amStoryWriterToolbar::OnUpdateZoom(wxUpdateUIEvent& event)
{
	((wxSlider*)FindControl(TOOL_ContentScale))->SetValue(m_currentTab.rtc->GetFontScale() * 100);
}


////////////////////////////////////////////////////////////////////////////////
/////////////////////////// amStoryWriterNotebook //////////////////////////////
////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(amStoryWriterNotebook, wxPanel)
EVT_TEXT(TEXT_Content, amStoryWriterNotebook::OnText)
END_EVENT_TABLE()

amStoryWriterNotebook::amStoryWriterNotebook(wxWindow* parent, amStoryWriter* documentWriter) :
	wxPanel(parent)
{
	m_storyWriter = documentWriter;

	m_notebook = new wxAuiNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_CLOSE_ON_ALL_TABS |
		wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_BOTTOM | wxBORDER_NONE);
	
	wxAuiSimpleTabArt* pArt = new wxAuiSimpleTabArt();
	pArt->SetColour(wxColour(50, 50, 50));
	pArt->SetActiveColour(wxColour(30, 30, 30));
	m_notebook->SetArtProvider(pArt);

	m_notebook->GetAuiManager().GetArtProvider()->SetColour(wxAUI_DOCKART_BORDER_COLOUR, wxColour(80, 80, 80));
	m_notebook->GetAuiManager().GetArtProvider()->SetColour(wxAUI_DOCKART_SASH_COLOUR, wxColour(40, 40, 40));
	m_notebook->GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 2);
	
	m_notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &amStoryWriterNotebook::OnSelectionChanged, this);
	m_notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &amStoryWriterNotebook::OnPageClosing, this);
	m_notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &amStoryWriterNotebook::OnPageClosed, this);

	m_contentToolbar = new amStoryWriterToolbar(this);
	m_contentToolbar->SetDoubleBuffered(true);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_contentToolbar, wxSizerFlags(0).Expand());
	sizer->Add(m_notebook, wxSizerFlags(1).Expand());

	SetSizer(sizer);

	wxTimer m_timer(this, 12345);
	m_timer.Start(10000);
}

void amStoryWriterNotebook::AddTab(amStoryWriterTab& tab, const wxString& title)
{
	m_swTabs.push_back(tab);
	m_notebook->AddPage(tab.mainPanel, title, false);

	SetCurrentTab(tab, false);
}

void amStoryWriterNotebook::SetCurrentTab(amStoryWriterTab& tab, bool load)
{
	for ( int i = 0; i < m_notebook->GetPageCount(); i++ )
	{
		wxWindow* pPage = m_notebook->GetPage(i);

		if ( pPage == tab.mainPanel )
		{
			m_storyWriter->SetActiveTab(tab, load, load);
			m_contentToolbar->SetCurrentTab(tab);

			m_curTextCtrl = tab.rtc;
			m_curCorkboard = tab.notePanel;

			m_notebook->ChangeSelection(i);
			return;
		}
	}
}

void amStoryWriterNotebook::OnText(wxCommandEvent& WXUNUSED(event))
{
	m_storyWriter->m_statusBar->SetStatusText("Document modified. Autosaving soon...", 0);
}

void amStoryWriterNotebook::OnSelectionChanged(wxAuiNotebookEvent& event)
{
	int sel = event.GetSelection();
	wxWindow* window = m_notebook->GetPage(sel);
	for ( amStoryWriterTab& tab : m_swTabs )
	{
		if ( tab.mainPanel == window )
		{
			m_storyWriter->SetActiveTab(tab, true, true);
			m_contentToolbar->SetCurrentTab(tab);
			return;
		}
	}

	event.Skip();
}

void amStoryWriterNotebook::OnPageClosing(wxAuiNotebookEvent& event)
{
	if ( m_notebook->GetPageCount() == 1 )
	{
		event.Veto();
	}
	else
	{
		wxWindow* page = m_notebook->GetPage(event.GetSelection());
		for ( amStoryWriterTab& tab : m_swTabs )
		{
			if ( tab.mainPanel == page )
			{
				m_closingTab = tab;

				// Nullify the page about to be closed but leave the other one as it is
				if ( tab.rtc == page )
					m_closingTab.rtc = nullptr;
				else
					m_closingTab.notePanel = nullptr;

				break;
			}
		}
	}
}

void amStoryWriterNotebook::OnZoomChange(int zoom)
{
	m_storyWriter->PushStatusText("Zoom: " + std::to_string(zoom) + "%", 2);
}

void amStoryWriterNotebook::OnPageClosed(wxAuiNotebookEvent& event)
{
	if ( !m_closingTab.document )
		return;

	m_closingTab.document->CleanUp();

	for ( amStoryWriterTab& tab : m_swTabs )
	{
		if ( m_closingTab.document == tab.document )
		{
			m_swTabs.erase(&tab);
			break;
		}
	}

	m_closingTab.document = nullptr;
	m_closingTab.mainPanel = nullptr;
	m_closingTab.rtc = nullptr;
	m_closingTab.notePanel = nullptr;

	event.Skip();
}

bool amStoryWriterNotebook::HasRedNote()
{
	for ( Note*& pNote : m_storyWriter->GetActiveTab().document->notes )
	{
		if ( !pNote->isDone )
			return true;
	}

	return false;
}

void amStoryWriterNotebook::AddNote(Note* note)
{
	wxPanel* notePanel = new wxPanel(m_curCorkboard);

	wxTextCtrl* noteLabel = new wxTextCtrl(notePanel, -1, note->name);
	noteLabel->SetBackgroundColour(wxColour(240, 240, 240));
	wxPanel* options = new wxPanel(notePanel, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	options->SetBackgroundColour(wxColour(200, 200, 200));
	wxTextCtrl* noteContent = new wxTextCtrl(notePanel, -1, note->content, wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE);
	noteContent->SetBackgroundColour(wxColour(255, 250, 205));

	noteLabel->Bind(wxEVT_TEXT, &amStoryWriterNotebook::UpdateNoteLabel, this);
	noteContent->Bind(wxEVT_TEXT, &amStoryWriterNotebook::UpdateNote, this);

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
	siz->Add(noteContent, wxSizerFlags(1).Expand());
	siz->Add(bsiz, wxSizerFlags(0).Expand());
	notePanel->SetSizer(siz);

	if ( note->isDone )
	{
		notePanel->SetBackgroundColour(wxColour(0, 140, 0));
		b2->SetValue(true);
	}
	else
	{
		notePanel->SetBackgroundColour(wxColour(120, 0, 0));
		b1->SetValue(true);
	}

	wxWrapSizer* notesSizer = (wxWrapSizer*)m_curCorkboard->GetSizer();
	notesSizer->Add(notePanel, wxSizerFlags(1).Expand().Border(wxLEFT | wxTOP, 22));
	notesSizer->SetItemMinSize(notesSizer->GetItemCount() - 1, m_noteSize);
	notesSizer->Layout();

	notesSizer->FitInside(m_curCorkboard);
}

void amStoryWriterNotebook::PaintDots(wxPaintEvent& event)
{
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

void amStoryWriterNotebook::SetRed(wxCommandEvent& event)
{
	wxRadioButton* rb = (wxRadioButton*)event.GetEventObject();
	wxPanel* np = (wxPanel*)rb->GetParent();

	wxPanel* sp;
	int i = 0;
	for ( Note*& pNote : m_storyWriter->GetActiveTab().document->notes )
	{
		sp = (wxPanel*)m_curCorkboard->GetSizer()->GetItem(i)->GetWindow();

		if ( sp == np )
		{
			pNote->isDone = false;
			np->SetBackgroundColour(wxColour(120, 0, 0));
			np->Refresh();
			break;
		}

		i++;
	}

	m_storyWriter->CheckNotes();
}

void amStoryWriterNotebook::SetGreen(wxCommandEvent& event)
{
	wxRadioButton* gb = (wxRadioButton*)event.GetEventObject();
	wxPanel* np = (wxPanel*)gb->GetParent();

	wxPanel* sp;
	int i = 0;
	for ( Note*& pNote : m_storyWriter->GetActiveTab().document->notes )
	{
		sp = (wxPanel*)m_curCorkboard->GetSizer()->GetItem(i)->GetWindow();

		if ( sp == np )
		{
			pNote->isDone = true;
			np->SetBackgroundColour(wxColour(0, 140, 0));
			np->Refresh();
			break;
		}

		i++;
	}

	m_storyWriter->CheckNotes();
}

void amStoryWriterNotebook::OnDeleteNote(wxCommandEvent& WXUNUSED(event))
{
	if ( m_selNote )
	{
		wxPanel* sp;

		int i = 0;
		Document* pDocument = m_storyWriter->GetActiveTab().document;
		for ( Note*& pNote : pDocument->notes )
		{
			sp = (wxPanel*)m_curCorkboard->GetSizer()->GetItem(i)->GetWindow();

			if ( sp == m_selNote )
			{
				delete pNote;
				pDocument->notes.erase(&pNote);
				m_selNote->Destroy();
				m_curCorkboard->GetSizer()->Layout();
				m_curCorkboard->FitInside();
				break;
			}

			i++;
		}


		m_selNote = nullptr;
	}

	m_storyWriter->CheckNotes();
}

void amStoryWriterNotebook::OnNoteClick(wxMouseEvent& event)
{
	wxPanel* pan = (wxPanel*)event.GetEventObject();

	m_selNote = (wxPanel*)pan->GetParent();

	wxMenu menu;
	menu.Append(MENU_Delete, "Delete");
	menu.Bind(wxEVT_MENU, &amStoryWriterNotebook::OnDeleteNote, this, MENU_Delete);
	pan->PopupMenu(&menu, wxPoint(-1, pan->GetSize().y));
}

void amStoryWriterNotebook::UpdateNoteLabel(wxCommandEvent& event)
{
	wxTextCtrl* ttc = (wxTextCtrl*)event.GetEventObject();
	wxPanel* pan = (wxPanel*)ttc->GetParent();

	wxPanel* sp;
	int i = 0;
	for ( Note*& pNote : m_storyWriter->GetActiveTab().document->notes )
	{
		sp = (wxPanel*)m_curCorkboard->GetSizer()->GetItem(i)->GetWindow();
		if ( sp == pan )
		{
			pNote->name = ttc->GetValue();
		}

		i++;
	}
}

void amStoryWriterNotebook::UpdateNote(wxCommandEvent& event)
{
	wxTextCtrl* trtc = (wxTextCtrl*)event.GetEventObject();
	wxPanel* pan = (wxPanel*)trtc->GetParent();

	wxPanel* sp;
	int i = 0;
	for ( Note*& pNote : m_storyWriter->GetActiveTab().document->notes )
	{
		sp = (wxPanel*)m_curCorkboard->GetSizer()->GetItem(i)->GetWindow();
		if ( sp == pan )
		{
			pNote->content = trtc->GetValue();
		}

		i++;
	}
}