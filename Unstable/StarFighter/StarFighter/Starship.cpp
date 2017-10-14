#include "Starship.h"

extern Game* CurrentGame;

using namespace sf;

//STARSHIP
Starship::Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : StockEntity(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	SetStarshipState(StarshipState_Idle);

	m_mission = StarshipMission_Idle;

	m_scout_range = 0;
	m_armor_max = 1;
	m_armor = m_armor_max;
	m_fuel_tank_max = 100;
	m_speed_max = 100;
	m_nb_drills = 0;
	m_current_destination_location = NULL;
	m_mission_base_location = NULL;
	m_mission_task_location = NULL;
	m_current_location = NULL;
	m_arrived_at_destination = true;
	m_propulsion = 0;
	m_propulsion_speed_bonus = 0;
	m_current_zone = GetCurrentZone();
	m_weight = 0;
	m_fuel_tank = pair<string, size_t>("oil", 0);

	m_scout_circle.setRadius(1);
	m_scout_circle.setFillColor(sf::Color(0, 0, 255, 20));
	m_scout_circle.setOutlineThickness(2);
	m_scout_circle.setOutlineColor(sf::Color(0, 0, 255, 255));

	m_travel_circle.setRadius(1);
	m_travel_circle.setFillColor(sf::Color(0, 255, 0, 20));
	m_travel_circle.setOutlineThickness(2);
	m_travel_circle.setOutlineColor(sf::Color(0, 255, 0, 255));

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		if ((size_t)stoi(i->second[OreData_Propulsion]) > 0)
		{
			m_fuel_stock.insert(map<string, size_t>::value_type(i->first, 0));
		}
	}

	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); i++)
	{
		m_cost.insert(map<string, size_t>::value_type(i->first, 0));
	}

	//mining
	m_drill_attempts = 0;
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
	if (m_mission != StarshipMission_Idle)
	{
		ManageMission(deltaTime);
	}


	if (m_scout_range > 0)
	{
		Scout();
	}

	//update memory of position within the stellar map (which grid cell are we in?)
	UpdateZoneKnowledge();
	
	//switch (m_state)
	//{
	//	case StarshipState_MovingToLocation:
	//	{
	//		//ManagePropulsion();
	//		//CheckIfArrivedAtDestination(deltaTime);
	//		
	//		break;
	//	}
	//	case StarshipState_Idle:
	//	{
	//		if (m_mission_task_location)
	//		{
	//			//MoveToLocation(m_mission_task_location);
	//		}
	//		break;
	//	}
	//	case StarshipState_Drilling:
	//	{
	//		Drill();
	//		break;
	//	}
	//	case StarshipState_Extracting:
	//	{
	//		Extract(m_ore_found);
	//		break;
	//	}
	//	case StarshipState_Loading:
	//	{
	//		if (m_ore_found)
	//		{
	//			LoadInStock(m_ore_found->m_display_name, 1);
	//			m_ore_found = NULL;
	//			IsNewDrillAttemptAvailable();
	//		}
	//		else
	//		{
	//			m_current_destination_location->UnloadCarriage(this);//Loading
	//			//MoveToLocation(m_mission_base_location);
	//		}
	//		
	//		break;
	//	}
	//	case StarshipState_CarryToBase:
	//	{
	//		//GameObject* tmp_obj = (*CurrentGame).GetSceneGameObjectsTyped(LocationObject).front();
	//		//StockEntity* tmp_location = (StockEntity*)tmp_obj;
	//		//MoveToLocation(m_mission_base_location);
	//		//MoveToLocation(tmp_location);
	//		break;
	//	}
	//	case StarshipState_Unloading:
	//	{
	//		UnloadCarriage(m_current_destination_location);
	//		m_drill_attempts = 0;
	//
	//		//GameObject* tmp_obj = (*CurrentGame).GetSceneGameObjectsTyped(LocationObject).back();
	//		//StockEntity* tmp_location = (StockEntity*)tmp_obj;
	//		//if (!MoveToLocation(m_mission_task_location))
	//		//{
	//		//	SetStarshipState(StarshipState_Idle);
	//		//}
	//		
	//		//MoveToLocation(tmp_location);
	//		break;
	//	}
	//	case StarshipState_Scanning:
	//	{
	//		Scan();
	//		break;
	//	}
	//	default:
	//	{
	//		break;
	//	}
	//}

	StockEntity::update(deltaTime);

	m_scout_circle.setRadius(m_scout_range);
	m_scout_circle.setPosition(sf::Vector2f(getPosition().x - m_scout_range, getPosition().y - m_scout_range));

	//float propulsion_available = GetPropulsionAvailable();
	//float propulsion_radius = propulsion_available * LIGHTYEAR_IN_PIXELS / m_weight;
	//m_travel_circle.setRadius(propulsion_radius);
	//m_travel_circle.setPosition(sf::Vector2f(getPosition().x - propulsion_radius, getPosition().y - propulsion_radius));
}

