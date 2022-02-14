#include "ProjectManaging.h"

#include "Views/Wizards/ProjectWizard.h"
#include "Views/Wizards/BookWizard.h"
#include "Views/MainFrame.h"
#include "Views/Overview/Overview.h"
#include "Views/Elements/ElementsNotebook.h"
#include "Views/Story/StoryNotebook.h"
#include "Views/Outline/Outline.h"
#include "Views/Release/Release.h"
#include "Views/Outline/OutlineFiles.h"
#include "Views/Outline/Timeline/Timeline.h"
#include "Views/Elements/ElementShowcases.h"
#include "Views/StoryWriter/StoryWriter.h"
#include "Utils/SortFunctions.h"
#include "Views/Elements/ElementCreators.h"

#include <wx/progdlg.h>
#include <wx/mstream.h>
#include <wx/utils.h>
#include <fstream>

#include <thread>

#include "Utils/wxmemdbg.h"


///////////////////////////////////////////////////////////////////
//////////////////////////// SQLStorage ///////////////////////////
///////////////////////////////////////////////////////////////////


am::ProjectSQLDatabase::ProjectSQLDatabase(wxFileName& path)
{
	EnableForeignKeySupport(true);
	if ( path.IsOk() && !path.IsDir() )
	{
		Open(path.GetFullPath());
	}
}

bool am::ProjectSQLDatabase::Init()
{
	if ( !TableExists("books") )
	{
		CreateAllTables();
		return false;
	}

	CreateAllTables();
	return true;
}

