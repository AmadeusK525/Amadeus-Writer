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
