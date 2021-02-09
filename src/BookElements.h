#ifndef BOOKELEMENTS_H_
#define BOOKELEMENTS_H_
#pragma once

#include <wx\wx.h>
#include <wx\filename.h>
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

    int pos = -1;
    int chapterID = -1;

    Scene(int chapterID, int pos) : chapterID(chapterID), pos(pos) {}

    amDocument GenerateDocument();
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
    int sectionID = -1;

    Chapter() : characters(true),
        locations(true),
        items(true) {}

    Chapter(int sectionID, int position) :
        sectionID(sectionID),
        position(position),
        characters(true),
        locations(true),
        items(true) {}

    bool Init();

    bool HasRedNote();

    amDocument GenerateDocument();
    amDocument GenerateDocumentForID();

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

    int bookID = -1;
    int pos = -1;

    wxVector<Chapter> chapters{};
    SectionType type{ Section_Part };

    Section(int bookID, int pos) : bookID(bookID), pos(pos) {}

    amDocument GenerateDocument();
    amDocument GenerateDocumentForID();
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

    int pos = 0;

    wxVector<Section> sections{};

    amDocument GenerateDocument(wxVector<int>& sectionsToGen = wxVector<int>());
    amDocument GenerateDocumentForID();
};


/////////////////////////////////////////////////////////////////
//////////////////////////// Project ////////////////////////////
/////////////////////////////////////////////////////////////////


struct amProject {
	wxVector<Book> books{};

    wxVector<Character> characters{};
    wxVector<Location> locations{};
    wxVector<Item> items{};

    wxFileName amFile{};

    wxVector<Character>& GetCharacters() { return characters; }
    wxVector<Location>& GetLocations() { return locations; }
    wxVector<Item>& GetItems() { return items; }
    wxVector<Chapter> GetChapters(int bookPos);
};

#endif