#include "ProjectManager.h"

#include "ProjectWizard.h"
#include "MainFrame.h"
#include "ElementsNotebook.h"
#include "ChaptersGrid.h"
#include "Outline.h"
#include "Release.h"
#include "OutlineFiles.h"
#include "ElementShowcases.h"

#include <wx\progdlg.h>
#include <wx\utils.h>

///////////////////////////////////////////////////////////////////
//////////////////////////// SQLStorage ///////////////////////////
///////////////////////////////////////////////////////////////////


amdProjectSQLDatabase::amdProjectSQLDatabase(wxFileName& path) {
	if (path.IsOk() && !path.IsDir()) {
		Open(path.GetFullPath());
	}
}

void amdProjectSQLDatabase::Init() {
	if (!TableExists("characters")) {
		CreateAllTables();
	} else
		wxMessageBox("OH MY GOD FUCKER");
}

void amdProjectSQLDatabase::CreateAllTables() {
	wxString create("CREATE TABLE characters (id INTEGER PRIMARY KEY, "
		"name TEXT UNIQUE NOT NULL, "
		"age INTEGER UNSIGNED, "
		"sex TEXT NOT NULL, "
		"nationality TEXT, "
		"height TEXT, "
		"nickname TEXT, "
		"appearance TEXT, "
		"personality TEXT, "
		"backstory TEXT);");

	ExecuteUpdate(create);
}


///////////////////////////////////////////////////////////////////////
//////////////////////////// ProjectManager ///////////////////////////
///////////////////////////////////////////////////////////////////////


amdProjectManager::amdProjectManager() {

}

bool amdProjectManager::Init() {
	if (m_isInitialized)
		return false;

	if (m_project.amdFile.IsOk()) {
		if (!m_project.amdFile.FileExists()) {
			Book book;
			book.title = m_project.amdFile.GetName();

			m_project.books.push_back(book);
		}

		if (!m_mainFrame) {
			m_mainFrame = new amdMainFrame("New Amadeus project - " + m_project.amdFile.GetName(),
				this, wxDefaultPosition, wxDefaultSize);

			m_elements = (m_mainFrame->GetElementsNotebook());
			m_chaptersNote = (m_mainFrame->GetChaptersNotebook());
			m_outline = (m_mainFrame->GetOutline());
			m_release = (m_mainFrame->GetRelease());
		}

		try {
			wxSQLite3Database::InitializeSQLite();
			m_storage.Open(m_project.amdFile.GetFullPath());
			m_storage.Init();
		} catch (wxSQLite3Exception& e) {
			wxMessageBox(wxString("Exception thrown - ") << e.GetMessage());
		}

		m_isInitialized = true;

		return true;
	}

	return false;
}

bool amdProjectManager::SaveProject() {
	return DoSaveProject(m_project.amdFile.GetFullPath());
}

bool amdProjectManager::LoadProject() {
	return DoLoadProject(m_project.amdFile.GetFullPath());
}

