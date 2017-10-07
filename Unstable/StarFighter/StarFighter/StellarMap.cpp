#include "StellarMap.h"

extern Game* CurrentGame;

using namespace sf;

//STELLARMAP
StellarMap::StellarMap()
{
	for (size_t i = 0; i < STELLAR_ZONES_PER_LINE; i++)
	{
		for (size_t j = 0; j < STELLAR_ZONES_PER_LINE; j++)
		{
			StellarZone* zone = new StellarZone();
			m_zones[i][j] = zone;
			zone->setPosition(sf::Vector2f((float)(i * STELLAR_ZONE_SIZE), (float)(j * STELLAR_ZONE_SIZE)));
			(*CurrentGame).addToFeedbacks(zone);

			zone->m_zone_name.setFont(*(*CurrentGame).m_font[Font_Arial]);
			zone->m_zone_name.setCharacterSize(14);
			zone->m_zone_name.setColor(sf::Color(0, 0, 255, 128));
			zone->m_zone_name.setString(StellarMapVirtual::GetVectorString(sf::Vector2u((unsigned int)i, (unsigned int)j)));
			zone->m_zone_name.setPosition(sf::Vector2f(zone->getPosition().x + STELLAR_ZONE_SIZE / 2, zone->getPosition().y + STELLAR_ZONE_SIZE / 2));
			(*CurrentGame).addToFeedbacks(&zone->m_zone_name);
		}
	}
}

StellarMap::~StellarMap()
{
	
}

void StellarMap::update(sf::Time deltaTime)
{
	
}

void StellarMap::SetZoneAsKnown(sf::Vector2u zone_index)
{
	string key = GetVectorString(zone_index);
	m_known_zones[key] = true;

	m_zones[zone_index.x][zone_index.y]->m_zone_name.setColor(sf::Color(0, 255, 0, 128));
	m_zones[zone_index.x][zone_index.y]->setFillColor(sf::Color(255, 255, 255, 28));
}

Location* StellarMap::GenerateRandomZoneContent(sf::Vector2u zone_index, bool visible)
{
	if (RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < CHANCE_OF_LOCATION_IN_ZONE)
	{
		sf::Vector2f max_x_coordinates = sf::Vector2f(GetZoneCenter(zone_index).x - STELLAR_ZONE_SIZE * 0.5, GetZoneCenter(zone_index).x + STELLAR_ZONE_SIZE * 0.5);
		sf::Vector2f max_y_coordinates = sf::Vector2f(GetZoneCenter(zone_index).y - STELLAR_ZONE_SIZE * 0.5, GetZoneCenter(zone_index).y + STELLAR_ZONE_SIZE * 0.5);

		sf::Vector2f random_pos;
		
		bool position_is_valid = false;
		size_t tries = 0;//for safety
		//choose random positions until found one that is not too close from an existing location
		while (!position_is_valid)
		{
			position_is_valid = true;
			tries++;

			random_pos = sf::Vector2f(RandomizeFloatBetweenValues(sf::Vector2f(max_x_coordinates.x, max_x_coordinates.y)), RandomizeFloatBetweenValues(sf::Vector2f(max_y_coordinates.x, max_y_coordinates.y)));
			
			std::vector<GameObject*> locations = (*CurrentGame).GetSceneGameObjectsTyped(LocationObject);
			size_t locationsVectorSize = locations.size();
			for (size_t i = 0; i < locationsVectorSize; i++)
			{
				if (locations[i])
				{
					float distance = GameObject::GetDistanceBetweenPositions(locations[i]->getPosition(), random_pos);
					if (distance < MIN_DISTANCE_BETWEEN_LOCATIONS)
					{
						position_is_valid = false;
						break;
					}
				}
			}

			if (tries > 10)
			{
				return NULL;
			}
		}

		//TODO : create location
		printf("\n\n-------------->NEW LOCATION CREATED.\n\n");
		Planet* planet = new Planet(random_pos, sf::Vector2f(0, 0), "2D/Planet1.png", sf::Vector2f(150, 150), sf::Vector2f(75, 75), 1);
		(*CurrentGame).addToScene(planet, LocationLayer, LocationObject);
		planet->m_ore_presence_rates["oil"] = 1.0f;
		planet->m_ore_presence_rates["deuterium"] = 0.1f;
		//planet->Build("refinery", true);
		//planet->Build("refinery", true);
		planet->Build("factory", true);
		planet->Load("oil", 50);
		planet->Load("deuterium", 30);
		planet->m_display_name = "Planète mère";
		planet->m_visible = visible;

		return planet;
	}
	else
	{
		return NULL;
	}

	return NULL;
}