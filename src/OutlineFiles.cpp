#include "OutlineFiles.h"

#include "MainFrame.h"

#include <wx\xml\xml.h>
#include <wx\sstream.h>
#include <wx\richtext\richtextxml.h>

namespace fs = boost::filesystem;

OutlineTreeModel::OutlineTreeModel() {
	m_research = new OutlineTreeModelNode(nullptr, "Research");
	m_characters = new OutlineTreeModelNode(nullptr, "Characters");
	m_locations = new OutlineTreeModelNode(nullptr, "Locations");
}

wxString OutlineTreeModel::getTitle(const wxDataViewItem& item) const {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	if (!node)
		return wxEmptyString;

	return node->m_title;
}

wxRichTextBuffer& OutlineTreeModel::getBuffer(const wxDataViewItem& item) const {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node->m_buffer;
}

wxDataViewItem OutlineTreeModel::addToResearch(const string& title) {
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_research, title, wxRichTextBuffer());
	m_research->append(node);
	wxDataViewItem parent(m_research);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::addToCharacters(const string& title) {
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_characters, title, wxRichTextBuffer());
	wxDataViewItem parent(m_characters);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::addToLocations(const string& title) {
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_locations, title, wxRichTextBuffer());
	wxDataViewItem parent(m_locations);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::appendFile(wxDataViewItem& parent, const string& name, const wxRichTextBuffer& buffer) {
	OutlineTreeModelNode* parentNode = (OutlineTreeModelNode*)parent.GetID();
	OutlineTreeModelNode* node = new OutlineTreeModelNode(parentNode, name, buffer);
	wxDataViewItem item(node);

	if (!parentNode)
		otherRoots.Add(node);

	ItemAdded(parent, item);

	return item;
}

wxDataViewItem OutlineTreeModel::appendFolder(wxDataViewItem& parent, const string& name) {
	OutlineTreeModelNode* parentNode = (OutlineTreeModelNode*)parent.GetID();
	OutlineTreeModelNode* node = new OutlineTreeModelNode(parentNode, name);
	wxDataViewItem item(node);

	if (!parentNode)
		otherRoots.Add(node);

	ItemAdded(parent, item);

	return item;
}

bool OutlineTreeModel::isResearch(wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_research;
}

bool OutlineTreeModel::isCharacters(wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_characters;
}

bool OutlineTreeModel::isLocations(wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	return node == m_locations;
}

void OutlineTreeModel::deleteItem(const wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	if (!node)      // happens if item.IsOk()==false
		return;

	if (node == m_research || node == m_characters || node == m_locations) {
		wxMessageBox("Cannot remove special folders!");
		return;
	}
	// first remove the node from the parent's array of children;
	// NOTE: OutlineTreeModelNode is only an array of _pointers_
	//       thus removing the node from it doesn't result in freeing it

	wxDataViewItemArray children;
	GetChildren(item, children);

	for (int i = 0; i < children.GetCount(); i++)
		deleteItem(children[i]);

	OutlineTreeModelNode* parentNode = node->getParent();
	if (parentNode)
		parentNode->getChildren().Remove(node);
	else
		otherRoots.Remove(node);

	wxDataViewItem parent(parentNode);

	// free the node
	delete node;

	ItemDeleted(parent, item);
}

void OutlineTreeModel::setItemBackgroundColour(wxDataViewItem& item, wxColour& colour) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	wxDataViewItemAttr attr = node->getAttr();

	attr.SetBackgroundColour(colour);
}

void OutlineTreeModel::setItemForegroundColour(wxDataViewItem& item, wxColour& colour) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	wxDataViewItemAttr attr = node->getAttr();

	attr.SetColour(colour);
}

void OutlineTreeModel::setItemFont(wxDataViewItem& item, wxFont& font) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	wxDataViewItemAttr attr = node->getAttr();

	attr.SetBold(font.GetWeight() >= 400);
	attr.SetItalic(font.GetStyle() == wxFONTSTYLE_ITALIC);
	attr.SetStrikethrough(font.GetStrikethrough());
}

bool OutlineTreeModel::reparent(OutlineTreeModelNode* itemNode, OutlineTreeModelNode* newParentNode) {
	OutlineTreeModelNode* oldParentNode(itemNode->getParent());

	wxDataViewItem item((void*)itemNode);
	wxDataViewItem newParent((void*)newParentNode);
	wxDataViewItem oldParent((void*)oldParentNode);


	if (!itemNode)
		return false;

	itemNode->reparent(newParentNode);

	if (!oldParentNode)
		otherRoots.Remove(itemNode);

	if (!newParentNode)
		otherRoots.Add(itemNode);

	ItemAdded(newParent, item);
	ItemDeleted(oldParent, item);
	return true;
}

