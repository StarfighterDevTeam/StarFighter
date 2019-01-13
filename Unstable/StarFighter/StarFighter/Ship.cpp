#include "Ship.h"

extern Game* CurrentGame;

Ship::Ship(DMS_Coord coord, ShipType type) : GameEntity(UI_EnemyShip)
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

	//get on tile
	SetDMSCoord(coord);

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/enemy_icon.png", (int)WATERTILE_SIZE, (int)WATERTILE_SIZE * 2);

	setAnimation(texture, 1, 2);

	m_can_be_seen = false;

	m_speed = sf::Vector2f(0, 0);
	m_angle = -90;

	if (m_type == Ship_Goellete)
	{
		BuildShip(true);
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

void Ship::Update(Time deltaTime, DMS_Coord warship_DMS)
{
	//update tile information
	m_tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x][m_DMS.m_minute_y];

	m_can_be_seen = m_tile->m_can_be_seen;

	UpdatePosition(warship_DMS);
	UpdateAnimation();
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
	RoomConnexion* connexion = new RoomConnexion(pair<RoomTile*, RoomTile*>(tileA, tileB), false, m_is_minimized);

	m_connexions.push_back(connexion);
	tileA->m_room->m_connexions.push_back(connexion);
	tileB->m_room->m_connexions.push_back(connexion);

	tileA->m_connexion = connexion;
	tileB->m_connexion = connexion;

	return true;
}

Room* Ship::ConnectRooms()
{
	m_connexions.clear();

	Room* unconnected_room = NULL;

	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		for (vector<RoomTile*>::iterator it3 = (*it)->m_tiles.begin(); it3 != (*it)->m_tiles.end(); it3++)
		{
			(*it3)->m_connexion = NULL;
		}

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

	//flag hull (i.e. tiles that are the exterior border of the ship)
	vector<vector<RoomTile*> > hull_tiles;
	for (int i = 0; i < m_rooms_size.x; i++)
	{
		vector<RoomTile*> vec;
		for (int j = 0; j < m_rooms_size.y; j++)
		{
			RoomTile* tile = NULL;
			vec.push_back(tile);
		}
		hull_tiles.push_back(vec);
	}

	//fill all room tiles in an array with NULL pointer for holes
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			hull_tiles[(*it2)->m_coord_x][(*it2)->m_coord_y] = (*it2);
		}
	}

	//look out for room tiles adjacent to holes
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			(*it2)->m_hull = Hull_None;

			int x = (*it2)->m_coord_x;
			int y = (*it2)->m_coord_y;

			if (x == 0 || hull_tiles[x - 1][y] == NULL)
			{
				(*it2)->m_hull = Hull_Left;
			}
			if (x == m_rooms_size.x - 1 || hull_tiles[x + 1][y] == NULL)
			{
				(*it2)->m_hull = Hull_Right;
			}
			if (y == 0 || hull_tiles[x][y - 1] == NULL)
			{
				(*it2)->m_hull = Hull_Up;
			}
			if (y == m_rooms_size.y - 1 || hull_tiles[x][y + 1] == NULL)
			{
				(*it2)->m_hull = Hull_Down;
			}
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

	RoomTile* tile = weapon->GetFreeRoomTile(room);

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
	
	return weapon->Fire(deltaTime, m_position, m_angle, target->m_distance_combat, target_tile);
}

void Ship::BuildShip(bool minimized)
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

	//doors
	ConnectRooms();

	//center position of each room & room tiles
	CenterRoomPositions(m_is_minimized);
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

	//enemy (minimized) = flip the ship (180�)

	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		if (minimized == false)
		{
			(*it)->m_position.x = room_offset_x + ((*it)->m_upcorner_x + (*it)->m_width * 0.5f) * size;
			(*it)->m_position.y = room_offset_y + ((*it)->m_upcorner_y + (*it)->m_height * 0.5f) * size;
		}
		else
		{
			(*it)->m_position.x = room_offset_x + (m_rooms_size.x - ((*it)->m_upcorner_x + (*it)->m_width * 0.5f)) * size;
			(*it)->m_position.y = room_offset_y + (m_rooms_size.y - ((*it)->m_upcorner_y + (*it)->m_height * 0.5f)) * size;
		}
		

		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			int upcorner = (*it)->m_upcorner_x;
			int coord = (*it2)->m_coord_x;

			if (minimized == false)
			{
				(*it2)->m_position.x = room_offset_x + (0.5f + (*it2)->m_coord_x) * size;
				(*it2)->m_position.y = room_offset_y + (0.5f + (*it2)->m_coord_y) * size;
			}
			else
			{
				(*it2)->m_position.x = room_offset_x + (- 0.5f + m_rooms_size.x - (*it2)->m_coord_x) * size;
				(*it2)->m_position.y = room_offset_y + (- 0.5f + m_rooms_size.y - (*it2)->m_coord_y) * size;
			}
			
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
}