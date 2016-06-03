#include "StellarMap.h"

extern Game* CurrentGame;

using namespace sf;

//STELLARZONE
StellarZone::StellarZone()
{
	setSize(sf::Vector2f(STELLAR_ZONE_SIZE, STELLAR_ZONE_SIZE));
	setFillColor(sf::Color(0, 0, 0, 0));
	setOutlineThickness(1);
	setOutlineColor(sf::Color(0, 0, 255, 128));

	m_known = false;
}

StellarZone::~StellarZone()
{

}

//STELLARMAP
StellarMap::StellarMap()
{
	for (size_t i = 0; i < STELLAR_ZONES_PER_LINE; i++)
	{
		for (size_t j = 0; j < STELLAR_ZONES_PER_LINE; j++)
		{
			StellarZone* zone = new StellarZone();
			m_zones[i][j] = zone;
			zone->setPosition(sf::Vector2f(i * STELLAR_ZONE_SIZE, j * STELLAR_ZONE_SIZE));
			(*CurrentGame).addToFeedbacks(zone);
		}
	}
}

StellarMap::~StellarMap()
{
	delete[] m_zones;
}

void StellarMap::update(sf::Time deltaTime)
{
	
}