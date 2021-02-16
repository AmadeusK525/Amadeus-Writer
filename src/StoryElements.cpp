#include "StoryElements.h"
#include "BookElements.h"

#include <wx\mstream.h>
#include <wx\memory.h>

#include "ProjectManager.h"

CompType Element::elCompType = CompRole;
CompType Character::cCompType = CompRole;
CompType Location::lCompType = CompRole;
CompType Item::iCompType = CompRole;

bool Element::operator<(const Element& other) const {
	int i, j;

	switch (elCompType) {
	case CompRole:
		if (role != other.role)
			return role < other.role;

		break;
	case CompNameInverse:
		return name.Lower() > other.name.Lower();
		break;

	case CompChapters:
		i = chapters.size();
		j = other.chapters.size();

		if (i != j)
			return i > j;

		break;
	case CompFirst:
		i = 9999;
		j = 9999;

		for (auto& it : chapters) {
			if (it->position < i)
				i = it->position;
		}

		for (auto& it : other.chapters) {
			if (it->position < j)
				j = it->position;
		}

		if (i != j)
			return i < j;

		break;
	case CompLast:
		i = -1;
		j = -1;

		for (auto& it : chapters) {
			if (it->position > i)
				i = it->position;
		}

		for (auto& it : other.chapters) {
			if (it->position > j)
				j = it->position;
		}

		if (i != j)
			return i > j;

		break;
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

bool Element::operator==(const Element& other) const {
	return name == other.name;

}

void Element::operator=(const Element& other) {
	name = other.name;
	role = other.role;
	image = other.image;
	custom = other.custom;
	elCompType = other.elCompType;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Character ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void Character::Save(wxSQLite3Database* db) {
	try {
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
		bool doImage = image.IsOk();

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO characters (name, sex, age, nationality, height, nickname, "
			"appearance, personality, backstory, role, image) VALUES (");
		insert << "'%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', " << role;

		if (doImage)
			insert << ", ?";
		else
			insert << ", NULL";

		insert << ");";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)sex.ToUTF8(), (const char*)age.ToUTF8(),
			(const char*)nat.ToUTF8(), (const char*)height.ToUTF8(), (const char*)nick.ToUTF8(),
			(const char*)appearance.ToUTF8(), (const char*)personality.ToUTF8(), (const char*)backstory.ToUTF8());

		wxSQLite3Statement statement = storage->PrepareStatement(buffer);

		if (doImage) {
			wxMemoryOutputStream stream;

			image.SaveFile(stream, wxBITMAP_TYPE_BMP);

			wxMemoryBuffer membuffer;
			membuffer.SetBufSize(stream.GetSize());
			membuffer.SetDataLen(stream.GetSize());
			stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

			statement.Bind(1, membuffer);
		}

		statement.ExecuteUpdate();
		SetId(storage->GetDocumentId(GenerateDocumentForId()));

		for (auto& it : custom) {
			insert = "INSERT INTO characters_custom (name, content, character_id) VALUES ('%q', '%q', ";
			insert << id << ");";

			buffer.Format((const char*)insert, (const char*)it.first.ToUTF8(), (const char*)it.second.ToUTF8());

			storage->ExecuteUpdate(buffer);
		}
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}
}

bool Character::Update(wxSQLite3Database* db) {
	if (id == -1)
		return false;

	amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
	bool doImage = image.IsOk();

	wxString update("UPDATE characters SET name = '%q', sex = '%q', age = '%q', nationality = '%q', "
		"height = '%q', nickname = '%q', appearance = '%q', personality = '%q', backstory = '%q', role = ");
	update << role << ", image = ";

	if (doImage)
		update << "?";
	else
		update << "NULL";

	update << " WHERE id = " << id << "; ";

	wxSQLite3StatementBuffer buffer;
	buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)sex.ToUTF8(), (const char*)age.ToUTF8(),
		(const char*)nat.ToUTF8(), (const char*)height.ToUTF8(), (const char*)nick.ToUTF8(), (const char*)appearance.ToUTF8(),
		(const char*)personality.ToUTF8(), (const char*)backstory.ToUTF8());

	wxSQLite3Statement statement = storage->PrepareStatement(buffer);

	if (doImage) {
		wxMemoryOutputStream stream;

		image.SaveFile(stream, wxBITMAP_TYPE_BMP);

		wxMemoryBuffer membuffer;
		membuffer.SetBufSize(stream.GetSize());
		membuffer.SetDataLen(stream.GetSize());
		stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

		statement.Bind(1, membuffer);
	}

	statement.ExecuteUpdate();
	try {
		wxSQLite3Table customTable = storage->GetTable("SELECT * FROM characters_custom WHERE character_id = " + std::to_string(id));

		int prevSize = customTable.GetRowCount();
		int newSize = custom.size();

		if (newSize > prevSize) {
			int i = 0;
			for (i; i < prevSize; i++) {
				customTable.SetRow(i);
				update = "UPDATE characters_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			for (i; i < newSize; i++) {
				update = "INSERT INTO characters_custom (name, content, character_id) VALUES ('%q', '%q', "
					+ std::to_string(id) + ");";
				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());

				storage->ExecuteUpdate(buffer);
			}
		} else {
			int i = 0;
			for (i; i < newSize; i++) {
				customTable.SetRow(i);
				update = "UPDATE characters_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			if (newSize < prevSize)
				for (i; i < prevSize; i++) {
					customTable.SetRow(i);
					update = "DELETE FROM characters_custom WHERE rowid = ";
					update << customTable.GetInt("id") << ";";

					storage->ExecuteUpdate(update);
				}
		}

	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}

	return true;
}

