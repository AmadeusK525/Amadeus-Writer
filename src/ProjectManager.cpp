#include "ProjectManager.h"

#include "ProjectWizard.h"
#include "MainFrame.h"
#include "ElementsNotebook.h"
#include "StoryNotebook.h"
#include "Outline.h"
#include "Release.h"
#include "OutlineFiles.h"
#include "ElementShowcases.h"

#include <wx\progdlg.h>
#include <wx\mstream.h>
#include <wx\utils.h>
#include <fstream>

#include <thread>


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
	tLocations.Add("general TEXT");
	tLocations.Add("natural TEXT");
	tLocations.Add("architecture TEXT");
	tLocations.Add("politics TEXT");
	tLocations.Add("economy TEXT");
	tLocations.Add("culture TEXT");
	tLocations.Add("role INTEGER");
	tLocations.Add("image BLOB");

	wxArrayString tLocationsCustom;
	tLocationsCustom.Add("id INTEGER PRIMARY KEY");
	tLocationsCustom.Add("name TEXT");
	tLocationsCustom.Add("content TEXT");
	tLocationsCustom.Add("location_id INTEGER");
	tLocationsCustom.Add("FOREIGN KEY(location_id) REFERENCES locations(id)");

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
	tItems.Add("role INTEGER");
	tItems.Add("image BLOB");

	wxArrayString tItemsCustom;
	tItemsCustom.Add("id INTEGER PRIMARY KEY");
	tItemsCustom.Add("name TEXT");
	tItemsCustom.Add("content TEXT");
	tItemsCustom.Add("item_id INTEGER");
	tItemsCustom.Add("FOREIGN KEY(item_id) REFERENCES items(id)");

	wxArrayString tBooks;
	tBooks.Add("id INTEGER PRIMARY KEY");
	tBooks.Add("name TEXT NOT NULL");
	tBooks.Add("position INTEGER UNIQUE NOT NULL");
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

	wxArrayString tChapterNotes;
	tChapterNotes.Add("id INTEGER PRIMARY KEY");
	tChapterNotes.Add("name TEXT");
	tChapterNotes.Add("content TEXT");
	tChapterNotes.Add("isDone INTEGER");
	tChapterNotes.Add("chapter_id INTEGER");
	tChapterNotes.Add("FOREIGN KEY (chapter_id) REFERENCES chapters(id)");

	wxArrayString tScenes;
	tScenes.Add("id INTEGER PRIMARY KEY");
	tScenes.Add("name TEXT");
	tScenes.Add("content TEXT");
	tScenes.Add("position INTEGER");
	tScenes.Add("chapter_id INTEGER NOT NULL");
	tScenes.Add("character_id INTEGER");
	tScenes.Add("FOREIGN KEY(chapter_id) REFERENCES chapters(id)");
	tScenes.Add("FOREIGN KEY (character_id) REFERENCES characters(id)");

	wxArrayString tOutlineCorkboards;
	tOutlineCorkboards.Add("id INTEGER PRIMARY KEY");
	tOutlineCorkboards.Add("name TEXT");
	tOutlineCorkboards.Add("content TEXT");

	wxArrayString tOutlineTimelines;
	tOutlineTimelines.Add("id INTEGER PRIMARY KEY");
	tOutlineTimelines.Add("name TEXT");
	tOutlineTimelines.Add("content TEXT");
	tOutlineTimelines.Add("threads_xml");

	wxArrayString tOutlineFiles;
	tOutlineFiles.Add("id INTEGER PRIMARY KEY");
	tOutlineFiles.Add("name TEXT");
	tOutlineFiles.Add("content TEXT");

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

	Begin();

	CreateTable("characters", tCharacters);
	CreateTable("characters_custom", tCharactersCustom);

	CreateTable("locations", tLocations);
	CreateTable("locations_custom", tLocationsCustom);

	CreateTable("items", tItems);
	CreateTable("items_custom", tItemsCustom);

	CreateTable("books", tBooks);
	CreateTable("sections", tSections);
	CreateTable("chapters", tChapters);
	CreateTable("chapter_notes", tChapterNotes);
	CreateTable("scenes", tScenes);

	CreateTable("outline_corkboards", tOutlineCorkboards);
	CreateTable("outline_timelines", tOutlineTimelines);
	CreateTable("outline_files", tOutlineFiles);

	CreateTable("characters_chapters", tCharactersInChapters);
	CreateTable("locations_chapters", tLocationsInChapters);
	CreateTable("items_chapters", tItemsInChapters);

	Commit();
}