void Starship::Draw(sf::RenderTexture& screen)
{
	if (m_selected && m_visible)
	{
		if (m_scout_range > 0 && (m_state == StarshipState_MovingToZone))
		{
			screen.draw(m_scout_circle);
		}
		
		if (m_state != StarshipState_MovingToZone)
		{
			screen.draw(m_travel_circle);
		}
	}

	GameObject::Draw(screen);
}

void Starship::SetStarshipState(StarshipState state)
{
	m_state = state;

	switch (state)
	{
		//case StarshipState_MovingToLocation:
		//{
		//	break;
		//}
		case StarshipState_MovingToZone:
		{
			printf("Exploration begins: going to zone %d-%d.\n", (*CurrentGame).m_stellarmap->GetZoneIndex(m_current_destination_coordinates));
			break;
		}
		case StarshipState_Drilling:
		{
			m_drill_clock.restart();
			m_ore_found = NULL;
			printf("Starting drill attempt (%d/%d).\n", m_drill_attempts + 1, m_nb_drills);
			break;
		}
		case StarshipState_Scouting:
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
		case StarshipState_Scanning:
		{
			m_scan_clock.restart();
			break;
		}
		default:
		{
			break;
		}
	}
}

sf::Vector2u Starship::GetCurrentZone()
{
	return (*CurrentGame).m_stellarmap->GetZoneIndex(getPosition());
}

void Starship::UpdateZoneKnowledge()
{
	m_current_zone = GetCurrentZone();

	string key = StellarMapVirtual::GetVectorString(m_current_zone);
	if (!(*CurrentGame).m_stellarmap->isZoneKnown(key))
	{
		if (!(*CurrentGame).m_stellarmap->isZoneGenerated(key))
		{	
			(*CurrentGame).m_stellarmap->m_known_zones.insert(map<string, bool>::value_type(key, false));
		}
		(*CurrentGame).m_stellarmap->SetZoneAsKnown(m_current_zone);
		//(*CurrentGame).m_stellarmap->m_known_zones[key] = true;
		//(*CurrentGame).m_stellarmap->m_zones[m_current_zone.x][m_current_zone.y]->setOutlineColor(sf::Color(255, 255, 255, 128));
	
		(*CurrentGame).m_stellarmap->ExpandKnownStellarMap(m_current_zone);
	}
}

bool Starship::Scout()
{
	return (*CurrentGame).RevealObjectsAtPosition(getPosition(), m_scout_range, LocationObject);
}

size_t Starship::LoadFuelTank(string fuel_name, size_t quantity)
{
	//load no more than the max
	size_t quantity_accepted = quantity > m_fuel_tank_max - m_fuel_tank.second ? m_fuel_tank_max - m_fuel_tank.second : quantity;
	m_fuel_tank.first = fuel_name;
	m_fuel_tank.second += quantity_accepted;

	return quantity_accepted;
}

