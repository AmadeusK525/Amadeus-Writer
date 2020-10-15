#pragma once

#include <wx/bitmap.h>

#include <string>
#include <fstream>

struct Character {
    std::string name{}, sex{}, age{}, nat{},
        height{}, nick{}, role{}, appearance{},
        personality{}, backstory{};

    unsigned int chapters = 0;

    unsigned int firstChap = 0;
    bool hasAppeared = false;

    wxImage image{};


    void save(std::ofstream& out);
    void load(std::ifstream& in);
};

