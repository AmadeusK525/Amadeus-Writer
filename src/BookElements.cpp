#include "BookElements.h"

wxVector<Character> amProject::GetCharacters(int bookPos) {
    wxVector<Character> grouped(gCharacters.size() + books[bookPos].characters.size());

    std::set_union(gCharacters.begin(), gCharacters.end(), books[bookPos].characters.begin(),
        books[bookPos].characters.end(), grouped.begin());

    return grouped;
}

wxVector<Location> amProject::GetLocations(int bookPos) {
    wxVector<Location> grouped(gLocations.size() + books[bookPos].locations.size());

    std::set_union(gLocations.begin(), gLocations.end(), books[bookPos].locations.begin(),
        books[bookPos].locations.end(), grouped.begin());

    return grouped;
}

wxVector<Item> amProject::GetItems(int bookPos) {
    wxVector<Item> grouped(gItems.size() + books[bookPos].items.size());

    std::set_union(gItems.begin(), gItems.end(), books[bookPos].items.begin(),
        books[bookPos].items.end(), grouped.begin());

    return grouped;
}

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