/*
size_t Starship::LoadRequiredPropulsion(StockEntity* location, size_t propulsion_missing, bool simulation)
{
	if (!location || !m_arrived_at_destination || propulsion_missing == 0)
	{
		return propulsion_missing;
	}

	string fuel_type_selected = location->GetBestPropulsionAvailable();
	if (fuel_type_selected.empty())
	{
		return propulsion_missing;
	}

	size_t propulsion_required = propulsion_missing;
	map<string, size_t> virtual_storage = location->m_fuel_stock;
	map<string, size_t> assigned_storage;
	for (map<string, vector<string> >::iterator i = (*CurrentGame).m_oreConfig.begin(); i != (*CurrentGame).m_oreConfig.end(); ++i)
	{
		if ((size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) > 0)
		{
			assigned_storage[i->first] = 0;
		}
	}

	//fill the required propulsion with the best fuel type available, recursively
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
			if (fuel_type_selected == StockEntity::GetBestPropulsionAvailable(virtual_storage))
			{
				break;
			}
			else
			{
				fuel_type_selected = StockEntity::GetBestPropulsionAvailable(virtual_storage);
			}
		}
	}

	//found enough propulsion?
	if (simulation)
	{
		return propulsion_required;
	}
	else if (propulsion_required == 0)
	{
		for (map<string, size_t>::iterator i = assigned_storage.begin(); i != assigned_storage.end(); ++i)
		{
			m_fuel_stock[i->first] += i->second;
			m_fuel += i->second * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Weight]);
			location->m_fuel_stock[i->first] -= i->second;
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
	//subtracting fuel already assigned from the distance to cover
	size_t distance_remaining = m_propulsion_assigned == 0 ? distance : (distance > m_propulsion_assigned ? distance - m_propulsion_assigned : 0);

	string fuel_type_selected = GetBestPropulsionAvailable();
	while (!fuel_type_selected.empty() && distance_remaining > 0)
	{
		size_t quantity_assigned = MinBetweenSizeTValues(m_fuel_stock[fuel_type_selected], distance_remaining / (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]));
		m_fuel_stock[fuel_type_selected] -= quantity_assigned;
		m_fuel_assigned[fuel_type_selected] += quantity_assigned;
		propulsion_assigned += quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);
		distance_remaining -= quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);

		//resolving round up cases (some fuel will be wasted)
		if (distance_remaining > 0 && distance_remaining < (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]) && m_fuel_stock[fuel_type_selected] > 0)
		{
			m_fuel_stock[fuel_type_selected]--;
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

size_t Starship::ConsummePropulsion(size_t propulsion_to_consumme)
{
	size_t propulsion_consummed = 0;
	size_t distance_remaining = propulsion_to_consumme;

	string fuel_type_selected = GetBestAssignedPropulsionAvailable();
	while (!fuel_type_selected.empty() && distance_remaining > 0)
	{
		size_t quantity_assigned = MinBetweenSizeTValues(m_fuel_assigned[fuel_type_selected], distance_remaining / (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]));
		m_fuel_assigned[fuel_type_selected] -= quantity_assigned;
		m_fuel -= quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Weight]);
		size_t production_consummed_this_fuel_type = quantity_assigned * (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]);
		propulsion_consummed += production_consummed_this_fuel_type;
		distance_remaining -= production_consummed_this_fuel_type;

		if (production_consummed_this_fuel_type > m_propulsion_assigned)
		{
			printf("bug");
		}

		m_propulsion_assigned -= production_consummed_this_fuel_type;


		//resolving round up cases (no waste)
		if (distance_remaining > 0 && distance_remaining < (size_t)stoi((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_Propulsion]) && m_fuel_assigned[fuel_type_selected] > 0)
		{
			return propulsion_consummed;
		}
		

		if (distance_remaining > 0)
		{
			fuel_type_selected = GetBestAssignedPropulsionAvailable();
		}
	}

	m_propulsion_speed_bonus = stof((*CurrentGame).m_oreConfig[fuel_type_selected][OreData_PropulsionSpeedBonus]);
	ScaleSpeed(&m_speed, m_speed_max * (1 + m_propulsion_speed_bonus));

	return propulsion_consummed;
}


bool Starship::AssignToLocation(StockEntity* location)
{
	//what to do when assigning a ship to a location?
	if (!location || m_current_destination_location == location)
	{
		return false;
	}

	if (!MoveToLocation(location))
	{
		return false;
	}

	if (m_scout_range > 0 && !m_mission_task_location)//scout mission?
	{
		if (!location->m_identified)
		{
			//accept task (and there is no coming back)
			m_mission_task_location = location;
		}
	}
	else if (m_current_destination_location != m_mission_base_location)//change base location?
	{	
		if (location->CanSupplyFuel())
		{
			//go to planet and cancel task
			m_mission_task_location = NULL;
		}

		if (location->CanBeDrilled())//new drilling task?
		{
			//accept new task?
			m_mission_task_location = location;
		}
	}
	else if (m_current_destination_location != m_mission_task_location)//new drilling task?
	{
		if (location->CanBeDrilled())
		{
			//accept new next task
			m_mission_task_location = location;
		}
	}

	return true;
}



bool Starship::MoveToLocation(StockEntity* location)
{
	if (!location || !m_mission_base_location)
	{
		return false;
	}

	if (location == m_current_destination_location)
	{
		return false;
	}

	size_t distance = GetLightYearsBetweenObjects(this, location);
	//size_t distance_return = GetLightYearsBetweenObjects(location, m_mission_base_location);
	//size_t propulsion_required = location->CanSupplyFuel() ? distance : distance + distance_return;//prepare for a back and forth if destination cannot supply fuel
	//propulsion_required *= GetTotalWeight();

	size_t propulsion_required = GetPropulsionRequired(location);
	if (m_propulsion_assigned + m_propulsion < propulsion_required)
	{
		size_t propulsion_missing = LoadRequiredPropulsion(m_mission_base_location, propulsion_required - m_propulsion, false);
		if (propulsion_missing > 0)
		{
			//printf("Trying to assign to location too far: distance is %d, propulsion remaining is %d (%d missing).\n", distance, m_propulsion, propulsion_missing);
			return false;
		}
	}

	printf("Assigning to location %s (distance: %d, current propulsion: %d).\n", location->m_display_name.c_str(), propulsion_required, m_propulsion);

	//Move to location
	m_speed.x = location->getPosition().x - this->getPosition().x;
	m_speed.y = location->getPosition().y - this->getPosition().y;
	NormalizeSpeed(&m_speed, m_speed_max);

	size_t propulsion_assigned = AssignPropulsionToTravel(propulsion_required);
	propulsion_assigned = MinBetweenSizeTValues(propulsion_assigned, m_propulsion);
	m_propulsion -= propulsion_assigned;
	m_propulsion_assigned += propulsion_assigned;
	m_arrived_at_destination = false;
	m_current_destination_location = location;
	//if (location->CanSupplyFuel())
	//{
	//	m_mission_base_location = location;
	//}
	//else if (location->CanBeDrilled())
	//{
	//	m_mission_task_location = location;
	//}
	SetStarshipState(StarshipState_MovingToLocation);

	return true;
}

bool Starship::ManagePropulsion()
{
	if (!m_current_destination_location || !m_mission_base_location || m_arrived_at_destination)
	{
		return false;
	}

	size_t distance_remaining = m_current_destination_location == m_mission_base_location ? GetLightYearsBetweenObjects(this, m_current_destination_location) : GetLightYearsBetweenObjects(this, m_current_destination_location) + GetLightYearsBetweenObjects(m_current_destination_location, m_mission_base_location);
	if (distance_remaining < m_propulsion_assigned)
	{
		size_t propulsion_to_consumme = m_propulsion_assigned - distance_remaining;
		ConsummePropulsion(propulsion_to_consumme);

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

*/

