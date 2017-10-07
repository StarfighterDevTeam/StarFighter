#include "StockEntity.h"

extern Game* CurrentGame;

using namespace sf;

StockEntity::StockEntity(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_stock = 0;
	m_stock_max = 0;
	m_min_ore_weight = 0;
	m_fuel = 0;
	m_fuel_max = 0;

	m_hovered = false;
	m_selected = false;

	m_identified = false;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		if (stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) == 0)
		{
			m_ores_stocked.insert(map<string, size_t>::value_type(i->first, 0));
			m_ore_presence_rates.insert(map<string, size_t>::value_type(i->first, 0));
		}
		else
		{
			m_fuel_tanks.insert(map<string, size_t>::value_type(i->first, 0));
		}
	}
}

StockEntity::~StockEntity()
{
	
}

void StockEntity::update(sf::Time deltaTime)
{
	//selection and hovering feedbacks
	if (m_selected)
	{
		setColor(sf::Color(255, 0, 0, 255));
	}
	else if (m_hovered)
	{
		setColor(sf::Color(0, 255, 0, GHOST_ALPHA_VALUE));
	}
	else
	{
		setColor(sf::Color(255, 255, 255, 255));
	}

	GameObject::update(deltaTime);
}

string StockEntity::GetBestPropulsionAvailable()
{
	size_t propulsion = 0;
	string selected_fuel;

	for (map<string, size_t>::iterator i = m_fuel_tanks.begin(); i != m_fuel_tanks.end(); ++i)
	{
		if ((size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) > propulsion && i->second > 0)
		{
			selected_fuel = i->first;
			propulsion = (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]);
		}
	}

	return selected_fuel;
}

string StockEntity::GetBestPropulsionAvailable(map<string, size_t> fuel_tank)
{
	size_t propulsion = 0;
	string selected_fuel;
	for (map<string, size_t>::iterator i = fuel_tank.begin(); i != fuel_tank.end(); ++i)
	{
		if ((size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) > propulsion && i->second > 0)
		{
			selected_fuel = i->first;
			propulsion = (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]);
		}
	}

	return selected_fuel;
}

size_t StockEntity::Load(string ore_name, size_t quantity)
{
	if (ore_name.empty())
	{
		return 0;
	}

	bool is_fuel = stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Propulsion]) > 0;
	if (is_fuel)
	{
		return LoadFuel(ore_name, quantity);
	}
	else
	{
		return LoadOre(ore_name, quantity);
	}
}

size_t StockEntity::LoadOre(string ore_name, size_t quantity)
{
	if (ore_name.empty())
	{
		return 0;
	}

	size_t quantity_accepted = (GetStockMax() - m_stock) / (size_t)stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Weight]);
	size_t quantity_loaded = MinBetweenSizeTValues(quantity_accepted, quantity);
	m_ores_stocked[ore_name] += quantity_loaded;
	m_stock += quantity_loaded*(size_t)stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Weight]);

	printf("Stocking ore: stock (%d/%d), %s stocked: %d/%d requested.\n", m_stock, m_stock_max, ore_name.c_str(), quantity_loaded, quantity);

	return quantity_loaded;
}

size_t StockEntity::LoadFuel(string ore_name, size_t quantity)
{
	if (ore_name.empty())
	{
		return 0;
	}

	size_t quantity_accepted = (GetFuelMax() - m_fuel) / (size_t)stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Weight]);
	size_t quantity_loaded = MinBetweenSizeTValues(quantity_accepted, quantity);

	if (quantity_loaded > 0)
	{
		m_fuel_tanks[ore_name] += quantity_loaded;
		m_fuel += quantity_loaded*(size_t)stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Weight]);

		printf("Fuel loading: stock (%d/%d), %s quantity loaded / %d: %d.\n", m_fuel, m_fuel_max, ore_name.c_str(), quantity_loaded, quantity);
	}
	
	return quantity_loaded;
}

bool StockEntity::CanSupplyFuel()
{
	//see override in class Starship and Location
	return false;
}

size_t StockEntity::GetStockMax()
{
	//see override in class Planet
	return m_fuel_max;
}

size_t StockEntity::GetFuelMax()
{
	//see override in class Planet
	return m_fuel_max;
}

bool StockEntity::CanBeDrilled()
{
	for (map<string, float >::iterator i = m_ore_presence_rates.begin(); i != m_ore_presence_rates.end(); ++i)
	{
		if ((size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) == 0 && i->second > 0)
		{
			return true;
		}
	}

	return false;
}

size_t StockEntity::GetLightestOreWeight()
{
	int min = -1;
	for (map<string, float>::iterator i = m_ore_presence_rates.begin(); i != m_ore_presence_rates.end(); ++i)
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

string StockEntity::GetMostExpansiveOreAvailable()
{
	string ore;
	int max = -1;
	for (map<string, size_t>::iterator i = m_ores_stocked.begin(); i != m_ores_stocked.end(); ++i)
	{
		if (i->second > 0)
		{
			if (stoi((*CurrentGame).m_oreConfig[i->first][OreData_Price]) > max || max < 0)
			{
				max = stoi((*CurrentGame).m_oreConfig[i->first][OreData_Price]);
				ore = i->first;
			}
		}
	}

	return ore;
}

Ore* StockEntity::GetRandomOre(bool ore_with_propulsion, bool ore_without_propulsion)
{
	string ore_found;
	float p = 0;

	for (map<string, float>::iterator i = m_ore_presence_rates.begin(); i != m_ore_presence_rates.end(); ++i)
	{
		if (!ore_with_propulsion && (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) > 0)
		{
			continue;
		}

		if (!ore_without_propulsion && (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) == 0)
		{
			continue;
		}

		if (i->second == 0)
		{
			continue;
		}
		
		if (RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < i->second + p)
		{
				ore_found = i->first;
				break;
		}
		else
		{
			p += i->second;
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

void StockEntity::UnloadCarriage(StockEntity* location)
{
	if (!location)
	{
		return;
	}

	for (map<string, size_t>::iterator i = m_ores_stocked.begin(); i != m_ores_stocked.end(); i++)
	{
		if (i->second > 0)
		{
			size_t quantity_accepted = (location->m_stock_max - location->m_stock) / (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);
			size_t quantity_unloaded = MinBetweenSizeTValues(quantity_accepted, m_ores_stocked[i->first]);
			location->m_ores_stocked[i->first] += quantity_unloaded;
			location->m_stock += quantity_unloaded * (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);
			m_ores_stocked[i->first] -= quantity_unloaded;
			m_stock -= quantity_unloaded * (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);

			printf("\n%s unloaded on planet: quantity %d, total weight of %d (Planet new stock: %d/%d).\n", i->first.c_str(), quantity_unloaded, quantity_unloaded*(size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]), location->m_stock, location->m_stock_max);
		}
	}	
}