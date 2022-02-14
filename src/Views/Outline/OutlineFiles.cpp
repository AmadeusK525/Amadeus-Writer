#include "Views/Outline/OutlineFiles.h"

#include "Views/MainFrame.h"
#include "MyApp.h"

#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include <wx/richtext/richtextxml.h>
#include <wx/colordlg.h>

#include <thread>

#include "Utils/wxmemdbg.h"

wxVector<wxIcon> OutlineTreeModelNode::m_icons{};

OutlineTreeModel::OutlineTreeModel()
{
	OutlineTreeModelNode::InitAllIcons();

	m_research = new OutlineTreeModelNode(nullptr, _("Research"), true);
	m_characters = new OutlineTreeModelNode(nullptr, _("Characters"), true);
	m_locations = new OutlineTreeModelNode(nullptr, _("Locations"), true);
	m_items = new OutlineTreeModelNode(nullptr, _("Items"), true);
}

wxRichTextBuffer* OutlineTreeModel::GetBuffer(const wxDataViewItem& item) const
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node->GetBuffer();
}

wxDataViewItem OutlineTreeModel::AddToResearch(const wxString& title)
{
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_research, title, false);
	m_research->Append(node);
	wxDataViewItem parent(m_research);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::AddToCharacters(const wxString& title)
{
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_characters, title, false);
	wxDataViewItem parent(m_characters);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::AddToLocations(const wxString& title)
{
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_locations, title, false);
	wxDataViewItem parent(m_locations);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::AddToItems(const wxString& title)
{
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_items, title, false);
	wxDataViewItem parent(m_items);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::AppendFile(const wxDataViewItem& parent, const wxString& name)
{
	OutlineTreeModelNode* parentNode = (OutlineTreeModelNode*)parent.GetID();
	OutlineTreeModelNode* node = new OutlineTreeModelNode(parentNode, name, false);
	wxDataViewItem item(node);

	if ( !parentNode )
		m_otherRoots.push_back(node);

	ItemAdded(parent, item);

	return item;
}

wxDataViewItem OutlineTreeModel::AppendFolder(const wxDataViewItem& parent, const wxString& name)
{
	OutlineTreeModelNode* parentNode = (OutlineTreeModelNode*)parent.GetID();
	OutlineTreeModelNode* node = new OutlineTreeModelNode(parentNode, name, true);
	wxDataViewItem item(node);

	if ( !parentNode )
		m_otherRoots.push_back(node);

	ItemAdded(parent, item);

	return item;
}

bool OutlineTreeModel::IsResearch(const wxDataViewItem& item)
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_research;
}

bool OutlineTreeModel::IsCharacters(const wxDataViewItem& item)
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_characters;
}

bool OutlineTreeModel::IsLocations(const wxDataViewItem& item)
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_locations;
}

bool OutlineTreeModel::IsItems(const wxDataViewItem& item)
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_items;
}

bool OutlineTreeModel::IsSpecial(const wxDataViewItem& item)
{
	wxDataViewItem parent = GetParent(item);
	return IsResearch(item) || IsCharacters(item) || IsLocations(item) || IsItems(item) ||
		IsCharacters(parent) || IsLocations(parent) || IsItems(parent);
}

void OutlineTreeModel::DeleteItem(const wxDataViewItem& item)
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	if ( !node )      // happens if item.IsOk()==false
		return;

	if ( node == m_research || node == m_characters || node == m_locations )
	{
		wxMessageBox(_("Cannot remove special folders!"));
		return;
	}

	if ( m_handler )
	{
		if ( item == m_handler->GetItemUnderMouse() )
			m_handler->SetItemUnderMouse(wxDataViewItem(nullptr));
	}

	// first remove the node from the parent's array of children;
	// NOTE: OutlineTreeModelNode is only an array of _pointers_
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

	wxDataViewItem parent(parentNode);
	ItemDeleted(parent, item);

	// free the node
	delete node;
}

bool OutlineTreeModel::Reposition(const wxDataViewItem& item, int n)
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	if ( node )
	{
		node->Reposition(n);
		ItemChanged(wxDataViewItem(node->GetParent()));
	}
	else
		return false;

	return true;
}

void OutlineTreeModel::Clear()
{
	wxVector<amTreeModelNode*> array;

	array = m_research->GetChildren();
	for ( unsigned int i = 0; i < array.size(); i++ )
	{
		DeleteItem(wxDataViewItem(array[i]));
	}

	array = m_characters->GetChildren();
	for ( unsigned int i = 0; i < array.size(); i++ )
	{
		DeleteItem(wxDataViewItem(array[i]));
	}

	array = m_locations->GetChildren();
	for ( unsigned int i = 0; i < array.size(); i++ )
	{
		DeleteItem(wxDataViewItem(array[i]));
	}

	for ( unsigned int i = 0; i < m_otherRoots.size(); i++ )
		DeleteItem(wxDataViewItem(m_otherRoots[i]));
}

