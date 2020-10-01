#pragma once

#include <string>

using std::string;

struct Note {
    string content{};
    string name{};

    bool isDone = false;

    Note(string content, string name);
};

