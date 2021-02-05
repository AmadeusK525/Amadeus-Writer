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
#include <wx\mstream.h>
#include <wx\utils.h>

///////////////////////////////////////////////////////////////////
//////////////////////////// SQLStorage ///////////////////////////
///////////////////////////////////////////////////////////////////


amProjectSQLDatabase::amProjectSQLDatabase(wxFileName& path) {
	EnableForeignKeySupport(true);
	if (path.IsOk() && !path.IsDir()) {
		Open(path.GetFullPath());
	}
}

bool amProjectSQLDatabase::Init() {
	if (!TableExists("characters")) {
		CreateAllTables();
		return false;
	}

	return true;
}

void amProjectSQLDatabase::CreateAllTables() {
	wxArrayString tCharacters;
	tCharacters.Add("id INTEGER PRIMARY KEY");
	tCharacters.Add("name TEXT UNIQUE NOT NULL");
	tCharacters.Add("age TEXT");
	tCharacters.Add("sex TEXT NOT NULL");
	tCharacters.Add("nationality TEXT");
	tCharacters.Add("height TEXT");
	tCharacters.Add("nickname TEXT");
	tCharacters.Add("appearance TEXT");
	tCharacters.Add("personality TEXT");
	tCharacters.Add("backstory TEXT");
	tCharacters.Add("role INTEGER");
	tCharacters.Add("image BLOB");

	wxArrayString tCharactersCustom;
	tCharactersCustom.Add("id INTEGER PRIMARY KEY");
	tCharactersCustom.Add("name TEXT");
	tCharactersCustom.Add("content TEXT");
	tCharactersCustom.Add("character_id INTEGER");
	tCharactersCustom.Add("FOREIGN KEY(character_id) REFERENCES characters(id)");

	wxArrayString tLocations;
	tLocations.Add("id INTEGER PRIMARY KEY");
	tLocations.Add("name TEXT UNIQUE NOT NULL");
	tLocations.Add("age INTEGER");
	tLocations.Add("general TEXT");
	tLocations.Add("natural TEXT");
	tLocations.Add("architecture TEXT");
	tLocations.Add("politics TEXT");
	tLocations.Add("economy TEXT");
	tLocations.Add("culture TEXT");
	tLocations.Add("role INTEGER");
	tLocations.Add("image BLOB");

	wxArrayString tItems; 
	tItems.Add("id INTEGER PRIMARY KEY");
	tItems.Add("name TEXT UNIQUE NOT NULL");
	tItems.Add("general TEXT");
	tItems.Add("origin TEXT");
	tItems.Add("backstory TEXT");
	tItems.Add("appearance TEXT");
	tItems.Add("usage TEXT");
	tItems.Add("width TEXT");
	tItems.Add("height TEXT");
	tItems.Add("depth TEXT");
	tItems.Add("isMagic INTEGER");
	tItems.Add("isManMade INTEGER");
	tItems.Add("image BLOB");

	wxArrayString tBooks;
	tBooks.Add("id INTEGER PRIMARY KEY");
	tBooks.Add("name TEXT NOT NULL");
	tBooks.Add("author TEXT");
	tBooks.Add("genre TEXT");
	tBooks.Add("description TEXT");
	tBooks.Add("synopsys TEXT");

	wxArrayString tSections;
	tSections.Add("id INTEGER PRIMARY KEY");
	tSections.Add("name TEXT");
	tSections.Add("description TEXT");
	tSections.Add("position INTEGER");
	tSections.Add("type INTEGER");
	tSections.Add("book_id INTEGER");
	tSections.Add("FOREIGN KEY(book_id) REFERENCES books(id)");

	wxArrayString tChapters;
	tChapters.Add("id INTEGER PRIMARY KEY");
	tChapters.Add("name TEXT NOT NULL");
	tChapters.Add("synopsys TEXT");
	tChapters.Add("position INTEGER");
	tChapters.Add("section_id INTEGER");
	tChapters.Add("FOREIGN KEY(section_id) REFERENCES sections(id)");

	wxArrayString tScenes;
	tScenes.Add("id INTEGER PRIMARY KEY");
	tScenes.Add("name TEXT");
	tScenes.Add("content TEXT");
	tScenes.Add("chapter_id INTEGER NOT NULL");
	tScenes.Add("character_id INTEGER");
	tScenes.Add("FOREIGN KEY(chapter_id) REFERENCES chapters(id)");
	tScenes.Add("FOREIGN KEY (character_id) REFERENCES characters(id)");

	//////////////////// MANY-TO-MANY TABLES ////////////////////

	wxArrayString tCharactersInChapters;
	tCharactersInChapters.Add("character_id INTEGER");
	tCharactersInChapters.Add("chapter_id INTEGER");
	tCharactersInChapters.Add("FOREIGN KEY(character_id) REFERENCES characters(id)");
	tCharactersInChapters.Add("FOREIGN KEY(chapter_id) REFERENCES chapters(id)");

	wxArrayString tLocationsInChapters;
	tLocationsInChapters.Add("location_id INTEGER");
	tLocationsInChapters.Add("chapter_id INTEGER");
	tLocationsInChapters.Add("FOREIGN KEY(location_id) REFERENCES location(id)");
	tLocationsInChapters.Add("FOREIGN KEY(chapter_id) REFERENCES chapters(id)");

	wxArrayString tItemsInChapters;
	tItemsInChapters.Add("item_id INTEGER");
	tItemsInChapters.Add("chapter_id INTEGER");
	tItemsInChapters.Add("FOREIGN KEY(item_id) REFERENCES item(id)");
	tItemsInChapters.Add("FOREIGN KEY(chapter_id) REFERENCES chapters(id)");

	CreateTable("characters", tCharacters);
	CreateTable("characters_custom", tCharactersCustom);
	CreateTable("locations", tLocations);
	CreateTable("items", tItems);
	CreateTable("books", tBooks);
	CreateTable("sections", tSections);
	CreateTable("chapters", tChapters);
	CreateTable("characters_chapters", tCharactersInChapters);
	CreateTable("locations_chaptes", tLocationsInChapters);
	CreateTable("items_chapters", tItemsInChapters);
}

