#include "Asteroid.h"

extern Game* CurrentGame;

using namespace sf;

Asteroid::Asteroid(sf::Vector2i sector_index) : DestructibleObject()
{
	m_collision_damage = 10;
	m_health_max = 10;
	m_health = m_health_max;
	m_hostility = Hostility_Enemy;

	Init(m_position, m_speed, "2D/asteroid.png", sf::Vector2f(200, 224), 1, 1);

	(*CurrentGame).SetStarSectorIndex(this, sector_index);

	m_rotation_speed = RandomizeFloatBetweenValues(3, 15) * RandomizeSign();
	m_heading = RandomizeFloatBetweenValues(0, 359.9);
}

Asteroid::~Asteroid()
{
	
}

void Asteroid::Update(sf::Time deltaTime)
{
	m_heading += m_rotation_speed * deltaTime.asSeconds();

	DestructibleObject::Update(deltaTime);
}