#include "OutlineFiles.h"

#include "MainFrame.h"
#include "MyApp.h"

#include <wx\xml\xml.h>
#include <wx\sstream.h>
#include <wx\richtext\richtextxml.h>
#include <wx\colordlg.h>

namespace fs = boost::filesystem;

OutlineTreeModel::OutlineTreeModel() {
	m_research = new OutlineTreeModelNode(nullptr, _("Research"));
	m_characters = new OutlineTreeModelNode(nullptr, _("Characters"));
	m_locations = new OutlineTreeModelNode(nullptr, _("Locations"));
	m_items = new OutlineTreeModelNode(nullptr, _("Items"));
}

wxString OutlineTreeModel::GetTitle(const wxDataViewItem& item) const {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	if (!node)
		return wxEmptyString;

	return node->m_title;
}

wxRichTextBuffer& OutlineTreeModel::GetBuffer(const wxDataViewItem& item) const {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node->m_buffer;
}

wxDataViewItem OutlineTreeModel::AddToResearch(const wxString& title) {
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_research, title, wxRichTextBuffer());
	m_research->Append(node);
	wxDataViewItem parent(m_research);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::AddToCharacters(const wxString& title) {
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_characters, title, wxRichTextBuffer());
	wxDataViewItem parent(m_characters);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::AddToLocations(const wxString& title) {
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_locations, title, wxRichTextBuffer());
	wxDataViewItem parent(m_locations);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::AddToItems(const wxString& title) {
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_items, title, wxRichTextBuffer());
	wxDataViewItem parent(m_items);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::AppendFile(wxDataViewItem& parent, const wxString& name, const wxRichTextBuffer& buffer) {
	OutlineTreeModelNode* parentNode = (OutlineTreeModelNode*)parent.GetID();
	OutlineTreeModelNode* node = new OutlineTreeModelNode(parentNode, name, buffer);
	wxDataViewItem item(node);

	if (!parentNode)
		m_otherRoots.Add(node);

	ItemAdded(parent, item);

	return item;
}

wxDataViewItem OutlineTreeModel::AppendFolder(wxDataViewItem& parent, const wxString& name) {
	OutlineTreeModelNode* parentNode = (OutlineTreeModelNode*)parent.GetID();
	OutlineTreeModelNode* node = new OutlineTreeModelNode(parentNode, name);
	wxDataViewItem item(node);

	if (!parentNode)
		m_otherRoots.Add(node);

	ItemAdded(parent, item);

	return item;
}

bool OutlineTreeModel::IsResearch(wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_research;
}

bool OutlineTreeModel::IsCharacters(wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_characters;
}

bool OutlineTreeModel::IsLocations(wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_locations;
}

bool OutlineTreeModel::IsItems(wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_items;
}

bool OutlineTreeModel::IsDescendant(wxDataViewItem& item, wxDataViewItem& descendant) {
	wxDataViewItem& parent = GetParent(descendant);

	bool is = false;

	while (parent.IsOk()) {
		if (parent.GetID() == item.GetID())
			is = true;

		parent = GetParent(parent);
	}

	return is;
}

void OutlineTreeModel::DeleteItem(const wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	if (!node)      // happens if item.IsOk()==false
		return;

	if (node == m_research || node == m_characters || node == m_locations) {
		wxMessageBox(_("Cannot remove special folders!"));
		return;
	}
	// first remove the node from the parent's array of children;
	// NOTE: OutlineTreeModelNode is only an array of _pointers_
	//       thus removing the node from it doesn't result in freeing it

	wxDataViewItemArray children;
	GetChildren(item, children);

	for (int i = 0; i < children.GetCount(); i++)
		DeleteItem(children[i]);

	OutlineTreeModelNode* parentNode = node->GetParent();
	if (parentNode)
		parentNode->GetChildren().Remove(node);
	else
		m_otherRoots.Remove(node);

	wxDataViewItem parent(parentNode);

	// free the node
	delete node;

	ItemDeleted(parent, item);
}

void OutlineTreeModel::SetItemBackgroundColour(wxDataViewItem& item, wxColour& colour) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	wxDataViewItemAttr& attr = node->GetAttr();

	attr.SetBackgroundColour(colour);
}

void OutlineTreeModel::SetItemForegroundColour(wxDataViewItem& item, wxColour& colour) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	wxDataViewItemAttr& attr = node->GetAttr();

	attr.SetColour(colour);
}

void OutlineTreeModel::SetItemFont(wxDataViewItem& item, wxFont& font) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	wxDataViewItemAttr attr = node->GetAttr();

	attr.SetBold(font.GetWeight() >= 400);
	attr.SetItalic(font.GetStyle() == wxFONTSTYLE_ITALIC);
	attr.SetStrikethrough(font.GetStrikethrough());
}

