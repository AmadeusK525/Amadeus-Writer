#ifndef BOOKELEMENTS_H_
#define BOOKELEMENTS_H_
#pragma once

#include <wx\wx.h>
#include <wx\filename.h>
#include <wx\richtext\richtextbuffer.h>

#include "StoryElements.h"

/////////////////////////////////////////////////////////////////
///////////////////////////// Scene /////////////////////////////
/////////////////////////////////////////////////////////////////


struct Scene {
    wxString name{ "" };
    wxRichTextBuffer content{};

    wxString pointOfView{ "" };

    int pos = -1;
    int id = -1;
    int chapterID = -1;

    Scene(int chapterID, int pos) : chapterID(chapterID), pos(pos) {}

    void SetId(int id) { this->id = id; }

    amDocument GenerateDocument();
    amDocument GenerateDocumentForId();
};


/////////////////////////////////////////////////////////////////
/////////////////////////// Chapter /////////////////////////////
/////////////////////////////////////////////////////////////////


struct Note {
    wxString name{ "" };
    wxString content{ "" };

    bool isDone = false;

    Note(wxString content, wxString name);

    amDocument GenerateDocument();
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
    int id = -1;
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
    void SetId(int id) { this->id = id; }

    bool HasRedNote();

    void Save(wxSQLite3Database* db);
    bool Update(wxSQLite3Database* db, bool updateScenes, bool updateNotes);

    amDocument GenerateDocumentSimple();
    amDocument GenerateDocument();
    amDocument GenerateDocumentForId();

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

    int pos = -1;
    int id = -1;
    int bookID = -1;

    wxVector<Chapter> chapters{};
    SectionType type{ Section_Part };

    Section(int bookID, int pos) : bookID(bookID), pos(pos) {}

    void SetId(int id) { this->id = id; }

    void Save(wxSQLite3Database* db);
    bool Update(wxSQLite3Database* db, bool updateChapters);

    amDocument GenerateDocumentSimple();
    amDocument GenerateDocument();
    amDocument GenerateDocumentForId();
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
    int id = -1;

    wxVector<Section> sections{};

    Book() = default;

    Book(int pos) : pos(pos) {}

    bool Init();

    void SetId(int id) { this->id = id; }

    void Save(wxSQLite3Database* db);
    bool Update(wxSQLite3Database* db, bool updateSections, bool updateChapters);

    amDocument GenerateDocumentSimple();
    amDocument GenerateDocument(wxVector<int>& sectionsToGen = wxVector<int>());
    amDocument GenerateDocumentForId();
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
    wxVector<Chapter>& GetChapters(int bookPos, int sectionPos);

    wxVector<Chapter> GetChapters(int bookPos);
};

#endif