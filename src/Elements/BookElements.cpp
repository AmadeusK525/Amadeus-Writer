#include "Elements/BookElements.h"

#include <wx/sstream.h>
#include <wx/mstream.h>

#include <sstream>

#include "MyApp.h"
#include "ProjectManaging.h"

#include "Utils/wxmemdbg.h"


/////////////////////////////////////////////////////////////////
/////////////////////////// Document ////////////////////////////
/////////////////////////////////////////////////////////////////


am::Document::~Document()
{
	for ( TangibleElement*& pElement : vTangibleElements )
	{
		for ( am::Document*& pDocument : pElement->documents )
		{
			if ( pDocument == this )
			{
				pElement->documents.erase(&pDocument);
				break;
			}
		}
	}

	CleanUp();
}

bool am::Document::IsStory()
{
	return type == Document_Chapter || type == Document_Scene || type == Document_Other;
}

bool am::Document::HasRedNote()
{
	for ( Note*& pNote : notes )
	{
		if ( pNote->isDone == false )
			return true;
	}

	return false;
}

int am::Document::GetWordCount()
{
	if ( bIsDirty )
	{
		ProjectSQLDatabase* pDb = GetProjectDatabase();
		wxSQLite3ResultSet result = pDb->ExecuteQuery("SELECT plain_text FROM documents WHERE id = " + std::to_string(id));

		int count = 0;

		if ( result.NextRow() )
		{
			std::string content = result.GetAsString("plain_text");

			if ( !content.empty() )
			{
				std::stringstream sstream(content);
				count = std::distance(std::istream_iterator<std::string>(sstream), std::istream_iterator<std::string>());
			}
		}

		nWordCount = count;
		bIsDirty = false;
	}

	return nWordCount;
}

void am::Document::Save(ProjectSQLDatabase* db)
{
	try
	{
		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO documents (name, synopsys, content, position, type, book_id, isInTrash"
			", parent_document_id, character_pov_id) VALUES (");
		insert << "'%q', '%q', '%q', " << position << ", " << type << ", " << book->id << ", " << isInTrash << ", '%q', '%q');";

		wxString content;
		if ( this->buffer )
		{
			wxStringOutputStream sstream(&content);
			this->buffer->SaveFile(sstream, wxRICHTEXT_TYPE_XML);
		}
		else
			content = "NULL";

		wxString parentId;
		if ( parent )
			parentId = std::to_string(parent->id);
		else
			parentId = "NULL";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)synopsys.ToUTF8(),
			(const char*)content.ToUTF8(), (const char*)parentId, "NULL");

		db->ExecuteUpdate(buffer);
		SetId(db->GetSQLEntryId(GenerateSQLEntryForId()));
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

