#ifndef ASTEROIDFIELD_H_INCLUDED
#define ASTEROIDFIELD_H_INCLUDED

#include "Asteroid.h"

class AsteroidField
{
public :
	AsteroidField(int sector_index_bottom, int sector_index_left, int sector_index_size_x, int sector_index_size_y);
	virtual ~AsteroidField();

	static Asteroid* CreateAsteroid(sf::Vector2i sector_index, AsteroidType asteroid_type);
	bool IsOverlappingWithArea(int sector_index_bottom, int sector_index_left, int sector_index_size_x, int sector_index_size_y);

	vector<Asteroid*> m_asteroids;
	int m_sector_index_bottom;
	int m_sector_index_left;
	int m_sector_index_size_x;
	int m_sector_index_size_y;
	sf::Vector2i m_free_space;
};

#endif // ASTEROIDFIELD_H_INCLUDED
