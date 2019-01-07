#include "Warship.h"

extern Game* CurrentGame;

Warship::Warship(DMS_Coord coord) : GameEntity(UI_Warship)
{
	m_angle = 90.f;
	m_destination = NULL;
	m_speed = sf::Vector2f(0, 0);
	m_seaport = NULL;

	//get on tile
	SetDMSCoord(coord);

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/boat_icon.png", (int)WATERTILE_SIZE, (int)WATERTILE_SIZE * 2);

	setAnimation(texture, 1, 2);

	//water tile outline
	m_default_color = sf::Color(0, 0, 0, 0);

	m_shape_container.setPosition(m_position);
	m_shape_container.setSize(sf::Vector2f(WATERTILE_SIZE, WATERTILE_SIZE));
	m_shape_container.setOrigin(sf::Vector2f(WATERTILE_SIZE * 0.5f, WATERTILE_SIZE * 0.5f));
	m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));
	m_shape_container.setOutlineThickness(2.f);
	m_shape_container.setOutlineColor(m_default_color);

	//DMS coordinates display
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(24);
	m_text.setColor(sf::Color::Black);

	//ROOMS
	//left
	AddRoom(1, 3, 3, 4, Room_Weapon);
	AddRoom(0, 7, 4, 6, Room_Gold);
	AddRoom(0, 13, 4, 6, Room_Relic);
	AddRoom(0, 19, 4, 3, Room_Sword);

	//mid
	AddRoom(6, 0, 4, 3, Room_Weapon);
	AddRoom(4, 3, 8, 6, Room_Navigation);
	AddRoom(4, 9, 8, 10, Room_Crewquarter);
	AddRoom(4, 19, 8, 3, Room_Ammo);
	AddRoom(5, 22, 6, 3, Room_Engine);

	//right
	AddRoom(12, 3, 3, 4, Room_Weapon);
	Room* room2 = AddRoom(12, 7, 4, 6, Room_Fishing);
	AddRoom(12, 13, 4, 6, Room_Kitchen);
	Room* room = AddRoom(12, 19, 4, 3, Room_Lifeboat);

	//doors
	ConnectRooms();

	room->m_connexions.front()->SetLock(true);

	//CREW
	CrewMember* crew = new CrewMember(Crew_Pirate);
	AddCrewMember(crew, room);

	CrewMember* crew2 = new CrewMember(Crew_Civilian);
	AddCrewMember(crew2, room2);
}

Warship::~Warship()
{
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		delete *it;
	}

	for (vector<RoomConnexion*>::iterator it = m_connexions.begin(); it != m_connexions.end(); it++)
	{
		delete *it;
	}

	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		delete *it;
	}
}

void Warship::Update(Time deltaTime)
{
	//travel management (needs to be refreshed? arrived?)
	if (m_current_path.empty() == false)
	{
		WaterTile* waypoint = m_current_path.back();

		//arrived at waypoint?
		if (GetDistanceSquaredInSecondsDMS(waypoint) < 1.f)
		{
			m_DMS = waypoint->m_DMS;//snap boat to position
			waypoint->UpdatePosition(m_DMS);//snap tile to boat 
			m_current_path.pop_back();

			//arrived at final destination
			if (m_current_path.empty() == true)
			{
				m_speed = sf::Vector2f(0, 0);
				m_destination = NULL;
			}
			//go to next waypoint
			else
			{
				waypoint = m_current_path.back();
				waypoint->UpdatePosition(m_DMS);//update waypoint position (because of the previous snap)
				sf::Vector2f vec = waypoint->m_position - m_position;
				GameObject::ScaleVector(&vec, CRUISE_SPEED);
				m_speed = vec;
			}
		}
	}

	//apply movement
	m_DMS.m_second_x += m_speed.x * deltaTime.asSeconds();
	m_DMS.m_second_y -= m_speed.y * deltaTime.asSeconds();

	//rotation
	UpdateRotation();

	//sexadecimal position system
	if (m_DMS.m_second_x >= NB_WATERTILE_SUBDIVISION)
	{
		int minutes = m_DMS.m_second_x / NB_WATERTILE_SUBDIVISION;
		m_DMS.m_minute_x += minutes;
		m_DMS.m_second_x -= minutes * NB_WATERTILE_SUBDIVISION;
	}
	else if (m_DMS.m_second_x < 0)
	{
		int minutes = (-m_DMS.m_second_x) / NB_WATERTILE_SUBDIVISION + 1;
		m_DMS.m_minute_x -= minutes;
		m_DMS.m_second_x += minutes * NB_WATERTILE_SUBDIVISION;
	}

	if (m_DMS.m_second_y >= NB_WATERTILE_SUBDIVISION)
	{
		int minutes = m_DMS.m_second_y / NB_WATERTILE_SUBDIVISION;
		m_DMS.m_minute_y += minutes;
		m_DMS.m_second_y -= minutes * NB_WATERTILE_SUBDIVISION;
	}
	else if (m_DMS.m_second_y < 0)
	{
		int minutes = (-m_DMS.m_second_y) / NB_WATERTILE_SUBDIVISION + 1;
		m_DMS.m_minute_y -= minutes;
		m_DMS.m_second_y += minutes * NB_WATERTILE_SUBDIVISION;
	}
	
	//update tile information
	m_tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x][m_DMS.m_minute_y];

	//UI
	ostringstream ss;
	ss << "\n\n\n";
	ss << m_DMS.m_degree_y << "°" << m_DMS.m_minute_y << "' " << (int)m_DMS.m_second_y << "\"\N";
	ss << "\n";
	ss << m_DMS.m_degree_x << "°" << m_DMS.m_minute_x << "' " << (int)m_DMS.m_second_x << "\"\E";
	m_text.setString(ss.str());

	GameEntity::Update(deltaTime);
}