bool OutlineTreeModel::Reparent(OutlineTreeModelNode* itemNode, OutlineTreeModelNode* newParentNode) {
	OutlineTreeModelNode* oldParentNode(itemNode->GetParent());

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

bool OutlineTreeModel::Reparent(OutlineTreeModelNode* itemNode, OutlineTreeModelNode* newParentNode, int n) {
	OutlineTreeModelNode* oldParentNode(itemNode->GetParent());

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

bool OutlineTreeModel::Reposition(wxDataViewItem& item, int n) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	if (node) {
		node->Reposition(n);
		ItemChanged(wxDataViewItem(node->GetParent()));
	} else
		return false;
	
	return true;
}

void OutlineTreeModel::Clear() {
	OulineTreeModelNodePtrArray array;

	array = m_research->GetChildren();
	for (int i = 0; i < array.GetCount(); i++) {
		DeleteItem(wxDataViewItem(array[i]));
	}

	array = m_characters->GetChildren();
	for (int i = 0; i < array.GetCount(); i++) {
		DeleteItem(wxDataViewItem(array[i]));
	}

	array = m_locations->GetChildren();
	for (int i = 0; i < array.GetCount(); i++) {
		DeleteItem(wxDataViewItem(array[i]));
	}

	for (int i = 0; i < m_otherRoots.size(); i++)
		DeleteItem(wxDataViewItem(m_otherRoots[i]));
}

void OutlineTreeModel::GetValue(wxVariant& variant,
	const wxDataViewItem& item, unsigned int col) const {

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	
	switch (col) {
	case 0:
		variant = node->m_title;
		break;
	default:
		break;
	}
}

bool OutlineTreeModel::SetValue(const wxVariant& variant,
	const wxDataViewItem& item, unsigned int col) {

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	switch (col) {
	case 0:
		node->m_title = variant.GetString();
		return true;

	default:
		break;
	}
	return false;
}

wxDataViewItem OutlineTreeModel::GetParent(const wxDataViewItem& item) const {
	// the invisible root node has no parent
	if (!item.IsOk())
		return wxDataViewItem(0);

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	if (node == m_research || node == m_characters || node == m_locations)
		return wxDataViewItem(0);

	return wxDataViewItem(node->GetParent());
}

bool OutlineTreeModel::IsContainer(const wxDataViewItem& item) const {
	// the invisible root node can have children
	if (!item.IsOk())
		return true;

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	if (node)
		return node->IsContainer();
	else
		return false;
}

bool OutlineTreeModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	
	if (node)
		attr = node->GetAttr();

	return true;
}

unsigned int OutlineTreeModel::GetChildren(const wxDataViewItem& parent,
	wxDataViewItemArray& array) const {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)parent.GetID();
	if (!node) {
		int n = 0;

		if (m_research) {
			array.Add(wxDataViewItem(m_research));
			n++;
		}

		if (m_characters) {
			array.Add(wxDataViewItem(m_characters));
			n++;
		}

		if (m_locations) {
			array.Add(wxDataViewItem(m_locations));
			n++;
		}

		if (m_items) {
			array.Add(wxDataViewItem(m_items));
			n++;
		}

		for (int i = 0; i < m_otherRoots.GetCount(); i++) {
			array.Add(wxDataViewItem(m_otherRoots.at(i)));
		}

		return n + m_otherRoots.GetCount();
	}

	int count = node->GetChildCount();

	if (count == 0) {
		return 0;
	}

	for (int pos = 0; pos < count; pos++) {
		OutlineTreeModelNode* child = node->GetChildren().Item(pos);
		array.Add(wxDataViewItem(child));
	}

	return count;
}


BEGIN_EVENT_TABLE(amdOutlineFilesPanel, wxSplitterWindow)

EVT_DATAVIEW_SELECTION_CHANGED(TREE_Files, amdOutlineFilesPanel::OnSelectionChanged)

EVT_DATAVIEW_ITEM_EDITING_STARTED(TREE_Files, amdOutlineFilesPanel::OnEditingStart)
EVT_DATAVIEW_ITEM_EDITING_DONE(TREE_Files, amdOutlineFilesPanel::OnEditingEnd)

EVT_DATAVIEW_ITEM_BEGIN_DRAG(TREE_Files, amdOutlineFilesPanel::OnBeginDrag)
EVT_DATAVIEW_ITEM_DROP_POSSIBLE(TREE_Files, amdOutlineFilesPanel::OnDropPossible)
EVT_DATAVIEW_ITEM_DROP(TREE_Files, amdOutlineFilesPanel::OnDrop)

