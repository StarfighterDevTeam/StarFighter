#ifndef SIMPLECOLLISION_H_INCLUDED
#define SIMPLECOLLISION_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "../CollisionUtils.h"

//TODO: move this out of /includes ?

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

		//discus check: on regarde si la distance entre les centres des 2 sprites est plus grande que leurs rayons additionnés
		const float a = independantA->getPosition().x - independantB->getPosition().x;
		const float b = independantA->getPosition().y - independantB->getPosition().y;
		const float c = independantA->diag + independantB->diag;

		if (((a * a) + (b * b)) > (c * c))
			return false;


		else
		{
			if (PIXEL_PERFECT_COLLISION)
			{
				return Collision::PixelPerfectTest(independantA, independantB, MinAlphaLimitForCollision);
			}
			else
			{
				// No pixel perfect : are the corners included in the other sprite, or vice versa ?
				sf::IntRect boundsA(FToIRect(independantA->getGlobalBounds()));
				sf::IntRect boundsB(FToIRect(independantB->getGlobalBounds()));
				return boundsA.intersects(boundsB);
			}
		}
		return false;
	}

	static bool IsGrazing(const Independant* independantA, const Independant* independantB)
	{
		// If not visibe, let's not even bother with the collision
		if (!independantA->visible || !(independantB->visible))
			return false;

		//ghost is a property that prevents an object from colliding, so by definition we exclude it
		if (independantA->ghost || (independantB->ghost))
			return false;

		if (!independantA->isOnScene || !(independantB->isOnScene))
			return false;

		const float a = independantA->getPosition().x - independantB->getPosition().x;
		const float b = independantA->getPosition().y - independantB->getPosition().y;
		const float c = independantA->diag + independantB->diag + GRAZE_DISTANCE;

		if (((a * a) + (b * b)) < (c * c))
			return true;
		else
			return false;
	}

private:
	static const sf::Uint8 MinAlphaLimitForCollision = 127;
};

#endif // SIMPLECOLLISION_H_INCLUDED