int amProjectSQLDatabase::GetDocumentId(amDocument& document) {
	wxString query("SELECT DISTINCT id FROM ");
	query << document.tableName;
	query << " WHERE ";

	bool nameEmpty = document.name == "";

	if (!nameEmpty)
		query << "name = '" << document.name << "'";

	if (document.specialForeign) {
		if (!nameEmpty)
			query << " AND ";

		query << document.foreignKey.first << " = " << document.foreignKey.second;
	}

	if (!document.integers.empty()) {
		if (!nameEmpty || document.specialForeign)
			query << " AND ";

		pair<wxString, int>* it = document.integers.begin();
		query << it->first << " = " << it->second << ";";
	}

	wxSQLite3ResultSet result = ExecuteQuery(query);

	int id = -1;

	// I return the first row with found id, this is the best solution I found
	// without throwing an exception. Maybe one day I'll try and find another one.
	while (result.NextRow()) {
		id = result.GetInt("id");
		break;
	}

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

	for (auto& str : arguments) {
		if (!first)
			update << ", ";

		update << str;
		first = false;
	}

	update << ");";
	ExecuteUpdate(update);

	return true;
}

bool amProjectSQLDatabase::InsertDocument(amDocument& document) {
	wxSQLite3Statement statement = ConstructInsertStatement(document);

	statement.ExecuteUpdate();

	for (amDocument& documentIt : document.documents) {
		if (documentIt.specialForeign)
			documentIt.foreignKey.second = GetDocumentId(document);

		InsertDocument(documentIt);
	}
	return true;
}

bool amProjectSQLDatabase::UpdateDocument(amDocument& original, amDocument& edit) {
	int id = GetDocumentId(original);

	try {
		wxSQLite3Statement statement = ConstructUpdateStatement(edit, id);
		statement.ExecuteUpdate();
	} catch (wxSQLite3Exception& e) {
		wxMessageBox("Exception thrown - " + e.GetMessage());
	}

	int prevSize = original.documents.size();
	int newSize = edit.documents.size();

	if (prevSize == 0 && newSize == 0)
		return true;

	if (prevSize < newSize) {
		int i = 0;

		for (i; i < prevSize; i++) {
			original.documents[i].foreignKey.second = id;
			edit.documents[i].foreignKey.second = id;
			UpdateDocument(original.documents[i], edit.documents[i]);
		}

		for (i; i < newSize; i++) {
			edit.documents[i].foreignKey.second = id;
			InsertDocument(edit.documents[i]);
		}

	} else {
		if (prevSize > newSize) {
			for (int i = prevSize - 1; i > newSize - 1; i--) {
				original.documents[i].foreignKey.second = id;
				DeleteDocument(original.documents[i]);
			}
		}

		for (int i = 0; i < newSize; i++) {
			original.documents[i].foreignKey.second = id;
			edit.documents[i].foreignKey.second = id;
			UpdateDocument(original.documents[i], edit.documents[i]);
		}
	}
	return true;
}

bool amProjectSQLDatabase::InsertManyToMany(const wxString& tableName,
	int docID1, const wxString& arg1,
	int docID2, const wxString& arg2) {
	
	wxString insert("INSERT INTO ");
	insert << tableName << " (" << arg1 << ", " << arg2 << ") VALUES (";

	insert << docID1 << ", " << docID2 << ");";

	return ExecuteUpdate(insert);
}

bool amProjectSQLDatabase::InsertManyToMany(const wxString& tableName,
	amDocument& doc1, const wxString& arg1,
	amDocument& doc2, const wxString& arg2) {

	return InsertManyToMany(tableName, GetDocumentId(doc1), arg1, GetDocumentId(doc2), arg2);
}

bool amProjectSQLDatabase::DeleteDocument(amDocument& document) {
	wxString statement("DELETE FROM ");
	statement << document.tableName << " WHERE id = " << GetDocumentId(document) << ";";

	return ExecuteUpdate(statement);
}

bool amProjectSQLDatabase::DeleteManyToMany(const wxString& tableName,
	int docID1, const wxString& arg1,
	int docID2, const wxString& arg2) {

	wxString statement("DELETE FROM ");
	statement << tableName << " WHERE " << arg1 << " = " << docID1;
	statement << " AND " << arg2 << " = " << docID2 << ";";

	return ExecuteUpdate(statement);
}

bool amProjectSQLDatabase::DeleteManyToMany(const wxString& tableName,
	amDocument& doc1, const wxString& arg1,
	amDocument& doc2, const wxString& arg2) {

	return DeleteManyToMany(tableName, GetDocumentId(doc1), arg1, GetDocumentId(doc2), arg2);
}