void OutlineTreeModel::GetValue(wxVariant& variant,
	const wxDataViewItem& item, unsigned int col) const
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	wxDataViewIconText it;
	it.SetText(node->GetTitle());

	if ( node == m_research )
		it.SetIcon(node->m_icons[0]);
	else if ( node->IsContainer() )
		it.SetIcon(node->m_icons[1]);
	else
		it.SetIcon(node->m_icons[2]);

	switch ( col )
	{
	case 0:
		variant << it;
		break;
	default:
		break;
	}
}

unsigned int OutlineTreeModel::GetChildren(const wxDataViewItem& parent,
	wxDataViewItemArray& array) const
{
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)parent.GetID();
	if ( !node )
	{
		int n = 0;

		if ( m_research )
		{
			array.Add(wxDataViewItem(m_research));
			n++;
		}

		if ( m_characters )
		{
			array.Add(wxDataViewItem(m_characters));
			n++;
		}

		if ( m_locations )
		{
			array.Add(wxDataViewItem(m_locations));
			n++;
		}

		if ( m_items )
		{
			array.Add(wxDataViewItem(m_items));
			n++;
		}

		for ( unsigned int i = 0; i < m_otherRoots.size(); i++ )
			array.Add(wxDataViewItem(m_otherRoots[i]));


		return n + m_otherRoots.size();
	}

	int count = node->GetChildCount();

	if ( count == 0 )
	{
		return 0;
	}

	for ( int pos = 0; pos < count; pos++ )
	{
		amTreeModelNode* child = node->GetChild(pos);
		array.Add(wxDataViewItem(child));
	}

	return count;
}

void OutlineTreeModel::OnBeginDrag(wxDataViewEvent& event, wxDataViewCtrl* dvc)
{
	wxDataViewItem item(event.GetItem());

	if ( IsSpecial(item) )
	{
		event.Veto();
		return;
	}

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	wxTextDataObject* obj = new wxTextDataObject;
	obj->SetText(node->GetTitle());

	event.SetDataObject(obj);
	event.SetDataFormat(wxDF_UNICODETEXT);
	event.SetDragFlags(wxDrag_AllowMove); // allows both copy and move

	m_itemForDnD = item;
}

void OutlineTreeModel::OnDropPossible(wxDataViewEvent& event, wxDataViewCtrl* dvc)
{
	if ( event.GetDataFormat() != wxDF_UNICODETEXT )
	{
		m_itemForDnD = wxDataViewItem(nullptr);
		event.Veto();
	}
	else
	{
		wxDataViewItem ht;
		wxDataViewColumn* cht = dvc->GetColumn(0);

		dvc->HitTest(dvc->ScreenToClient(wxGetMousePosition()), ht, cht);
		wxDataViewItem htParent = GetParent(ht);

		if ( !IsDescendant(m_itemForDnD, ht) &&
			!((IsSpecial(ht) || IsSpecial(htParent)) && (!IsResearch(ht) && !IsResearch(htParent))) )
			event.SetDropEffect(wxDragMove);
		else
			event.SetDropEffect(wxDragNone);
	}
}

void OutlineTreeModel::OnDrop(wxDataViewEvent& event, wxDataViewCtrl* dvc)
{
	wxDataViewItem item(event.GetItem());

	if ( IsCharacters(item) || IsLocations(item) || IsItems(item) || IsDescendant(m_itemForDnD, item) || m_itemForDnD == item )
	{
		UnsetItemForDnD();
		return;
	}

	if ( event.GetDataFormat() != wxDF_UNICODETEXT )
	{
		event.Veto();
		UnsetItemForDnD();
		return;
	}

	int index = event.GetProposedDropIndex();

	if ( item.IsOk() )
	{
		if ( IsContainer(item) )
		{
			if ( m_itemForDnD.IsOk() )
			{
				if ( GetParent(m_itemForDnD) != item )
				{
					if ( index == -1 )
						Reparent(m_itemForDnD, item);
					else
						Reparent(m_itemForDnD, item, index);
				}
				else
				{
					if ( index == -1 )
						Reposition(m_itemForDnD, 0);
					else
						Reposition(m_itemForDnD, index);
				}
			}
		}
		else
			wxLogMessage(_("Dropped on item. Nothing happened."));

	}
	else
	{
		if ( GetParent(m_itemForDnD) != item )
		{
			if ( index > 3 )
				Reparent(m_itemForDnD, wxDataViewItem(nullptr), index - 4);
			else
				Reparent(m_itemForDnD, wxDataViewItem(nullptr));
		}
	}

	dvc->Select(m_itemForDnD);
	UnsetItemForDnD();
}


BEGIN_EVENT_TABLE(amOutlineFilesPanel, wxSplitterWindow)

EVT_DATAVIEW_SELECTION_CHANGED(TREE_Files, amOutlineFilesPanel::OnSelectionChanged)

EVT_DATAVIEW_ITEM_EDITING_STARTED(TREE_Files, amOutlineFilesPanel::OnEditingStart)
EVT_DATAVIEW_ITEM_EDITING_DONE(TREE_Files, amOutlineFilesPanel::OnEditingEnd)

