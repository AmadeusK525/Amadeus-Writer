#ifndef LOCATION_H_
#define LOCATION_H_
#pragma once

#include "wx/bitmap.h"

#include <fstream>
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::pair;

struct Location {
    string name{""}, background{""}, natural{""}, architecture{""},
        type{""}, economy{""}, culture{""}, importance{""};

    vector<pair<string, string>> custom{};

    int chapters = 0;

    bool hasAppeared = false;
    int firstChap = 0;

    wxImage image{};

    void save(std::ofstream& out);
    void load(std::ifstream& in);
};

#endif