bool am::Document::Update(ProjectSQLDatabase* db, bool updateContent, bool updateNotes)
{
	if ( id == -1 )
		return false;

	try
	{
		wxSQLite3StatementBuffer buffer;

		wxString update("UPDATE documents SET name = '%q', synopsys = '%q', position = ");

		update << position << ", type = " << (int)type << ", isInTrash = " << isInTrash << ", book_id = " << book->id
			<< ", parent_document_id = ";
		if ( parent )
			update << parent->id;
		else
			update << "NULL";

		if ( updateContent )
		{
			wxString content;
			wxString plainText;
			update << ", content = '%q', plain_text = '%q' WHERE id = " << id << ";";

			if ( this->buffer )
			{
				wxStringOutputStream sstream(&content);
				this->buffer->SaveFile(sstream, wxRICHTEXT_TYPE_XML);
				plainText = this->buffer->GetText();
			}
			else
			{
				content = "NULL";
				plainText = "NULL";
			}

			buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)synopsys.ToUTF8(),
				(const char*)content.ToUTF8(), (const char*)plainText.ToUTF8());
		}
		else
		{
			update << " WHERE id = " << id << ";";
			buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)synopsys.ToUTF8());
		}

		db->ExecuteUpdate(buffer);

		if ( updateNotes )
		{
			wxSQLite3Table customTable = db->GetTable("SELECT * FROM document_notes WHERE document_id = " + std::to_string(id) + ";");

			int prevSize = customTable.GetRowCount();
			int newSize = notes.size();

			if ( newSize > prevSize )
			{
				int i = 0;
				for ( i; i < prevSize; i++ )
				{
					customTable.SetRow(i);

					update = "UPDATE document_notes SET name = '%q', content = '%q', isDone = ";
					update << notes[i]->isDone << ", document_id = " << id << " WHERE id = " << customTable.GetInt("id") << ";";

					buffer.Format((const char*)update, (const char*)notes[i]->name.ToUTF8(), (const char*)notes[i]->content.ToUTF8());
					db->ExecuteUpdate(buffer);
				}

				for ( i; i < newSize; i++ )
				{
					update = "INSERT INTO document_notes (name, content, isDone, document_id) VALUES ('%q', '%q', ";
					update << notes[i]->isDone << ", " << id << ");";

					buffer.Format((const char*)update, (const char*)notes[i]->name.ToUTF8(), (const char*)notes[i]->content.ToUTF8());
					db->ExecuteUpdate(buffer);
				}
			}
			else
			{
				int i = 0;
				for ( i; i < newSize; i++ )
				{
					customTable.SetRow(i);

					update = "UPDATE document_notes SET name = '%q', content = '%q', isDone = ";
					update << notes[i]->isDone << ", document_id = " << id << " WHERE id = " << customTable.GetInt("id") << ";";

					buffer.Format((const char*)update, (const char*)notes[i]->name.ToUTF8(), (const char*)notes[i]->content.ToUTF8());
					db->ExecuteUpdate(buffer);
				}

				if ( newSize < prevSize )
					for ( i; i < prevSize; i++ )
					{
						customTable.SetRow(i);
						update = "DELETE FROM document_notes WHERE id = ";
						update << customTable.GetInt("id") << ";";

						db->ExecuteUpdate(update);
					}
			}
		}

	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return true;
}

void am::Document::LoadSelf(ProjectSQLDatabase* db, wxRichTextCtrl* targetRtc)
{
	wxSQLite3ResultSet result = db->ExecuteQuery("SELECT * FROM document_notes WHERE document_id = " + std::to_string(id) + ";");

	while ( result.NextRow() )
	{
		Note* pNote = new Note(result.GetAsString("content"), result.GetAsString("name"));
		pNote->isDone = result.GetInt("isDone");

		notes.push_back(pNote);
	}

	result = db->ExecuteQuery("SELECT content FROM documents WHERE id = " + std::to_string(id) + ";");
	if ( result.NextRow() )
	{
		buffer = &targetRtc->GetBuffer();
		wxString content = result.GetAsString("content");

		if ( !content.IsEmpty() && !(content == "NULL") )
		{
			wxStringInputStream sstream(content);
			buffer->LoadFile(sstream, wxRICHTEXT_TYPE_XML);
		}
	}
}

void am::Document::CleanUp()
{
	for ( Note*& pNote : notes )
	{
		delete pNote;
	}
	notes.clear();

	buffer = nullptr;
}

am::SQLEntry am::Document::GenerateSQLEntrySimple()
{
	am::SQLEntry sqlEntry;
	sqlEntry.tableName = "documents";
	sqlEntry.name = name;

	sqlEntry.integers.reserve(2);
	sqlEntry.integers["position"] = position;
	sqlEntry.integers["book_id"] = book->id;

	sqlEntry.strings["synopsys"] = synopsys;

	return sqlEntry;
}

am::SQLEntry am::Document::GenerateSQLEntry()
{
	am::SQLEntry sqlEntry = GenerateSQLEntrySimple();

	sqlEntry.strings["content"] = wxString();


	for ( Note*& pNote : notes )
	{
		am::SQLEntry noteDoc;
		noteDoc.tableName = "document_notes";

		noteDoc.name = pNote->name;
		noteDoc.strings["content"] = pNote->content;

		noteDoc.integers["isDone"] = pNote->isDone;

		noteDoc.specialForeign = true;
		noteDoc.foreignKey.first = "document_id";

		sqlEntry.childEntries.push_back(noteDoc);
	}

	return sqlEntry;
}

am::SQLEntry am::Document::GenerateSQLEntryForId()
{
	am::SQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "documents";

	sqlEntry.specialForeign = true;
	sqlEntry.foreignKey.first = "book_id";
	sqlEntry.foreignKey.second = book->id;

	sqlEntry.integers["position"] = position;

	return sqlEntry;
}

bool am::Document::operator<(const am::Document& other) const
{
	if ( book->pos == other.book->pos )
		return position < other.position;

	return book->pos < other.book->pos;
}

