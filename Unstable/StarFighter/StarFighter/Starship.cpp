#include "Starship.h"

extern Game* CurrentGame;

using namespace sf;

//STARSHIP
Starship::Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : StockEntity(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	SetStarshipState(StarshipState_Idle);

	m_scout_range = 0;
	m_armor_max = 1;
	m_armor = m_armor_max;
	m_fuel_max = 100;
	m_speed_max = 100;
	m_nb_drills = 0;
	m_target_location = NULL;
	m_base_location = NULL;
	m_task_location = NULL;
	m_propulsion_assigned = 0;
	m_arrived_at_distination = true;
	m_propulsion = 0;
	m_propulsion_speed_bonus = 0;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		if ((size_t)stoi(i->second[OreData_Propulsion]) > 0)
		{
			m_fuel_tanks.insert(map<string, size_t>::value_type(i->first, 0));
			m_fuel_assigned.insert(map<string, size_t>::value_type(i->first, 0));
		}
	}

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); i++)
	{
		m_cost.insert(map<string, size_t>::value_type(i->first, 0));
	}

	//mining
	m_current_drill_attempts = 0;
	m_drill_duration = 3.f;
	m_drill_sucess_rate_bonus = 0.01f;
	m_extraction_duration_bonus = 0.5f;
	m_ore_found = NULL;
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
			ManagePropulsion();
			CheckIfArrivedAtDestination(deltaTime);
			
			break;
		}
		case StarshipState_Idle:
		{
			if (m_task_location)
			{
				MoveToLocation(m_task_location);
			}
			break;
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
			if (m_ore_found)
			{
				LoadOre(m_ore_found->m_display_name, 1);
				m_ore_found = NULL;
				IsNewDrillAttemptAvailable();
			}
			else
			{
				m_target_location->UnloadCarriage(this);//Loading
				MoveToLocation(m_base_location);
			}
			
			break;
		}
		case StarshipState_CarryToBase:
		{
			//GameObject* tmp_obj = (*CurrentGame).GetSceneGameObjectsTyped(LocationObject).front();
			//StockEntity* tmp_location = (StockEntity*)tmp_obj;
			MoveToLocation(m_base_location);
			//MoveToLocation(tmp_location);
			break;
		}
		case StarshipState_Unloading:
		{
			UnloadCarriage(m_target_location);
			m_current_drill_attempts = 0;

			//GameObject* tmp_obj = (*CurrentGame).GetSceneGameObjectsTyped(LocationObject).back();
			//StockEntity* tmp_location = (StockEntity*)tmp_obj;
			MoveToLocation(m_task_location);
			//MoveToLocation(tmp_location);
			break;
		}
		default:
		{
			break;
		}
	}

	GameObject::update(deltaTime);
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
		case StarshipState_Drilling:
		{
			m_drill_clock.restart();
			m_ore_found = NULL;
			printf("Starting drill attempt (%d/%d).\n", m_current_drill_attempts + 1, m_nb_drills);
			break;
		}
		case StarshipState_Searching:
		{
			SetStarshipState(StarshipState_Drilling);
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
			break;
		}
		case StarshipState_Loading:
		{
			break;
		}
		case StarshipState_Unloading:
		{
			break;
		}
		default:
		{
			break;
		}
	}
}

size_t Starship::LoadFuel(string ore_name, size_t quantity)
{
	size_t quantity_loaded = StockEntity::LoadFuel(ore_name, quantity);
	m_propulsion += quantity_loaded * (size_t)stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Propulsion]);

	return quantity_loaded;
}

size_t Starship::LoadRequiredPropulsion(StockEntity* location, size_t propulsion_missing)
{
	if (!location || location != m_target_location || !m_arrived_at_distination || !location->CanSupplyFuel() || propulsion_missing == 0)
	{
		return propulsion_missing;
	}

	string fuel_type_selected = location->GetBestPropulsionAvailable();
	if (fuel_type_selected.empty())
	{
		return propulsion_missing;
	}

	size_t propulsion_required = propulsion_missing;
	map<string, size_t> virtual_storage = location->m_fuel_tanks;
	map<string, size_t> assigned_storage;
	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		assigned_storage[i->first] = 0;
	}

	while (!fuel_type_selected.empty() && propulsion_required > 0)
	{
		size_t quantity_assigned = MinBetweenSizeTValues(virtual_storage[fuel_type_selected], propulsion_required / (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]));
		virtual_storage[fuel_type_selected] -= quantity_assigned;
		assigned_storage[fuel_type_selected] += quantity_assigned;
		propulsion_required -= quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);

		//resolving round up cases (some fuel will be wasted)
		if (propulsion_required > 0 && propulsion_required < (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]) && virtual_storage[fuel_type_selected] > 0)
		{
			virtual_storage[fuel_type_selected]--;
			assigned_storage[fuel_type_selected]++;
			propulsion_required = 0;
		}

		if (propulsion_required > 0)
		{
			if (fuel_type_selected == GetBestPropulsionAvailable(virtual_storage))
			{
				break;
			}
			else
			{
				fuel_type_selected = GetBestPropulsionAvailable(virtual_storage);
			}
		}
	}

	//found enough propulsion?
	if (propulsion_required == 0)
	{
		for (map<string, size_t>::iterator i = assigned_storage.begin(); i != assigned_storage.end(); ++i)
		{
			m_fuel_tanks[i->first] += i->second;
			location->m_fuel_tanks[i->first] -= i->second;
			location->m_fuel -= i->second * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Weight]);
		}

		printf("Got %d of the propulsion required from planet's fuel.\n", propulsion_missing);
		m_propulsion += propulsion_missing;

		return 0;
	}
	else
	{
		return propulsion_required;
	}
}

