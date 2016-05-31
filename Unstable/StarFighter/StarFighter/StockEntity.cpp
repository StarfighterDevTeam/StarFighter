#include "StockEntity.h"

extern Game* CurrentGame;

using namespace sf;

StockEntity::StockEntity(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{

	m_stock_max = 0;
	m_stock = 0;
	m_min_ore_weight = 0;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		m_ores_stocked[i->first] = 0;
		m_ore_presence_rates[i->first] = 0;
	}
}

StockEntity::~StockEntity()
{

}

void StockEntity::update(sf::Time deltaTime)
{
	//see override in class Starship and Location
}

string StockEntity::GetBestPropulsionAvailable()
{
	//see override in class Starship and Location
	return "";
}

bool StockEntity::Stock(Ore* ore)
{
	if (!ore)
	{
		return false;
	}

	if (m_stock + ore->m_weight > m_stock_max)
	{
		printf("Ore loading unsuccessful: insufficient stock.\n");
		return false;
	}

	m_ores_stocked[ore->m_display_name]++;
	m_stock += ore->m_weight;
	printf("Ore loading successfull.\n\n");

	return true;
}

bool StockEntity::CanSupplyFuel()
{
	//see override in class Starship and Location
	return false;
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


Ore* StockEntity::GetRandomOre()
{
	string ore_found;
	float min_drill_rate = 0;

	for (map<string, float>::iterator i = m_ore_presence_rates.begin(); i != m_ore_presence_rates.end(); ++i)
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