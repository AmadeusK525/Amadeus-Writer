#include "Character.h"
#include "MainFrame.h"

#include "wxmemdbg.h"

CompType Character::compType = CompRole;

void Character::save(std::ofstream& out) {
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

void Character::load(std::ifstream& in) {
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

        size = role;
        in.read(&size, sizeof(char));
    }
}

bool Character::operator<(const Character& other) const{
    switch (compType) {
    case CompRole:
        if (role != other.role) {
            return role < other.role;
        }
    case CompName:
        return name.Lower() < other.name.Lower();
        break;
    }
}

bool Character::operator==(const Character& other) const {
    return name == other.name && role == other.role;
}