bool OutlineTreeModel::reparent(OutlineTreeModelNode* itemNode, OutlineTreeModelNode* newParentNode, int n) {
	OutlineTreeModelNode* oldParentNode(itemNode->getParent());

	wxDataViewItem item((void*)itemNode);
	wxDataViewItem newParent((void*)newParentNode);
	wxDataViewItem oldParent((void*)oldParentNode);


	if (!itemNode)
		return false;

	itemNode->reparent(newParentNode, n);

	if (!oldParentNode)
		otherRoots.Remove(itemNode);

	if (!newParentNode)
		otherRoots.Insert(itemNode, n);

	ItemAdded(newParent, item);
	ItemDeleted(oldParent, item);
	return true;
}

bool OutlineTreeModel::reposition(wxDataViewItem& item, int n) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	if (node) {
		node->reposition(n);
		ItemChanged(wxDataViewItem(node->getParent()));
	} else
		return false;
	
	return true;
}

void OutlineTreeModel::clear() {
	OulineTreeModelNodePtrArray array;

	array = m_research->getChildren();
	for (int i = 0; i < array.GetCount(); i++) {
		deleteItem(wxDataViewItem(array[i]));
	}

	array = m_characters->getChildren();
	for (int i = 0; i < array.GetCount(); i++) {
		deleteItem(wxDataViewItem(array[i]));
	}

	array = m_locations->getChildren();
	for (int i = 0; i < array.GetCount(); i++) {
		deleteItem(wxDataViewItem(array[i]));
	}

	for (int i = 0; i < otherRoots.size(); i++)
		deleteItem(wxDataViewItem(otherRoots[i]));
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

	return wxDataViewItem(node->getParent());
}

bool OutlineTreeModel::IsContainer(const wxDataViewItem& item) const {
	// the invisible root node can have children
	if (!item.IsOk())
		return true;

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	if (node)
		return node->isContainer();
	else
		return false;
}

bool OutlineTreeModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	if (node)
		attr = node->getAttr();

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

		for (int i = 0; i < otherRoots.GetCount(); i++) {
			array.Add(wxDataViewItem(otherRoots.at(i)));
		}

		return n + otherRoots.GetCount();
	}

	int count = node->getChildCount();

	if (count == 0) {
		return 0;
	}

	for (int pos = 0; pos < count; pos++) {
		OutlineTreeModelNode* child = node->getChildren().Item(pos);
		array.Add(wxDataViewItem(child));
	}

	return count;
}


BEGIN_EVENT_TABLE(OutlineFilesPanel, wxSplitterWindow)

EVT_DATAVIEW_SELECTION_CHANGED(TREE_Files, OutlineFilesPanel::onSelectionChanged)
EVT_DATAVIEW_ITEM_EDITING_STARTED(TREE_Files, OutlineFilesPanel::onEditingStart)
EVT_DATAVIEW_ITEM_EDITING_DONE(TREE_Files, OutlineFilesPanel::onEditingEnd)
EVT_DATAVIEW_ITEM_BEGIN_DRAG(TREE_Files, OutlineFilesPanel::onBeginDrag)
EVT_DATAVIEW_ITEM_DROP_POSSIBLE(TREE_Files, OutlineFilesPanel::onDropPossible)
EVT_DATAVIEW_ITEM_DROP(TREE_Files, OutlineFilesPanel::onDrop)

EVT_TOOL(TOOL_NewFile, OutlineFilesPanel::newFile)
EVT_TOOL(TOOL_NewFolder, OutlineFilesPanel::newFolder)

END_EVENT_TABLE()

