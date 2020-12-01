#pragma once

#include "wx/bitmap.h"

#include <fstream>
#include <string>

struct Location {
    std::string name{}, background{}, natural{}, architecture{},
        type{}, economy{}, culture{}, importance{};

    int chapters = 0;

    bool hasAppeared = false;
    int firstChap = 0;

    wxImage image;

    void save(std::ofstream& out);
    void load(std::ifstream& in);
};