void am::ProjectSQLDatabase::CreateAllTables()
{
	wxArrayString tStoryElements;
	tStoryElements.Add("id INTEGER PRIMARY KEY");
	tStoryElements.Add("name TEXT NOT NULL");
	tStoryElements.Add("serialized TEXT NOT NULL");
	tStoryElements.Add("image BLOB");
	tStoryElements.Add("class TEXT NOT NULL");

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
	tDocuments.Add("plain_text TEXT");
	tDocuments.Add("position INTEGER");
	tDocuments.Add("type INTEGER");
	tDocuments.Add("parent_document_id INTEGER");
	tDocuments.Add("character_pov_id INTEGER");
	tDocuments.Add("book_id INTEGER");
	tDocuments.Add("isInTrash INTEGER");
	tDocuments.Add("FOREIGN KEY (parent_document_id) REFERENCES documents(id)");
	tDocuments.Add("FOREIGN KEY (character_pov_id) REFERENCES story_elements(id)");
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

	// Don't use element foreign keys because they don't point to one table only.
	wxArrayString tElementsInDocuments;
	tElementsInDocuments.Add("element_id INTEGER");
	tElementsInDocuments.Add("document_id INTEGER");
	tElementsInDocuments.Add("element_class TEXT");
	tElementsInDocuments.Add("FOREIGN KEY(document_id) REFERENCES documents(id)");

	wxArrayString tElementsToElements;
	tElementsToElements.Add("element_id INTEGER");
	tElementsToElements.Add("related_id INTEGER");
	tElementsToElements.Add("element_class TEXT");
	tElementsToElements.Add("related_class TEXT");
	tElementsToElements.Add("relation INTEGER");
	
	///////////////////// PROJECT TABLES ///////////////////////
	
	wxArrayString tProject;
	tProject.Add("id INTEGER PRIMARY KEY");
	tProject.Add("name TEXT");
	tProject.Add("version TEXT");
	tProject.Add("preferences TEXT");

	wxArrayString tSessions;
	tSessions.Add("id INTEGER PRIMARY KEY");
	tSessions.Add("total_word_count INTEGER");
	tSessions.Add("story_word_count INTEGER");
	tSessions.Add("date TEXT");

	wxArrayString tLastSession;
	tLastSession.Add("id INTEGER PRIMARY KEY");
	tLastSession.Add("selected_book_id INTEGER");
	tLastSession.Add("session_attributes TEXT");
	tLastSession.Add("recent_documents TEXT");
	tLastSession.Add("FOREIGN KEY (selected_book_id) REFERENCES books(id)");

	try
	{
		Begin();

		CreateTable("story_elements", tStoryElements);

		CreateTable("books", tBooks);
		CreateTable("documents", tDocuments);
		CreateTable("document_notes", tDocumentNotes);

		CreateTable("outline_corkboards", tOutlineCorkboards);
		CreateTable("outline_timelines", tOutlineTimelines);
		CreateTable("outline_files", tOutlineFiles);

		CreateTable("elements_documents", tElementsInDocuments);
		CreateTable("elements_elements", tElementsToElements);

		//CreateTable("project", tProject);
		CreateTable("sessions", tSessions);
		CreateTable("last_session", tLastSession);

		Commit();
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

int am::ProjectSQLDatabase::GetSQLEntryId(am::SQLEntry& sqlEntry)
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

bool am::ProjectSQLDatabase::CreateTable(const wxString& tableName, const wxArrayString& arguments,
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

bool am::ProjectSQLDatabase::InsertSQLEntry(am::SQLEntry& sqlEntry)
{
	wxSQLite3Statement statement = ConstructInsertStatement(sqlEntry);

	statement.ExecuteUpdate();

	for ( am::SQLEntry& documentIt : sqlEntry.childEntries )
	{
		if ( documentIt.specialForeign )
			documentIt.foreignKey.second = GetSQLEntryId(sqlEntry);

		InsertSQLEntry(documentIt);
	}
	return true;
}

bool am::ProjectSQLDatabase::UpdateSQLEntry(am::SQLEntry& original, am::SQLEntry& edit)
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

bool am::ProjectSQLDatabase::InsertManyToMany(const wxString& tableName,
	int sqlID1, const wxString& arg1,
	int sqlID2, const wxString& arg2)
{

	wxString insert("INSERT INTO ");
	insert << tableName << " (" << arg1 << ", " << arg2 << ") VALUES (";

	insert << sqlID1 << ", " << sqlID2 << ");";

	return ExecuteUpdate(insert);
}

bool am::ProjectSQLDatabase::InsertManyToMany(const wxString& tableName,
	am::SQLEntry& sqlEntry1, const wxString& arg1,
	am::SQLEntry& sqlEntry2, const wxString& arg2)
{

	return InsertManyToMany(tableName, GetSQLEntryId(sqlEntry1), arg1, GetSQLEntryId(sqlEntry2), arg2);
}

bool am::ProjectSQLDatabase::DeleteSQLEntry(am::SQLEntry& sqlEntry)
{
	wxString statement("DELETE FROM ");
	statement << sqlEntry.tableName << " WHERE id = " << GetSQLEntryId(sqlEntry) << ";";

	return ExecuteUpdate(statement);
}

bool am::ProjectSQLDatabase::DeleteManyToMany(const wxString& tableName,
	int sqlID1, const wxString& arg1,
	int sqlID2, const wxString& arg2)
{
	wxString statement("DELETE FROM ");
	statement << tableName << " WHERE " << arg1 << " = " << sqlID1;
	statement << " AND " << arg2 << " = " << sqlID2 << ";";

	return ExecuteUpdate(statement);
}

bool am::ProjectSQLDatabase::DeleteManyToMany(const wxString& tableName,
	am::SQLEntry& sqlEntry1, const wxString& arg1,
	am::SQLEntry& sqlEntry2, const wxString& arg2)
{
	return DeleteManyToMany(tableName, GetSQLEntryId(sqlEntry1), arg1, GetSQLEntryId(sqlEntry2), arg2);
}

wxSQLite3Statement am::ProjectSQLDatabase::ConstructInsertStatement(am::SQLEntry& sqlEntry)
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

wxSQLite3Statement am::ProjectSQLDatabase::ConstructUpdateStatement(am::SQLEntry& sqlEntry, int id)
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

bool am::ProjectSQLDatabase::RowExists(am::SQLEntry& sqlEntry)
{
	return GetSQLEntryId(sqlEntry) != -1;
}


///////////////////////////////////////////////////////////////////////
///////////////////////// ProjectPreferences //////////////////////////
///////////////////////////////////////////////////////////////////////


void am::ProjectPreferences::MarkSerializableDataMembers()
{}

void am::SessionAttributes::MarkSerializableDataMembers()
{
	XS_SERIALIZE_INT(nMainFrameSashPos, "main-frame-sash-pos");
	XS_SERIALIZE_INT(nCharacterSashPos, "character-sash-pos");
	XS_SERIALIZE_INT(nLocationSashPos, "location-sash-pos");
	XS_SERIALIZE_INT(nItemSashPos, "item-sash-pos");
	XS_SERIALIZE_INT(nOutlineFilesSashPos, "outline-files-sash-pos");
}


///////////////////////////////////////////////////////////////////////
/////////////////////////// ProjectManaging ///////////////////////////
///////////////////////////////////////////////////////////////////////

am::Project project;
am::ProjectSQLDatabase* pDatabase;
am::Book* pCurrentBook = nullptr;

amMainFrame* pMainFrame = nullptr;
amOverview* pOverview = nullptr;
amElementNotebook* pElementNotebook = nullptr;
amStoryNotebook* pStoryNotebook = nullptr;
amOutline* pOutline = nullptr;
amRelease* pRelease = nullptr;

amStoryWriter* pStoryWriter = nullptr;

wxFileName executablePath{};

bool bIsInitialized = false;


bool am::Init(const wxString& path)
{
	if ( bIsInitialized )
		return false;

	wxFileName amFile(path);

	if ( amFile.IsOk() )
	{
		wxBusyCursor busy;
		project.amFile.Assign(amFile);

		if ( !pMainFrame )
		{
			pMainFrame = new amMainFrame(project.amFile.GetFullName() + "Amadeus Writer - ", wxDefaultPosition, wxDefaultSize);
			pMainFrame->Hide();
			pMainFrame->Maximize();

			pOverview = pMainFrame->GetOverview();
			pElementNotebook = pMainFrame->GetElementsNotebook();
			pStoryNotebook = pMainFrame->GetStoryNotebook();
			pOutline = pMainFrame->GetOutline();
			pRelease = pMainFrame->GetRelease();
		}

		am::Book::coverSize = { pMainFrame->FromDIP(wxSize(660, 900)) };

		wxSQLite3Database::InitializeSQLite();
		pDatabase = new am::ProjectSQLDatabase;
		pDatabase->Open(project.amFile.GetFullPath());

		if ( !pDatabase->Init() )
		{
			pCurrentBook = new am::Book(project.amFile.GetName(), 1);
			AddBook(pCurrentBook);
			
			SetLastSavePath();
		}
		else
			LoadProject();

		pElementNotebook->InitShowChoices();
		bIsInitialized = true;

		pMainFrame->Show();
		return true;
	}

	return false;
}

void am::ShutDown()
{
	pDatabase->Close();
	wxSQLite3Database::ShutdownSQLite();
	delete pDatabase;
}

am::ProjectSQLDatabase* am::GetProjectDatabase()
{
	return pDatabase;
}

bool am::SaveProject()
{
	return am::DoSaveProject(project.amFile.GetFullPath());
}

bool am::LoadProject()
{
	return am::DoLoadProject(project.amFile.GetFullPath());
}

bool am::SaveSessionToDb()
{
	try
	{
		// Saving last session
		{
			SessionAttributes attr;
			attr.nMainFrameSashPos = pMainFrame->GetMainSplitter()->GetSashPosition();
			attr.nCharacterSashPos = pElementNotebook->m_characterPage->GetSashPosition();
			attr.nLocationSashPos = pElementNotebook->m_locationPage->GetSashPosition();
			attr.nItemSashPos = pElementNotebook->m_itemPage->GetSashPosition();
			attr.nOutlineFilesSashPos = pOutline->GetOutlineFiles()->GetSashPosition();

			wxXmlDocument attrDoc;
			wxXmlNode* pAttrRootNode = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "Session-Attributes");
			attrDoc.SetRoot(pAttrRootNode);
			pAttrRootNode->AddChild(attr.SerializeObject(nullptr));

			wxStringOutputStream attrStream;
			attrDoc.Save(attrStream);

			wxXmlDocument doc;
			wxXmlNode* pDocsRootNode = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "Recent-Documents");
			doc.SetRoot(pDocsRootNode);

			for ( Book*& pBook : project.books )
			{
				wxXmlNode* pBookNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Book");
				pBookNode->AddAttribute("name", pBook->title);
				pBookNode->AddAttribute("id", std::to_string(pBook->id));

				for ( am::Document*& pDocument : pBook->vRecentDocuments )
				{
					wxXmlNode* pDocNode = new wxXmlNode(wxXML_ELEMENT_NODE, "am::Document");
					pDocNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, pDocument->name));
					pDocNode->AddAttribute("id", std::to_string(pDocument->id));

					pBookNode->AddChild(pDocNode);
				}

				pDocsRootNode->AddChild(pBookNode);
			}

			wxStringOutputStream docStream;
			doc.Save(docStream);


			wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT id FROM last_session;");
			wxSQLite3StatementBuffer sqlBuffer;
			if ( result.NextRow() )
			{
				wxString strUpdate = "UPDATE last_session SET selected_book_id = ";
				strUpdate << GetCurrentBook()->id << ", recent_documents = '%q', session_attributes = '%q';";

				sqlBuffer.Format((const char*)strUpdate, (const char*)docStream.GetString().ToUTF8(),
					(const char*)attrStream.GetString().ToUTF8());
			}
			else
			{
				wxString strInsert = "INSERT INTO last_session (selected_book_id, recent_documents, session_attributes) VALUES (";
				strInsert << GetCurrentBook()->id << ", '%q', '%q')";

				sqlBuffer.Format((const char*)strInsert, (const char*)docStream.GetString().ToUTF8(),
					(const char*)attrStream.GetString().ToUTF8());
			}

			pDatabase->ExecuteUpdate(sqlBuffer);
		}

		// Saving persistent session attributes
		{
			int totalWordCount = 0, storyWordCount = 0;
			for ( Book*& pBook : project.books )
			{
				for ( am::Document*& pDocument : pBook->documents )
				{
					int wordCount = pDocument->GetWordCount();
					totalWordCount += wordCount;
					
					if ( pDocument->IsStory() )
						storyWordCount += wordCount;
				}
			}

			wxString strDate = wxDateTime::Now().FormatDate();
			wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT id FROM sessions WHERE date = '" + strDate + "';");
			wxString update;

			if ( result.NextRow() )
			{
				update << "UPDATE sessions SET total_word_count = " << totalWordCount
					<< ", story_word_count = " << storyWordCount << " WHERE date = '" << strDate << "';";
			}
			else
			{
				update << "INSERT INTO sessions (total_word_count, story_word_count, date) VALUES (" << totalWordCount
					<< ", " << storyWordCount << ", '" << strDate << "');";
			}

			pDatabase->ExecuteUpdate(update);
		}
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return true;
}

