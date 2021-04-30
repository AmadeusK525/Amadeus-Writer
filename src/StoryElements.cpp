#include "StoryElements.h"
#include "BookElements.h"

#include <wx\mstream.h>

#include "ProjectManager.h"

#include "wxmemdbg.h"

CompType Element::elCompType = CompRole;
CompType Character::cCompType = CompRole;
CompType Location::lCompType = CompRole;
CompType Item::iCompType = CompRole;

wxIMPLEMENT_CLASS(Element, wxObject);

bool Element::operator<(const Element& other) const
{
	int i, j;

	switch ( elCompType )
	{
	case CompRole:
		if ( role != other.role )
			return role < other.role;

		break;
	case CompNameInverse:
		return name.Lower() > other.name.Lower();
		break;

	case CompDocuments:
		i = documents.size();
		j = other.documents.size();

		if ( i != j )
			return i > j;

		break;
	case CompFirst:
		i = 9999;
		j = 9999;

		for ( Document* document : documents )
		{
			if ( document->position < i )
				i = document->position;
		}

		for ( Document* document : other.documents )
		{
			if ( document->position < j )
				j = document->position;
		}

		if ( i != j )
			return i < j;

		break;
	case CompLast:
		i = -1;
		j = -1;

		for ( Document* document : documents )
		{
			if ( document->position > i )
				i = document->position;
		}

		for ( Document* document : other.documents )
		{
			if ( document->position > j )
				j = document->position;
		}

		if ( i != j )
			return i > j;

		break;
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

bool Element::operator==(const Element& other) const
{
	return name == other.name;

}

void Element::operator=(const Element& other)
{
	name = other.name;
	role = other.role;
	image = other.image;
	custom = other.custom;
	elCompType = other.elCompType;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Character ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_CLASS(Character, Element);

void Character::Save(wxSQLite3Database* db)
{
	try
	{
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
		bool doImage = image.IsOk();

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO characters (name, sex, age, nationality, height, nickname, "
			"appearance, personality, backstory, role, image) VALUES (");
		insert << "'%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', " << role;

		if ( doImage )
			insert << ", ?";
		else
			insert << ", NULL";

		insert << ");";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)sex.ToUTF8(), (const char*)age.ToUTF8(),
			(const char*)nat.ToUTF8(), (const char*)height.ToUTF8(), (const char*)nick.ToUTF8(),
			(const char*)appearance.ToUTF8(), (const char*)personality.ToUTF8(), (const char*)backstory.ToUTF8());

		wxSQLite3Statement statement = storage->PrepareStatement(buffer);

		if ( doImage )
		{
			wxMemoryOutputStream stream;

			image.SaveFile(stream, wxBITMAP_TYPE_PNG);

			wxMemoryBuffer membuffer;
			membuffer.SetBufSize(stream.GetSize());
			membuffer.SetDataLen(stream.GetSize());
			stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

			statement.Bind(1, membuffer);
		}

		statement.ExecuteUpdate();
		SetId(storage->GetSQLEntryId(GenerateSQLEntryForId()));

		for ( pair<wxString, wxString>& it : custom )
		{
			insert = "INSERT INTO characters_custom (name, content, character_id) VALUES ('%q', '%q', ";
			insert << id << ");";

			buffer.Format((const char*)insert, (const char*)it.first.ToUTF8(), (const char*)it.second.ToUTF8());

			storage->ExecuteUpdate(buffer);
		}
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

bool Character::Update(wxSQLite3Database* db)
{
	if ( id == -1 )
		return false;

	amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
	bool doImage = image.IsOk();

	wxString update("UPDATE characters SET name = '%q', sex = '%q', age = '%q', nationality = '%q', "
		"height = '%q', nickname = '%q', appearance = '%q', personality = '%q', backstory = '%q', role = ");
	update << role << ", image = ";

	if ( doImage )
		update << "?";
	else
		update << "NULL";

	update << " WHERE id = " << id << "; ";

	wxSQLite3StatementBuffer buffer;
	buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)sex.ToUTF8(), (const char*)age.ToUTF8(),
		(const char*)nat.ToUTF8(), (const char*)height.ToUTF8(), (const char*)nick.ToUTF8(), (const char*)appearance.ToUTF8(),
		(const char*)personality.ToUTF8(), (const char*)backstory.ToUTF8());

	wxSQLite3Statement statement = storage->PrepareStatement(buffer);

	if ( doImage )
	{
		wxMemoryOutputStream stream;

		image.SaveFile(stream, wxBITMAP_TYPE_PNG);

		wxMemoryBuffer membuffer;
		membuffer.SetBufSize(stream.GetSize());
		membuffer.SetDataLen(stream.GetSize());
		stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

		statement.Bind(1, membuffer);
	}

	statement.ExecuteUpdate();
	try
	{
		wxSQLite3Table customTable = storage->GetTable("SELECT * FROM characters_custom WHERE character_id = " + std::to_string(id));

		int prevSize = customTable.GetRowCount();
		int newSize = custom.size();

		if ( newSize > prevSize )
		{
			int i = 0;
			for ( i; i < prevSize; i++ )
			{
				customTable.SetRow(i);
				update = "UPDATE characters_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			for ( i; i < newSize; i++ )
			{
				update = "INSERT INTO characters_custom (name, content, character_id) VALUES ('%q', '%q', "
					+ std::to_string(id) + ");";
				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());

				storage->ExecuteUpdate(buffer);
			}
		}
		else
		{
			int i = 0;
			for ( i; i < newSize; i++ )
			{
				customTable.SetRow(i);
				update = "UPDATE characters_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			if ( newSize < prevSize )
				for ( i; i < prevSize; i++ )
				{
					customTable.SetRow(i);
					update = "DELETE FROM characters_custom WHERE rowid = ";
					update << customTable.GetInt("id") << ";";

					storage->ExecuteUpdate(update);
				}
		}

	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return true;
}

amSQLEntry Character::GenerateSQLEntrySimple()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "characters";

	sqlEntry.integers["role"] = role;

	sqlEntry.strings.reserve(8);
	sqlEntry.strings["sex"] = sex;
	sqlEntry.strings["age"] = age;
	sqlEntry.strings["nationality"] = nat;
	sqlEntry.strings["height"] = height;
	sqlEntry.strings["nickname"] = nick;
	sqlEntry.strings["appearance"] = appearance;
	sqlEntry.strings["personality"] = personality;
	sqlEntry.strings["backstory"] = backstory;

	if ( image.IsOk() )
	{
		wxMemoryOutputStream stream;
		image.SaveFile(stream, image.GetType());

		wxMemoryBuffer buffer;
		stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

		sqlEntry.memBuffers["image"] = buffer;
	}

	return sqlEntry;
}

amSQLEntry Character::GenerateSQLEntry()
{
	amSQLEntry sqlEntry = GenerateSQLEntrySimple();

	for ( pair<wxString, wxString>& it : custom )
	{
		amSQLEntry customDoc;
		customDoc.tableName = "characters_custom";

		customDoc.name = it.first;
		customDoc.strings["content"] = it.second;

		customDoc.specialForeign = true;
		customDoc.foreignKey.first = "character_id";
		customDoc.foreignKey.second = this->id;

		sqlEntry.childEntries.push_back(customDoc);
	}

	return sqlEntry;
}

amSQLEntry Character::GenerateSQLEntryForId()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "characters";

	sqlEntry.integers["role"] = role;

	return sqlEntry;
}

bool Character::operator<(const Character& other) const
{
	int i, j;

	switch ( cCompType )
	{
	case CompRole:
		if ( role != other.role )
			return role < other.role;

		break;

	case CompNameInverse:
		return name.Lower() > other.name.Lower();
		break;

	case CompDocuments:
		i = documents.size();
		j = other.documents.size();

		if ( i != j )
			return i > j;

		break;
	case CompFirst:
		i = 9999;
		j = 9999;

		for ( Document* document : documents )
		{
			if ( document->position < i )
				i = document->position;
		}

		for ( Document* document : other.documents )
		{
			if ( document->position < j )
				j = document->position;
		}

		if ( i != j )
			return i < j;

		break;
	case CompLast:
		i = -1;
		j = -1;

		for ( Document* document : documents )
		{
			if ( document->position > i )
				i = document->position;
		}

		for ( Document* document : other.documents )
		{
			if ( document->position > j )
				j = document->position;
		}

		if ( i != j )
			return i > j;

		break;
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

void Character::operator=(const Character& other)
{
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


wxIMPLEMENT_CLASS(Location, Element);

void Location::Save(wxSQLite3Database* db)
{
	try
	{
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
		bool doImage = image.IsOk();

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO locations (name, general, natural, architecture, politics, "
			"economy, culture, role, image) VALUES (");
		insert << "'%q', '%q', '%q', '%q', '%q', '%q', '%q', " << role;

		if ( doImage )
			insert << ", ?";
		else
			insert << ", NULL";

		insert << ");";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)general.ToUTF8(), (const char*)natural.ToUTF8(),
			(const char*)architecture.ToUTF8(), (const char*)politics.ToUTF8(),
			(const char*)economy.ToUTF8(), (const char*)culture.ToUTF8());
		wxSQLite3Statement statement = storage->PrepareStatement(buffer);

		if ( doImage )
		{
			wxMemoryOutputStream stream;

			image.SaveFile(stream, wxBITMAP_TYPE_PNG);

			wxMemoryBuffer membuffer;
			membuffer.SetBufSize(stream.GetSize());
			membuffer.SetDataLen(stream.GetSize());
			stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

			statement.Bind(1, membuffer);
		}

		statement.ExecuteUpdate();
		SetId(storage->GetSQLEntryId(GenerateSQLEntryForId()));

		for ( pair<wxString, wxString>& it : custom )
		{
			insert = "INSERT INTO locations_custom (name, content, location_id) VALUES ('%q', '%q', ";
			insert << id << ");";

			buffer.Format((const char*)insert, (const char*)it.first.ToUTF8(), (const char*)it.second.ToUTF8());

			storage->ExecuteUpdate(buffer);
		}
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

bool Location::Update(wxSQLite3Database* db)
{
	if ( id == -1 )
		return false;

	amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
	bool doImage = image.IsOk();

	wxString update("UPDATE locations SET name = '%q', general = '%q', natural = '%q', architecture = '%q', "
		"politics = '%q', economy = '%q', culture = '%q', role = ");
	update << role << ", image = ";

	if ( doImage )
		update << "?";
	else
		update << "NULL";

	update << " WHERE id = " << id << "; ";

	wxSQLite3StatementBuffer buffer;
	buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)general.ToUTF8(), (const char*)natural.ToUTF8(),
		(const char*)architecture.ToUTF8(), (const char*)politics.ToUTF8(),
		(const char*)economy.ToUTF8(), (const char*)culture.ToUTF8());

	wxSQLite3Statement statement = storage->PrepareStatement(buffer);

	if ( doImage )
	{
		wxMemoryOutputStream stream;

		image.SaveFile(stream, wxBITMAP_TYPE_PNG);

		wxMemoryBuffer membuffer;
		membuffer.SetBufSize(stream.GetSize());
		membuffer.SetDataLen(stream.GetSize());
		stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

		statement.Bind(1, membuffer);
	}

	statement.ExecuteUpdate();
	try
	{
		wxSQLite3Table customTable = storage->GetTable("SELECT * FROM locations_custom WHERE location_id = " + std::to_string(id));

		int prevSize = customTable.GetRowCount();
		int newSize = custom.size();

		if ( newSize > prevSize )
		{
			int i = 0;
			for ( i; i < prevSize; i++ )
			{
				customTable.SetRow(i);
				update = "UPDATE locations_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			for ( i; i < newSize; i++ )
			{
				update = "INSERT INTO locations_custom (name, content, location_id) VALUES ('%q', '%q', "
					+ std::to_string(id) + ");";
				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());

				storage->ExecuteUpdate(buffer);
			}
		}
		else
		{
			int i = 0;
			for ( i; i < newSize; i++ )
			{
				customTable.SetRow(i);
				update = "UPDATE locations_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			if ( newSize < prevSize )
				for ( i; i < prevSize; i++ )
				{
					customTable.SetRow(i);
					update = "DELETE FROM locations_custom WHERE rowid = ";
					update << customTable.GetInt("id") << ";";

					storage->ExecuteUpdate(update);
				}
		}

	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return true;
}

amSQLEntry Location::GenerateSQLEntrySimple()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "locations";

	sqlEntry.integers["role"] = role;

	sqlEntry.strings.reserve(6);
	sqlEntry.strings["general"] = general;
	sqlEntry.strings["natural"] = natural;
	sqlEntry.strings["architecture"] = architecture;
	sqlEntry.strings["politics"] = politics;
	sqlEntry.strings["economy"] = economy;
	sqlEntry.strings["culture"] = culture;

	if ( image.IsOk() )
	{
		wxMemoryOutputStream stream;
		image.SaveFile(stream, image.GetType());

		wxMemoryBuffer buffer;
		stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

		sqlEntry.memBuffers["image"] = buffer;
	}

	return sqlEntry;
}

amSQLEntry Location::GenerateSQLEntry()
{
	amSQLEntry sqlEntry = GenerateSQLEntrySimple();

	sqlEntry.childEntries.reserve(custom.size());

	for ( pair<wxString, wxString>& it : custom )
	{
		amSQLEntry customDoc;
		customDoc.tableName = "locations_custom";

		customDoc.name = it.first;
		customDoc.strings["content"] = it.second;

		customDoc.specialForeign = true;
		customDoc.foreignKey.first = "location_id";
		customDoc.foreignKey.second = this->id;

		sqlEntry.childEntries.push_back(customDoc);
	}

	return sqlEntry;
}

amSQLEntry Location::GenerateSQLEntryForId()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "locations";

	sqlEntry.integers["role"] = role;

	return sqlEntry;
}

bool Location::operator<(const Location& other) const
{
	int i, j;

	switch ( lCompType )
	{
	case CompRole:
		if ( role != other.role )
			return role < other.role;

		break;

	case CompNameInverse:
		return name.Lower() > other.name.Lower();
		break;

	case CompDocuments:
		i = documents.size();
		j = other.documents.size();

		if ( i != j )
			return i > j;

		break;
	case CompFirst:
		i = 9999;
		j = 9999;

		for ( Document* document : documents )
		{
			if ( document->position < i )
				i = document->position;
		}

		for ( Document* document : other.documents )
		{
			if ( document->position < j )
				j = document->position;
		}

		if ( i != j )
			return i < j;

		break;
	case CompLast:
		i = -1;
		j = -1;

		for ( Document* document : documents )
		{
			if ( document->position > i )
				i = document->position;
		}

		for ( Document* document : other.documents )
		{
			if ( document->position > j )
				j = document->position;
		}

		if ( i != j )
			return i > j;

		break;
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

void Location::operator=(const Location& other)
{
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


wxIMPLEMENT_CLASS(Item, Element);

void Item::Save(wxSQLite3Database* db)
{
	try
	{
		amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
		bool doImage = image.IsOk();

		wxSQLite3StatementBuffer buffer;

		wxString insert("INSERT INTO items (name, general, origin, backstory, appearance, "
			"usage, width, height, depth, role, isManMade, isMagic, image) VALUES (");
		insert << "'%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', " << role << ", " << isManMade <<
			", " << isMagic;

		if ( doImage )
			insert << ", ?";
		else
			insert << ", NULL";

		insert << ");";

		buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)general.ToUTF8(), (const char*)origin.ToUTF8(),
			(const char*)backstory.ToUTF8(), (const char*)appearance.ToUTF8(), (const char*)usage.ToUTF8(), (const char*)width.ToUTF8(),
			(const char*)height.ToUTF8(), (const char*)depth.ToUTF8());

		wxSQLite3Statement statement = storage->PrepareStatement(buffer);

		if ( doImage )
		{
			wxMemoryOutputStream stream;

			image.SaveFile(stream, wxBITMAP_TYPE_PNG);

			wxMemoryBuffer membuffer;
			membuffer.SetBufSize(stream.GetSize());
			membuffer.SetDataLen(stream.GetSize());
			stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

			statement.Bind(1, membuffer);
		}

		statement.ExecuteUpdate();
		SetId(storage->GetSQLEntryId(GenerateSQLEntryForId()));

		for ( pair<wxString, wxString>& it : custom )
		{
			insert = "INSERT INTO items_custom (name, content, item_id) VALUES ('%q', '%q', ";
			insert << id << ");";

			buffer.Format((const char*)insert, (const char*)it.first.ToUTF8(), (const char*)it.second.ToUTF8());

			storage->ExecuteUpdate(buffer);
		}
	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}
}

bool Item::Update(wxSQLite3Database* db)
{
	if ( id == -1 )
		return false;

	amProjectSQLDatabase* storage = (amProjectSQLDatabase*)db;
	bool doImage = image.IsOk();

	wxString update("UPDATE items SET name = '%q', general = '%q', origin = '%q', backstory = '%q', "
		"appearance = '%q', usage = '%q', width = '%q', height = '%q', depth = '%q', role = ");
	update << role << ", isManMade = " << isManMade << ", isMagic = " << isMagic << ", image = ";

	if ( doImage )
		update << "?";
	else
		update << "NULL";

	update << " WHERE id = " << id << "; ";

	wxSQLite3StatementBuffer buffer;
	buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)general.ToUTF8(), (const char*)origin.ToUTF8(),
		(const char*)backstory.ToUTF8(), (const char*)appearance.ToUTF8(), (const char*)usage.ToUTF8(), (const char*)width.ToUTF8(),
		(const char*)height.ToUTF8(), (const char*)depth.ToUTF8());

	wxSQLite3Statement statement = storage->PrepareStatement(buffer);

	if ( doImage )
	{
		wxMemoryOutputStream stream;

		image.SaveFile(stream, wxBITMAP_TYPE_PNG);

		wxMemoryBuffer membuffer;
		membuffer.SetBufSize(stream.GetSize());
		membuffer.SetDataLen(stream.GetSize());
		stream.CopyTo(membuffer.GetData(), membuffer.GetDataLen());

		statement.Bind(1, membuffer);
	}

	statement.ExecuteUpdate();
	try
	{
		wxSQLite3Table customTable = storage->GetTable("SELECT * FROM items_custom WHERE item_id = " + std::to_string(id));

		int prevSize = customTable.GetRowCount();
		int newSize = custom.size();

		if ( newSize > prevSize )
		{
			int i = 0;
			for ( i; i < prevSize; i++ )
			{
				customTable.SetRow(i);
				update = "UPDATE items_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			for ( i; i < newSize; i++ )
			{
				update = "INSERT INTO items_custom (name, content, item_id) VALUES ('%q', '%q', "
					+ std::to_string(id) + ");";
				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());

				storage->ExecuteUpdate(buffer);
			}
		}
		else
		{
			int i = 0;
			for ( i; i < newSize; i++ )
			{
				customTable.SetRow(i);
				update = "UPDATE items_custom SET name = '%q', content = '%q' WHERE rowid = ";
				update << customTable.GetInt("id") << ";";

				buffer.Format((const char*)update, (const char*)custom[i].first.ToUTF8(), (const char*)custom[i].second.ToUTF8());
				storage->ExecuteUpdate(buffer);
			}

			if ( newSize < prevSize )
				for ( i; i < prevSize; i++ )
				{
					customTable.SetRow(i);
					update = "DELETE FROM items_custom WHERE rowid = ";
					update << customTable.GetInt("id") << ";";

					storage->ExecuteUpdate(update);
				}
		}

	}
	catch ( wxSQLite3Exception& e )
	{
		wxMessageBox(e.GetMessage());
	}

	return true;
}

amSQLEntry Item::GenerateSQLEntrySimple()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "items";

	sqlEntry.integers.reserve(3);
	sqlEntry.integers["role"] = role;
	sqlEntry.integers["isMagic"] = isMagic;
	sqlEntry.integers["isManMade"] = isManMade;

	sqlEntry.strings.reserve(8);
	sqlEntry.strings["general"] = general;
	sqlEntry.strings["origin"] = origin;
	sqlEntry.strings["backstory"] = backstory;
	sqlEntry.strings["appearance"] = appearance;
	sqlEntry.strings["usage"] = usage;
	sqlEntry.strings["width"] =  width;
	sqlEntry.strings["height"] = height;
	sqlEntry.strings["depth"] = depth;

	if ( image.IsOk() )
	{
		wxMemoryOutputStream stream;
		image.SaveFile(stream, image.GetType());

		wxMemoryBuffer buffer;
		stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

		sqlEntry.memBuffers["image"] = buffer;
	}

	return sqlEntry;
}

