#include "Elements/StoryElements.h"
#include "Elements/BookElements.h"

#include <wx/mstream.h>

#include "ProjectManaging.h"

#include "Utils/wxmemdbg.h"

wxIMPLEMENT_DYNAMIC_CLASS(xsStringMultimapIO, xsPropertyIO);

void xsStringMultimapIO::Read(xsProperty* property, wxXmlNode* source)
{
	wxStringMultimap* pMap = (wxStringMultimap*)property->m_pSourceVariable;
	pMap->clear();

	wxXmlNode* listNode = source->GetChildren();
	while ( listNode )
	{
		if ( listNode->GetName() == wxT("item") )
		{
			pMap->insert({ listNode->GetAttribute(wxT("key"), wxT("undef_key")), listNode->GetNodeContent() });
		}

		listNode = listNode->GetNext();
	}
}

void xsStringMultimapIO::Write(xsProperty* property, wxXmlNode* target)
{
	wxStringMultimap& map = *((wxStringMultimap*)property->m_pSourceVariable);

	if ( !map.empty() )
	{
		wxXmlNode* pXmlNode, * newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));

		for ( auto& it : map )
		{
			pXmlNode = AddPropertyNode(newNode, wxT("item"), it.second);
			pXmlNode->AddAttribute(wxT("key"), it.first);
		}

		target->AddChild(newNode);
		AppendPropertyType(property, newNode);
	}
}

wxString xsStringMultimapIO::GetValueStr(xsProperty* property)
{
	return ToString(*((wxStringMultimap*)property->m_pSourceVariable));
}

