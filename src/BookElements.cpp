#include "BookElements.h"

#include <wx\sstream.h>

#include "MyApp.h"

/////////////////////////////////////////////////////////////////
///////////////////////////// Scene /////////////////////////////
/////////////////////////////////////////////////////////////////


amDocument Scene::GenerateDocument() {
	amDocument document;
	document.tableName = "scenes";
	document.name = name;

	document.integers.reserve(2);
	document.integers.push_back(pair<wxString, int>("position", pos));
	document.integers.push_back(pair<wxString, int>("chapter_id", chapterID));

	document.strings.push_back(pair<wxString, wxString>("content", wxString()));

	wxStringOutputStream stream(&document.strings.front().second);
	content.SaveFile(stream, wxRICHTEXT_TYPE_XML);

	return document;
}

amDocument Scene::GenerateDocumentForId() {
	amDocument document;
	document.name = name;
	document.tableName = "scenes";

	document.specialForeign = true;
	document.foreignKey.first = "chapter_id";
	document.foreignKey.second = chapterID;

	document.integers.push_back(pair<wxString, int>("position", pos));
	return document;
}


/////////////////////////////////////////////////////////////////
/////////////////////////// Chapter /////////////////////////////
/////////////////////////////////////////////////////////////////


bool Chapter::Init() {
	if (id == -1)
		return false;

	if (scenes.empty())
		scenes.push_back(Scene(id, 1));

	return true;
}

bool Chapter::HasRedNote() {
	for (unsigned int i = 0; i < notes.size(); i++) {
		if (notes[i].isDone == false)
			return true;
	}

	return false;
}

void Chapter::Save(wxSQLite3Database* db) {
	try {
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO chapters (name, synopsys, position, section_id) VALUES (");
		insert << "'%q', '%q', " << position << ", " << sectionID << ");";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)synopsys.ToUTF8());

		storage->ExecuteUpdate(buffer);
		SetId(storage->GetDocumentId(GenerateDocumentForId()));

		Init();
		for (Scene& scene : scenes) {
			insert = "INSERT INTO scenes (name, content, position, chapter_id) VALUES ('%q', '%q', ";
			insert << scene.pos << ", " << id << ");";

			wxStringOutputStream stream;
			scene.content.SaveFile(stream, wxRICHTEXT_TYPE_XML);

			buffer.Format((const char*)insert, (const char*)scene.name.ToUTF8(), (const char*)stream.GetString());
			storage->ExecuteUpdate(buffer);
		}
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}
}

