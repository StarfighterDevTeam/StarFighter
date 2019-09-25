#include "AsteroidField.h"

extern Game* CurrentGame;

using namespace sf;

AsteroidField::AsteroidField(int sector_index_bottom, int sector_index_left, int sector_index_size_x, int sector_index_size_y)
{	
	m_sector_index_bottom = sector_index_bottom;
	m_sector_index_left = sector_index_left;
	m_sector_index_size_x = sector_index_size_x;
	m_sector_index_size_y = sector_index_size_y;

	//we'll keep at least one empty space at the middle of the asteroid field, to spawn mission objectives later
	m_free_space = sf::Vector2i(sector_index_left + sector_index_size_x / 2, sector_index_bottom + sector_index_size_y / 2);

	//create asteroids
	for (int i = 0; i < sector_index_size_x * sector_index_size_y; i++)
	{
		sf::Vector2i index = sf::Vector2i(sector_index_left + (i % sector_index_size_y), sector_index_bottom + (i / sector_index_size_y));
		if (index != m_free_space && RandomizeFloatBetweenValues(0, 1) < 0.6)
		{
			Asteroid* asteroid = AsteroidField::CreateAsteroid(index, (AsteroidType)RandomizeIntBetweenValues(0, NB_ASTEROID_TYPES - 1));
			asteroid->m_position.x += RandomizeFloatBetweenValues(-STAR_SECTOR_SIZE * 0.25, STAR_SECTOR_SIZE * 0.25);
			asteroid->m_position.y += RandomizeFloatBetweenValues(-STAR_SECTOR_SIZE * 0.25, STAR_SECTOR_SIZE * 0.25);
			m_asteroids.push_back(asteroid);
		}
	}
}

AsteroidField::~AsteroidField()
{
	for (Asteroid* asteroid : m_asteroids)
		asteroid->m_garbageMe = true;

	m_asteroids.clear();
}

Asteroid* AsteroidField::CreateAsteroid(sf::Vector2i sector_index, AsteroidType asteroid_type)
{
	Asteroid* asteroid = new Asteroid(sector_index, asteroid_type);

	if ((*CurrentGame).StoreObjectIfNecessary(asteroid) == false)
		(*CurrentGame).addToScene(asteroid, asteroid->m_layer, asteroid->m_collider, false);

	return asteroid;
}

bool AsteroidField::IsOverlappingWithArea(int sector_index_bottom, int sector_index_left, int sector_index_size_x, int sector_index_size_y)
{
	const int a = m_sector_index_left + m_sector_index_size_x - sector_index_left;
	const int b = m_sector_index_left - (sector_index_left + sector_index_size_x);

	const int c = m_sector_index_bottom + m_sector_index_size_y - sector_index_left;
	const int d = m_sector_index_left - (sector_index_left + sector_index_size_x);


	bool is_inside_area = true;
	if (a < 0 || b > 0 || c < 0 || d > 0)
		is_inside_area = false;

	return is_inside_area;
}