void Warship::UpdateRotation()
{
	//find angle for speed vector
	if (m_speed.x != 0 || m_speed.y != 0)
	{
		if (m_speed.x == 0)
		{
			m_angle = m_speed.y >= 0 ? 180.f : 0.f;
		}
		else if (m_speed.y == 0)
		{
			m_angle = m_speed.x >= 0 ? 90.f : 270.f;
		}
		else
		{
			if (m_speed.x >= 0)
			{
				m_angle = (atan(m_speed.y / m_speed.x) * 180.f / M_PI) + 90.f;
			}
			else
			{
				m_angle = (atan(m_speed.y / m_speed.x) * 180.f / M_PI) + 90.f + 180.f;
			}
		}
	}
	else
	{
		//default value
		m_angle = m_currentAnimationIndex == 0 ? 90.f : 270.f;
	}

	//flip the sprite according to the direction
	m_angle = fmod(m_angle, 360.f);
	if (m_angle > 0.f && m_angle < 180.f)
	{
		setAnimationLine(0);//Boat facing right
	}
	else if (m_angle > 180.f)
	{
		setAnimationLine(1);//Boat facing left
	}
	setRotation(m_angle);
}

Room* Warship::AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type)
{
	Room* room = new Room(upcorner_x, upcorner_y, width, height, type);

	m_rooms.push_back(room);

	return room;
}

CrewMember* Warship::AddCrewMember(CrewMember* crew, Room* room)
{
	if (room == NULL)
	{
		return NULL;
	}

	if (room->m_nb_crew[Crew_All] >= room->m_nb_crew_max)
	{
		return NULL;
	}

	RoomTile* tile = crew->GetFreeRoomTile(room);

	if (tile == NULL)
	{
		return NULL;
	}

	//position
	crew->m_position = tile->m_position;

	//add to crew lists
	m_crew.push_back(crew);

	//assign crew to tile
	crew->m_tile = tile;
	tile->m_crew = crew;

	//UI
	crew->m_shape_container.setPosition(crew->m_position);
	crew->m_text.SetPosition(crew->m_position);

	return crew;
}

bool Warship::AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y)
{
	//Checking connexion requirements
	bool can_be_connected = (abs(tileA_x - tileB_x) == 1 && abs(tileA_y - tileB_y) == 0) || (abs(tileA_x - tileB_x) == 0 && abs(tileA_y - tileB_y) == 1);

	if (can_be_connected == false)
	{
		return false;
	}

	RoomTile* tileA = RoomTile::GetRoomTileAtCoord(tileA_x, tileA_y);
	RoomTile* tileB = RoomTile::GetRoomTileAtCoord(tileB_x, tileB_y);

	if (tileA == NULL || tileB == NULL || tileA->m_room == tileB->m_room)
	{
		return false;
	}

	//Connexion between the two rooms already existing?
	for (vector<RoomConnexion*>::iterator it = tileA->m_room->m_connexions.begin(); it != tileA->m_room->m_connexions.end(); it++)
	{
		for (vector<RoomConnexion*>::iterator it2 = tileB->m_room->m_connexions.begin(); it2 != tileB->m_room->m_connexions.end(); it2++)
		{
			if (*it == *it2)
			{
				return false;
			}
		}
	}

	//Create the connexion
	RoomConnexion* connexion = new RoomConnexion(pair<RoomTile*, RoomTile*> (tileA, tileB), false);

	m_connexions.push_back(connexion);
	tileA->m_room->m_connexions.push_back(connexion);
	tileB->m_room->m_connexions.push_back(connexion);

	return true;
}

