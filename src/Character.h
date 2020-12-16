#pragma once

#include <wx/bitmap.h>

#include <string>
#include <vector>
#include <fstream>

using std::string;
using std::vector;
using std::pair;

struct Character {
    string name{""}, sex{""}, age{""}, nat{""},
        height{""}, nick{""}, role{""}, appearance{""},
        personality{""}, backstory{""};

    vector<pair<string, string>> custom{0};

    unsigned int chapters = 0;

    unsigned int firstChap = 0;
    bool hasAppeared = false;

    wxImage image{};


    void save(std::ofstream& out);
    void load(std::ifstream& in);
};

