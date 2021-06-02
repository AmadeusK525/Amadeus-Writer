#include "BookElements.h"

#include <wx\sstream.h>
#include <wx\mstream.h>

#include <sstream>

#include "MyApp.h"

#include "wxmemdbg.h"


/////////////////////////////////////////////////////////////////
/////////////////////////// Document ////////////////////////////
/////////////////////////////////////////////////////////////////


Document::~Document()
{
	for ( TangibleElement*& pElement : vTangibleElements )
	{
		for ( Document*& pDocument : pElement->documents )
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

bool Document::IsStory()
{
	return type == Document_Chapter || type == Document_Scene || type == Document_Other;
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

int Document::GetWordCount()
{
	if ( bIsDirty )
	{
		amProjectSQLDatabase* pDb = amGetManager()->GetStorage();
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

void Document::Save(wxSQLite3Database* db)
{
	try
	{
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

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

void Document::LoadSelf(wxSQLite3Database* db, wxRichTextCtrl* targetRtc)
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

void Document::CleanUp()
{
	for ( Note*& pNote : notes )
	{
		delete pNote;
	}
	notes.clear();

	buffer = nullptr;
}

amSQLEntry Document::GenerateSQLEntrySimple()
{
	amSQLEntry sqlEntry;
	sqlEntry.tableName = "documents";
	sqlEntry.name = name;

	sqlEntry.integers.reserve(2);
	sqlEntry.integers["position"] = position;
	sqlEntry.integers["book_id"] = book->id;

	sqlEntry.strings["synopsys"] = synopsys;

	return sqlEntry;
}

amSQLEntry Document::GenerateSQLEntry()
{
	amSQLEntry sqlEntry = GenerateSQLEntrySimple();

	sqlEntry.strings["content"] = wxString();


	for ( Note*& pNote : notes )
	{
		amSQLEntry noteDoc;
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

amSQLEntry Document::GenerateSQLEntryForId()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "documents";

	sqlEntry.specialForeign = true;
	sqlEntry.foreignKey.first = "book_id";
	sqlEntry.foreignKey.second = book->id;

	sqlEntry.integers["position"] = position;

	return sqlEntry;
}

bool Document::operator<(const Document& other) const
{
	if ( book->pos == other.book->pos )
		return position < other.position;

	return book->pos < other.book->pos;
}

bool Document::operator==(const Document& other) const
{
	return book->id == other.book->id && position == other.position;
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
	CleanUpDocuments();
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

void Book::CleanUpDocuments()
{
	for ( Document*& pDocument : documents )
		delete pDocument;

	documents.clear();
}

void Book::PushRecentDocument(Document* document)
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

	sqlEntry.integers["position"] = pos;

	sqlEntry.strings["synopsys"] = synopsys;
	sqlEntry.strings["author"] = author;
	sqlEntry.strings["genre"] = genre;
	sqlEntry.strings["description"] = description;

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
	for ( StoryElement*& pElement : vStoryElements )
		delete pElement;

	for ( Book*& pBook : books )
		delete pBook;
}

wxVector<Character*> amProject::GetCharacters()
{
	wxVector<Character*> characters;
	for ( StoryElement*& pElement : vStoryElements )
	{
		Character* pCharacter = dynamic_cast<Character*>(pElement);
		if ( pCharacter )
			characters.push_back(pCharacter);
	}

	return characters;
}

wxVector<Location*> amProject::GetLocations()
{
	wxVector<Location*> locations;
	for ( StoryElement*& pElement : vStoryElements )
	{
		Location* pLocation = dynamic_cast<Location*>(pElement);
		if ( pLocation )
			locations.push_back(pLocation);
	}

	return locations;
}

wxVector<Item*> amProject::GetItems()
{
	wxVector<Item*> items;
	for ( StoryElement*& pElement : vStoryElements )
	{
		Item* pItem = dynamic_cast<Item*>(pElement);
		if ( pItem )
			items.push_back(pItem);
	}

	return items;
}

wxVector<Document*>& amProject::GetDocuments(int bookPos)
{
	return books[bookPos - 1]->documents;
}