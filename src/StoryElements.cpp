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


//void Character::Save(std::ofstream& out) {
//    if (out.is_open()) {
//        int size;
//
//        size = name.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(name.c_str(), size);
//
//        size = sex.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(sex.c_str(), size);
//
//        size = age.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(age.c_str(), size);
//
//        size = nat.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(nat.c_str(), size);
//
//        size = height.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(height.c_str(), size);
//
//        size = nick.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(nick.c_str(), size);
//
//        size = appearance.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(appearance.c_str(), size);
//
//        size = personality.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(personality.c_str(), size);
//
//        size = backstory.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(backstory.c_str(), size);
//
//        size = custom.size();
//        out.write((char*)&size, sizeof(int));
//
//        for (int i = 0; i < custom.size(); i++) {
//            size = custom[i].first.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(custom[i].first.c_str(), size);
//
//            size = custom[i].second.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(custom[i].second.c_str(), size);
//        }
//
//        size = role;
//        out.write((char*)&size, sizeof(int));
//    }
//}
//
//void Character::Load(std::ifstream& in) {
//    if (in.is_open()) {
//        int size;
//        char* data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        name = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        sex = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        age = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        nat = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        height = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        nick = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        appearance = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        personality = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        backstory = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//
//        int size2;
//        for (int i = 0; i < size; i++) {
//            custom.push_back(pair<wxString, wxString>(wxString(), wxString()));
//
//            in.read((char*)&size2, sizeof(int));
//            data = new char[size2];
//            in.read(data, size2);
//            custom[i].first = data;
//            delete[] data;
//
//            in.read((char*)&size2, sizeof(int));
//            data = new char[size2];
//            in.read(data, size2);
//            custom[i].second = data;
//            delete[] data;
//        }
//
//        in.read((char*)&size, sizeof(int));
//        role = (Role)size;
//    }
//}

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

        customDoc.needsForeign = true;
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


//void Location::Save(std::ofstream& out) {
//    if (out.is_open()) {
//        int size;
//
//        size = name.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(name.c_str(), size);
//
//        size = general.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(general.c_str(), size);
//
//        size = natural.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(natural.c_str(), size);
//
//        size = architecture.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(architecture.c_str(), size);
//        
//        size = politics.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(politics.c_str(), size);
//
//        size = economy.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(economy.c_str(), size);
//
//        size = culture.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(culture.c_str(), size);
//
//        size = custom.size();
//        out.write((char*)&size, sizeof(int));
//
//        for (int i = 0; i < custom.size(); i++) {
//            size = custom[i].first.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(custom[i].first.c_str(), size);
//
//            size = custom[i].second.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(custom[i].second.c_str(), size);
//        }
//
//        size = role;
//        out.write((char*)&size, sizeof(int));
//    }
//}
//
//void Location::Load(std::ifstream& in) {
//    if (in.is_open()) {
//        int size;
//        char* data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        name = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        general = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        natural = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        architecture = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        politics = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        economy = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//        data = new char[size];
//        in.read(data, size);
//        culture = data;
//        delete[] data;
//
//        in.read((char*)&size, sizeof(int));
//
//        int size2;
//        for (int i = 0; i < size; i++) {
//            custom.push_back(pair<wxString, wxString>(wxString(), wxString()));
//
//            in.read((char*)&size2, sizeof(int));
//            data = new char[size2];
//            in.read(data, size2);
//            custom[i].first = data;
//            delete[] data;
//
//            in.read((char*)&size2, sizeof(int));
//            data = new char[size2];
//            in.read(data, size2);
//            custom[i].second = data;
//            delete[] data;
//        }
//
//        in.read((char*)&size, sizeof(int));
//        role = (Role)size;
//    }
//}

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
