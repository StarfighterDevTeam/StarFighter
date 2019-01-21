#include "Ship.h"

extern Game* CurrentGame;

Ship::Ship(DMS_Coord coord, ShipType type, bool is_player) : GameEntity(UI_EnemyShip)
{
	m_type = type;
	m_destination = NULL;
	m_seaport = NULL;
	m_is_minimized = true;
	m_distance_combat = DISTANCE_COMBAT_INIT;

	m_rooms_min_upcorner_x = 0;
	m_rooms_min_upcorner_y = 0;
	m_rooms_max_offset_x = 0;
	m_rooms_max_offset_y = 0;

	m_health_max = 0;
	m_flood_max = 0;
	m_flood = 0;

	//get on tile
	SetDMSCoord(coord);

	//shape for water tiles
	m_textureName = "2D/enemy_icon.png";
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(m_textureName, (int)WATERTILE_SIZE, (int)WATERTILE_SIZE * 2);

	setAnimation(texture, 1, 2);

	m_can_be_seen = false;

	m_speed = sf::Vector2f(0, 0);
	m_angle = -90;

	if (is_player == false)
	{
		BuildShip();
	}
}

Ship::~Ship()
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

	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		delete *it;
	}
}

WaterTile* Ship::GetWaterTileAtDMSCoord(DMS_Coord coord)
{
	WaterTile* tile = (*CurrentGame).m_waterzones[coord.m_degree_x][coord.m_degree_y]->m_watertiles[coord.m_minute_x][coord.m_minute_y];

	return tile;
}

bool Ship::SetDMSCoord(DMS_Coord coord)
{
	WaterTile* tile = Ship::GetWaterTileAtDMSCoord(coord);

	m_DMS = coord;
	m_tile = tile;

	//new seaport?
	if (tile->m_seaport != NULL && tile != (WaterTile*)m_seaport)
	{
		m_seaport = tile->m_seaport;
		m_seaport->m_ships.push_back(this);
	}

	return true;
}

void Ship::UpdatePosition(DMS_Coord warship_DMS)
{
	m_position.x = WATERTILE_OFFSET_X + WATERTILE_SIZE * (0.5f - (warship_DMS.m_minute_x + warship_DMS.m_second_x / 60) + NB_WATERTILE_VIEW_RANGE + (m_DMS.m_minute_x + m_DMS.m_second_x / 60));
	m_position.y = WATERTILE_OFFSET_Y + WATERTILE_SIZE * (0.5f - (NB_WATERTILE_Y - warship_DMS.m_minute_y - warship_DMS.m_second_y / 60) + NB_WATERTILE_VIEW_RANGE + NB_WATERTILE_Y - (m_DMS.m_minute_y + m_DMS.m_second_y / 60));//from bottom to top

	GameEntity::UpdatePosition();
}

void Ship::Update(Time deltaTime, DMS_Coord warship_DMS, bool tactical_combat)
{
	//update tile information
	m_tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x][m_DMS.m_minute_y];

	m_can_be_seen = m_tile->m_can_be_seen;

	UpdatePosition(warship_DMS);
	UpdateAnimation();

	UpdateFlooding(deltaTime, true);
}

void Ship::UpdateAnimation()
{
	if (m_angle < 0.f)
	{
		m_angle += 360.f;
	}
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

void Ship::GetAngleForSpeed(float& angle)
{
	//find angle for speed vector
	if (m_speed.x != 0 || m_speed.y != 0)
	{
		if (m_speed.x == 0)
		{
			angle = m_speed.y >= 0 ? 180.f : 0.f;
		}
		else if (m_speed.y == 0)
		{
			angle = m_speed.x >= 0 ? 90.f : 270.f;
		}
		else
		{
			if (m_speed.x >= 0)
			{
				angle = (atan(m_speed.y / m_speed.x) * 180.f / M_PI) + 90.f;
			}
			else
			{
				angle = (atan(m_speed.y / m_speed.x) * 180.f / M_PI) + 90.f + 180.f;
			}
		}
	}
	else
	{
		//default value
		angle = m_currentAnimationIndex == 0 ? 90.f : 270.f;
	}
}


Room* Ship::AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type, bool minimized)
{
	Room* room = new Room(upcorner_x, upcorner_y, width, height, type, minimized);
	m_rooms.push_back(room);

	if (upcorner_x < m_rooms_min_upcorner_x)
	{
		m_rooms_min_upcorner_x = upcorner_x;
	}
	if (upcorner_y < m_rooms_min_upcorner_y)
	{
		m_rooms_min_upcorner_y = upcorner_y;
	}
	if (upcorner_x + width - 1 > m_rooms_max_offset_x)
	{
		m_rooms_max_offset_x = upcorner_x + width - 1;
	}
	if (upcorner_y + height - 1 > m_rooms_max_offset_y)
	{
		m_rooms_max_offset_y = upcorner_y + height - 1;
	}

	m_rooms_size.x = m_rooms_max_offset_x - m_rooms_min_upcorner_x + 1;
	m_rooms_size.y = m_rooms_max_offset_y - m_rooms_min_upcorner_y + 1;

	m_health_max += room->m_tiles.size() * ROOMTILE_HEALTH_MAX;
	m_flood_max += room->m_tiles.size();

	return room;
}

