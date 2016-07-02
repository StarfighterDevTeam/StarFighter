#include "Location.h"

extern Game* CurrentGame;

using namespace sf;

//LOCATION
Location::Location(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : StockEntity(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_stock = 0;
	m_stock_max = 100;

	m_unknown_sprite = new GameObject(position, speed, UNKNOWN_PLANET_TEXTURE, size);
}

Location::~Location()
{
	delete m_unknown_sprite;
}

void Location::update(sf::Time deltaTime)
{
	StockEntity::update(deltaTime);
}

void Location::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		if (!m_identified)
		{
			if (m_unknown_sprite)
			{
				screen.draw(*m_unknown_sprite);
			}
		}
		else
		{
			screen.draw(*this);
		}
	}
}

bool Location::CanSupplyFuel()
{
	//see override in class Planet
	return false;
}

size_t Location::GetStockMax()
{
	//see override in class Planet
	return StockEntity::GetStockMax();
}

size_t Location::GetFuelMax()
{
	//see override in class Planet
	return StockEntity::GetFuelMax();
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
	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		m_ore_presence_rates.insert(map<string, float>::value_type(i->first, 0));
	}

	m_building_slots = 10;
	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_starshipConfig.begin(); i != (*CurrentGame).m_starshipConfig.end(); ++i)
	{
		m_starship_productions.insert(map<string, size_t>::value_type(i->first, 0));
	}
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
	return GetStockMax() > 0;
}

size_t Planet::GetStockMax()
{
	size_t stock_max = 0;
	size_t BuildingsVectorSize = m_buildings.size();
	for (size_t i = 0; i < BuildingsVectorSize; i++)
	{
		if (m_buildings[i])
		{
			stock_max += m_buildings[i]->m_stock_max;
		}	
	}

	m_stock_max = stock_max;
	m_stock = MinBetweenSizeTValues(m_stock_max, m_stock);

	return m_stock_max;
}

size_t Planet::GetFuelMax()
{
	size_t fuel_max = 0;
	size_t BuildingsVectorSize = m_buildings.size();
	for (size_t i = 0; i < BuildingsVectorSize; i++)
	{
		if (m_buildings[i])
		{
			fuel_max += m_buildings[i]->m_fuel_max;
		}
	}

	m_fuel_max = fuel_max;
	m_fuel = MinBetweenSizeTValues(m_fuel_max, m_fuel);

	return m_fuel_max;
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

	//Update location's data
	GetStockMax();
	GetFuelMax();
	for (map<string, size_t>::iterator i = new_building->m_starship_productions.begin(); i != new_building->m_starship_productions.end(); ++i)
	{
		m_starship_productions[i->first] += new_building->m_starship_productions[i->first];
	}

	return true;
}

bool Planet::Produce(string name, bool ignore_cost)
{
	//check production capacity
	if (!ignore_cost)
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
	new_starship->m_identified = true;
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
				if (Load(ore->m_display_name, 1))
				{
					m_buildings[i]->m_current_extraction = NULL;
					m_buildings[i]->m_extraction_clock.restart();
				}
			}
		}
	}
}

