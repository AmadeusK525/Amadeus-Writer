#include "OutlineFiles.h"

#include "MainFrame.h"

#include <wx\wx.h>

BEGIN_EVENT_TABLE(OutlineFiles, wxSplitterWindow)

EVT_DATAVIEW_SELECTION_CHANGED(TREE_Files, OutlineFiles::onSelectionChanged)
EVT_DATAVIEW_ITEM_EDITING_STARTED(TREE_Files, OutlineFiles::onEditingStart)

END_EVENT_TABLE()

OutlineFiles::OutlineFiles(wxWindow* parent) : wxSplitterWindow(parent, -1, wxDefaultPosition, wxDefaultSize, 768L | wxSP_LIVE_UPDATE) {
	files = new wxDataViewTreeCtrl(this, TREE_Files);
	content = new wxRichTextCtrl(this);
	files->SetMinSize(wxSize(20, -1));
	content->SetMinSize(wxSize(20, -1));

	SplitVertically(files, content, 200);

	wxImageList* imageList = new wxImageList(24, 24);
	imageList->Add(wxICON(research));
	files->AssignImageList(imageList);

	researchRoot = files->AppendContainer(wxDataViewItem(0), "Research", 0);
	charactersRoot = files->AppendContainer(wxDataViewItem(0), "Characters");
	locationsRoot = files->AppendContainer(wxDataViewItem(0), "Locations");
}

void OutlineFiles::init() {
	std::map<std::string, Character>& charMap = MainFrame::characters;
	std::map<std::string, Location>& locMap = MainFrame::locations;

	for (auto it = charMap.begin(); it != charMap.end(); it++)
		appendCharacter(it->second);

	for (auto it = locMap.begin(); it != locMap.end(); it++)
		appendLocation(it->second);
}

void OutlineFiles::appendCharacter(Character& character) {
	wxDataViewItem item = files->AppendItem(charactersRoot, character.name);

	wxRichTextBuffer buffer;

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
	buffer.InsertTextWithUndo(buffer.GetText().size(), character.name, nullptr);
	buffer.EndFontSize();

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
	charBuffers.push_back(buffer);
	charItems.push_back(item);
}

void OutlineFiles::appendLocation(Location& location) {
	wxDataViewItem item = files->AppendItem(locationsRoot, location.name);

	wxRichTextBuffer buffer;

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
	buffer.InsertTextWithUndo(buffer.GetText().size(), location.name, nullptr);
	buffer.EndFontSize();

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
	locBuffers.push_back(buffer);
	locItems.push_back(item);
}

void OutlineFiles::deleteCharacter(Character& character) {
	auto itBuffer = charBuffers.begin();
	auto itFile = charItems.begin();

	for (int i = 0; i < charItems.size(); i++) {
		if (files->GetItemText(charItems[i]) == character.name) {
			files->DeleteItem(charItems[i]);

			if (current == itBuffer->GetName())
				content->Clear();

			charBuffers.erase(itBuffer);
			charItems.erase(itFile);
			return;
		}

		itBuffer++;
		itFile++;
	}
}

void OutlineFiles::deleteLocation(Location& location) {
	auto itBuffer = locBuffers.begin();
	auto itFile = locItems.begin();

	for (int i = 0; i < locItems.size(); i++) {
		if (files->GetItemText(locItems[i]) == location.name) {
			files->DeleteItem(locItems[i]);

			if (current == itBuffer->GetName())
				content->Clear();

			locBuffers.erase(itBuffer);
			locItems.erase(itFile);
			return;
		}

		itBuffer++;
		itFile++;
	}
}

void OutlineFiles::onSelectionChanged(wxDataViewEvent& event) {
	string name = files->GetItemText(event.GetItem());

	for (auto it : charBuffers) {
		if (it.GetName() == name) {
			content->GetBuffer() = it;
			content->Refresh();
			content->SetEditable(false);

			current = it.GetName();
			return;
		}
	}

	for (auto it : locBuffers) {
		if (it.GetName() == name) {
			content->GetBuffer() = it;
			content->Refresh();
			content->SetEditable(false);

			current = it.GetName();
			return;
		}
	}

	content->SetEditable(false);
	content->Clear();
}

void OutlineFiles::onEditingStart(wxDataViewEvent& event) {
	string name = files->GetItemText(event.GetItem());
	
	wxMessageBox(name);

	if (name == "Research" || name == "Characters" || name == "Locations")
		event.Veto();
}

void OutlineFiles::OnUnsplit(wxWindow* window) {
	SplitVertically(files, content, 200);
}

bool OutlineFiles::save() {
	return false;
}

bool OutlineFiles::load() {
	clearAll();

	init();
	return false;
}

void OutlineFiles::clearAll() {
	files->DeleteChildren(researchRoot);
	files->DeleteChildren(charactersRoot);
	files->DeleteChildren(locationsRoot);

	researchBuffers.clear();
	charBuffers.clear();
	locBuffers.clear();

	researchItems.clear();
	charItems.clear();
	locItems.clear();
}