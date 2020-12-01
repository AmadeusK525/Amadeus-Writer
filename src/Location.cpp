#include "Location.h"
#include "MainFrame.h"

#include "wxmemdbg.h"

void Location::save(std::ofstream& out) {
    if (out.is_open()) {
        int size;

        size = name.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(name.c_str(), size);

        size = background.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(background.c_str(), size);

        size = natural.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(natural.c_str(), size);

        size = architecture.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(architecture.c_str(), size);

        size = type.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(type.c_str(), size);

        size = economy.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(economy.c_str(), size);

        size = culture.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(culture.c_str(), size);

        size = importance.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(importance.c_str(), size);

        out.write((char*)&chapters, sizeof(int));

        out.write((char*)&firstChap, sizeof(int));

        out.write((char*)&hasAppeared, sizeof(bool));
    }
}
void Location::load(std::ifstream& in) {
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
        background = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        natural = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        architecture = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        type = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        economy = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        culture = data;
        delete data;

        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        in.read(data, size);
        importance = data;
        delete data;

        in.read((char*)&chapters, sizeof(int));

        in.read((char*)&firstChap, sizeof(int));

        in.read((char*)&hasAppeared, sizeof(bool));
    }
}