int amProjectSQLDatabase::GetDocumentId(amDocument& document) {
	wxString query("SELECT DISTINCT id FROM ");
	query << document.tableName;
	query << " WHERE name = '" << document.name << "'";

	wxSQLite3ResultSet result = ExecuteQuery(query);

	int id = -1;
	int count = 0;

	while (result.NextRow()) {
		id = result.GetInt("id");
		count++;
	}

	if (count > 1)
		throw ("There was an error.");

	return id;
}

bool amProjectSQLDatabase::CreateTable(const wxString& tableName, const wxArrayString& arguments,
	bool ifNotExists) {
	if (arguments.IsEmpty()) {
		wxMessageBox("You can't create a table with no arguments!");
		return false;
	}

	wxString update("CREATE TABLE ");
	if (ifNotExists)
		update << "IF NOT EXISTS ";

	update << tableName;
	update << " (";

	bool first = true;

	for (auto& it : arguments) {
		if (!first)
			update << ", ";

		update << it;
		first = false;
	}

	update << ");";
	ExecuteUpdate(update);

	return true;
}

bool amProjectSQLDatabase::InsertDocument(amDocument& document) {
	wxSQLite3Statement statement = ConstructInsertStatement(document);
	
	statement.ExecuteUpdate();

	for (auto& it : document.documents) {
		if (it.needsForeign)
			it.foreignKey.second = GetDocumentId(document);

		InsertDocument(it);
	}

	return true;
}

bool amProjectSQLDatabase::UpdateDocument(const wxString& tableName, int id, amDocument& document) {

	return false;
}