EVT_TOOL(TOOL_NewFile, amdOutlineFilesPanel::NewFile)
EVT_TOOL(TOOL_NewFolder, amdOutlineFilesPanel::NewFolder)

EVT_TIMER(TIMER_OutlineFiles, amdOutlineFilesPanel::OnTimerEvent)

END_EVENT_TABLE()

amdOutlineFilesPanel::amdOutlineFilesPanel(wxWindow* parent) : wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize, 768L | wxSP_LIVE_UPDATE) {
	m_textCtrl = new wxRichTextCtrl(this);
	m_textCtrl->SetBackgroundColour(wxColour(40, 40, 40));
	m_textCtrl->Refresh();

	m_basicAttr.SetFontSize(13);
	m_basicAttr.SetTextColour(wxColour(245, 245, 245));

	m_textCtrl->SetBasicStyle(m_basicAttr);

	m_leftPanel = new wxPanel(this);
	m_files = new wxDataViewCtrl(m_leftPanel, TREE_Files, wxDefaultPosition, wxDefaultSize,
		wxDV_NO_HEADER | wxDV_SINGLE);
	m_files->GetMainWindow()->Bind(wxEVT_KEY_DOWN, &amdOutlineFilesPanel::OnKeyDownDataView, this);
	m_files->GetMainWindow()->Bind(wxEVT_RIGHT_DOWN, &amdOutlineFilesPanel::OnRightDownDataView, this);

	m_filesTB = new wxToolBar(m_leftPanel, -1);
	m_filesTB->AddTool(TOOL_NewFile, "", wxBITMAP_PNG(addFile), _("Add new file."));
	m_filesTB->AddTool(TOOL_NewFolder, "", wxBITMAP_PNG(addFolder), _("Add new folder."));
	m_filesTB->SetBackgroundColour(wxColour(60, 60, 60));

	m_filesTB->Realize();

	m_files->EnableDragSource(wxDataFormat(wxRichTextBufferDataObject::GetRichTextBufferFormatId()));
	m_files->EnableDropTarget(wxDataFormat(wxRichTextBufferDataObject::GetRichTextBufferFormatId()));
	m_files->EnableDragSource(wxDataFormat(wxDF_FILENAME));
	m_files->EnableDragSource(wxDataFormat(wxDF_FILENAME));
	m_files->SetBackgroundColour(wxColour(250, 250, 250));

	m_outlineTreeModel = new OutlineTreeModel();
	m_files->AssociateModel(m_outlineTreeModel.get());

	wxDataViewTextRenderer* tr = new wxDataViewTextRenderer(wxDataViewTextRenderer::GetDefaultType(),
		wxDATAVIEW_CELL_EDITABLE);
	wxDataViewColumn* column0 = new wxDataViewColumn(_("Files"), tr, 0, FromDIP(200), wxALIGN_LEFT);
	m_files->AppendColumn(column0);

	wxBoxSizer* panSizer = new wxBoxSizer(wxVERTICAL);
	panSizer->Add(m_filesTB, wxSizerFlags(0).Expand());
	panSizer->Add(m_files, wxSizerFlags(1).Expand());

	m_leftPanel->SetSizer(panSizer);

	m_textCtrl->SetMinSize(wxSize(20, -1));
	m_leftPanel->SetMinSize(wxSize(20, -1));

	SplitVertically(m_leftPanel, m_textCtrl, 200);

	m_timer.Start(10000);
}

void amdOutlineFilesPanel::Init() {
	wxVector<Character>& charList = amdGetManager()->GetCharacters();
	wxVector<Location>& locList = amdGetManager()->GetLocations();

	for (auto it = charList.begin(); it != charList.end(); it++)
		AppendCharacter(*it);

	for (auto it = locList.begin(); it != locList.end(); it++)
		AppendLocation(*it);
}