Room* Warship::ConnectRooms()
{
	m_connexions.clear();

	Room* unconnected_room = NULL;

	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		bool found = false;
		for (vector<Room*>::iterator it2 = m_rooms.begin(); it2 != m_rooms.end(); it2++)
		{
			if (*it == *it2)
				continue;

			//adjacent rooms? (quick filter, factually wrong)
			if ((*it2)->m_upcorner_x == (*it)->m_upcorner_x + (*it)->m_width || (*it2)->m_upcorner_y == (*it)->m_upcorner_y + (*it)->m_height
				|| (*it)->m_upcorner_x == (*it2)->m_upcorner_x + (*it2)->m_width || (*it)->m_upcorner_y == (*it2)->m_upcorner_y + (*it2)->m_height)
			{
				//find connected tiles
				vector<RoomTile*> connected_tiles;

				//on the right
				if ((*it2)->m_upcorner_x == (*it)->m_upcorner_x + (*it)->m_width)
				{
					for (vector<RoomTile*>::iterator it3 = (*it)->m_tiles.begin(); it3 != (*it)->m_tiles.end(); it3++)
					{
						if ((*it3)->m_coord_x == (*it)->m_upcorner_x + (*it)->m_width - 1 && (*it3)->m_coord_y >= (*it2)->m_upcorner_y && (*it3)->m_coord_y <= (*it2)->m_upcorner_y + (*it2)->m_height)
						{
							connected_tiles.push_back(*it3);
						}
					}

					int size = connected_tiles.size() / 2;
					if (size >  0)
					{
						found = AddConnexion(connected_tiles[size]->m_coord_x, connected_tiles[size]->m_coord_y, connected_tiles[size]->m_coord_x + 1, connected_tiles[size]->m_coord_y);
					}
				}
				//on the left
				else if ((*it)->m_upcorner_x == (*it2)->m_upcorner_x + (*it2)->m_width)
				{
					for (vector<RoomTile*>::iterator it3 = (*it)->m_tiles.begin(); it3 != (*it)->m_tiles.end(); it3++)
					{
						if ((*it3)->m_coord_x == (*it)->m_upcorner_x && (*it3)->m_coord_y >= (*it2)->m_upcorner_y && (*it3)->m_coord_y <= (*it2)->m_upcorner_y + (*it2)->m_height)
						{
							connected_tiles.push_back(*it3);
						}
					}

					int size = connected_tiles.size() / 2;
					if (size > 0)
					{
						found = AddConnexion(connected_tiles[size]->m_coord_x, connected_tiles[size]->m_coord_y, connected_tiles[size]->m_coord_x - 1, connected_tiles[size]->m_coord_y);
					}
				}
				//up
				else if ((*it)->m_upcorner_y == (*it2)->m_upcorner_y + (*it2)->m_height)
				{
					for (vector<RoomTile*>::iterator it3 = (*it)->m_tiles.begin(); it3 != (*it)->m_tiles.end(); it3++)
					{
						if ((*it3)->m_coord_y == (*it)->m_upcorner_y && (*it3)->m_coord_x >= (*it2)->m_upcorner_x && (*it3)->m_coord_x <= (*it2)->m_upcorner_x + (*it2)->m_width)
						{
							connected_tiles.push_back(*it3);
						}
					}

					int size = connected_tiles.size() / 2;
					if (size > 0)
					{
						found = AddConnexion(connected_tiles[size]->m_coord_x, connected_tiles[size]->m_coord_y, connected_tiles[size]->m_coord_x, connected_tiles[size]->m_coord_y - 1);
					}
				}
				//down
				else if ((*it2)->m_upcorner_y == (*it)->m_upcorner_y + (*it)->m_height)
				{
					for (vector<RoomTile*>::iterator it3 = (*it)->m_tiles.begin(); it3 != (*it)->m_tiles.end(); it3++)
					{
						if ((*it3)->m_coord_y == (*it)->m_upcorner_y + (*it)->m_height - 1 && (*it3)->m_coord_x >= (*it2)->m_upcorner_x && (*it3)->m_coord_x <= (*it2)->m_upcorner_x + (*it2)->m_width)
						{
							connected_tiles.push_back(*it3);
						}
					}

					int size = connected_tiles.size() / 2;
					if (size > 0)
					{
						found = AddConnexion(connected_tiles[size]->m_coord_x, connected_tiles[size]->m_coord_y, connected_tiles[size]->m_coord_x, connected_tiles[size]->m_coord_y + 1);
					}
				}
			}
		}

		if (found == false)
		{
			unconnected_room = *it;
		}
	}

	//everything ok
	return unconnected_room;
}

