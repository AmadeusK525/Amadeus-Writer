#include "Elements.h"

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
        i = chapters.Count();
        j = other.chapters.Count();

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


void Character::Save(std::ofstream& out) {
    if (out.is_open()) {
        char size;

        size = name.size() + 1;
        out.write(&size, sizeof(char));
        out.write(name.c_str(), size);

        size = sex.size() + 1;
        out.write(&size, sizeof(char));
        out.write(sex.c_str(), size);

        size = age.size() + 1;
        out.write(&size, sizeof(char));
        out.write(age.c_str(), size);

        size = nat.size() + 1;
        out.write(&size, sizeof(char));
        out.write(nat.c_str(), size);

        size = height.size() + 1;
        out.write(&size, sizeof(char));
        out.write(height.c_str(), size);

        size = nick.size() + 1;
        out.write(&size, sizeof(char));
        out.write(nick.c_str(), size);

        size = appearance.size() + 1;
        out.write(&size, sizeof(char));
        out.write(appearance.c_str(), size);

        size = personality.size() + 1;
        out.write(&size, sizeof(char));
        out.write(personality.c_str(), size);

        size = backstory.size() + 1;
        out.write(&size, sizeof(char));
        out.write(backstory.c_str(), size);

        size = custom.size();
        out.write(&size, sizeof(char));

        for (int i = 0; i < custom.size(); i++) {
            size = custom[i].first.size() + 1;
            out.write(&size, sizeof(char));
            out.write(custom[i].first.c_str(), size);

            size = custom[i].second.size() + 1;
            out.write(&size, sizeof(char));
            out.write(custom[i].second.c_str(), size);
        }

        size = role;
        out.write(&size, sizeof(char));
    }
}

void Character::Load(std::ifstream& in) {
    if (in.is_open()) {
        char size;
        char* data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        name = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        sex = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        age = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        nat = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        height = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        nick = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        appearance = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        personality = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        backstory = data;
        delete[] data;

        in.read(&size, sizeof(char));

        char size2;
        for (int i = 0; i < size; i++) {
            custom.push_back(pair<string, string>(string(), string()));

            in.read(&size2, sizeof(char));
            data = new char[size2];
            in.read(data, size2);
            custom[i].first = data;
            delete[] data;

            in.read(&size2, sizeof(char));
            data = new char[size2];
            in.read(data, size2);
            custom[i].second = data;
            delete[] data;
        }

        in.read(&size, sizeof(char));
        role = (Role)size;
    }
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
        i = chapters.Count();
        j = other.chapters.Count();

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


void Location::Save(std::ofstream& out) {
    if (out.is_open()) {
        char size;

        size = name.size() + 1;
        out.write(&size, sizeof(char));
        out.write(name.c_str(), size);

        size = general.size() + 1;
        out.write(&size, sizeof(char));
        out.write(general.c_str(), size);

        size = natural.size() + 1;
        out.write(&size, sizeof(char));
        out.write(natural.c_str(), size);

        size = architecture.size() + 1;
        out.write(&size, sizeof(char));
        out.write(architecture.c_str(), size);

        size = politics.size() + 1;
        out.write(&size, sizeof(char));
        out.write(politics.c_str(), size);

        size = economy.size() + 1;
        out.write(&size, sizeof(char));
        out.write(economy.c_str(), size);

        size = culture.size() + 1;
        out.write(&size, sizeof(char));
        out.write(culture.c_str(), size);

        size = custom.size();
        out.write(&size, sizeof(char));

        for (int i = 0; i < custom.size(); i++) {
            size = custom[i].first.size() + 1;
            out.write(&size, sizeof(char));
            out.write(custom[i].first.c_str(), size);

            size = custom[i].second.size() + 1;
            out.write(&size, sizeof(char));
            out.write(custom[i].second.c_str(), size);
        }

        size = role;
        out.write(&size, sizeof(char));
    }
}

void Location::Load(std::ifstream& in) {
    if (in.is_open()) {
        char size;
        char* data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        name = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        general = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        natural = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        architecture = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        politics = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        economy = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        culture = data;
        delete[] data;

        in.read(&size, sizeof(char));

        char size2;
        for (int i = 0; i < size; i++) {
            custom.push_back(pair<string, string>(string(), string()));

            in.read(&size2, sizeof(char));
            data = new char[size2];
            in.read(data, size2);
            custom[i].first = data;
            delete[] data;

            in.read(&size2, sizeof(char));
            data = new char[size2];
            in.read(data, size2);
            custom[i].second = data;
            delete[] data;
        }

        in.read(&size, sizeof(char));
        role = (Role)size;
    }
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
        i = chapters.Count();
        j = other.chapters.Count();

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


void Item::Save(std::ofstream& out) {

}

void Item::Load(std::ifstream& in) {

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
        i = chapters.Count();
        j = other.chapters.Count();

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

bool Item::operator=(const Item& other) {
    return false;
}
