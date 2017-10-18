#include "Starship.h"

extern Game* CurrentGame;

using namespace sf;

//STARSHIP
Starship::Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : StockEntity(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
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
	m_loop_mission = false;
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
	m_state = StarshipState_Drilling;

	//Drilling attempt
	if (m_drill_clock.getElapsedTime().asSeconds() > m_drill_duration)
	{
		m_drill_attempts++;

		m_ore_found = m_mission_task_location->DigRandomOre();
		
		if (m_ore_found)
		{
			//printf("Ore found: %s\n", m_ore_found->m_display_name.c_str());
			//printf("Extraction...");
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
	if (m_extraction_clock.getElapsedTime().asSeconds() > ore->m_extraction_duration * (1 - m_extraction_duration_bonus))
	{
		return true;
	}
	else
	{
		m_state = StarshipState_Extracting;
	}

	return false;
}

bool Starship::AssignMission(StarshipMission mission, sf::Vector2f destination, StockEntity* task_location, StockEntity* base_location)
{
	switch (mission)
	{
		case StarshipState_Idle:
		{
			m_state = StarshipState_Idle;
			m_mission = StarshipMission_Idle;
			m_mission_task_location = NULL;
			m_mission_base_location = NULL;
			m_current_destination_location = NULL;
			m_current_location = NULL;

			return true;
		}
		case StarshipMission_Scout:
		{
			if (!CheckIfEnoughFuelToDestination(destination))
			{
				return false;
			}

			m_fuel_tank.second -= GetFuelCostToDestination(destination);

			SetSpeedForConstantSpeedToDestination(destination, m_speed_max);

			m_current_destination_coordinates = destination;

			m_current_destination_location = NULL;
			m_mission_base_location = NULL;
			m_mission_task_location = NULL;
			m_current_location = NULL;

			m_mission = mission;
			m_arrived_at_destination = false;

			return true;
		}
		case StarshipMission_Scan:
		{
			if (!CheckIfEnoughFuelToDestination(task_location->getPosition()))
			{
				return false;
			}

			m_fuel_tank.second -= GetFuelCostToDestination(task_location->getPosition());

			SetSpeedForConstantSpeedToDestination(task_location->getPosition(), m_speed_max);

			m_current_destination_location = task_location;
			m_mission_base_location = NULL;
			m_mission_task_location = task_location;
			m_current_location = NULL;

			m_mission = mission;
			m_arrived_at_destination = false;

			return true;
		}
		case StarshipMission_Drill:
		{
			if (!CheckIfEnoughFuelToDestination(task_location->getPosition()))
			{
				return false;
			}

			m_fuel_tank.second -= GetFuelCostToDestination(task_location->getPosition());

			SetSpeedForConstantSpeedToDestination(task_location->getPosition(), m_speed_max);

			m_current_destination_location = task_location;
			m_mission_base_location = base_location;
			m_mission_task_location = task_location;
			m_current_location = NULL;

			m_loop_mission = true;

			m_mission = mission;
			m_arrived_at_destination = false;
			m_state = StarshipState_MovingToLocation;

			return true;
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
				else//ship is back to base location
				{
					UnloadStocks(m_current_location);
					m_drill_attempts = 0;

					if (m_loop_mission)
					{
						if (!AssignMission(StarshipMission_Drill, sf::Vector2f(0, 0), m_mission_task_location, m_mission_base_location))
						{
							AssignMission(StarshipMission_Idle, sf::Vector2f(0, 0), NULL, NULL);
						}
					}
					else
					{
						AssignMission(StarshipMission_Idle, sf::Vector2f(0, 0), NULL, NULL);
					}
				}
				 
				break;
		}
	}
	else if (!m_arrived_at_destination)
	{
		//travel
		
	}
	else//if (m_arrived_at_destination)
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
						if (m_state != StarshipState_CarryToBase)
						{
							LoadInStock(m_ore_found->m_display_name, 1);
							m_drill_clock.restart();
						}
						
						//time to go back?
						if (m_drill_attempts >= m_nb_drills)
						{
							m_state = StarshipState_CarryToBase;

							if (CheckIfEnoughFuelToDestination(m_mission_base_location->getPosition()))
							{
								m_fuel_tank.second -= GetFuelCostToDestination(m_mission_base_location->getPosition());
								m_arrived_at_destination = false;
								m_current_destination_location = m_mission_base_location;
								SetSpeedForConstantSpeedToDestination(m_mission_base_location->getPosition(), m_speed_max);
								m_ore_found = NULL;
							}
						}
						else
						{
							printf("Starting drill attempt (%d/%d).\n", m_drill_attempts + 1, m_nb_drills);
							m_state = StarshipState_Drilling;
							m_ore_found = NULL;
						}
					}
				}
				
				break;
		}
	}
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

bool Starship::CheckIfEnoughFuelToDestination(sf::Vector2f destination)
{
	//size_t distance = (size_t)GetDistanceBetweenPositions(getPosition(), destination) / LIGHTYEAR_IN_PIXELS;
	//
	//size_t propulsion_available = m_fuel_tank.second * (size_t)stoi((*CurrentGame).m_oreConfig[m_fuel_tank.first][OreData_Propulsion]);
	//
	//return distance <= propulsion_available;

	if (destination == getPosition())
	{
		return true;
	}

	size_t propulsion_required = GetFuelCostToDestination(destination);
	size_t propulsion_available = GetPropulsionAvailable();

	return propulsion_required <= propulsion_available;
}

size_t Starship::GetFuelCostToDestination(sf::Vector2f destination)
{
	if (getPosition() == destination)
	{
		return 0;
	}

	size_t propulsion_required = (size_t)(GetDistanceBetweenPositions(getPosition(), destination) / m_speed_max * m_weight) + 1;

	return propulsion_required;
}

size_t Starship::GetPropulsionAvailable()
{
	size_t propulsion_available = m_fuel_tank.second * (size_t)stoi((*CurrentGame).m_oreConfig[m_fuel_tank.first][OreData_Propulsion]);

	return propulsion_available;
}

size_t Starship::UnloadFuelTank(StockEntity* entity)
{
	size_t quantity_unloaded = entity->LoadInStock(m_fuel_tank.first, m_fuel_tank.second);

	m_fuel_tank.second -= quantity_unloaded;

	return quantity_unloaded;
}