OutlineFilesPanel::OutlineFilesPanel(wxWindow* parent) : wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize, 768L | wxSP_LIVE_UPDATE) {
	content = new wxRichTextCtrl(this);
	content->SetBackgroundColour(wxColour(40, 40, 40));
	content->Refresh();

	basicAttr.SetFontSize(13);
	basicAttr.SetTextColour(wxColour(245, 245, 245));

	content->SetBasicStyle(basicAttr);

	leftPanel = new wxPanel(this);
	files = new wxDataViewCtrl(leftPanel, TREE_Files, wxDefaultPosition, wxDefaultSize,
		wxDV_NO_HEADER | wxDV_SINGLE);
	files->GetMainWindow()->Bind(wxEVT_KEY_DOWN, &OutlineFilesPanel::onKeyDownDataView, this);
	files->GetMainWindow()->Bind(wxEVT_RIGHT_DOWN, &OutlineFilesPanel::onRightDownDataView, this);

	filesTB = new wxToolBar(leftPanel, -1);
	filesTB->AddTool(TOOL_NewFile, "", wxBITMAP_PNG(addFile), "Add new file.");
	filesTB->AddTool(TOOL_NewFolder, "", wxBITMAP_PNG(addFolder), "Add new folder.");
	filesTB->SetBackgroundColour(wxColour(60, 60, 60));

	filesTB->Realize();

	files->EnableDragSource(wxDataFormat(wxRichTextBufferDataObject::GetRichTextBufferFormatId()));
	files->EnableDropTarget(wxDataFormat(wxRichTextBufferDataObject::GetRichTextBufferFormatId()));
	files->EnableDragSource(wxDataFormat(wxDF_FILENAME));
	files->EnableDragSource(wxDataFormat(wxDF_FILENAME));
	files->SetBackgroundColour(wxColour(250, 250, 250));

	outlineTreeModel = new OutlineTreeModel();
	files->AssociateModel(outlineTreeModel.get());

	wxDataViewTextRenderer* tr = new wxDataViewTextRenderer();
	wxDataViewColumn* column0 = new wxDataViewColumn("Files", tr, 0, FromDIP(200), wxALIGN_LEFT);
	files->AppendColumn(column0);

	wxBoxSizer* panSizer = new wxBoxSizer(wxVERTICAL);
	panSizer->Add(filesTB, wxSizerFlags(0).Expand());
	panSizer->Add(files, wxSizerFlags(1).Expand());

	leftPanel->SetSizer(panSizer);

	content->SetMinSize(wxSize(20, -1));
	leftPanel->SetMinSize(wxSize(20, -1));

	SplitVertically(leftPanel, content, 200);
}

void OutlineFilesPanel::init() {
	std::map<std::string, Character>& charMap = MainFrame::characters;
	std::map<std::string, Location>& locMap = MainFrame::locations;

	for (auto it = charMap.begin(); it != charMap.end(); it++)
		appendCharacter(it->second);

	for (auto it = locMap.begin(); it != locMap.end(); it++)
		appendLocation(it->second);
}

void OutlineFilesPanel::appendCharacter(Character& character) {
	wxDataViewItem item = outlineTreeModel->addToCharacters(character.name);
	wxRichTextBuffer& buffer = ((OutlineTreeModelNode*)item.GetID())->m_buffer;
	buffer.SetBasicStyle(content->GetBasicStyle());
	buffer.BeginSuppressUndo();

	if (character.image.IsOk()) {
		wxImage image = character.image;
		double ratio = (double)image.GetWidth() / (double)image.GetHeight();

		image.Rescale(200, 200 / ratio, wxIMAGE_QUALITY_HIGH);
		buffer.AddImage(image);
	}

	buffer.BeginFontSize(14);
	buffer.BeginBold();
	buffer.InsertTextWithUndo(2, "\nName: ", nullptr);
	buffer.EndBold();
	buffer.EndFontSize();
	buffer.InsertTextWithUndo(buffer.GetText().size(), character.name, nullptr);

	if (character.age != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nAge: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.age, nullptr);
	}

	if (character.sex != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nSex: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.sex, nullptr);
	}

	if (character.height != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nHeight: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.height, nullptr);
	}

	if (character.nick != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nNickname: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.nick, nullptr);
	}

	if (character.nat != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nNationality: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.nat, nullptr);
	}

	if (character.appearance != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nAppearance:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.appearance, nullptr);
	}

	if (character.personality != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nPersonality:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.personality, nullptr);
	}

	if (character.backstory != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nBackstory:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.backstory, nullptr);
	}

	buffer.SetName(character.name);
}