Room* Ship::AddRoomMinimized(int upcorner_x, int upcorner_y, int width, int height, RoomType type)
{
	return AddRoom(upcorner_x, upcorner_y, width, height, type, true);
}

CrewMember* Ship::AddCrewMember(CrewMember* crew, Room* room)
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

	m_nb_crew_max++;

	//UI
	crew->m_shape_container.setPosition(crew->m_position);
	crew->m_text.SetPosition(crew->m_position);

	return crew;
}

bool Ship::AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y)
{
	//Checking connexion requirements
	bool can_be_connected = (abs(tileA_x - tileB_x) == 1 && abs(tileA_y - tileB_y) == 0) || (abs(tileA_x - tileB_x) == 0 && abs(tileA_y - tileB_y) == 1);

	if (can_be_connected == false)
	{
		return false;
	}

	RoomTile* tileA = RoomTile::GetRoomTileAtCoord(tileA_x, tileA_y, m_is_minimized);
	RoomTile* tileB = RoomTile::GetRoomTileAtCoord(tileB_x, tileB_y, m_is_minimized);

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
	RoomConnexion* connexion = new RoomConnexion(pair<RoomTile*, RoomTile*>(tileA, tileB), false, m_is_minimized, this);

	m_connexions.push_back(connexion);
	tileA->m_room->m_connexions.push_back(connexion);
	tileB->m_room->m_connexions.push_back(connexion);

	tileA->m_connexion = connexion;
	tileB->m_connexion = connexion;

	return true;
}

void Ship::FlagHullRoomTiles()
{
	//look out for room tiles adjacent to holes
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			(*it2)->m_hull = Hull_None;

			int x = (*it2)->m_coord_x;
			int y = (*it2)->m_coord_y;

			if (x == 0 || m_tiles[x - 1][y] == NULL)
			{
				(*it2)->m_hull = Hull_Left;
			}
			else if (x == m_rooms_size.x - 1 || m_tiles[x + 1][y] == NULL)
			{
				(*it2)->m_hull = Hull_Right;
			}
			else if (y == 0 || m_tiles[x][y - 1] == NULL)
			{
				(*it2)->m_hull = Hull_Up;
			}
			else if (y == m_rooms_size.y - 1 || m_tiles[x][y + 1] == NULL)
			{
				(*it2)->m_hull = Hull_Down;
			}
		}
	}
}

Room* Ship::ConnectRooms()
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

	//everything ok if this pointer is still NULL. Otherwise, it points out to the unlinked room
	return unconnected_room;
}

Weapon* Ship::AddWeapon(Weapon* weapon, Room* room, Ship* ship)
{
	if (room == NULL)
	{
		return NULL;
	}

	RoomTile* tile = weapon->GetFreeWeaponTile(room);

	if (tile == NULL)
	{
		return NULL;
	}

	//position
	weapon->m_position = tile->m_position;

	//add to crew lists
	m_weapons.push_back(weapon);

	//assign crew to tile
	weapon->m_tile = tile;
	tile->m_weapon = weapon;

	//save owner ship
	weapon->m_ship = this;

	//gunner tile
	if (tile->m_hull == Hull_Left && m_tiles[tile->m_coord_x + 1][tile->m_coord_y] != NULL)
	{
		tile->m_weapon_gunner = m_tiles[tile->m_coord_x + 1][tile->m_coord_y];
	}
	else if (tile->m_hull == Hull_Right && m_tiles[tile->m_coord_x - 1][tile->m_coord_y] != NULL)
	{
		tile->m_weapon_gunner = m_tiles[tile->m_coord_x - 1][tile->m_coord_y];
	}
	else if (tile->m_hull == Hull_Up && m_tiles[tile->m_coord_x][tile->m_coord_y + 1] != NULL)
	{
		tile->m_weapon_gunner = m_tiles[tile->m_coord_x][tile->m_coord_y + 1];
	}
	else if (tile->m_hull == Hull_Down && m_tiles[tile->m_coord_x][tile->m_coord_y - 1] != NULL)
	{
		tile->m_weapon_gunner = m_tiles[tile->m_coord_x][tile->m_coord_y - 1];
	}

	//UI
	weapon->m_shape_container.setPosition(weapon->m_position);
	weapon->m_text.SetPosition(weapon->m_position);

	return weapon;
}

