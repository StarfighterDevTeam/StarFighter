#include "Ship.h"

extern Game* CurrentGame;

Ship::Ship(DMS_Coord coord, ShipType type) : GameEntity(UI_EnemyShip)
{
	m_type = type;
	m_destination = NULL;
	m_seaport = NULL;
	m_tactical_icon = NULL;

	//get on tile
	SetDMSCoord(coord);

	//shape for water tiles
	sf::Texture* texture = TextureLoader::getInstance()->loadTexture(SHIP_TEXTURE_NAME, (int)WATERTILE_SIZE, (int)WATERTILE_SIZE * 2);
	sf::Texture* texture2 = TextureLoader::getInstance()->loadTexture(SHIP_TACTICAL_TEXTURE_NAME, (int)TACTICALTILE_SIZE * 4, (int)TACTICALTILE_SIZE * 8);

	setAnimation(texture, 1, 2);

	m_can_be_seen = false;

	//m_nb_crew[NB_CREW_TYPES];
	//m_nb_crew_working[NB_CREW_TYPES];
	//m_nb_crew_max;

	//vector<Room*> m_rooms;
	//vector<RoomConnexion*> m_connexions;
	//vector<CrewMember*> m_crew;

	m_speed = sf::Vector2f(0, 0);
	m_angle = -90;
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

	delete m_tactical_icon;
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

void Ship::UpdatePositionOnMap(DMS_Coord warship_DMS)
{
	float diff_x = (m_DMS.m_minute_x + m_DMS.m_second_x / NB_WATERTILE_SUBDIVISION) - (warship_DMS.m_minute_x + warship_DMS.m_second_x / NB_WATERTILE_SUBDIVISION);
	float diff_y = (m_DMS.m_minute_y + m_DMS.m_second_y / NB_WATERTILE_SUBDIVISION) - (warship_DMS.m_minute_y + warship_DMS.m_second_y / NB_WATERTILE_SUBDIVISION);

	m_position.x = WARSHIP_MAP_OFFSET_X + WATERTILE_SIZE * diff_x;
	m_position.y = WARSHIP_MAP_OFFSET_Y - WATERTILE_SIZE * diff_y;//from bottom to top

	//tactical scale representation
	if (m_tactical_icon != NULL)
	{
		m_tactical_icon->setPosition(m_position);
		m_tactical_icon->setRotation(m_angle);
		m_tactical_icon->setAnimationLine(m_currentAnimationIndex);
	}

	GameEntity::UpdatePosition();
}

void Ship::Update(Time deltaTime, DMS_Coord warship_DMS)
{
	//tactical scale representation
	if (m_tactical_icon != NULL)
	{
		//apply movement
		m_speed.x = TACTICAL_SPEED_FACTOR * CRUISE_SPEED * sin(m_angle * M_PI / 180.f);
		m_speed.y = -TACTICAL_SPEED_FACTOR * CRUISE_SPEED * cos(m_angle * M_PI / 180.f);

		m_tactical_posx += m_speed.x * deltaTime.asSeconds();
		m_tactical_posy -= m_speed.y * deltaTime.asSeconds();

		UpdateTacticalPositionOnMap();

		m_tactical_icon->setPosition(m_position);
		m_tactical_icon->setRotation(m_angle);
		m_tactical_icon->setAnimationLine(m_currentAnimationIndex);
	}
	//strategic scale
	else
	{
		UpdatePositionOnMap(warship_DMS);
	}

	UpdateAnimation();

	//update tile information
	m_tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x][m_DMS.m_minute_y];
	m_can_be_seen = m_tile->m_can_be_seen;

	
}

void Ship::Draw(sf::RenderTexture& screen)
{
	//strategic scale
	if (m_tactical_icon == NULL)
	{
		GameEntity::Draw(screen);
	}
	else//tactical scale
	{
		m_tactical_icon->Draw(screen);
	}
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


Room* Ship::AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type)
{
	Room* room = new Room(upcorner_x, upcorner_y, width, height, type);

	m_rooms.push_back(room);

	return room;
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
	RoomConnexion* connexion = new RoomConnexion(pair<RoomTile*, RoomTile*>(tileA, tileB), false);

	m_connexions.push_back(connexion);
	tileA->m_room->m_connexions.push_back(connexion);
	tileB->m_room->m_connexions.push_back(connexion);

	return true;
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

	//everything ok
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

bool Ship::FireWeapon(Weapon* weapon, Time deltaTime)
{
	return weapon->Fire(deltaTime, m_position, m_angle);
}

void Ship::UpdateTacticalPositionOnMap()
{
	float delta_pox_x = - NB_TACTILAL_TILES_VIEW_RANGE + m_tactical_posx;
	float delta_pox_y = - NB_TACTILAL_TILES_VIEW_RANGE + m_tactical_posy;

	m_position.x = WARSHIP_TACTICALMAP_OFFSET_X + TACTICALTILE_SIZE * (0.5f + delta_pox_x);
	m_position.y = WARSHIP_TACTICALMAP_OFFSET_Y - TACTICALTILE_SIZE * (0.5f + delta_pox_y);
}