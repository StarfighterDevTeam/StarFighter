#ifndef SIMPLECOLLISION_H_INCLUDED
#define SIMPLECOLLISION_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "CollisionUtils.h"

//TODO: move this out of /includes ?

class SimpleCollision
{
public:
	static inline sf::IntRect FToIRect(const sf::FloatRect& f)
	{
		return sf::IntRect((int)f.left, (int)f.top, (int)f.width, (int)f.height);
	}

	static bool AreColliding(const GameObject* GameObjectA, const GameObject* GameObjectB) {
		// If not visibe, let's not even bother with the collision
		if (!GameObjectA->visible || !(GameObjectB->visible))
			return false;

		//ghost is a property that prevents an object from colliding, so by definition we exclude it
		if (GameObjectA->ghost || (GameObjectB->ghost))
			return false;

		if (!GameObjectA->isOnScene || !(GameObjectB->isOnScene))
			return false;

		//discus check: on regarde si la distance entre les centres des 2 sprites est plus grande que leurs rayons additionnés
		const float a = GameObjectA->getPosition().x - GameObjectB->getPosition().x;
		const float b = GameObjectA->getPosition().y - GameObjectB->getPosition().y;
		const float c = GameObjectA->diag + GameObjectB->diag;

		if (((a * a) + (b * b)) > (c * c))
			return false;

		else
		{
			if (PIXEL_PERFECT_COLLISION)
			{
				return Collision::PixelPerfectTest(GameObjectA, GameObjectB, MinAlphaLimitForCollision);
			}
			else
			{
				// No pixel perfect : are the corners included in the other sprite, or vice versa ?
				sf::IntRect boundsA(FToIRect(GameObjectA->getGlobalBounds()));
				sf::IntRect boundsB(FToIRect(GameObjectB->getGlobalBounds()));
				return boundsA.intersects(boundsB);
			}
		}
		return false;
	}

private:
	static const sf::Uint8 MinAlphaLimitForCollision = 127;
};

#endif // SIMPLECOLLISION_H_INCLUDED