#ifndef CHAPTER_H_
#define CHAPTER_H_
#pragma once

#include <wx\richtext\richtextbuffer.h>

#include "Note.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

struct Chapter {
    string name{ "" };
    string summary{ "" };

    wxRichTextBuffer content{};

    wxArrayString characters{};
    wxArrayString locations{};
    vector<Note> notes{};

    string pointOfView{""};

    int position = -1;

    bool hasRedNote();

    void save(std::ofstream& out);
    void load(std::ifstream& in);
};

WX_DEFINE_ARRAY_PTR(Chapter*, ChapterPtrArray);

#endif