void am::SaveSQLEntry(am::SQLEntry& original, am::SQLEntry& edit)
{
	if ( pDatabase->RowExists(original) )
		pDatabase->UpdateSQLEntry(original, edit);
	else
		pDatabase->InsertSQLEntry(edit);
}

bool am::DoSaveProject(const wxString& path)
{
	// Sets Save state as saved, updates the current title and all that good stuff.
	// The attribute "saveDialog" currently does nothing.
	pMainFrame->SetTitle("Amadeus Writer - " + project.amFile.GetFullName());
	pMainFrame->SetFocus();
	SetLastSavePath();

	return true;
}

bool am::DoLoadProject(const wxString& path)
{
	try
	{
		project.amFile.Assign(path);
		pMainFrame->ClearAll();

		pDatabase->Begin();

		LoadStoryElements();
		LoadBooks();

		LoadStandardRelations();

		pDatabase->Commit();

		Book* currentBook = GetCurrentBook();

		pOverview->LoadBookContainer();
		pOutline->LoadOutline(pDatabase);

		if ( !LoadLastSession() )
			SetCurrentBook(currentBook);

		pMainFrame->SetTitle(project.amFile.GetFullName() + " - Amadeus Writer");
		SetLastSavePath();
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return true;
}

void am::LoadBooks()
{
	wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT name, id FROM books ORDER BY position;");

	int i = 0;
	while ( result.NextRow() )
	{
		am::Book* book = new am::Book(result.GetAsString("name"), i + 1);
		book->SetId(result.GetInt("id"));

		LoadBookContent(book, true);

		project.books.push_back(book);
		i++;
	}
}

void am::LoadBookContent(am::Book* book, bool loadDocuments)
{
	wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT * FROM books WHERE id = " +
		std::to_string(book->id) + " ORDER BY position ASC;");

	while ( result.NextRow() )
	{
		book->author = result.GetAsString("author");
		book->genre = result.GetAsString("genre");
		book->description = result.GetAsString("description");
		book->synopsys = result.GetAsString("synopsys");

		if ( loadDocuments )
			LoadDocuments(book);

		break;
	}
}