bool amdProjectManager::DoSaveProject(const wxString& path) {
	// First, check whether the current path of the project exists. If it doesn't,
    // the "OnSaveFileAs" is called, which calls back the "saveAs" function.

	std::ofstream file(path.ToStdString(), std::ios::binary | std::ios::out);

	if (!file.is_open()) {
		wxMessageBox("There was an issue when writing to file '" + path + "'");
		return false;
	}

	// This for loop writes how many characters there are, how many locations, how many chapters
	// and how many items (not yet implemented). It'll be used when actually loading them.
	char toWrite = m_characters.size();
	file.write(&toWrite, sizeof(char));
	toWrite = m_locations.size();
	file.write(&toWrite, sizeof(char));
	toWrite = m_chapters.size();
	file.write(&toWrite, sizeof(char));

	// Get number of all "stuff" on the project and initialize gauge
	// with the max number being that one, so that it can be incremented
	// by one each time you load something.
	int progressSize = m_characters.size() + m_locations.size() + m_chapters.size() + 1;
	int currentSize = 0;

	wxProgressDialog progress("Saving project...", path, progressSize, m_mainFrame);
	progress.Show(false);
	wxString imagePath;

	wxString cImagePath(GetPath(true) + "Images\\Characters\\");
	wxString lImagePath(GetPath(true) + "Images\\Locations\\");
	
	// This for loop calls the Save funtion of each character and saves it, besides
	// saving the image (if it exists) that is attached to it. It increments the gauge
	// by one each time.
	for (Character& it : m_characters) {
		it.Save(file);

		imagePath = cImagePath + it.name + ".jpg";
		if (wxFileName::Exists(imagePath.ToStdString()) && !it.image.IsOk())
			wxRemoveFile(imagePath.ToStdString());
		else
			if (it.image.IsOk())
				it.image.SaveFile(imagePath);

		progress.Update(currentSize++);
	} 

	char compType = Character::cCompType;
	file.write(&compType, sizeof(char));

	// Same as the above but for locations.
	for (Location& it : m_locations) {
		it.Save(file);

		imagePath = lImagePath + it.name + ".jpg";
		if (wxFileName::Exists(imagePath.ToStdString()) && !it.image.IsOk())
			wxRemoveFile(imagePath.ToStdString());
		else
			if (it.image.IsOk())
				it.image.SaveFile(imagePath);

		progress.Update(currentSize++);
	}

	compType = Location::lCompType;
	file.write(&compType, sizeof(char));

	// Same as the above but for chapters. No images are saved.
	for (Chapter& it : m_chapters) {
		it.Save(file);
		progress.Update(currentSize++);
	}
	
	file.close();

	// This calls the Save function of the outline page, which saves the corkboard
	// elements and will futurely Save other stuff.

	m_outline->SaveOutline(currentSize, &progress);

	// The following lines are just notes written in each directory warning the
	// user not to move stuff around, since that'll break the Save system (which
	// is something I'd like to avoid, so I'm looking into making it
	// way more difficult to break.
	wxString message("Please, NEVER change the names of the images nor move them somewhere else!");

	std::ofstream note(cImagePath.ToStdString() + "!!!Note!!!.txt", std::ios::out);
	note << message;
	note.close();

	note.open(lImagePath.ToStdString() + "!!!Note!!!.txt", std::ios::out);
	note << message;
	note.close();

	note.open(GetPath(true).ToStdString() + "Images\\Outline\\!!!Note!!!.txt", std::ios::out);
	note << message;
	note.close();

	note.open(GetPath(true).ToStdString() + "Files\\!!!Note!!!.txt", std::ios::out);
	note << message;
	note.close();

	// Sets Save state as saved, updates the current title and all that good stuff.
	// The attribute "saveDialog" currently does nothing.
	SetSaved(true);

	m_mainFrame->SetTitle(m_project.amdFile.GetName() + " - Amadeus Writer");
	m_mainFrame->SetFocus();
	SetLastSave();

	return true;
}

bool amdProjectManager::DoLoadProject(const wxString& path) {
	// This is the load function. It works by getting the amount of characters
	// (written at the beginning of the "saveAs" function), then calling the load
	// character function that amount of times. It does that for locations, chapters, notes and the corkboard as well.
	std::ifstream file(path.ToStdString(), std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		wxMessageBox("File could not be loaded.");
		ClearPath();
		return false;
	}
	SetProjectFileName(path);

	m_mainFrame->OnNewFile(wxCommandEvent());

	int progressSize = 0;
	int currentSize = 0;

	char charSize, locSize, chapSize;

	file.read(&charSize, sizeof(char));
	file.read(&locSize, sizeof(char));
	file.read(&chapSize, sizeof(char));

	progressSize = charSize + locSize + chapSize;

	wxProgressDialog progress("Loading project...", m_project.amdFile.GetFullPath(), progressSize, m_mainFrame,
		wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH);

	wxString cImagePath(GetPath(true) + "Images\\Characters\\");
	wxString lImagePath(GetPath(true) + "Images\\Locations\\");

	m_characters.clear();
	for (int i = 0; i < charSize; i++) {
		Character character;
		character.Load(file);

		if (wxFileName::DirExists(cImagePath.ToStdString() + character.name.ToStdString() + ".jpg")) {
			character.image.LoadFile(cImagePath +
				character.name + ".jpg");
		}

		m_characters.push_back(character);
		progress.Update(++currentSize);
	}

	char compType;
	file.read(&compType, sizeof(char));
	Character::cCompType = (CompType)compType;

	m_locations.clear();
	for (int i = 0; i < locSize; i++) {
		Location location;
		location.Load(file);

		if (wxFileName::Exists(lImagePath.ToStdString() + location.name.ToStdString() + ".jpg")) {
			location.image.LoadFile(lImagePath +
				location.name + ".jpg");
		}

		m_locations.push_back(location);
		progress.Update(++currentSize);
	}

	file.read(&compType, sizeof(char));
	Location::lCompType = (CompType)compType;

	for (int i = 0; i < chapSize; i++) {
		Chapter chapter;
		chapter.Load(file);
		m_chapters.push_back(chapter);
		m_chaptersNote->AddToList(chapter, i);
		m_chaptersNote->GetGrid()->AddButton();

		progress.Update(++currentSize);
	}
	RedeclareChapsInElements();
	file.close();

	m_outline->LoadOutline(++currentSize, &progress);
	m_release->UpdateContent();

	m_mainFrame->SetTitle(m_project.amdFile.GetName() + " - Amadeus Writer");
	m_elements->SetSearchAC(wxBookCtrlEvent());

	wxCommandEvent event;
	event.SetInt(Character::cCompType);
	m_elements->OnCharactersSortBy(event);

	event.SetInt(Location::lCompType);
	m_elements->OnLocationsSortBy(event);

	m_mainFrame->UpdateElements(wxCommandEvent());

	progress.Update(++progressSize);
	progress.Hide();

	m_isSaved = true;
	SetLastSave();
	return true;
}