wxSQLite3Statement amProjectSQLDatabase::ConstructInsertStatement(amDocument& document) {
	wxString insert("INSERT INTO ");
	insert << document.tableName << " (";

	wxString columnNames;
	wxString valueNames;

	bool first = true;

	wxSQLite3StatementBuffer buffer;

	if (document.name != "") {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		} else
			first = false;

		columnNames << "name";
		valueNames << "'" << buffer.Format("%q", (const char*)document.name) << "'";
	}

	for (pair<wxString, int>& it : document.integers) {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		} else
			first = false;

		columnNames << it.first;
		valueNames << it.second;
	}

	for (pair<wxString, wxString>& it : document.strings) {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		} else
			first = false;

		columnNames << it.first;
		valueNames << "'" << buffer.Format("%q", (const char*)it.second) << "'";
	}

	for (pair<wxString, wxMemoryBuffer>& it : document.memBuffers) {
		if (!first) {
			columnNames << ", ";
			valueNames << ", ";
		} else
			first = false;

		columnNames << it.first;
		valueNames << "?";
	}

	if (document.specialForeign) {
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

		for (pair<wxString, wxMemoryBuffer>& it : document.memBuffers)
			statement.Bind(i++, it.second);

		return statement;
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}

	return wxSQLite3Statement();
}

wxSQLite3Statement amProjectSQLDatabase::ConstructUpdateStatement(amDocument& document, int id) {
	wxString update("UPDATE ");
	update << document.tableName << " SET ";

	bool first = true;
	wxSQLite3StatementBuffer buffer;
	if (document.name != "") {
		if (!first)
			update << ", ";

		update << "name = '" << buffer.Format("%q", (const char*)document.name) << "'";
		first = false;
	}

	for (pair<wxString, int>& it : document.integers) {
		if (!first)
			update << ", ";

		update << it.first << " = " << it.second;
		first = false;
	}

	for (pair<wxString, wxString>& it : document.strings) {
		if (!first)
			update << ", ";

		update << it.first << " = '" << buffer.Format("%q", (const char*)it.second) << "'";
		first = false;
	}

	for (pair<wxString, wxMemoryBuffer>& it : document.memBuffers) {
		if (!first)
			update << ", ";

		update << it.first << " = ?";
		first = false;
	}

	if (document.specialForeign) {
		if (!first)
			update << ", ";

		update << document.foreignKey.first << " = " << document.foreignKey.second;
		first = false;
	}

	update << " WHERE id = " << id << ";";

	try {
		wxSQLite3Statement statement = PrepareStatement(update);

		int i = 1;

		for (pair<wxString, wxMemoryBuffer>& it : document.memBuffers)
			statement.Bind(i++, it.second);

		return statement;
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}

	return wxSQLite3Statement();

}

bool amProjectSQLDatabase::RowExists(amDocument& document) {
	return GetDocumentId(document) != -1;
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
			m_mainFrame = new amMainFrame("Amadeus Writer - " + m_project.amFile.GetFullName(),
				this, wxDefaultPosition, wxDefaultSize);

			m_elements = (m_mainFrame->GetElementsNotebook());
			m_storyNotebook = (m_mainFrame->GetStoryNotebook());
			m_outline = (m_mainFrame->GetOutline());
			m_release = (m_mainFrame->GetRelease());
		}

		wxSQLite3Database::InitializeSQLite();
		m_storage.Open(m_project.amFile.GetFullPath());

		if (!m_storage.Init()) {
			Book book;
			book.title = m_project.amFile.GetName();

			book.Save(&m_storage);
			m_project.books.push_back(book);
		} else {
			LoadProject();
		}

		m_elements->InitShowChoices();
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

void amProjectManager::SaveDocument(amDocument& original, amDocument& edit) {
	if (m_storage.RowExists(original))
		m_storage.UpdateDocument(original, edit);
	else
		m_storage.InsertDocument(edit);
}

bool amProjectManager::DoSaveProject(const wxString& path) {
	// Sets Save state as saved, updates the current title and all that good stuff.
	// The attribute "saveDialog" currently does nothing.
	m_mainFrame->SetTitle("Amadeus Writer - " + m_project.amFile.GetFullName());
	m_mainFrame->SetFocus();
	SetLastSave();

	return true;
}

bool amProjectManager::DoLoadProject(const wxString& path) {
	try {
		SetProjectFileName(path);
		m_mainFrame->OnNewFile(wxCommandEvent());

		m_storage.Begin();

		LoadCharacters();
		LoadLocations();
		LoadItems();

		LoadBooks();

		wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT content FROM outline_corkboards");
		wxString str1, str2, str3;

		while (result.NextRow())
			str1 = result.GetAsString("content");

		result = m_storage.ExecuteQuery("SELECT content FROM outline_timelines");

		while (result.NextRow())
			str2 = result.GetAsString("content");

		result = m_storage.ExecuteQuery("SELECT content FROM outline_files");

		while (result.NextRow())
			str3 = result.GetAsString("content");

		wxStringInputStream corkboard(str1), timeline(str2), files(str3);

		m_storage.Commit();

		m_elements->UpdateAll();
		m_outline->LoadOutline(corkboard, timeline, files);

		m_mainFrame->SetTitle("Amadeus Writer - " + m_project.amFile.GetFullName());
		SetLastSave();
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}

	return true;
}