bool Chapter::Update(wxSQLite3Database* db, bool updateScenes, bool updateNotes) {
	if (id == -1)
		return false;

	try {
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

		wxSQLite3StatementBuffer buffer;

		wxString update("UPDATE chapters SET name = '%q', synopsys = '%q', position = ");

		update << position << ", section_id = " << sectionID << " WHERE id = " << id << ";";

		buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)synopsys.ToUTF8());
		storage->ExecuteUpdate(buffer);

		if (updateScenes) {
			wxSQLite3Table customTable = storage->GetTable("SELECT * FROM scenes WHERE chapter_id = " + std::to_string(id));

			int prevSize = customTable.GetRowCount();
			int newSize = scenes.size();

			if (newSize > prevSize) {
				int i = 0;
				for (i; i < prevSize; i++) {
					customTable.SetRow(i);

					update = "UPDATE scenes SET name = '%q', content = '%q', position = ";
					update << scenes[i].pos << ", chapter_id = " << scenes[i].id << " WHERE id = " <<
						customTable.GetInt("id") << ";";

					wxStringOutputStream stream;
					scenes[i].content.SaveFile(stream, wxRICHTEXT_TYPE_XML);

					buffer.Format((const char*)update, (const char*)scenes[i].name.ToUTF8(), (const char*)stream.GetString());
					storage->ExecuteUpdate(buffer);
				}

				for (i; i < newSize; i++) {
					update = "INSERT INTO scenes (name, content, position, chapter_id) VALUES ('%q', '%q', ";
					update << scenes[i].pos << ", " << id << ");";

					wxStringOutputStream stream;
					scenes[i].content.SaveFile(stream, wxRICHTEXT_TYPE_XML);

					buffer.Format((const char*)update, (const char*)scenes[i].name.ToUTF8(), (const char*)stream.GetString());
					storage->ExecuteUpdate(buffer);
				}
			} else {
				int i = 0;
				for (i; i < newSize; i++) {
					customTable.SetRow(i);

					update = "UPDATE scenes SET name = '%q', content = '%q', position = ";
					update << scenes[i].pos << ", chapter_id = " << scenes[i].id << " WHERE id = " <<
						customTable.GetInt("id") << ";";

					wxStringOutputStream stream;
					scenes[i].content.SaveFile(stream, wxRICHTEXT_TYPE_XML);

					buffer.Format((const char*)update, (const char*)scenes[i].name.ToUTF8(), (const char*)stream.GetString());
					storage->ExecuteUpdate(buffer);
				}

				if (newSize < prevSize)
					for (i; i < prevSize; i++) {
						customTable.SetRow(i);

						update = "DELETE FROM scenes WHERE id = ";
						update << customTable.GetInt("id") << ";";

						storage->ExecuteUpdate(update);
					}
			}
		}

		if (updateNotes) {
			wxSQLite3Table customTable = storage->GetTable("SELECT * FROM chapter_notes WHERE chapter_id = " + std::to_string(id));

			int prevSize = customTable.GetRowCount();
			int newSize = notes.size();

			if (newSize > prevSize) {
				int i = 0;
				for (i; i < prevSize; i++) {
					customTable.SetRow(i);

					update = "UPDATE chapter_notes SET name = '%q', content = '%q', isDone = ";
					update << notes[i].isDone << ", chapter_id = " << id << " WHERE id = " << customTable.GetInt("id") << ";";

					buffer.Format((const char*)update, (const char*)notes[i].name.ToUTF8(), (const char*)notes[i].content.ToUTF8());
					storage->ExecuteUpdate(buffer);
				}

				for (i; i < newSize; i++) {
					update = "INSERT INTO chapter_notes (name, content, isDone, chapter_id) VALUES ('%q', '%q', ";
					update << notes[i].isDone << ", " << id << ");";

					buffer.Format((const char*)update, (const char*)notes[i].name.ToUTF8(), (const char*)notes[i].content.ToUTF8());
					storage->ExecuteUpdate(buffer);
				}
			} else {
				int i = 0;
				for (i; i < newSize; i++) {
					customTable.SetRow(i);

					update = "UPDATE chapter_notes SET name = '%q', content = '%q', isDone = ";
					update << notes[i].isDone << ", chapter_id = " << id << " WHERE id = " << customTable.GetInt("id") << ";";

					buffer.Format((const char*)update, (const char*)notes[i].name.ToUTF8(), (const char*)notes[i].content.ToUTF8());
					storage->ExecuteUpdate(buffer);
				}

				if (newSize < prevSize)
					for (i; i < prevSize; i++) {
						customTable.SetRow(i);
						update = "DELETE FROM chapter_notes WHERE id = ";
						update << customTable.GetInt("id") << ";";

						storage->ExecuteUpdate(update);
					}
			}
		}

	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}

	return true;
}

void Chapter::LoadSceneBuffers(wxSQLite3Database* db) {
	wxSQLite3ResultSet result = db->ExecuteQuery("SELECT content FROM scenes WHERE chapter_id = " +
		std::to_string(id) + " ORDER BY position ASC;");

	int i = 0;
	int size = scenes.size();

	while (result.NextRow()) {
		if (i >= size)
			break;

		wxStringInputStream sstream(result.GetAsString("content"));
		scenes[i].content.LoadFile(sstream, wxRICHTEXT_TYPE_XML);
		i++;
	}
}

void Chapter::ClearSceneBuffers() {
	for (Scene& scene : scenes)
		scene.content.Clear();
}

