#include "StoryElements.h"
#include "BookElements.h"

#include <wx\mstream.h>
#include <wx\memory.h>

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


amDocument Character::GenerateDocument() {
    amDocument document;
    document.name = name;
    document.tableName = "characters";

    document.integers["role"] = role;
    
    document.strings["sex"] = sex;
    document.strings["age"] = age;
    document.strings["nationality"] = nat;
    document.strings["height"] = height;
    document.strings["nickname"] = nick;
    document.strings["appearance"] = appearance;
    document.strings["personality"] = personality;
    document.strings["backstory"] = backstory;

    if (image.IsOk()) {
        wxMemoryOutputStream stream;
        image.SaveFile(stream, image.GetType());

        wxMemoryBuffer buffer;
        stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

        document.memBuffers["image"] = buffer;
    }

    for (auto& it : custom) {
        amDocument customDoc;
        customDoc.tableName = "characters_custom";

        customDoc.strings["name"] = it.first;
        customDoc.strings["content"] = it.second;

        customDoc.specialForeign = true;
        customDoc.foreignKey.first = "character_id";

        document.documents.push_back(customDoc);
    }

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

void Character::operator=(const Character& other)  {
    name = other.name;
    role = other.role;
    image = other.image;
    sex = other.sex;
    age = other.age;
    nat = other.nat;
    nick = other.nick;
    appearance = other.appearance;
    personality = other.personality;
    backstory = other.backstory;
    height = other.height;
    cCompType = other.cCompType;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Location ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


amDocument Location::GenerateDocument() {
    amDocument document;
    document.name = name;
    document.tableName = "locations";

    document.integers["role"] = role;

    document.strings["general"] = general;
    document.strings["natural"] = natural;
    document.strings["architecture"] = architecture;
    document.strings["politics"] = politics;
    document.strings["economy"] = economy;
    document.strings["culture"] = culture;

    if (image.IsOk()) {
        wxMemoryOutputStream stream;
        image.SaveFile(stream, image.GetType());

        wxMemoryBuffer buffer;
        stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

        document.memBuffers["image"] = buffer;
    }

    for (auto& it : custom) {
        amDocument customDoc;
        customDoc.tableName = "locations_custom";

        customDoc.strings["name"] = it.first;
        customDoc.strings["content"] = it.second;

        customDoc.specialForeign = true;
        customDoc.foreignKey.first = "location_id";

        document.documents.push_back(customDoc);
    }

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
    lCompType = other.lCompType;
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Item /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

amDocument Item::GenerateDocument() {
    amDocument document;
    document.name = name;
    document.tableName = "items";

    document.integers["role"] = role;
    document.integers["isMagic"] = isMagic;
    document.integers["isManMade"] = isManMade;

    document.strings["general"] = general;
    document.strings["origin"] = origin;
    document.strings["backstory"] = backstory;
    document.strings["appearance"] = appearance;
    document.strings["usage"] = usage;
    document.strings["width"] = width;
    document.strings["height"] = height;
    document.strings["depth"] = depth;

    if (image.IsOk()) {
        wxMemoryOutputStream stream;
        image.SaveFile(stream, image.GetType());

        wxMemoryBuffer buffer;
        stream.CopyTo(buffer.GetWriteBuf(stream.GetLength()), stream.GetLength());

        document.memBuffers["image"] = buffer;
    }

    for (auto& it : custom) {
        amDocument customDoc;
        customDoc.tableName = "items_custom";

        customDoc.strings["name"] = it.first;
        customDoc.strings["content"] = it.second;

        customDoc.specialForeign = true;
        customDoc.foreignKey.first = "item_id";

        document.documents.push_back(customDoc);
    }

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
    iCompType = other.iCompType;
}
