#ifndef CHAPTER_H_
#define CHAPTER_H_
#pragma once

#include <wx\richtext\richtextbuffer.h>

#include "Note.h"

struct Scene {
    wxString name{ "" };
    wxRichTextBuffer content{};

    wxString pointOfView{ "" };

};

struct Chapter {
    wxString name{ "" };
    wxString synopsys{ "" };

    wxVector<Scene> scenes{};

    wxArrayString characters{};
    wxArrayString locations{};
    wxArrayString items{};
    wxVector<Note> notes{};

    int position = -1;

    Chapter() : characters(true), locations(true), items(true) {}

    bool HasRedNote();

    void Save(std::ofstream& out);
    void Load(std::ifstream& in);

    bool operator<(const Chapter& other) const;
    bool operator==(const Chapter& other) const;
};

WX_DEFINE_ARRAY_PTR(Chapter*, ChapterPtrArray);

#endif