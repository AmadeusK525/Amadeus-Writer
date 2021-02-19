#ifndef CHAPTER_H_
#define CHAPTER_H_
#pragma once

#include <wx\richtext\richtextbuffer.h>

#include "Note.h"

enum SectionType {
    Section_Part,
    Section_FrontMatter,
    Section_BackMatter
};

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

    void Save(std::ofstream& out) {}
    void Load(std::ifstream& in) {}

    bool operator<(const Chapter& other) const;
    bool operator==(const Chapter& other) const;
};


struct Section {
    wxString name{ "" };
    wxString description{ "" };

    unsigned int pos = -1;

    wxVector<Chapter> chapters{};

    SectionType type{ Section_Part };
};

WX_DEFINE_ARRAY_PTR(Chapter*, ChapterPtrArray);

#endif