#include "ProjectManager.h"

#include "ProjectWizard.h"
#include "MainFrame.h"
#include "Overview.h"
#include "ElementsNotebook.h"
#include "StoryNotebook.h"
#include "Outline.h"
#include "Release.h"
#include "OutlineFiles.h"
#include "Timeline.h"
#include "ElementShowcases.h"
#include "StoryWriter.h"
#include "SortFunctions.h"

#include <wx\progdlg.h>
#include <wx\mstream.h>
#include <wx\utils.h>
#include <fstream>

#include <thread>

#include "wxmemdbg.h"


///////////////////////////////////////////////////////////////////
//////////////////////////// SQLStorage ///////////////////////////
///////////////////////////////////////////////////////////////////


amProjectSQLDatabase::amProjectSQLDatabase(wxFileName& path)
{
	EnableForeignKeySupport(true);
	if ( path.IsOk() && !path.IsDir() )
	{
		Open(path.GetFullPath());
	}
}

bool amProjectSQLDatabase::Init()
{
	if ( !TableExists("characters") )
	{
		CreateAllTables();
		return false;
	}

	return true;
}

void amProjectSQLDatabase::CreateAllTables()
{
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
	tBooks.Add("cover BLOB");

	wxArrayString tDocuments;
	tDocuments.Add("id INTEGER PRIMARY KEY");
	tDocuments.Add("name TEXT NOT NULL");
	tDocuments.Add("synopsys TEXT");
	tDocuments.Add("content TEXT");
	tDocuments.Add("position INTEGER");
	tDocuments.Add("type INTEGER");
	tDocuments.Add("parent_document_id INTEGER");
	tDocuments.Add("character_pov_id INTEGER");
	tDocuments.Add("book_id INTEGER");
	tDocuments.Add("isInTrash INTEGER");
	tDocuments.Add("FOREIGN KEY (parent_document_id) REFERENCES documents(id)");
	tDocuments.Add("FOREIGN KEY (character_pov_id) REFERENCES characters(id)");
	tDocuments.Add("FOREIGN KEY (book_id) REFERENCES books(id)");

	wxArrayString tDocumentNotes;
	tDocumentNotes.Add("id INTEGER PRIMARY KEY");
	tDocumentNotes.Add("name TEXT");
	tDocumentNotes.Add("content TEXT");
	tDocumentNotes.Add("isDone INTEGER");
	tDocumentNotes.Add("document_id INTEGER");
	tDocumentNotes.Add("FOREIGN KEY (document_id) REFERENCES documents(id)");

	wxArrayString tOutlineCorkboards;
	tOutlineCorkboards.Add("id INTEGER PRIMARY KEY");
	tOutlineCorkboards.Add("name TEXT");
	tOutlineCorkboards.Add("content TEXT");

	wxArrayString tOutlineTimelines;
	tOutlineTimelines.Add("id INTEGER PRIMARY KEY");
	tOutlineTimelines.Add("name TEXT");
	tOutlineTimelines.Add("content TEXT");
	tOutlineTimelines.Add("timeline_elements TEXT");

	wxArrayString tOutlineFiles;
	tOutlineFiles.Add("id INTEGER PRIMARY KEY");
	tOutlineFiles.Add("name TEXT");
	tOutlineFiles.Add("content TEXT");

	//////////////////// MANY-TO-MANY TABLES ////////////////////

	wxArrayString tCharactersInDocuments;
	tCharactersInDocuments.Add("character_id INTEGER");
	tCharactersInDocuments.Add("document_id INTEGER");
	tCharactersInDocuments.Add("FOREIGN KEY(character_id) REFERENCES characters(id)");
	tCharactersInDocuments.Add("FOREIGN KEY(document_id) REFERENCES documents(id)");

	wxArrayString tLocationsInDocuments;
	tLocationsInDocuments.Add("location_id INTEGER");
	tLocationsInDocuments.Add("document_id INTEGER");
	tLocationsInDocuments.Add("FOREIGN KEY(location_id) REFERENCES location(id)");
	tLocationsInDocuments.Add("FOREIGN KEY(document_id) REFERENCES documents(id)");

	wxArrayString tItemsInDocuments;
	tItemsInDocuments.Add("item_id INTEGER");
	tItemsInDocuments.Add("document_id INTEGER");
	tItemsInDocuments.Add("FOREIGN KEY(item_id) REFERENCES item(id)");
	tItemsInDocuments.Add("FOREIGN KEY(document_id) REFERENCES documents(id)");

	wxArrayString tCharactersToCharacters;
	tCharactersToCharacters.Add("element_id INTEGER");
	tCharactersToCharacters.Add("related_id INTEGER");
	tCharactersToCharacters.Add("relation INTEGER");
	tCharactersToCharacters.Add("FOREIGN KEY(element_id) REFERENCES characters(id)");
	tCharactersToCharacters.Add("FOREIGN KEY(related_id) REFERENCES characters(id)");

	wxArrayString tCharactersToLocations;
	tCharactersToLocations.Add("element_id INTEGER");
	tCharactersToLocations.Add("related_id INTEGER");
	tCharactersToLocations.Add("FOREIGN KEY(element_id) REFERENCES characters(id)");
	tCharactersToLocations.Add("FOREIGN KEY(related_id) REFERENCES locations(id)");

	wxArrayString tCharactersToItems;
	tCharactersToItems.Add("element_id INTEGER");
	tCharactersToItems.Add("related_id INTEGER");
	tCharactersToItems.Add("FOREIGN KEY(element_id) REFERENCES characters(id)");
	tCharactersToItems.Add("FOREIGN KEY(related_id) REFERENCES items(id)");

	wxArrayString tLocationsToLocations;
	tLocationsToLocations.Add("element_id INTEGER");
	tLocationsToLocations.Add("related_id INTEGER");
	tLocationsToLocations.Add("FOREIGN KEY(element_id) REFERENCES locations(id)");
	tLocationsToLocations.Add("FOREIGN KEY(related_id) REFERENCES locations(id)");

	wxArrayString tLocationsToItems;
	tLocationsToItems.Add("element_id INTEGER");
	tLocationsToItems.Add("related_id INTEGER");
	tLocationsToItems.Add("FOREIGN KEY(element_id) REFERENCES locations(id)");
	tLocationsToItems.Add("FOREIGN KEY(related_id) REFERENCES items(id)");

	wxArrayString tItemsToItems;
	tItemsToItems.Add("element_id INTEGER");
	tItemsToItems.Add("related_id INTEGER");
	tItemsToItems.Add("FOREIGN KEY(element_id) REFERENCES items(id)");
	tItemsToItems.Add("FOREIGN KEY(related_id) REFERENCES items(id)");

	try
	{
		Begin();

		CreateTable("characters", tCharacters);
		CreateTable("characters_custom", tCharactersCustom);

		CreateTable("locations", tLocations);
		CreateTable("locations_custom", tLocationsCustom);

		CreateTable("items", tItems);
		CreateTable("items_custom", tItemsCustom);

		CreateTable("books", tBooks);
		CreateTable("documents", tDocuments);
		CreateTable("document_notes", tDocumentNotes);

		CreateTable("outline_corkboards", tOutlineCorkboards);
		CreateTable("outline_timelines", tOutlineTimelines);
		CreateTable("outline_files", tOutlineFiles);

		CreateTable("characters_documents", tCharactersInDocuments);
		CreateTable("locations_documents", tLocationsInDocuments);
		CreateTable("items_documents", tItemsInDocuments);

		CreateTable("characters_characters", tCharactersToCharacters);
		CreateTable("characters_locations", tCharactersToLocations);
		CreateTable("characters_items", tCharactersToItems);
		CreateTable("locations_locations", tLocationsToLocations);
		CreateTable("locations_items", tLocationsToItems);
		CreateTable("items_items", tItemsToItems);

		Commit();
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

int amProjectSQLDatabase::GetSQLEntryId(amSQLEntry& sqlEntry)
{
	wxString query("SELECT DISTINCT id FROM ");
	query << sqlEntry.tableName;
	query << " WHERE ";

	wxSQLite3StatementBuffer buffer;
	
	bool nameEmpty = sqlEntry.name.IsEmpty();

	if ( !nameEmpty )
		query << "name = '" << buffer.Format("%q", (const char*)sqlEntry.name) << "'";

	if ( sqlEntry.specialForeign )
	{
		if ( !nameEmpty )
			query << " AND ";

		query << buffer.Format("%q", (const char*)sqlEntry.foreignKey.first) << " = " << sqlEntry.foreignKey.second;
	}

	if ( !sqlEntry.integers.empty() )
	{
		if ( !nameEmpty || sqlEntry.specialForeign )
			query << " AND ";

		int i = 0;
		for ( std::pair<const wxString, int>& it : sqlEntry.integers )
		{
			if ( i != 0 )
				query << " AND ";

			query << buffer.Format("%q", (const char*)it.first) << " = " << it.second;
			i++;
		}

	}

	query << ";";
	wxSQLite3ResultSet result = ExecuteQuery(query);

	int id = -1;

	// I return the first row with found id, this is the best solution I found
	// without throwing an exception. Maybe one day I'll try and find another one.
	while ( result.NextRow() )
	{
		id = result.GetInt("id");
		break;
	}

	return id;
}

bool amProjectSQLDatabase::CreateTable(const wxString& tableName, const wxArrayString& arguments,
	bool ifNotExists)
{
	if ( arguments.IsEmpty() )
	{
		wxMessageBox("You can't create a table with no arguments!");
		return false;
	}

	wxString update("CREATE TABLE ");
	if ( ifNotExists )
		update << "IF NOT EXISTS ";

	update << tableName;
	update << " (";

	bool first = true;

	for ( auto& str : arguments )
	{
		if ( !first )
			update << ", ";

		update << str;
		first = false;
	}

	update << ");";
	ExecuteUpdate(update);

	return true;
}

bool amProjectSQLDatabase::InsertSQLEntry(amSQLEntry& sqlEntry)
{
	wxSQLite3Statement statement = ConstructInsertStatement(sqlEntry);

	statement.ExecuteUpdate();

	for ( amSQLEntry& documentIt : sqlEntry.childEntries )
	{
		if ( documentIt.specialForeign )
			documentIt.foreignKey.second = GetSQLEntryId(sqlEntry);

		InsertSQLEntry(documentIt);
	}
	return true;
}

bool amProjectSQLDatabase::UpdateSQLEntry(amSQLEntry& original, amSQLEntry& edit)
{
	int id = GetSQLEntryId(original);

	try
	{
		wxSQLite3Statement statement = ConstructUpdateStatement(edit, id);
		statement.ExecuteUpdate();
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox("Exception thrown - " + e.GetMessage());
	}

	int prevSize = original.childEntries.size();
	int newSize = edit.childEntries.size();

	if ( prevSize == 0 && newSize == 0 )
		return true;

	if ( prevSize < newSize )
	{
		int i = 0;

		for ( i; i < prevSize; i++ )
		{
			original.childEntries[i].foreignKey.second = id;
			edit.childEntries[i].foreignKey.second = id;
			UpdateSQLEntry(original.childEntries[i], edit.childEntries[i]);
		}

		for ( i; i < newSize; i++ )
		{
			edit.childEntries[i].foreignKey.second = id;
			InsertSQLEntry(edit.childEntries[i]);
		}

	}
	else
	{
		if ( prevSize > newSize )
		{
			for ( int i = prevSize - 1; i > newSize - 1; i-- )
			{
				original.childEntries[i].foreignKey.second = id;
				DeleteSQLEntry(original.childEntries[i]);
			}
		}

		for ( int i = 0; i < newSize; i++ )
		{
			original.childEntries[i].foreignKey.second = id;
			edit.childEntries[i].foreignKey.second = id;
			UpdateSQLEntry(original.childEntries[i], edit.childEntries[i]);
		}
	}
	return true;
}

bool amProjectSQLDatabase::InsertManyToMany(const wxString& tableName,
	int sqlID1, const wxString& arg1,
	int sqlID2, const wxString& arg2)
{

	wxString insert("INSERT INTO ");
	insert << tableName << " (" << arg1 << ", " << arg2 << ") VALUES (";

	insert << sqlID1 << ", " << sqlID2 << ");";

	return ExecuteUpdate(insert);
}

bool amProjectSQLDatabase::InsertManyToMany(const wxString& tableName,
	amSQLEntry& sqlEntry1, const wxString& arg1,
	amSQLEntry& sqlEntry2, const wxString& arg2)
{

	return InsertManyToMany(tableName, GetSQLEntryId(sqlEntry1), arg1, GetSQLEntryId(sqlEntry2), arg2);
}

bool amProjectSQLDatabase::DeleteSQLEntry(amSQLEntry& sqlEntry)
{
	wxString statement("DELETE FROM ");
	statement << sqlEntry.tableName << " WHERE id = " << GetSQLEntryId(sqlEntry) << ";";

	return ExecuteUpdate(statement);
}

bool amProjectSQLDatabase::DeleteManyToMany(const wxString& tableName,
	int sqlID1, const wxString& arg1,
	int sqlID2, const wxString& arg2)
{
	wxString statement("DELETE FROM ");
	statement << tableName << " WHERE " << arg1 << " = " << sqlID1;
	statement << " AND " << arg2 << " = " << sqlID2 << ";";

	return ExecuteUpdate(statement);
}

bool amProjectSQLDatabase::DeleteManyToMany(const wxString& tableName,
	amSQLEntry& sqlEntry1, const wxString& arg1,
	amSQLEntry& sqlEntry2, const wxString& arg2)
{
	return DeleteManyToMany(tableName, GetSQLEntryId(sqlEntry1), arg1, GetSQLEntryId(sqlEntry2), arg2);
}

wxSQLite3Statement amProjectSQLDatabase::ConstructInsertStatement(amSQLEntry& sqlEntry)
{
	wxString insert("INSERT INTO ");
	insert << sqlEntry.tableName << " (";

	wxString columnNames;
	wxString valueNames;

	bool first = true;

	wxSQLite3StatementBuffer buffer;

	if ( sqlEntry.name != "" )
	{
		if ( !first )
		{
			columnNames << ", ";
			valueNames << ", ";
		}
		else
			first = false;

		columnNames << "name";
		valueNames << "'" << buffer.Format("%q", (const char*)sqlEntry.name) << "'";
	}

	for ( pair<const wxString, int>& it : sqlEntry.integers )
	{
		if ( !first )
		{
			columnNames << ", ";
			valueNames << ", ";
		}
		else
			first = false;

		columnNames << it.first;
		valueNames << it.second;
	}

	for ( pair<const wxString, wxString>& it : sqlEntry.strings )
	{
		if ( !first )
		{
			columnNames << ", ";
			valueNames << ", ";
		}
		else
			first = false;

		columnNames << it.first;
		valueNames << "'" << buffer.Format("%q", (const char*)it.second) << "'";
	}

	for ( pair<const wxString, wxMemoryBuffer>& it : sqlEntry.memBuffers )
	{
		if ( !first )
		{
			columnNames << ", ";
			valueNames << ", ";
		}
		else
			first = false;

		columnNames << it.first;
		valueNames << "?";
	}

	if ( sqlEntry.specialForeign )
	{
		if ( !first )
		{
			columnNames << ", ";
			valueNames << ", ";
		}

		columnNames << sqlEntry.foreignKey.first;
		valueNames << sqlEntry.foreignKey.second;
	}

	insert << columnNames << ") VALUES (" << valueNames << ");";

	try
	{
		wxSQLite3Statement statement = PrepareStatement(insert);

		int i = 1;

		for ( pair<const wxString, wxMemoryBuffer>& it : sqlEntry.memBuffers )
			statement.Bind(i++, it.second);

		return statement;
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return wxSQLite3Statement();
}

wxSQLite3Statement amProjectSQLDatabase::ConstructUpdateStatement(amSQLEntry& sqlEntry, int id)
{
	wxString update("UPDATE ");
	update << sqlEntry.tableName << " SET ";

	bool first = true;
	wxSQLite3StatementBuffer buffer;
	if ( sqlEntry.name != "" )
	{
		if ( !first )
			update << ", ";

		update << "name = '" << buffer.Format("%q", (const char*)sqlEntry.name.ToUTF8()) << "'";
		first = false;
	}

	for ( pair<const wxString, int>& it : sqlEntry.integers )
	{
		if ( !first )
			update << ", ";

		update << it.first << " = " << it.second;
		first = false;
	}

	for ( pair<const wxString, wxString>& it : sqlEntry.strings )
	{
		if ( !first )
			update << ", ";

		update << it.first << " = '" << buffer.Format("%q", (const char*)it.second) << "'";
		first = false;
	}

	for ( pair<const wxString, wxMemoryBuffer>& it : sqlEntry.memBuffers )
	{
		if ( !first )
			update << ", ";

		update << it.first << " = ?";
		first = false;
	}

	if ( sqlEntry.specialForeign )
	{
		if ( !first )
			update << ", ";

		update << sqlEntry.foreignKey.first << " = " << sqlEntry.foreignKey.second;
		first = false;
	}

	update << " WHERE id = " << id << ";";

	try
	{
		wxSQLite3Statement statement = PrepareStatement(update);

		int i = 1;

		for ( pair<const wxString, wxMemoryBuffer>& it : sqlEntry.memBuffers )
			statement.Bind(i++, it.second);

		return statement;
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return wxSQLite3Statement();

}

bool amProjectSQLDatabase::RowExists(amSQLEntry& sqlEntry)
{
	return GetSQLEntryId(sqlEntry) != -1;
}


///////////////////////////////////////////////////////////////////////
//////////////////////////// ProjectManager ///////////////////////////
///////////////////////////////////////////////////////////////////////


amProjectManager::amProjectManager()
{

}

amProjectManager::~amProjectManager()
{
	m_storage.Close();
	wxSQLite3Database::ShutdownSQLite();
}

bool amProjectManager::Init()
{
	if ( m_isInitialized )
		return false;

	if ( m_project.amFile.IsOk() )
	{
		if ( !m_mainFrame )
		{
			m_mainFrame = new amMainFrame("Amadeus Writer - " + m_project.amFile.GetFullName(),
				this, wxDefaultPosition, wxDefaultSize);

			m_overview = m_mainFrame->GetOverview();
			m_elements = m_mainFrame->GetElementsNotebook();
			m_storyNotebook = m_mainFrame->GetStoryNotebook();
			m_outline = m_mainFrame->GetOutline();
			m_release = m_mainFrame->GetRelease();
		}

		wxSQLite3Database::InitializeSQLite();
		m_storage.Open(m_project.amFile.GetFullPath());

		if ( !m_storage.Init() )
		{
			Book* book = new Book(m_project.amFile.GetName(), 1);

			book->Save(&m_storage);
			m_project.books.push_back(book);
			m_overview->LoadOverview();
		}
		else
			LoadProject();

		m_elements->InitShowChoices();
		m_isInitialized = true;

		return true;
	}
	return false;
}

bool amProjectManager::SaveProject()
{
	return DoSaveProject(m_project.amFile.GetFullPath());
}

bool amProjectManager::LoadProject()
{
	return DoLoadProject(m_project.amFile.GetFullPath());
}

void amProjectManager::SaveSQLEntry(amSQLEntry& original, amSQLEntry& edit)
{
	if ( m_storage.RowExists(original) )
		m_storage.UpdateSQLEntry(original, edit);
	else
		m_storage.InsertSQLEntry(edit);
}

bool amProjectManager::DoSaveProject(const wxString& path)
{
	// Sets Save state as saved, updates the current title and all that good stuff.
	// The attribute "saveDialog" currently does nothing.
	m_mainFrame->SetTitle("Amadeus Writer - " + m_project.amFile.GetFullName());
	m_mainFrame->SetFocus();
	SetLastSave();

	return true;
}

bool amProjectManager::DoLoadProject(const wxString& path)
{
	try
	{
		SetProjectFileName(path);
		m_mainFrame->OnNewFile(wxCommandEvent());

		m_storage.Begin();

		LoadCharacters();
		LoadLocations();
		LoadItems();

		LoadBooks();

		LoadStandardRelations();

		m_storage.Commit();

		m_overview->LoadOverview();
		m_elements->UpdateAll();
		m_outline->LoadOutline(&m_storage);

		m_mainFrame->SetTitle("Amadeus Writer - " + m_project.amFile.GetFullName());
		SetLastSave();
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return true;
}

void amProjectManager::LoadBooks()
{
	wxSQLite3Table table = m_storage.GetTable("SELECT * FROM books");
	int count = table.GetRowCount();
	m_project.books.reserve(count);

	for ( int i = 0; i < count; i++ )
	{
		table.SetRow(i);
		int id = table.GetInt("id");

		Book* book = new Book(table.GetAsString("name"), i + 1);

		book->author = table.GetAsString("author");
		book->genre = table.GetAsString("genre");
		book->description = table.GetAsString("description");
		book->synopsys = table.GetAsString("synopsys");

		book->SetId(id);

		LoadDocuments(book->documents, id);

		m_project.books.push_back(book);
	}
}

void amProjectManager::LoadBookContent(Book* book)
{
	wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT * FROM books WHERE book_id = " +
		std::to_string(book->id) + " ORDER BY position ASC;");
	int i = 0;
}

void amProjectManager::LoadDocuments(wxVector<Document*>& documents, int bookId)
{
	wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT * FROM documents WHERE book_id = " +
		std::to_string(bookId) + " ORDER BY position ASC;");

	while ( result.NextRow() )
	{
		int documentId = result.GetInt("id");

		Document* pDocument = new Document(nullptr, bookId, result.GetInt("position"), (DocumentType)result.GetInt("type"));

		pDocument->name = result.GetAsString("name");
		pDocument->synopsys = result.GetAsString("synopsys");
		pDocument->position = result.GetInt("position");
		pDocument->type = (DocumentType)result.GetInt("type");
		pDocument->bookID = bookId;
		pDocument->isInTrash = result.GetInt("isInTrash");

		if ( !result.IsNull("content") )
		{
			wxString content = result.GetAsString("content");

			if ( content != "NULL" )
			{
				pDocument->buffer = new wxRichTextBuffer;
				wxStringInputStream sstream(content);
				pDocument->buffer->LoadFile(sstream, wxRICHTEXT_TYPE_XML);
			}
		}

		pDocument->SetId(documentId);

		wxSQLite3ResultSet results2 = m_storage.ExecuteQuery("SELECT * FROM document_notes WHERE document_id = " +
			std::to_string(documentId) + ";");

		while ( results2.NextRow() )
		{
			Note* note = new Note(results2.GetAsString("content"), results2.GetAsString("name"));
			note->isDone = results2.GetBool("isDone");

			pDocument->notes.push_back(note);
		}

		results2 = m_storage.ExecuteQuery("SELECT * FROM characters_documents WHERE document_id = " +
			std::to_string(documentId) + ";");

		while ( results2.NextRow() )
		{
			wxSQLite3ResultSet characterResult = m_storage.ExecuteQuery("SELECT name FROM characters WHERE id = " +
				std::to_string(results2.GetInt("character_id")));

			AddElementToDocument(GetElementByName(characterResult.GetAsString("name")), pDocument, false);
		}

		results2 = m_storage.ExecuteQuery("SELECT * FROM locations_documents WHERE document_id = " +
			std::to_string(documentId) + ";");

		while ( results2.NextRow() )
		{
			wxSQLite3ResultSet locationResult = m_storage.ExecuteQuery("SELECT name FROM locations WHERE id = " +
				std::to_string(results2.GetInt("location_id")));

			AddElementToDocument(GetElementByName(locationResult.GetAsString("name")), pDocument, false);
		}

		results2 = m_storage.ExecuteQuery("SELECT * FROM items_documents WHERE document_id = " +
			std::to_string(documentId) + ";");

		while ( results2.NextRow() )
		{
			wxSQLite3ResultSet itemResult = m_storage.ExecuteQuery("SELECT name FROM items WHERE id = " +
				std::to_string(results2.GetInt("item_id")));

			AddElementToDocument(GetElementByName(itemResult.GetAsString("name")), pDocument, false);
		}

		documents.push_back(pDocument);
		m_storyNotebook->AddDocument(pDocument);
	}
}

void amProjectManager::SetupDocumentHierarchy(Book* book)
{
	wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT book_id, parent_document_id FROM documents ORDER BY position ASC;");

	while ( result.NextRow() )
	{
		if ( !result.IsNull("parent_document_id") )
		{
			Document* pDocument = GetDocumentById(result.GetInt("id"));
			if ( pDocument )
			{
				Document* pParent = GetDocumentById(result.GetInt("parent_document_id"));
				if ( pParent )
				{
					pParent->children.push_back(pDocument);
					pDocument->parent = pParent;
				}
			}
		}
	}
}

void amProjectManager::LoadCharacters()
{
	wxSQLite3Table table = m_storage.GetTable("SELECT * FROM characters;");
	int count = table.GetRowCount();
	m_project.characters.reserve(count);

	for ( int i = 0; i < count; i++ )
	{
		table.SetRow(i);
		int id = table.GetInt("id");

		Character* pCharacter = new Character();

		pCharacter->name = table.GetAsString("name");
		pCharacter->sex = table.GetAsString("sex");
		pCharacter->age = table.GetAsString("age");
		pCharacter->nat = table.GetAsString("nationality");
		pCharacter->height = table.GetAsString("height");
		pCharacter->nick = table.GetAsString("nickname");
		pCharacter->appearance = table.GetAsString("appearance");
		pCharacter->personality = table.GetAsString("personality");
		pCharacter->backstory = table.GetAsString("backstory");

		pCharacter->role = (Role)table.GetInt("role");
		pCharacter->id = id;

		if ( !table.IsNull("image") )
		{
			wxSQLite3Blob blob = m_storage.GetReadOnlyBlob(id, "image", "characters");
			if ( blob.IsOk() && blob.GetSize() > 12 )
			{
				wxMemoryBuffer imageBuf;
				blob.Read(imageBuf, blob.GetSize(), 0);

				wxMemoryInputStream stream(imageBuf.GetData(), imageBuf.GetDataLen());
				pCharacter->image.LoadFile(stream, wxBITMAP_TYPE_PNG);
			}
		}

		wxSQLite3ResultSet result = m_storage.ExecuteQuery(wxString("SELECT name, content FROM characters_custom WHERE character_id = ") <<
			id << ";");

		while ( result.NextRow() )
		{
			pCharacter->custom.push_back(pair<wxString, wxString>(result.GetAsString("name"), result.GetAsString("content")));
		}

		m_project.characters.push_back(pCharacter);
	}

	std::sort(m_project.characters.begin(), m_project.characters.end(), amSortCharacters);
}

void amProjectManager::LoadLocations()
{
	wxSQLite3Table table = m_storage.GetTable("SELECT * FROM locations");
	int count = table.GetRowCount();
	m_project.locations.reserve(count);

	for ( int i = 0; i < count; i++ )
	{
		table.SetRow(i);
		int id = table.GetInt("id");

		Location* pLocation = new Location();

		pLocation->name = table.GetAsString("name");
		pLocation->general = table.GetAsString("general");
		pLocation->natural = table.GetAsString("natural");
		pLocation->architecture = table.GetAsString("architecture");
		pLocation->politics = table.GetAsString("politics");
		pLocation->economy = table.GetAsString("economy");
		pLocation->culture = table.GetAsString("culture");

		pLocation->role = (Role)table.GetInt("role");
		pLocation->id = id;

		if ( !table.IsNull("image") )
		{
			wxSQLite3Blob blob = m_storage.GetReadOnlyBlob(id, "image", "locations");
			if ( blob.IsOk() && blob.GetSize() > 12 )
			{
				wxMemoryBuffer imageBuf;
				blob.Read(imageBuf, blob.GetSize(), 0);

				wxMemoryInputStream stream(imageBuf.GetData(), imageBuf.GetDataLen());
				pLocation->image.LoadFile(stream, wxBITMAP_TYPE_PNG);
			}
		}

		wxSQLite3ResultSet result = m_storage.ExecuteQuery(wxString("SELECT name, content FROM locations_custom WHERE location_id = ") <<
			id << ";");

		while ( result.NextRow() )
		{
			pLocation->custom.push_back(pair<wxString, wxString>(result.GetAsString("name"), result.GetAsString("content")));
		}

		m_project.locations.push_back(pLocation);
	}

	std::sort(m_project.locations.begin(), m_project.locations.end(), amSortLocations);
}

void amProjectManager::LoadItems()
{
	wxSQLite3Table table = m_storage.GetTable("SELECT * FROM items");
	int count = table.GetRowCount();
	m_project.items.reserve(count);

	for ( int i = 0; i < count; i++ )
	{
		table.SetRow(i);
		int id = table.GetInt("id");

		Item* pItem = new Item();

		pItem->name = table.GetAsString("name");
		pItem->width = table.GetAsString("width");
		pItem->height = table.GetAsString("height");
		pItem->depth = table.GetAsString("depth");
		pItem->general = table.GetAsString("general");
		pItem->appearance = table.GetAsString("appearance");
		pItem->origin = table.GetAsString("origin");
		pItem->backstory = table.GetAsString("backstory");
		pItem->usage = table.GetAsString("usage");

		pItem->isManMade = table.GetBool("isManMade");
		pItem->isMagic = table.GetBool("isMagic");
		pItem->role = (Role)table.GetInt("role");
		pItem->id = id;

		if ( !table.IsNull("image") )
		{
			wxSQLite3Blob blob = m_storage.GetReadOnlyBlob(id, "image", "items");
			if ( blob.IsOk() && blob.GetSize() > 12 )
			{
				wxMemoryBuffer imageBuf;
				blob.Read(imageBuf, blob.GetSize(), 0);

				wxMemoryInputStream stream(imageBuf.GetData(), imageBuf.GetDataLen());
				pItem->image.LoadFile(stream, wxBITMAP_TYPE_PNG);
			}
		}

		wxSQLite3ResultSet result = m_storage.ExecuteQuery(wxString("SELECT name, content FROM items_custom WHERE item_id = ") <<
			id << ";");

		while ( result.NextRow() )
		{
			pItem->custom.push_back(pair<wxString, wxString>(result.GetAsString("name"), result.GetAsString("content")));
		}

		m_project.items.push_back(pItem);
	}

	std::sort(m_project.items.begin(), m_project.items.end(), amSortItems);
}

void amProjectManager::LoadStandardRelations()
{
	wxVector<std::pair<wxString, wxString>> tableNames;
	tableNames.push_back(std::make_pair("characters", "characters"));
	tableNames.push_back(std::make_pair("characters", "locations"));
	tableNames.push_back(std::make_pair("characters", "items"));
	tableNames.push_back(std::make_pair("locations", "locations"));
	tableNames.push_back(std::make_pair("locations", "items"));
	tableNames.push_back(std::make_pair("items", "items"));

	for ( std::pair<wxString, wxString>& table : tableNames )
	{
		wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT * FROM " + table.first + "_" + table.second + ";");
		int elementType, relatedType;
		
		if ( table.first == "characters" )
			elementType = 0;
		else if ( table.first == "locations" )
			elementType = 1;
		else
			elementType = 2;

		if ( table.second == "characters" )
			relatedType = 0;
		else if ( table.second == "locations" )
			relatedType = 1;
		else
			relatedType = 2;

		while ( result.NextRow() )
		{
			int elementID = result.GetInt("element_id");
			int relatedID = result.GetInt("related_id");

			wxVector<Element*> elements;
			switch ( elementType )
			{
			case 0:
				for ( Character*& pCharacter : GetCharacters() )
					elements.push_back(pCharacter); 
				
				break;

			case 1:
				for ( Location*& pLocation : GetLocations() )
					elements.push_back(pLocation);

				break;

			case 2:
				for ( Item*& pItem : GetItems() )
					elements.push_back(pItem);

				break;
			}

			for ( Element*& pElement : elements )
			{
				if ( pElement->id == elementID )
				{
					wxVector<Element*> related;
					switch ( relatedType )
					{
					case 0:
						for ( Character*& pCharacter : GetCharacters() )
							related.push_back(pCharacter);

						break;

					case 1:
						for ( Location*& pLocation : GetLocations() )
							related.push_back(pLocation);

						break;

					case 2:
						for ( Item*& pItem : GetItems() )
							related.push_back(pItem);

						break;
					}

					for ( Element*& pRelated : related )
					{
						if ( pRelated->id == relatedID )
						{
							RelateElements(pElement, pRelated, false);
							break;
						}
					}

					break;
				}
			}
		}
	}

	for ( Element*& pElement : GetAllElements() )
	{
		if ( !pElement->relatedElements.empty() )
			std::sort(pElement->relatedElements.begin(), pElement->relatedElements.end(), amSortElements);
	}
}

void amProjectManager::LoadCharacterRelations()
{}

void amProjectManager::SetExecutablePath(const wxString& path)
{
	m_executablePath.Assign(path);
}

void amProjectManager::SetProjectFileName(const wxFileName& fileName)
{
	m_project.amFile.Assign(fileName);
}

amMainFrame* amProjectManager::GetMainFrame()
{
	return m_mainFrame;
}

amElementsNotebook* amProjectManager::GetElementsNotebook()
{
	return m_elements;
}

amStoryNotebook* amProjectManager::GetStoryNotebook()
{
	return m_storyNotebook;
}

amOutline* amProjectManager::GetOutline()
{
	return m_outline;
}

amRelease* amProjectManager::GetRelease()
{
	return m_release;
}

wxString amProjectManager::GetPath(bool withSeparator)
{
	if ( withSeparator )
		return m_project.amFile.GetPathWithSep();
	else
		return m_project.amFile.GetPath();
}

void amProjectManager::ClearPath()
{
	m_project.amFile.Clear();
}

void amProjectManager::SetLastSave()
{
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

bool amProjectManager::GetLastSave()
{
	// Nothing special here, just reads the 88165468 file and, if succesful, calls the load function.
	// Else, just clear the paths and load a standard new project.
	std::ifstream last(GetExecutablePath(true).ToStdString() + "88165468", std::ios::binary | std::ios::in);

	if ( last.is_open() )
	{
		int size;
		char* data;

		last.read((char*)&size, sizeof(int));
		data = new char[size];
		last.read(data, size);
		SetProjectFileName(wxFileName(data));
		delete[] data;

		last.close();
	}
	else
		return false;

	if ( wxFileName::Exists(GetFullPath().ToStdString()) )
	{
		return true;
	}
	else
	{
		ClearPath();
		return false;
	}
}

int amProjectManager::GetSQLEntryId(amSQLEntry& sqlEntry)
{
	int id = -1;
	try
	{
		id = m_storage.GetSQLEntryId(sqlEntry);
	}
	catch ( wxString& e )
	{
		wxMessageBox("There was an issue when saving - " + e);
	}

	return id;
}

void amProjectManager::OpenDocument(int documentIndex)
{
	OpenDocument(GetCurrentBook()->documents[documentIndex]);
}

void amProjectManager::OpenDocument(Document* document)
{
	if ( m_storyWriter )
	{
		m_storyWriter->SetCurrentDocument(document, true, true);
		m_storyWriter->Maximize();
	}
	else
	{
		m_storyWriter = new amStoryWriter(m_mainFrame, this, document);

		if ( m_mainFrame->IsFullScreen() )
			m_storyWriter->ToggleFullScreen(true);
	}
}

bool amProjectManager::ScanForDocumentLinear(int toFind, int& current, Document* scanBegin, Document*& emptyPointer)
{
	if ( emptyPointer )
		return true;

	if ( current == toFind )
	{
		if ( !emptyPointer )
			emptyPointer = scanBegin;

		return true;
	}

	current++;
	for ( Document*& pChild : scanBegin->children )
	{
		if ( ScanForDocumentLinear(toFind, current, pChild, emptyPointer) )
			return true;
	}

	return false;
}

void amProjectManager::AddCharacter(Character* character, bool refreshElements)
{
	m_storage.Begin();
	character->Save(&m_storage);
	m_storage.Commit();

	m_project.characters.push_back(character);
	std::sort(m_project.characters.begin(), m_project.characters.end(), amSortCharacters);

	if ( refreshElements )
	{
		m_elements->UpdateCharacterList();
		m_elements->SetSearchAC(wxBookCtrlEvent());
	}

	m_outline->GetOutlineFiles()->AppendCharacter(character);
}

void amProjectManager::AddLocation(Location* location, bool refreshElements)
{
	m_storage.Begin();
	location->Save(&m_storage);
	m_storage.Commit();

	m_project.locations.push_back(location);
	std::sort(m_project.locations.begin(), m_project.locations.end(), amSortLocations);

	if ( refreshElements )
	{
		m_elements->UpdateLocationList();
		m_elements->SetSearchAC(wxBookCtrlEvent());
	}

	m_outline->GetOutlineFiles()->AppendLocation(location);
}

void amProjectManager::AddItem(Item* item, bool refreshElements)
{
	m_storage.Begin();
	item->Save(&m_storage);
	m_storage.Commit();

	m_project.items.push_back(item);
	std::sort(m_project.items.begin(), m_project.items.end(), amSortItems);

	if ( refreshElements )
	{
		m_elements->UpdateItemList();
		m_elements->SetSearchAC(wxBookCtrlEvent());
	}

	m_outline->GetOutlineFiles()->AppendItem(item);
}

/// <summary>
/// Add new Document to selected book and in specified section.
/// </summary>
/// <param name="document">Reference to document object</param>
/// <param name="book">Reference to desired book object</param>
/// <param name="sectionPos">Section position in Book (First section is number 1!)</param>
/// <param name="pos">Where in the section will the document be inserted</param>
void amProjectManager::AddDocument(Document* document, Book* book, int pos)
{
	document->bookID = book->id;
	m_storyNotebook->AddDocument(document, pos);

	document->Save(&m_storage);

	if ( pos < book->documents.size() + 1 && pos > -1 )
	{
		Document** documentIt = book->documents.begin();
		for ( int i = 0; i < pos; i++ )
			documentIt++;

		book->documents.insert(documentIt, document);

		m_storage.Begin();
		for ( int i = 0; i < book->documents.size(); i++ )
		{
			book->documents[i]->position = i + 1;
			book->documents[i]->Update(&m_storage, false, false);
		}
		m_storage.Commit();
	}
	else
	{
		book->documents.push_back(document);
	}
}

void amProjectManager::EditCharacter(Character* original, Character& edit, bool sort)
{
	try
	{
		m_outline->GetOutlineFiles()->DeleteCharacter(original);
		*original = edit;
		std::thread thread([&]()
			{
				m_storage.Begin();
				original->Update(&m_storage);
				m_storage.Commit();
			}
		);

		if ( sort )
		{
			std::sort(m_project.characters.begin(), m_project.characters.end(), amSortCharacters);
			m_elements->UpdateCharacterList();
		}
		else
		{
			int n = 0;
			for ( Character*& character : m_project.characters )
			{
				if ( character == original )
				{
					m_elements->UpdateCharacter(n, character);
					break;
				}

				n++;
			}
		}

		m_elements->m_charShow->SetData(original);
		m_mainFrame->Enable(true);

		thread.detach();

		m_outline->GetOutlineFiles()->AppendCharacter(original);

	}
	catch ( wxString& e )
	{
		wxMessageBox(e);
	}
}

void amProjectManager::EditLocation(Location* original, Location& edit, bool sort)
{
	try
	{
		m_outline->GetOutlineFiles()->DeleteLocation(original);

		*original = edit;
		std::thread thread([&]()
			{
				m_storage.Begin();
				original->Update(&m_storage);
				m_storage.Commit();
			}
		);

		if ( sort )
		{
			std::sort(m_project.locations.begin(), m_project.locations.end(), amSortLocations);
			m_elements->UpdateLocationList();
		}
		else
		{
			int n = 0;
			for ( Location*& location : m_project.locations )
			{
				if ( location == original )
				{
					m_elements->UpdateLocation(n, location);
					break;
				}

				n++;
			}
		}

		m_elements->m_locShow->SetData(original);
		m_mainFrame->Enable(true);

		thread.detach();

		m_outline->GetOutlineFiles()->AppendLocation(original);
	}
	catch ( wxString& e )
	{
		wxMessageBox(e);
	}
}

void amProjectManager::EditItem(Item* original, Item& edit, bool sort)
{
	try
	{
		m_outline->GetOutlineFiles()->DeleteItem(original);

		*original = edit;
		std::thread thread([&]()
			{
				m_storage.Begin();
				original->Update(&m_storage);
				m_storage.Commit();
			}
		);

		if ( sort )
		{
			std::sort(m_project.items.begin(), m_project.items.end(), amSortItems);
			m_elements->UpdateItemList();
		}
		else
		{
			int n = 0;
			for ( Item*& item : m_project.items )
			{
				if ( item == original )
				{
					m_elements->UpdateItem(n, item);
					break;
				}

				n++;
			}
		}
		m_elements->m_itemShow->SetData(original);
		m_mainFrame->Enable(true);

		thread.detach();

		m_outline->GetOutlineFiles()->AppendItem(original);
	}
	catch ( wxString& e )
	{
		wxMessageBox(e);
	}
}

void amProjectManager::AddElementToDocument(Element* element, Document* document, bool addToDb)
{
	if ( !element || !document )
		return;

	bool had = false;
	for ( Document*& pDocInElement : element->documents )
	{
		if ( pDocInElement == document )
		{
			had = true;
			break;
		}
	}

	if ( !had )
	{
		element->documents.push_back(document);
		std::sort(element->documents.begin(), element->documents.end(), amSortDocuments);

		if ( addToDb )
		{
			try
			{
				amSQLEntry elementEntry = element->GenerateSQLEntryForId();
				m_storage.InsertManyToMany(elementEntry.tableName + "_documents",
					elementEntry, elementEntry.tableName.Left(elementEntry.tableName.size() - 1) + "_id",
					document->GenerateSQLEntryForId(), "document_id");
			}
			catch ( wxSQLite3Exception& e )
			{
				wxMessageBox(e.GetMessage());
			}
		}
	}

	had = false;
	for ( Element*& pElementInDoc : document->elements )
	{
		if ( pElementInDoc == element )
		{
			had = true;
			break;
		}
	}

	if ( !had )
	{
		std::sort(document->elements.begin(), document->elements.end(), amSortElements);
		document->elements.push_back(element);
	}
}

void amProjectManager::RemoveElementFromDocument(Element * element, Document * document)
{
	if ( !element || !document )
		return;

	for ( Document*& pDocInElement : element->documents )
	{
		if ( pDocInElement == document )
		{
			element->documents.erase(&pDocInElement);

			amSQLEntry elementEntry = element->GenerateSQLEntryForId();
			m_storage.DeleteManyToMany(elementEntry.tableName + "_documents",
				element->id, elementEntry.tableName.Left(elementEntry.tableName.size() - 1) + "_id",
				document->id, "document_id");

			break;
		}
	}

	for ( Element*& pElementInDoc : document->elements)
	{
		if ( pElementInDoc == element)
		{
			document->elements.erase(&pElementInDoc);
			break;
		}
	}

}

void amProjectManager::RelateElements(Element* element1, Element* element2, bool addToDb)
{
	bool had1 = false;
	for ( Element*& pPresent : element1->relatedElements )
	{
		if ( pPresent == element2 )
		{
			had1 = true;
			break;
		}
	}

	if ( !had1 )
	{
		element1->relatedElements.push_back(element2);
		std::sort(element1->relatedElements.begin(), element1->relatedElements.end(), amSortElements);
	}

	bool had2 = false;
	for ( Element*& pPresent : element2->relatedElements )
	{
		if ( pPresent == element1 )
		{
			had2 = true;
			break;
		}
	}

	if ( !had2 )
	{
		element2->relatedElements.push_back(element1);
		std::sort(element2->relatedElements.begin(), element2->relatedElements.end(), amSortElements);
	}

	if ( addToDb )
	{
		amSQLEntry sqlEntry1 = element1->GenerateSQLEntryForId();
		amSQLEntry sqlEntry2 = element2->GenerateSQLEntryForId();

		if ( !had1 )
		{
			wxString table1 = sqlEntry1.tableName + "_" + sqlEntry2.tableName;
			if ( m_storage.TableExists(table1) )
			{
				m_storage.InsertManyToMany(table1,
					sqlEntry1, "element_id",
					sqlEntry2, "related_id");
			}
		}

		if ( !had2 )
		{
			wxString table2 = sqlEntry2.tableName + "_" + sqlEntry1.tableName;
			if ( m_storage.TableExists(table2) )
			{
				m_storage.InsertManyToMany(table2,
					sqlEntry2, "element_id",
					sqlEntry1, "related_id");
			}
		}
	}
}

void amProjectManager::UnrelateElements(Element* element1, Element* element2, bool removeFromDb)
{
	bool had1 = false;
	for ( Element*& pPresent : element1->relatedElements )
	{
		if ( pPresent == element2 )
		{
			element1->relatedElements.erase(&pPresent);
			had1 = true;
			break;
		}
	}

	bool had2 = false;
	for ( Element*& pPresent : element2->relatedElements )
	{
		if ( pPresent == element1 )
		{
			element2->relatedElements.erase(&pPresent);
			had2 = true;
			break;
		}
	}

	if ( removeFromDb )
	{
		amSQLEntry sqlEntry1 = element1->GenerateSQLEntryForId();
		amSQLEntry sqlEntry2 = element2->GenerateSQLEntryForId();

		if ( had1 )
		{
			wxString table1 = sqlEntry1.tableName + "_" + sqlEntry2.tableName;

			if ( m_storage.TableExists(table1) )
			{
				m_storage.DeleteManyToMany(table1,
					sqlEntry1, "element_id",
					sqlEntry2, "related_id");
			}
		}

		if ( had2 )
		{
			wxString table2 = sqlEntry2.tableName + "_" + sqlEntry1.tableName;

			if ( m_storage.TableExists(table2) )
			{
				m_storage.DeleteManyToMany(table2,
					sqlEntry2, "element_id",
					sqlEntry1, "related_id");
			}
		}
	}
}

Element* amProjectManager::GetElementByName(const wxString& name)
{
	for ( Element* pElement : GetAllElements() )
	{
		if ( pElement->name == name )
			return pElement;
	}

	return nullptr;
}

void amProjectManager::DeleteCharacter(Character* character, bool clearShowcase)
{
	wxBusyCursor cursor;
	for ( Document*& pDocument : character->documents )
		RemoveElementFromDocument(character, pDocument);

	if ( clearShowcase )
		m_elements->GetCharacterShowcase()->SetData(nullptr);

	for ( amTLThread*& thread : m_outline->GetTimeline()->GetThreads() )
	{
		if ( thread->GetCharacter() == character->name )
		{
			m_outline->GetTimeline()->DeleteThread(thread, false);
		}
	}

	amSQLEntry sqlEntry = character->GenerateSQLEntry();
	for ( amSQLEntry& childEntry : sqlEntry.childEntries )
	{
		m_storage.DeleteSQLEntry(childEntry);
	}

	m_storage.DeleteSQLEntry(sqlEntry);
	for ( Character*& pCharacter : m_project.characters )
	{
		if ( pCharacter == character )
		{
			delete pCharacter;
			m_project.characters.erase(&pCharacter);
			break;
		}
	}
}

void amProjectManager::DeleteLocation(Location* location, bool clearShowcase)
{
	for ( Document*& pDocument : location->documents )
		RemoveElementFromDocument(location, pDocument);

	if ( clearShowcase )
		m_elements->GetLocationShowcase()->SetData(nullptr);

	amSQLEntry sqlEntry = location->GenerateSQLEntry();
	for ( amSQLEntry& childEntry : sqlEntry.childEntries )
	{
		m_storage.DeleteSQLEntry(childEntry);
	}

	m_storage.DeleteSQLEntry(sqlEntry);
	for ( Location*& pLocation : m_project.locations )
	{
		if ( pLocation == location )
		{
			delete pLocation;
			m_project.locations.erase(&pLocation);
			break;
		}
	}
}

void amProjectManager::DeleteItem(Item* item, bool clearShowcase)
{
	for ( Document*& pDocument : item->documents )
		RemoveElementFromDocument(item, pDocument);

	if ( clearShowcase )
		m_elements->GetItemShowcase()->SetData(nullptr);

	amSQLEntry sqlEntry = item->GenerateSQLEntry();
	for ( amSQLEntry& childEntry : sqlEntry.childEntries )
	{
		m_storage.DeleteSQLEntry(childEntry);
	}

	m_storage.DeleteSQLEntry(sqlEntry);

	for ( Item*& pItem : m_project.items )
	{
		if ( pItem == item )
		{
			delete pItem;
			m_project.items.erase(&pItem);
			break;
		}
	}
}

void amProjectManager::DeleteDocument(Document* document)
{
	for ( Document* pChild : document->children )
	{
		DeleteDocument(document);
	}

	for ( Element*& pElement : document->elements )
		RemoveElementFromDocument(pElement, document);

	m_storyNotebook->DeleteDocument(document);
	m_storage.DeleteSQLEntry(document->GenerateSQLEntryForId());

	if ( document->parent )
	{
		for ( Document*& pSibling : document->parent->children )
		{
			if ( pSibling == document )
				document->parent->children.erase(&pSibling);
		}
	}

	Book* currentBook = GetCurrentBook();
	for ( Document*& pDocInBook : currentBook->documents )
	{
		if ( pDocInBook == document )
		{
			delete pDocInBook;
			currentBook->documents.erase(&pDocInBook);
		}
	}
}

wxVector<Character*>& amProjectManager::GetCharacters()
{
	return m_project.GetCharacters();
}

wxVector<Location*>& amProjectManager::GetLocations()
{
	return m_project.GetLocations();
}

wxVector<Item*>& amProjectManager::GetItems()
{
	return m_project.GetItems();
}

wxVector<Element*> amProjectManager::GetAllElements()
{
	wxVector<Element*> elements;

	for ( Character*& pCharacter : GetCharacters() )
		elements.push_back(pCharacter);

	for ( Location*& pLocation: GetLocations() )
		elements.push_back(pLocation);

	for ( Item*& pItem : GetItems() )
		elements.push_back(pItem);

	return elements;
}

wxVector<Document*>& amProjectManager::GetDocumentsInCurrentBook()
{
	return m_project.GetDocuments(m_currentBook);
}

Document* amProjectManager::GetDocumentById(int id)
{
	for ( Book*& pBook : m_project.books )
	{
		for ( Document*& pDocument : pBook->documents )
		{
			if ( pDocument->id == id )
				return pDocument;
		}
	}

	return nullptr;
}

wxArrayString amProjectManager::GetCharacterNames()
{
	wxArrayString names(true);
	for ( Character*& pCharacter : m_project.characters )
		names.Add(pCharacter->name);

	return names;
}

wxArrayString amProjectManager::GetLocationNames()
{
	wxArrayString names(true);
	for ( Location*& pLocation : m_project.locations )
		names.Add(pLocation->name);

	return names;
}

wxArrayString amProjectManager::GetItemNames()
{
	wxArrayString names(true);
	for ( Item*& pItem : m_project.items )
		names.Add(pItem->name);

	return names;
}

wxArrayString amProjectManager::GetBookTitles()
{
	wxArrayString names;
	for ( Book*& book : m_project.books )
		names.Add(book->title);

	return names;
}