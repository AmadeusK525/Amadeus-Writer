#ifndef BOOKELEMENTS_H_
#define BOOKELEMENTS_H_
#pragma once

#include <wx\wx.h>
#include <wx\richtext\richtextbuffer.h>

#include "StoryElements.h"
#include "Note.h"


/////////////////////////////////////////////////////////////////
///////////////////////////// Scene /////////////////////////////
/////////////////////////////////////////////////////////////////


struct Scene {
    wxString name{ "" };
    wxRichTextBuffer content{};

    wxString pointOfView{ "" };
};


/////////////////////////////////////////////////////////////////
/////////////////////////// Chapter /////////////////////////////
/////////////////////////////////////////////////////////////////


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
WX_DEFINE_ARRAY_PTR(Chapter*, ChapterPtrArray);

/////////////////////////////////////////////////////////////////
//////////////////////////// Section //////////////////////////// 
/////////////////////////////////////////////////////////////////


enum SectionType {
    Section_Part,
    Section_FrontMatter,
    Section_BackMatter
};

struct Section {
    wxString name{ "" };
    wxString description{ "" };

    unsigned int pos = -1;

    wxVector<Chapter> chapters{};

    SectionType type{ Section_Part };
};


/////////////////////////////////////////////////////////////////
///////////////////////////// Book //////////////////////////////
/////////////////////////////////////////////////////////////////


struct Book {
	wxString title{ "" };

	wxString publisher{ "" };

	wxString author{ "" },
        genre{ "" },
        description{ "" },
        synopsys{ "" };

    wxVector<Section> sections{};

	wxVector<Character> characters{};
	wxVector<Location> locations{};
	wxVector<Item> items{};
	wxVector<Chapter> chapters{};
};


/////////////////////////////////////////////////////////////////
//////////////////////////// Project ////////////////////////////
/////////////////////////////////////////////////////////////////


class amdProject {
	wxVector<Book> books{};


};

#endif