void amdProjectManager::SetExecutablePath(const wxString& path) {
	m_executablePath.Assign(path);
}

void amdProjectManager::SetProjectFileName(const wxFileName& fileName) {
	m_project.amdFile.Assign(fileName);
}

amdMainFrame* amdProjectManager::GetMainFrame() {
	return m_mainFrame;
}

amdElementsNotebook* amdProjectManager::GetElementsNotebook() {
	return m_elements;
}

amdChaptersNotebook* amdProjectManager::GetChaptersNotebook() {
	return m_chaptersNote;
}

amdOutline* amdProjectManager::GetOutline() {
	return m_outline;
}

amdRelease* amdProjectManager::GetRelease() {
	return m_release;
}

wxString amdProjectManager::GetPath(bool withSeparator) {
	if (withSeparator)
		return m_project.amdFile.GetPathWithSep();
	else
		return m_project.amdFile.GetPath();
}

void amdProjectManager::ClearPath() {
	m_project.amdFile.Clear();
}

void amdProjectManager::SetSaved(bool saved) {
	m_isSaved = saved;
}

void amdProjectManager::SetLastSave() {
	// This function is called at every Save and it writes a file named 88165468
		// next to the executable. In the file, there are paths to the project.
		// These paths are used when loading so that the user doesn't need to manually
		// load a project at startup, it automatically loads the last project that was worked on.

	std::ofstream last(GetExecutablePath(true).ToStdString() + "88165468", std::ios::binary | std::ios::out);

	int size = GetFullPath().size() + 1;

	last.write((char*)&size, sizeof(int));
	last.write(GetFullPath().c_str(), size);

	last.close();
}

bool amdProjectManager::GetLastSave() {
	// Nothing special here, just reads the 88165468 file and, if succesful, calls the load function.
	// Else, just clear the paths and load a standard new project.
	std::ifstream last(GetExecutablePath(true).ToStdString() + "88165468", std::ios::binary | std::ios::in);

	if (last.is_open()) {
		int size;
		char* data;

		last.read((char*)&size, sizeof(int));
		data = new char[size];
		last.read(data, size);
		SetProjectFileName(wxFileName(data));
		delete[] data;

		last.close();
	} else
		return false;

	if (wxFileName::Exists(GetFullPath().ToStdString())) {
		return true;
	} else {
		ClearPath();
		return false;
	}
}

void amdProjectManager::AddCharacter(Character& character, int pos) {
	if (pos > -1) {
		auto it = m_characters.begin();
		for (int i = 0; i < pos; i++)
			it++;

		m_characters.insert(it, character);
	} else
		m_characters.push_back(character);

	wxVectorSort(m_characters);
	m_elements->UpdateCharacterList();
	m_elements->SetSearchAC(wxBookCtrlEvent());

	m_mainFrame->GetOutline()->GetOutlineFiles()->AppendCharacter(character);
	SetSaved(false);
}

void amdProjectManager::AddLocation(Location& location, int pos) {
	if (pos > -1) {
		auto it = m_locations.begin();
		for (int i = 0; i < pos; i++)
			it++;

		m_locations.insert(it, location);
	} else
		m_locations.push_back(location);

	wxVectorSort(m_locations);
	m_elements->UpdateLocationList();
	m_elements->SetSearchAC(wxBookCtrlEvent());

	m_mainFrame->GetOutline()->GetOutlineFiles()->AppendLocation(location);
	SetSaved(false);
}