void Warship::UpdateCrewMembersCountPerRoom(Room* room)
{
	for (int i = 0; i < NB_CREW_TYPES; i++)
	{
		room->m_nb_crew[i] = 0;
		room->m_nb_crew_working[i] = 0;
	}
		
	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		if ((*it)->m_destination != NULL && (*it)->m_destination->m_room == room)
		{
			//present
			room->m_nb_crew[(*it)->m_type]++;
			room->m_nb_crew[Crew_All]++;
		}

		if ((*it)->m_tile->m_room == room && (*it)->m_destination == NULL)
		{
			//working
			room->m_nb_crew_working[(*it)->m_type]++;
			room->m_nb_crew_working[Crew_All]++;

			//present
			room->m_nb_crew[(*it)->m_type]++;
			room->m_nb_crew[Crew_All]++;
		}
	}
}

//WATER PART
WaterTile* Warship::GetWaterTileAtDMSCoord(DMS_Coord coord)
{
	WaterTile* tile = (*CurrentGame).m_waterzones[coord.m_degree_x][coord.m_degree_y]->m_watertiles[coord.m_minute_x][coord.m_minute_y];

	return tile;
}

bool Warship::SetDMSCoord(DMS_Coord coord)
{
	WaterTile* tile = GetWaterTileAtDMSCoord(coord);

	m_DMS = coord;
	m_tile = tile;
	m_tile->UpdatePosition(coord);
	m_position = tile->m_position;
	m_zone = tile->m_zone;

	//new seaport?
	if (tile->m_seaport != NULL && tile != (WaterTile*)m_seaport)
	{
		m_seaport = tile->m_seaport;
		m_seaport->m_ships.push_back(this);
	}

	return true;
}

int Warship::GetDistanceToWaterTile(WaterTile* tile)
{
	int diff_x = tile->m_coord_x - m_DMS.m_minute_x;
	int diff_y = tile->m_coord_y - m_DMS.m_minute_y;
	float distance_f = sqrt(diff_x * diff_x + diff_y * diff_y);

	int distance_i = (int)distance_f;

	if (distance_f - distance_i > 0.10f)//custom round-up rule
	{
		distance_i++;
	}

	return distance_i;
}

DMS_Coord Warship::GetDMSCoord(sf::Vector2f position)
{
	DMS_Coord dms;
	float f_minute_x;
	float f_minute_y;

	f_minute_x = (position.x - WATERTILE_OFFSET_X - WATERTILE_SIZE * (0.5f - (this->m_DMS.m_minute_x + this->m_DMS.m_second_x / 60) + NB_WATERTILE_VIEW_RANGE)) / WATERTILE_SIZE;
	f_minute_y = (position.y - WATERTILE_OFFSET_Y - WATERTILE_SIZE * (0.5f - (NB_WATERTILE_Y - this->m_DMS.m_minute_y - this->m_DMS.m_second_y / 60) + NB_WATERTILE_VIEW_RANGE + NB_WATERTILE_Y)) / (-WATERTILE_SIZE);

	int minute_x = (int)f_minute_x;
	int minute_y = (int)f_minute_y;

	float second_x = (f_minute_x - minute_x) * NB_WATERTILE_SUBDIVISION;
	float second_y = (f_minute_y - minute_y) * NB_WATERTILE_SUBDIVISION;

	dms = { 0, minute_x, second_x, 0, minute_y, second_y };
	return dms;
}

