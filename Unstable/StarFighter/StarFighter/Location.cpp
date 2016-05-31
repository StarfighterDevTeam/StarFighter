#include "Location.h"

extern Game* CurrentGame;

using namespace sf;

//LOCATION
Location::Location(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_type = LocationType_None;

	m_stock = 0;
	m_stock_max = 100;
	m_fuel_refill = false;
}

Location::~Location()
{

}

void Location::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}

bool Location::CanSupplyFuel()
{
	return false;
}

string Location::GetBestPropulsionAvailable()
{
	size_t propulsion = 0;
	string selected_fuel;

	for (map<string, size_t >::iterator i = m_ores_stored.begin(); i != m_ores_stored.end(); ++i)
	{
		if ((size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) > propulsion && i->second > 0)
		{
			selected_fuel = i->first;
			propulsion = (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]);
		}
	}

	return selected_fuel;
}

//ORE FIELD
OreField::OreField(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Location(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_type = LocationType_OreField;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i) 
	{
		m_drill_sucess_rates.insert(map<string, float>::value_type(i->first, 0));
	}

	m_min_ore_weight = 0;
	m_drill_slots = 10;
}

OreField::~OreField()
{

}

void OreField::update(sf::Time deltaTime)
{
	Location::update(deltaTime);
}

size_t OreField::GetLightestOreWeight()
{
	int min = -1;
	for (map<string, float>::iterator i = m_drill_sucess_rates.begin(); i != m_drill_sucess_rates.end(); ++i)
	{
		if (i->second > 0)
		{
			if (stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]) < min || min < 0)
			{
				min = stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);
			}
		}
	}

	return min < 0 ? 0 : (size_t)min;
}

//PLANET
Planet::Planet(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Location(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_type = LocationType_Planet;

	m_fuel_refill = true;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		m_drill_sucess_rates.insert(map<string, float>::value_type(i->first, 0));
	}

	m_building_slots = 10;
	//for (size_t i = 0; i < m_building_slots; i++)
	//{
	//	Building* building = NULL;
	//	m_buildings.push_back(building);
	//}
}

Planet::~Planet()
{
	size_t BuildingsVectorSize = m_buildings.size();
	for (size_t i = 0; i < BuildingsVectorSize; i++)
	{
		delete m_buildings[i];
	}
	m_buildings.clear();

}

void Planet::update(sf::Time deltaTime)
{
	Harvest();

	Location::update(deltaTime);
}

bool Planet::CanSupplyFuel()
{
	size_t buildingsVectorSize = m_buildings.size();
	for (size_t i = 0; i < buildingsVectorSize; i++)
	{
		if (m_buildings[i]->m_can_extract_ore)
		{
			return true;
		}
	}

	return false;
}

size_t Planet::GetNbSlotsTaken()
{
	size_t slots_taken = 0;
	size_t BuildingsVectorSize = m_buildings.size();
	for (size_t i = 0; i < BuildingsVectorSize; i++)
	{
		if (m_buildings[i])
		{
			slots_taken += m_buildings[i]->m_slots;
		}
	}

	return slots_taken;
}

bool Planet::Build(string name, bool ignore_cost)
{
	size_t slots_taken = GetNbSlotsTaken();
	if ((size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_Slots]) + slots_taken > m_building_slots)
	{	
		return false;
	}
	
	//check cost
	if (!ignore_cost)
	{
		if (!(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType1].empty() && m_ores_stored[(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType1]] < (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_OreCostQuantity1]))
		{
			return false;
		}
		if (!(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType2].empty() && m_ores_stored[(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType2]] < (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_OreCostQuantity2]))
		{
			return false;
		}
	}

	Building* new_building = Building::CreateBuilding(name);
	m_buildings.push_back(new_building);
	//pay the cost
	if (!ignore_cost)
	{
		if (!(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType1].empty())
		{
			m_ores_stored[(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType1]] -= (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_OreCostQuantity1]);
		}
		if (!(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType2].empty())
		{
			m_ores_stored[(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType2]] -= (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_OreCostQuantity2]);
		}
	}

	printf("\nBuilding %s successfully (Planet: %d/10 slots).\n\n", name.c_str(), slots_taken + new_building->m_slots, m_building_slots);

	return true;
}

Ore* Planet::GetRandomOre()
{
	string ore_found;
	float min_drill_rate = 0;

	for (map<string, float>::iterator i = m_drill_sucess_rates.begin(); i != m_drill_sucess_rates.end(); ++i)
	{
		
		if (i->second == 0)
		{
			continue;
		}
		if (RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < i->second)
		{
			if (ore_found.empty() || i->second < min_drill_rate)
			{
				ore_found = i->first;
				min_drill_rate = i->second;
			}
		}
	}

	if (ore_found.empty())
	{
		return NULL;
	}
	else
	{
		Ore* new_ore = Ore::CreateOre(ore_found);
		return new_ore;
	}
}

void Planet::Harvest()
{
	size_t buildingsVectorSize = m_buildings.size();
	for (size_t i = 0; i < buildingsVectorSize; i++)
	{
		if (m_buildings[i] && m_buildings[i]->m_can_extract_ore)
		{
			Ore* ore = GetRandomOre();
			if (m_buildings[i]->Extract(ore))
			{
				if (Stock(ore))
				{
					m_buildings[i]->m_current_extraction = NULL;
					m_buildings[i]->m_extraction_clock.restart();
				}
			}
		}
	}
}

bool Planet::Stock(Ore* ore)
{
	if (!ore)
	{
		return false;
	}

	if (ore->m_weight + m_stock > m_stock_max)
	{
		return false;
	}

	m_stock += ore->m_weight;
	m_ores_stored[ore->m_display_name] ++;

	printf("Extracted: %s, stored successfully (Planet stock: %d/%d).\n", ore->m_display_name.c_str(), m_stock, m_stock_max);

	return true;
}

