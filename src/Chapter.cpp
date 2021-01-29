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

//void Chapter::Save(std::ofstream& out) {
//    if (out.is_open()) {
//        int size;
//
//        size = name.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(name.c_str(), size);
//
//        size = synopsys.size() + 1;
//        out.write((char*)&size, sizeof(int));
//        out.write(synopsys.c_str(), size);
//
//        int writeMe;
//
//        writeMe = characters.size();
//        out.write((char*)&writeMe, sizeof(int));
//        
//        for (auto& it : characters) {
//            size = it.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(it.c_str(), size);
//        }
//
//        writeMe = locations.size();
//        out.write((char*)&writeMe, sizeof(int));
//
//        for (auto& it : locations) {
//            size = it.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(it.c_str(), size);
//        }
//
//        writeMe = items.size();
//        out.write((char*)&writeMe, sizeof(int));
//
//        for (auto& it : items) {
//            size = it.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(it.c_str(), size);
//        }
//
//        writeMe = notes.size();
//        out.write((char*)&writeMe, sizeof(int));
//
//        for (auto it = notes.begin(); it != notes.end(); it++) {
//            size = it->name.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(it->name.c_str(), size);
//
//            size = it->content.size() + 1;
//            out.write((char*)&size, sizeof(int));
//            out.write(it->content.c_str(), size);
//
//            writeMe = it->isDone;
//            out.write((char*)&writeMe, sizeof(int));
//        }
//        
//        out.write((char*)&position, sizeof(int));
//    }
//
//    amdProjectManager* man = amdGetManager();
//    wxString path(man->GetPath(true).ToStdString() + "Files\\Chapters\\");
//
//    if (fs::exists(path.ToStdString()))
//        for (auto& it : scenes)
//            it.content.SaveFile(path << position << " - " << name << " / " << it.name << ".xml");
//}
//
//void Chapter::Load(std::ifstream& in) {
//    if (in.is_open()) {
//        int size;
//        char* data;
//        amdProjectManager* manager = amdGetManager();
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
//        synopsys = data;
//        delete[] data;
//
//        int number;
//        wxString tempName;
//
//        in.read((char*)&number, sizeof(int));
//        for (int i = 0; i < number; i++) {
//            in.read((char*)&size, sizeof(int));
//            data = new char[size];
//            in.read(data, size);
//            tempName = data;
//            delete[] data;
//
//            characters.push_back(tempName);
//        }
//
//        in.read((char*)&number, sizeof(int));
//        for (int i = 0; i < number; i++) {
//            in.read((char*)&size, sizeof(int));
//            data = new char[size];
//            in.read(data, size);
//            tempName = data;
//            delete[] data;
//
//            locations.push_back(tempName);
//        }
//
//        in.read((char*)&number, sizeof(int));
//        for (int i = 0; i < number; i++) {
//            in.read((char*)&size, sizeof(int));
//            data = new char[size];
//            in.read(data, size);
//            tempName = data;
//            delete[] data;
//
//            items.push_back(tempName);
//        }
//
//        in.read((char*)&number, sizeof(int));
//        wxString tempName2;
//        for (int i = 0; i < number; i++) {
//            in.read((char*)&size, sizeof(int));
//            data = new char[size];
//            in.read(data, size);
//            tempName2 = data;
//            delete[] data;
//
//            in.read((char*)&size, sizeof(int));
//            data = new char[size];
//            in.read(data, size);
//            tempName = data;
//            delete[] data;
//
//            in.read((char*)&size, sizeof(int));
//
//            Note note(tempName, tempName2);
//            note.isDone = size;
//
//            notes.push_back(note);
//        }
//
//        in.read((char*)&position, sizeof(int));
//
//        wxString path(manager->GetPath(true).ToStdString() + "Files\\Chapters\\" +
//            std::to_string(position) + " - " + name + ".xml");
//
//        if (fs::exists(path.ToStdString()))
//            content.LoadFile(path, wxRICHTEXT_TYPE_XML);
//    }
//}

bool Chapter::operator<(const Chapter& other) const {
    return position < other.position;
}

bool Chapter::operator==(const Chapter& other) const {
    return name == other.name && position == other.position;
}