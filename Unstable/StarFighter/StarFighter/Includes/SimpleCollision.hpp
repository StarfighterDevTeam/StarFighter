#ifndef SIMPLECOLLISION_H_INCLUDED
#define SIMPLECOLLISION_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "../Independant.h"

class SimpleCollision 
{
public:
	static bool AreColliding(const Independant* independantA, const Independant* independantB) {
		// If not visibe, let's not even bother with the collision
		if (!independantA->visible || !(independantB->visible))
			return false;

		if (!independantA->isOnScene || !(independantB->isOnScene))
			return false;

		// Preliminary test : are the two sprites even aligned ?
		float x = independantB->getPosition().x;
		float y = independantB->getPosition().y;

		if (x - (independantB->m_size.x/2) > independantA->getPosition().x + (independantA->m_size.x/2)
			|| x + (independantB->m_size.x/2) < independantA->getPosition().x - (independantA->m_size.x/2)
			|| y + (independantB->m_size.y/2) < independantA->getPosition().y - (independantA->m_size.y/2)
			|| y - (independantB->m_size.y/2) > independantA->getPosition().y + (independantA->m_size.y/2))
			return false;                        

		// Second test : are the corners included in the other sprite, or vice versa ?
		else
		{
			if ( (   ( (independantA->getPosition().x - (independantA->m_size.x/2) < x - (independantB->m_size.x/2)) && (x - (independantB->m_size.x/2) < independantA->getPosition().x + (independantA->m_size.x/2))   // up-left corner
				&& (independantA->getPosition().y - (independantA->m_size.y/2) < y - (independantB->m_size.y/2)) && (y - (independantB->m_size.y/2) < independantA->getPosition().y + (independantA->m_size.y/2)) )

				||  ( (independantA->getPosition().x - (independantA->m_size.x/2) < x + (independantB->m_size.x/2)) && (x + (independantB->m_size.x/2) < independantA->getPosition().x + (independantA->m_size.x/2))   // up-right corner
				&& ( independantA->getPosition().y - (independantA->m_size.y/2) < y - (independantB->m_size.y/2)) && (y - (independantB->m_size.y/2) < independantA->getPosition().y + (independantA->m_size.y/2)) )

				||  ( (independantA->getPosition().x - (independantA->m_size.x/2) < x - (independantB->m_size.x/2)) && (x - (independantB->m_size.x/2) < independantA->getPosition().x + (independantA->m_size.x/2))   // down-left corner
				&& (independantA->getPosition().y - (independantA->m_size.y/2) < y + (independantB->m_size.y/2)) && (y + (independantB->m_size.y/2) < independantA->getPosition().y + (independantA->m_size.y/2)) )

				||  ( (independantA->getPosition().x - (independantA->m_size.x/2) < x + (independantB->m_size.x/2)) && (x + (independantB->m_size.x/2) < independantA->getPosition().x + (independantA->m_size.x/2))   // down-right corner
				&& (independantA->getPosition().y - (independantA->m_size.y/2) < y + (independantB->m_size.y/2)) && (y + (independantB->m_size.y/2) < independantA->getPosition().y + (independantA->m_size.y/2)))  )

				||

				(  ((x - (independantB->m_size.x/2) < independantA->getPosition().x - (independantA->m_size.x/2)) && (independantA->getPosition().x - (independantA->m_size.x/2) < x + (independantB->m_size.x/2))   // up-left corner
				&& (y - (independantB->m_size.y/2) < independantA->getPosition().y - (independantA->m_size.y/2)) && (independantA->getPosition().y - (independantA->m_size.y/2) < y + (independantB->m_size.y/2)) )
				//return true;
				|| ( (x - (independantB->m_size.x/2) < independantA->getPosition().x + (independantA->m_size.x/2)) && (independantA->getPosition().x + (independantA->m_size.x/2) < x + (independantB->m_size.x/2))  // up-right corner
				&& (y - (independantB->m_size.y/2) < independantA->getPosition().y - (independantA->m_size.y/2)) && (independantA->getPosition().y - (independantA->m_size.y/2) < y + (independantB->m_size.y/2)) )
				//return true;
				|| ( (x - (independantB->m_size.x/2) < independantA->getPosition().x - (independantA->m_size.x/2)) && (independantA->getPosition().x - (independantA->m_size.x/2) < x + (independantB->m_size.x/2))   // down-left corner
				&& (y - (independantB->m_size.y/2) < independantA->getPosition().y + (independantA->m_size.y/2)) && (independantA->getPosition().y + (independantA->m_size.y/2) < y + (independantB->m_size.y/2)) )
				//return true;
				|| ( (x - (independantB->m_size.x/2) < independantA->getPosition().x + (independantA->m_size.x/2)) && (independantA->getPosition().x + (independantA->m_size.x/2) < x + (independantB->m_size.x/2))   // down-right corner
				&& (y - (independantB->m_size.y/2) < independantA->getPosition().y + (independantA->m_size.y/2)) && (independantA->getPosition().y + (independantA->m_size.y/2) < y + (independantB->m_size.y/2)))  )  )

				return true;
			else
				return false;
		}
		return false;
	}
};

#endif // SIMPLECOLLISION_H_INCLUDED