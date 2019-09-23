#ifndef ASTEROID_H_INCLUDED
#define ASTEROID_H_INCLUDED

#include "DestructibleObject.h"

class Asteroid : public DestructibleObject
{
public :
	Asteroid(sf::Vector2i sector_index);
	virtual ~Asteroid();
	void Update(sf::Time deltaTime) override;

	float m_rotation_speed;
};

#endif // ASTEROID_H_INCLUDED