amDocument Character::GenerateDocumentSimple() {
	amDocument document;
	document.name = name;
	document.tableName = "characters";

	document.integers.push_back(pair<wxString, int>("role", role));

	document.strings.reserve(8);
	document.strings.push_back(pair<wxString, wxString>("sex", sex));
	document.strings.push_back(pair<wxString, wxString>("age", age));
	document.strings.push_back(pair<wxString, wxString>("nationality", nat));
	document.strings.push_back(pair<wxString, wxString>("height", height));
	document.strings.push_back(pair<wxString, wxString>("nickname", nick));
	document.strings.push_back(pair<wxString, wxString>("appearance", appearance));
	document.strings.push_back(pair<wxString, wxString>("personality", personality));
	document.strings.push_back(pair<wxString, wxString>("backstory", backstory));

	if (image.IsOk()) {
		wxMemoryOutputStream stream;
		image.SaveFile(stream, image.GetType());

		wxMemoryBuffer buffer;
		stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

		document.memBuffers.push_back(pair<wxString, wxMemoryBuffer>("image", buffer));
	}

	return document;
}

amDocument Character::GenerateDocument() {
	amDocument document = GenerateDocumentSimple();

	for (auto& it : custom) {
		amDocument customDoc;
		customDoc.tableName = "characters_custom";

		customDoc.name = it.first;
		customDoc.strings.push_back(pair<wxString, wxString>("content", it.second));

		customDoc.specialForeign = true;
		customDoc.foreignKey.first = "character_id";

		document.documents.push_back(customDoc);
	}

	return document;
}

amDocument Character::GenerateDocumentForId() {
	amDocument document;
	document.name = name;
	document.tableName = "characters";

	document.integers.push_back(pair<wxString, int>("role", role));

	return document;
}