void amdOutlineFilesPanel::GenerateCharacterBuffer(Character& character, wxRichTextBuffer& buffer) {
	buffer.SetBasicStyle(m_textCtrl->GetBasicStyle());
	buffer.BeginSuppressUndo();

	int begin = 0;

	if (character.image.IsOk()) {
		wxImage image = character.image;
		double ratio = (double)image.GetWidth() / (double)image.GetHeight();

		if (ratio > 1)
			image.Rescale(180 * ratio, 180, wxIMAGE_QUALITY_HIGH);
		else
			image.Rescale(220 * ratio, 220, wxIMAGE_QUALITY_HIGH);

		buffer.BeginAlignment(wxTEXT_ALIGNMENT_CENTER);
		buffer.AddImage(image);
		buffer.EndAlignment();

		begin += 2;
	}

	buffer.BeginFontSize(16);
	buffer.BeginBold();
	buffer.InsertTextWithUndo(begin, "\n" + _("Name: "), nullptr);
	buffer.EndBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), " " + character.name, nullptr);
	buffer.EndFontSize();

	wxString separator("\n\n");

	if (character.age != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), separator + _("Age: "), nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.age, nullptr);
		separator = "\n";
	}

	if (character.sex != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), separator + _("Sex: "), nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.sex, nullptr);
		separator = "\n";
	}

	if (character.height != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), separator + _("Height: "), nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.height, nullptr);
		separator = "\n";
	}

	if (character.nick != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), separator + _("Nickname: "), nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.nick, nullptr);
		separator = "\n";
	}

	if (character.nat != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), separator + _("Nationality: "), nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.nat, nullptr);
		separator = "\n";
	}

	if (character.appearance != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\n" + _("Appearance:") + "\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.appearance, nullptr);
	}

	if (character.personality != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\n" + _("Personality:") + "\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.personality, nullptr);
	}

	if (character.backstory != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\n" + _("Backstory:") + "\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.backstory, nullptr);
	}

	for (auto& it : character.custom) {
		if (it.second != "") {
			buffer.BeginBold();
			buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\n" + it.first + ":\n", nullptr);
			buffer.EndBold();
			buffer.InsertTextWithUndo(buffer.GetText().size(), it.second, nullptr);
		}
	}

	buffer.SetName(character.name);
}

void amdOutlineFilesPanel::GenerateLocationBuffer(Location& location, wxRichTextBuffer& buffer) {
	buffer.SetBasicStyle(m_textCtrl->GetBasicStyle());
	buffer.BeginSuppressUndo();

	if (location.image.IsOk()) {
		wxImage image = location.image;
		double ratio = (double)image.GetWidth() / (double)image.GetHeight();

		if (ratio > 1)
			image.Rescale(180 * ratio, 180, wxIMAGE_QUALITY_HIGH);
		else 
			image.Rescale(220 * ratio, 220, wxIMAGE_QUALITY_HIGH);

		buffer.BeginAlignment(wxTEXT_ALIGNMENT_CENTER);
		buffer.AddImage(image);
		buffer.EndAlignment();
	}

	buffer.BeginFontSize(16);
	buffer.BeginBold();
	buffer.InsertTextWithUndo(2, "\nName: ", nullptr);
	buffer.EndBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), " " + location.name, nullptr);
	buffer.EndFontSize();

	buffer.BeginBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nImportance: ", nullptr);
	buffer.EndBold();
	wxString role;
	switch (location.role) {
	case lHigh:
		role = "High";
		break;

	case lLow:
		role = "Low";
		break;

	default:
		role = "--/--";
		break;
	}
	buffer.InsertTextWithUndo(buffer.GetText().size(), role, nullptr);

	if (location.general != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nGeneral:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.general, nullptr);
	}

	if (location.natural != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nNatural characteristics:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.natural, nullptr);
	}

	if (location.architecture != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nArchitecture:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.architecture, nullptr);
	}

	if (location.politics != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nPolitics:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.politics, nullptr);
	}

	if (location.economy != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nEconomy:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.economy, nullptr);
	}

	if (location.culture != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nCulture:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.culture, nullptr);
	}

	for (auto& it : location.custom) {
		if (it.second != "") {
			buffer.BeginBold();
			buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\n" + it.first + ":\n", nullptr);
			buffer.EndBold();
			buffer.InsertTextWithUndo(buffer.GetText().size(), it.second, nullptr);
		}
	}

	buffer.SetName(location.name);
}