void am::LoadDocuments(am::Book* book)
{
	wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT * FROM documents WHERE book_id = " +
		std::to_string(book->id) + " ORDER BY position ASC;");

	while ( result.NextRow() )
	{
		int documentId = result.GetInt("id");

		am::Document* pDocument = new am::Document(nullptr, result.GetInt("position"), book, (DocumentType)result.GetInt("type"));

		pDocument->name = result.GetAsString("name");
		pDocument->synopsys = result.GetAsString("synopsys");
		pDocument->position = result.GetInt("position");
		pDocument->type = (DocumentType)result.GetInt("type");
		pDocument->book = book;
		pDocument->isInTrash = result.GetInt("isInTrash");

		pDocument->SetId(documentId);

		wxSQLite3ResultSet results2 = pDatabase->ExecuteQuery("SELECT * FROM elements_documents WHERE document_id = " +
			std::to_string(documentId) + ";");

		while ( results2.NextRow() )
		{
			wxClassInfo* elementClass = wxClassInfo::FindClass(results2.GetAsString("element_class"));

			for ( am::StoryElement*& pElement : GetAllElements() )
			{
				if ( pElement->IsKindOf(elementClass) && pElement->dbID == results2.GetInt("element_id") )
				{
					am::AddElementToDocument((am::TangibleElement*)pElement, pDocument, false);
				}
			}
		}

		book->documents.push_back(pDocument);
	}
}

void am::SetupDocumentHierarchy(am::Book* book)
{
	wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT book_id, parent_document_id FROM documents ORDER BY position ASC;");

	while ( result.NextRow() )
	{
		if ( !result.IsNull("parent_document_id") )
		{
			am::Document* pDocument = GetDocumentById(result.GetInt("id"));
			if ( pDocument )
			{
				am::Document* pParent = GetDocumentById(result.GetInt("parent_document_id"));
				if ( pParent )
				{
					pParent->children.push_back(pDocument);
					pDocument->parent = pParent;
				}
			}
		}
	}
}

void am::LoadStoryElements()
{
	wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT * FROM story_elements");

	while ( result.NextRow() )
	{
		int id = result.GetInt("id");

		am::StoryElement* pElement = (am::StoryElement*)wxClassInfo::FindClass(result.GetAsString("class"))->CreateObject();
		if ( !pElement )
			continue;

		pElement->dbID = id;

		wxStringInputStream sstream(result.GetAsString("serialized"));
		wxXmlDocument xmlDoc(sstream);
		pElement->DeserializeObject(xmlDoc.GetRoot());

		if ( !result.IsNull("image") )
		{
			wxSQLite3Blob blob = pDatabase->GetReadOnlyBlob(id, "image", "story_elements");
			if ( blob.IsOk() && blob.GetSize() > 12 )
			{
				wxMemoryBuffer imageBuf;
				blob.Read(imageBuf, blob.GetSize(), 0);

				wxMemoryInputStream stream(imageBuf.GetData(), imageBuf.GetDataLen());
				pElement->image.LoadFile(stream, wxBITMAP_TYPE_PNG);
			}
		}
	
		project.vStoryElements.push_back(pElement);
	}
}

void am::LoadStandardRelations()
{
	wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT * FROM elements_elements;");

	while ( result.NextRow() )
	{
		am::StoryElement* pElement = nullptr;
		am::StoryElement* pRelated = nullptr;
		int elementID = result.GetInt("element_id");
		int relatedID = result.GetInt("related_id");
		wxClassInfo* elementClass = wxClassInfo::FindClass(result.GetAsString("element_class"));
		wxClassInfo* relatedClass = wxClassInfo::FindClass(result.GetAsString("related_class"));

		wxVector<am::StoryElement*> vElements = GetAllElements();

		for ( am::StoryElement*& pElementInVector : vElements )
		{
			if ( pElementInVector->IsKindOf(elementClass) && pElementInVector->dbID == elementID )
			{
				pElement = pElementInVector;
				break;
			}
		}
	
		for ( am::StoryElement*& pRelatedInVector : vElements )
		{
			if ( pRelatedInVector->IsKindOf(relatedClass) && pRelatedInVector->dbID == relatedID )
			{
				pRelated = pRelatedInVector;
				break;
			}
		}

		if ( pElement && pRelated )
			am::RelateStoryElements(pElement, pRelated, false);
	}

	for ( am::StoryElement*& pElement : GetAllElements() )
	{
		if ( !pElement->relatedElements.empty() )
			std::sort(pElement->relatedElements.begin(), pElement->relatedElements.end(), amSortElements);
	}
}