size_t Starship::AssignPropulsionToTravel(size_t distance)
{
	size_t propulsion_assigned = 0;
	size_t distance_remaining = distance;

	string fuel_type_selected = GetBestPropulsionAvailable();
	while (!fuel_type_selected.empty() && distance_remaining > 0)
	{
		size_t quantity_assigned = MinBetweenSizeTValues(m_fuel_tanks[fuel_type_selected], distance_remaining / (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]));
		m_fuel_tanks[fuel_type_selected] -= quantity_assigned;
		m_fuel_assigned[fuel_type_selected] += quantity_assigned;
		propulsion_assigned += quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);
		distance_remaining -= quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);

		//resolving round up cases (some fuel will be wasted)
		if (distance_remaining > 0 && distance_remaining < (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]) && m_fuel_tanks[fuel_type_selected] > 0)
		{
			m_fuel_tanks[fuel_type_selected]--;
			m_fuel_assigned[fuel_type_selected]++;
			propulsion_assigned += (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);
			distance_remaining = 0;
		}

		if (distance_remaining > 0)
		{
			fuel_type_selected = GetBestPropulsionAvailable();
		}
	}

	return propulsion_assigned;
}

size_t Starship::ConsummePropulsion(size_t distance)
{
	size_t propulsion_consummed = 0;
	size_t distance_remaining = distance;

	string fuel_type_selected = GetBestPropulsionAvailable(m_fuel_assigned);
	while (!fuel_type_selected.empty() && distance_remaining > 0)
	{
		size_t quantity_assigned = MinBetweenSizeTValues(m_fuel_assigned[fuel_type_selected], distance_remaining / (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]));
		m_fuel_assigned[fuel_type_selected] -= quantity_assigned;
		m_fuel -= quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Weight]);
		propulsion_consummed += quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);
		distance_remaining -= quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);

		//resolving round up cases (no waste)
		if (distance_remaining > 0 && distance_remaining < (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]) && m_fuel_assigned[fuel_type_selected] > 0)
		{
			return propulsion_consummed;
		}

		if (distance_remaining > 0)
		{
			fuel_type_selected = GetBestPropulsionAvailable(m_fuel_assigned);
		}
	}

	m_propulsion_speed_bonus = stof((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_PropulsionSpeedBonus]);

	return propulsion_consummed;
}

bool Starship::AssignToLocation(StockEntity* location)
{
	//what to do when assigning a ship to a location?
	if (!location || m_target_location == location)
	{
		return false;
	}

	if (location->CanBeDrilled())
	{
		if (m_target_location == m_task_location)
		{
			if (MoveToLocation(location))
			{
				m_task_location = location;
			}
		}
		else if (m_target_location == m_base_location)
		{
			m_task_location = location;
		}
		else
		{
			if (MoveToLocation(location))
			{
				m_task_location = location;
			}
		}
	}
	else if (location->CanSupplyFuel())
	{
		if (m_target_location == m_task_location)
		{
			if (MoveToLocation(location))
			{
				m_task_location = NULL;
			}
		}
		else if (m_target_location == m_base_location)
		{
			m_task_location = location;
		}
		else
		{
			if (MoveToLocation(location))
			{
				m_task_location = location;
			}
		}
	}

	return true;
}

bool Starship::MoveToLocation(StockEntity* location)
{
	if (!location || !m_base_location)
	{
		return false;
	}

	if (location == m_target_location)
	{
		return false;
	}

	size_t distance = GetLightYearsBetweenObjects(this, location);
	size_t distance_return = GetLightYearsBetweenObjects(location, m_base_location);
	size_t propulsion_required = location->CanSupplyFuel() ? distance : distance + distance_return;//prepare for a back and forth if destination cannot supply fuel
	if (m_propulsion_assigned + m_propulsion < propulsion_required)
	{
		size_t propulsion_missing = LoadRequiredPropulsion(m_base_location, propulsion_required - m_propulsion);
		if (propulsion_missing > 0)
		{
			printf("Trying to assign to location too far: distance is %d, propulsion remaining is %d (%d missing).\n", distance, m_propulsion, propulsion_missing);
			return false;
		}
	}

	printf("Assigning to location %s (distance: %d, current propulsion: %d).\n", location->m_display_name.c_str(), propulsion_required, m_propulsion);

	//Move to location
	m_speed.x = location->getPosition().x - this->getPosition().x;
	m_speed.y = location->getPosition().y - this->getPosition().y;
	NormalizeSpeed(&m_speed, m_speed_max * (1+m_propulsion_speed_bonus));

	size_t propulsion_assigned = AssignPropulsionToTravel(propulsion_required);
	m_propulsion -= propulsion_assigned;
	m_propulsion_assigned = propulsion_assigned;
	m_arrived_at_distination = false;
	m_target_location = location;
	//if (location->CanSupplyFuel())
	//{
	//	m_base_location = location;
	//}
	//else if (location->CanBeDrilled())
	//{
	//	m_task_location = location;
	//}
	SetStarshipState(StarshipState_MovingToLocation);

	return true;
}