void amProjectManager::LoadBooks() {
	wxSQLite3Table table = m_storage.GetTable("SELECT * FROM books");
	int count = table.GetRowCount();
	m_project.books.reserve(count);

	for (int i = 0; i < count; i++) {
		table.SetRow(i);
		int id = table.GetInt("id");

		Book book;

		book.title = table.GetAsString("name");
		book.pos = table.GetInt("position");
		book.author = table.GetAsString("author");
		book.genre = table.GetAsString("genre");
		book.description = table.GetAsString("description");
		book.synopsys = table.GetAsString("synopsys");

		book.SetId(id);

		LoadSections(book.sections, id);

		m_project.books.push_back(book);
	}

	RedeclareChapsInElements(m_project.books[0]);
}

void amProjectManager::LoadSections(wxVector<Section>& sections, int bookId) {
	wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT * FROM sections WHERE book_id = " +
		std::to_string(bookId) + " ORDER BY position ASC;");
	int i = 0;

	while (result.NextRow()) {
		int id = result.GetInt("id");

		Section section(bookId, result.GetInt("position"));

		section.name = result.GetAsString("name");
		section.description = result.GetAsString("description");
		section.type = (SectionType)result.GetInt("type");
		section.SetId(id);

		LoadChapters(section.chapters, id);

		sections.push_back(section);
	}
}

void amProjectManager::LoadChapters(wxVector<Chapter>& chapters, int sectionId) {
	wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT * FROM chapters WHERE section_id = " +
		std::to_string(sectionId) + " ORDER BY position ASC;");

	while (result.NextRow()) {
		int chapterId = result.GetInt("id");

		Chapter chapter(sectionId, result.GetInt("position"));

		chapter.name = result.GetAsString("name");
		chapter.synopsys = result.GetAsString("synopsys");
		chapter.position = result.GetInt("position");

		chapter.SetId(chapterId);

		wxSQLite3ResultSet results2 = m_storage.ExecuteQuery("SELECT * FROM chapter_notes WHERE chapter_id = " +
			std::to_string(chapterId) + ";");

		while (results2.NextRow()) {
			Note note(results2.GetAsString("content"), results2.GetAsString("name"));
			note.isDone = results2.GetBool("isDone");

			chapter.notes.push_back(note);
		}

		results2 = m_storage.ExecuteQuery("SELECT * FROM characters_chapters WHERE chapter_id = " +
			std::to_string(chapterId) + ";");

		while (results2.NextRow()) {
			wxSQLite3ResultSet character = m_storage.ExecuteQuery("SELECT name FROM characters WHERE id = " +
				std::to_string(results2.GetInt("character_id")));

			chapter.characters.Add(character.GetAsString("name"));
		}

		results2 = m_storage.ExecuteQuery("SELECT * FROM locations_chapters WHERE chapter_id = " +
			std::to_string(chapterId) + ";");

		while (results2.NextRow()) {
			wxSQLite3ResultSet location = m_storage.ExecuteQuery("SELECT name FROM locations WHERE id = " +
				std::to_string(results2.GetInt("location_id")));

			chapter.locations.Add(location.GetAsString("name"));
		}

		results2 = m_storage.ExecuteQuery("SELECT * FROM items_chapters WHERE chapter_id = " +
			std::to_string(chapterId) + ";");

		while (results2.NextRow()) {
			wxSQLite3ResultSet item = m_storage.ExecuteQuery("SELECT name FROM items WHERE id = " +
				std::to_string(results2.GetInt("item_id")));

			chapter.items.Add(item.GetAsString("name"));
		}

		LoadScenes(chapter.scenes, chapterId, false);

		chapters.push_back(chapter);
		m_storyNotebook->AddChapter(chapter);
	}
}

void amProjectManager::LoadScenes(wxVector<Scene>& scenes, int chapterId, bool loadBuffers) {
	wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT * FROM scenes WHERE chapter_id = " +
		std::to_string(chapterId) + " ORDER BY position ASC;");

	while (result.NextRow()) {
		Scene scene(chapterId, result.GetInt("position"));

		scene.name = result.GetAsString("name");

		if (loadBuffers) {
			wxStringInputStream sstream(result.GetAsString("content"));
			scene.content.LoadFile(sstream, wxRICHTEXT_TYPE_XML);
		}

		scene.chapterID = chapterId;
		scene.pos = result.GetInt("position");
		scene.SetId(result.GetInt("id"));

		scenes.push_back(scene);
	}
}