void am::LoadCharacterRelations()
{}

bool am::LoadLastSession()
{
	wxSQLite3ResultSet result = pDatabase->ExecuteQuery("SELECT * FROM last_session;");
	if ( result.NextRow() )
	{
		wxStringInputStream sstream(result.GetAsString("recent_documents"));
		wxXmlDocument doc(sstream);

		wxXmlNode* pRootNode = doc.GetRoot();
		
		wxXmlNode* pBookNode = pRootNode->GetChildren();
		while ( pBookNode )
		{
			Book* pBook = GetBookById(wxAtoi(pBookNode->GetAttribute("id")));
			wxXmlNode* pDocNode = pBookNode->GetChildren();
			
			while ( pDocNode )
			{
				pBook->PushRecentDocument(GetDocumentById(wxAtoi(pDocNode->GetAttribute("id"))));
				pDocNode = pDocNode->GetNext();
			}

			pBookNode = pBookNode->GetNext();
		}

		wxStringInputStream attrStream(result.GetAsString("session_attributes"));
		wxXmlDocument attrDoc(attrStream);

		SessionAttributes attr;
		attr.DeserializeObject(attrDoc.GetRoot()->GetChildren());
		LoadSessionAttr(attr);

		Book* pSelectedBook = GetBookById(result.GetInt("selected_book_id"));
		if ( pSelectedBook )
			SetCurrentBook(pSelectedBook);

		return true;
	}

	return false;
}

void am::LoadSessionAttr(const SessionAttributes& attr)
{
	pMainFrame->Layout();

	if ( attr.nMainFrameSashPos != -1 )
		pMainFrame->GetMainSplitter()->SetSashPosition(attr.nMainFrameSashPos);
	
	if ( attr.nCharacterSashPos != -1 )
		pElementNotebook->m_characterPage->SetSashPosition(attr.nCharacterSashPos, false);

	if ( attr.nLocationSashPos != -1 )	
		pElementNotebook->m_locationPage->SetSashPosition(attr.nLocationSashPos, false);

	if ( attr.nItemSashPos != -1 )
		pElementNotebook->m_itemPage->SetSashPosition(attr.nItemSashPos, false);

	if ( attr.nOutlineFilesSashPos != -1 )
		pOutline->GetOutlineFiles()->SetSashPosition(attr.nOutlineFilesSashPos, false);
}

void am::SetNewProjectPath(const wxFileName& path)
{
	project.amFile.Assign(path);
}

void am::SetExecutablePath(const wxString& path)
{
	executablePath.Assign(path);
}

bool am::SetCurrentBook(int bookPos) 
{
	return SetCurrentBook(project.books[bookPos - 1]); 
}

bool am::SetCurrentBook(am::Book* book)
{
	if ( !book )
		return false;

	pOverview->SetBookData(book);

	pElementNotebook->UpdateAll();
	amElementShowcase* showcase = pElementNotebook->GetCharacterShowcase();
	showcase->SetData(showcase->GetElement());
	showcase = pElementNotebook->GetLocationShowcase();
	showcase->SetData(showcase->GetElement());
	showcase = pElementNotebook->GetItemShowcase();
	showcase->SetData(showcase->GetElement());

	pStoryNotebook->SetBookData(book);
	pRelease->SetBookData(book);

	pCurrentBook = book;
	return true;
}

void am::StartCreatingBook()
{
	amBookWizard wizard(pMainFrame);
	wizard.RunWizard(wizard.GetFirstPage());
}

amMainFrame* am::GetMainFrame()
{
	return pMainFrame;
}

amElementNotebook* am::GetElementsNotebook()
{
	return pElementNotebook;
}

amStoryNotebook* am::GetStoryNotebook()
{
	return pStoryNotebook;
}

amOutline* am::GetOutline()
{
	return pOutline;
}

amRelease* am::GetRelease()
{
	return pRelease;
}

wxString am::GetPath(bool withSeparator)
{
	if ( withSeparator )
		return project.amFile.GetPathWithSep();
	else
		return project.amFile.GetPath();
}

wxString am::GetFullPath()
{
	return project.amFile.GetFullPath();
}

wxString am::GetExecutablePath(bool withSeparator)
{
	return executablePath.GetPath(withSeparator);
}

wxString am::GetTitle()
{
	return project.amFile.GetName();
}

void am::ClearPath()
{
	project.amFile.Clear();
}

void am::SetLastSavePath()
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

wxString am::GetLastSavePath()
{
	// Nothing special here, just reads the 88165468 file and, if succesful, calls the load function.
	// Else, just clear the paths and load a standard new project.
	std::ifstream last(GetExecutablePath(true).ToStdString() + "88165468", std::ios::binary | std::ios::in);
	wxString strPath;

	if ( last.is_open() )
	{
		int size;
		char* data;

		last.read((char*)&size, sizeof(int));
		data = new char[size];
		last.read(data, size);
		strPath = data;
		delete[] data;

		last.close();
	}

	if ( wxFileName::Exists(strPath) )
	{
		return strPath;
	}
	else
	{
		return "";
	}
}

