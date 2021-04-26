#include "ProjectManager.h"

#include "ProjectWizard.h"
#include "MainFrame.h"
#include "Overview.h"
#include "ElementsNotebook.h"
#include "StoryNotebook.h"
#include "Outline.h"
#include "Release.h"
#include "OutlineFiles.h"
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

	bool nameEmpty = sqlEntry.name == "";

	if ( !nameEmpty )
		query << "name = '" << sqlEntry.name << "'";

	if ( sqlEntry.specialForeign )
	{
		if ( !nameEmpty )
			query << " AND ";

		query << sqlEntry.foreignKey.first << " = " << sqlEntry.foreignKey.second;
	}

	if ( !sqlEntry.integers.empty() )
	{
		if ( !nameEmpty || sqlEntry.specialForeign )
			query << " AND ";

		pair<wxString, int>* it = sqlEntry.integers.begin();
		query << it->first << " = " << it->second << ";";
	}

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

bool amProjectSQLDatabase::InsertDocument(amSQLEntry& sqlEntry)
{
	wxSQLite3Statement statement = ConstructInsertStatement(sqlEntry);

	statement.ExecuteUpdate();

	for ( amSQLEntry& documentIt : sqlEntry.childEntries )
	{
		if ( documentIt.specialForeign )
			documentIt.foreignKey.second = GetSQLEntryId(sqlEntry);

		InsertDocument(documentIt);
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
			InsertDocument(edit.childEntries[i]);
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

	for ( pair<wxString, int>& it : sqlEntry.integers )
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

	for ( pair<wxString, wxString>& it : sqlEntry.strings )
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

	for ( pair<wxString, wxMemoryBuffer>& it : sqlEntry.memBuffers )
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

		for ( pair<wxString, wxMemoryBuffer>& it : sqlEntry.memBuffers )
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

	for ( pair<wxString, int>& it : sqlEntry.integers )
	{
		if ( !first )
			update << ", ";

		update << it.first << " = " << it.second;
		first = false;
	}

	for ( pair<wxString, wxString>& it : sqlEntry.strings )
	{
		if ( !first )
			update << ", ";

		update << it.first << " = '" << buffer.Format("%q", (const char*)it.second) << "'";
		first = false;
	}

	for ( pair<wxString, wxMemoryBuffer>& it : sqlEntry.memBuffers )
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

		for ( pair<wxString, wxMemoryBuffer>& it : sqlEntry.memBuffers )
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
		m_storage.InsertDocument(edit);
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

		wxString str1, str2, str3, str4;

		wxSQLite3ResultSet result = m_storage.ExecuteQuery("SELECT content FROM outline_corkboards;");
		while ( result.NextRow() )
			str1 = result.GetAsString("content");

		result = m_storage.ExecuteQuery("SELECT content, timeline_elements FROM outline_timelines;");
		while ( result.NextRow() )
		{
			str2 = result.GetAsString("content");
			str3 = result.GetAsString("timeline_elements");
		}

		result = m_storage.ExecuteQuery("SELECT content FROM outline_files;");
		while ( result.NextRow() )
			str4 = result.GetAsString("content");

		wxStringInputStream corkboard(str1), timelineCanvas(str2), timelineElements(str3), files(str4);

		m_storage.Commit();

		m_overview->LoadOverview();
		m_elements->UpdateAll();
		m_outline->LoadOutline(corkboard, timelineCanvas, timelineElements, files);

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
			pDocument->buffer = new wxRichTextBuffer();
			wxStringInputStream sstream(result.GetAsString("content"));
			pDocument->buffer->LoadFile(sstream, wxRICHTEXT_TYPE_XML);
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

			for ( Character*& pCharacter : m_project.GetCharacters() )
			{
				if ( pCharacter->name == characterResult.GetAsString("name") )
					AddDocumentToCharacter(pCharacter->name, pDocument, false);
			}
		}

		results2 = m_storage.ExecuteQuery("SELECT * FROM locations_documents WHERE document_id = " +
			std::to_string(documentId) + ";");

		while ( results2.NextRow() )
		{
			wxSQLite3ResultSet locationResult = m_storage.ExecuteQuery("SELECT name FROM locations WHERE id = " +
				std::to_string(results2.GetInt("location_id")));

			for ( Location*& pLocation : m_project.GetLocations() )
			{
				if ( pLocation->name == locationResult.GetAsString("name") )
					AddDocumentToLocation(pLocation->name, pDocument, false);
			}
		}

		results2 = m_storage.ExecuteQuery("SELECT * FROM items_documents WHERE document_id = " +
			std::to_string(documentId) + ";");

		while ( results2.NextRow() )
		{
			wxSQLite3ResultSet itemResult = m_storage.ExecuteQuery("SELECT name FROM items WHERE id = " +
				std::to_string(results2.GetInt("item_id")));

			for ( Item*& pItem : m_project.GetItems() )
			{
				if ( pItem->name == itemResult.GetAsString("name") )
					AddDocumentToItem(pItem->name, pDocument, false);
			}
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
}

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
	Document* toSet = GetCurrentBook()->documents[documentIndex];

	if ( m_storyWriter )
	{
		m_storyWriter->SetCurrentDocument(toSet, true, true);
		m_storyWriter->Maximize();
	}
	else
	{
		m_storyWriter = new amStoryWriter(m_mainFrame, this, toSet);

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

void amProjectManager::AddDocumentToCharacter(const wxString& characterName, Document* document, bool addToDb)
{
	for ( Character*& pCharacter : m_project.characters )
	{
		if ( characterName == pCharacter->name )
		{

			bool has = false;
			for ( Document*& pDocInChar : pCharacter->documents )
			{
				if ( pDocInChar == document )
				{
					has = true;
					break;
				}
			}

			if ( !has )
			{
				pCharacter->documents.push_back(document);

				if ( addToDb )
					m_storage.InsertManyToMany("characters_documents",
					pCharacter->id, "character_id",
					document->id, "document_id");
			}

			has = false;
			for ( Character*& pCharInDoc : document->characters )
			{
				if ( pCharInDoc == pCharacter )
				{
					has = true;
					break;
				}
			}

			if ( !has )
				document->characters.push_back(pCharacter);

			break;
		}
	}
}

void amProjectManager::AddDocumentToLocation(const wxString& locationName, Document* document, bool addToDb)
{
	for ( Location*& pLocation : m_project.locations )
	{
		if ( locationName == pLocation->name )
		{

			bool has = false;
			for ( Document*& pDocInLoc : pLocation->documents )
				if ( pDocInLoc == document )
				{
					has = true;
					break;
				}

			if ( !has )
			{
				pLocation->documents.push_back(document);

				if ( addToDb )
					m_storage.InsertManyToMany("locations_documents",
					pLocation->id, "location_id",
					document->id, "document_id");
			}

			has = false;
			for ( Location*& pLocInDoc : document->locations )
			{
				if ( pLocInDoc == pLocation )
				{
					has = true;
					break;
				}
			}

			if ( !has )
				document->locations.push_back(pLocation);

			break;
		}
	}
}

void amProjectManager::AddDocumentToItem(const wxString& itemName, Document* document, bool addToDb)
{
	for ( Item*& pItem : m_project.items )
	{
		if ( itemName == pItem->name )
		{
			bool has = false;
			for ( Document*& pDocInItem : pItem->documents )
			{
				if ( pDocInItem == document )
				{
					has = true;
					break;
				}
			}

			if ( !has )
			{
				pItem->documents.push_back(document);

				if ( addToDb )
					m_storage.InsertManyToMany("items_documents",
					pItem->id, "item_id",
					document->id, "document_id");
			}

			has = false;
			for ( Item*& pItemInDoc : document->items )
			{
				if ( pItemInDoc == pItem )
				{
					has = true;
					break;
				}
			}

			if ( !has )
				document->items.push_back(pItem);

			break;
		}
	}
}

void amProjectManager::RemoveDocumentFromCharacter(const wxString& characterName, Document* document)
{
	for ( Character*& pCharacter : m_project.characters )
	{
		if ( characterName == pCharacter->name )
		{
			for ( Document*& pDocInChar : pCharacter->documents )
			{
				if ( pDocInChar == document )
				{
					pCharacter->documents.erase(&pDocInChar);
					m_storage.DeleteManyToMany("characters_documents",
						pCharacter->id, "character_id",
						document->id, "document_id");

					break;
				}
			}

			for ( Character*& pCharInDoc : document->characters )
			{
				if ( pCharInDoc == pCharacter )
				{
					document->characters.erase(&pCharInDoc);
					break;
				}
			}

			return;
		}
	}

	wxLogMessage("Could not remove character '%s' from document '%s'", characterName, document->name);
}

void amProjectManager::RemoveDocumentFromLocation(const wxString& locationName, Document* document)
{
	for ( Location*& pLocation : m_project.locations )
	{
		if ( locationName == pLocation->name )
		{
			for ( Document*& pDocInLoc : pLocation->documents )
			{
				if ( pDocInLoc == document )
				{
					pLocation->documents.erase(&pDocInLoc);
					m_storage.DeleteManyToMany("locations_documents",
						pLocation->id, "location_id",
						document->id, "document_id");

					break;
				}
			}

			for ( Location*& pLocInDoc : document->locations )
			{
				if ( pLocInDoc == pLocation )
				{
					document->locations.erase(&pLocInDoc);
					break;
				}
			}

			return;
		}
	}

	wxLogMessage("Could not remove location '%s' from document '%s'", locationName, document->name);
}

void amProjectManager::RemoveDocumentFromItem(const wxString& itemName, Document* document)
{
	for ( Item*& pItem : m_project.items )
	{
		if ( itemName == pItem->name )
		{
			for ( Document*& pDocInItem : pItem->documents )
			{
				if ( pDocInItem == document )
				{
					pItem->documents.erase(&pDocInItem);
					m_storage.DeleteManyToMany("items_documents",
						pItem->id, "item_id",
						document->id, "document_id");

					break;
				}
			}

			for ( Item*& pItemInDoc : document->items )
			{
				if ( pItemInDoc == pItem )
				{
					document->items.erase(&pItemInDoc);
					break;
				}
			}

			return;
		}
	}

	wxLogMessage("Could not remove item '%s' from document '%s'", itemName, document->name);
}

void amProjectManager::DeleteCharacter(Character* character)
{
	for ( Document*& pDocument : character->documents )
		RemoveDocumentFromCharacter(character->name, pDocument);

	m_storage.DeleteSQLEntry(character->GenerateSQLEntryForId());

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

void amProjectManager::DeleteLocation(Location* location)
{
	for ( Document*& pDocument : location->documents )
		RemoveDocumentFromLocation(location->name, pDocument);

	m_storage.DeleteSQLEntry(location->GenerateSQLEntryForId());

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

void amProjectManager::DeleteItem(Item* item)
{
	for ( Document*& pDocument : item->documents )
		RemoveDocumentFromItem(item->name, pDocument);

	m_storage.DeleteSQLEntry(item->GenerateSQLEntryForId());

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

	for ( Character*& pCharacter : document->characters )
		RemoveDocumentFromCharacter(pCharacter->name, document);

	for ( Location*& pLocation : document->locations )
		RemoveDocumentFromLocation(pLocation->name, document);

	for ( Item*& pItem : document->items )
		RemoveDocumentFromItem(pItem->name, document);

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