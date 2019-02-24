#include "CrewMember.h"

extern Game* CurrentGame;

CrewMember::CrewMember(CrewMemberType type, ShipAlliance alliance, CrewMemberRace race) : GameEntity(UI_CrewMember)
{
	m_type = type;
	m_alliance = alliance;
	m_UI_type = UI_CrewMember;
	m_ref_speed = CREWMEMBER_SPEED;
	m_speed = sf::Vector2f(0, 0);
	m_size = sf::Vector2f(CREWMEMBER_SIZE, CREWMEMBER_SIZE);
	m_destination = NULL;
	m_repair_timer = HULL_REPAIR_TIMER;
	m_drowning_timer = DROWNING_TIMER;
	m_healing_timer = HEALING_TIMER;
	m_is_prisoner = false;

	m_health_max = CREWMEMBER_HEALTH_MAX;
	m_health = m_health_max;
	m_prisoner_roaming_timer = RandomizeFloatBetweenValues(PRISONER_ROAM_TIMER_MIN, PRISONER_ROAM_TIMER_MAX);

	//UI
	m_shape_container.setSize(sf::Vector2f(m_size));
	m_shape_container.setOrigin(sf::Vector2f(m_size.x * 0.5f, m_size.y * 0.5f));
	m_shape_container.setOutlineThickness(1.f);
	m_shape_container.setOutlineColor(sf::Color::White);

	m_texture_name = "2D/crew_icon.png";
	m_texture_big_name = "2D/crew_icon_big.png";

	int skill_max_value = 0;
	switch (type)
	{
		case Crew_Pirate:
		{
			m_shape_container.setFillColor(sf::Color::Magenta);
			skill_max_value = 20;
			break;
		}
		case Crew_Civilian:
		{
			m_shape_container.setFillColor(sf::Color::Magenta);
			skill_max_value = 10;
			break;
		}
		case Crew_Slave:
		{
			m_shape_container.setFillColor(sf::Color::Magenta);
			skill_max_value = 5;
			break;
		}
		case Crew_Undead:
		{
			m_shape_container.setFillColor(sf::Color::Magenta);
			skill_max_value = 15;
			break;
		}
	}

	//randomization of skill values
	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		int skill_value = RandomizeIntBetweenValues(skill_max_value / 4, skill_max_value);
		m_skills[i] = skill_value;
		m_skills_max[i] = skill_max_value;;
	}

	//race
	if (race != NB_CREW_RACES)
	{
		m_race = race;
	}
	else
	{
		int r = RandomizeIntBetweenValues(0, NB_CREW_RACES - 1);
		m_race = (CrewMemberRace)r;
	}

	//m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	//m_text.setCharacterSize(20);
	//m_text.setColor(sf::Color::White);
	//m_text.setString(dico_crew[type]);

	m_lifebar = new GameEntity(UI_None);
	m_lifebar->m_shape_container.setSize(sf::Vector2f(LIFEBAR_SIZE_X, LIFEBAR_SIZE_Y));
	m_lifebar->m_shape_container.setOrigin(sf::Vector2f(LIFEBAR_SIZE_X * 0.5f, LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_lifebar->m_shape_container.setOutlineThickness(1.f);
	m_lifebar->m_shape_container.setOutlineColor(sf::Color::Black);

	m_lifebar->m_shape.setSize(sf::Vector2f(LIFEBAR_SIZE_X, LIFEBAR_SIZE_Y));
	m_lifebar->m_shape.setOrigin(sf::Vector2f(LIFEBAR_SIZE_X * 0.5f, LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape.setFillColor(sf::Color::Green);

	//randomization of gender and name
	int gender = RandomizeIntBetweenValues(0, 1);
	m_display_name = (*CurrentGame).GetRandomCrewMemberName(gender);
}

CrewMember::~CrewMember()
{
	//free tile (if any)
	if (m_tile != NULL && m_tile->m_crew == this)
	{
		m_tile->m_crew = NULL;
	}

	if (m_destination != NULL && m_destination->m_crew == this)
	{
		m_destination->m_crew = NULL;
	}

	delete m_lifebar;
}

void CrewMember::MoveToRoomTile(RoomTile* tile)
{
	//pathfind
	FindShortestPath(m_tile, tile);
}

void CrewMember::Update(Time deltaTime)
{
	if ((*CurrentGame).m_pause == false)
	{
		float speed = m_tile->m_flood == 0 ? m_ref_speed : m_ref_speed * CREWMEMBER_SPEED_FLOOD_FACTOR;
		float speed2 = m_is_prisoner == false ? m_ref_speed : m_ref_speed * CREWMEMBER_SPEED_PRISONER_FACTOR;
		speed = Minf(speed, speed2);

		//update cooldowns
		if (m_repair_timer > 0)
		{
			m_repair_timer -= deltaTime.asSeconds();
		}
		if (m_drowning_timer > 0)
		{
			m_drowning_timer -= deltaTime.asSeconds();
		}
		if (m_healing_timer > 0)
		{
			m_healing_timer -= deltaTime.asSeconds();
		}

		//get new move order
		if (m_current_path.empty() == true && m_destination != NULL)
		{
			//search pathfind
			if (FindShortestPath(m_tile, m_destination) == true)
			{
				//free departure tile
				m_tile->m_crew = NULL;
				m_repair_timer = HULL_REPAIR_TIMER;
			}
			else
			{
				//no path found => cancel
				m_destination->m_crew = NULL;
				m_destination = NULL;
				m_speed = sf::Vector2f(0, 0);
			}
		}
		//order changed
		else if (m_current_path.empty() == false && m_destination != NULL && m_destination != m_current_path.front())
		{
			FindShortestPath(m_tile, m_destination);
		}

		//arrived at waypoint? get next waypoint
		if (m_current_path.empty() == false)
		{
			RoomTile* waypoint = m_current_path.back();
			sf::Vector2f vec = waypoint->m_position - m_position;

			//arrived at waypoint?
			if (vec.x * vec.x + vec.y * vec.y < speed * deltaTime.asSeconds() * 8.f)
			{
				//update his tile reference
				m_tile = waypoint;
				m_repair_timer = HULL_REPAIR_TIMER;

				//set next waypoint
				m_current_path.pop_back();

				//arrived at final destination
				if (m_current_path.empty() == true)
				{
					vec = sf::Vector2f(0, 0);
					m_position = waypoint->m_position;
					m_destination = NULL;
				}
				//set speed vector to next waypoint
				else
				{
					waypoint = m_current_path.back();

					//check if waypoint is stil accessible (door locked?)
					if (waypoint->m_connexion != NULL && (waypoint->m_connexion->m_tiles.first == m_tile || waypoint->m_connexion->m_tiles.second == m_tile) && waypoint->m_connexion->m_locked == true)
					{
						//cancel destination, stay where we are
						vec = sf::Vector2f(0, 0);
						m_position = m_tile->m_position;
						m_destination->m_crew = NULL;
						m_destination = NULL;
						m_current_path.clear();
					}
					else
					{
						//clean 90° turns with a snap on one axis
						if (m_tile->m_coord_x != waypoint->m_coord_x && m_speed.y != 0)
						{
							m_position.y = m_tile->m_position.y;
						}
						else if (m_tile->m_coord_y != waypoint->m_coord_y && m_speed.x != 0)
						{
							m_position.x = m_tile->m_position.x;
						}

						vec = waypoint->m_position - m_position;
					}
				}
			}

			//set speed to waypoint
			ScaleVector(&vec, speed);
			m_speed = vec;
		}

		//apply movement
		m_position.x += m_speed.x * deltaTime.asSeconds();
		m_position.y += m_speed.y * deltaTime.asSeconds();

		//repair hull
		if (m_tile != NULL && m_tile->m_pierced == true)
		{
			if (m_repair_timer <= 0)
			{
				for (int i = 0; i < 2; i++)
				{
					if (m_tile->m_health < m_tile->m_health_max)
					{
						m_tile->m_health++;
					}
				}

				m_repair_timer = HULL_REPAIR_TIMER;
			}

			if (m_tile->m_health == m_tile->m_health_max)
			{
				m_tile->m_pierced = false;
			}
		}

		//drowning
		if (m_tile->m_room->m_is_flooded == true && m_drowning_timer <= 0)
		{
			m_drowning_timer = DROWNING_TIMER;

			m_health--;
		}

		//healing in Crew quarter
		if (m_tile->m_room->m_type == Room_Crewquarter && m_healing_timer <= 0 && m_health < m_health_max)
		{
			m_healing_timer = HEALING_TIMER;

			m_health++;
		}
	}

	GameEntity::Update(deltaTime);

	//UI lifebar
	sf::Vector2f position = m_position + m_ship_offset;
	m_lifebar->m_shape_container.setPosition(position.x, position.y - m_size.y * 0.5f - LIFEBAR_OFFSET_Y);
	m_lifebar->m_shape.setPosition(position.x, position.y - m_size.y * 0.5f - LIFEBAR_OFFSET_Y);

	int health = m_health;
	Bound(health, sf::Vector2i(0, m_health_max));

	float life_ratio = 1.0f * health / m_health_max;

	m_lifebar->m_shape.setSize(sf::Vector2f(life_ratio * LIFEBAR_SIZE_X, LIFEBAR_SIZE_Y));

	float threshold[3] = { 0.7, 0.5, 0.3 };
	if (life_ratio >= threshold[1])
	{
		m_lifebar->m_shape.setFillColor(sf::Color::Green);
	}
	else if (life_ratio >= threshold[2])
	{
		m_lifebar->m_shape.setFillColor(sf::Color(255, 127, 39, 255));//orange "damaged"
	}
	else
	{
		m_lifebar->m_shape.setFillColor(sf::Color::Red);
	}
}

void CrewMember::IteratePathFinding(RoomTile* tileA, RoomTile* tileB)
{
	m_closed_list_pathfind.push_back(tileA);
	m_open_list_pathfind.remove(tileA);

	const int tileA_x = tileA->m_coord_x;
	const int tileA_y = tileA->m_coord_y;
	const int tileB_x = tileB->m_coord_x;
	const int tileB_y = tileB->m_coord_y;

	//are we arrived at destination? (1 tile away from destination)
	if (Room::IsConnectedToRoomTile(tileA, tileB))
	{
		tileB->m_parent = tileA;
		return;
	}
	
	//looks through all tiles to find the best next waypoint.
	size_t vector_size = (*CurrentGame).m_tiles.size();
	for (size_t i = 0; i < vector_size; i++)
	{
		if (Room::IsConnectedToRoomTile(tileA, (*CurrentGame).m_tiles[i]) && (*CurrentGame).m_tiles[i]->m_system == NULL)//tiles that are not blocking (tile systems are blocking pathfind)
		{
			//tiles that are legitimate to compute	
			if (find(m_closed_list_pathfind.begin(), m_closed_list_pathfind.end(), (*CurrentGame).m_tiles[i]) == m_closed_list_pathfind.end())//tile unknown until now
			{
				if (find(m_open_list_pathfind.begin(), m_open_list_pathfind.end(), (*CurrentGame).m_tiles[i]) == m_open_list_pathfind.end())
				{
					//CASE where the tile is not on the closed list nor on the open list
					m_open_list_pathfind.push_back((*CurrentGame).m_tiles[i]);

					//compute Heuristic value (distance between the computed tile and the target) - we avoid using square root here
					const int pos2_x = tileB->m_coord_x;
					const int pos2_y = tileB->m_coord_y;
					const int posit_x = tileB->m_coord_x;
					const int posit_y = tileB->m_coord_y;

					int H_value_x = posit_x > pos2_x ? posit_x - pos2_x : pos2_x - posit_x;
					int H_value_y = posit_y > pos2_y ? posit_y - pos2_y : pos2_y - posit_y;
					(*CurrentGame).m_tiles[i]->m_heuristic = H_value_x + H_value_y;

					//compute Movement cost
					(*CurrentGame).m_tiles[i]->m_movement_cost = 10;
					(*CurrentGame).m_tiles[i]->m_movement_cost += tileA->m_movement_cost;

					//G value
					(*CurrentGame).m_tiles[i]->m_G_value = (*CurrentGame).m_tiles[i]->m_heuristic + (*CurrentGame).m_tiles[i]->m_movement_cost;

					//parent node
					(*CurrentGame).m_tiles[i]->m_parent = tileA;
				}
			}
		}
	}
}

void CrewMember::IterateEnemyPathFinding(RoomTile* tileA, RoomTile* tileB)
{
	m_closed_list_pathfind.push_back(tileA);
	m_open_list_pathfind.remove(tileA);

	const int tileA_x = tileA->m_coord_x;
	const int tileA_y = tileA->m_coord_y;
	const int tileB_x = tileB->m_coord_x;
	const int tileB_y = tileB->m_coord_y;

	//are we arrived at destination? (1 tile away from destination)
	if (Room::IsConnectedToRoomTile(tileA, tileB))
	{
		tileB->m_parent = tileA;
		return;
	}

	//looks through all tiles to find the best next waypoint.
	size_t vector_size = (*CurrentGame).m_enemy_tiles.size();
	for (size_t i = 0; i < vector_size; i++)
	{
		if (Room::IsConnectedToRoomTile(tileA, (*CurrentGame).m_enemy_tiles[i]) && (*CurrentGame).m_enemy_tiles[i]->m_system == NULL)//tiles that are not blocking (tile systems are blocking pathfind))
		{
			//tiles that are legitimate to compute	
			if (find(m_closed_list_pathfind.begin(), m_closed_list_pathfind.end(), (*CurrentGame).m_enemy_tiles[i]) == m_closed_list_pathfind.end())//tile unknown until now
			{
				if (find(m_open_list_pathfind.begin(), m_open_list_pathfind.end(), (*CurrentGame).m_enemy_tiles[i]) == m_open_list_pathfind.end())
				{
					//CASE where the tile is not on the closed list nor on the open list
					m_open_list_pathfind.push_back((*CurrentGame).m_enemy_tiles[i]);

					//compute Heuristic value (distance between the computed tile and the target) - we avoid using square root here
					const int pos2_x = tileB->m_coord_x;
					const int pos2_y = tileB->m_coord_y;
					const int posit_x = tileB->m_coord_x;
					const int posit_y = tileB->m_coord_y;

					int H_value_x = posit_x > pos2_x ? posit_x - pos2_x : pos2_x - posit_x;
					int H_value_y = posit_y > pos2_y ? posit_y - pos2_y : pos2_y - posit_y;
					(*CurrentGame).m_enemy_tiles[i]->m_heuristic = H_value_x + H_value_y;

					//compute Movement cost
					(*CurrentGame).m_enemy_tiles[i]->m_movement_cost = 10;
					(*CurrentGame).m_enemy_tiles[i]->m_movement_cost += tileA->m_movement_cost;

					//G value
					(*CurrentGame).m_enemy_tiles[i]->m_G_value = (*CurrentGame).m_enemy_tiles[i]->m_heuristic + (*CurrentGame).m_enemy_tiles[i]->m_movement_cost;

					//parent node
					(*CurrentGame).m_enemy_tiles[i]->m_parent = tileA;
				}
			}
		}
	}
}

bool CrewMember::FindShortestPath(RoomTile* tileA, RoomTile* tileB)
{
	if (tileA == tileB || tileB->m_system != NULL)
	{
		return false;
	}

	//start
	m_open_list_pathfind.push_back(tileA);
	while (m_open_list_pathfind.empty() == false && tileB->m_parent == NULL)
	{
		//choose next best tile to compute
		size_t min_G_value = 0;
		RoomTile* next_tile = NULL;
		for (list<RoomTile*>::iterator it = m_open_list_pathfind.begin(); it != m_open_list_pathfind.end(); it++)
		{
			if ((*it)->m_G_value < min_G_value || min_G_value == 0)
			{
				min_G_value = (*it)->m_G_value;
				next_tile = *it;
			}
		}

		//compute this tile
		if (m_alliance == Alliance_Player)
		{
			IteratePathFinding(next_tile, tileB);
		}
		else
		{
			IterateEnemyPathFinding(next_tile, tileB);
		}
	}

	//path found -> save all waypoints into a member path
	vector<RoomTile*>::iterator begin = m_alliance == Alliance_Player ? (*CurrentGame).m_tiles.begin() : (*CurrentGame).m_enemy_tiles.begin();
	vector<RoomTile*>::iterator end = m_alliance == Alliance_Player ? (*CurrentGame).m_tiles.end() : (*CurrentGame).m_enemy_tiles.end();
	
	m_current_path.clear();
	RoomTile* way_point = tileB;
	while (way_point != tileA)
	{
		m_current_path.push_back(way_point);
		//path not existing?
		if (way_point->m_parent == NULL)
		{
			//clear data

			for (vector<RoomTile*>::iterator it = begin; it != end; it++)
			{
				(*it)->m_heuristic = 0;
				(*it)->m_movement_cost = 0;
				(*it)->m_G_value = 0;
				(*it)->m_parent = NULL;
			}
			m_open_list_pathfind.clear();
			m_closed_list_pathfind.clear();
			m_current_path.clear();
			return false;
		}
		way_point = way_point->m_parent;	
	}
	
	//clear data
	for (vector<RoomTile*>::iterator it = begin; it != end; it++)
	{
		(*it)->m_heuristic = 0;
		(*it)->m_movement_cost = 0;
		(*it)->m_G_value = 0;
		(*it)->m_parent = NULL;
	}
	m_open_list_pathfind.clear();
	m_closed_list_pathfind.clear();

	//compute the best diagonals and eliminate useless waypoints
	//int path_size = temp_path.size();
	//int index = 0;
	//while (index < path_size)
	//{
	//	if (index == 0 || temp_path[index - 1]->m_room != temp_path[index]->m_room || (index != path_size - 1 && temp_path[index + 1]->m_room != temp_path[index]->m_room))
	//	{
	//		m_current_path.push_back(temp_path[index]);
	//	}
	//	
	//	index++;
	//}

	return true;
}

RoomTile* CrewMember::GetFreeRoomTile(Room* room)
{
	for (vector<RoomTile*>::iterator it = room->m_tiles.begin(); it != room->m_tiles.end(); it++)
	{
		if (((*it)->m_crew == NULL && (*it)->m_weapon == NULL) || (*it)->m_crew == this)
		{
			return *it;
		}
	}

	return NULL;
}

bool CrewMember::Imprison(RoomTile* prison_cell)
{
	if (prison_cell->m_crew != NULL)
	{
		//printf("Prison cell already full");
		return false;
	}

	m_is_prisoner = true;
	m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Yellow_Prisoner]);

	prison_cell->m_crew = this;
	m_tile = prison_cell;
	m_position = prison_cell->m_position;
	m_speed = sf::Vector2f(0, 0);
}