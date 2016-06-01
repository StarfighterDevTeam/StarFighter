#include "Location.h"

extern Game* CurrentGame;

using namespace sf;

//LOCATION
Location::Location(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : StockEntity(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
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

//ORE FIELD
OreField::OreField(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Location(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_drill_slots = 10;
}

OreField::~OreField()
{

}

void OreField::update(sf::Time deltaTime)
{
	Location::update(deltaTime);
}

//PLANET
Planet::Planet(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Location(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_fuel_refill = true;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		m_ore_presence_rates.insert(map<string, float>::value_type(i->first, 0));
	}

	m_building_slots = 10;
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

string Location::GetBestPropulsionAvailable()
{
	size_t propulsion = 0;
	string selected_fuel;

	for (map<string, size_t >::iterator i = m_ores_stocked.begin(); i != m_ores_stocked.end(); ++i)
	{
		if ((size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) > propulsion && i->second > 0)
		{
			selected_fuel = i->first;
			propulsion = (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]);
		}
	}

	return selected_fuel;
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
		if (!(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType1].empty() && m_ores_stocked[(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType1]] < (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_OreCostQuantity1]))
		{
			return false;
		}
		if (!(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType2].empty() && m_ores_stocked[(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType2]] < (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_OreCostQuantity2]))
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
			m_ores_stocked[(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType1]] -= (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_OreCostQuantity1]);
		}
		if (!(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType2].empty())
		{
			m_ores_stocked[(*CurrentGame).m_buildingConfig[name][BuildingData_OreCostType2]] -= (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_OreCostQuantity2]);
		}
	}

	printf("\nBuilding %s successfully (Planet: %d/10 slots).\n\n", name.c_str(), slots_taken + new_building->m_slots, m_building_slots);

	return true;
}

bool Planet::Produce(string name, bool ignore_cost)
{
	size_t nb_productions = 0;
	size_t buildingsVectorSize = m_buildings.size();
	for (size_t i = 0; i < buildingsVectorSize; i++)
	{
		if (m_buildings[i])
		{
			nb_productions += m_buildings[i]->m_starship_productions[name];
		}
	}

	if (nb_productions == 0)
	{
		return false;
	}

	//check cost
	if (!ignore_cost)
	{
		if (!(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType1].empty() && m_ores_stocked[(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType1]] < (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_OreCostQuantity1]))
		{
			return false;
		}
		if (!(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType2].empty() && m_ores_stocked[(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType2]] < (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_OreCostQuantity2]))
		{
			return false;
		}
	}

	Starship* new_starship = Starship::CreateStarship(name);
	new_starship->setPosition(getPosition());
	new_starship->m_target_location = this;
	new_starship->m_base_location = this;
	(*CurrentGame).addToScene(new_starship, StarshipLayer, StarshipObject);

	//pay the cost
	if (!ignore_cost)
	{
		if (!(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType1].empty())
		{
			m_ores_stocked[(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType1]] -= (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_OreCostQuantity1]);
		}
		if (!(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType2].empty())
		{
			m_ores_stocked[(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType2]] -= (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_OreCostQuantity2]);
		}
	}

	return true;
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
				if (Stock(ore->m_display_name, 1))
				{
					m_buildings[i]->m_current_extraction = NULL;
					m_buildings[i]->m_extraction_clock.restart();
				}
			}
		}
	}
}