wxSQLite3Statement amProjectSQLDatabase::ConstructInsertStatement(amDocument& document) {
	wxString insert("INSERT INTO ");
	insert << document.tableName << " (";

	wxString columnNames;
	wxString valueNames;

	bool first = true;

	if (document.name != "") {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		}

		columnNames << "name";
		valueNames << "'" << document.name << "'";

		first = false;
	}

	for (auto& it : document.integers) {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		}

		columnNames << it.first;
		valueNames << it.second;

		first = false;
	}

	for (auto& it : document.strings) {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		}

		columnNames << it.first;
		valueNames << "'" << it.second << "'";

		first = false;
	}

	for (auto& it : document.memBuffers) {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		}

		columnNames << it.first;
		valueNames << "?";

		first = false;
	}

	if (document.needsForeign) {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		}

		columnNames << document.foreignKey.first;
		valueNames << document.foreignKey.second;
	}

	insert << columnNames << ") VALUES (" << valueNames << ");";

	try {
		wxSQLite3Statement statement = PrepareStatement(insert);

		int i = 1;

		for (auto& it : document.memBuffers)
			statement.Bind(i++, it.second);

		return statement;
	} catch (wxSQLite3Exception& e){
		wxMessageBox(e.GetMessage());
	}

	return wxSQLite3Statement();
}


///////////////////////////////////////////////////////////////////////
//////////////////////////// ProjectManager ///////////////////////////
///////////////////////////////////////////////////////////////////////


amProjectManager::amProjectManager() {

}

bool amProjectManager::Init() {
	if (m_isInitialized)
		return false;

	if (m_project.amFile.IsOk()) {
		if (!m_mainFrame) {
			m_mainFrame = new amMainFrame("New Amadeus project - " + m_project.amFile.GetName(),
				this, wxDefaultPosition, wxDefaultSize);

			m_elements = (m_mainFrame->GetElementsNotebook());
			m_chaptersNote = (m_mainFrame->GetChaptersNotebook());
			m_outline = (m_mainFrame->GetOutline());
			m_release = (m_mainFrame->GetRelease());
		}

		wxSQLite3Database::InitializeSQLite();
		m_storage.Open(m_project.amFile.GetFullPath());

		if (!m_storage.Init()) {
			Book book;
			book.title = m_project.amFile.GetName();

			m_project.books.push_back(book);
		} else {
			LoadProject();
		}

		m_isInitialized = true;

		return true;
	}

	return false;
}

bool amProjectManager::SaveProject() {
	return DoSaveProject(m_project.amFile.GetFullPath());
}

bool amProjectManager::LoadProject() {
	return DoLoadProject(m_project.amFile.GetFullPath());
}

bool amProjectManager::DoSaveProject(const wxString& path) {
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

	m_mainFrame->SetTitle(m_project.amFile.GetName() + " - Amadeus Writer");
	m_mainFrame->SetFocus();
	SetLastSave();

	return true;
}

bool amProjectManager::DoLoadProject(const wxString& path) {
	// This is the load function. It works by getting the amount of characters
	// (written at the beginning of the "saveAs" function), then calling the load
	// character function that amount of times. It does that for locations, chapters, notes and the corkboard as well.

	SetProjectFileName(path);

	m_mainFrame->OnNewFile(wxCommandEvent());

	m_isSaved = true;
	SetLastSave();
	return true;
}

void amProjectManager::SetExecutablePath(const wxString& path) {
	m_executablePath.Assign(path);
}

void amProjectManager::SetProjectFileName(const wxFileName& fileName) {
	m_project.amFile.Assign(fileName);
}

amMainFrame* amProjectManager::GetMainFrame() {
	return m_mainFrame;
}

amElementsNotebook* amProjectManager::GetElementsNotebook() {
	return m_elements;
}

amChaptersNotebook* amProjectManager::GetChaptersNotebook() {
	return m_chaptersNote;
}

amOutline* amProjectManager::GetOutline() {
	return m_outline;
}

amRelease* amProjectManager::GetRelease() {
	return m_release;
}

wxString amProjectManager::GetPath(bool withSeparator) {
	if (withSeparator)
		return m_project.amFile.GetPathWithSep();
	else
		return m_project.amFile.GetPath();
}

void amProjectManager::ClearPath() {
	m_project.amFile.Clear();
}

void amProjectManager::SetSaved(bool saved) {
	m_isSaved = saved;
}