void amdOutlineFilesPanel::GenerateItemBuffer(Item& item, wxRichTextBuffer& buffer) {
	buffer.SetBasicStyle(m_textCtrl->GetBasicStyle());
	buffer.BeginSuppressUndo();

	if (item.image.IsOk()) {
		wxImage image = item.image;
		double ratio = (double)image.GetWidth() / (double)image.GetHeight();

		if (ratio > 1)
			image.Rescale(180 * ratio, 180, wxIMAGE_QUALITY_HIGH);
		else
			image.Rescale(220 * ratio, 220, wxIMAGE_QUALITY_HIGH);

		buffer.BeginAlignment(wxTEXT_ALIGNMENT_CENTER);
		buffer.AddImage(image);
		buffer.EndAlignment();
	}

	buffer.BeginFontSize(16);
	buffer.BeginBold();
	buffer.InsertTextWithUndo(2, "\nName: ", nullptr);
	buffer.EndBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), " " + item.name, nullptr);
	buffer.EndFontSize();

	buffer.BeginBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nImportance: ", nullptr);
	buffer.EndBold();
	wxString role;
	switch (item.role) {
	case iHigh:
		role = "High";
		break;

	case iLow:
		role = "Low";
		break;

	default:
		role = "--/--";
		break;
	}
	buffer.InsertTextWithUndo(buffer.GetText().size(), role, nullptr);

	//if (item.general != "") {
	//	buffer.BeginBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nGeneral:\n", nullptr);
	//	buffer.EndBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), item.general, nullptr);
	//}

	//if (item.natural != "") {
	//	buffer.BeginBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nNatural characteristics:\n", nullptr);
	//	buffer.EndBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), location.natural, nullptr);
	//}

	//if (location.architecture != "") {
	//	buffer.BeginBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nArchitecture:\n", nullptr);
	//	buffer.EndBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), location.architecture, nullptr);
	//}

	//if (location.politics != "") {
	//	buffer.BeginBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nPolitics:\n", nullptr);
	//	buffer.EndBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), location.politics, nullptr);
	//}

	//if (location.economy != "") {
	//	buffer.BeginBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nEconomy:\n", nullptr);
	//	buffer.EndBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), location.economy, nullptr);
	//}

	//if (location.culture != "") {
	//	buffer.BeginBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\nCulture:\n", nullptr);
	//	buffer.EndBold();
	//	buffer.InsertTextWithUndo(buffer.GetText().size(), location.culture, nullptr);
	//}

	//for (auto& it : location.custom) {
	//	if (it.second != "") {
	//		buffer.BeginBold();
	//		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\n\n\n" + it.first + ":\n", nullptr);
	//		buffer.EndBold();
	//		buffer.InsertTextWithUndo(buffer.GetText().size(), it.second, nullptr);
	//	}
	//}

	//buffer.SetName(location.name);
}

void amdOutlineFilesPanel::AppendCharacter(Character& character) {
	wxDataViewItem item = m_outlineTreeModel->AddToCharacters(character.name.ToStdString());
	wxRichTextBuffer& buffer = ((OutlineTreeModelNode*)item.GetID())->m_buffer;

	GenerateCharacterBuffer(character, buffer);
}

void amdOutlineFilesPanel::AppendLocation(Location& location) {
	wxDataViewItem item = m_outlineTreeModel->AddToLocations(location.name.ToStdString());
	wxRichTextBuffer& buffer = ((OutlineTreeModelNode*)item.GetID())->m_buffer;
	
	GenerateLocationBuffer(location, buffer);
}

void amdOutlineFilesPanel::AppendItem(Item& item) {
	wxDataViewItem dvitem = m_outlineTreeModel->AddToItems(item.name.ToStdString());
	wxRichTextBuffer& buffer = ((OutlineTreeModelNode*)dvitem.GetID())->m_buffer;

	GenerateItemBuffer(item, buffer);
}

void amdOutlineFilesPanel::DeleteCharacter(Character& character) {
	OulineTreeModelNodePtrArray& characters = m_outlineTreeModel->GetCharacters();
	for (int i = 0; i < characters.Count(); i++) {
		if (characters[i]->m_title == character.name) {
			if (m_currentNode == characters[i]) {
				m_currentNode = nullptr;
				m_textCtrl->Clear();
			}

			wxDataViewItem item(characters[i]);
			m_outlineTreeModel->DeleteItem(item);
			return;
		}
	}
}

void amdOutlineFilesPanel::DeleteLocation(Location& location) {
	OulineTreeModelNodePtrArray& locations = m_outlineTreeModel->GetLocations();
	for (int i = 0; i < locations.Count(); i++) {
		if (locations[i]->m_title == location.name) {
			if (m_currentNode == locations[i]) {
				m_currentNode = nullptr;
				m_textCtrl->Clear();
			}

			wxDataViewItem item(locations[i]);
			m_outlineTreeModel->DeleteItem(item);
			return;
		}
	}
}

void amdOutlineFilesPanel::DeleteItem(Item& item) {
	OulineTreeModelNodePtrArray& items = m_outlineTreeModel->GetItems();
	for (int i = 0; i < items.Count(); i++) {
		if (items[i]->m_title == item.name) {
			if (m_currentNode == items[i]) {
				m_currentNode = nullptr;
				m_textCtrl->Clear();
			}

			wxDataViewItem dvitem(items[i]);
			m_outlineTreeModel->DeleteItem(dvitem);
			return;
		}
	}
}