bool Character::operator<(const Character& other) const {
	int i, j;

	switch (cCompType) {
	case CompRole:
		if (role != other.role)
			return role < other.role;

		break;

	case CompNameInverse:
		return name.Lower() > other.name.Lower();
		break;

	case CompChapters:
		i = chapters.size();
		j = other.chapters.size();

		if (i != j)
			return i > j;

		break;
	case CompFirst:
		i = 9999;
		j = 9999;

		for (auto& it : chapters) {
			if (it->position < i)
				i = it->position;
		}

		for (auto& it : other.chapters) {
			if (it->position < j)
				j = it->position;
		}

		if (i != j)
			return i < j;

		break;
	case CompLast:
		i = -1;
		j = -1;

		for (auto& it : chapters) {
			if (it->position > i)
				i = it->position;
		}

		for (auto& it : other.chapters) {
			if (it->position > j)
				j = it->position;
		}

		if (i != j)
			return i > j;

		break;
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

void Character::operator=(const Character& other) {
	name = other.name;
	role = other.role;
	image = other.image;
	sex = other.sex;
	height = other.height;
	age = other.age;
	nat = other.nat;
	nick = other.nick;
	appearance = other.appearance;
	personality = other.personality;
	backstory = other.backstory;
	custom = other.custom;
	id = other.id;
	cCompType = other.cCompType;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Location ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void Location::Save(wxSQLite3Database* db) {
	try {
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
		bool doImage = image.IsOk();

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO locations (name, general, natural, architecture, politics, "
			"economy, culture, role, image) VALUES (");
		insert << "'%q', '%q', '%q', '%q', '%q', '%q', '%q', " << role;

		if (doImage)
			insert << ", ?";
		else
			insert << ", NULL";

		insert << ");";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)general.ToUTF8(), (const char*)natural.ToUTF8(),
			(const char*)architecture.ToUTF8(), (const char*)politics.ToUTF8(),
			(const char*)economy.ToUTF8(), (const char*)culture.ToUTF8());
		wxSQLite3Statement statement = storage->PrepareStatement(buffer);

		if (doImage) {
			wxMemoryOutputStream stream;

			image.SaveFile(stream, wxBITMAP_TYPE_BMP);

			wxMemoryBuffer membuffer;
			membuffer.SetBufSize(stream.GetSize());
			membuffer.SetDataLen(stream.GetSize());
			stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

			statement.Bind(1, membuffer);
		}

		statement.ExecuteUpdate();
		SetId(storage->GetDocumentId(GenerateDocumentForId()));

		for (auto& it : custom) {
			insert = "INSERT INTO locations_custom (name, content, location_id) VALUES ('%q', '%q', ";
			insert << id << ");";

			buffer.Format((const char*)insert, (const char*)it.first.ToUTF8(), (const char*)it.second.ToUTF8());

			storage->ExecuteUpdate(buffer);
		}
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}
}

bool Location::Update(wxSQLite3Database* db) {
	if (id == -1)
		return false;

	amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
	bool doImage = image.IsOk();

	wxString update("UPDATE locations SET name = '%q', general = '%q', natural = '%q', architecture = '%q', "
		"politics = '%q', economy = '%q', culture = '%q', role = ");
	update << role << ", image = ";

	if (doImage)
		update << "?";
	else
		update << "NULL";

	update << " WHERE id = " << id << "; ";

	wxSQLite3StatementBuffer buffer;
	buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)general.ToUTF8(), (const char*)natural.ToUTF8(),
		(const char*)architecture.ToUTF8(), (const char*)politics.ToUTF8(),
		(const char*)economy.ToUTF8(), (const char*)culture.ToUTF8());

	wxSQLite3Statement statement = storage->PrepareStatement(buffer);

	if (doImage) {
		wxMemoryOutputStream stream;

		image.SaveFile(stream, wxBITMAP_TYPE_BMP);

		wxMemoryBuffer membuffer;
		membuffer.SetBufSize(stream.GetSize());
		membuffer.SetDataLen(stream.GetSize());
		stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

		statement.Bind(1, membuffer);
	}

	statement.ExecuteUpdate();
	try {
		wxSQLite3Table customTable = storage->GetTable("SELECT * FROM locations_custom WHERE location_id = " + std::to_string(id));

		int prevSize = customTable.GetRowCount();
		int newSize = custom.size();

		if (newSize > prevSize) {
			int i = 0;
			for (i; i < prevSize; i++) {
				customTable.SetRow(i);
				update = "UPDATE locations_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			for (i; i < newSize; i++) {
				update = "INSERT INTO locations_custom (name, content, location_id) VALUES ('%q', '%q', "
					+ std::to_string(id) + ");";
				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());

				storage->ExecuteUpdate(buffer);
			}
		} else {
			int i = 0;
			for (i; i < newSize; i++) {
				customTable.SetRow(i);
				update = "UPDATE locations_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			if (newSize < prevSize)
				for (i; i < prevSize; i++) {
					customTable.SetRow(i);
					update = "DELETE FROM locations_custom WHERE rowid = ";
					update << customTable.GetInt("id") << ";";

					storage->ExecuteUpdate(update);
				}
		}

	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}

	return true;
}

amDocument Location::GenerateDocumentSimple() {
	amDocument document;
	document.name = name;
	document.tableName = "locations";

	document.integers.push_back(pair<wxString, int>("role", role));

	document.strings.reserve(6);
	document.strings.push_back(pair<wxString, wxString>("general", general));
	document.strings.push_back(pair<wxString, wxString>("natural", natural));
	document.strings.push_back(pair<wxString, wxString>("architecture", architecture));
	document.strings.push_back(pair<wxString, wxString>("politics", politics));
	document.strings.push_back(pair<wxString, wxString>("economy", economy));
	document.strings.push_back(pair<wxString, wxString>("culture", culture));

	if (image.IsOk()) {
		wxMemoryOutputStream stream;
		image.SaveFile(stream, image.GetType());

		wxMemoryBuffer buffer;
		stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

		document.memBuffers.push_back(pair<wxString, wxMemoryBuffer>("image", buffer));
	}

	return document;
}

amDocument Location::GenerateDocument() {
	amDocument document = GenerateDocumentSimple();

	document.documents.reserve(custom.size());

	for (auto& it : custom) {
		amDocument customDoc;
		customDoc.tableName = "locations_custom";

		customDoc.name = it.first;
		customDoc.strings.push_back(pair<wxString, wxString>("content", it.second));

		customDoc.specialForeign = true;
		customDoc.foreignKey.first = "location_id";

		document.documents.push_back(customDoc);
	}

	return document;
}