EVT_DATAVIEW_ITEM_EXPANDED(TREE_Files, amOutlineFilesPanel::OnItemExpanded)
EVT_DATAVIEW_ITEM_COLLAPSED(TREE_Files, amOutlineFilesPanel::OnItemCollapsed)

EVT_DATAVIEW_ITEM_DROP(TREE_Files, amOutlineFilesPanel::OnDrop)

EVT_TOOL(TOOL_NewFile, amOutlineFilesPanel::NewFile)
EVT_TOOL(TOOL_NewFolder, amOutlineFilesPanel::NewFolder)

EVT_TIMER(TIMER_OutlineFiles, amOutlineFilesPanel::OnTimerEvent)

END_EVENT_TABLE()

amOutlineFilesPanel::amOutlineFilesPanel(wxWindow* parent) : amSplitterWindow(parent, -1, wxDefaultPosition,
	wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_NOBORDER | wxSP_THIN_SASH | wxSP_NO_XP_THEME)
{
	SetBackgroundColour(wxColour(20, 20, 20));
	SetDoubleBuffered(true);

	m_textCtrl = new wxRichTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, 32L | wxBORDER_NONE);
	m_textCtrl->SetBackgroundColour(wxColour(40, 40, 40));
	m_textCtrl->Refresh();

	m_basicAttr.SetFontSize(13);
	m_basicAttr.SetTextColour(wxColour(245, 245, 245));

	m_textCtrl->SetBasicStyle(m_basicAttr);

	m_leftPanel = new wxPanel(this);

	m_outlineTreeModel = new OutlineTreeModel;
	m_files = m_filesHTHandler.Create(m_leftPanel, TREE_Files, m_outlineTreeModel.get());

	m_files->GetMainWindow()->Bind(wxEVT_KEY_DOWN, &amOutlineFilesPanel::OnKeyDownDataView, this);
	m_files->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &amOutlineFilesPanel::OnRightDownDataView, this);

	m_filesTB = new wxToolBar(m_leftPanel, -1);
	m_filesTB->AddTool(TOOL_NewFile, "", wxBITMAP_PNG(addFile), _("Add new file."));
	m_filesTB->AddTool(TOOL_NewFolder, "", wxBITMAP_PNG(addFolder), _("Add new folder."));
	m_filesTB->SetBackgroundColour(wxColour(30, 30, 30));

	m_filesTB->Realize();

	m_files->EnableDragSource(wxDF_UNICODETEXT);
	m_files->EnableDropTarget(wxDF_UNICODETEXT);
	//m_files->EnableDragSource(wxDataFormat(wxDF_FILENAME));
	//m_files->EnableDragSource(wxDataFormat(wxDF_FILENAME));
	m_files->SetBackgroundColour(wxColour(100, 100, 100));
	m_files->GetMainWindow()->SetBackgroundColour(wxColour(20, 20, 20));

	wxDataViewIconTextRenderer* itr = new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType(),
		wxDATAVIEW_CELL_EDITABLE);
	itr->EnableEllipsize(wxELLIPSIZE_END);
	wxDataViewColumn* column0 = new wxDataViewColumn(_("Files"), itr, 0, FromDIP(200), wxALIGN_LEFT);
	m_files->AppendColumn(column0);

	wxBoxSizer* panSizer = new wxBoxSizer(wxVERTICAL);
	panSizer->Add(m_filesTB, wxSizerFlags(0).Expand());
	panSizer->Add(m_files, wxSizerFlags(1).Expand());

	m_leftPanel->SetSizer(panSizer);

	m_textCtrl->SetMinSize(wxSize(20, -1));
	m_leftPanel->SetMinSize(wxSize(20, -1));

	SetMinimumPaneSize(FromDIP(60));
	SplitVertically(m_leftPanel, m_textCtrl, 200);

	m_timer.Start(10000);
}

void amOutlineFilesPanel::Init()
{
	for ( am::StoryElement*& pElement : am::GetAllElements() )
	{
		AppendStoryElement(pElement);
	}
}

void amOutlineFilesPanel::AppendStoryElement(am::StoryElement* element)
{
	if ( !element )
		return;

	wxDataViewItem item;
	if ( element->IsKindOf(wxCLASSINFO(am::Character)) )
		item = m_outlineTreeModel->AddToCharacters(element->name.ToStdString());
	else if ( element->IsKindOf(wxCLASSINFO(am::Location)) )
		item = m_outlineTreeModel->AddToLocations(element->name.ToStdString());
	else if ( element->IsKindOf(wxCLASSINFO(am::Item)) )
		item = m_outlineTreeModel->AddToItems(element->name.ToStdString());

	GenerateElementBuffer(element, ((OutlineTreeModelNode*)item.GetID())->GetBuffer());
}

void amOutlineFilesPanel::DeleteStoryElement(am::StoryElement* element)
{
	if ( element->IsKindOf(wxCLASSINFO(am::Character)) )
		DeleteCharacter((am::Character*)element);
	else if ( element->IsKindOf(wxCLASSINFO(am::Location)) )
		DeleteLocation((am::Location*)element);
	else if ( element->IsKindOf(wxCLASSINFO(am::Item)) )
		DeleteItem((am::Item*)element);
}