float Warship::GetDistanceSquaredInSecondsDMS(WaterTile* tile)
{
	float xA = m_DMS.m_minute_x * NB_WATERTILE_SUBDIVISION + m_DMS.m_second_x;
	float yA = m_DMS.m_minute_y * NB_WATERTILE_SUBDIVISION + m_DMS.m_second_y;

	float xB = tile->m_DMS.m_minute_x * NB_WATERTILE_SUBDIVISION + tile->m_DMS.m_second_x;
	float yB = tile->m_DMS.m_minute_y * NB_WATERTILE_SUBDIVISION + tile->m_DMS.m_second_y;

	float distance_squared = (xA - xB) * (xA - xB) + (yA - yB) * (yA - yB);

	return distance_squared;
}

float Warship::GetDistanceFloatToWaterTile(WaterTile* tile)
{
	int diff_x = NB_WATERTILE_SUBDIVISION * (tile->m_coord_x - m_DMS.m_minute_x) - m_DMS.m_second_x;
	int diff_y = NB_WATERTILE_SUBDIVISION * (tile->m_coord_y - m_DMS.m_minute_y) - m_DMS.m_second_y;
	float distance_f = sqrt(diff_x * diff_x + diff_y * diff_y) * 1.f / NB_WATERTILE_SUBDIVISION;

	return distance_f;
}

