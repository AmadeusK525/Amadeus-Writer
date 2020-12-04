#include "OutlineFiles.h"

#include "MainFrame.h"

#include <wx\wx.h>

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
	m_characters->append(node);
	wxDataViewItem parent(m_characters);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

wxDataViewItem OutlineTreeModel::addToLocations(const string& title) {
	OutlineTreeModelNode* node = new OutlineTreeModelNode(m_locations, title, wxRichTextBuffer());
	m_locations->append(node);
	wxDataViewItem parent(m_locations);
	wxDataViewItem child(node);
	this->ItemAdded(parent, child);

	return child;
}

void OutlineTreeModel::deleteItem(const wxDataViewItem& item) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)item.GetID();
	if (!node)      // happens if item.IsOk()==false
		return;

	if (node == m_research || node == m_characters || node == m_locations)
		wxMessageBox("Cannot remove special folders!");

	// first remove the node from the parent's array of children;
	// NOTE: OutlineTreeModelNode is only an array of _pointers_
	//       thus removing the node from it doesn't result in freeing it
	node->getParent()->getChildren().Remove(node);
	wxDataViewItem parent(node->getParent());

	// free the node
	delete node;

	ItemDeleted(parent, item);
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
	return node->isContainer();
}

unsigned int OutlineTreeModel::GetChildren(const wxDataViewItem& parent,
	wxDataViewItemArray& array) const {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)parent.GetID();
	if (!node) {
		array.Add(wxDataViewItem(m_research));
		array.Add(wxDataViewItem(m_characters));
		array.Add(wxDataViewItem(m_locations));
		return 3;
	}

	if (node->getChildCount() == 0) {
		return 0;
	}

	unsigned int count = node->getChildren().GetCount();

	for (unsigned int pos = 0; pos < count; pos++) {
		OutlineTreeModelNode* child = node->getChildren().Item(pos);
		array.Add(wxDataViewItem(child));
	}

	return count;
}


BEGIN_EVENT_TABLE(OutlineFilesPanel, wxSplitterWindow)

EVT_DATAVIEW_SELECTION_CHANGED(TREE_Files, OutlineFilesPanel::onSelectionChanged)
EVT_DATAVIEW_ITEM_EDITING_STARTED(TREE_Files, OutlineFilesPanel::onEditingStart)
EVT_DATAVIEW_ITEM_EDITING_DONE(TREE_Files, OutlineFilesPanel::onEditingEnd)

END_EVENT_TABLE()

OutlineFilesPanel::OutlineFilesPanel(wxWindow* parent) : wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize, 768L | wxSP_LIVE_UPDATE) {
	content = new wxRichTextCtrl(this);

	wxPanel* leftPanel = new wxPanel(this);
	files = new wxDataViewCtrl(leftPanel, TREE_Files, wxDefaultPosition, wxDefaultSize,
		wxDV_HORIZ_RULES | wxDV_NO_HEADER | wxDV_SINGLE | wxDV_ROW_LINES);
	filesTB = new wxToolBar(leftPanel, -1);
	filesTB->SetBackgroundColour(wxColour(50, 50, 50));

	filesTB->AddTool(-1, "", wxBITMAP_PNG(addFile), "Add new file.");
	filesTB->AddTool(-1, "", wxBITMAP_PNG(addFolder), "Add new folder.");

	filesTB->Realize();

	files->EnableDragSource(wxDataFormat(wxDF_UNICODETEXT));
	files->EnableDropTarget(wxDataFormat(wxDF_UNICODETEXT));

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
	files->SetMinSize(wxSize(20, -1));

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

void OutlineFilesPanel::onSelectionChanged(wxDataViewEvent& event) {
	OutlineTreeModelNode* node = (OutlineTreeModelNode*)(event.GetItem().GetID());
	OutlineTreeModelNode* parent = nullptr;
	if (node)
		parent = node->getParent();

	if (parent == nullptr) {
		content->Clear();
		content->SetEditable(false);
		return;
	}

	content->GetBuffer() = node->m_buffer;
	content->Refresh();

	if (parent == outlineTreeModel->getCharactersNode() ||
		parent == outlineTreeModel->getLocationsNode())
		content->SetEditable(false);
}

void OutlineFilesPanel::onEditingStart(wxDataViewEvent& event) {
	event.Veto();
}

void OutlineFilesPanel::onEditingEnd(wxDataViewEvent& WXUNUSED(event)) {}

void OutlineFilesPanel::OnUnsplit(wxWindow* WXUNUSED(window)) {
	SplitVertically(files, content, 200);
}

bool OutlineFilesPanel::save() {
	return false;
}

bool OutlineFilesPanel::load() {
	clearAll();

	init();
	return false;
}

void OutlineFilesPanel::clearAll() {

}