void amOutlineFilesPanel::GenerateElementBuffer(am::StoryElement* element, wxRichTextBuffer* buffer)
{
	if ( !element || !buffer )
		return;

	buffer->SetBasicStyle(m_textCtrl->GetBasicStyle());
	buffer->BeginSuppressUndo();

	int begin = 0;

	if ( element->image.IsOk() )
	{
		wxImage image = element->image;
		double ratio = (double)image.GetWidth() / (double)image.GetHeight();

		if ( ratio > 1 )
			image.Rescale(180 * ratio, 180, wxIMAGE_QUALITY_HIGH);
		else
			image.Rescale(220 * ratio, 220, wxIMAGE_QUALITY_HIGH);

		buffer->BeginAlignment(wxTEXT_ALIGNMENT_CENTER);
		buffer->AddImage(image);
		buffer->EndAlignment();

		begin += 2;
	}

	buffer->BeginFontSize(16);
	buffer->BeginBold();
	buffer->InsertTextWithUndo(begin, "\n" + _("Name: "), nullptr);
	buffer->EndBold();
	buffer->InsertTextWithUndo(buffer->GetText().size(), " " + element->name, nullptr);
	buffer->EndFontSize();

	if ( element->IsKindOf(wxCLASSINFO(am::Item)) )
	{

	}

	wxString separator("\n\n");

	for ( auto& it : element->mShortAttributes )
	{
		buffer->BeginBold();
		buffer->InsertTextWithUndo(buffer->GetText().size(), separator + it.first + ": ", nullptr);
		buffer->EndBold();
		buffer->InsertTextWithUndo(buffer->GetText().size(), it.second, nullptr);

		if ( separator != '\n' )
			separator = '\n';
	}

	for ( auto& it : element->mLongAttributes )
	{
		buffer->BeginBold();
		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\n" + it.first + "\n", nullptr);
		buffer->EndBold();
		buffer->InsertTextWithUndo(buffer->GetText().size(), it.second, nullptr);
	}

	buffer->SetName(element->name);
}

