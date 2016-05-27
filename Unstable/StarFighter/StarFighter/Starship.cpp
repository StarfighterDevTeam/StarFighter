#include "Starship.h"

extern Game* CurrentGame;

using namespace sf;

//STARSHIP
Starship::Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_scout_range = 0;
	m_armor = 1;
	m_fuel_max = 100;
	m_fuel = 100;
	m_speed_max = 100;
	m_stock_max = 0;
	m_stock = 0;
	m_nb_drills = 0;
	m_drill_duration = 1.f;
	m_location= NULL;
}

Starship::~Starship()
{

}

void Starship::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}

//MINER
Miner::Miner(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Starship(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	SetMiningState(Mining_Drilling);

	m_scout_range = 0;
	m_armor = 1;
	m_fuel_max = 100;
	m_fuel = 100;
	m_speed_max = 100;
	m_stock_max = 100;
	m_stock = 0;
	m_nb_drills = 10;
	m_current_drill_attempts = 0;
	m_drill_duration = 3.f;
	m_drill_sucess_rate_bonus = 0.01f;
	m_extraction_duration_bonus = 0.5f;

	m_ore_found = NULL;
}

Miner::~Miner()
{

}

void Miner::update(sf::Time deltaTime)
{
	switch (m_mining_state)
	{
		case Mining_Drilling:
		{
			Drill();
			break;
		}
		case Mining_Extracting:
		{
			Extract(m_ore_found);
			break;
		}
		case Mining_Loading:
		{
			LoadOre(m_ore_found);
			IsNewDrillAttemptAvailable();
			break;
		}
	}

	GameObject::update(deltaTime);
}

void Miner::Drill()
{
	if (m_location && m_location->m_type == LocationType_OreField && m_mining_state == Mining_Drilling)
	{
		if (m_nb_drills > 0 && m_current_drill_attempts < m_nb_drills)
		{
			//Drilling attempt
			if (m_drill_clock.getElapsedTime().asSeconds() > m_drill_duration)
			{
				OreField* field = (OreField*)m_location;
				m_ore_found = GetRandomOre(field);
				m_current_drill_attempts++;
				if (m_ore_found)
				{
					printf("Ore found: %d\n", (int)m_ore_found->m_type);
					SetMiningState(Mining_Extracting);
				}
				else
				{
					printf("No ore found.\n");
					IsNewDrillAttemptAvailable();
				}
			}
		}
	}
}

void Miner::Extract(Ore* ore)
{
	if (ore && m_mining_state == Mining_Extracting)
	{
		//Extract
		if (m_extraction_clock.getElapsedTime().asSeconds() > MaxBetweenValues(sf::Vector2f(ore->m_extraction_duration - m_extraction_duration_bonus, 0)))
		{
			printf("extraction complete. Loading ore.\n");
			SetMiningState(Mining_Loading);
		}
	}
}

void Miner::LoadOre(Ore* ore)
{
	if (!ore)
	{
		return;
	}

	if (m_stock + ore->m_weight > m_stock_max)
	{
		printf("Ore loading unsuccessful: insufficient stock.\n");
		return;
	}

	m_ores_carried.push_back(ore);
	m_stock += ore->m_weight;
	printf("Ore loading successfull.\n\n");
}

Ore* Miner::GetRandomOre(OreField* ore_field)
{
	if (ore_field)
	{
		for (size_t i = 0; i < NBVAL_OreTypes; i++)
		{
			if (ore_field->m_drill_sucess_rates[i] == 0)
			{
				continue;
			}
			if (RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < ore_field->m_drill_sucess_rates[i] + m_drill_sucess_rate_bonus)
			{
				Ore* new_ore = Ore::CreateOre((OreType)i);
				return new_ore;
			}
		}
	}

	return NULL;
}

void Miner::SetMiningState(MiningState state)
{
	m_mining_state = state;

	switch (state)
	{
		case Mining_MovingToLocation:
		{
			break;
		}
		case Mining_Searching:
		{
			break;
		}
		case Mining_Drilling:
		{
			m_drill_clock.restart();
			m_ore_found = NULL;
			printf("Starting new drill attempt.\n");
			break;
		}
		case Mining_Extracting:
		{
			printf("Extraction...");
			m_extraction_clock.restart();
			break;
		}
		case Mining_CarryToBase:
		{
			printf("Carry to base.\n\n");
			m_current_drill_attempts = 0;
			break;
		}
	}
}

bool Miner::IsNewDrillAttemptAvailable()
{
	OreField* field = (OreField*)m_location;
	if (m_current_drill_attempts == m_nb_drills || m_stock >= m_stock_max - field->m_min_ore_weight)
	{
		if (m_current_drill_attempts == m_nb_drills)
		{
			printf("Drill attempts exhausted.\n");
		}
		else if (m_stock >= m_stock_max - field->m_min_ore_weight)
		{
			printf("Stock full.\n");
		}

		SetMiningState(Mining_CarryToBase);
		return false;
	}
	else
	{
		SetMiningState(Mining_Drilling);
		return true;
	}
}