void amProjectManager::LoadCharacters() {
	wxSQLite3Table table = m_storage.GetTable("SELECT * FROM characters;");
	int count = table.GetRowCount();
	m_project.characters.reserve(count);

	for (int i = 0; i < count; i++) {
		table.SetRow(i);
		int id = table.GetInt("id");

		Character character;

		character.name = table.GetAsString("name");
		character.sex = table.GetAsString("sex");
		character.age = table.GetAsString("age");
		character.nat = table.GetAsString("nationality");
		character.height = table.GetAsString("height");
		character.nick = table.GetAsString("nickname");
		character.appearance = table.GetAsString("appearance");
		character.personality = table.GetAsString("personality");
		character.backstory = table.GetAsString("backstory");

		character.role = (Role)table.GetInt("role");
		character.id = id;

		if (!table.IsNull("image")) {
			wxSQLite3Blob blob = m_storage.GetReadOnlyBlob(id, "image", "characters");
			if (blob.IsOk() && blob.GetSize() > 12) {
				wxMemoryBuffer imageBuf;
				blob.Read(imageBuf, blob.GetSize(), 0);

				wxMemoryInputStream stream(imageBuf.GetData(), imageBuf.GetDataLen());
				character.image.LoadFile(stream, wxBITMAP_TYPE_PNG);
			}
		}

		wxSQLite3ResultSet result = m_storage.ExecuteQuery(wxString("SELECT name, content FROM characters_custom WHERE character_id = ") <<
			id << ";");

		while (result.NextRow()) {
			character.custom.push_back(pair<wxString, wxString>(result.GetAsString("name"), result.GetAsString("content")));
		}

		m_project.characters.push_back(character);
	}

	wxVectorSort(m_project.characters);
}

void amProjectManager::LoadLocations() {
	wxSQLite3Table table = m_storage.GetTable("SELECT * FROM locations");
	int count = table.GetRowCount();
	m_project.locations.reserve(count);

	for (int i = 0; i < count; i++) {
		table.SetRow(i);
		int id = table.GetInt("id");

		Location location;

		location.name = table.GetAsString("name");
		location.general = table.GetAsString("general");
		location.natural = table.GetAsString("natural");
		location.architecture = table.GetAsString("architecture");
		location.politics = table.GetAsString("politics");
		location.economy = table.GetAsString("economy");
		location.culture = table.GetAsString("culture");

		location.role = (Role)table.GetInt("role");
		location.id = id;

		if (!table.IsNull("image")) {
			wxSQLite3Blob blob = m_storage.GetReadOnlyBlob(id, "image", "locations");
			if (blob.IsOk() && blob.GetSize() > 12) {
				wxMemoryBuffer imageBuf;
				blob.Read(imageBuf, blob.GetSize(), 0);

				wxMemoryInputStream stream(imageBuf.GetData(), imageBuf.GetDataLen());
				location.image.LoadFile(stream, wxBITMAP_TYPE_PNG);
			}
		}

		wxSQLite3ResultSet result = m_storage.ExecuteQuery(wxString("SELECT name, content FROM locations_custom WHERE location_id = ") <<
			id << ";");

		while (result.NextRow()) {
			location.custom.push_back(pair<wxString, wxString>(result.GetAsString("name"), result.GetAsString("content")));
		}

		m_project.locations.push_back(location);
	}
}