//void amOutlineFilesPanel::GenerateCharacterBuffer(am::Character* character, wxRichTextBuffer* buffer)
//{
//	buffer->SetBasicStyle(m_textCtrl->GetBasicStyle());
//	buffer->BeginSuppressUndo();
//
//	int begin = 0;
//
//	if ( character->image.IsOk() )
//	{
//		wxImage image = character->image;
//		double ratio = (double)image.GetWidth() / (double)image.GetHeight();
//
//		if ( ratio > 1 )
//			image.Rescale(180 * ratio, 180, wxIMAGE_QUALITY_HIGH);
//		else
//			image.Rescale(220 * ratio, 220, wxIMAGE_QUALITY_HIGH);
//
//		buffer->BeginAlignment(wxTEXT_ALIGNMENT_CENTER);
//		buffer->AddImage(image);
//		buffer->EndAlignment();
//
//		begin += 2;
//	}
//
//	buffer->BeginFontSize(16);
//	buffer->BeginBold();
//	buffer->InsertTextWithUndo(begin, "\n" + _("Name: "), nullptr);
//	buffer->EndBold();
//	buffer->InsertTextWithUndo(buffer->GetText().size(), " " + character->name, nullptr);
//	buffer->EndFontSize();
//
//	wxString separator("\n\n");
//
//	if ( !character->age.IsEmpty() )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), separator + _("Age: "), nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), character->age, nullptr);
//		separator = "\n";
//	}
//
//	if ( character->sex != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), separator + _("Sex: "), nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), character->sex, nullptr);
//		separator = "\n";
//	}
//
//	if ( character->height != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), separator + _("Height: "), nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), character->height, nullptr);
//		separator = "\n";
//	}
//
//	if ( character->nick != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), separator + _("Nickname: "), nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), character->nick, nullptr);
//		separator = "\n";
//	}
//
//	if ( character->nat != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), separator + _("Nationality: "), nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), character->nat, nullptr);
//		separator = "\n";
//	}
//
//	if ( character->appearance != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\n" + _("Appearance:") + "\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), character->appearance, nullptr);
//	}
//
//	if ( character->personality != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\n" + _("Personality:") + "\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), character->personality, nullptr);
//	}
//
//	if ( character->backstory != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\n" + _("Backstory:") + "\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), character->backstory, nullptr);
//	}
//
//	for ( pair<wxString, wxString>& it : character->custom )
//	{
//		if ( it.second != "" )
//		{
//			buffer->BeginBold();
//			buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\n" + it.first + ":\n", nullptr);
//			buffer->EndBold();
//			buffer->InsertTextWithUndo(buffer->GetText().size(), it.second, nullptr);
//		}
//	}
//
//	buffer->SetName(character->name);
//}
//
//void amOutlineFilesPanel::GenerateLocationBuffer(am::Location* location, wxRichTextBuffer* buffer)
//{
//	buffer->SetBasicStyle(m_textCtrl->GetBasicStyle());
//	buffer->BeginSuppressUndo();
//
//	if ( location->image.IsOk() )
//	{
//		wxImage image = location->image;
//		double ratio = (double)image.GetWidth() / (double)image.GetHeight();
//
//		if ( ratio > 1 )
//			image.Rescale(180 * ratio, 180, wxIMAGE_QUALITY_HIGH);
//		else
//			image.Rescale(220 * ratio, 220, wxIMAGE_QUALITY_HIGH);
//
//		buffer->BeginAlignment(wxTEXT_ALIGNMENT_CENTER);
//		buffer->AddImage(image);
//		buffer->EndAlignment();
//	}
//
//	buffer->BeginFontSize(16);
//	buffer->BeginBold();
//	buffer->InsertTextWithUndo(2, "\nName: ", nullptr);
//	buffer->EndBold();
//	buffer->InsertTextWithUndo(buffer->GetText().size(), " " + location->name, nullptr);
//	buffer->EndFontSize();
//
//	buffer->BeginBold();
//	buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\nImportance: ", nullptr);
//	buffer->EndBold();
//	wxString role;
//	switch ( location->role )
//	{
//	case lHigh:
//		role = "High";
//		break;
//
//	case lLow:
//		role = "Low";
//		break;
//
//	default:
//		role = "--/--";
//		break;
//	}
//	buffer->InsertTextWithUndo(buffer->GetText().size(), role, nullptr);
//
//	if ( location->general != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\nGeneral:\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), location->general, nullptr);
//	}
//
//	if ( location->natural != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\nNatural characteristics:\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), location->natural, nullptr);
//	}
//
//	if ( location->architecture != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\nArchitecture:\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), location->architecture, nullptr);
//	}
//
//	if ( location->politics != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\nPolitics:\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), location->politics, nullptr);
//	}
//
//	if ( location->economy != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\nEconomy:\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), location->economy, nullptr);
//	}
//
//	if ( location->culture != "" )
//	{
//		buffer->BeginBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\nCulture:\n", nullptr);
//		buffer->EndBold();
//		buffer->InsertTextWithUndo(buffer->GetText().size(), location->culture, nullptr);
//	}
//
//	for ( pair<wxString, wxString>& it : location->custom )
//	{
//		if ( it.second != "" )
//		{
//			buffer->BeginBold();
//			buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\n\n\n" + it.first + ":\n", nullptr);
//			buffer->EndBold();
//			buffer->InsertTextWithUndo(buffer->GetText().size(), it.second, nullptr);
//		}
//	}
//
//	buffer->SetName(location->name);
//}
//
//void amOutlineFilesPanel::GenerateItemBuffer(am::Item* item, wxRichTextBuffer* buffer)
//{
//	buffer->SetBasicStyle(m_textCtrl->GetBasicStyle());
//	buffer->BeginSuppressUndo();
//
//	if ( item->image.IsOk() )
//	{
//		wxImage image = item->image;
//		double ratio = (double)image.GetWidth() / (double)image.GetHeight();
//
//		if ( ratio > 1 )
//			image.Rescale(180 * ratio, 180, wxIMAGE_QUALITY_HIGH);
//		else
//			image.Rescale(220 * ratio, 220, wxIMAGE_QUALITY_HIGH);
//
//		buffer->BeginAlignment(wxTEXT_ALIGNMENT_CENTER);
//		buffer->AddImage(image);
//		buffer->EndAlignment();
//	}
//
//	buffer->BeginFontSize(16);
//	buffer->BeginBold();
//	buffer->InsertTextWithUndo(2, "\nName: ", nullptr);
//	buffer->EndBold();
//	buffer->InsertTextWithUndo(buffer->GetText().size(), " " + item->name, nullptr);
//	buffer->EndFontSize();
//
//	buffer->BeginBold();
//	buffer->InsertTextWithUndo(buffer->GetText().size(), "\n\nImportance: ", nullptr);
//	buffer->EndBold();
//	wxString role;
//	switch ( item->role )
//	{
//	case iHigh:
//		role = "High";
//		break;
//
//	case iLow:
//		role = "Low";
//		break;
//
//	default:
//		role = "--/--";
//		break;
//	}
//	buffer->InsertTextWithUndo(buffer->GetText().size(), role, nullptr);
//
//	buffer->SetName(item->name);
//}
//
//void amOutlineFilesPanel::AppendCharacter(am::Character* character)
//{
//	wxDataViewItem item = m_outlineTreeModel->AddToCharacters(character->name.ToStdString());
//	GenerateCharacterBuffer(character, ((OutlineTreeModelNode*)item.GetID())->GetBuffer());
//}
//
//void amOutlineFilesPanel::AppendLocation(am::Location* location)
//{
//	wxDataViewItem item = m_outlineTreeModel->AddToLocations(location->name.ToStdString());
//	GenerateLocationBuffer(location, ((OutlineTreeModelNode*)item.GetID())->GetBuffer());
//}
//
//void amOutlineFilesPanel::AppendItem(am::Item* item)
//{
//	wxDataViewItem dvitem = m_outlineTreeModel->AddToItems(item->name.ToStdString());
//	GenerateItemBuffer(item, ((OutlineTreeModelNode*)dvitem.GetID())->GetBuffer());
//}

