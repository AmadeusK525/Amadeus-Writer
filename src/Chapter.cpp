#include "Chapter.h"
#include "MainFrame.h"
#include "boost/filesystem.hpp"

#include "wxmemdbg.h"

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif

namespace fs = boost::filesystem;

void Chapter::save(std::ofstream& out) {
    if (out.is_open()) {
        int size;

        size = name.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(name.c_str(), size);

        size = summary.size() + 1;
        out.write(reinterpret_cast<char*>(&size), sizeof(int));
        out.write(summary.c_str(), size);

        int writeMe;

        writeMe = characters.size();
        out.write((char*)&writeMe, sizeof(int));
        
        for (auto it = characters.begin(); it != characters.end(); it++) {
            size = it->size() + 1;
            out.write(reinterpret_cast<char*>(&size), sizeof(int));
            out.write(it->c_str(), size);
        }

        writeMe = locations.size();
        out.write((char*)&writeMe, sizeof(int));

        for (auto it = locations.begin(); it != locations.end(); it++) {
            size = it->size() + 1;
            out.write(reinterpret_cast<char*>(&size), sizeof(int));
            out.write(it->c_str(), size);
        }

        writeMe = notes.size();
        out.write((char*)&writeMe, sizeof(int));

        for (auto it = notes.begin(); it != notes.end(); it++) {
            size = it->name.size() + 1;
            out.write(reinterpret_cast<char*>(&size), sizeof(int));
            out.write(it->name.c_str(), size);

            size = it->content.size() + 1;
            out.write(reinterpret_cast<char*>(&size), sizeof(int));
            out.write(it->content.c_str(), size);

            out.write((char*)&it->isDone, sizeof(bool));
        }
        
        out.write((char*)&position, sizeof(int));
    }

    content.SaveFile(MainFrame::currentDocFolder + "\\Files\\Chapters\\" + std::to_string(position) + " - " + name + ".xml", wxRICHTEXT_TYPE_XML);
}

void Chapter::load(std::ifstream& in) {
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
        summary = data;
        delete data;

        int number;
        string tempName;

        in.read((char*)&number, sizeof(int));
        for (int i = 0; i < number; i++) {
            in.read(reinterpret_cast<char*>(&size), sizeof(int));
            data = new char[size];
            in.read(data, size);
            tempName = data;
            delete data;

            characters.push_back(tempName);
        }

        in.read((char*)&number, sizeof(int));
        for (int i = 0; i < number; i++) {
            in.read(reinterpret_cast<char*>(&size), sizeof(int));
            data = new char[size];
            in.read(data, size);
            tempName = data;
            delete data;

            locations.push_back(tempName);
        }

        in.read((char*)&number, sizeof(int));
        bool k;
        string tempName2;
        for (int i = 0; i < number; i++) {
            in.read(reinterpret_cast<char*>(&size), sizeof(int));
            data = new char[size];
            in.read(data, size);
            tempName2 = data;
            delete data;

            in.read(reinterpret_cast<char*>(&size), sizeof(int));
            data = new char[size];
            in.read(data, size);
            tempName = data;
            delete data;

            in.read((char*)&k, sizeof(bool));

            Note note(tempName, tempName2);
            note.isDone = k;

            notes.push_back(note);
        }

        in.read((char*)&position, sizeof(int));

        if (fs::exists(MainFrame::currentDocFolder + "\\Files\\Chapters\\" +
            std::to_string(position) + " - " + name + ".xml")) {
            content.LoadFile(MainFrame::currentDocFolder + "\\Files\\Chapters\\" +
                std::to_string(position) + " - " + name + ".xml", wxRICHTEXT_TYPE_XML);
        }
    }
}