void OutlineFilesPanel::appendLocation(Location& location) {
	wxDataViewItem item = outlineTreeModel->addToLocations(location.name);
	wxRichTextBuffer& buffer = ((OutlineTreeModelNode*)item.GetID())->m_buffer;
	buffer.SetBasicStyle(content->GetBasicStyle());
	buffer.BeginSuppressUndo();

	if (location.image.IsOk()) {
		wxImage image = location.image;
		double ratio = (double)image.GetWidth() / (double)image.GetHeight();

		image.Rescale(200, 200 / ratio, wxIMAGE_QUALITY_HIGH);
		buffer.AddImage(image);
	}

	buffer.BeginFontSize(14);
	buffer.BeginBold();
	buffer.InsertTextWithUndo(2, "\nName: ", nullptr);
	buffer.EndBold();
	buffer.EndFontSize();
	buffer.InsertTextWithUndo(buffer.GetText().size(), location.name, nullptr);

	buffer.BeginBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nImportance: ", nullptr);
	buffer.EndBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), location.importance, nullptr);
	
	buffer.BeginBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), "\nSpace type: ", nullptr);
	buffer.EndBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), location.type, nullptr);

	if (location.background != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nHistorical background:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.background, nullptr);
	}

	if (location.natural != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nNatural characteristics:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.natural, nullptr);
	}

	if (location.architecture != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nArchitecture:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.architecture, nullptr);
	}

	if (location.economy != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nEconomy:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.economy, nullptr);
	}

	if (location.culture != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\n\nCulture:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), location.culture, nullptr);
	}

	buffer.SetName(location.name);
}

void OutlineFilesPanel::deleteCharacter(Character& character) {
	OulineTreeModelNodePtrArray& characters = outlineTreeModel->getCharacters();
	for (int i = 0; i < characters.Count(); i++) {
		if (characters[i]->m_title == character.name) {
			wxDataViewItem item(characters[i]);
			outlineTreeModel->deleteItem(item);
			return;
		}
	}
}

void OutlineFilesPanel::deleteLocation(Location& location) {
	OulineTreeModelNodePtrArray& locations = outlineTreeModel->getLocations();
	for (int i = 0; i < locations.Count(); i++) {
		if (locations[i]->m_title == location.name) {
			wxDataViewItem item(locations[i]);
			outlineTreeModel->deleteItem(item);
			return;
		}
	}
}

void OutlineFilesPanel::newFile(wxCommandEvent& event) {
	wxDataViewItem sel = files->GetSelection(); 

	if (sel.IsOk()) {
		if (!outlineTreeModel->IsContainer(sel))
			sel = outlineTreeModel->GetParent(sel);
	}

	if (outlineTreeModel->isCharacters(sel) || outlineTreeModel->isLocations(sel))
		return;

	outlineTreeModel->appendFile(sel, "New file", wxRichTextBuffer());
	MainFrame::isSaved = false;
}

void OutlineFilesPanel::newFolder(wxCommandEvent& event) {
	wxDataViewItem sel = files->GetSelection();

	if (sel.IsOk()) {
		if (!outlineTreeModel->IsContainer(sel))
			sel = outlineTreeModel->GetParent(sel);
	}

	if (outlineTreeModel->isCharacters(sel) || outlineTreeModel->isLocations(sel))
		return;

	outlineTreeModel->appendFolder(sel, "New folder");
	MainFrame::isSaved = false;
}

void OutlineFilesPanel::deleteItem(wxDataViewItem& item) {
	wxDataViewItem parent = outlineTreeModel->GetParent(item);

	if (outlineTreeModel->isResearch(item) || outlineTreeModel->isCharacters(item) ||
		outlineTreeModel->isLocations(item) || outlineTreeModel->isCharacters(parent) ||
		outlineTreeModel->isLocations(parent)) {
		wxMessageBox("Cannot delete special items!");
		return;
	}

	string msg = "'" + outlineTreeModel->getTitle(item) + "'";

	if (outlineTreeModel->IsContainer(item))
		msg.insert(0, "folder ");
	else
		msg.insert(0, "file ");

	wxMessageDialog* dlg = new wxMessageDialog(nullptr,
		"Are you sure you want to delete " + msg + "? This action cannot be undone.", "Warning",
		wxYES_NO | wxNO_DEFAULT | wxICON_WARNING | wxCENTER);

	if (dlg->ShowModal() == wxID_YES) {
		outlineTreeModel->deleteItem(item);
	}

	if (dlg)
		delete dlg;
}

void OutlineFilesPanel::onKeyDownDataView(wxKeyEvent& event) {
	switch (event.GetKeyCode()) {
	case WXK_DELETE:
		wxDataViewItem sel = files->GetSelection();

		if (!sel.IsOk())
			return;

		deleteItem(sel);
		break;
	}
}

