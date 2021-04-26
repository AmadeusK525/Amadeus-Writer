#ifndef SORTFUNCTIONS_H_
#define SORTFUNCTIONS_H_
#pragma once

#include "BookElements.h"

inline bool amSortCharacters(Character* character1, Character* character2)
{
	return (*character1 < *character2);
}

inline bool amSortLocations(Location* location1, Location* location2)
{
	return (*location1 < *location2);
}

inline bool amSortItems(Item* item1, Item* item2)
{
	return (*item1 < *item2);
}

inline bool amSortDocuments(Document* document1, Document* document2)
{
	return (*document1 < *document2);
}

#endif