#ifndef CHARACTER_H_
#define CHARACTER_H_
#pragma once

#include <wx/bitmap.h>

#include <string>
#include <vector>
#include <fstream>

#include "Chapter.h"

using std::string;
using std::vector;
using std::pair;

enum CompType {
    CompRole,
    CompName
};

enum Role {
    cProtagonist,
    cSupporting,
    cSecondary,
    cVillian,
    None
};

struct Character {
    wxString name{ "" };

    string sex{ "" }, age{ "" }, nat{ "" },
        height{ "" }, nick{ "" }, appearance{ "" },
        personality{ "" }, backstory{ "" };

    Role role{ None };

    vector<pair<string, string>> custom{ 0 };

    ChapterPtrArray chapters{};
    wxImage image{};

    static CompType compType;

    Character() = default;

    void save(std::ofstream& out);
    void load(std::ifstream& in);

    bool operator<(const Character& other) const;
    bool operator==(const Character& other) const;
};

#endif