void amOutlineFilesPanel::DeleteCharacter(am::Character* character)
{
	wxVector<amTreeModelNode*>& characters = m_outlineTreeModel->GetCharacters();
	for ( unsigned int i = 0; i < characters.size(); i++ )
	{
		if ( characters[i]->GetTitle() == character->name )
		{
			if ( m_currentNode == characters[i] )
			{
				m_currentNode = nullptr;
				m_textCtrl->Clear();
			}

			wxDataViewItem item(characters[i]);
			m_outlineTreeModel->DeleteItem(item);
			return;
		}
	}
}

void amOutlineFilesPanel::DeleteLocation(am::Location* location)
{
	wxVector<amTreeModelNode*>& locations = m_outlineTreeModel->GetLocations();
	for ( unsigned int i = 0; i < locations.size(); i++ )
	{
		if ( locations[i]->GetTitle() == location->name )
		{
			if ( m_currentNode == locations[i] )
			{
				m_currentNode = nullptr;
				m_textCtrl->Clear();
			}

			wxDataViewItem item(locations[i]);
			m_outlineTreeModel->DeleteItem(item);
			return;
		}
	}
}

void amOutlineFilesPanel::DeleteItem(am::Item* item)
{
	wxVector<amTreeModelNode*>& items = m_outlineTreeModel->GetItems();
	for ( unsigned int i = 0; i < items.size(); i++ )
	{
		if ( items[i]->GetTitle() == item->name )
		{
			if ( m_currentNode == items[i] )
			{
				m_currentNode = nullptr;
				m_textCtrl->Clear();
			}

			wxDataViewItem dvitem(items[i]);
			m_outlineTreeModel->DeleteItem(dvitem);
			return;
		}
	}
}

void amOutlineFilesPanel::NewFile(wxCommandEvent& event)
{
	wxDataViewItem sel = m_files->GetSelection();

	if ( sel.IsOk() )
	{
		if ( !m_outlineTreeModel->IsContainer(sel) )
			sel = m_outlineTreeModel->GetParent(sel);
	}

	if ( m_outlineTreeModel->IsCharacters(sel) || m_outlineTreeModel->IsLocations(sel) || m_outlineTreeModel->IsItems(sel) )
		return;

	wxDataViewItem item = m_outlineTreeModel->AppendFile(sel, "New file");
	m_files->Select(item);
	m_currentNode = (OutlineTreeModelNode*)item.GetID();

	Save();
}

void amOutlineFilesPanel::NewFolder(wxCommandEvent& event)
{
	wxDataViewItem sel = m_files->GetSelection();

	if ( sel.IsOk() )
	{
		if ( !m_outlineTreeModel->IsContainer(sel) )
			sel = m_outlineTreeModel->GetParent(sel);
	}

	if ( m_outlineTreeModel->IsCharacters(sel) || m_outlineTreeModel->IsLocations(sel) || m_outlineTreeModel->IsItems(sel) )
		return;

	wxDataViewItem item = m_outlineTreeModel->AppendFolder(sel, "New folder");
	m_files->Select(item);
	m_currentNode = (OutlineTreeModelNode*)item.GetID();
	Save();
}

void amOutlineFilesPanel::DeleteItem(wxDataViewItem& item)
{
	if ( m_outlineTreeModel->IsSpecial(item) )
	{
		wxMessageBox("Cannot delete special items!");
		return;
	}

	wxString msg = "'" + m_outlineTreeModel->GetTitle(item) + "'";

	if ( m_outlineTreeModel->IsContainer(item) )
		msg.insert(0, "folder ");
	else
		msg.insert(0, "file ");

	wxMessageDialog dlg(nullptr, "Are you sure you want to delete " +
		msg + "? This action cannot be undone.", "Warning",
		wxYES_NO | wxNO_DEFAULT | wxICON_WARNING | wxCENTER);

	if ( dlg.ShowModal() == wxID_YES )
	{
		m_outlineTreeModel->DeleteItem(item);
	}

	m_currentNode = nullptr;
}

void amOutlineFilesPanel::OnKeyDownDataView(wxKeyEvent& event)
{
	switch ( event.GetKeyCode() )
	{
	case WXK_DELETE:
	{
		wxDataViewItem sel = m_files->GetSelection();

		if ( !sel.IsOk() )
			return;

		DeleteItem(sel);
		break;
	}
	default:
		event.Skip();
		break;
	}
}