void amdOutlineFilesPanel::NewFile(wxCommandEvent& event) {
	wxDataViewItem sel = m_files->GetSelection(); 

	if (sel.IsOk()) {
		if (!m_outlineTreeModel->IsContainer(sel))
			sel = m_outlineTreeModel->GetParent(sel);
	}

	if (m_outlineTreeModel->IsCharacters(sel) || m_outlineTreeModel->IsLocations(sel))
		return;

	m_files->Select(m_outlineTreeModel->AppendFile(sel, "New file", wxRichTextBuffer()));
	amdGetManager()->SetSaved(false);
}

void amdOutlineFilesPanel::NewFolder(wxCommandEvent& event) {
	wxDataViewItem sel = m_files->GetSelection();

	if (sel.IsOk()) {
		if (!m_outlineTreeModel->IsContainer(sel))
			sel = m_outlineTreeModel->GetParent(sel);
	}

	if (m_outlineTreeModel->IsCharacters(sel) || m_outlineTreeModel->IsLocations(sel))
		return;

	m_files->Select(m_outlineTreeModel->AppendFolder(sel, "New folder"));
	amdGetManager()->SetSaved(false);
}

void amdOutlineFilesPanel::DeleteItem(wxDataViewItem& item) {
	wxDataViewItem parent = m_outlineTreeModel->GetParent(item);

	if (m_outlineTreeModel->IsResearch(item) || m_outlineTreeModel->IsCharacters(item) ||
		m_outlineTreeModel->IsLocations(item) || m_outlineTreeModel->IsCharacters(parent) ||
		m_outlineTreeModel->IsLocations(parent)) {
		wxMessageBox("Cannot delete special items!");
		return;
	}

	wxString msg = "'" + m_outlineTreeModel->GetTitle(item) + "'";

	if (m_outlineTreeModel->IsContainer(item))
		msg.insert(0, "folder ");
	else
		msg.insert(0, "file ");

	wxMessageDialog* dlg = new wxMessageDialog(nullptr,
		"Are you sure you want to delete " + msg + "? This action cannot be undone.", "Warning",
		wxYES_NO | wxNO_DEFAULT | wxICON_WARNING | wxCENTER);

	if (dlg->ShowModal() == wxID_YES) {
		m_outlineTreeModel->DeleteItem(item);
	}

	if (dlg)
		delete dlg;
}

void amdOutlineFilesPanel::OnKeyDownDataView(wxKeyEvent& event) {
	switch (event.GetKeyCode()) {
	case WXK_DELETE:
	{
		wxDataViewItem sel = m_files->GetSelection();

		if (!sel.IsOk())
			return;

		DeleteItem(sel);
		break;
	}
	default:
		event.Skip();
		break;
	}
}

void amdOutlineFilesPanel::OnRightDownDataView(wxMouseEvent& event) {
	wxDataViewItem item;
	wxDataViewColumn* col;
	m_files->HitTest(event.GetPosition(), item, col);

	if (!item.IsOk())
		return;

	m_files->Select(item);

	wxMenu menu;
	menu.Append(MENU_ChangeItemFgColour, "Change text color");
	menu.Append(MENU_ChangeItemBgColour, "Change background color");
	menu.Append(MENU_DeleteItem, "Delete");

	menu.Bind(wxEVT_MENU, &amdOutlineFilesPanel::OnMenuDataView, this);
	PopupMenu(&menu);
}

void amdOutlineFilesPanel::OnMenuDataView(wxCommandEvent& event) {
	wxDataViewItem sel = m_files->GetSelection();
	wxDataViewItemAttr attr;

	m_outlineTreeModel->GetAttr(sel, 0, attr);

	switch (event.GetId()) {
	case MENU_ChangeItemFgColour:
	{
		wxColourData data;
		data.SetColour(attr.GetColour());
		wxColourDialog dlg(this, &data);

		if (dlg.ShowModal() == wxID_OK) {
			m_outlineTreeModel->SetItemForegroundColour(sel, dlg.GetColourData().GetColour());
			amdGetManager()->SetSaved(false);
		}
		
		break;
	}
	case MENU_ChangeItemBgColour:
	{
		wxColourData data;
		data.SetColour(attr.GetColour());
		wxColourDialog dlg(this, &data);

		if (dlg.ShowModal() == wxID_OK) {
			m_outlineTreeModel->SetItemBackgroundColour(sel, dlg.GetColourData().GetColour());
			amdGetManager()->SetSaved(false);
		}

		break;
	}
	case MENU_DeleteItem:
		DeleteItem(m_files->GetSelection());
		amdGetManager()->SetSaved(false);
		break;
	}
}

