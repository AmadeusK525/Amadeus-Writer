#include "BookElements.h"

#include <wx\sstream.h>
#include <wx\mstream.h>

#include "MyApp.h"

#include "wxmemdbg.h"


/////////////////////////////////////////////////////////////////
/////////////////////////// Document ////////////////////////////
/////////////////////////////////////////////////////////////////


Document::~Document()
{
	for ( Note*& pNote : notes )
		delete pNote;

	if ( buffer )
		delete buffer;
}

bool Document::HasRedNote()
{
	for ( Note*& pNote : notes )
	{
		if ( pNote->isDone == false )
			return true;
	}

	return false;
}

void Document::Save(wxSQLite3Database* db)
{
	try
	{
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO documents (name, synopsys, content, position, type, book_id, isInTrash"
			", parent_document_id, character_pov_id) VALUES (");
		insert << "'%q', '%q', '%q', " << position << ", " << type << ", " << bookID << ", " << isInTrash << ", '%q', '%q');";

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

		storage->ExecuteUpdate(buffer);
		SetId(storage->GetSQLEntryId(GenerateSQLEntryForId()));
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

bool Document::Update(wxSQLite3Database* db, bool updateContent, bool updateNotes)
{
	if ( id == -1 )
		return false;

	try
	{
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

		wxSQLite3StatementBuffer buffer;

		wxString update("UPDATE documents SET name = '%q', synopsys = '%q', position = ");

		update << position << ", type = " << (int)type << ", isInTrash = " << isInTrash << ", book_id = " << bookID
			<< ", parent_document_id = ";
		if ( parent )
			update << parent->id;
		else
			update << "NULL";

		if ( updateContent )
		{
			wxString content;
			update << ", content = '%q' WHERE id = " << id << ";";

			if ( this->buffer )
			{
				wxStringOutputStream sstream(&content);
				this->buffer->SaveFile(sstream, wxRICHTEXT_TYPE_XML);
			}
			else
			{
				content = "NULL";
			}

			buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)synopsys.ToUTF8(),
				(const char*)content.ToUTF8());
		}
		else
		{
			update << " WHERE id = " << id << ";";
			buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)synopsys.ToUTF8());
		}

		storage->ExecuteUpdate(buffer);

		if ( updateNotes )
		{
			wxSQLite3Table customTable = storage->GetTable("SELECT * FROM document_notes WHERE document_id = " + std::to_string(id) + ";");

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
					storage->ExecuteUpdate(buffer);
				}

				for ( i; i < newSize; i++ )
				{
					update = "INSERT INTO document_notes (name, content, isDone, document_id) VALUES ('%q', '%q', ";
					update << notes[i]->isDone << ", " << id << ");";

					buffer.Format((const char*)update, (const char*)notes[i]->name.ToUTF8(), (const char*)notes[i]->content.ToUTF8());
					storage->ExecuteUpdate(buffer);
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
					storage->ExecuteUpdate(buffer);
				}

				if ( newSize < prevSize )
					for ( i; i < prevSize; i++ )
					{
						customTable.SetRow(i);
						update = "DELETE FROM document_notes WHERE id = ";
						update << customTable.GetInt("id") << ";";

						storage->ExecuteUpdate(update);
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

amSQLEntry Document::GenerateSQLEntrySimple()
{
	amSQLEntry sqlEntry;
	sqlEntry.tableName = "documents";
	sqlEntry.name = name;

	sqlEntry.integers.reserve(2);
	sqlEntry.integers.push_back(pair<wxString, int>("position", position));
	sqlEntry.integers.push_back(pair<wxString, int>("book_id", bookID));

	sqlEntry.strings.push_back(pair<wxString, wxString>("synopsys", synopsys));

	return sqlEntry;
}

amSQLEntry Document::GenerateSQLEntry()
{
	amSQLEntry sqlEntry = GenerateSQLEntrySimple();

	sqlEntry.strings.push_back(pair<wxString, wxString>("content", wxString()));


	for ( Note*& pNote : notes )
	{
		amSQLEntry noteDoc;
		noteDoc.tableName = "document_notes";

		noteDoc.name = pNote->name;
		noteDoc.strings.push_back(pair<wxString, wxString>("content", pNote->content));

		noteDoc.integers.push_back(pair<wxString, int>("isDone", pNote->isDone));

		noteDoc.specialForeign = true;
		noteDoc.foreignKey.first = "document_id";

		sqlEntry.childEntries.push_back(noteDoc);
	}

	return sqlEntry;
}

amSQLEntry Document::GenerateSQLEntryForId()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "documents";

	sqlEntry.specialForeign = true;
	sqlEntry.foreignKey.first = "book_id";
	sqlEntry.foreignKey.second = bookID;

	sqlEntry.integers.push_back(pair<wxString, int>("position", position));

	return sqlEntry;
}

bool Document::operator<(const Document& other) const
{
	return position < other.position;
}

bool Document::operator==(const Document& other) const
{
	return bookID == other.bookID && position == other.position;
}

Note::Note(wxString content, wxString name)
{
	this->content = content;
	this->name = name;
}

amSQLEntry Note::GenerateSQLEntry()
{

	return amSQLEntry();
}


/////////////////////////////////////////////////////////////////
///////////////////////////// Book //////////////////////////////
/////////////////////////////////////////////////////////////////


wxSize Book::coverSize{ 660, 900 };

Book::~Book()
{
	for ( Document*& pDocument : documents )
	{
		delete pDocument;
	}
}

bool Book::Init()
{
	if ( id == -1 )
		return false;

	InitCover();
	return true;
}

void Book::InitCover()
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

		if ( textSize.x >= coverSize.x - 20 )
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

void Book::Save(wxSQLite3Database* db)
{
	try
	{
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

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
		SetId(storage->GetSQLEntryId(GenerateSQLEntryForId()));

		Init();
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

bool Book::Update(wxSQLite3Database* db, bool updateDocuments)
{
	if ( id == -1 )
		return false;

	try
	{
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

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
			for ( Document*& pDocument : documents )
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

amSQLEntry Book::GenerateSQLEntrySimple()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = title;
	sqlEntry.tableName = "books";

	sqlEntry.integers.push_back(pair<wxString, int>("position", pos));

	sqlEntry.strings.push_back(pair<wxString, wxString>("synopsys", synopsys));
	sqlEntry.strings.push_back(pair<wxString, wxString>("author", author));
	sqlEntry.strings.push_back(pair<wxString, wxString>("genre", genre));
	sqlEntry.strings.push_back(pair<wxString, wxString>("description", description));

	return sqlEntry;
}

amSQLEntry Book::GenerateSQLEntry(wxVector<int>* documentsToInclude)
{
	amSQLEntry sqlEntry = GenerateSQLEntrySimple();

	if ( !documentsToInclude )
	{
		sqlEntry.childEntries.reserve(documents.size());
		for ( Document*& pDocument : documents )
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

amSQLEntry Book::GenerateSQLEntryForId()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = title;
	sqlEntry.tableName = "books";

	return sqlEntry;
}


/////////////////////////////////////////////////////////////////
//////////////////////////// Project ////////////////////////////
/////////////////////////////////////////////////////////////////


amProject::~amProject()
{
	for ( Character*& pCharacter : characters )
		delete pCharacter;

	for ( Location*& pLocations : locations )
		delete pLocations;

	for ( Item*& pItem : items )
		delete pItem;

	for ( Book*& pBook : books )
		delete pBook;
}

wxVector<Document*>& amProject::GetDocuments(int bookPos)
{
	return books[bookPos - 1]->documents;
}