void amOutlineFilesPanel::OnRightDownDataView(wxDataViewEvent& event)
{
	wxDataViewItem item(event.GetItem());

	if ( !item.IsOk() )
		return;

	m_files->Select(item);

	wxMenu menu;
	menu.Append(MENU_ChangeItemFgColour, "Change text color");
	menu.Append(MENU_ChangeItemBgColour, "Change background color");
	menu.Append(MENU_DeleteItem, "Delete");

	menu.Bind(wxEVT_MENU, &amOutlineFilesPanel::OnMenuDataView, this);
	PopupMenu(&menu);

	event.Skip();
}

void amOutlineFilesPanel::OnMenuDataView(wxCommandEvent& event)
{
	wxDataViewItem sel = m_files->GetSelection();
	wxDataViewItemAttr attr;

	m_outlineTreeModel->GetAttr(sel, 0, attr);

	switch ( event.GetId() )
	{
	case MENU_ChangeItemFgColour:
	{
		wxColourData data;
		data.SetColour(attr.GetColour());
		wxColourDialog dlg(this, &data);

		if ( dlg.ShowModal() == wxID_OK )
		{
			m_outlineTreeModel->SetItemForegroundColour(sel, dlg.GetColourData().GetColour());
			Save();
		}

		break;
	}
	case MENU_ChangeItemBgColour:
	{
		wxColourData data;
		data.SetColour(attr.GetColour());
		wxColourDialog dlg(this, &data);

		if ( dlg.ShowModal() == wxID_OK )
		{
			m_outlineTreeModel->SetItemBackgroundColour(sel, dlg.GetColourData().GetColour());
			Save();
		}

		break;
	}
	case MENU_DeleteItem:
		DeleteItem(m_files->GetSelection());
		Save();
		break;
	}
}

void amOutlineFilesPanel::OnSelectionChanged(wxDataViewEvent& event)
{
	wxDataViewItem item(event.GetItem());
	m_outlineTreeModel->UnsetItemForDnD();

	SaveCurrentBuffer();

	if ( m_outlineTreeModel->IsContainer(item) )
	{
		m_textCtrl->Clear();
		m_textCtrl->Disable();
		m_currentNode = nullptr;
		return;
	}

	m_textCtrl->Enable();

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)(item.GetID());

	node->GetBuffer()->SetBasicStyle(m_basicAttr);
	m_textCtrl->GetBuffer() = *node->GetBuffer();

	wxDataViewItem parentItem(node->GetParent());

	if ( m_outlineTreeModel->IsCharacters(parentItem) ||
		m_outlineTreeModel->IsLocations(parentItem) ||
		m_outlineTreeModel->IsItems(parentItem) )
		m_textCtrl->SetEditable(false);
	else
		m_textCtrl->SetEditable(true);

	m_textCtrl->GetBuffer().Invalidate(wxRICHTEXT_ALL);
	m_textCtrl->Refresh();

	m_currentNode = node;
}

void amOutlineFilesPanel::OnEditingStart(wxDataViewEvent& event)
{
	wxDataViewItem item = event.GetItem();

	if ( m_outlineTreeModel->IsSpecial(item) )
	{
		event.Veto();
		return;
	}
	else
		event.Allow();
}

void amOutlineFilesPanel::OnEditingEnd(wxDataViewEvent& WXUNUSED(event))
{
	Save();
}

void amOutlineFilesPanel::OnItemExpanded(wxDataViewEvent& event) {}

void amOutlineFilesPanel::OnItemCollapsed(wxDataViewEvent& event) {}

void amOutlineFilesPanel::OnDrop(wxDataViewEvent& event)
{
	Save();
	event.Skip();
}

void amOutlineFilesPanel::OnTimerEvent(wxTimerEvent& event)
{
	Save();
}

void amOutlineFilesPanel::SaveCurrentBuffer()
{
	if ( m_currentNode && IsShownOnScreen() )
	{
		if ( m_currentNode->GetBuffer() && m_currentNode->GetBuffer()->GetText() != m_textCtrl->GetBuffer().GetText() )
			*m_currentNode->GetBuffer() = m_textCtrl->GetBuffer();
	}
}

wxXmlNode* amOutlineFilesPanel::SerializeFolder(wxDataViewItem& item)
{
	if ( !m_outlineTreeModel->IsContainer(item) )
		return nullptr;

	wxDataViewItemAttr attr;
	m_outlineTreeModel->GetAttr(item, 0, attr);

	wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, "Folder");
	node->AddAttribute("name", m_outlineTreeModel->GetTitle(item));
	node->AddAttribute("bg-color", attr.GetBackgroundColour().GetAsString());
	node->AddAttribute("fg-color", attr.GetColour().GetAsString());

	wxDataViewItemArray children;
	m_outlineTreeModel->GetChildren(item, children);

	wxString name;
	wxXmlNode* child;

	for ( unsigned int i = 0; i < children.GetCount(); i++ )
	{
		if ( m_outlineTreeModel->IsContainer(children[i]) )
		{
			child = SerializeFolder(children[i]);
			node->AddChild(child);
			continue;
		}

		node->AddChild(SerializeFile(children[i]));
	}

	return node;
}