bool Starship::CheckIfArrivedAtDestination(sf::Time deltaTime)
{
	if (!m_current_destination_location)
	{
		return false;
	}

	if (GetDistanceBetweenObjects(this, m_current_destination_location) < this->GetAbsoluteSpeed() * deltaTime.asSeconds())
	{
		setPosition(m_current_destination_location->getPosition());
		m_speed = sf::Vector2f(0, 0);
		m_arrived_at_destination = true;
		m_current_location = m_current_destination_location;

		if (m_current_destination_location == m_mission_base_location)
		{
			SetStarshipState(StarshipState_Unloading);
		}
		if (m_current_destination_location == m_mission_task_location)
		{
			if (m_scout_range > 0)
			{
				SetStarshipState(StarshipState_Scanning);
			}
			else
			{
				if (m_current_destination_location->CanBeDrilled())
				{
					SetStarshipState(StarshipState_Scouting);
				}
				else
				{
					SetStarshipState(StarshipState_Loading);
					m_drill_attempts = m_nb_drills;
				}
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
	new_starship->m_fuel_tank_max = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_FuelTankMax]);
	new_starship->m_weight = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_Weight]);
	new_starship->m_speed_max = stof((*CurrentGame).m_starshipConfig[name][StarshipData_SpeedMax]);
	new_starship->m_ore_stock_max = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_OreStockMax]);
	new_starship->m_fuel_stock_max = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_FuelStockMax]);

	new_starship->m_nb_drills = (size_t)stoi((*CurrentGame).m_starshipConfig[name][StarshipData_NbDrills]);
	new_starship->m_drill_sucess_rate_bonus = stof((*CurrentGame).m_starshipConfig[name][StarshipData_DrillSuccessRateBonus]);
	new_starship->m_drill_duration = (float)(stof((*CurrentGame).m_starshipConfig[name][StarshipData_DrillDuration]));
	new_starship->m_extraction_duration_bonus = stof((*CurrentGame).m_starshipConfig[name][StarshipData_ExtractionDurationBonus]);

	new_starship->m_scout_range = (float)(stof((*CurrentGame).m_starshipConfig[name][StarshipData_ScoutRange]));

	return new_starship;
}

