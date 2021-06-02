#include "StoryElements.h"
#include "BookElements.h"

#include <wx\mstream.h>

#include "ProjectManager.h"

#include "wxmemdbg.h"

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


CompType StoryElement::elCompType = CompRole;
CompType Character::cCompType = CompRole;
CompType Location::lCompType = CompRole;
CompType Item::iCompType = CompRole;

wxIMPLEMENT_DYNAMIC_CLASS(StoryElement, wxObject);

StoryElement::StoryElement()
{
	XS_SERIALIZE_STRING(name, wxT("name"));
	XS_SERIALIZE_INT(role, wxT("role"));
	XS_SERIALIZE_ARRAYSTRING(aliases, wxT("aliases"));
	XS_SERIALIZE_PROPERTY(mShortAttributes, wxT("unordered_string_map"), "short_attributes");
	XS_SERIALIZE_PROPERTY(mLongAttributes, wxT("unordered_string_map"), "long_attributes");
}

void StoryElement::Save(amProjectSQLDatabase* db)
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

bool StoryElement::Update(amProjectSQLDatabase* db)
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

wxString StoryElement::GetXMLString()
{
	wxXmlDocument xmlDoc;
	wxXmlNode* pRootNode = this->SerializeObject(nullptr);
	xmlDoc.SetRoot(pRootNode);

	wxStringOutputStream sstream;
	xmlDoc.Save(sstream);

	return sstream.GetString();
}

void StoryElement::EditTo(const StoryElement& other)
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

amSQLEntry StoryElement::GenerateSQLEntrySimple()
{
	amSQLEntry sqlEntry;
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

amSQLEntry StoryElement::GenerateSQLEntry()
{
	return GenerateSQLEntrySimple();
}

amSQLEntry StoryElement::GenerateSQLEntryForId()
{
	amSQLEntry sqlEntry;
	sqlEntry.name = name;
	sqlEntry.tableName = "story_elements";

	sqlEntry.strings["class"] = GetClassInfo()->GetClassName();

	return sqlEntry;
}

bool StoryElement::operator<(const StoryElement& other) const
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

bool StoryElement::operator==(const StoryElement& other) const
{
	return name == other.name;

}


wxIMPLEMENT_DYNAMIC_CLASS(TangibleElement, StoryElement);

bool TangibleElement::IsInBook(Book* book) const
{
	for ( Document* const& pDocument : documents )
	{
		if ( pDocument->book == book )
			return true;
	}

	return false;
}

Document* TangibleElement::GetFirstDocument() const
{
	Document* pFirstDocument = nullptr;
	for ( Document* const& pDocument : documents )
	{
		if ( !pFirstDocument || *pDocument < *pFirstDocument )
			pFirstDocument = pDocument;
	}

	return pFirstDocument;
}

Document* TangibleElement::GetLastDocument() const
{
	Document* pLastDocument = nullptr;
	for ( Document* const& pDocument : documents )
	{
		if ( !pLastDocument || *pLastDocument < *pDocument )
			pLastDocument = pDocument;
	}

	return pLastDocument;
}

Document* TangibleElement::GetFirstDocumentInBook(Book* book) const
{
	return nullptr;
}

Document* TangibleElement::GetLastDocumentInBook(Book* book) const
{
	return nullptr;
}

void TangibleElement::EditTo(const StoryElement& other)
{
	StoryElement::EditTo(other);

	if ( other.IsKindOf(wxCLASSINFO(TangibleElement)) )
		documents = ((TangibleElement*)&other)->documents;
}

bool TangibleElement::operator<(const TangibleElement& other) const
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
		Document* pFirstDocument = GetFirstDocument();
		Document* pOtherFirstDocument = other.GetFirstDocument();

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
		Document* pLastDocument = GetLastDocument();
		Document* pOtherLastDocument = other.GetLastDocument();

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

TangibleElement::~TangibleElement()
{
	for ( Document*& pDocument : documents )
	{
		for ( TangibleElement*& pElement : pDocument->vTangibleElements )
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
////////////////////////////////// Character ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(Character, TangibleElement);

Character::Character()
{
	XS_SERIALIZE_BOOL(isAlive, "isAlive");
}

void Character::EditTo(const StoryElement& other)
{
	TangibleElement::EditTo(other);

	if ( other.IsKindOf(wxCLASSINFO(Character)) )
		isAlive = ((Character*)&other)->isAlive;
}

bool Character::operator<(const Character& other) const
{
	elCompType = cCompType;
	return this->TangibleElement::operator<(other);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Location ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(Location, TangibleElement);

Location::Location()
{
	XS_SERIALIZE_INT(type, "locationType");
}

void Location::EditTo(const StoryElement& other)
{
	TangibleElement::EditTo(other);

	if ( other.IsKindOf(wxCLASSINFO(Location)) )
		type = ((Location*)&other)->type;
}

bool Location::operator<(const Location& other) const
{
	elCompType = lCompType;
	return this->TangibleElement::operator<(other);
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Item /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(Item, TangibleElement);

Item::Item()
{
	XS_SERIALIZE_BOOL_EX(isMagic, "isMagic", false);
	XS_SERIALIZE_BOOL_EX(isManMade, "isManMade", true);
}

void Item::EditTo(const StoryElement& other)
{
	TangibleElement::EditTo(other);

	if ( other.IsKindOf(wxCLASSINFO(Item)) )
	{
		Item* pItem = (Item*)&other;

		isMagic = pItem->isMagic;
		isManMade = pItem->isManMade;
	}
}

bool Item::operator<(const Item& other) const
{
	elCompType = iCompType;
	return this->TangibleElement::operator<(other);
}