void amdProjectManager::AddItem(Item& item, int pos) {
	if (pos > -1) {
		auto it = m_items.begin();
		for (int i = 0; i < pos; i++)
			it++;

		m_items.insert(it, item);
	} else
		m_items.push_back(item);

	wxVectorSort(m_items);
	m_elements->UpdateItemList();
	m_elements->SetSearchAC(wxBookCtrlEvent());

	m_mainFrame->GetOutline()->GetOutlineFiles()->AppendItem(item);
	SetSaved(false);
}

void amdProjectManager::AddChapter(Chapter& chapter, int pos) {
	size_t capacityBefore = m_chapters.capacity();
	if (pos < m_chapters.size() + 1 && pos > -1) {
		auto it = m_chapters.begin();
		for (int i = 0; i < pos; i++) {
			it++;
		}
		m_chapters.insert(it, chapter);
		
		for (int i = 0; i < m_chapters.size(); i++)
			m_chapters[i].position = i + 1;
	} else {
		m_chapters.push_back(chapter);
	}

	if (m_chapters.capacity() > capacityBefore)
		RedeclareChapsInElements();

	m_chaptersNote->AddChapter(chapter, pos);
	SetSaved(false);
}

void amdProjectManager::EditCharacter(Character& original, Character& edit, bool sort) {
	m_outline->GetOutlineFiles()->DeleteCharacter(original);
	m_outline->GetOutlineFiles()->AppendCharacter(edit);

	if (sort) {
		for (auto& it : original.chapters) {
			it->characters.Remove(original.name);
			it->characters.Add(edit.name);
		}
	}

	original = edit;

	if (sort) {
		wxVectorSort(m_characters);
		m_elements->UpdateCharacterList();
	} else {
		int n = 0;
		for (auto& it : m_characters) {
			if (it == original)
				m_elements->UpdateCharacter(n++, it);
		}
	}

	m_elements->m_charShow->SetData(original);
	m_mainFrame->Enable(true);
	SetSaved(false);
}

void amdProjectManager::EditLocation(Location& original, Location& edit, bool sort) {
	m_outline->GetOutlineFiles()->DeleteLocation(original);
	m_outline->GetOutlineFiles()->AppendLocation(edit);
	if (sort) {
		for (auto& it : original.chapters) {
			it->locations.Remove(original.name);
			it->locations.Add(edit.name);
		}
	}

	original = edit;

	if (sort) {
		wxVectorSort(m_locations);
		m_elements->UpdateLocationList();
	} else {
		int n = 0;
		for (auto& it : m_locations) {
			if (it == original)
				m_elements->UpdateLocation(n++, it);
		}
		m_elements->m_locShow->SetData(original);
		m_mainFrame->Enable(true);
		SetSaved(false);
	}
}

void amdProjectManager::EditItem(Item& original, Item& edit, bool sort) {
	m_outline->GetOutlineFiles()->DeleteItem(original);
	m_outline->GetOutlineFiles()->AppendItem(edit);

	if (sort) {
		for (auto& it : original.chapters) {
			it->items.Remove(original.name);
			it->items.Add(edit.name);
		}
	}

	original = edit;

	if (sort) {
		wxVectorSort(m_items);
		m_elements->UpdateItemList();
	} else {
		int n = 0;
		for (auto& it : m_items) {
			if (it == original)
				m_elements->UpdateItem(n++, it);
		}
		m_elements->m_itemShow->SetData(original);
		m_mainFrame->Enable(true);
		SetSaved(false);
	}
}

void amdProjectManager::AddChapterToCharacter(const wxString& characterName, Chapter& chapter) {
	for (auto& it : m_characters) {
		if (characterName == it.name) {

			bool has = false;
			for (int i = 0; i < it.chapters.size(); i++)
				if (it.chapters[i] == &chapter)
					has = true;
			
			if (!has)
				it.chapters.push_back(&chapter);

			if (chapter.characters.Index(characterName) == -1)
				chapter.characters.Add(characterName);
		}
	}
}

void amdProjectManager::AddChapterToLocation(const wxString& locationName, Chapter& chapter) {
	for (auto& it : m_locations) {
		if (locationName == it.name) {

			bool has = false;
			for (int i = 0; i < it.chapters.size(); i++)
				if (it.chapters[i] == &chapter)
					has = true;

			if (!has)
				it.chapters.push_back(&chapter);

			if (chapter.locations.Index(locationName) == -1)
				chapter.locations.Add(locationName);
		}
	}
}

void amdProjectManager::AddChapterToItem(const wxString& itemName, Chapter& chapter) {
	for (auto& it : m_items) {
		if (itemName == it.name) {

			bool has = false;
			for (int i = 0; i < it.chapters.size(); i++)
				if (it.chapters[i] == &chapter)
					has = true;

			if (!has)
				it.chapters.push_back(&chapter);

			if (chapter.items.Index(itemName) == -1)
				chapter.items.Add(itemName);
		}
	}
}

