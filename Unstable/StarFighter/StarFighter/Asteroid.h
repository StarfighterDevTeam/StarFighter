#ifndef ASTEROID_H_INCLUDED
#define ASTEROID_H_INCLUDED

#include "Destructible.h"

enum AsteroidType
{
	Asteroid_XXS,
	Asteroid_XS,
	Asteroid_S,
	Asteroid_M,
	Asteroid_L,
	Asteroid_XL,
	Asteroid_XXL,
	NB_ASTEROID_TYPES,
};

class Asteroid : public Destructible
{
public :
	Asteroid(sf::Vector2i sector_index, AsteroidType asteroid_type);
	virtual ~Asteroid();
	void Update(sf::Time deltaTime) override;
	void GetHitByObject(GameObject* object) override;
	void Death() override;

	AsteroidType m_asteroid_type;
	float m_rotation_speed;
	float m_chance_of_loot;
};

#endif // ASTEROID_H_INCLUDED
