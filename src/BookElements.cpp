#include "BookElements.h"

#include <wx\sstream.h>

#include "MyApp.h"

/////////////////////////////////////////////////////////////////
///////////////////////////// Scene /////////////////////////////
/////////////////////////////////////////////////////////////////


amDocument Scene::GenerateDocument() {
    amDocument document;
    document.tableName = "scenes";
    document.name = name;

    document.integers["position"] = pos;
    document.integers["chapter_id"] = chapterID;
    
    wxStringOutputStream stream;
    content.SaveFile(stream, wxRICHTEXT_TYPE_XML);

    document.strings["content"] = stream.GetString();

    return document;
}


/////////////////////////////////////////////////////////////////
/////////////////////////// Chapter /////////////////////////////
/////////////////////////////////////////////////////////////////


bool Chapter::Init() {
    if (scenes.empty()) {
        Scene scene(amGetManager()->GetDocumentId(GenerateDocumentForID()), 1);
        amGetManager()->InsertDocument(scene.GenerateDocument());

        scenes.push_back(scene);
        return true;
    }

    return false;
}

bool Chapter::HasRedNote() {
    for (unsigned int i = 0; i < notes.size(); i++) {
        if (notes[i].isDone == false)
            return true;
    }

    return false;
}

amDocument Chapter::GenerateDocumentSimple() {
    amDocument document;
    document.tableName = "chapters";
    document.name = name;

    document.integers["position"] = position;
    document.integers["section_id"] = sectionID;

    document.strings["synopsys"] = synopsys;

    return document;
}

amDocument Chapter::GenerateDocument() {
    amDocument document = GenerateDocumentSimple();
    
    for (auto& it : scenes)
        document.documents.push_back(it.GenerateDocument());

    return document;
}

amDocument Chapter::GenerateDocumentForID() {
    amDocument document;
    document.name = name;
    document.tableName = "chapters";

    document.specialForeign = true;
    document.foreignKey.first = "section_id";
    document.foreignKey.second = sectionID;

    document.integers["position"] = position;

    return document;
}

bool Chapter::operator<(const Chapter& other) const {
    return position < other.position;
}

bool Chapter::operator==(const Chapter& other) const {
    return name == other.name && position == other.position;
}


/////////////////////////////////////////////////////////////////
//////////////////////////// Section //////////////////////////// 
/////////////////////////////////////////////////////////////////


amDocument Section::GenerateDocumentSimple() {
    amDocument document;
    document.tableName = "sections";
    document.name = name;

    document.integers["book_id"] = bookID;
    document.integers["position"] = pos;
    document.integers["type"] = type;

    document.strings["description"] = description;

    return document;
}

amDocument Section::GenerateDocument() {
    amDocument document = GenerateDocumentSimple();
    
    for (auto& it : chapters)
        document.documents.push_back(it.GenerateDocument());
    
    return document;
}

amDocument Section::GenerateDocumentForID() {
    amDocument document;
    document.name = name;
    document.tableName = "sections";

    document.specialForeign = true;
    document.foreignKey.first = "book_id";
    document.foreignKey.second = bookID;

    document.integers["position"] = pos;

    return document;
}


/////////////////////////////////////////////////////////////////
///////////////////////////// Book //////////////////////////////
/////////////////////////////////////////////////////////////////


bool Book::Init() {
    if (sections.empty()) {
        Section section(amGetManager()->GetDocumentId(GenerateDocumentForID()), 1);
        amGetManager()->InsertDocument(section.GenerateDocument());

        sections.push_back(section);
        return true;
    }

    return false;
}

amDocument Book::GenerateDocumentSimple() {
    amDocument document;
    document.name = title;
    document.tableName = "books";

    document.integers["position"] = pos;

    document.strings["synopsys"] = synopsys;
    document.strings["author"] = author;
    document.strings["genre"] = genre;
    document.strings["description"] = description;

    return document;
}

amDocument Book::GenerateDocument(wxVector<int>& sectionsToGen) {
    amDocument document = GenerateDocumentSimple();

    if (sectionsToGen.empty()) {
        document.documents.reserve(sections.size());
        for (auto& it : sections)
            document.documents.push_back(it.GenerateDocument());

    } else {
        int size = sectionsToGen.size();
        document.documents.reserve(size);

        for (auto& it : sectionsToGen) {
            if (it < size)
                document.documents.push_back(sections[it].GenerateDocument());
            
        }
    }

    return document;
}

amDocument Book::GenerateDocumentForID() {
    amDocument document;
    document.name = title;
    document.tableName = "books";

    return document;
}


/////////////////////////////////////////////////////////////////
//////////////////////////// Project ////////////////////////////
/////////////////////////////////////////////////////////////////


wxVector<Chapter>& amProject::GetChapters(int bookPos, int sectionPos) {
    return books[bookPos - 1].sections[sectionPos - 1].chapters;
}

wxVector<Chapter> amProject::GetChapters(int bookPos) {
    Book& book = books[bookPos - 1];
    int totalSize = 0;

    for (auto& it : book.sections)
        totalSize += it.chapters.size();

    wxVector<Chapter> grouped;
    grouped.reserve(totalSize);

    for (auto& it : book.sections)
        for (auto& it2 : it.chapters)
            grouped.push_back(it2);

    return grouped;
}
