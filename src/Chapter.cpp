#include "Chapter.h"
#include "MyApp.h"

#include <boost/filesystem.hpp>

#include "wxmemdbg.h"

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif

namespace fs = boost::filesystem;

bool Chapter::HasRedNote() {
    for (int i = 0; i < notes.size(); i++) {
        if (notes[i].isDone == false)
            return true;
    }

    return false;
}

void Chapter::Save(std::ofstream& out) {
    if (out.is_open()) {
        char size;

        size = name.size() + 1;
        out.write(&size, sizeof(char));
        out.write(name.c_str(), size);

        size = summary.size() + 1;
        out.write(&size, sizeof(char));
        out.write(summary.c_str(), size);

        char writeMe;

        writeMe = characters.size();
        out.write(&writeMe, sizeof(char));
        
        for (auto& it : characters) {
            size = it.size() + 1;
            out.write(&size, sizeof(char));
            out.write(it.c_str(), size);
        }

        writeMe = locations.size();
        out.write(&writeMe, sizeof(char));

        for (auto& it : locations) {
            size = it.size() + 1;
            out.write(&size, sizeof(char));
            out.write(it.c_str(), size);
        }

        writeMe = notes.size();
        out.write(&writeMe, sizeof(char));

        for (auto it = notes.begin(); it != notes.end(); it++) {
            size = it->name.size() + 1;
            out.write(&size, sizeof(char));
            out.write(it->name.c_str(), size);

            size = it->content.size() + 1;
            out.write(&size, sizeof(char));
            out.write(it->content.c_str(), size);

            writeMe = it->isDone;
            out.write(&writeMe, sizeof(char));
        }
        
        out.write(&position, sizeof(char));
    }

    amdProjectManager* man = amdGetManager();
    string path(man->GetPath(true).ToStdString() + "Files\\Chapters\\" +
        std::to_string(position) + " - " + name + ".xml");

    if (fs::exists(path))
        content.SaveFile(path, wxRICHTEXT_TYPE_XML);
}

void Chapter::Load(std::ifstream& in) {
    if (in.is_open()) {
        char size;
        char* data;
        amdProjectManager* manager = amdGetManager();

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        name = data;
        delete[] data;

        in.read(&size, sizeof(char));
        data = new char[size];
        in.read(data, size);
        summary = data;
        delete[] data;

        char number;
        string tempName;

        in.read(&number, sizeof(char));
        for (int i = 0; i < number; i++) {
            in.read(&size, sizeof(char));
            data = new char[size];
            in.read(data, size);
            tempName = data;
            delete[] data;

            characters.push_back(tempName);
        }

        in.read(&number, sizeof(char));
        for (int i = 0; i < number; i++) {
            in.read(&size, sizeof(char));
            data = new char[size];
            in.read(data, size);
            tempName = data;
            delete[] data;

            locations.push_back(tempName);
        }

        in.read(&number, sizeof(char));
        string tempName2;
        for (int i = 0; i < number; i++) {
            in.read(&size, sizeof(char));
            data = new char[size];
            in.read(data, size);
            tempName2 = data;
            delete[] data;

            in.read(&size, sizeof(char));
            data = new char[size];
            in.read(data, size);
            tempName = data;
            delete[] data;

            in.read(&size, sizeof(char));

            Note note(tempName, tempName2);
            note.isDone = size;

            notes.push_back(note);
        }

        in.read(&position, sizeof(char));

        string path(manager->GetPath(true).ToStdString() + "Files\\Chapters\\" +
            std::to_string(position) + " - " + name + ".xml");

        if (fs::exists(path))
            content.LoadFile(path, wxRICHTEXT_TYPE_XML);
    }
}

bool Chapter::operator<(const Chapter& other) const {
    return position < other.position;
}

bool Chapter::operator==(const Chapter& other) const {
    return name == other.name && position == other.position;
}