bool Starship::ManagePropulsion()
{
	if (!m_target_location || !m_base_location || m_arrived_at_distination)
	{
		return false;
	}

	size_t distance_remaining = m_target_location == m_base_location ? GetLightYearsBetweenObjects(this, m_target_location) : GetLightYearsBetweenObjects(this, m_target_location) + GetLightYearsBetweenObjects(m_target_location, m_base_location);
	if (distance_remaining < m_propulsion_assigned)
	{
		
		size_t propulsion_to_consumme = m_propulsion_assigned - distance_remaining;
		ConsummePropulsion(propulsion_to_consumme);
		m_propulsion_assigned -= propulsion_to_consumme;

		//finish by a waste?
		if (distance_remaining == 0)
		{
			for (map<string, size_t>::iterator i = m_fuel_assigned.begin(); i != m_fuel_assigned.end(); ++i)
			{
				size_t fuel_remaining = i->second * (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);
				i->second = 0;
				m_fuel -= fuel_remaining;
			}

			m_propulsion_assigned = 0;
		}
	}

	return true;
}

bool Starship::CheckIfArrivedAtDestination(sf::Time deltaTime)
{
	if (!m_target_location)
	{
		return false;
	}

	if (GetDistanceBetweenObjects(this, m_target_location) < this->GetAbsoluteSpeed() * deltaTime.asSeconds())
	{
		setPosition(m_target_location->getPosition());
		m_speed = sf::Vector2f(0, 0);
		m_arrived_at_distination = true;

		if (m_target_location == m_base_location)
		{
			SetStarshipState(StarshipState_Unloading);
		}
		if (m_target_location == m_task_location)
		{
			if (m_target_location->CanBeDrilled())
			{
				SetStarshipState(StarshipState_Searching);
			}
			else
			{
				SetStarshipState(StarshipState_Loading);
				m_current_drill_attempts = m_nb_drills;
			}
		}

		return true;
	}

	return false;
}

Starship* Starship::CreateStarship(string name)
{
	if (name.empty())
	{
		return NULL;
	}

	string textureName = (*CurrentGame).m_starshipConfig[name][StarshipData_TextureName];
	
	Starship* new_starship = new Starship(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(69, 84), sf::Vector2f(34.5, 42), 3, 1);

	//cost
	if (!(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType1].empty())
	{
		new_starship->m_cost[(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType1]] = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_OreCostQuantity1]);
	}
	if (!(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType2].empty())
	{
		new_starship->m_cost[(*CurrentGame).m_starshipConfig[name][StarshipData_OreCostType2]] = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_OreCostQuantity2]);
	}

	new_starship->m_display_name = (*CurrentGame).m_starshipConfig[name][StarshipData_Name];
	new_starship->m_armor_max = stoi((*CurrentGame).m_starshipConfig[name][StarshipData_Armor]);
	new_starship->m_fuel_max = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_FuelMax]);
	new_starship->m_speed_max = stof((*CurrentGame).m_starshipConfig[name][StarshipData_SpeedMax]);
	new_starship->m_stock_max = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_StockMax]);

	new_starship->m_nb_drills = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_NbDrills]);
	new_starship->m_drill_sucess_rate_bonus = stof((*CurrentGame).m_starshipConfig[name][StarshipData_DrillSuccessRateBonus]);
	new_starship->m_drill_duration = stof((*CurrentGame).m_starshipConfig[name][StarshipData_DrillDuration]);
	new_starship->m_extraction_duration_bonus = stof((*CurrentGame).m_starshipConfig[name][StarshipData_ExtractionDurationBonus]);

	return new_starship;
}

void Starship::Drill()
{
	if (m_target_location && m_target_location->CanBeDrilled() && m_state == StarshipState_Drilling)
	{
		if (m_nb_drills > 0 && m_current_drill_attempts < m_nb_drills)
		{
			//Drilling attempt
			if (m_drill_clock.getElapsedTime().asSeconds() > m_drill_duration)
			{
				m_ore_found = m_target_location->GetRandomOre();
				m_current_drill_attempts++;
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

void Starship::Extract(Ore* ore)
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

bool Starship::IsNewDrillAttemptAvailable()
{
	if (m_current_drill_attempts == m_nb_drills || m_stock >= m_stock_max - m_target_location->m_min_ore_weight)
	{
		if (m_current_drill_attempts == m_nb_drills)
		{
			printf("Drill attempts exhausted.\n");
		}
		else if (m_stock >= m_stock_max - m_target_location->m_min_ore_weight)
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