bool Ship::FireWeapon(Weapon* weapon, Time deltaTime, Ship* target)
{
	if (0)//chance of miss
	{
		return false;
	}

	int r = RandomizeIntBetweenValues(0, target->m_rooms.size() - 1);
	Room* target_room = target->m_rooms[r];
	int t = RandomizeIntBetweenValues(0, target->m_rooms[r]->m_tiles.size() - 1);
	RoomTile* target_tile = target->m_rooms[r]->m_tiles[t];
	
	return weapon->Fire(deltaTime, m_position, m_angle, target->m_distance_combat, target, target_tile);
}

void Ship::BuildShip()
{
	//ROOMS
	//left
	AddRoomMinimized(0, 3, 4, 4, Room_Weapon);
	AddRoomMinimized(0, 7, 4, 6, Room_Gold);
	AddRoomMinimized(0, 13, 4, 6, Room_Relic);
	AddRoomMinimized(0, 19, 4, 3, Room_Sword);

	//mid
	AddRoomMinimized(6, 0, 4, 3, Room_Weapon);
	AddRoomMinimized(4, 3, 8, 6, Room_Navigation);
	AddRoomMinimized(4, 9, 8, 10, Room_Crewquarter);
	AddRoomMinimized(4, 19, 8, 3, Room_Ammo);
	AddRoomMinimized(5, 22, 6, 3, Room_Engine);

	//right
	AddRoomMinimized(12, 3, 4, 4, Room_Weapon);
	AddRoomMinimized(12, 7, 4, 6, Room_Fishing);
	AddRoomMinimized(12, 13, 4, 6, Room_Kitchen);
	AddRoomMinimized(12, 19, 4, 3, Room_Lifeboat);

	m_health = m_health_max;

	//center position of each room & room tiles
	CenterRoomPositions(m_is_minimized);

	//doors
	ConnectRooms();

	//hull
	FlagHullRoomTiles();

	//crew
	m_nb_crew_max = 0;
	for (int i = 0; i < 10; i++)
	{
		int r = RandomizeIntBetweenValues(0, m_rooms.size() - 1);
		CrewMember* crew = new CrewMember(Crew_Civilian);
		AddCrewMember(crew, m_rooms[r]);
	}

	m_nb_crew = m_nb_crew_max;
}

void Ship::CenterRoomPositions(bool minimized)
{
	float size = minimized == false ? ROOMTILE_SIZE : ROOMTILE_MINI_SIZE;
	float room_offset_x = minimized == false ? ROOMTILE_OFFSET_X : ROOMTILE_MINI_OFFSET_X;
	float room_offset_y = minimized == false ? ROOMTILE_OFFSET_Y : ROOMTILE_MINI_OFFSET_Y;
	float tile_offset_x = minimized == false ? ROOMTILE_OFFSET_X : ROOMTILE_MINI_OFFSET_X;
	float tile_offset_y = minimized == false ? ROOMTILE_OFFSET_Y : ROOMTILE_MINI_OFFSET_Y;

	room_offset_x -= 1.f * m_rooms_size.x / 2 * size;
	room_offset_y -= 1.f * m_rooms_size.y / 2 * size;

	//fill hull array with NULL pointers
	for (int i = 0; i < m_rooms_size.x; i++)
	{
		vector<RoomTile*> vec;
		for (int j = 0; j < m_rooms_size.y; j++)
		{
			RoomTile* tile = NULL;
			vec.push_back(tile);
		}
		m_tiles.push_back(vec);
	}

	//enemy (minimized) = flip the ship (180°)
	if (minimized == true)
	{
		for (vector<vector<RoomTile*> >::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
		{
			for (vector<RoomTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				*it2 = NULL;
			}
		}
	}

	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		if (minimized == true)
		{
			//flip it
			(*it)->m_upcorner_x = m_rooms_size.x - (*it)->m_upcorner_x - (*it)->m_width;
			(*it)->m_upcorner_y = m_rooms_size.y - (*it)->m_upcorner_y - (*it)->m_height;
		}
		
		(*it)->m_position.x = room_offset_x + ((*it)->m_upcorner_x + (*it)->m_width * 0.5f) * size;
		(*it)->m_position.y = room_offset_y + ((*it)->m_upcorner_y + (*it)->m_height * 0.5f) * size;
		
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			int upcorner = (*it)->m_upcorner_x;
			int coord = (*it2)->m_coord_x;

			if (minimized == true)
			{
				//flip it
				(*it2)->m_coord_x = m_rooms_size.x - 1 - (*it2)->m_coord_x;
				(*it2)->m_coord_y = m_rooms_size.y - 1 - (*it2)->m_coord_y;
			}
			
			(*it2)->m_position.x = room_offset_x + (0.5f + (*it2)->m_coord_x) * size;
			(*it2)->m_position.y = room_offset_y + (0.5f + (*it2)->m_coord_y) * size;

			(*it2)->UpdatePosition();
		}
	}

	for (vector<RoomConnexion*>::iterator it = m_connexions.begin(); it != m_connexions.end(); it++)
	{
		(*it)->m_position = sf::Vector2f(0.5f * ((*it)->m_tiles.first->m_position.x + (*it)->m_tiles.second->m_position.x), 0.5f * ((*it)->m_tiles.first->m_position.y + (*it)->m_tiles.second->m_position.y));
	}

	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		(*it)->m_position = (*it)->m_tile->m_position;
	}

	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		(*it)->m_position = (*it)->m_tile->m_position;
	}

	//fill all room tiles in the array
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			m_tiles[(*it2)->m_coord_x][(*it2)->m_coord_y] = (*it2);
		}
	}
}

