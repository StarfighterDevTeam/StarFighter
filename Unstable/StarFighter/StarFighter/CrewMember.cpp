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

RoomTile* CrewMember::GetFreeRoomTile(Room* room)
{
	for (vector<RoomTile*>::iterator it = room->m_tiles.begin(); it != room->m_tiles.end(); it++)
	{
		if ((*it)->m_crew == NULL || (*it)->m_crew == this)
		{
			return *it;
		}
	}

	return NULL;
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
		GameObject::ScaleVector(&vec, m_speed_max);
		m_speed = vec;
	}

	//apply movement
	m_position.x += m_speed.x * deltaTime.asSeconds();
	m_position.y += m_speed.y * deltaTime.asSeconds();

	GameEntity::Update(deltaTime);
}


void CrewMember::IteratePathFindingOnIndex(RoomTile* tileA, RoomTile* tileB)
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
	//we try to reduce as soon as possible the number of tiles to iterate on, by filtering same room and connected rooms only.
	vector<Room*> search_rooms;
	search_rooms.push_back(tileA->m_room);
	for (vector<RoomConnexion*>::iterator it = tileA->m_room->m_connexions.begin(); it != tileA->m_room->m_connexions.end(); it++)
	{
		if ((*it)->m_locked == false && (*it)->m_tiles.first->m_room != tileA->m_room)
		{
			search_rooms.push_back((*it)->m_tiles.first->m_room);
		}
		else if ((*it)->m_locked == false && (*it)->m_tiles.second->m_room != tileA->m_room)
		{
			search_rooms.push_back((*it)->m_tiles.second->m_room);
		}
	}

	vector<RoomTile*> search_tiles;
	for (vector<Room*>::iterator it = search_rooms.begin(); it != search_rooms.end(); it++)
	{
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			search_tiles.push_back(*it2);
		}
	}

	for (vector<RoomTile*>::iterator it = search_tiles.begin(); it != search_tiles.end(); it++)
	{
		if (Room::IsConnectedToRoomTile(tileA, *it))
		{
			//tiles that are legitimate to compute	
			if (find(m_closed_list_pathfind.begin(), m_closed_list_pathfind.end(), *it) == m_closed_list_pathfind.end())//tile unknown until now
			{
				if (find(m_open_list_pathfind.begin(), m_open_list_pathfind.end(), *it) == m_open_list_pathfind.end())
				{
					//CASE where the tile is not on the closed list nor on the open list
					m_open_list_pathfind.push_back(*it);

					//compute Heuristic value (distance between the computed tile and the target) - we avoid using square root here
					const int pos2_x = tileB->m_coord_x;
					const int pos2_y = tileB->m_coord_y;
					const int posit_x = tileB->m_coord_x;
					const int posit_y = tileB->m_coord_y;

					int H_value_x = posit_x > pos2_x ? posit_x - pos2_x : pos2_x - posit_x;
					int H_value_y = posit_y > pos2_y ? posit_y - pos2_y : pos2_y - posit_y;
					(*it)->m_heuristic = H_value_x + H_value_y;

					//compute Movement cost
					(*it)->m_movement_cost = 10;
					(*it)->m_movement_cost += tileA->m_movement_cost;

					//G value
					(*it)->m_G_value = (*it)->m_heuristic + (*it)->m_movement_cost;

					//parent node
					(*it)->m_parent = tileA;
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
		IteratePathFindingOnIndex(next_tile, tileB);
	}

	//path found -> save all waypoints into a member path
	m_current_path.clear();
	RoomTile* way_point = tileB;
	while (way_point != tileA)
	{
		m_current_path.push_back(way_point);
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
}