void amProjectManager::LoadItems() {
	wxSQLite3Table table = m_storage.GetTable("SELECT * FROM items");
	int count = table.GetRowCount();
	m_project.items.reserve(count);

	for (int i = 0; i < count; i++) {
		table.SetRow(i);
		int id = table.GetInt("id");

		Item item;

		item.name = table.GetAsString("name");
		item.width = table.GetAsString("width");
		item.height = table.GetAsString("height");
		item.depth = table.GetAsString("depth");
		item.general = table.GetAsString("general");
		item.appearance = table.GetAsString("appearance");
		item.origin = table.GetAsString("origin");
		item.backstory = table.GetAsString("backstory");
		item.usage = table.GetAsString("usage");

		item.isManMade = table.GetBool("isManMade");
		item.isMagic = table.GetBool("isMagic");
		item.role = (Role)table.GetInt("role");
		item.id = id;

		if (!table.IsNull("image")) {
			wxSQLite3Blob blob = m_storage.GetReadOnlyBlob(id, "image", "items");
			if (blob.IsOk() && blob.GetSize() > 12) {
				wxMemoryBuffer imageBuf;
				blob.Read(imageBuf, blob.GetSize(), 0);

				wxMemoryInputStream stream(imageBuf.GetData(), imageBuf.GetDataLen());
				item.image.LoadFile(stream, wxBITMAP_TYPE_PNG);
			}
		}

		wxSQLite3ResultSet result = m_storage.ExecuteQuery(wxString("SELECT name, content FROM items_custom WHERE item_id = ") <<
			id << ";");

		while (result.NextRow()) {
			item.custom.push_back(pair<wxString, wxString>(result.GetAsString("name"), result.GetAsString("content")));
		}

		m_project.items.push_back(item);
	}
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

amStoryNotebook* amProjectManager::GetStoryNotebook() {
	return m_storyNotebook;
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

int amProjectManager::GetDocumentId(amDocument& document) {
	int id = -1;
	try {
		id = m_storage.GetDocumentId(document);
	} catch (wxString& e) {
		wxMessageBox("There was an issue when saving - " + e);
	}

	return id;
}

void amProjectManager::AddCharacter(Character& character, bool refreshElements) {
	m_storage.Begin();
	character.Save(&m_storage);
	m_storage.Commit();

	m_project.characters.push_back(character);
	wxVectorSort(m_project.characters);

	if (refreshElements) {
		m_elements->UpdateCharacterList();
		m_elements->SetSearchAC(wxBookCtrlEvent());
	}

	m_outline->GetOutlineFiles()->AppendCharacter(character);
}

void amProjectManager::AddLocation(Location& location, bool refreshElements) {
	m_storage.Begin();
	location.Save(&m_storage);
	m_storage.Commit();

	m_project.locations.push_back(location);
	wxVectorSort(m_project.locations);

	if (refreshElements) {
		m_elements->UpdateLocationList();
		m_elements->SetSearchAC(wxBookCtrlEvent());
	}

	m_outline->GetOutlineFiles()->AppendLocation(location);
}

void amProjectManager::AddItem(Item& item, bool refreshElements) {
	m_storage.Begin();
	item.Save(&m_storage);
	m_storage.Commit();

	m_project.items.push_back(item);
	wxVectorSort(m_project.items);

	if (refreshElements) {
		m_elements->UpdateItemList();
		m_elements->SetSearchAC(wxBookCtrlEvent());
	}

	m_outline->GetOutlineFiles()->AppendItem(item);
}

/// <summary>
/// Add new Chapter to selected book and in specified section.
/// </summary>
/// <param name="chapter">Reference to chapter object</param>
/// <param name="book">Reference to desired book object</param>
/// <param name="sectionPos">Section position in Book (First section is number 1!)</param>
/// <param name="pos">Where in the section will the chapter be inserted</param>
void amProjectManager::AddChapter(Chapter& chapter, Book& book, int sectionPos, int pos) {
	Section& section = book.sections[sectionPos - 1];

	chapter.sectionID =section.id;
	size_t capacityBefore = section.chapters.capacity();

	m_storyNotebook->AddChapter(chapter, pos);

	chapter.Save(&m_storage);

	if (pos < section.chapters.size() + 1 && pos > -1) {
		Chapter* chapterIt = section.chapters.begin();
		for (int i = 0; i < pos; i++) {
			chapterIt++;
		}
		section.chapters.insert(chapterIt, chapter);

		m_storage.Begin();
		for (int i = 0; i < section.chapters.size(); i++) {
			section.chapters[i].position = i + 1;
			section.chapters[i].Update(&m_storage, false, false);
		}
		m_storage.Commit();
	} else {
		section.chapters.push_back(chapter);
	}

	if (section.chapters.capacity() > capacityBefore)
		RedeclareChapsInElements(book);
}

void amProjectManager::EditCharacter(Character& original, Character& edit, bool sort) {
	try {
		if (sort) {
			for (Chapter* chapter : original.chapters) {
				chapter->characters.Remove(original.name);
				chapter->characters.Add(edit.name);
			}
		}

	m_outline->GetOutlineFiles()->DeleteCharacter(original);
	original = edit;
	std::thread thread([&]() {
		m_storage.Begin();
		original.Update(&m_storage);
		m_storage.Commit();
		}
	);
	if (sort) {
			wxVectorSort(m_project.characters);
			m_elements->UpdateCharacterList();
		} else {
			int n = 0;
			for (Character& character : m_project.characters) {
				if (character == original) {
					m_elements->UpdateCharacter(n, character);
					break;
				}

				n++;
			}
		}

	m_elements->m_charShow->SetData(original);
	m_mainFrame->Enable(true);

	thread.detach();

	m_outline->GetOutlineFiles()->AppendCharacter(edit);

	} catch (wxString& e) {
		wxMessageBox(e);
	}
}

void amProjectManager::EditLocation(Location& original, Location& edit, bool sort) {
	try {
		m_outline->GetOutlineFiles()->DeleteLocation(original);
		if (sort) {
			for (Chapter* chapter : original.chapters) {
				chapter->characters.Remove(original.name);
				chapter->characters.Add(edit.name);
			}
		}

		original = edit;
		std::thread thread([&]() {
			m_storage.Begin();
			original.Update(&m_storage);
			m_storage.Commit();
			}
		);
		if (sort) {
			wxVectorSort(m_project.locations);
			m_elements->UpdateLocationList();
		} else {
			int n = 0;
			for (Location& location : m_project.locations) {
				if (location == original) {
					m_elements->UpdateLocation(n, location);
					break;
				}

				n++;
			}
		}
		
		m_elements->m_locShow->SetData(original);
		m_mainFrame->Enable(true);

		thread.detach();

		m_outline->GetOutlineFiles()->AppendLocation(edit);
	} catch (wxString& e) {
		wxMessageBox(e);
	}
}

void amProjectManager::EditItem(Item& original, Item& edit, bool sort) {
	try {
		m_outline->GetOutlineFiles()->DeleteItem(original);

		if (sort) {
			for (Chapter* chapter : original.chapters) {
				chapter->characters.Remove(original.name);
				chapter->characters.Add(edit.name);
			}
		}

		original = edit;
		std::thread thread([&]() {
			m_storage.Begin();
			original.Update(&m_storage);
			m_storage.Commit();
			}
		);

		if (sort) {
			wxVectorSort(m_project.items);
			m_elements->UpdateItemList();
		} else {
			int n = 0;
			for (Item& item : m_project.items) {
				if (item == original) {
					m_elements->UpdateItem(n, item);
					break;
				}

				n++;
			}
		}
		m_elements->m_itemShow->SetData(original);
		m_mainFrame->Enable(true);

		thread.detach();

		m_outline->GetOutlineFiles()->AppendItem(edit);
	} catch (wxString& e) {
		wxMessageBox(e);
	}
}

void amProjectManager::AddChapterToCharacter(const wxString& characterName, Chapter& chapter, bool addToDb) {
	for (Character& character : m_project.characters) {
		if (characterName == character.name) {

			bool has = false;
			for (unsigned int i = 0; i < character.chapters.size(); i++)
				if (character.chapters[i] == &chapter)
					has = true;

			if (!has) {
				character.chapters.push_back(&chapter);

				if (addToDb)
					m_storage.InsertManyToMany("characters_chapters",
					character.id, "character_id",
					chapter.id, "chapter_id");
			}

			if (chapter.characters.Index(characterName) == -1)
				chapter.characters.Add(characterName);
		}
	}
}

void amProjectManager::AddChapterToLocation(const wxString& locationName, Chapter& chapter, bool addToDb) {
	for (Location& location : m_project.locations) {
		if (locationName == location.name) {

			bool has = false;
			for (unsigned int i = 0; i < location.chapters.size(); i++)
				if (location.chapters[i] == &chapter)
					has = true;

			if (!has) {
				location.chapters.push_back(&chapter);

				if (addToDb)
					m_storage.InsertManyToMany("locations_chapters",
					location.id, "location_id",
					chapter.id, "chapter_id");
			}
			if (chapter.locations.Index(locationName) == -1)
				chapter.locations.Add(locationName);
		}
	}
}

void amProjectManager::AddChapterToItem(const wxString& itemName, Chapter& chapter, bool addToDb) {
	for (Item& item : m_project.items) {
		if (itemName == item.name) {
			bool has = false;
			for (unsigned int i = 0; i < item.chapters.size(); i++)
				if (item.chapters[i] == &chapter)
					has = true;

			if (!has) {
				item.chapters.push_back(&chapter);

				if (addToDb)
					m_storage.InsertManyToMany("items_chapters",
					item.id, "item_id",
					chapter.id, "chapter_id");
			}

			if (chapter.items.Index(itemName) == -1)
				chapter.items.Add(itemName);
		}
	}
}

void amProjectManager::RemoveChapterFromCharacter(const wxString& characterName, Chapter& chapter) {
	for (Character& character : m_project.characters) {
		if (characterName == character.name) {
			for (wxVector<Chapter*>::iterator chapterIt = character.chapters.begin();
				chapterIt != character.chapters.end(); chapterIt++) {
				if (*(*chapterIt) == chapter) {
					character.chapters.erase(chapterIt);
					m_storage.DeleteManyToMany("characters_chapters",
						character.id, "character_id",
						chapter.id, "chapter_id");
				
					break;
				}
			}

			chapter.characters.Remove(characterName);
			return;
		}
	}

	wxLogMessage("Could not remove character '%s' from chapter '%s'", characterName, chapter.name);
}

void amProjectManager::RemoveChapterFromLocation(const wxString& locationName, Chapter& chapter) {
	for (Location& location : m_project.locations) {
		if (locationName == location.name) {
			for (wxVector<Chapter*>::iterator chapterIt = location.chapters.begin();
				chapterIt != location.chapters.end(); chapterIt++) {
				if (*(*chapterIt) == chapter) {
					location.chapters.erase(chapterIt);
					m_storage.DeleteManyToMany("locations_chapters",
						location.id, "location_id",
						chapter.id, "chapter_id");

					break;
				}
			}

			chapter.locations.Remove(locationName);
			return;
		}
	}

	wxLogMessage("Could not remove location '%s' from chapter '%s'", locationName, chapter.name);
}

void amProjectManager::RemoveChapterFromItem(const wxString& itemName, Chapter& chapter) {
	for (Item& item : m_project.items) {
		if (itemName == item.name) {
			for (wxVector<Chapter*>::iterator chapterIt = item.chapters.begin();
				chapterIt != item.chapters.end(); chapterIt++) {
				if (*(*chapterIt) == chapter) {
					item.chapters.erase(chapterIt);
					m_storage.DeleteManyToMany("items_chapters",
						item.id, "item_id",
						chapter.id, "chapter_id");
				
					break;
				}
			}

			chapter.items.Remove(itemName);
			return;
		}
	}

	wxLogMessage("Could not remove item '%s' from chapter '%s'", itemName, chapter.name);
}

void amProjectManager::RedeclareChapsInElements(Book& book) {
	for (Character& character : m_project.characters)
		character.chapters.clear();

	for (Location& location : m_project.locations)
		location.chapters.clear();

	for (Item& item : m_project.items)
		item.chapters.clear();

	for (Section& section : book.sections) {
		for (Chapter& chapter : section.chapters) {
			for (wxString& characterName : chapter.characters)
				AddChapterToCharacter(characterName, chapter, false);

			for (wxString& locationName : chapter.locations)
				AddChapterToLocation(locationName, chapter, false);

			for (wxString& itemName : chapter.items)
				AddChapterToItem(itemName, chapter, false);
		}
	}
}

void amProjectManager::DeleteCharacter(Character& character) {
	for (Chapter* chapter : character.chapters)
		RemoveChapterFromCharacter(character.name, *chapter);

	m_storage.DeleteDocument(character.GenerateDocumentForId());
	m_project.characters.erase(&character);
}

void amProjectManager::DeleteLocation(Location& location) {
	for (Chapter* chapter : location.chapters)
		chapter->characters.Remove(location.name);

	m_project.locations.erase(&location);
	m_storage.DeleteDocument(location.GenerateDocumentForId());

}

void amProjectManager::DeleteItem(Item& item) {
	for (Chapter* chapter : item.chapters)
		chapter->characters.Remove(item.name);

	m_project.items.erase(&item);
	m_storage.DeleteDocument(item.GenerateDocumentForId());
}

void amProjectManager::DeleteChapter(Chapter& chapter, Section& section) {
	for (wxString& characterName : chapter.characters) {
		for (Character& character : m_project.characters) {

			if (characterName == character.name) {

				for (Chapter* chapterIt : character.chapters) {
					if (chapterIt == &chapter)
						character.chapters.erase(&chapterIt);

				}
			}
		}
	}

	for (wxString& locationName : chapter.locations) {
		for (Location& location : m_project.locations) {

			if (locationName == location.name) {

				for (Chapter* chapterIt : location.chapters) {
					if (chapterIt == &chapter)
						location.chapters.erase(&chapterIt);

				}
			}
		}
	}

	for (wxString& itemName : chapter.items) {
		for (Item& item : m_project.items) {

			if (itemName == item.name) {

				for (Chapter* chapterIt : item.chapters) {
					if (chapterIt == &chapter)
						item.chapters.erase(&chapterIt);

				}
			}
		}
	}

	section.chapters.erase(&chapter);
	m_storage.DeleteDocument(chapter.GenerateDocumentForId());

}

wxVector<Character>& amProjectManager::GetCharacters() {
	return m_project.GetCharacters();
}

wxVector<Location>& amProjectManager::GetLocations() {
	return m_project.GetLocations();
}

wxVector<Item>& amProjectManager::GetItems() {
	return m_project.GetItems();
}

wxVector<Chapter>& amProjectManager::GetChapters(int bookPos, int sectionPos) {
	return m_project.GetChapters(bookPos, sectionPos);
}

/// <summary>
/// Get copy of all chapters in a book, regardless of section
/// </summary>
/// <param name="bookPos">Book position (First book has position 1!)</param>
/// <returns></returns>
wxVector<Chapter> amProjectManager::GetChapters(int bookPos) {
	return m_project.GetChapters(bookPos);
}

wxArrayString amProjectManager::GetCharacterNames() {
	wxArrayString names(true);
	for (Character& character : m_project.characters)
		names.Add(character.name);

	return names;
}

wxArrayString amProjectManager::GetLocationNames() {
	wxArrayString names(true);
	for (Location& location : m_project.locations)
		names.Add(location.name);

	return names;
}

wxArrayString amProjectManager::GetItemNames() {
	wxArrayString names(true);
	for (Item& item : m_project.items)
		names.Add(item.name);

	return names;
}

wxArrayString amProjectManager::GetBookTitles() {
	wxArrayString names;
	for (Book& book : m_project.books)
		names.Add(book.title);

	return names;
}