amDocument Chapter::GenerateDocumentSimple() {
	amDocument document;
	document.tableName = "chapters";
	document.name = name;

	document.integers.reserve(2);
	document.integers.push_back(pair<wxString, int>("position", position));
	document.integers.push_back(pair<wxString, int>("section_id", sectionID));

	document.strings.push_back(pair<wxString, wxString>("synopsys", synopsys));

	return document;
}

amDocument Chapter::GenerateDocument() {
	amDocument document = GenerateDocumentSimple();

	document.documents.reserve(scenes.size() + notes.size());

	for (auto& it : scenes)
		document.documents.push_back(it.GenerateDocument());

	for (auto& it : notes) {
		amDocument noteDoc;
		noteDoc.tableName = "chapter_notes";

		noteDoc.name = it.name;
		noteDoc.strings.push_back(pair<wxString, wxString>("content", it.content));

		noteDoc.integers.push_back(pair<wxString, int>("isDone", it.isDone));

		noteDoc.specialForeign = true;
		noteDoc.foreignKey.first = "chapter_id";

		document.documents.push_back(noteDoc);
	}

	return document;
}

amDocument Chapter::GenerateDocumentForId() {
	amDocument document;
	document.name = name;
	document.tableName = "chapters";

	document.specialForeign = true;
	document.foreignKey.first = "section_id";
	document.foreignKey.second = sectionID;

	document.integers.push_back(pair<wxString, int>("position", position));

	return document;
}

bool Chapter::operator<(const Chapter& other) const {
	return position < other.position;
}

bool Chapter::operator==(const Chapter& other) const {
	return sectionID == other.sectionID && position == other.position;
}

Note::Note(wxString content, wxString name) {
	this->content = content;
	this->name = name;
}

amDocument Note::GenerateDocument() {

	return amDocument();
}


/////////////////////////////////////////////////////////////////
//////////////////////////// Section //////////////////////////// 
/////////////////////////////////////////////////////////////////


void Section::Save(wxSQLite3Database* db) {
	try {
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO sections (name, description, position, type, book_id) VALUES ('%q', '%q', ");
		insert << pos << ", " << type << ", " << bookID << ");";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)description.ToUTF8());

		storage->ExecuteUpdate(buffer);
		SetId(storage->GetDocumentId(GenerateDocumentForId()));
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}
}

bool Section::Update(wxSQLite3Database* db, bool updateChapters) {

	amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

	wxSQLite3StatementBuffer buffer;

	wxString update("UPDATE sections SET name = '%q', description = '%q', position = ");
	update << pos << ", type = " << type << ", book_id = " << bookID << " WHERE id = " << id << ";";

	buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)description.ToUTF8());
	storage->ExecuteUpdate(buffer); 

	return false;
}

amDocument Section::GenerateDocumentSimple() {
	amDocument document;
	document.tableName = "sections";
	document.name = name;

	document.integers.reserve(3);
	document.integers.push_back(pair<wxString, int>("book_id", bookID));
	document.integers.push_back(pair<wxString, int>("position", pos));
	document.integers.push_back(pair<wxString, int>("type", type));

	document.strings.push_back(pair<wxString, wxString>("description", description));

	return document;
}

amDocument Section::GenerateDocument() {
	amDocument document = GenerateDocumentSimple();

	document.documents.reserve(chapters.size());
	for (auto& it : chapters)
		document.documents.push_back(it.GenerateDocument());

	return document;
}

amDocument Section::GenerateDocumentForId() {
	amDocument document;
	document.name = name;
	document.tableName = "sections";

	document.specialForeign = true;
	document.foreignKey.first = "book_id";
	document.foreignKey.second = bookID;

	document.integers.push_back(pair<wxString, int>("position", pos));

	return document;
}


/////////////////////////////////////////////////////////////////
///////////////////////////// Book //////////////////////////////
/////////////////////////////////////////////////////////////////


bool Book::Init() {
	if (id == -1)
		return false;

	if (sections.empty())
		sections.push_back(Section(id, 1));

	return true;
}