void amdOutlineFilesPanel::OnSelectionChanged(wxDataViewEvent& event) {
	wxDataViewItem item(event.GetItem());

	SaveCurrentBuffer();

	if (m_outlineTreeModel->IsContainer(item)) {
		m_textCtrl->Clear();
		m_textCtrl->Disable();
		m_currentNode = nullptr;
		return;
	}

	m_textCtrl->Enable();

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)(item.GetID());

	node->m_buffer.SetBasicStyle(m_basicAttr);
	m_textCtrl->GetBuffer() = node->m_buffer;

	wxDataViewItem parentItem(node->GetParent());

	if (m_outlineTreeModel->IsCharacters(parentItem) ||
		m_outlineTreeModel->IsLocations(parentItem))
		m_textCtrl->SetEditable(false);
	else
		m_textCtrl->SetEditable(true);

	m_textCtrl->GetBuffer().Invalidate(wxRICHTEXT_ALL);
	m_textCtrl->RecreateBuffer();
	m_textCtrl->Refresh();

	m_currentNode = node;
}

void amdOutlineFilesPanel::OnEditingStart(wxDataViewEvent& event) {
	wxDataViewItem item = event.GetItem();
	wxDataViewItem parent = m_outlineTreeModel->GetParent(item);

	if (m_outlineTreeModel->IsResearch(item) || m_outlineTreeModel->IsCharacters(item) ||
		m_outlineTreeModel->IsLocations(item) || m_outlineTreeModel->IsCharacters(parent) ||
		m_outlineTreeModel->IsLocations(parent)) {
		event.Veto();
		return;
	}
	else
		event.Allow();
}

void amdOutlineFilesPanel::OnEditingEnd(wxDataViewEvent& WXUNUSED(event)) {}

void amdOutlineFilesPanel::OnBeginDrag(wxDataViewEvent& event) {
	wxDataViewItem item(event.GetItem());
	wxDataViewItem parent = m_outlineTreeModel->GetParent(item);

	if (m_outlineTreeModel->IsResearch(item) || m_outlineTreeModel->IsCharacters(item) ||
		m_outlineTreeModel->IsLocations(item) || m_outlineTreeModel->IsCharacters(parent) ||
		m_outlineTreeModel->IsLocations(parent)) {
		event.Veto();
		return; 
	}

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	wxRichTextBufferDataObject* obj = new wxRichTextBufferDataObject(new wxRichTextBuffer(node->m_buffer));
	event.SetDataObject(obj);
	event.SetDragFlags(wxDrag_AllowMove); // allows both copy and move

	m_nodeForDnD = node;
	m_itemForDnD = item;
}

void amdOutlineFilesPanel::OnDropPossible(wxDataViewEvent& event) {
	if (event.GetDataFormat() != wxRichTextBufferDataObject::GetRichTextBufferFormatId()) {
		m_nodeForDnD = nullptr;
		event.Veto();
	} else {
		wxDataViewItem ht;
		wxDataViewColumn* cht = m_files->GetColumn(0);

		m_files->HitTest(event.GetPosition(), ht, cht);

		if (m_outlineTreeModel->IsContainer(ht) && ht != m_itemForDnD)
			event.SetDropEffect(wxDragMove);
		else
			event.SetDropEffect(wxDragNone);
	}
}

void amdOutlineFilesPanel::OnDrop(wxDataViewEvent& event) {
	wxDataViewItem item(event.GetItem());
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	if (m_nodeForDnD == node || m_outlineTreeModel->IsCharacters(item) ||
		m_outlineTreeModel->IsLocations(item) || m_outlineTreeModel->IsDescendant(m_itemForDnD, item))
		return;

	if (event.GetDataFormat() != wxRichTextBufferDataObject::GetRichTextBufferFormatId()) {
		event.Veto();
		return;
	}

	wxRichTextBufferDataObject obj;
	obj.SetData(event.GetDataFormat(), event.GetDataSize(), event.GetDataBuffer());

	int index = event.GetProposedDropIndex();

	if (item.IsOk()) {
		if (m_outlineTreeModel->IsContainer(item)) {
			if (m_nodeForDnD) {
				if (m_nodeForDnD->GetParent() != node) {
					if (index == -1)
						m_outlineTreeModel->Reparent(m_nodeForDnD, node);
					else
						m_outlineTreeModel->Reparent(m_nodeForDnD, node, index);
				} else {
					if (index == -1)
						m_outlineTreeModel->Reposition(m_itemForDnD, 0);
					else
						m_outlineTreeModel->Reposition(m_itemForDnD, index);
				}
			}

			amdGetManager()->SetSaved(false);
		} else
			wxLogMessage(_("Dropped on item. Nothing happened."));

	} else {
		if (m_nodeForDnD->GetParent() != node) {
			if (index > 3)
				m_outlineTreeModel->Reparent(m_nodeForDnD, nullptr, index -3);
			else
				m_outlineTreeModel->Reparent(m_nodeForDnD, nullptr);
		}

		amdGetManager()->SetSaved(false);
	}

	m_files->Select(m_itemForDnD);
}

