#include "BookElements.h"

wxVector<Chapter> amProject::GetChapters(int bookPos) {
    Book& book = books[bookPos];
    int totalSize = 0;

    for (auto& it : book.sections)
        totalSize += it.chapters.size();

    wxVector<Chapter> grouped(totalSize);

    for (auto& it : book.sections)
        for (auto& it2 : it.chapters)
            grouped.push_back(it2);

    return grouped;
}

amDocument Book::GenerateDocument() {
    amDocument document;
    document.name = title;

    document.strings["synopsys"] = synopsys;
    document.strings["author"] = author;
    document.strings["genre"] = genre;
    document.strings["description"] = description;

    for (auto& it : sections)
        document.documents.push_back(it.GenerateDocument());

    return document;
}

amDocument Section::GenerateDocument() {
    return amDocument();
}