bool Warship::CanViewWaterTile(WaterTile* tile)
{
	if (tile->m_zone != m_zone)
	{
		return false;
	}
	else if (tile->m_coord_x > m_DMS.m_minute_x + NB_WATERTILE_VIEW_RANGE + 1 || tile->m_coord_x < m_DMS.m_minute_x - NB_WATERTILE_VIEW_RANGE - 1)
	{
		return false;
	}
	else if (tile->m_coord_y > m_DMS.m_minute_y + NB_WATERTILE_VIEW_RANGE + 1 || tile->m_coord_y < m_DMS.m_minute_y - NB_WATERTILE_VIEW_RANGE - 1)
	{
		return false;
	}

	//custom round-up system for visibility distance
	float distance = GetDistanceFloatToWaterTile(tile);
	if (distance <= NB_WATERTILE_VIEW_RANGE + 0.15f || (tile->m_can_be_seen == true && m_destination != NULL && distance <= NB_WATERTILE_VIEW_RANGE + 0.99f))
	{
		//return true;
		//line of sight blocked by islands
		if (RayTracingContainsIsland(m_tile, tile) == false)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Warship::SetSailsToWaterTile(WaterTile* tile)
{
	if (tile->m_type != Water_Empty)
	{
		return false;
	}

	sf::Vector2f move_vector = tile->m_position - m_position;
	GameObject::ScaleVector(&move_vector, CRUISE_SPEED);
	m_speed = move_vector;
	
	m_destination = tile;
	
	//Find path and set speed
	FindShortestPath(m_tile, m_destination);
	WaterTile* waypoint = m_current_path.back();
	sf::Vector2f vec = waypoint->m_position - m_position;
	GameObject::ScaleVector(&vec, CRUISE_SPEED);
	m_speed = vec;
	
	//leaving port?
	if (m_seaport != NULL && m_destination != (WaterTile*)m_seaport)
	{
		m_seaport->RemoveShip(this);
		m_seaport = NULL;
	}

	//bool test = RayTracingContainsIsland(m_tile, tile);
	//if (test == true)
	//{
	//	printf("not visible\n");
	//}
	//else
	//{
	//	printf("visible\n");
	//}

	return true;
}

void Warship::FindShortestPath(WaterTile* tileA, WaterTile* tileB)
{
	if (tileA == tileB)
	{
		//left and down moves "fix" (because of the degree/minute/second metric system): we can be on the same tile (same minute) but not the same position (same seconds)
		int patch_x = 0;
		int patch_y = 0;
		if (m_DMS.m_second_x > 0)
		{
			patch_x = 1;
		}
		if (m_DMS.m_second_y > 0)
		{
			patch_y = 1;
		}

		if (patch_x > 0 || patch_y > 0)
		{
			tileA = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x + patch_x][m_DMS.m_minute_y + patch_y];
		}
	}

	//start
	m_open_list_pathfind.push_back(tileA);
	while (m_open_list_pathfind.empty() == false && tileB->m_parent == NULL)
	{
		//choose next best tile to compute
		size_t min_G_value = 0;
		WaterTile* next_tile = NULL;
		for (list<WaterTile*>::iterator it = m_open_list_pathfind.begin(); it != m_open_list_pathfind.end(); it++)
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
	vector<WaterTile*> temp_path;
	WaterTile* way_point = tileB;
	while (way_point != tileA)
	{
		temp_path.push_back(way_point);
		way_point = way_point->m_parent;
	}

	//clear data
	for (vector<WaterTile*>::iterator it = m_tiles_can_be_seen.begin(); it != m_tiles_can_be_seen.end(); it++)
	{
		(*it)->m_heuristic = 0;
		(*it)->m_movement_cost = 0;
		(*it)->m_G_value = 0;
		(*it)->m_parent = NULL;
	}

	m_open_list_pathfind.clear();
	m_closed_list_pathfind.clear();

	//compute the best diagonals and eliminate useless waypoints
	temp_path.push_back(tileA);
	int path_size = temp_path.size();

	//left and down moves "fix" (because of the degree/minute/second metric system)
	int patch_x = 0;
	int patch_y = 0;

	//patch x
	if (m_DMS.m_second_x > 0)
	{
		for (int i = path_size - 1; i >= 0; i--)
		{
			//going up or down?
			if (temp_path[i]->m_DMS.m_minute_y != tileA->m_DMS.m_minute_y)
			{
				patch_x = 1;
				break;
			}
			//going right = stop
			else if (temp_path[i]->m_DMS.m_minute_x > tileA->m_DMS.m_minute_x)
			{
				break;
			}
		}
	}

	//patch y
	if (m_DMS.m_second_y > 0)
	{
		for (int i = path_size - 1; i >= 0; i--)
		{
			//going left or right?
			if (temp_path[i]->m_DMS.m_minute_x != tileA->m_DMS.m_minute_x)
			{
				patch_y = 1;
				break;
			}
			//going up = stop
			else if (temp_path[i]->m_DMS.m_minute_y > tileA->m_DMS.m_minute_y)
			{
				break;
			}
		}
	}

	//apply fix
	if (patch_x > 0 || patch_y > 0)
	{
		WaterTile* tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x + patch_x][m_DMS.m_minute_y + patch_y];
		temp_path.push_back(tile);
	}

	path_size = temp_path.size();
	int index = 0;
	while (index < path_size)
	{
		//we don't want to add TileA to m_current_path because it's our current position already, but we need it in temp_path to compute shortcuts
		//but because of the DMS coordinates that only increment when we complete a move on the right (1 tile = 1 minute), we still have to add it when moving to the left or down
		if (index == path_size - 1)
		{
			break;
		}
	
		m_current_path.push_back(temp_path[index]);

		int temp_index = index;

		bool straight_line = true;
		for (int i = index + 1; i < path_size; i++)
		{
			if (index == path_size - 2)
			{
				break;
			}

			//waypoints in line = cannot optimize
			if (straight_line == true && (temp_path[index]->m_coord_x == temp_path[i]->m_coord_x || temp_path[index]->m_coord_y == temp_path[i]->m_coord_y))
			{
				continue;
			}
			else
			{
				straight_line = false;
			}

			bool only_water = IsOnlyWaterInsideRectangle(temp_path[index], temp_path[i]);

			//chance of optimization
			if (only_water == true)
			{
				//skip all other waypoints
				temp_index = i;
			}
		}

		//we found a shortcut? continue from this waypoint on...
		if (index != temp_index)
		{
			index = temp_index;
		}
		//...otherwise, continue with the next waypoint
		else
		{
			index++;
		}
	}
}

void Warship::IteratePathFinding(WaterTile* tileA, WaterTile* tileB)
{
	m_closed_list_pathfind.push_back(tileA);
	m_open_list_pathfind.remove(tileA);

	const int tileA_x = tileA->m_coord_x;
	const int tileA_y = tileA->m_coord_y;
	const int tileB_x = tileB->m_coord_x;
	const int tileB_y = tileB->m_coord_y;

	//are we arrived at destination? (1 tile away from destination)
	if ((abs(tileA_x - tileB_x) == 1 && abs(tileA_y - tileB_y) == 0) || (abs(tileA_x - tileB_x) == 0 && abs(tileA_y - tileB_y) == 1))
	{
		tileB->m_parent = tileA;
		return;
	}

	//looks through all tiles to find the best next waypoint.
	size_t vector_size = m_tiles_can_be_seen.size();
	for (size_t i = 0; i < vector_size; i++)
	{
		if (m_tiles_can_be_seen[i]->m_type == Water_Empty)
		{
			if ((abs(tileA_x - m_tiles_can_be_seen[i]->m_coord_x) == 1 && abs(tileA_y - m_tiles_can_be_seen[i]->m_coord_y) == 0)
				|| (abs(tileA_x - m_tiles_can_be_seen[i]->m_coord_x) == 0 && abs(tileA_y - m_tiles_can_be_seen[i]->m_coord_y) == 1))
			{
				//tiles that are legitimate to compute	
				if (find(m_closed_list_pathfind.begin(), m_closed_list_pathfind.end(), m_tiles_can_be_seen[i]) == m_closed_list_pathfind.end())//tile unknown until now
				{
					if (find(m_open_list_pathfind.begin(), m_open_list_pathfind.end(), m_tiles_can_be_seen[i]) == m_open_list_pathfind.end())
					{
						//CASE where the tile is not on the closed list nor on the open list
						m_open_list_pathfind.push_back(m_tiles_can_be_seen[i]);

						//compute Heuristic value (distance between the computed tile and the target) - we avoid using square root here
						const int pos2_x = tileB->m_coord_x;
						const int pos2_y = tileB->m_coord_y;
						const int posit_x = tileB->m_coord_x;
						const int posit_y = tileB->m_coord_y;

						int H_value_x = posit_x > pos2_x ? posit_x - pos2_x : pos2_x - posit_x;
						int H_value_y = posit_y > pos2_y ? posit_y - pos2_y : pos2_y - posit_y;
						m_tiles_can_be_seen[i]->m_heuristic = H_value_x + H_value_y;

						//compute Movement cost
						m_tiles_can_be_seen[i]->m_movement_cost = 10;
						m_tiles_can_be_seen[i]->m_movement_cost += tileA->m_movement_cost;

						//G value
						m_tiles_can_be_seen[i]->m_G_value = m_tiles_can_be_seen[i]->m_heuristic + m_tiles_can_be_seen[i]->m_movement_cost;

						//parent node
						m_tiles_can_be_seen[i]->m_parent = tileA;
					}
				}
			}
		}
	}
}

bool Warship::IsOnlyWaterInsideRectangle(WaterTile* tileA, WaterTile* tileB)
{
	int x_min = Min(tileA->m_coord_x, tileB->m_coord_x);
	int x_max = Max(tileA->m_coord_x, tileB->m_coord_x);
	int y_min = Min(tileA->m_coord_y, tileB->m_coord_y);
	int y_max = Max(tileA->m_coord_y, tileB->m_coord_y);

	bool only_water = true;
	for (int j = x_min; j <= x_max; j++)
	{
		for (int k = y_min; k <= y_max; k++)
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[j][k]->m_type != Water_Empty)
			{
				only_water = false;
				break;
			}
		}

		if (only_water == false)
		{
			break;
		}
	}

	return only_water;
}

bool Warship::RayTracingContainsIsland(WaterTile* tileA, WaterTile* tileB)
{
	if (tileA == tileB)
	{
		return false;
	}

	int coord_x = tileA->m_coord_x;
	int coord_y = tileA->m_coord_y;

	bool contains_island = false;

	//Bresenham line-drawing algorithm
	int X = abs(tileA->m_coord_x - tileB->m_coord_x);
	int Y = abs(tileA->m_coord_y - tileB->m_coord_y);
	int sum = X + Y;

	int tx = 1;
	int ty = 1;

	for (int i = 0; i < sum; i++)
	{
		//going horizontally is shorter?
		if (Y == 0 || 1.f * tx/X < 1.f * ty/Y)
		{
			tx++;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
		}//going vertically is shorter?
		else if (X == 0 || 1.f * tx / X > 1.f * ty / Y)
		{
			ty++;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
		}
		else//perfect diagonal
		{
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
			tx++;
			ty++;
			i++;
		}

		//island?
		if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty)
		{
			contains_island = true;
		}
		else
		{
			//water-island-water? = water cannot be seen
			if (contains_island == true)
			{
				return true;
			}
		}
		
	}

	return false;
}