void OutlineFilesPanel::onRightDownDataView(wxMouseEvent& event) {
	wxDataViewItem item;
	wxDataViewColumn* col;
	files->HitTest(event.GetPosition(), item, col);
	files->Select(item);

	wxMenu menu;
	menu.Append(MENU_DeleteItem, "Delete");

	menu.Bind(wxEVT_MENU, &OutlineFilesPanel::onMenuDataView, this);
	PopupMenu(&menu);
}

void OutlineFilesPanel::onMenuDataView(wxCommandEvent& event) {
	switch (event.GetId()) {
	case MENU_DeleteItem:
		deleteItem(files->GetSelection());
		break;
	}
}

void OutlineFilesPanel::onSelectionChanged(wxDataViewEvent& event) {
	wxDataViewItem item(event.GetItem());

	if (outlineTreeModel->IsContainer(item)) {
		content->Clear();
		content->Disable();
		return;
	}

	content->Enable();

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)(item.GetID());

	content->GetBuffer() = node->m_buffer;
	content->Refresh();
	wxDataViewItem parentItem(node->getParent());

	if (outlineTreeModel->isCharacters(parentItem) ||
		outlineTreeModel->isLocations(parentItem))
		content->SetEditable(false);
}

void OutlineFilesPanel::onEditingStart(wxDataViewEvent& event) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)event.GetId();
	string name = node->m_title;

	if (name == "Research" || name == "Characters" || name == "Locations")
		event.Veto();
	else
		files->EditItem(event.GetItem(), files->GetColumn(0));
}

void OutlineFilesPanel::onEditingEnd(wxDataViewEvent& WXUNUSED(event)) {}

void OutlineFilesPanel::onBeginDrag(wxDataViewEvent& event) {
	wxDataViewItem item(event.GetItem());
	wxDataViewItem parent = outlineTreeModel->GetParent(item);

	if (outlineTreeModel->isResearch(item) || outlineTreeModel->isCharacters(item) ||
		outlineTreeModel->isLocations(item) || outlineTreeModel->isCharacters(parent) ||
		outlineTreeModel->isLocations(parent)) {
		event.Veto();
		return; 
	}

	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	wxRichTextBufferDataObject* obj = new wxRichTextBufferDataObject(new wxRichTextBuffer(node->m_buffer));
	event.SetDataObject(obj);
	event.SetDragFlags(wxDrag_AllowMove); // allows both copy and move

	nodeForDnD = node;
	itemForDnD = item;
}

void OutlineFilesPanel::onDropPossible(wxDataViewEvent& event) {
	if (event.GetDataFormat() != wxRichTextBufferDataObject::GetRichTextBufferFormatId()) {
		nodeForDnD = nullptr;
		event.Veto();
	} else {
		wxDataViewItem ht;
		wxDataViewColumn* cht = files->GetColumn(0);

		files->HitTest(event.GetPosition(), ht, cht);

		if (outlineTreeModel->IsContainer(ht) && ht != itemForDnD)
			event.SetDropEffect(wxDragMove);
		else
			event.SetDropEffect(wxDragNone);
	}
}

void OutlineFilesPanel::onDrop(wxDataViewEvent& event) {
	wxDataViewItem item(event.GetItem());
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();

	int ipc = 0;
	int cpc = 0;

	OutlineTreeModelNode* dummy = node;

	while (dummy) {
		dummy = dummy->getParent();
		ipc++;
	}

	dummy = nodeForDnD;

	while (dummy) {
		dummy = dummy->getParent();
		cpc++;
	}

	bool isChild = ipc > cpc;

	if (nodeForDnD == node || outlineTreeModel->isCharacters(item) ||
		outlineTreeModel->isLocations(item) || isChild)
		return;

	if (event.GetDataFormat() != wxRichTextBufferDataObject::GetRichTextBufferFormatId()) {
		event.Veto();
		return;
	}

	wxRichTextBufferDataObject obj;
	obj.SetData(event.GetDataFormat(), event.GetDataSize(), event.GetDataBuffer());

	int index = event.GetProposedDropIndex();

	if (item.IsOk()) {
		if (outlineTreeModel->IsContainer(item)) {
			if (nodeForDnD) {
				if (nodeForDnD->getParent() != node) {
					if (index == -1)
						outlineTreeModel->reparent(nodeForDnD, node);
					else
						outlineTreeModel->reparent(nodeForDnD, node, index);
				} else {
					if (index == -1)
						outlineTreeModel->reposition(itemForDnD, 0);
					else
						outlineTreeModel->reposition(itemForDnD, index);
				}
			}

		} else
			wxLogMessage("Dropped on item. Nothing happened.");

	} else {
		if (nodeForDnD->getParent() != node) {
			if (index == -1)
				outlineTreeModel->reparent(nodeForDnD, nullptr);
			else
				outlineTreeModel->reparent(nodeForDnD, nullptr, index - 3);
		}
	}

	files->Select(itemForDnD);
}

