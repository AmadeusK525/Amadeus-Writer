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

        size = custom.size();
        out.write(reinterpret_cast<char*>(&size), sizeof(int));

        for (int i = 0; i < custom.size(); i++) {
            size = custom[i].first.size() + 1;
            out.write(reinterpret_cast<char*>(&size), sizeof(int));
            out.write(custom[i].first.c_str(), size);

            size = custom[i].second.size() + 1;
            out.write(reinterpret_cast<char*>(&size), sizeof(int));
            out.write(custom[i].second.c_str(), size);
        }

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

        in.read(reinterpret_cast<char*>(&size), sizeof(int));

        int size2;
        for (int i = 0; i < size; i++) {
            custom.push_back(pair<string, string>(string(), string()));

            in.read(reinterpret_cast<char*>(&size2), sizeof(int));
            data = new char[size2];
            in.read(data, size2);
            custom[i].first = data;
            delete data;

            in.read(reinterpret_cast<char*>(&size2), sizeof(int));
            data = new char[size2];
            in.read(data, size2);
            custom[i].second = data;
            delete data;
        }

        in.read((char*)&chapters, sizeof(int));

        in.read((char*)&firstChap, sizeof(int));

        in.read((char*)&hasAppeared, sizeof(bool));
    }
}