wxXmlNode* amOutlineFilesPanel::SerializeFile(wxDataViewItem& item)
{
	if ( m_outlineTreeModel->IsContainer(item) )
		return nullptr;

	wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, "File");

	wxDataViewItemAttr attr;
	wxString name = m_outlineTreeModel->GetTitle(item);
	m_outlineTreeModel->GetAttr(item, 0, attr);

	node->AddAttribute("name", name);
	node->AddAttribute("bg-color", attr.GetBackgroundColour().GetAsString());
	node->AddAttribute("fg-color", attr.GetColour().GetAsString());

	wxString buffers;
	wxStringOutputStream stream(&buffers);
	m_outlineTreeModel->GetBuffer(item)->SaveFile(stream, wxRICHTEXT_TYPE_XML);

	node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", buffers));

	return node;
}

void amOutlineFilesPanel::DeserializeNode(wxXmlNode* node, wxDataViewItem& parent)
{
	wxXmlNode* child = node->GetChildren();
	wxDataViewItem item;

	if ( node->GetName() == "File" )
	{
		wxString bufString = child->GetContent();
		wxStringInputStream stream(bufString);

		wxRichTextBuffer localBuffer;
		localBuffer.LoadFile(stream, wxRICHTEXT_TYPE_XML);
		item = m_outlineTreeModel->AppendFile(parent, node->GetAttribute("name").ToStdString());

		wxRichTextBuffer* pNodeBuffer = ((OutlineTreeModelNode*)item.GetID())->GetBuffer();
		*pNodeBuffer = localBuffer;
	}
	else if ( node->GetName() == "Folder" )
	{
		if ( child )
		{
			item = m_outlineTreeModel->AppendFolder(parent, node->GetAttribute("name").ToStdString());
			while ( child )
			{
				DeserializeNode(child, item);
				child = child->GetNext();
			}
		}
		else
			item = m_outlineTreeModel->AppendFolder(parent, node->GetAttribute("name").ToStdString());
	}

	wxString colours;
	wxColour colour;

	colours = node->GetAttribute("bg-color");
	colour.Set(colours);
	m_outlineTreeModel->SetItemBackgroundColour(item, colour);

	colours = node->GetAttribute("fg-color");
	colour.Set(colours);
	m_outlineTreeModel->SetItemForegroundColour(item, colour);
}

bool amOutlineFilesPanel::Save()
{
	if ( m_isSaving || !IsShownOnScreen() )
		return false;

	bool bSucceeded = false;

	std::thread thread([&]()
		{
			m_isSaving = true;

			SaveCurrentBuffer();
			wxXmlDocument doc;

			wxXmlNode* root = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "AO-Files");
			doc.SetRoot(root);

			wxDataViewItemArray rootFiles;
			m_outlineTreeModel->GetChildren(wxDataViewItem(nullptr), rootFiles);

			for ( unsigned int i = 0; i < rootFiles.GetCount(); i++ )
			{
				if ( m_outlineTreeModel->IsContainer(rootFiles[i]) )
					root->AddChild(SerializeFolder(rootFiles[i]));
				else
					root->AddChild(SerializeFile(rootFiles[i]));
			}

			am::SQLEntry sqlEntry;
			sqlEntry.strings["content"] = wxString();

			wxStringOutputStream stream(&sqlEntry.strings["content"]);

			if ( doc.Save(stream) )
			{
				sqlEntry.tableName = "outline_files";
				sqlEntry.name = "Outline Files";

				am::SaveSQLEntry(sqlEntry, sqlEntry);

				bSucceeded = true;
			}
			else
				bSucceeded = false;

			m_isSaving = false;
		}
	);

	thread.detach();

	return bSucceeded;
}

bool amOutlineFilesPanel::Load(am::ProjectSQLDatabase* db)
{
	wxSQLite3ResultSet result = db->ExecuteQuery("SELECT * FROM outline_files;");
	while ( result.NextRow() )
	{
		if ( !result.IsNull("content") )
		{
			wxString str = result.GetAsString("content");
			wxStringInputStream sstream(str);

			ClearAll();
			Init();

			wxXmlDocument doc;
			if ( !doc.Load(sstream) )
				return false;

			wxDataViewItemArray array;
			array.Add(wxDataViewItem(m_outlineTreeModel->GetResearchNode()));
			//array.Add(wxDataViewItem(m_outlineTreeModel->GetCharactersNode()));
			//array.Add(wxDataViewItem(m_outlineTreeModel->GetLocationsNode()));

			wxXmlNode* child = doc.GetRoot()->GetChildren();
			wxXmlNode* child2 = child->GetChildren();

			while ( child2 )
			{
				DeserializeNode(child2, wxDataViewItem(m_outlineTreeModel->GetResearchNode()));
				child2 = child2->GetNext();
			}

			child = child->GetNext()->GetNext()->GetNext()->GetNext();

			while ( child )
			{
				DeserializeNode(child, wxDataViewItem(nullptr));
				child = child->GetNext();
			}

			return true;
		}
	}
}

void amOutlineFilesPanel::ClearAll()
{
	m_outlineTreeModel->Clear();
	m_textCtrl->Clear();
}