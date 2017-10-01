#include "StellarMapVirtual.h"

//extern Game* CurrentGame;

//STELLARZONE
StellarZone::StellarZone()
{
	setSize(sf::Vector2f(STELLAR_ZONE_SIZE, STELLAR_ZONE_SIZE));
	setFillColor(sf::Color(0, 0, 0, 255));
	setOutlineThickness(1);
	setOutlineColor(sf::Color(0, 0, 255, 128));
}

StellarZone::~StellarZone()
{
	
}

//STELLARMAP VIRTUAL
StellarMapVirtual::StellarMapVirtual()
{
	
}

StellarMapVirtual::~StellarMapVirtual()
{
	delete[] m_zones;
}

void StellarMapVirtual::update(sf::Time deltaTime)
{
	
}

sf::Vector2u StellarMapVirtual::GetZoneIndex(sf::Vector2f position)
{
	unsigned int x = (unsigned int)(position.x / STELLAR_ZONE_SIZE);
	unsigned int y = (unsigned int)(position.y / STELLAR_ZONE_SIZE);
	
	return sf::Vector2u(x, y);
}

sf::Vector2f StellarMapVirtual::GetZoneCenter(sf::Vector2u zone_index)
{
	float x = (float)((zone_index.x + 0.5) * STELLAR_ZONE_SIZE);
	float y = (float)((zone_index.y + 0.5) * STELLAR_ZONE_SIZE);
	
	return sf::Vector2f(x, y);
}

string StellarMapVirtual::GetVectorString(sf::Vector2u vector)
{
	ostringstream ss;
	ss << vector.x << "," << vector.y;
	return ss.str();
}

bool StellarMapVirtual::isZoneGenerated(string key)
{
	map<string, bool>::iterator i = m_known_zones.find(key);
	return i != m_known_zones.end();
}

bool StellarMapVirtual::isZoneKnown(string key)
{
	map<string, bool>::iterator i = m_known_zones.find(key);
	return i != m_known_zones.end() && i->second == true;
}

void StellarMapVirtual::SetZoneAsKnown(sf::Vector2u zone_index)
{
	//see override function in StellarMap class
}

void StellarMapVirtual::ExpandKnownStellarMap(sf::Vector2u zone_index)
{
	//generate zones around a designated zone, if they do not exist yet
	for (size_t i = 0; i <= 2; i++)
	{
		for (size_t j = 0; j <= 2; j++)
		{
			string key = GetVectorString(sf::Vector2u(zone_index.x + i - 1, zone_index.y + j - 1));
			if (!isZoneGenerated(key))
			{
				m_known_zones.insert(map<string, bool>::value_type(key, false));

				//Location* location = GenerateRandomZoneContent(sf::Vector2u(zone_index.x + i - 1, zone_index.y + j - 1), false);
				GenerateRandomZoneContent(sf::Vector2u(zone_index.x + i - 1, zone_index.y + j - 1), false);
			}
		}
	}
}

Location* StellarMapVirtual::GenerateRandomZoneContent(sf::Vector2u zone_index, bool visible)
{
	//see override function in StellarMap class
	return NULL;
}