Ore* Starship::Drill()
{
	//Drilling attempt
	if (m_drill_clock.getElapsedTime().asSeconds() > m_drill_duration)
	{
		m_drill_attempts++;

		m_ore_found = m_mission_task_location->DigRandomOre();
		
		if (m_ore_found)
		{
			printf("Ore found: %s\n", m_ore_found->m_display_name.c_str());
			printf("Extraction...");
			m_extraction_clock.restart();
		}
		else
		{
			printf("No ore found.\n");
		}

		return m_ore_found;
	}

	return NULL;
}

bool Starship::Scan(StockEntity* entity)
{
	if (m_scan_clock.getElapsedTime().asSeconds() > SCAN_DURATION)
	{
		entity->m_identified = true;
	}

	return  entity->m_identified;
}

bool Starship::Extract(Ore* ore)
{
	//Extract
	if (m_extraction_clock.getElapsedTime().asSeconds() > MaxBetweenValues(sf::Vector2f(ore->m_extraction_duration - m_extraction_duration_bonus, 0)))
	{
		printf("complete. Loading ore.\n");
		return true;
	}
	else
	{
		m_state = StarshipState_Extracting;
	}

	return false;
}

bool Starship::IsNewDrillAttemptAvailable()
{
	if (m_drill_attempts == m_nb_drills || m_current_ore_stock >= m_ore_stock_max)
	{
		if (m_drill_attempts == m_nb_drills)
		{
			printf("Drill attempts exhausted.\n");
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

//string Starship::GetBestAssignedPropulsionAvailable()
//{
//	size_t propulsion = 0;
//	string selected_fuel;
//
//	for (map<string, size_t>::iterator i = m_fuel_assigned.begin(); i != m_fuel_assigned.end(); ++i)
//	{
//		if ((size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]) > propulsion && i->second > 0)
//		{
//			selected_fuel = i->first;
//			propulsion = (size_t)stoi((*CurrentGame).m_oreConfig[i->first][OreData_Propulsion]);
//		}
//	}
//
//	return selected_fuel;
//}

size_t Starship::GetPropulsionRequired(GameObject* destination)
{
	size_t distance = GameObject::GetLightYearsBetweenObjects(this, destination);
	size_t distance_return = GameObject::GetLightYearsBetweenObjects(destination, m_mission_base_location);
	size_t propulsion_required = destination->CanSupplyFuel() ? distance : distance + distance_return;//prepare for a back and forth if destination cannot supply fuel
	propulsion_required *= m_weight;

	return propulsion_required;
}

bool Starship::AssignMission(StarshipMission mission, sf::Vector2f destination, StockEntity* task_location, StockEntity* base_location)
{
	m_mission = mission;
	m_arrived_at_destination = false;

	switch (mission)
	{
		case StarshipMission_Scout:
		{
			SetSpeedForConstantSpeedToDestination(destination, m_speed_max);

			m_current_destination_coordinates = destination;

			m_current_destination_location = NULL;
			m_mission_base_location = NULL;
			m_mission_task_location = NULL;

			//SetStarshipState(StarshipState_MovingToZone);
			break;
		}
		case StarshipMission_Scan:
		{
			SetSpeedForConstantSpeedToDestination(task_location->getPosition(), m_speed_max);

			m_current_destination_location = task_location;
			m_mission_base_location = NULL;
			m_mission_task_location = task_location;

			//SetStarshipState(StarshipState_MovingToLocation);
			break;
		}
		case StarshipMission_Drill:
		{
			SetSpeedForConstantSpeedToDestination(task_location->getPosition(), m_speed_max);

			m_current_destination_location = task_location;
			m_mission_base_location = base_location;
			m_mission_task_location = task_location;

			break;
		}
	}

	return false;
}

void Starship::ManageMission(sf::Time deltaTime)
{
	if (!m_arrived_at_destination && ArrivingAtDestination(deltaTime))
	{
		switch (m_mission)
		{
			case StarshipMission_Scout:
				m_visible = false;
				m_GarbageMe = true;
				break;
			case StarshipMission_Scan:
				m_scan_clock.restart();
				break;
			case StarshipMission_Drill:
				if (m_current_destination_location == m_mission_task_location)
				{
					m_state = StarshipState_Drilling;
					m_drill_clock.restart();
					printf("Starting drill attempt (%d/%d).\n", m_drill_attempts + 1, m_nb_drills);
				}
				else
				{
					UnloadCarriage(m_current_location);
					m_drill_attempts = 0;
					//m_arrived_at_destination = false;
					//m_current_destination_location = m_mission_task_location;
					//SetSpeedForConstantSpeedToDestination(m_mission_task_location->getPosition(), m_speed_max);
					m_state = StarshipState_Idle;
					m_mission = StarshipMission_Idle;
					m_mission_task_location = NULL;
					m_mission_base_location = NULL;
					m_current_destination_location = NULL;
				}
				 
				break;
		}
	}

	if (m_arrived_at_destination)
	{
		switch (m_mission)
		{
			case StarshipMission_Scan:
				if (Scan(m_mission_task_location))
				{
					m_visible = false;
					m_GarbageMe = true;
				}
				break;
			case StarshipMission_Drill:
				if (m_ore_found || Drill() != NULL)
				{
					if (Extract(m_ore_found))
					{
						LoadInStock(m_ore_found->m_display_name, 1);
						m_ore_found = NULL;
						m_drill_clock.restart();

						//time to go back?
						if (m_drill_attempts >= m_nb_drills)
						{
							m_state = StarshipState_CarryToBase;
							m_arrived_at_destination = false;
							m_current_destination_location = m_mission_base_location;
							SetSpeedForConstantSpeedToDestination(m_mission_base_location->getPosition(), m_speed_max);
						}
						else
						{
							printf("Starting drill attempt (%d/%d).\n", m_drill_attempts + 1, m_nb_drills);
						}
					}
				}
				
				break;
		}
	}

//	if (m_current_destination_location == m_mission_base_location)
//	{
//		SetStarshipState(StarshipState_Unloading);
//	}
//	if (m_current_destination_location == m_mission_task_location)
//	{
//		if (m_scout_range > 0)
//		{
//			SetStarshipState(StarshipState_Scanning);
//		}
//		else
//		{
//			if (m_current_destination_location->CanBeDrilled())
//			{
//				SetStarshipState(StarshipState_Scouting);
//			}
//			else
//			{
//				SetStarshipState(StarshipState_Loading);
//				m_drill_attempts = m_nb_drills;
//			}
//		}
//	}
//
//	return true;
//}
//
//return false;
}

bool Starship::ArrivingAtDestination(sf::Time deltaTime)
{
	//going to coordinates or going to a location's coordinates?
	sf::Vector2f destination = m_mission == StarshipMission_Scout ? m_current_destination_coordinates : m_current_destination_location->getPosition();

	//are we supposed to arrive at destination at the next frame?
	if (GetDistanceBetweenPositions(getPosition(), destination) < GetAbsoluteSpeed() * deltaTime.asSeconds())
	{
		//yes? let's land then!
		setPosition(destination);
		m_speed = sf::Vector2f(0, 0);
		m_arrived_at_destination = true;
		m_current_location = m_current_destination_location;
	}

	return m_arrived_at_destination;
}