void Book::Save(wxSQLite3Database* db) {
	try {
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO books (name, author, genre, description, synopsys, position) VALUES ("
			"'%q', '%q',  '%q', '%q', '%q', ");
		insert << pos << ");";

		buffer.Format((const char*)insert, (const char*)title.ToUTF8(), (const char*)author.ToUTF8(),
			(const char*)genre.ToUTF8(), (const char*)description.ToUTF8(), (const char*)synopsys.ToUTF8());

		storage->ExecuteUpdate(buffer);
		SetId(storage->GetDocumentId(GenerateDocumentForId()));

		Init();
		for (auto& it : sections)
			it.Save(storage);

	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}
}

bool Book::Update(wxSQLite3Database* db, bool updateSections, bool updateChapters) {
	if (id == -1)
		return false;

	try {
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;

		wxSQLite3StatementBuffer buffer;

		wxString update("UPDATE books SET name = '%q', author = '%q', genre = '%q', description = '&q', synopsys = '%q', position = ");
		update << pos << " WHERE id = " << id << ";";

		buffer.Format((const char*)update, (const char*)title.ToUTF8(), (const char*)author.ToUTF8(),
			(const char*)genre.ToUTF8(), (const char*)description.ToUTF8(), (const char*)synopsys.ToUTF8());
		storage->ExecuteUpdate(buffer);

		if (updateSections) {
			wxSQLite3Table customTable = storage->GetTable("SELECT * FROM sections WHERE book_id = " + std::to_string(id));

			int prevSize = customTable.GetRowCount();
			int newSize = sections.size();

			if (newSize > prevSize) {
				int i = 0;
				for (i; i < prevSize; i++)
					sections[i].Update(storage, updateChapters);

				for (i; i < newSize; i++)
					sections[i].Save(storage);
			} else {
				int i = 0;
				for (i; i < newSize; i++)
					sections[i].Update(storage, updateChapters);

				if (newSize < prevSize)
					for (i; i < prevSize; i++) {
						customTable.SetRow(i);

						update = "DELETE FROM sections WHERE id = ";
						update << customTable.GetInt("id") << ";";

						storage->ExecuteUpdate(update);
					}
			}
		}
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}

	return true;
}

amDocument Book::GenerateDocumentSimple() {
	amDocument document;
	document.name = title;
	document.tableName = "books";

	document.integers.push_back(pair<wxString, int>("position", pos));

	document.strings.push_back(pair<wxString, wxString>("synopsys", synopsys));
	document.strings.push_back(pair<wxString, wxString>("author", author));
	document.strings.push_back(pair<wxString, wxString>("genre", genre));
	document.strings.push_back(pair<wxString, wxString>("description", description));

	return document;
}

amDocument Book::GenerateDocument(wxVector<int>& sectionsToGen) {
	amDocument document = GenerateDocumentSimple();

	if (sectionsToGen.empty()) {
		document.documents.reserve(sections.size());
		for (auto& it : sections)
			document.documents.push_back(it.GenerateDocument());

	} else {
		int size = sectionsToGen.size();
		document.documents.reserve(size);

		for (auto& it : sectionsToGen) {
			if (it < size)
				document.documents.push_back(sections[it].GenerateDocument());

		}
	}

	return document;
}

amDocument Book::GenerateDocumentForId() {
	amDocument document;
	document.name = title;
	document.tableName = "books";

	return document;
}


/////////////////////////////////////////////////////////////////
//////////////////////////// Project ////////////////////////////
/////////////////////////////////////////////////////////////////


wxVector<Chapter>& amProject::GetChapters(int bookPos, int sectionPos) {
	return books[bookPos - 1].sections[sectionPos - 1].chapters;
}

wxVector<Chapter> amProject::GetChapters(int bookPos) {
	Book& book = books[bookPos - 1];
	int totalSize = 0;

	for (auto& it : book.sections)
		totalSize += it.chapters.size();

	wxVector<Chapter> grouped;
	grouped.reserve(totalSize);

	for (auto& it : book.sections)
		for (auto& it2 : it.chapters)
			grouped.push_back(it2);

	return grouped;
}