amSQLEntry Item::GenerateSQLEntry()
{
	amSQLEntry sqlEntry = GenerateSQLEntrySimple();

	sqlEntry.childEntries.reserve(custom.size());
	for ( pair<wxString, wxString>& it : custom )
	{
		amSQLEntry customDoc;
		customDoc.tableName = "items_custom";

		customDoc.name = it.first;
		customDoc.strings["content"] = it.second;

		customDoc.specialForeign = true;
		customDoc.foreignKey.first = "item_id";
		customDoc.foreignKey.second = this->id;

		sqlEntry.childEntries.push_back(customDoc);
	}

	return sqlEntry;
}

amSQLEntry Item::GenerateSQLEntryForId()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "items";

	sqlEntry.integers["role"] = role;

	return sqlEntry;
}

bool Item::operator<(const Item& other) const
{
	int i, j;

	switch ( iCompType )
	{
	case CompRole:
		if ( role != other.role )
			return role < other.role;

		break;

	case CompNameInverse:
		return name.Lower() > other.name.Lower();
		break;

	case CompDocuments:
		i = documents.size();
		j = other.documents.size();

		if ( i != j )
			return i > j;

		break;
	case CompFirst:
		i = 9999;
		j = 9999;

		for ( Document* document : documents )
		{
			if ( document->position < i )
				i = document->position;
		}

		for ( Document* document : other.documents )
		{
			if ( document->position < j )
				j = document->position;
		}

		if ( i != j )
			return i < j;

		break;
	case CompLast:
		i = -1;
		j = -1;

		for ( Document* document : documents )
		{
			if ( document->position > i )
				i = document->position;
		}

		for ( Document* document : other.documents )
		{
			if ( document->position > j )
				j = document->position;
		}

		if ( i != j )
			return i > j;

		break;
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

void Item::operator=(const Item& other)
{
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