int am::GetSQLEntryId(am::SQLEntry& sqlEntry)
{
	int id = -1;
	try
	{
		id = pDatabase->GetSQLEntryId(sqlEntry);
	}
	catch ( wxString& e )
	{
		wxMessageBox("There was an issue when saving - " + e);
	}

	return id;
}

void am::InsertSQLEntry(am::SQLEntry& sqlEntry)
{
	pDatabase->InsertSQLEntry(sqlEntry);
}

void am::OpenDocument(int documentIndex)
{
	OpenDocument(GetCurrentBook()->documents[documentIndex]);
}

void am::OpenDocument(am::Document* document)
{
	if ( pStoryWriter )
	{
		pStoryWriter->LoadDocument(document);
		pStoryWriter->Maximize();
	}
	else
	{
		pStoryWriter = new amStoryWriter(pMainFrame, document);

		if ( pMainFrame->IsFullScreen() )
			pStoryWriter->ToggleFullScreen(true);
	}
}

bool am::ScanForDocumentLinear(int toFind, int& current, am::Document* scanBegin, am::Document*& emptyPointer)
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
	for ( am::Document*& pChild : scanBegin->children )
	{
		if ( ScanForDocumentLinear(toFind, current, pChild, emptyPointer) )
			return true;
	}

	return false;
}

void am::NullifyStoryWriter()
{
	pStoryWriter = nullptr;
}

void am::AddBook(am::Book* book)
{
	if ( book->pos < project.books.size() )
	{
		for ( int i = project.books.size() - 1; i >= 0; i-- )
		{
			am::Book*& pBook = project.books[i];

			pBook->pos++;
			pBook->Update(pDatabase, false);

			if ( pBook != book && pBook->pos == book->pos + 1 )
			{
				project.books.insert(&pBook, book);
				break;
			}
		}
	}
	else
		project.books.push_back(book);

	book->Save(pDatabase);

	if ( bIsInitialized )
	{
		pOverview->LoadBookContainer();
		pElementNotebook->InitShowChoices();
	}
}

void am::AddStoryElement(am::StoryElement* element, bool refreshElements)
{
	pDatabase->Begin();
	element->Save(pDatabase);
	pDatabase->Commit();

	project.vStoryElements.push_back(element);
	ResortElements();

	if ( refreshElements )
	{
		pElementNotebook->GetAppropriatePage(element)->UpdateList();
		pElementNotebook->UpdateSearchAutoComplete(wxBookCtrlEvent());
	}

	pOutline->GetOutlineFiles()->AppendStoryElement(element);
}

/// <summary>
/// Add new am::Document to selected book and in specified section.
/// </summary>
/// <param name="document">Reference to document object</param>
/// <param name="book">Reference to desired book object</param>
/// <param name="sectionPos">Section position in Book (First section is number 1!)</param>
/// <param name="pos">Where in the section will the document be inserted</param>
void am::AddDocument(am::Document* document, am::Book* book, int pos)
{
	document->book = book;
	pStoryNotebook->AddDocument(document, pos);

	document->Save(pDatabase);

	if ( pos < book->documents.size() + 1 && pos > -1 )
	{
		am::Document** documentIt = book->documents.begin();
		for ( int i = 0; i < pos; i++ )
			documentIt++;

		book->documents.insert(documentIt, document);

		pDatabase->Begin();
		for ( int i = 0; i < book->documents.size(); i++ )
		{
			book->documents[i]->position = i + 1;
			book->documents[i]->Update(pDatabase, false, false);
		}
		pDatabase->Commit();
	}
	else
	{
		book->documents.push_back(document);
	}
}

void am::ResortElements()
{
	std::sort(project.vStoryElements.begin(), project.vStoryElements.end(), amSortElements);

	for ( am::StoryElement*& pElement : project.vStoryElements )
	{
		std::sort(pElement->relatedElements.begin(), pElement->relatedElements.end(), amSortElements);
	}

	for ( Book*& pBook : project.books )
	{
		for ( am::Document*& pDocument : pBook->documents )
		{
			std::sort(pDocument->vTangibleElements.begin(), pDocument->vTangibleElements.end(), amSortElements);
		}
	}
}

void am::StartEditingElement(am::StoryElement* element)
{
	wxString strCreatorClass("am" + wxString(element->GetClassInfo()->GetClassName()).AfterLast(':') + "Creator");

	wxClassInfo* pCreatorClass = wxClassInfo::FindClass(strCreatorClass);
	amElementCreator* pCreator = (amElementCreator*)pCreatorClass->CreateObject();
	if ( !pCreator->Create(pMainFrame, -1, "Edit " + element->name, wxDefaultPosition, pCreator->GetBestSize()) )
		return;
	
	pCreator->CenterOnParent();
	pCreator->StartEditing(element);
	pCreator->Show(true);
}

