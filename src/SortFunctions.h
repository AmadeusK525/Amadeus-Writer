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

inline bool amSortElements(Element* element1, Element* element2)
{
	Character* pCharacter1 = dynamic_cast<Character*>(element1);
	Character* pCharacter2 = dynamic_cast<Character*>(element2);

	if ( pCharacter1 )
	{
		if ( pCharacter2 )
		{
			return pCharacter1 < pCharacter2;
		}
		else
			return true;
	}
	else if ( pCharacter2 )
		return false;


	Location* pLocation1 = dynamic_cast<Location*>(element1);
	Location* pLocation2 = dynamic_cast<Location*>(element2);

	if ( pLocation1 )
	{
		if ( pLocation2 )
		{
			return pLocation1 < pLocation2;
		}
		else 
			return true;
	}
	else if ( pLocation2 )
		return false;


	Item* pItem1 = dynamic_cast<Item*>(element1);
	Item* pItem2 = dynamic_cast<Item*>(element2);

	if ( pItem1 )
	{
		if ( pItem2 )
		{
			return pItem1 < pItem2;
		}
		else
			return true;
	}
	else if ( pItem2 )
		return false;
}

#endif