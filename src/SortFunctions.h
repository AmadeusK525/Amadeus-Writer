#ifndef SORTFUNCTIONS_H_
#define SORTFUNCTIONS_H_
#pragma once

#include "BookElements.h"

inline bool amSortCharacters(am::Character* character1, am::Character* character2)
{
	return (*character1 < *character2);
}

inline bool amSortLocations(am::Location* location1, am::Location* location2)
{
	return (*location1 < *location2);
}

inline bool amSortItems(am::Item* item1, am::Item* item2)
{
	return (*item1 < *item2);
}

inline bool amSortDocuments(am::Document* document1, am::Document* document2)
{
	return (*document1 < *document2);
}

inline bool amSortElements(am::StoryElement* element1, am::StoryElement* element2)
{
	am::Character* pCharacter1 = dynamic_cast<am::Character*>(element1);
	am::Character* pCharacter2 = dynamic_cast<am::Character*>(element2);

	if ( pCharacter1 )
	{
		if ( pCharacter2 )
		{
			return *pCharacter1 < *pCharacter2;
		}
		else
			return true;
	}
	else if ( pCharacter2 )
		return false;


	am::Location* pLocation1 = dynamic_cast<am::Location*>(element1);
	am::Location* pLocation2 = dynamic_cast<am::Location*>(element2);

	if ( pLocation1 )
	{
		if ( pLocation2 )
		{
			return *pLocation1 < *pLocation2;
		}
		else 
			return true;
	}
	else if ( pLocation2 )
		return false;


	am::Item* pItem1 = dynamic_cast<am::Item*>(element1);
	am::Item* pItem2 = dynamic_cast<am::Item*>(element2);

	if ( pItem1 )
	{
		if ( pItem2 )
		{
			return *pItem1 < *pItem2;
		}
		else
			return true;
	}
	else if ( pItem2 )
		return false;

	return true;
}

#endif