void am::DoEditStoryElement(am::StoryElement* original, am::StoryElement& edit, bool sort)
{
	wxASSERT(original->GetClassInfo() == edit.GetClassInfo());

	try
	{
		pOutline->GetOutlineFiles()->DeleteStoryElement(original);
		original->EditTo(edit);

		am::UpdateStoryElement(original);

		if ( sort )
		{
			ResortElements();
			pElementNotebook->GetAppropriatePage(original)->UpdateList();
		}

		am::UpdateStoryElementInGUI(original);

		pOutline->GetOutlineFiles()->AppendStoryElement(original);

	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

void am::UpdateStoryElement(StoryElement* element)
{
	std::thread thread([element]() { element->Update(pDatabase); });
	thread.detach();
}

void am::UpdateStoryElementInGUI(am::StoryElement* element)
{
	bool bShouldShowInNotebook = pElementNotebook->ShouldShow(element);
	amElementNotebookPage* pPage = pElementNotebook->GetAppropriatePage(element);

	int n = pPage->GetList()->FindItem(0, element->name);
	if ( n != -1 )
	{
		if ( !bShouldShowInNotebook )
			pElementNotebook->RemoveElementFromList(element);
		else
			pElementNotebook->UpdateElementInList(n, element);
	}
	else
	{
		if ( bShouldShowInNotebook )
			pPage->UpdateList();
	}

	if ( pPage->GetShowcase()->GetElement() == element )
		pPage->GetShowcase()->SetData(element);

	if ( pStoryWriter )
	{
		if ( element->IsKindOf(wxCLASSINFO(am::Character)) )
			pStoryWriter->UpdateCharacterList();
		else if ( element->IsKindOf(wxCLASSINFO(am::Location)) )
			pStoryWriter->UpdateLocationList();
		else if ( element->IsKindOf(wxCLASSINFO(am::Item)) )
			pStoryWriter->UpdateItemList();
	}
}

void am::GoToStoryElement(am::StoryElement* element)
{
	if ( !element )
		return;

	pMainFrame->GetSimplebook()->SetSelection(1);
	pElementNotebook->GoToStoryElement(element);

	if ( pStoryWriter )
		pStoryWriter->Iconize();
}

void am::AddElementToDocument(am::TangibleElement* element, am::Document* document, bool addToDb)
{
	if ( !element || !document )
		return;

	bool had = false;
	for ( am::Document*& pDocInElement : element->documents )
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
				wxString update("INSERT INTO elements_documents (element_id, document_id, element_class) VALUES (");
				update << element->dbID << ", " << document->id << ", '" << element->GetClassInfo()->GetClassName() << "');";

				pDatabase->ExecuteUpdate(update);
			}
			catch ( wxSQLite3Exception& e )
			{
				wxMessageBox(e.GetMessage());
			}
		}
	}

	had = false;
	for ( am::TangibleElement*& pElementInDoc : document->vTangibleElements )
	{
		if ( pElementInDoc == element )
		{
			had = true;
			break;
		}
	}

	if ( !had )
	{
		std::sort(document->vTangibleElements.begin(), document->vTangibleElements.end(), amSortElements);
		document->vTangibleElements.push_back(element);
	}

	UpdateStoryElementInGUI(element);
}

void am::RemoveElementFromDocument(am::TangibleElement* element, am::Document * document)
{
	if ( !element || !document )
		return;

	for ( am::Document*& pDocInElement : element->documents )
	{
		if ( pDocInElement == document )
		{
			element->documents.erase(&pDocInElement);

			wxString update("DELETE FROM elements_documents WHERE element_id = ");
			update << element->dbID << " AND document_id = " << document->id <<
				" AND element_class = '" << element->GetClassInfo()->GetClassName() << "';";

			pDatabase->ExecuteUpdate(update);
			break;
		}
	}

	for ( am::TangibleElement*& pElementInDoc : document->vTangibleElements)
	{
		if ( pElementInDoc == element)
		{
			document->vTangibleElements.erase(&pElementInDoc);
			break;
		}
	}

	UpdateStoryElementInGUI(element);
}

void am::RelateStoryElements(am::StoryElement* element1, am::StoryElement* element2, bool addToDb)
{
	bool had1 = false;
	for ( am::StoryElement*& pPresent : element1->relatedElements )
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
	for ( am::StoryElement*& pPresent : element2->relatedElements )
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

	if ( addToDb && !had1 && !had2 )
	{
		wxString update("INSERT INTO elements_elements (element_id, related_id, element_class, related_class) VALUES (");
		update << element1->dbID << "," << element2->dbID << ", '" <<
			element1->GetClassInfo()->GetClassName() << "', '" << element2->GetClassInfo()->GetClassName() << "');";

		pDatabase->ExecuteUpdate(update);
	}

	UpdateStoryElementInGUI(element1);
	UpdateStoryElementInGUI(element2);
}

void am::UnrelateStoryElements(am::StoryElement* element1, am::StoryElement* element2, bool removeFromDb)
{
	bool had1 = false;
	for ( am::StoryElement*& pPresent : element1->relatedElements )
	{
		if ( pPresent == element2 )
		{
			element1->relatedElements.erase(&pPresent);
			had1 = true;
			break;
		}
	}

	bool had2 = false;
	for ( am::StoryElement*& pPresent : element2->relatedElements )
	{
		if ( pPresent == element1 )
		{
			element2->relatedElements.erase(&pPresent);
			had2 = true;
			break;
		}
	}

	if ( removeFromDb && !had1 && !had2 )
	{
		wxString update("DELETE FROM elements_elements WHERE element_id = ");
		update << element1->dbID << " AND related_id = " << element2->dbID <<
			" AND element_class = '" << element1->GetClassInfo()->GetClassName() << 
			"' AND related_class = '" << element2->GetClassInfo()->GetClassName() << "';";

		pDatabase->ExecuteUpdate(update);

		update = "DELETE FROM elements_elements WHERE element_id = ";
		update << element2->dbID << " AND related_id = " << element1->dbID <<
			" AND element_class = '" << element2->GetClassInfo()->GetClassName() <<
			"' AND related_class = '" << element1->GetClassInfo()->GetClassName() << "';";

		pDatabase->ExecuteUpdate(update);
	}

	UpdateStoryElementInGUI(element1);
	UpdateStoryElementInGUI(element2);
}