void xsStringMultimapIO::SetValueStr(xsProperty* property, const wxString& valstr)
{
	*((wxStringMultimap*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsStringMultimapIO::ToString(const wxStringMultimap& value)
{
	wxString out;

	bool bFirst = true;
	for ( auto& it : value )
	{
		if ( bFirst )
			bFirst = false;
		else
			out << wxT("|");

		out << it.first << wxT("->") << it.second;
	}

	return out;
}

wxStringMultimap xsStringMultimapIO::FromString(const wxString& value)
{
	wxStringMultimap mapData;

	wxString token;
	wxStringTokenizer tokens(value, wxT("|"));
	while ( tokens.HasMoreTokens() )
	{
		token = tokens.GetNextToken();
		token.Replace(wxT("->"), wxT("|"));
		mapData.insert({ token.BeforeFirst(wxT('|')),  token.AfterFirst(wxT('|')) });
	}

	return mapData;
}


am::CompType am::StoryElement::elCompType = CompRole;
am::CompType am::Character::cCompType = CompRole;
am::CompType am::Location::lCompType = CompRole;
am::CompType am::Item::iCompType = CompRole;

wxIMPLEMENT_DYNAMIC_CLASS(am::StoryElement, wxObject);

am::StoryElement::StoryElement()
{
	XS_SERIALIZE_STRING(name, wxT("name"));
	XS_SERIALIZE_INT(role, wxT("role"));
	XS_SERIALIZE_ARRAYSTRING(aliases, wxT("aliases"));
	XS_SERIALIZE_PROPERTY(mShortAttributes, wxT("unordered_string_map"), "short_attributes");
	XS_SERIALIZE_PROPERTY(mLongAttributes, wxT("unordered_string_map"), "long_attributes");
}

void am::StoryElement::Save(ProjectSQLDatabase* db)
{
	bool bDoImage = image.IsOk();

	wxString insert("INSERT INTO story_elements (name, serialized, class, image) VALUES ('%q', '%q', '");
	insert << GetClassInfo()->GetClassName();

	if ( bDoImage )
		insert << "', ?);";
	else
		insert << "', NULL);";

	wxSQLite3StatementBuffer buffer;
	buffer.Format((const char*)insert, (const char*)name.ToUTF8(), (const char*)GetXMLString().ToUTF8());

	wxSQLite3Statement statement = db->PrepareStatement(buffer);

	if ( bDoImage )
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
	SetId(db->GetSQLEntryId(GenerateSQLEntryForId()));
}

bool am::StoryElement::Update(ProjectSQLDatabase* db)
{
	if ( dbID == -1 )
		return false;

	bool bDoImage = image.IsOk();

	wxString update("UPDATE story_elements SET name = '%q', serialized = '%q', image = ");

	if ( bDoImage )
		update << "?";
	else
		update << "NULL";

	update << " WHERE id = " << dbID << "; ";

	wxSQLite3StatementBuffer buffer;
	buffer.Format((const char*)update, (const char*)name.ToUTF8(), (const char*)GetXMLString().ToUTF8());

	wxSQLite3Statement statement = db->PrepareStatement(buffer);

	if ( bDoImage )
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
	return true;
}

wxString am::StoryElement::GetXMLString()
{
	wxXmlDocument xmlDoc;
	wxXmlNode* pRootNode = this->SerializeObject(nullptr);
	xmlDoc.SetRoot(pRootNode);

	wxStringOutputStream sstream;
	xmlDoc.Save(sstream);

	return sstream.GetString();
}

void am::StoryElement::EditTo(const am::StoryElement& other)
{
	name = other.name;
	role = other.role;
	mShortAttributes = other.mShortAttributes;
	mLongAttributes = other.mLongAttributes;
	aliases = other.aliases;
	dbID = other.dbID;
	image = other.image;
	relatedElements = other.relatedElements;
}

am::SQLEntry am::StoryElement::GenerateSQLEntrySimple()
{
	am::SQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "story_elements";

	sqlEntry.integers["role"] = role;

	for ( auto& it : mShortAttributes )
	{
		sqlEntry.strings[it.first] = it.second;
	}

	for ( auto& it : mLongAttributes )
	{
		sqlEntry.strings[it.first] = it.second;
	}

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

am::SQLEntry am::StoryElement::GenerateSQLEntry()
{
	return GenerateSQLEntrySimple();
}

am::SQLEntry am::StoryElement::GenerateSQLEntryForId()
{
	am::SQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "story_elements";

	sqlEntry.strings["class"] = GetClassInfo()->GetClassName();

	return sqlEntry;
}

bool am::StoryElement::operator<(const am::StoryElement& other) const
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

	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

bool am::StoryElement::operator==(const am::StoryElement& other) const
{
	return name == other.name;

}


wxIMPLEMENT_DYNAMIC_CLASS(am::TangibleElement, am::StoryElement);

bool am::TangibleElement::IsInBook(Book* book) const
{
	for ( am::Document* const& pDocument : documents )
	{
		if ( pDocument->book == book )
			return true;
	}

	return false;
}

am::Document* am::TangibleElement::GetFirstDocument() const
{
	am::Document* pFirstDocument = nullptr;
	for ( am::Document* const& pDocument : documents )
	{
		if ( !pFirstDocument || *pDocument < *pFirstDocument )
			pFirstDocument = pDocument;
	}

	return pFirstDocument;
}

am::Document* am::TangibleElement::GetLastDocument() const
{
	am::Document* pLastDocument = nullptr;
	for ( am::Document* const& pDocument : documents )
	{
		if ( !pLastDocument || *pLastDocument < *pDocument )
			pLastDocument = pDocument;
	}

	return pLastDocument;
}

am::Document* am::TangibleElement::GetFirstDocumentInBook(Book* book) const
{
	return nullptr;
}

am::Document* am::TangibleElement::GetLastDocumentInBook(Book* book) const
{
	return nullptr;
}

void am::TangibleElement::EditTo(const am::StoryElement& other)
{
	am::StoryElement::EditTo(other);

	if ( other.IsKindOf(wxCLASSINFO(am::TangibleElement)) )
		documents = ((am::TangibleElement*)&other)->documents;
}

bool am::TangibleElement::operator<(const am::TangibleElement& other) const
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
	{
		am::Document* pFirstDocument = GetFirstDocument();
		am::Document* pOtherFirstDocument = other.GetFirstDocument();

		if ( pFirstDocument && pOtherFirstDocument )
			return *pFirstDocument < *pOtherFirstDocument;
		else if ( !pFirstDocument && pOtherFirstDocument )
			return false;
		else if ( pFirstDocument && !pOtherFirstDocument )
			return true;

		break;
	}
	case CompLast:
	{
		am::Document* pLastDocument = GetLastDocument();
		am::Document* pOtherLastDocument = other.GetLastDocument();

		if ( pLastDocument && pOtherLastDocument )
			return *pOtherLastDocument < *pLastDocument;
		else if ( !pLastDocument && pOtherLastDocument )
			return false;
		else if ( pLastDocument && !pOtherLastDocument )
			return true;

		break;
	}
	default:
		break;
	}

	return name.Lower() < other.name.Lower();
}

am::TangibleElement::~TangibleElement()
{
	for ( am::Document*& pDocument : documents )
	{
		for ( am::TangibleElement*& pElement : pDocument->vTangibleElements )
		{
			if ( pElement == this )
			{
				pDocument->vTangibleElements.erase(&pElement);
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// am::Character ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(am::Character, am::TangibleElement);

am::Character::Character()
{
	XS_SERIALIZE_BOOL(isAlive, "isAlive");
}

void am::Character::EditTo(const am::StoryElement& other)
{
	am::TangibleElement::EditTo(other);

	if ( other.IsKindOf(wxCLASSINFO(am::Character)) )
		isAlive = ((am::Character*)&other)->isAlive;
}

bool am::Character::operator<(const am::Character& other) const
{
	elCompType = cCompType;
	return this->am::TangibleElement::operator<(other);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// am::Location ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(am::Location, am::TangibleElement);

am::Location::Location()
{
	XS_SERIALIZE_INT(type, "locationType");
}

void am::Location::EditTo(const am::StoryElement& other)
{
	am::TangibleElement::EditTo(other);

	if ( other.IsKindOf(wxCLASSINFO(am::Location)) )
		type = ((am::Location*)&other)->type;
}

bool am::Location::operator<(const am::Location& other) const
{
	elCompType = lCompType;
	return this->am::TangibleElement::operator<(other);
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// am::Item /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(am::Item, am::TangibleElement);

am::Item::Item()
{
	XS_SERIALIZE_BOOL_EX(isMagic, "isMagic", false);
	XS_SERIALIZE_BOOL_EX(isManMade, "isManMade", true);
}

void am::Item::EditTo(const am::StoryElement& other)
{
	am::TangibleElement::EditTo(other);

	if ( other.IsKindOf(wxCLASSINFO(am::Item)) )
	{
		am::Item* pItem = (am::Item*)&other;

		isMagic = pItem->isMagic;
		isManMade = pItem->isManMade;
	}
}

bool am::Item::operator<(const am::Item& other) const
{
	elCompType = iCompType;
	return this->am::TangibleElement::operator<(other);
}
