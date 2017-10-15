#include "StockEntity.h"

extern Game* CurrentGame;

using namespace sf;

StockEntity::StockEntity(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_current_ore_stock = 0;
	m_ore_stock_max = 0;
	m_current_fuel_stock = 0;
	m_fuel_stock_max = 0;

	m_hovered = false;
	m_selected = false;

	m_identified = false;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		if (stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) == 0)
		{
			m_ore_stock.insert(map<string, size_t>::value_type(i->first, 0));
			m_ore_presence_rates.insert(map<string, size_t>::value_type(i->first, 0));
		}
		else
		{
			m_fuel_stock.insert(map<string, size_t>::value_type(i->first, 0));
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

size_t StockEntity::LoadInStock(string resource_name, size_t quantity)
{
	bool is_fuel = (*CurrentGame).IsFuel(resource_name);

	size_t ore_weight = (size_t)stoi((*CurrentGame).m_oreConfig[resource_name][OreData_Weight]);

	if (is_fuel)
	{
		//load no more than the max
		GetFuelMax();//update max value
		size_t quantity_accepted = quantity * ore_weight > m_fuel_stock_max - m_current_fuel_stock ? (m_fuel_stock_max - m_current_fuel_stock) / ore_weight : quantity;
		m_fuel_stock[resource_name] += quantity_accepted;
		m_current_fuel_stock += quantity_accepted * ore_weight;

		return quantity_accepted;
	}
	else
	{
		//load no more than the max
		GetOreMax();//update max value
		size_t quantity_accepted = quantity * ore_weight > (m_ore_stock_max - m_current_ore_stock) ? (m_ore_stock_max - m_current_ore_stock) / ore_weight : quantity;
		m_ore_stock[resource_name] += quantity_accepted;
		m_current_ore_stock += quantity_accepted * ore_weight;

		return quantity_accepted;
	}
}

bool StockEntity::CanSupplyFuel()
{
	//see override in class Starship and Location
	return false;
}

size_t StockEntity::GetOreMax()
{
	//see override in class Planet
	return m_ore_stock_max;
}

size_t StockEntity::GetFuelMax()
{
	//see override in class Planet
	return m_fuel_stock_max;
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

Ore* StockEntity::DigRandomOre(bool can_be_fuel, bool can_be_ore)
{
	string ore_found;
	float p = 0;

	for (map<string, float>::iterator i = m_ore_presence_rates.begin(); i != m_ore_presence_rates.end(); ++i)
	{
		if (!can_be_fuel && (*CurrentGame).IsFuel(i->first))
		{
			continue;
		}

		if (!can_be_ore && !(*CurrentGame).IsFuel(i->first) == 0)
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

void StockEntity::UnloadCarriage(StockEntity* entity)
{
	for (map<string, size_t>::iterator i = m_ore_stock.begin(); i != m_ore_stock.end(); i++)
	{
		size_t quantity_unloaded = entity->LoadInStock(i->first, i->second);

		size_t ore_weight = (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);

		m_ore_stock[i->first] -= quantity_unloaded;
		m_current_ore_stock -= quantity_unloaded * ore_weight;
	}	

	for (map<string, size_t>::iterator i = m_fuel_stock.begin(); i != m_fuel_stock.end(); i++)
	{
		size_t quantity_unloaded = entity->LoadInStock(i->first, i->second);

		m_fuel_stock[i->first] -= quantity_unloaded;
		m_current_fuel_stock -= quantity_unloaded;
	}
}

size_t StockEntity::SupplyOilTank(pair<string, size_t>& starship_fuel_tank, StockEntity& starship)
{
	for (map<string, size_t>::iterator i = m_fuel_stock.begin(); i != m_fuel_stock.end(); ++i)
	{
		if (i->second > 0 && (starship_fuel_tank.first == i->first || starship_fuel_tank.second == 0))
		{
			size_t quantity_accepted = starship.LoadFuelTank(i->first, i->second);
			m_fuel_stock[i->first] -= quantity_accepted;
			m_current_fuel_stock -= quantity_accepted;

			return quantity_accepted;
		}
	}

	return 0;
}

size_t StockEntity::LoadFuelTank(string fuel_name, size_t quantity)
{
	//see override function in Starship class
	return 0;
}