#include "Character.h"
#include "MainFrame.h"

#include "wxmemdbg.h"

void Character::save(std::ofstream& out) {
    if (out.is_open()) {
        int size;

        size = name.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(name.c_str(), size);

        size = sex.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(sex.c_str(), size);

        size = age.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(age.c_str(), size);
        
        size = nat.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(nat.c_str(), size);

        size = height.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(height.c_str(), size);

        size = nick.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(nick.c_str(), size);

        size = role.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(role.c_str(), size);

        size = appearance.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(appearance.c_str(), size);

        size = personality.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(personality.c_str(), size);

        size = backstory.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(backstory.c_str(), size);

        out.write((char*)&chapters, sizeof(int));

        out.write((char*)&firstChap, sizeof(int));

        out.write((char*)&hasAppeared, sizeof(bool));
    }
}

void Character::load(std::ifstream& in) {
    if (in.is_open()) {
        int size;
        char* data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        name = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        sex = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        age = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        nat = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        height = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        nick = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        role = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        appearance = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        personality = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        backstory = data;
        delete data;

        in.read((char*)&chapters, sizeof(int));
        
        in.read((char*)&firstChap, sizeof(int));

        in.read((char*)&hasAppeared, sizeof(bool));
    }
}