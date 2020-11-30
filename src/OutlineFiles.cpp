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
}

void OutlineFiles::init() {
	if (load())
		return;

	researchRoot = files->AppendContainer(wxDataViewItem(0), "Research", 0);
	charactersRoot = files->AppendContainer(wxDataViewItem(0), "Characters");
	locationsRoot = files->AppendContainer(wxDataViewItem(0), "Locations");

	std::map<std::string, Character>& charMap = MainFrame::characters;

	for (auto it = charMap.begin(); it != charMap.end(); it++)
		appendCharacter(it->second);
}

void OutlineFiles::appendCharacter(Character& character) {
	wxDataViewItem item = files->AppendItem(charactersRoot, character.name);

	wxRichTextBuffer buffer;

	buffer.BeginSuppressUndo();

	if (character.image.IsOk()) {
		wxImage image = character.image;
		double ratio = (double)image.GetWidth() / (double)image.GetHeight();

		image.Rescale(200, 200 / ratio);
		buffer.AddImage(image);
	}

	buffer.BeginBold();
	buffer.InsertTextWithUndo(2, "\nName: ", nullptr);
	buffer.EndBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), character.name + "\n", nullptr);

	if (character.age != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Age: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.age + "\n", nullptr);
	}

	if (character.sex != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Sex: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.sex + "\n", nullptr);
	}

	if (character.height != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Height: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.height + "\n", nullptr);
	}

	if (character.nick != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Nickname: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.nick + "\n", nullptr);
	}

	if (character.nat != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Nationality: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.nat + "\n", nullptr);
	}

	if (character.appearance != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nAppearance:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.appearance + "\n", nullptr);
	}

	if (character.personality != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nPersonality:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.personality + "\n", nullptr);
	}

	if (character.backstory != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nBackstory:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.backstory + "\n", nullptr);
	}

	buffer.SetName(character.name);
	charBuffers.push_back(buffer);
	charFiles.push_back(item);
}
/*
void OutlineFiles::appendLocation(Location& location) {
	wxDataViewItem item = files->AppendItem(locationsRoot, location.name);

	wxRichTextBuffer buffer;

	buffer.BeginSuppressUndo();

	if (location.image.IsOk()) {
		wxImage image = location.image;
		double ratio = (double)image.GetWidth() / (double)image.GetHeight();

		image.Rescale(200, 200 / ratio);
		buffer.AddImage(image);
	}

	buffer.BeginBold();
	buffer.InsertTextWithUndo(2, "Name: ", nullptr);
	buffer.EndBold();
	buffer.InsertTextWithUndo(buffer.GetText().size(), location.name + "\n", nullptr);

	if (location. != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Age: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.age + "\n", nullptr);
	}

	if (character.sex != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Sex: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.sex + "\n", nullptr);
	}

	if (character.height != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Height: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.height + "\n", nullptr);
	}

	if (character.nick != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Nickname: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.nick + "\n", nullptr);
	}

	if (character.nat != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "Nationality: ", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.nat + "\n", nullptr);
	}

	if (character.appearance != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nAppearance:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.appearance + "\n", nullptr);
	}

	if (character.personality != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nPersonality:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.personality + "\n", nullptr);
	}

	if (character.backstory != "") {
		buffer.BeginBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), "\nBackstory:\n", nullptr);
		buffer.EndBold();
		buffer.InsertTextWithUndo(buffer.GetText().size(), character.backstory + "\n", nullptr);
	}

	buffer.SetName(character.name);
	charBuffers.push_back(buffer);
	charFiles.push_back(item);
}
*/
void OutlineFiles::deleteCharacter(Character& character) {
	auto itBuffer = charBuffers.begin();
	auto itFile = charFiles.begin();

	for (int i = 0; i < charFiles.size(); i++) {
		if (files->GetItemText(charFiles[i]) == character.name) {
			files->DeleteItem(charFiles[i]);

			if (current == itBuffer->GetName())
				content->Clear();

			charBuffers.erase(itBuffer);
			charFiles.erase(itFile);
			return;
		}

		itBuffer++;
		itFile++;
	}
}

void OutlineFiles::deleteLocations(Location& location) {}

void OutlineFiles::onSelectionChanged(wxDataViewEvent& event) {
	string name = files->GetItemText(event.GetItem());

	for (auto it : charBuffers) {
		if (it.GetName() == name) {
			content->GetBuffer() = it;
			content->Refresh();
			content->SetEditable(false);
			return;
		}
	}

	for (auto it : locBuffers) {
		if (it.GetName() == name) {
			content->GetBuffer() = it;
			content->Refresh();
			content->SetEditable(false);
			return;
		}
	}

	content->SetEditable(false);
	content->Clear();
}

void OutlineFiles::onEditingStart(wxDataViewEvent& event) {
	string name = files->GetItemText(event.GetItem());
	 
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
	return false;
}
