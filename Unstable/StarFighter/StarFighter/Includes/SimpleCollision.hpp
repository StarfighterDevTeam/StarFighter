#ifndef SIMPLECOLLISION_H_INCLUDED
#define SIMPLECOLLISION_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "../Independant.h"


class SimpleCollision 
{
public:
	static inline sf::IntRect FToIRect(const sf::FloatRect& f) 
	{
		return sf::IntRect((int)f.left, (int)f.top, (int)f.width, (int)f.height);
	}

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

		// Second test : are the corners included in the other sprite, or vice versa ?
		else
		{
			sf::IntRect boundsA(FToIRect(independantA->getGlobalBounds()));
			sf::IntRect boundsB(FToIRect(independantB->getGlobalBounds()));
				return boundsA.intersects(boundsB);
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