void amdOutlineFilesPanel::OnUnsplit(wxWindow* WXUNUSED(window)) {
	SplitVertically(m_leftPanel, m_textCtrl, 200);
}

void amdOutlineFilesPanel::OnTimerEvent(wxTimerEvent& event) {
	SaveCurrentBuffer();
}

void amdOutlineFilesPanel::SaveCurrentBuffer() {
	if (m_currentNode) {
		if (m_currentNode->m_buffer.GetText() != m_textCtrl->GetBuffer().GetText()) {
			m_currentNode->m_buffer = m_textCtrl->GetBuffer();
			amdGetManager()->SetSaved(false);
		}
	}
}

wxXmlNode* amdOutlineFilesPanel::SerializeFolder(wxDataViewItem& item) {
	if (!m_outlineTreeModel->IsContainer(item))
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

	for (int i = 0; i < children.GetCount(); i++) {
		if (m_outlineTreeModel->IsContainer(children[i])) {
			child = SerializeFolder(children[i]);
			node->AddChild(child);
			continue;
		}

		node->AddChild(SerializeFile(children[i]));
	}

	return node;
}

wxXmlNode* amdOutlineFilesPanel::SerializeFile(wxDataViewItem& item) {
	if (m_outlineTreeModel->IsContainer(item))
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
	m_outlineTreeModel->GetBuffer(item).SaveFile(stream, wxRICHTEXT_TYPE_XML);

	node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", buffers));

	return node;
}

void amdOutlineFilesPanel::DeserializeNode(wxXmlNode* node, wxDataViewItem& parent) {
	wxXmlNode* child = node->GetChildren();
	wxDataViewItem item;

	if (node->GetName() == "File") {
		wxString bufString = child->GetContent();
		wxStringInputStream stream(bufString);

		wxRichTextBuffer buffer;
		buffer.LoadFile(stream, wxRICHTEXT_TYPE_XML);
		item = m_outlineTreeModel->AppendFile(parent, node->GetAttribute("name").ToStdString(), buffer);
	} else if (node->GetName() == "Folder") {
		if (child) {
			item = m_outlineTreeModel->AppendFolder(parent, node->GetAttribute("name").ToStdString());
			while (child) {
				DeserializeNode(child, item);
				child = child->GetNext();
			}
		} else
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

bool amdOutlineFilesPanel::Save() {
	if (!fs::exists(amdGetManager()->GetPath(true).ToStdString()))
		return false;
	
	SaveCurrentBuffer();
	wxXmlDocument doc;
	
	wxXmlNode* root = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "AO-Files");
	doc.SetRoot(root);

	wxDataViewItemArray rootFiles;
	m_outlineTreeModel->GetChildren(wxDataViewItem(nullptr), rootFiles);

	for (int i = 0; i < rootFiles.GetCount(); i++) {
		if (m_outlineTreeModel->IsContainer(rootFiles[i]))
			root->AddChild(SerializeFolder(rootFiles[i]));
		else
			root->AddChild(SerializeFile(rootFiles[i]));
	}

	if (doc.Save(amdGetManager()->GetPath(true) + "Files\\Outline\\OutlineFiles.xml"))
		return true;
	else
		return false;
}

bool amdOutlineFilesPanel::Load() {
	ClearAll();
	Init();

	wxXmlDocument doc;
	if (!doc.Load(amdGetManager()->GetPath(true) + "Files\\Outline\\OutlineFiles.xml")) {
		Init();
		return false;
	}

	wxDataViewItemArray array;
	array.Add(wxDataViewItem(m_outlineTreeModel->GetResearchNode()));
	//array.Add(wxDataViewItem(m_outlineTreeModel->GetCharactersNode()));
	//array.Add(wxDataViewItem(m_outlineTreeModel->GetLocationsNode()));
	
	wxXmlNode* child = doc.GetRoot()->GetChildren();
	wxXmlNode* child2 = child->GetChildren();
	
	while (child2) {
		DeserializeNode(child2, wxDataViewItem(m_outlineTreeModel->GetResearchNode()));
		child2 = child2->GetNext();
	}

	child = child->GetNext()->GetNext()->GetNext();

	while (child) {
		DeserializeNode(child, wxDataViewItem(nullptr));
		child = child->GetNext();
	}

	return true;
}

void amdOutlineFilesPanel::ClearAll() {
	m_outlineTreeModel->Clear();
	m_textCtrl->Clear();
}