am::StoryElement* am::GetStoryElementByName(const wxString& name)
{
	for ( am::StoryElement* pElement : GetAllElements() )
	{
		if ( pElement->name == name )
			return pElement;
	}

	return nullptr;
}

am::Book* am::GetBookByName(const wxString& name)
{
	for ( Book*& pBook : GetBooks() )
	{
		if ( pBook->title == name )
			return pBook;
	}

	return nullptr;
}

void am::DeleteElement(am::StoryElement* element)
{
	if ( !element )
		return;

	wxBusyCursor cursor;

	am::TangibleElement* pTangible = dynamic_cast<am::TangibleElement*>(element);
	if ( pTangible )
	{
		for ( am::Document*& pDocument : pTangible->documents )
			RemoveElementFromDocument(pTangible, pDocument);
	}

	if ( element->IsKindOf(wxCLASSINFO(am::Character)) )
	{
		for ( amTLThread*& thread : pOutline->GetTimeline()->GetThreads() )
		{
			if ( thread->GetCharacter() == element->name )
				pOutline->GetTimeline()->DeleteThread(thread, false);
		}
	}

	am::SQLEntry sqlEntry = element->GenerateSQLEntry();
	for ( am::SQLEntry& childEntry : sqlEntry.childEntries )
	{
		pDatabase->DeleteSQLEntry(childEntry);
	}

	pDatabase->DeleteSQLEntry(sqlEntry);
	for ( am::StoryElement*& pElement : project.vStoryElements )
	{
		if ( pElement == element )
		{
			project.vStoryElements.erase(&pElement);
			delete element;
			break;
		}
	}
}

void am::DeleteDocument(am::Document* document)
{
	for ( am::Document* pChild : document->children )
	{
		DeleteDocument(document);
	}

	for ( am::TangibleElement*& pElement : document->vTangibleElements )
		RemoveElementFromDocument(pElement, document);

	pStoryNotebook->DeleteDocument(document);
	pDatabase->DeleteSQLEntry(document->GenerateSQLEntryForId());

	if ( document->parent )
	{
		for ( am::Document*& pSibling : document->parent->children )
		{
			if ( pSibling == document )
				document->parent->children.erase(&pSibling);
		}
	}

	Book* currentBook = GetCurrentBook();
	for ( am::Document*& pDocInBook : currentBook->documents )
	{
		if ( pDocInBook == document )
		{
			delete pDocInBook;
			currentBook->documents.erase(&pDocInBook);
		}
	}
}

am::Project* am::GetProject()
{
	return &project;
}

int am::GetCurrentBookPos()
{
	return pCurrentBook->pos;
}

am::Book* am::GetCurrentBook()
{
	return pCurrentBook;
}

wxVector<am::Book*>& am::GetBooks()
{
	return project.books;
}

wxVector<am::Character*> am::GetCharacters()
{
	return project.GetCharacters();
}

wxVector<am::Location*> am::GetLocations()
{
	return project.GetLocations();
}

wxVector<am::Item*> am::GetItems()
{
	return project.GetItems();
}

wxVector<am::StoryElement*>& am::GetAllElements()
{
	return project.vStoryElements;
}

wxVector<am::Document*>& am::GetDocumentsInCurrentBook()
{
	return pCurrentBook->documents;
}

am::Document* am::GetDocumentById(int id)
{
	for ( Book*& pBook : project.books )
	{
		for ( am::Document*& pDocument : pBook->documents )
		{
			if ( pDocument->id == id )
				return pDocument;
		}
	}

	return nullptr;
}

am::Book* am::GetBookById(int id)
{
	for ( Book*& pBook : project.books )
	{
		if ( pBook->id == id )
			return pBook;
	}

	return nullptr;
}

wxArrayString am::GetCharacterNames()
{
	wxArrayString names(true);
	for ( am::Character*& pCharacter : project.GetCharacters() )
		names.Add(pCharacter->name);

	return names;
}

wxArrayString am::GetLocationNames()
{
	wxArrayString names(true);
	for ( am::Location*& pLocation : project.GetLocations() )
		names.Add(pLocation->name);

	return names;
}

wxArrayString am::GetItemNames()
{
	wxArrayString names(true);
	for ( am::Item*& pItem : project.GetItems() )
		names.Add(pItem->name);

	return names;
}

wxArrayString am::GetBookTitles()
{
	wxArrayString names;
	for ( Book*& book : project.books )
		names.Add(book->title);

	return names;
}

unsigned int am::GetBookCount()
{
	return project.books.size();
}

unsigned int am::GetDocumentCount(int bookPos)
{
	return project.books[bookPos - 1]->documents.size();
}

unsigned int am::GetCharacterCount()
{
	return project.GetCharacters().size();
}

unsigned int am::GetLocationCount()
{
	return project.GetLocations().size();
}

unsigned int am::GetItemCount()
{
	return project.GetItems().size();
}