void Ship::UpdateFlooding(Time deltaTime, bool is_minimized)
{
	m_flood = 0;

	for (vector<vector<RoomTile*> >::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
	{
		for (vector<RoomTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			if ((*it2) == NULL)
			{
				continue;
			}

			if ((*it2)->m_flood == 0)
			{
				continue;
			}

			//ostringstream ss;
			//ss << (*it2)->m_flood;
			//(*it2)->m_text.setString(ss.str());

			if ((*it2)->m_flooding_timer > 0)
			{
				(*it2)->m_flooding_timer -= deltaTime.asSeconds();
			}

			if ((*it2)->m_flooding_timer <= 0)
			{
				(*it2)->m_flooding_timer = FLOODING_TIMER;

				//update flooding algo
				//Flood dir:
				//0: Hull_Left,
				//1: Hull_Up,
				//2: Hull_Right,
				//3: Hull_Down,

				//1. "Generate"
				if ((*it2)->m_pierced == true)
				{
					//if hull is pierced here, open a stream of water entering, that constantly generates a flow
					(*it2)->m_flood_dir[((*it2)->m_hull + 2) % 4] = true;
					for (int i = 0; i < ROOMTILE_FLOODING_GENERATION; i++)
					{
						if ((*it2)->m_flood < ROOMTILE_FLOODING_MAX)
						{
							(*it2)->m_flood++;
						}
					}
				}

				//2. "Flow"
				int x = (*it2)->m_coord_x;
				int y = (*it2)->m_coord_y;

				if ((*it2)->m_flood >= FLOOD_MIN_VALUE_FOR_TRANSFER)
				{
					//for each flood direction (left, up, right, down)
					for (int i = 0; i < 4; i++)
					{
						RoomTile* tile = NULL;
						if ((*it2)->m_flood_dir[i] == true)
						{
							switch (i)
							{
								case 0://check on the left
									if (x > 0)
										tile = m_tiles[x - 1][y];
									break;

								case 1://check below
									if (y < m_rooms_size.y - 1)
										tile = m_tiles[x][y + 1];
									break;

								case 2://check on the right
									if (x < m_rooms_size.x - 1)
										tile = m_tiles[x + 1][y];
									break;

								case 3://check above
									if (y > 0)
										tile = m_tiles[x][y - 1];
									break;
							}

							//tile next to us that is not full of water, is in the same room or in a connected room tile (door not locked)?
							if (tile != NULL && tile->m_flood < ROOMTILE_FLOODING_MAX && (tile->m_room == (*it2)->m_room || ((*it2)->m_connexion != NULL && (*it2)->m_connexion->m_locked == false && ((*it2)->m_connexion->m_tiles.first == tile || (*it2)->m_connexion->m_tiles.second == tile))))
							{
								tile->m_flood++;//transfer
								(*it2)->m_flood--;
								tile->m_flood_dir[i] = true;//keep the same momentum direction in the receiving tile
							}
						}
					}

					//3. "Expand"
					int r = RandomizeIntBetweenValues(0, 16);
					if (r < 4)
						(*it2)->m_flood_dir[r] = true;
				}
			}

			//Update ship flood total value (1 point per tile flooded)
			m_flood++;
		}
	}
}