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

    char position = -1;

    bool HasRedNote();

    void Save(std::ofstream& out);
    void Load(std::ifstream& in);

    bool operator<(const Chapter& other) const;
    bool operator==(const Chapter& other) const;
};

WX_DEFINE_ARRAY_PTR(Chapter*, ChapterPtrArray);

#endif