amDocument Location::GenerateDocumentForId() {
	amDocument document;
	document.name = name;
	document.tableName = "locations";

	document.integers.push_back(pair<wxString, int>("role", role));

	return document;
}

bool Location::operator<(const Location& other) const {
	int i, j;

	switch (lCompType) {
	case CompRole:
		if (role != other.role)
			return role < other.role;

		break;

	case CompNameInverse:
		return name.Lower() > other.name.Lower();
		break;

	case CompChapters:
		i = chapters.size();
		j = other.chapters.size();

		if (i != j)
			return i > j;

		break;
	case CompFirst:
		i = 9999;
		j = 9999;

		for (auto& it : chapters) {
			if (it->position < i)
				i = it->position;
		}

		for (auto& it : other.chapters) {
			if (it->position < j)
				j = it->position;
		}

		if (i != j)
			return i < j;

		break;
	case CompLast:
		i = -1;
		j = -1;

		for (auto& it : chapters) {
			if (it->position > i)
				i = it->position;
		}

		for (auto& it : other.chapters) {
			if (it->position > j)
				j = it->position;
		}

		if (i != j)
			return i > j;

		break;
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

void Location::operator=(const Location& other) {
	name = other.name;
	role = other.role;
	image = other.image;
	custom = other.custom;
	general = other.general;
	natural = other.natural;
	architecture = other.architecture;
	politics = other.politics;
	economy = other.economy;
	culture = other.culture;
	id = other.id;
	lCompType = other.lCompType;
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Item /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void Item::Save(wxSQLite3Database* db) {
	try {
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
		bool doImage = image.IsOk();

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO items (name, general, origin, backstory, appearance, "
			"usage, width, height, depth, role, isManMade, isMagic, image) VALUES (");
		insert << "'%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', " << role << ", " << isManMade <<
			", " << isMagic;

		if (doImage)
			insert << ", ?";
		else
			insert << ", NULL";

		insert << ");";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)general.ToUTF8(), (const char*)origin.ToUTF8(),
			(const char*)backstory.ToUTF8(), (const char*)appearance.ToUTF8(), (const char*)usage.ToUTF8(), (const char*)width.ToUTF8(),
			(const char*)height.ToUTF8(), (const char*)depth.ToUTF8());

		wxSQLite3Statement statement = storage->PrepareStatement(buffer);

		if (doImage) {
			wxMemoryOutputStream stream;

			image.SaveFile(stream, wxBITMAP_TYPE_BMP);

			wxMemoryBuffer membuffer;
			membuffer.SetBufSize(stream.GetSize());
			membuffer.SetDataLen(stream.GetSize());
			stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

			statement.Bind(1, membuffer);
		}

		statement.ExecuteUpdate();
		SetId(storage->GetDocumentId(GenerateDocumentForId()));

		for (auto& it : custom) {
			insert = "INSERT INTO items_custom (name, content, item_id) VALUES ('%q', '%q', ";
			insert << id << ");";

			buffer.Format((const char*)insert, (const char*)it.first.ToUTF8(), (const char*)it.second.ToUTF8());

			storage->ExecuteUpdate(buffer);
		}
	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}
}

bool Item::Update(wxSQLite3Database* db) {
	if (id == -1)
		return false;

	amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
	bool doImage = image.IsOk();

	wxString update("UPDATE items SET name = '%q', general = '%q', origin = '%q', backstory = '%q', "
		"appearance = '%q', usage = '%q', width = '%q', height = '%q', depth = '%q', role = ");
	update << role << ", isManMade = " << isManMade << ", isMagic = " << isMagic << ", image = ";

	if (doImage)
		update << "?";
	else
		update << "NULL";

	update << " WHERE id = " << id << "; ";

	wxSQLite3StatementBuffer buffer;
	buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)general.ToUTF8(), (const char*)origin.ToUTF8(),
		(const char*)backstory.ToUTF8(), (const char*)appearance.ToUTF8(), (const char*)usage.ToUTF8(), (const char*)width.ToUTF8(),
		(const char*)height.ToUTF8(), (const char*)depth.ToUTF8());

	wxSQLite3Statement statement = storage->PrepareStatement(buffer);

	if (doImage) {
		wxMemoryOutputStream stream;

		image.SaveFile(stream, wxBITMAP_TYPE_BMP);

		wxMemoryBuffer membuffer;
		membuffer.SetBufSize(stream.GetSize());
		membuffer.SetDataLen(stream.GetSize());
		stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

		statement.Bind(1, membuffer);
	}

	statement.ExecuteUpdate();
	try {
		wxSQLite3Table customTable = storage->GetTable("SELECT * FROM items_custom WHERE item_id = " + std::to_string(id));

		int prevSize = customTable.GetRowCount();
		int newSize = custom.size();

		if (newSize > prevSize) {
			int i = 0;
			for (i; i < prevSize; i++) {
				customTable.SetRow(i);
				update = "UPDATE items_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			for (i; i < newSize; i++) {
				update = "INSERT INTO items_custom (name, content, item_id) VALUES ('%q', '%q', "
					+ std::to_string(id) + ");";
				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());

				storage->ExecuteUpdate(buffer);
			}
		} else {
			int i = 0;
			for (i; i < newSize; i++) {
				customTable.SetRow(i);
				update = "UPDATE items_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			if (newSize < prevSize)
				for (i; i < prevSize; i++) {
					customTable.SetRow(i);
					update = "DELETE FROM items_custom WHERE rowid = ";
					update << customTable.GetInt("id") << ";";

					storage->ExecuteUpdate(update);
				}
		}

	} catch (wxSQLite3Exception& e) {
		wxMessageBox(e.GetMessage());
	}

	return true;
}