void amProjectManager::SetLastSave() {
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

bool amProjectManager::GetLastSave() {
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

void amProjectManager::AddCharacter(Character& character, int book) {
	if (book == -1) {
		m_project.gCharacters.push_back(character);
		wxVectorSort(m_project.gCharacters);
	} else {
		m_project.books[book].characters.push_back(character);
		wxVectorSort(m_project.books[book].characters);
	}

	m_elements->UpdateCharacterList();
	m_elements->SetSearchAC(wxBookCtrlEvent());

	m_outline->GetOutlineFiles()->AppendCharacter(character);

	m_storage.InsertDocument(character.GenerateDocument());
	SetSaved(false);
}

void amProjectManager::AddLocation(Location& location, int book) {
	if (book == -1) {
		m_project.gLocations.push_back(location);
		wxVectorSort(m_project.gLocations);
	} else {
		m_project.books[book].locations.push_back(location);
		wxVectorSort(m_project.books[book].locations);
	}

	m_elements->UpdateLocationList();
	m_elements->SetSearchAC(wxBookCtrlEvent());

	m_outline->GetOutlineFiles()->AppendLocation(location);
	SetSaved(false);
}

void amProjectManager::AddItem(Item& item, int book) {
	if (book == -1) {
		m_project.gItems.push_back(item);
		wxVectorSort(m_project.gItems);
	} else {
		m_project.books[book].items.push_back(item);
		wxVectorSort(m_project.books[book].items);
	}

	m_elements->UpdateItemList();
	m_elements->SetSearchAC(wxBookCtrlEvent());

	m_outline->GetOutlineFiles()->AppendItem(item);
	SetSaved(false);
}

void amProjectManager::AddChapter(Chapter& chapter, int book, int pos) {
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

void amProjectManager::EditCharacter(Character& original, Character& edit, bool sort) {
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

void amProjectManager::EditLocation(Location& original, Location& edit, bool sort) {
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

void amProjectManager::EditItem(Item& original, Item& edit, bool sort) {
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

void amProjectManager::AddChapterToCharacter(const wxString& characterName, Chapter& chapter) {
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

void amProjectManager::AddChapterToLocation(const wxString& locationName, Chapter& chapter) {
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

void amProjectManager::AddChapterToItem(const wxString& itemName, Chapter& chapter) {
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

void amProjectManager::RemoveChapterFromCharacter(const wxString& characterName, Chapter& chapter) {
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

void amProjectManager::RemoveChapterFromLocation(const wxString& locationName, Chapter& chapter) {
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

void amProjectManager::RemoveChapterFromItem(const wxString& itemName, Chapter& chapter) {
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

void amProjectManager::RedeclareChapsInElements() {
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

void amProjectManager::DeleteCharacter(Character& character) {
	for (auto it : character.chapters)
		it->characters.Remove(character.name);

	m_characters.erase(&character);
	SetSaved(false);
}

void amProjectManager::DeleteLocation(Location& location) {
	for (auto it : location.chapters)
		it->characters.Remove(location.name);

	m_locations.erase(&location);
	SetSaved(false);
}

void amProjectManager::DeleteItem(Item& item) {
	for (auto it : item.chapters)
		it->characters.Remove(item.name);

	m_items.erase(&item);
	SetSaved(false);
}

void amProjectManager::DeleteChapter(Chapter& chapter) {
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

wxVector<Character> amProjectManager::GetCharacters(int bookPos) {
	return m_project.GetCharacters(bookPos);
}

wxVector<Location>& amProjectManager::GetLocations(int bookPos) {
	return m_project.GetLocations(bookPos);
}

wxVector<Item>& amProjectManager::GetItems(int bookPos) {
	return m_project.GetItems(bookPos);
}

wxVector<Chapter>& amProjectManager::GetChapters(int bookPos) {
	return m_project.GetChapters(bookPos);
}

wxArrayString amProjectManager::GetCharacterNames() {
	wxArrayString names(true);
	for (auto& it : m_characters)
		names.Add(it.name);

	return names;
}

wxArrayString amProjectManager::GetLocationNames() {
	wxArrayString names(true);
	for (auto& it : m_locations)
		names.Add(it.name);

	return names;
}

wxArrayString amProjectManager::GetItemNames() {
	wxArrayString names(true);
	for (auto& it : m_items)
		names.Add(it.name);

	return names;
}

wxArrayString amProjectManager::GetBookTitles() {
	wxArrayString names;
	for (auto& it : m_project.books)
		names.Add(it.title);

	return names;
}