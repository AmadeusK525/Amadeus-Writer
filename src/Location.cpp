#include "Location.h"
#include "MainFrame.h"

#include "wxmemdbg.h"

CompType Location::compType = CompRole;

void Location::save(std::ofstream& out) {
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

        size = importance.size() + 1;
        out.write(&size, sizeof(char));
        out.write(importance.c_str(), size);

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
    }
}
void Location::load(std::ifstream& in) {
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
        data = new char[size];
        in.read(data, size);
        importance = data;
        delete[] data;

        in.read(&size, sizeof(char));

        char size2;
        for (int i = 0; i < size; i++) {
            custom.push_back(pair<string, string>(string(), string()));

            in.read((char*)&size2, sizeof(int));
            data = new char[size2];
            in.read(data, size2);
            custom[i].first = data;
            delete[] data;

            in.read((char*)&size2, sizeof(int));
            data = new char[size2];
            in.read(data, size2);
            custom[i].second = data;
            delete[] data;
        }

    }
}

bool Location::operator<(const Location& other) const {
    switch (compType) {
    case CompRole:
        if (importance == "High" && other.importance == "Low")
            return true;
        else if (importance == "Low" && other.importance == "High")
            return false;
    case CompName:
        return name.Lower() < other.name.Lower();
        break;
    }
}

bool Location::operator==(const Location& other) const {
    return name == other.name;
}
