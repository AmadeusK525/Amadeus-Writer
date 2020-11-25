#ifndef CHAPTER_H_
#define CHAPTER_H_
#pragma once

#include "wx/richtext/richtextbuffer.h"

#include "Character.h"
#include "Location.h"
#include "Note.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

struct Chapter {
    string name{};
    string summary{};
   
    wxRichTextBuffer content{};

    vector<string> characters{};
    vector<string> locations{};
    vector<Note> notes{};

    string pointOfView = "";

    int position{};

    void save(std::ofstream& out);
    void load(std::ifstream& in);
};

#endif