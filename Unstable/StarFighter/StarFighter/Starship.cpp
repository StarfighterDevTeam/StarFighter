#include "Starship.h"

extern Game* CurrentGame;

using namespace sf;

//STARSHIP
Starship::Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	SetStarshipState(StarshipState_Idle);

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
	m_assigned_fuel = 0;
	m_arrived_at_distination = false;
}

Starship::~Starship()
{

}

void Starship::update(sf::Time deltaTime)
{
	switch (m_state)
	{
		case StarshipState_MovingToLocation:
		{
			ManageFuel();
			CheckIfArrivedAtDestination(deltaTime);
			
			break;
		}
		default:
		{
			break;
		}
	}

	GameObject::update(deltaTime);
}

bool Starship::AssignToLocation(Location* location)
{
	if (!location)
	{
		return false;
	}

	if (location == m_location)
	{
		return false;
	}

	size_t distance = GetLightYearsBetweenObjects(this, location);
	if (m_fuel < distance)
	{
		return false;
	}

	//Move to location
	m_speed.x = location->getPosition().x - this->getPosition().x;
	m_speed.y = location->getPosition().y - this->getPosition().y;
	NormalizeSpeed(&m_speed, m_speed_max);

	m_assigned_fuel = distance;
	m_arrived_at_distination = false;
	m_location = location;
	SetStarshipState(StarshipState_MovingToLocation);

	return true;
}

bool Starship::ManageFuel()
{
	if (!m_location || m_arrived_at_distination)
	{
		return false;
	}

	size_t distance_remaining = GetLightYearsBetweenObjects(this, m_location);
	if (distance_remaining < m_assigned_fuel)
	{
		m_assigned_fuel = distance_remaining;
	}

	return true;
}

bool Starship::CheckIfArrivedAtDestination(sf::Time deltaTime)
{
	if (!m_location)
	{
		return false;
	}

	if (GetDistanceBetweenObjects(this, m_location) < this->GetAbsoluteSpeed() * deltaTime.asSeconds())
	{
		setPosition(m_location->getPosition());
		m_speed = sf::Vector2f(0, 0);
		m_arrived_at_distination = true;
		SetStarshipState(StarshipState_Idle);

		return true;
	}

	return false;
}

void Starship::SetStarshipState(StarshipState state)
{
	m_state = state;

	switch (state)
	{
		case StarshipState_MovingToLocation:
		{

			break;
		}
	}
}

//MINER
Miner::Miner(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Starship(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_scout_range = 0;
	m_armor = 1;
	m_fuel_max = 100;
	m_fuel = 100;
	m_speed_max = 50;
	m_stock_max = 100;
	m_stock = 0;
	m_nb_drills = 2;
	m_remaining_drills = 0;
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
	Starship::update(deltaTime);

	switch (m_state)
	{
		case StarshipState_Idle:
		{
			if (m_location)
			{
				switch (m_location->m_type)
				{
					case LocationType_OreField:
					{
						SetStarshipState(StarshipState_Drilling);
						break;
					}
					case LocationType_Planet:
					{
						SetStarshipState(StarshipState_Unloading);
						break;
					}
					default:
					{

						break;
					}
				}
			}
		}
		case StarshipState_Drilling:
		{
			Drill();
			break;
		}
		case StarshipState_Extracting:
		{
			Extract(m_ore_found);
			break;
		}
		case StarshipState_Loading:
		{
			LoadOre(m_ore_found);
			IsNewDrillAttemptAvailable();
			break;
		}
		case StarshipState_CarryToBase:
		{
			GameObject* tmp_obj = (*CurrentGame).GetSceneGameObjectsTyped(LocationObject).front();
			Location* tmp_location = (Location*)tmp_obj;

			AssignToLocation(tmp_location);
			break;
		}
		case StarshipState_Unloading:
		{
			size_t carriage_unloaded = UnloadCarriage();
			printf("%d weight of ore (Planet: %d/%d).\n\n", carriage_unloaded, m_location->m_stock, m_location->m_stock_max);

			GameObject* tmp_obj = (*CurrentGame).GetSceneGameObjectsTyped(LocationObject).back();
			Location* tmp_location = (Location*)tmp_obj;

			AssignToLocation(tmp_location);
			break;
		}
		default:
		{
			break;
		}
	}

	GameObject::update(deltaTime);
}

void Miner::Drill()
{
	if (m_location && m_location->m_type == LocationType_OreField && m_state == StarshipState_Drilling)
	{
		if (m_nb_drills > 0 && m_remaining_drills < m_nb_drills)
		{
			//Drilling attempt
			if (m_drill_clock.getElapsedTime().asSeconds() > m_drill_duration)
			{
				OreField* field = (OreField*)m_location;
				m_ore_found = GetRandomOre(field);
				m_remaining_drills++;
				if (m_ore_found)
				{
					printf("Ore found: %s\n", m_ore_found->m_display_name.c_str());
					SetStarshipState(StarshipState_Extracting);
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
	if (ore && m_state == StarshipState_Extracting)
	{
		//Extract
		if (m_extraction_clock.getElapsedTime().asSeconds() > MaxBetweenValues(sf::Vector2f(ore->m_extraction_duration - m_extraction_duration_bonus, 0)))
		{
			printf("extraction complete. Loading ore.\n");
			SetStarshipState(StarshipState_Loading);
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

void Miner::SetStarshipState(StarshipState state)
{
	Starship::SetStarshipState(state);

	switch (state)
	{
		case StarshipState_Drilling:
		{
			m_drill_clock.restart();
			m_ore_found = NULL;
			printf("Starting drill attempt (%d/%d).\n", m_remaining_drills+1, m_nb_drills);
			break;
		}
		case StarshipState_Searching:
		{
			
			break;
		}
		case StarshipState_Extracting:
		{
			printf("Extraction...");
			m_extraction_clock.restart();
			break;
		}
		case StarshipState_CarryToBase:
		{
			printf("\nCarry to base.\n");
			m_remaining_drills = 0;
			break;
		}
		case StarshipState_Unloading:
		{
			printf("Unloading carriage: ");
			break;
		}
		default:
		{
			break;
		}
	}
}

bool Miner::IsNewDrillAttemptAvailable()
{
	OreField* field = (OreField*)m_location;
	if (m_remaining_drills == m_nb_drills || m_stock >= m_stock_max - field->m_min_ore_weight)
	{
		if (m_remaining_drills == m_nb_drills)
		{
			printf("Drill attempts exhausted.\n");
		}
		else if (m_stock >= m_stock_max - field->m_min_ore_weight)
		{
			printf("Stock full.\n");
		}

		SetStarshipState(StarshipState_CarryToBase);
		return false;
	}
	else
	{
		SetStarshipState(StarshipState_Drilling);
		return true;
	}
}

size_t Miner::UnloadCarriage()
{
	if (!m_location)
	{
		return 0;
	}

	size_t stock_unloaded = 0;

	size_t CarriageVectorSize = m_ores_carried.size();
	for (size_t i = 0; i < CarriageVectorSize; i++)
	{
		if (m_location->m_stock + m_ores_carried[i]->m_weight <= m_stock_max)
		{
			m_location->m_ores_stored.push_back(m_ores_carried[i]);
			m_stock -= m_ores_carried[i]->m_weight;
			m_location->m_stock += m_ores_carried[i]->m_weight;

			stock_unloaded += m_ores_carried[i]->m_weight;
		}

		if (m_location->m_stock == m_stock_max && i != CarriageVectorSize - 1)
		{
			return stock_unloaded;
		}
		
	}

	return stock_unloaded;
}