bool am::Document::operator==(const am::Document& other) const
{
	return book->id == other.book->id && position == other.position;
}

am::Note::Note(wxString content, wxString name)
{
	this->content = content;
	this->name = name;
}

am::SQLEntry am::Note::GenerateSQLEntry()
{

	return am::SQLEntry();
}


/////////////////////////////////////////////////////////////////
///////////////////////////// Book //////////////////////////////
/////////////////////////////////////////////////////////////////


wxSize am::Book::coverSize{ 660, 900 };

am::Book::~Book()
{
	CleanUpDocuments();
}

bool am::Book::Init()
{
	if ( id == -1 )
		return false;

	InitCover();
	return true;
}

void am::Book::InitCover()
{
	wxBitmap bitmapCover(cover);
	wxMemoryDC dc(bitmapCover);
	dc.SetFont(wxFontInfo(64).Bold());
	dc.SetTextForeground(wxColour(255, 255, 255));

	wxString textToDraw = title;
	wxSize textSize;

	int begin = 0;
	int end = 0;
	int strLen = title.Length();

	bool first = true;
	int maxChar = 0;

	while ( end < strLen )
	{
		if ( end >= begin + maxChar || first )
		{
			end += 2;
		}
		else
			end += maxChar;

		if ( end > strLen )
			end = strLen;

		dc.GetMultiLineTextExtent(textToDraw.SubString(begin, end), &textSize.x, &textSize.y);

		if ( textSize.x >= coverSize.x - 40 )
		{
			if ( first )
			{
				maxChar = end - 1;
				first = false;
			}

			size_t found = textToDraw.rfind(" ", end);

			if ( found != std::string::npos )
			{
				textToDraw.replace(found, 1, "\n");
				begin = found;
			}
			else
			{
				textToDraw.replace(begin + maxChar, 2, "-\n");
				begin += maxChar + 1;
			}

			end = begin;
		}
		else if ( end >= strLen )
		{
			textToDraw.Remove(end);
			dc.GetMultiLineTextExtent(textToDraw, &textSize.x, &textSize.y);
			break;
		}
	}

	dc.DrawText(textToDraw, 10, (coverSize.y / 2) - (textSize.y / 2));
	cover = bitmapCover.ConvertToImage();
}

void am::Book::CleanUpDocuments()
{
	for ( am::Document*& pDocument : documents )
		delete pDocument;

	documents.clear();
}

void am::Book::PushRecentDocument(am::Document* document)
{
	if ( document->book != this || (!vRecentDocuments.empty() && vRecentDocuments.back() == document) )
		return;

	for ( auto it = vRecentDocuments.begin(); it != vRecentDocuments.end(); it++ )
	{
		if ( *it == document )
		{
			vRecentDocuments.erase(it);
			vRecentDocuments.push_back(document);
			return;
		}
	}

	if ( vRecentDocuments.size() > 10 )
	{
		vRecentDocuments.erase(vRecentDocuments.begin());
	}

	vRecentDocuments.push_back(document);
	return;
}

