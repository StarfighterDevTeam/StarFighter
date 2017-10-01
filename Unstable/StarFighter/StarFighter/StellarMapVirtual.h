#ifndef STELLARMAPVIRTUAL_H_INCLUDED
#define STELLARMAPVIRTUAL_H_INCLUDED

//#include "Game.h"
#include "Globals.h"

using namespace std;
using namespace sf;

class Location;

class StellarZone : public sf::RectangleShape
{
public:
	StellarZone();
	virtual ~StellarZone();

	sf::Text m_zone_name;
};

//bool operator < (sf::Vector2u& vector1, sf::Vector2u& vector2) { return vector1.x < vector2.x || vector1.y < vector2.y; }

class StellarMapVirtual
{
public :
	StellarMapVirtual();
	virtual ~StellarMapVirtual();
	virtual void update(sf::Time deltaTime);
	
	sf::Vector2u GetZoneIndex(sf::Vector2f position);
	sf::Vector2f GetZoneCenter(sf::Vector2u zone_index);
	bool isZoneGenerated(string key);
	bool isZoneKnown(string key);
	void ExpandKnownStellarMap(sf::Vector2u zone_index);
	static string GetVectorString(sf::Vector2u vector);
	virtual void SetZoneAsKnown(sf::Vector2u zone_index);

	virtual Location* GenerateRandomZoneContent(sf::Vector2u zone_index, bool visible);
	
	StellarZone* m_zones[STELLAR_ZONES_PER_LINE][STELLAR_ZONES_PER_LINE];
	map<string, bool> m_known_zones;
};

#endif // STELLARMAPVIRTUAL_H_INCLUDED
