#include "Starship.h"

extern Game* CurrentGame;

using namespace sf;

//STARSHIP
Starship::Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	SetStarshipState(StarshipState_Idle);

	m_scout_range = 0;
	m_armor = 1;
	m_fuel = 0;
	m_fuel_max = 100;
	m_speed_max = 100;
	m_stock_max = 0;
	m_stock = 0;
	m_nb_drills = 0;
	m_drill_duration = 1.f;
	m_location= NULL;
	m_assigned_propulsion = 0;
	m_arrived_at_distination = false;
	m_propulsion = 0;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i) 
	{
		if ((size_t)stoi(i->second[OreData_Propulsion]) > 0)
		{
			m_fuel_tanks.insert(map<string, size_t>::value_type(i->first, 0));
			m_fuel_assigned.insert(map<string, size_t>::value_type(i->first, 0));
		}
	}
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
	}
}

string Starship::GetBestPropulsionAvailable()
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

size_t Starship::LoadFuel(string ore_name, size_t quantity)
{
	if (ore_name.empty())
	{
		return 0;
	}

	size_t quantity_accepted = (m_fuel_max - m_fuel) / (size_t)stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Weight]);
	size_t quantity_loaded = MinBetweenSizeTValues(quantity_accepted, quantity);
	m_fuel_tanks[ore_name] += quantity_loaded;
	m_fuel += quantity_loaded*(size_t)stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Weight]);
	m_propulsion += quantity_loaded*(size_t)stoi((*CurrentGame).m_oreConfig[ore_name][OreData_Propulsion]);

	printf("Fuel loading: stock (%d/%d), %s quantity loaded / %d: %d (new propulsion: %d).\n", m_fuel, m_fuel_max, ore_name.c_str(), quantity_loaded, quantity, m_propulsion);

	return quantity_loaded;
}

bool Starship::LoadRequiredPropulsion(Location* location, size_t propulsion_missing)
{
	if (!location || !m_arrived_at_distination)
	{
		return false;
	}

	string fuel_type_selected = location->GetBestPropulsionAvailable();
	if (fuel_type_selected.empty())
	{
		return false;
	}

	size_t propulsion_required = propulsion_missing;
	map<string, size_t> virtual_storage = location->m_ores_stored;
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
			if (fuel_type_selected == GetBestPropulsionAvailable())
			{
				break;
			}
			else
			{
				fuel_type_selected = GetBestPropulsionAvailable();
			}
		}
	}

	//found enough propulsion?
	if (propulsion_required == 0)
	{
		for (map<string, size_t>::iterator i = assigned_storage.begin(); i != assigned_storage.end(); ++i)
		{
			m_fuel_tanks[i->first] += i->second;
			location->m_ores_stored[i->first] -= i->second;
			location->m_stock -= i->second * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Weight]);
		}

		printf("Got %d of the propulsion required from planet's fuel.\n", propulsion_missing);
		m_propulsion += propulsion_missing;

		return true;
	}
	else
	{
		return false;
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

	string fuel_type_selected = GetBestPropulsionAvailable();
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
			fuel_type_selected = GetBestPropulsionAvailable();
		}
	}

	return propulsion_consummed;
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
	size_t propulsion_required = location->CanSupplyFuel() ? distance : 2*distance;//prepare for a back and forth if destination cannot supply fuel
	if (m_propulsion < propulsion_required && !LoadRequiredPropulsion(m_location, propulsion_required - m_propulsion))
	{
		printf("Trying to assign to location too far: distance is %d, propulsion remaining is %d (%d missing).\n", distance, m_propulsion, propulsion_required - m_propulsion);
		return false;
	}

	printf("Assigning to location %s (distance: %d, current propulsion: %d).\n", location->m_display_name.c_str(), propulsion_required, m_propulsion);

	//Move to location
	m_speed.x = location->getPosition().x - this->getPosition().x;
	m_speed.y = location->getPosition().y - this->getPosition().y;
	NormalizeSpeed(&m_speed, m_speed_max);

	m_propulsion -= AssignPropulsionToTravel(propulsion_required);
	m_arrived_at_distination = false;
	m_location = location;
	SetStarshipState(StarshipState_MovingToLocation);

	return true;
}

bool Starship::ManagePropulsion()
{
	if (!m_location || m_arrived_at_distination)
	{
		return false;
	}

	size_t distance_remaining = GetLightYearsBetweenObjects(this, m_location);
	if (distance_remaining < m_assigned_propulsion)
	{
		size_t propulsion_to_consumme = m_assigned_propulsion - distance_remaining;
		ConsummePropulsion(propulsion_to_consumme);
		m_assigned_propulsion -= propulsion_to_consumme;

		//finish by a waste?
		if (distance_remaining == 0)
		{
			for (map<string, size_t>::iterator i = m_fuel_assigned.begin(); i != m_fuel_assigned.end(); ++i)
			{
				size_t fuel_remaining = i->second * (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);
				i->second = 0;
				m_fuel -= fuel_remaining;
			}

			m_assigned_propulsion = 0;
		}
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

//MINER
Miner::Miner(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Starship(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_fuel_max = 100;
	m_speed_max = 150;
	m_stock_max = 100;
	m_nb_drills = 2;
	m_current_drill_attempts = 0;
	m_drill_duration = 3.f;
	m_drill_sucess_rate_bonus = 0.01f;
	m_extraction_duration_bonus = 0.5f;

	m_ore_found = NULL;

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		m_ores_carried[i->first] = 0;
	}
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
			UnloadCarriage(m_location);
			
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

	m_ores_carried[ore->m_display_name]++;
	m_stock += ore->m_weight;
	printf("Ore loading successfull.\n\n");
}

Ore* Miner::GetRandomOre(OreField* ore_field)
{
	if (ore_field)
	{
		for (map<string, float>::iterator i = ore_field->m_drill_sucess_rates.begin(); i != ore_field->m_drill_sucess_rates.end(); ++i)
		{
			if (i->second == 0)
			{
				continue;
			}
			if (RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < i->second + m_drill_sucess_rate_bonus)
			{
				Ore* new_ore = Ore::CreateOre(i->first);
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
			printf("Starting drill attempt (%d/%d).\n", m_current_drill_attempts+1, m_nb_drills);
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

		SetStarshipState(StarshipState_CarryToBase);
		return false;
	}
	else
	{
		SetStarshipState(StarshipState_Drilling);
		return true;
	}
}

void Miner::UnloadCarriage(Location* location)
{
	if (!location || location != m_location || !m_arrived_at_distination)
	{
		return;
	}

	m_current_drill_attempts = 0;

	for (map<string, size_t>::iterator i = m_ores_carried.begin(); i != m_ores_carried.end(); i++)
	{
		if (i->second > 0)
		{
			size_t quantity_accepted = (m_location->m_stock_max - m_location->m_stock) / (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);
			size_t quantity_unloaded = MinBetweenSizeTValues(quantity_accepted, i->second);
			m_location->m_ores_stored[i->first] += quantity_unloaded;
			m_location->m_stock += quantity_unloaded * (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]);
			m_ores_carried[i->first] -= quantity_unloaded;

			printf("\n%s unloaded on planet: quantity %d, total weight of %d (Planet new stock: %d/%d).\n", i->first.c_str(), quantity_unloaded, quantity_unloaded*(size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Weight]), m_location->m_stock, m_location->m_stock_max);
		}
	}

	
}