void am::Book::Save(ProjectSQLDatabase* db)
{
	try
	{
		wxSQLite3StatementBuffer buffer;
		bool doCover = cover.IsOk();

		wxString insert("INSERT INTO books (name, author, genre, description, synopsys, position, cover) VALUES ("
			"'%q', '%q',  '%q', '%q', '%q', ");
		insert << pos;

		if ( doCover )
			insert << ", ?);";
		else
			insert << ", NULL);";

		buffer.Format((const char*)insert, (const char*)title.ToUTF8(), (const char*)author.ToUTF8(),
			(const char*)genre.ToUTF8(), (const char*)description.ToUTF8(), (const char*)synopsys.ToUTF8());

		wxSQLite3Statement statement = db->PrepareStatement(buffer);

		if ( doCover )
		{
			wxMemoryOutputStream stream;

			cover.SaveFile(stream, wxBITMAP_TYPE_PNG);

			wxMemoryBuffer membuffer;
			membuffer.SetBufSize(stream.GetSize());
			membuffer.SetDataLen(stream.GetSize());
			stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

			statement.Bind(1, membuffer);
		}

		statement.ExecuteUpdate();
		SetId(db->GetSQLEntryId(GenerateSQLEntryForId()));

		Init();
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

bool am::Book::Update(am::ProjectSQLDatabase* db, bool updateDocuments)
{
	if ( id == -1 )
		return false;

	try
	{
		ProjectSQLDatabase* storage = (ProjectSQLDatabase*)db;

		wxSQLite3StatementBuffer buffer;
		bool doCover = cover.IsOk();

		wxString update("UPDATE books SET name = '%q', author = '%q', genre = '%q', description = '&q', synopsys = '%q', position = ");
		update << pos << ", cover = ";

		if ( doCover )
			update << "?";
		else
			update << "NULL";

		update << "WHERE id = " << id;

		buffer.Format((const char*)update, (const char*)title.ToUTF8(), (const char*)author.ToUTF8(),
			(const char*)genre.ToUTF8(), (const char*)description.ToUTF8(), (const char*)synopsys.ToUTF8());

		wxSQLite3Statement statement = storage->PrepareStatement(buffer);


		if ( doCover )
		{
			wxMemoryOutputStream stream;

			cover.SaveFile(stream, wxBITMAP_TYPE_PNG);

			wxMemoryBuffer membuffer;
			membuffer.SetBufSize(stream.GetSize());
			membuffer.SetDataLen(stream.GetSize());
			stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

			statement.Bind(1, membuffer);
		}

		statement.ExecuteUpdate();

		if ( updateDocuments )
		{
			for ( am::Document*& pDocument : documents )
			{
				pDocument->Update(db, true, true);
			}
		}
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return true;
}

am::SQLEntry am::Book::GenerateSQLEntrySimple()
{
	am::SQLEntry sqlEntry;
	sqlEntry.name = title;
	sqlEntry.tableName = "books";

	sqlEntry.integers["position"] = pos;

	sqlEntry.strings["synopsys"] = synopsys;
	sqlEntry.strings["author"] = author;
	sqlEntry.strings["genre"] = genre;
	sqlEntry.strings["description"] = description;

	return sqlEntry;
}

am::SQLEntry am::Book::GenerateSQLEntry(wxVector<int>* documentsToInclude)
{
	am::SQLEntry sqlEntry = GenerateSQLEntrySimple();

	if ( !documentsToInclude )
	{
		sqlEntry.childEntries.reserve(documents.size());
		for ( am::Document*& pDocument : documents )
			sqlEntry.childEntries.push_back(pDocument->GenerateSQLEntry());

	}
	else
	{
		int size = documentsToInclude->size();
		sqlEntry.childEntries.reserve(size);

		for ( int& it : *documentsToInclude )
		{
			if ( it < size )
				sqlEntry.childEntries.push_back(documents[it]->GenerateSQLEntry());

		}
	}

	return sqlEntry;
}

am::SQLEntry am::Book::GenerateSQLEntryForId()
{
	am::SQLEntry sqlEntry;
	sqlEntry.name = title;
	sqlEntry.tableName = "books";

	return sqlEntry;
}


/////////////////////////////////////////////////////////////////
//////////////////////////// Project ////////////////////////////
/////////////////////////////////////////////////////////////////


am::Project::~Project()
{
	for ( am::StoryElement*& pElement : vStoryElements )
		delete pElement;

	for ( am::Book*& pBook : books )
		delete pBook;
}

wxVector<am::Character*> am::Project::GetCharacters()
{
	wxVector<am::Character*> characters;
	for ( am::StoryElement*& pElement : vStoryElements )
	{
		am::Character* pCharacter = dynamic_cast<am::Character*>(pElement);
		if ( pCharacter )
			characters.push_back(pCharacter);
	}

	return characters;
}

wxVector<am::Location*> am::Project::GetLocations()
{
	wxVector<am::Location*> locations;
	for ( am::StoryElement*& pElement : vStoryElements )
	{
		am::Location* pLocation = dynamic_cast<am::Location*>(pElement);
		if ( pLocation )
			locations.push_back(pLocation);
	}

	return locations;
}

wxVector<am::Item*> am::Project::GetItems()
{
	wxVector<am::Item*> items;
	for ( am::StoryElement*& pElement : vStoryElements )
	{
		am::Item* pItem = dynamic_cast<am::Item*>(pElement);
		if ( pItem )
			items.push_back(pItem);
	}

	return items;
}

wxVector<am::Document*>& am::Project::GetDocuments(int bookPos)
{
	return books[bookPos - 1]->documents;
}