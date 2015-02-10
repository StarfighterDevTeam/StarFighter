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

		//ghost is a property that prevents an object from colliding, so by definition we exclude it
		if (independantA->ghost || (independantB->ghost))
			return false;

		if (!independantA->isOnScene || !(independantB->isOnScene))
			return false;

		float x = independantB->getPosition().x;
		float y = independantB->getPosition().y;

		//discus check: on regarde si la distance entre les centres des 2 sprites est plus grande que leurs rayons additionnés
		if ((pow((independantA->getPosition().x - independantB->getPosition().x),2) + pow((independantA->getPosition().y - independantB->getPosition().y),2)) 
			> pow(independantA->diag + independantB->diag,2))
			return false;
			
		//alignement check
		/*if (x - (independantB->m_size.x/2) > independantA->getPosition().x + (independantA->m_size.x/2)
			|| x + (independantB->m_size.x/2) < independantA->getPosition().x - (independantA->m_size.x/2)
			|| y + (independantB->m_size.y/2) < independantA->getPosition().y - (independantA->m_size.y/2)
			|| y - (independantB->m_size.y/2) > independantA->getPosition().y + (independantA->m_size.y/2))
			return false;                        

		*/

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

	static bool IsGrazing(const Independant* independantA, const Independant* independantB) 
	{
		if ((pow((independantA->getPosition().x - independantB->getPosition().x),2) + pow((independantA->getPosition().y - independantB->getPosition().y),2)) 
			< pow(independantA->diag + independantB->diag + GRAZE_DISTANCE,2))
			return true;
		else 
			return false;
	}
};

#endif // SIMPLECOLLISION_H_INCLUDED