void OutlineFilesPanel::OnUnsplit(wxWindow* WXUNUSED(window)) {
	SplitVertically(leftPanel, content, 200);
}

wxXmlNode* OutlineFilesPanel::getNodeWithChildren(wxDataViewItem& item) {
	wxDataViewItemAttr attr;
	outlineTreeModel->GetAttr(item, 0, attr);

	wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, "Folder");
	node->AddAttribute("name", outlineTreeModel->getTitle(item));
	node->AddAttribute("bg-color", attr.GetBackgroundColour().GetAsString());

	wxDataViewItemArray children;
	outlineTreeModel->GetChildren(item, children);

	string name;
	string dummyName;
	wxXmlNode* child;

	for (int i = 0; i < children.GetCount(); i++) {
		if (outlineTreeModel->IsContainer(children[i])) {
			child = getNodeWithChildren(children[i]);
			node->AddChild(child);
			continue;
		}

		wxString buffer;
		wxStringOutputStream stream(&buffer);
		outlineTreeModel->getBuffer(children[i]).SaveFile(stream, wxRICHTEXT_TYPE_XML);

		name = outlineTreeModel->getTitle(children[i]);
		outlineTreeModel->GetAttr(children[i], 0, attr);
		child = new wxXmlNode(node, wxXML_ELEMENT_NODE, "File");
		child->AddAttribute("name", name);
		child->AddAttribute("bg-color", attr.GetBackgroundColour().GetAsString());
		child->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", buffer));
	}

	return node;
}

void OutlineFilesPanel::deserializeNode(wxXmlNode* node, wxDataViewItem& parent) {
	wxXmlNode* child = node->GetChildren();
		
	if (node->GetName() == "File") {
		wxString bufString = child->GetContent();
		wxStringInputStream stream(bufString);

		wxRichTextBuffer buffer;
		buffer.LoadFile(stream, wxRICHTEXT_TYPE_XML);

		outlineTreeModel->appendFile(parent, node->GetAttribute("name").ToStdString(), buffer);
	} else if (node->GetName() == "Folder") {
		if (child) {
			wxDataViewItem item = outlineTreeModel->appendFolder(parent, node->GetAttribute("name").ToStdString());
			while (child) {
				deserializeNode(child, item);
				child = child->GetNext();
			}
		} else
			outlineTreeModel->appendFolder(parent, node->GetAttribute("name").ToStdString());
	}
}

bool OutlineFilesPanel::save() {
	if (!fs::exists(MainFrame::currentDocFolder))
		return false;

	wxXmlDocument doc;
	
	wxXmlNode* root = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "AO-Files");
	doc.SetRoot(root);

	wxDataViewItemArray rootFiles;
	outlineTreeModel->GetChildren(wxDataViewItem(nullptr), rootFiles);

	for (int i = 0; i < rootFiles.GetCount(); i++) {
		root->AddChild(getNodeWithChildren(rootFiles[i]));
	}

	if (doc.Save(MainFrame::currentDocFolder + "\\Files\\Outline\\OutlineFiles.xml"))
		return true;
	else
		return false;
}

bool OutlineFilesPanel::load() {
	clearAll();

	wxXmlDocument doc;
	if (!doc.Load(MainFrame::currentDocFolder + "\\Files\\Outline\\OutlineFiles.xml")) {
		init();
		return false;
	}

	wxDataViewItemArray array;
	array.Add(wxDataViewItem(outlineTreeModel->getResearchNode()));
	array.Add(wxDataViewItem(outlineTreeModel->getCharactersNode()));
	array.Add(wxDataViewItem(outlineTreeModel->getLocationsNode()));
	
	wxXmlNode* child = doc.GetRoot()->GetChildren();
	wxXmlNode* child2 = child->GetChildren();

	for (int i = 0; i < 3; i++) {
		while (child2) {
			deserializeNode(child2, array[i]);
			child2 = child2->GetNext();
		}

		if (child)
			child = child->GetNext();

		if (child)
			child2 = child->GetChildren();
	}

	while (child) {
		deserializeNode(child, wxDataViewItem(nullptr));
		child = child->GetNext();
	}

	return true;
}

void OutlineFilesPanel::clearAll() {
	outlineTreeModel->clear();
	content->Clear();
}