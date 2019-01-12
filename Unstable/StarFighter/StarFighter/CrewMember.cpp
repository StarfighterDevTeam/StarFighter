#include "CrewMember.h"

extern Game* CurrentGame;

string dico_crew[NB_CREW_TYPES] = {
	"All",			//Crew_All,
	"Pirate",	//Crew_Pirate,
	"Civilian",	//Crew_Civilian,
	"Slave",	//Crew_Slave,
	"Mecha",	//Crew_Mecha,
};

CrewMember::CrewMember(CrewMemberType type) : GameEntity(UI_CrewMember)
{
	m_type = type;
	m_UI_type = UI_CrewMember;
	m_speed_max = CREWMEMBER_SPEED;
	m_speed = sf::Vector2f(0, 0);
	m_size = sf::Vector2f(CREWMEMBER_SIZE, CREWMEMBER_SIZE);
	m_pathfind_cooldown_timer = 0.f;
	m_destination = NULL;

	//UI
	m_shape_container.setSize(sf::Vector2f(m_size));
	m_shape_container.setOrigin(sf::Vector2f(m_size.x * 0.5f, m_size.y * 0.5f));
	m_shape_container.setOutlineThickness(1.f);
	m_shape_container.setOutlineColor(sf::Color::White);

	switch (type)
	{
		case Crew_Pirate:
		{
			m_shape_container.setFillColor(sf::Color::Red);
			break;
		}
		case Crew_Civilian:
		{
			m_shape_container.setFillColor(sf::Color::Cyan);
			break;
		}
		case Crew_Slave:
		{
			m_shape_container.setFillColor(sf::Color::Magenta);
			break;
		}
		case Crew_Mecha:
		{
			m_shape_container.setFillColor(sf::Color::Green);
			break;
		}
	}

	//m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	//m_text.setCharacterSize(20);
	//m_text.setColor(sf::Color::White);
	//m_text.setString(dico_crew[type]);
}

CrewMember::~CrewMember()
{
	
}

bool CrewMember::MoveToRoom(Room* room)
{
	if (room == m_tile->m_room)
	{
		return false;//already in that room
	}

	//look for a free tile
	RoomTile* tile = GetFreeRoomTile(room);

	if (tile == NULL)
	{
		return false;
	}
	else
	{
		//destination changed?
		if (m_destination != NULL && tile != m_destination)
		{
			//...free the previous booking
			m_destination->m_crew = NULL;
		}

		//assign tile
		tile->m_crew = this;
		m_destination = tile;

		//search pathfind (this will update m_current_path)
		MoveToRoomTile(tile);
		return true;
	}
}

void CrewMember::MoveToRoomTile(RoomTile* tile)
{
	//pathfind
	FindShortestPath(m_tile, tile);
}

void CrewMember::Update(Time deltaTime)
{
	//update cooldown timer
	if (m_pathfind_cooldown_timer > 0)
	{
		m_pathfind_cooldown_timer -= deltaTime.asSeconds();
	}

	//get new move order
	if (m_current_path.empty() == true && m_destination != NULL)
	{
		//free departure tile
		m_tile->m_crew = NULL;

		//search pathfind
		MoveToRoom(m_destination->m_room);
		m_pathfind_cooldown_timer = CREWMEMBER_ROUTE_REFRESH_TIMER;
	}
	//order changed
	else if (m_current_path.empty() == false && m_destination != NULL && m_destination != m_current_path.front())
	{
		//refresh order (only after a given cooldown)
		if (m_pathfind_cooldown_timer > CREWMEMBER_MOVEORDER_COOLDOWN_TIMER)
		{
			m_pathfind_cooldown_timer = CREWMEMBER_MOVEORDER_COOLDOWN_TIMER;
		}
		if (m_pathfind_cooldown_timer <= 0)
		{
			MoveToRoom(m_destination->m_room);
			m_pathfind_cooldown_timer = CREWMEMBER_ROUTE_REFRESH_TIMER;
		}
	}
		
	//arrived at waypoint? get next waypoint
	if (m_current_path.empty() == false)
	{
		//refresh route (every X seconds) <!> this will possibly change m_destination
		if (m_pathfind_cooldown_timer <= 0)
		{
			MoveToRoom(m_destination->m_room);
			m_pathfind_cooldown_timer = CREWMEMBER_ROUTE_REFRESH_TIMER;
		}

		RoomTile* waypoint = m_current_path.back();
		sf::Vector2f vec = waypoint->m_position - m_position;
		
		//arrived at waypoint?
		if (vec.x * vec.x + vec.y * vec.y < 8.f)
		{
			m_tile = waypoint;
			m_current_path.pop_back();

			//arrived at final destination
			if (m_current_path.empty() == true)
			{
				vec = sf::Vector2f(0, 0);
				m_position = waypoint->m_position;
				m_destination = NULL;
			}
		}

		//set speed to waypoint
		ScaleVector(&vec, m_speed_max);
		m_speed = vec;
	}

	//apply movement
	m_position.x += m_speed.x * deltaTime.asSeconds();
	m_position.y += m_speed.y * deltaTime.asSeconds();

	GameEntity::Update(deltaTime);
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
		if (Room::IsConnectedToRoomTile(tileA, (*CurrentGame).m_tiles[i]))
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

void CrewMember::FindShortestPath(RoomTile* tileA, RoomTile* tileB)
{
	if (tileA == tileB)
	{
		return;
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
		IteratePathFinding(next_tile, tileB);
	}

	//path found -> save all waypoints into a member path
	m_current_path.clear();
	vector<RoomTile*> temp_path;
	RoomTile* way_point = tileB;
	while (way_point != tileA)
	{
		temp_path.push_back(way_point);
		way_point = way_point->m_parent;
	}
	
	//clear data
	for (vector<RoomTile*>::iterator it = (*CurrentGame).m_tiles.begin(); it != (*CurrentGame).m_tiles.end(); it++)
	{
		(*it)->m_heuristic = 0;
		(*it)->m_movement_cost = 0;
		(*it)->m_G_value = 0;
		(*it)->m_parent = NULL;
	}
	m_open_list_pathfind.clear();
	m_closed_list_pathfind.clear();

	//compute the best diagonals and eliminate useless waypoints
	int path_size = temp_path.size();
	int index = 0;
	while (index < path_size)
	{
		if (index == 0 || temp_path[index - 1]->m_room != temp_path[index]->m_room || (index != path_size - 1 && temp_path[index + 1]->m_room != temp_path[index]->m_room))
		{
			m_current_path.push_back(temp_path[index]);
		}
		
		index++;
	}
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