void amdProjectManager::RemoveChapterFromCharacter(const wxString& characterName, Chapter& chapter) {
	for (auto& it : m_characters) {
		if (characterName == it.name) {
			for (auto& it2 : it.chapters)
				if (it2 == &chapter)
					it.chapters.erase(&it2);

			chapter.characters.Remove(characterName);
			return;
		}
	}

	wxLogMessage("Could not remove character '%s' from chapter '%s'", characterName, chapter.name);
}

void amdProjectManager::RemoveChapterFromLocation(const wxString& locationName, Chapter& chapter) {
	for (auto& it : m_locations) {
		if (locationName == it.name) {
			for (auto& it2 : it.chapters)
				if (it2 == &chapter)
					it.chapters.erase(&it2);

			chapter.locations.Remove(locationName);
			return;
		}
	}

	wxLogMessage("Could not remove location '%s' from chapter '%s'", locationName, chapter.name);
}

void amdProjectManager::RemoveChapterFromItem(const wxString& itemName, Chapter& chapter) {
	for (auto& it : m_items) {
		if (itemName == it.name) {
			for (auto& it2 : it.chapters)
				if (it2 == &chapter)
					it.chapters.erase(&it2);

			chapter.items.Remove(itemName);
			return;
		}
	}

	wxLogMessage("Could not remove item '%s' from chapter '%s'", itemName, chapter.name);
}

void amdProjectManager::RedeclareChapsInElements() {
	for (auto& it : m_characters)
		it.chapters.clear();
	
	for (auto& it : m_locations)
		it.chapters.clear();

	for (auto& it : m_items)
		it.chapters.clear();

	for (auto& it : m_chapters) {
		for (auto& it2 : it.characters)
			AddChapterToCharacter(it2, it);

		for (auto& it2 : it.locations)
			AddChapterToLocation(it2, it);

		for (auto& it2 : it.items)
			AddChapterToItem(it2, it);
	}
}

void amdProjectManager::DeleteCharacter(Character& character) {
	for (auto it : character.chapters)
		it->characters.Remove(character.name);

	m_characters.erase(&character);
	SetSaved(false);
}

void amdProjectManager::DeleteLocation(Location& location) {
	for (auto it : location.chapters)
		it->characters.Remove(location.name);

	m_locations.erase(&location);
	SetSaved(false);
}

void amdProjectManager::DeleteItem(Item& item) {
	for (auto it : item.chapters)
		it->characters.Remove(item.name);

	m_items.erase(&item);
	SetSaved(false);
}

void amdProjectManager::DeleteChapter(Chapter& chapter) {
	for (auto& it : chapter.characters) {
		for (auto& it2 : m_characters) {
			
			if (it == it2.name) {
			
				for (auto& it3 : it2.chapters) {
					if (it3 == &chapter)
						it2.chapters.erase(&it3);
				
				}
			}
		}
	}

	for (auto& it : chapter.locations) {
		for (auto& it2 : m_locations) {

			if (it == it2.name) {

				for (auto& it3 : it2.chapters) {
					if (it3 == &chapter)
						it2.chapters.erase(&it3);
			
				}
			}
		}
	}

	m_chapters.erase(&chapter);
	SetSaved(false);
}

wxVector<Character> amdProjectManager::GetCharacters(int bookPos) {
	return m_project.GetCharacters(bookPos);
}

wxVector<Location>& amdProjectManager::GetLocations(int bookPos) {
	return m_project.GetLocations(bookPos);
}

wxVector<Item>& amdProjectManager::GetItems(int bookPos) {
	return m_project.GetItems(bookPos);
}

wxVector<Chapter>& amdProjectManager::GetChapters(int bookPos) {
	return m_project.GetChapters(bookPos);
}

wxArrayString amdProjectManager::GetCharacterNames() {
	wxArrayString names(true);
	for (auto& it : m_characters)
		names.Add(it.name);

	return names;
}

wxArrayString amdProjectManager::GetLocationNames() {
	wxArrayString names(true);
	for (auto& it : m_locations)
		names.Add(it.name);

	return names;
}

wxArrayString amdProjectManager::GetItemNames() {
	wxArrayString names(true);
	for (auto& it : m_items)
		names.Add(it.name);

	return names;
}

wxArrayString amdProjectManager::GetBookTitles() {
	wxArrayString names;
	for (auto& it : m_project.books)
		names.Add(it.title);

	return names;
}