amDocument Item::GenerateDocumentSimple() {
	amDocument document;
	document.name = name;
	document.tableName = "items";

	document.integers.reserve(3);
	document.integers.push_back(pair<wxString, int>("role", role));
	document.integers.push_back(pair<wxString, int>("isMagic", isMagic));
	document.integers.push_back(pair<wxString, int>("isManMade", isManMade));

	document.strings.reserve(8);
	document.strings.push_back(pair<wxString, wxString>("general", general));
	document.strings.push_back(pair<wxString, wxString>("origin", origin));
	document.strings.push_back(pair<wxString, wxString>("backstory", backstory));
	document.strings.push_back(pair<wxString, wxString>("appearance", appearance));
	document.strings.push_back(pair<wxString, wxString>("usage", usage));
	document.strings.push_back(pair<wxString, wxString>("width", width));
	document.strings.push_back(pair<wxString, wxString>("height", height));
	document.strings.push_back(pair<wxString, wxString>("depth", depth));

	if (image.IsOk()) {
		wxMemoryOutputStream stream;
		image.SaveFile(stream, image.GetType());

		wxMemoryBuffer buffer;
		stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

		document.memBuffers.push_back(pair<wxString, wxMemoryBuffer>("image", buffer));
	}

	return document;
}

amDocument Item::GenerateDocument() {
	amDocument document = GenerateDocumentSimple();

	document.documents.reserve(custom.size());
	for (auto& it : custom) {
		amDocument customDoc;
		customDoc.tableName = "items_custom";

		customDoc.name = it.first;
		customDoc.strings.push_back(pair<wxString, wxString>("content", it.second));

		customDoc.specialForeign = true;
		customDoc.foreignKey.first = "item_id";

		document.documents.push_back(customDoc);
	}

	return document;
}

amDocument Item::GenerateDocumentForId() {
	amDocument document;
	document.name = name;
	document.tableName = "items";

	document.integers.push_back(pair<wxString, int>("role", role));

	return document;
}

bool Item::operator<(const Item& other) const {
	int i, j;

	switch (iCompType) {
	case CompRole:
		if (role != other.role)
			return role < other.role;

		break;

	case CompNameInverse:
		return name.Lower() > other.name.Lower();
		break;

	case CompChapters:
		i = chapters.size();
		j = other.chapters.size();

		if (i != j)
			return i > j;

		break;
	case CompFirst:
		i = 9999;
		j = 9999;

		for (auto& it : chapters) {
			if (it->position < i)
				i = it->position;
		}

		for (auto& it : other.chapters) {
			if (it->position < j)
				j = it->position;
		}

		if (i != j)
			return i < j;

		break;
	case CompLast:
		i = -1;
		j = -1;

		for (auto& it : chapters) {
			if (it->position > i)
				i = it->position;
		}

		for (auto& it : other.chapters) {
			if (it->position > j)
				j = it->position;
		}

		if (i != j)
			return i > j;

		break;
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

void Item::operator=(const Item& other) {
	name = other.name;
	role = other.role;
	image = other.image;
	origin = other.origin;
	backstory = other.backstory;
	appearance = other.appearance;
	usage = other.usage;
	general = other.general;
	width = other.width;
	height = other.height;
	depth = other.depth;
	isMagic = other.isMagic;
	isManMade = other.isManMade;
	custom = other.custom;
	id = other.id;
	iCompType = other.iCompType;
}
