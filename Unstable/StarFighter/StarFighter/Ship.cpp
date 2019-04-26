#include "Ship.h"

extern Game* CurrentGame;

Ship::Ship(DMS_Coord coord, ShipType type, ShipAlliance alliance) : GameEntity(UI_EnemyShip)
{
	m_type = type;
	m_alliance = alliance;
	m_destination = NULL;
	m_seaport = NULL;
	m_distance_combat = DISTANCE_COMBAT_INIT;
	m_lifeboats = 0;
	m_display_name = (*CurrentGame).GetRandomShipName();

	m_rooms_min_upcorner_x = 0;
	m_rooms_min_upcorner_y = 0;
	m_rooms_max_offset_x = 0;
	m_rooms_max_offset_y = 0;

	m_health_max = 0;
	m_flood_max = 0;
	m_flood = 0;
	m_flee_timer = 0.f;
	m_flee_count = 0.f;
	m_is_charging_flee_count = false;
	m_is_fleeing = false;

	m_rudder = NULL;

	m_sinking_timer = 0.f;

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

	//resources
	m_resources[Resource_Gold] = 0;
	m_resources[Resource_Fish] = 0;
	m_resources[Resource_Mech] = 0;
	m_resources[Resource_Fidelity] = 0;
	m_resources[Resource_Days] = 0;
}

Ship::Ship(DMS_Coord coord, ShipType type, ShipAlliance alliance, string display_name) : Ship(coord, type, alliance)
{
	m_display_name = display_name;
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

	//for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	//{
	//	delete *it;
	//}

	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		delete *it;
	}

	for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
	{
		delete *it;
	}

	delete m_rudder;

	m_combat_interface.Destroy();
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
	if (tile->m_location != NULL && tile->m_location->m_type == Location_Seaport && tile != (WaterTile*)m_seaport)
	{
		m_seaport = (Seaport*)tile->m_location;
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

void Ship::UpdateTactical(Time deltaTime)
{
	UpdateFlooding(deltaTime);

	if (m_is_charging_flee_count == true)
	{
		UpdateFleeing(deltaTime);
	}

	if ((IsFlooded() == true || m_health == 0) && m_is_fleeing == false)
	{
		UpdateSinking(deltaTime);
	}
}

void Ship::UpdateStrategical(Time deltaTime)
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
				SetDMSCoord(m_DMS);//allows to assign a seaport if this is the final tile
			}
			//go to next waypoint
			else
			{
				waypoint = m_current_path.back();
				waypoint->UpdatePosition(m_DMS);//update waypoint position (because of the previous snap)
				sf::Vector2f vec = waypoint->m_position - m_position;
				ScaleVector(&vec, CRUISE_SPEED);
				m_speed = vec;
			}
		}
	}

	m_DMS.m_second_x += m_speed.x * deltaTime.asSeconds();
	m_DMS.m_second_y -= m_speed.y * deltaTime.asSeconds();

	//rotation
	GetAngleForSpeed(m_angle);
	UpdateAnimation();

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
	Room* room = new Room(upcorner_x, upcorner_y, width, height, type, m_alliance);
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

CrewMember* Ship::AddCrewMember(CrewMember* crew, Room* room)
{
	//if (m_nb_crew >= m_nb_crew_max)
	//{
	//	return NULL;
	//}

	if (room == NULL)
	{
		for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
		{
			if ((*it)->m_type == Room_Crewquarter)
			{
				room = *it;
				break;
			}
		}
	}

	RoomTile* tile = crew->GetFreeRoomTile(room);

	if (tile == NULL)
	{
		return NULL;
	}

	//position
	crew->m_position = tile->m_position;

	//add to crew lists
	m_crew[0].push_back(crew);

	//assign crew to tile
	crew->m_tile = tile;
	tile->m_crew = crew;

	m_nb_crew++;
	crew->m_alliance = m_alliance;

	crew->m_is_prisoner = false;
	crew->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Magenta_Crew]);

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

	RoomTile* tileA = RoomTile::GetRoomTileAtCoord(tileA_x, tileA_y, m_alliance != Alliance_Player);
	RoomTile* tileB = RoomTile::GetRoomTileAtCoord(tileB_x, tileB_y, m_alliance != Alliance_Player);

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
	RoomConnexion* connexion = new RoomConnexion(pair<RoomTile*, RoomTile*>(tileA, tileB), false, this);

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

void Ship::AddWeaponToTile(Weapon* weapon, RoomTile* tile)
{
	//position
	weapon->m_position = tile->m_position;

	//add to crew lists
	m_weapons.push_back(weapon);

	//assign crew to tile
	weapon->m_tile = tile;
	tile->m_weapon = weapon;

	//save owner ship
	weapon->m_ship = this;

	weapon->UpdatePosition();
}

void Ship::AddEngineToTile(RoomTile* tile)
{
	Engine* engine = new Engine();

	//position
	engine->m_position = tile->m_position;

	//add to crew lists
	m_engines.push_back(engine);

	//assign crew to tile
	engine->m_tile = tile;
	tile->m_engine = engine;

	//save owner ship
	engine->m_ship = this;

	engine->UpdatePosition();
}

void Ship::AddRudderToTile(RoomTile* tile)
{
	m_rudder = new Rudder();

	//position
	m_rudder->m_position = tile->m_position;

	//assign crew to tile
	m_rudder->m_tile = tile;
	tile->m_rudder = m_rudder;

	//save owner ship
	m_rudder->m_ship = this;

	m_rudder->UpdatePosition();
}

bool Ship::FireWeapon(Weapon* weapon, Time deltaTime, Ship* target)
{
	if (m_sinking_timer > 0.f || m_is_fleeing == true)
	{
		return false;
	}

	//turn angle towards the center of the targeted room
	Room* room = weapon->m_target_room;
	int width = room->m_width % 2 == 0 ? room->m_width - 1 : room->m_width;
	int height = room->m_height % 2 == 0 ? room->m_height - 1 : room->m_height;
	int x = (2 * room->m_upcorner_x + width - 1) / 2;
	int y = (2 * room->m_upcorner_y + height - 1) / 2;

	RoomTile* virtual_target_tile = target->m_tiles[x][y];
	sf::Vector2f vec = virtual_target_tile->m_position - weapon->m_position;
	float angle;
	GetAngleForVector(vec, angle);
	float current_rotation = weapon->getRotation();

	if (current_rotation != angle)
	{
		float delta = (current_rotation + weapon->m_angle) - angle;
		if (delta > 180)
			delta -= 360;
		else if (delta < -180)
			delta += 360;

		if (abs(delta) > abs(weapon->m_angle_speed)*deltaTime.asSeconds())
		{
			weapon->rotate(delta >= 0 ? - weapon->m_angle_speed * deltaTime.asSeconds() : weapon->m_angle_speed * deltaTime.asSeconds());
			return false;//not aligned = not ready to fire
		}
		else
		{
			weapon->setRotation(angle - weapon->m_angle);
		}
	}

	//cooldown ready?
	if (weapon->m_rof_timer > 0)
	{
		return false;
	}

	//hit or miss?
	float hit_success = RandomizeFloatBetweenValues(0.f, 1.f);
	bool hit = true;
	if (weapon->m_is_guided == false && hit_success < target->GetDodgeChances())//chance of miss
	{
		hit = false;
	}

	//pick a random enemy tile within the targeted room
	int r;
	RoomTile* target_tile;

	if (weapon->m_type != Weapon_Torpedo)
	{
		r = RandomizeIntBetweenValues(0, weapon->m_target_room->m_tiles.size() - 1);
		target_tile = weapon->m_target_room->m_tiles[r];
	}
	else if (weapon->m_type == Weapon_Torpedo)
	{
		vector<RoomTile*> possible_targets;
		for (vector<RoomTile*>::iterator it = room->m_tiles.begin(); it != room->m_tiles.end(); it++)
		{
			//hull tiles occupied by a weapon cannot be pierced, so cannot be targeted by torpedoes
			if ((*it)->m_weapon == NULL && (((*it)->m_hull == Hull_Left && target->m_alliance != Alliance_Player) || ((*it)->m_hull == Hull_Right && target->m_alliance == Alliance_Player)))
			{
				possible_targets.push_back(*it);
			}
		}

		if (possible_targets.empty() == true)
		{
			weapon->m_target_room = NULL;//weapon should choose a new target
			return false;
		}
		r = RandomizeIntBetweenValues(0, possible_targets.size() - 1);
		target_tile = possible_targets[r];
	}
	
	sf::Vector2f vec2 = target_tile->m_position - weapon->m_position;
	float angle2;
	GetAngleForVector(vec2, angle2);

	sf::Vector2f target_position = hit == true ? target_tile->m_position : (weapon->m_position + target_tile->m_position) * 0.5f;

	weapon->Fire(angle2, target->m_distance_combat, target, hit == true ? target_tile : NULL, target_position);
	return true;
}

void Ship::BuildShip()
{
	//ROOMS
	//left
	//left
	Room* weapon_room = AddRoom(0, 1, 3, 4, Room_Weapon);
	AddRoom(0, 5, 3, 4, Room_Weapon);

	//mid
	Room* nav_room = AddRoom(3, 0, 5, 2, Room_Navigation);
	AddRoom(3, 2, 5, 5, Room_Crewquarter);
	Room* engine_room = AddRoom(3, 7, 5, 2, Room_Engine);
	Room* lifeboats = AddRoom(3, 9, 5, 1, Room_Lifeboat);

	//right
	Room* prison_room = AddRoom(8, 1, 2, 4, Room_Prison);
	Room* prison_cell = NULL;
	for (int i = 0; i < 4; i++)
	{
		Room* cell = AddRoom(10, 1 + i, 1, 1, Room_PrisonCell);
		if (i == 0)
		{
			prison_cell = cell;
		}
		cell->m_tiles.front()->m_is_prison = true;
		m_prison_cells.push_back(cell->m_tiles.front());
	}

	AddRoom(8, 5, 3, 4, Room_Diving);
	
	m_health = m_health_max;

	//center position of each room & room tiles, fill m_tiles and (*CurrentGame).m_tiles / m_enemy_tiles.
	CenterRoomPositions(m_alliance != Alliance_Player);

	//doors
	ConnectRooms();

	//hardcode to add and lock doors of prison cells
	AddConnexion(prison_cell->m_tiles[0]->m_coord_x, prison_cell->m_tiles[0]->m_coord_y, prison_room->m_tiles[prison_room->m_width - 1]->m_coord_x, prison_room->m_tiles[prison_room->m_width - 1]->m_coord_y);
	for (vector<RoomConnexion*>::iterator it = m_connexions.begin(); it != m_connexions.end(); it++)
	{
		if ((*it)->m_tiles.second->m_room->m_type == Room_PrisonCell || (*it)->m_tiles.first->m_room->m_type == Room_PrisonCell)
		{
			(*it)->SetLock(true);
		}
	}

	//hull
	FlagHullRoomTiles();

	//gunner tiles
	int size = weapon_room->m_tiles.size() - 1;
	for (int i = 0; i < 3; i++)
	{
		int x = i * weapon_room->m_width;
		weapon_room->m_tiles[x]->m_operator_tile = weapon_room->m_tiles[x + 1];
		weapon_room->m_tiles[x + 1]->m_system_tile = weapon_room->m_tiles[x];

		weapon_room->m_tiles[x]->m_system = System_Weapon;

		Weapon* weapon = NULL;
		if (i == 0)
		{
			weapon = new Weapon(Weapon_Cannon, false);
		}
		else if (i == 1)
		{
			weapon = new Weapon(Weapon_Torpedo, false);
		}
		else if (i == 2)
		{
			weapon = new Weapon(Weapon_Shrapnel, false);
		}
		
		AddWeaponToTile(weapon, weapon_room->m_tiles[x]);
		weapon->setAnimationLine(1);//horizontal mirroring
		weapon->m_angle = 270.f;
	}

	//navigation tile
	if (nav_room->m_tiles[2]->m_coord_y == nav_room->m_upcorner_y)
	{
		nav_room->m_tiles[2]->m_operator_tile = nav_room->m_tiles[2 + nav_room->m_width];
		nav_room->m_tiles[2 + nav_room->m_width]->m_system_tile = nav_room->m_tiles[2];

		nav_room->m_tiles[2]->m_system = System_Navigation;

		AddRudderToTile(nav_room->m_tiles[2]);
	}

	//engine tiles
	for (int i = 0; i < 2; i++)
	{
		i *= engine_room->m_width - 1;
		engine_room->m_tiles[i + engine_room->m_width]->m_operator_tile = engine_room->m_tiles[i];
		engine_room->m_tiles[i]->m_system_tile = engine_room->m_tiles[i + engine_room->m_width];
		engine_room->m_tiles[i + engine_room->m_width]->m_system = System_Engine;
		AddEngineToTile(engine_room->m_tiles[i + engine_room->m_width]);
	}

	//lifeboats
	m_lifeboats += lifeboats->m_tiles.size();

	//crew
	m_nb_crew_max = 12;
	m_nb_crew = 0;

	vector<Room*> possible_rooms;
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		if ((*it)->m_type != Room_PrisonCell)
		{
			possible_rooms.push_back(*it);
		}
	}
	for (int i = 0; i < 11; i++)
	{
		int r = RandomizeIntBetweenValues(0, possible_rooms.size() - 1);
		//r = 12;
		CrewMember* crew = new CrewMember(Crew_Civilian, m_alliance);
		AddCrewMember(crew, possible_rooms[r]);
	}

	CrewMember* prisoner = new CrewMember(Crew_Civilian, m_alliance);
	ImprisonCrew(prisoner);

	//CHEAT DEBUG
	m_health = /*m_health_max * 0.2 +*/ 8;
}

void Ship::CenterRoomPositions(bool is_enemy)
{
	float room_offset_x = is_enemy == false ? ROOMTILE_OFFSET_X : ROOMTILE_ENEMY_OFFSET_X;
	float room_offset_y = is_enemy == false ? ROOMTILE_OFFSET_Y : ROOMTILE_ENEMY_OFFSET_Y;

	room_offset_x -= 1.f * m_rooms_size.x / 2 * ROOMTILE_SIZE;
	room_offset_y -= 1.f * m_rooms_size.y / 2 * ROOMTILE_SIZE;

	//fill hull array with NULL pointers
	for (int i = 0; i < m_rooms_size.x; i++)
	{
		vector<RoomTile*> vec;
		for (int j = 0; j < m_rooms_size.y; j++)
		{
			vec.push_back(NULL);
		}
		m_tiles.push_back(vec);
	}

	//set centered postion for each room and tiles, and fill the existing tiles into Ship m_tiles and (*CurrentGame).m_tiles / m_enemy_tiles
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{	
		(*it)->m_position.x = room_offset_x + ((*it)->m_upcorner_x + (*it)->m_width * 0.5f) * ROOMTILE_SIZE;
		(*it)->m_position.y = room_offset_y + ((*it)->m_upcorner_y + (*it)->m_height * 0.5f) * ROOMTILE_SIZE;
		
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			int upcorner = (*it)->m_upcorner_x;
			int coord = (*it2)->m_coord_x;
			
			(*it2)->m_position.x = room_offset_x + (0.5f + (*it2)->m_coord_x) * ROOMTILE_SIZE;
			(*it2)->m_position.y = room_offset_y + (0.5f + (*it2)->m_coord_y) * ROOMTILE_SIZE;

			(*it2)->UpdatePosition();

			m_tiles[(*it2)->m_coord_x][(*it2)->m_coord_y] = (*it2);

			if (is_enemy == false)
			{
				(*CurrentGame).m_tiles.push_back(*it2);
			}
			else
			{
				(*CurrentGame).m_enemy_tiles.push_back(*it2);
			}
		}
	}

	//update connexion, crew, and weapon positions
	for (vector<RoomConnexion*>::iterator it = m_connexions.begin(); it != m_connexions.end(); it++)
	{
		(*it)->m_position = sf::Vector2f(0.5f * ((*it)->m_tiles.first->m_position.x + (*it)->m_tiles.second->m_position.x), 0.5f * ((*it)->m_tiles.first->m_position.y + (*it)->m_tiles.second->m_position.y));
	}

	//crew & prisoners
	for (int j = 0; j < 2; j++)
	{
		for (vector<CrewMember*>::iterator it = m_crew[j].begin(); it != m_crew[j].end(); it++)
		{
			(*it)->m_position = (*it)->m_tile->m_position;
		}
	}

	//weapons
	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		(*it)->m_position = (*it)->m_tile->m_position;
	}
}


void Ship::InitCombat()
{
	m_ship_offset = sf::Vector2f(0.f, 0.f);
	m_speed = sf::Vector2f(0.f, 0.f);

	//reset weapon rof timers
	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		(*it)->m_rof_timer = (*it)->GetRof();
		(*it)->m_target_room = NULL;
	}

	//reset flee timer and counter
	m_flee_timer = 0.f;
	m_flee_count = 0.f;
	m_is_fleeing = false;
	m_sinking_timer = 0.f;
	ApplyAlphaToShip(255);//and cancel alpha from sinking feedback
}

void Ship::RestoreHealth()
{
	m_health = m_health_max;
	m_flood = 0;

	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			(*it2)->m_health = (*it2)->m_health_max;
			(*it2)->m_is_pierced = false;
			(*it2)->m_flood = 0;

			if ((*it2)->m_weapon != NULL)
			{
				(*it2)->m_weapon->m_health = (*it2)->m_weapon->m_health_max;
			}

			if ((*it2)->m_connexion != NULL)
			{
				(*it2)->m_connexion->m_destroyed = false;
			}
		}
	}

	for (int j = 0; j < 2; j++)
	{
		for (vector<CrewMember*>::iterator it = m_crew[j].begin(); it != m_crew[j].end(); it++)
		{
			(*it)->m_health = (*it)->m_health_max;
		}
	}
}

void Ship::RestoreWeaponsHealth()
{
	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		(*it)->m_health = (*it)->m_health_max;
	}
}

void Ship::UpdateFlooding(Time deltaTime)
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

			if ((*it2)->m_flooding_timer > 0)
			{
				(*it2)->m_flooding_timer -= deltaTime.asSeconds();
			}

			if ((*it2)->m_evaporation_timer > 0)
			{
				(*it2)->m_evaporation_timer -= deltaTime.asSeconds();
			}

			//1. "Generate"
			if ((*it2)->m_is_pierced == true)
			{
				if ((*it2)->m_flooding_timer <= 0)
				{
					(*it2)->m_flooding_timer = FLOODING_TIMER;
					(*it2)->m_evaporation_timer = FLOOD_EVAPORATION_TIME;

					if ((*it2)->m_flood < ROOMTILE_FLOODING_GENERATION)
					{
						(*it2)->m_flood = ROOMTILE_FLOODING_GENERATION;
					}
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
			}

			//update flooding algo
			//Flood dir:
			//0: Hull_Left,
			//1: Hull_Up,
			//2: Hull_Right,
			//3: Hull_Down,

			if ((*it2)->m_flood == 0)
			{
				continue;
			}

			//Evaporation
			if ((*it2)->m_evaporation_timer <= 0)
			{
				(*it2)->m_evaporation_timer = FLOOD_EVAPORATION_TIME;
				(*it2)->m_flood--;
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
						if (tile != NULL && tile->m_flood < ROOMTILE_FLOODING_MAX && (tile->m_room == (*it2)->m_room || ((*it2)->m_connexion != NULL && (*it2)->m_connexion->m_is_locked == false && ((*it2)->m_connexion->m_tiles.first == tile || (*it2)->m_connexion->m_tiles.second == tile))))
						{
							tile->m_flood++;//transfer
							(*it2)->m_flood--;
							tile->m_flood_dir[i] = true;//keep the same momentum direction in the receiving tile
							tile->m_evaporation_timer = FLOOD_EVAPORATION_TIME;
						}
					}
				}

				//3. "Expand"
				int r = RandomizeIntBetweenValues(0, 16);
				if (r < 4)
					(*it2)->m_flood_dir[r] = true;
			}

			//Update ship flood total value (1 point per tile flooded)
			m_flood++;
		}
	}
}

bool Ship::IsSystemOperational(ShipSystem system, RoomTile* tile)
{
	bool is_system = tile->m_system == system;
	bool is_operational = tile->m_operator_tile != NULL && tile->m_operator_tile->m_crew != NULL && tile->m_operator_tile->m_crew->m_tile == tile->m_operator_tile;
	bool is_weapon_alive = tile->m_weapon == NULL || tile->m_weapon->m_health > 0;

	return is_system && is_operational && is_weapon_alive;
}

float Ship::GetDodgeChances()
{
	float dodge = 0.f;

	if (m_rudder != NULL)
	{
		if (IsSystemOperational(System_Navigation, m_rudder->m_tile))
		{
			dodge += NAVIGATION_DODGE_CHANCE + (1.f * m_rudder->m_tile->m_operator_tile->m_crew->m_skills[Skill_Navigation] / 100);
		}
	}

	for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
	{
		if (IsSystemOperational(System_Engine, (*it)->m_tile))
		{
			dodge += NAVIGATION_DODGE_CHANCE + (1.f * (*it)->m_tile->m_operator_tile->m_crew->m_skills[Skill_Engine] / 100);
		}
	}

	dodge = Minf(dodge, DODGE_CHANCE_HARDCAP);

	return dodge;
}

void Ship::UpdateShipOffset()
{
	//rooms
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		(*it)->m_ship_offset = m_ship_offset;

		//room tiles
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			(*it2)->m_ship_offset = m_ship_offset;
		}
	}

	//doors
	for (vector<RoomConnexion*>::iterator it = m_connexions.begin(); it != m_connexions.end(); it++)
	{
		(*it)->m_ship_offset = m_ship_offset;
	}

	//weapons
	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		(*it)->m_ship_offset = m_ship_offset;
	}

	//engines
	for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
	{
		(*it)->m_ship_offset = m_ship_offset;
	}

	//rudder
	if (m_rudder != NULL)
	{
		m_rudder->m_ship_offset = m_ship_offset;
	}

	//crew
	for (int j = 0; j < 2; j++)
	{
		for (vector<CrewMember*>::iterator it = m_crew[j].begin(); it != m_crew[j].end(); it++)
		{
			(*it)->m_ship_offset = m_ship_offset;
		}
	}
}

void Ship::UpdateFleeing(Time deltaTime)
{
	//position
	//for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
	//{
	//	sf::Vector2f position = (*it)->m_position + m_ship_offset;
	//	(*it)->m_systembar->m_shape_container.setPosition(position.x, position.y - (*it)->m_size.y * 0.5f - LIFEBAR_OFFSET_Y - (LIFEBAR_SIZE_Y * 0.5f * 2));
	//	(*it)->m_systembar->m_shape.setPosition(position.x, position.y - (*it)->m_size.y * 0.5f - LIFEBAR_OFFSET_Y - (LIFEBAR_SIZE_Y * 0.5f * 2));
	//}

	//charging
	if (m_flee_count < ENGINE_FLEE_COUNT)
	{
		if (m_flee_timer > 0)
		{
			m_flee_timer -= deltaTime.asSeconds();
		}
		if (m_flee_timer <= 0)
		{
			m_flee_timer = 1.f;

			for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
			{
				if (IsSystemOperational(System_Engine, (*it)->m_tile) == true)
				{
					m_flee_count += 1.f + (1.f * (*it)->m_tile->m_operator_tile->m_crew->m_skills[Skill_Engine] / 100);
				}
			}

			m_flee_count = Minf(m_flee_count, ENGINE_FLEE_COUNT);

			for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
			{
				float flee_ratio = 1.0f * m_flee_count / ENGINE_FLEE_COUNT;
				(*it)->m_systembar->m_shape.setSize(sf::Vector2f(flee_ratio * LIFEBAR_SIZE_X, LIFEBAR_SIZE_Y));
			}
		}
	}

	//color
	bool one_is_operational = false;
	for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
	{
		if (IsSystemOperational(System_Engine, (*it)->m_tile) == true)
		{
			one_is_operational = true;
			break;
		}
	}

	for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
	{
		if (m_flee_count == ENGINE_FLEE_COUNT && one_is_operational == true)
		{
			(*it)->m_systembar->m_shape.setFillColor((*CurrentGame).m_dico_colors[Color_Magenta_EngineCharged]);
		}
		else
		{
			(*it)->m_systembar->m_shape.setFillColor((*CurrentGame).m_dico_colors[Color_Cyan_System]);
		}
	}

	//fleeing?
	if (m_is_fleeing == true)
	{
		if (m_speed.y > -SHIP_FLEE_SPEED_MAX)
		{
			m_speed.y -= deltaTime.asSeconds() * SHIP_FLEE_ACCELERATION;
		}
		m_ship_offset.y += m_speed.y;
	}

	UpdateShipOffset();
}

void Ship::ApplyAlphaToShip(Uint8 alpha)
{
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		sf::Color c = (*it)->m_shape_container.getOutlineColor();
		(*it)->m_shape_container.setOutlineColor(sf::Color(c.r, c.g, c.b, alpha));

		sf::Color c4 = (*it)->m_text.getColor();
		(*it)->m_text.setColor(sf::Color(c4.r, c4.g, c4.b, alpha));

		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			sf::Color c2 = (*it2)->m_shape_container.getFillColor();
			(*it2)->m_shape_container.setFillColor(sf::Color(c2.r, c2.g, c2.b, alpha));

			sf::Color c3 = (*it2)->m_shape_container.getOutlineColor();
			(*it2)->m_shape_container.setOutlineColor(sf::Color(c3.r, c3.g, c3.b, alpha));
		}
	}

	for (vector<RoomConnexion*>::iterator it = m_connexions.begin(); it != m_connexions.end(); it++)
	{
		sf::Color c = (*it)->m_shape_container.getFillColor();
		(*it)->m_shape_container.setFillColor(sf::Color(c.r, c.g, c.b, alpha));

		sf::Color c2 = (*it)->m_shape_container.getOutlineColor();
		(*it)->m_shape_container.setOutlineColor(sf::Color(c2.r, c2.g, c2.b, alpha));
	}

	for (vector<Weapon*>::iterator it = m_weapons.begin(); it != m_weapons.end(); it++)
	{
		(*it)->setColor(sf::Color(255, 255, 255, alpha));
	}

	for (vector<Engine*>::iterator it = m_engines.begin(); it != m_engines.end(); it++)
	{
		(*it)->setColor(sf::Color(255, 255, 255, alpha));
	}

	if (m_rudder != NULL)
	{
		m_rudder->setColor(sf::Color(255, 255, 255, alpha));
	}
}

void Ship::UpdateSinking(Time deltaTime)
{
	if (m_sinking_timer < SHIP_SINKING_TIME)
	{
		m_sinking_timer += deltaTime.asSeconds();

		Uint8 alpha = (Uint8)Lerp(m_sinking_timer, 0, SHIP_SINKING_TIME, 255, 0);

		ApplyAlphaToShip(alpha);

		//stop crew members
		for (int j = 0; j < 2; j++)
		{
			for (vector<CrewMember*>::iterator it = m_crew[j].begin(); it != m_crew[j].end(); it++)
			{
				//(*it)->m_tile = NULL;
				(*it)->m_destination = NULL;
				(*it)->m_speed = sf::Vector2f(0, 0);
				(*it)->m_melee_opponent = NULL;
			}
		}
	}
}

bool Ship::IsFlooded()
{
	return m_flood > m_flood_max * 0.5f;
}

bool Ship::IsPrisonCellFree()
{
	for (vector<RoomTile*>::iterator it = m_prison_cells.begin(); it != m_prison_cells.end(); it++)
	{
		if ((*it)->m_crew == NULL)
		{
			return true;
		}
	}

	return false;
}

bool Ship::ImprisonCrew(CrewMember* crew)
{
	int i = 0;
	for (vector<RoomTile*>::iterator it = m_prison_cells.begin(); it != m_prison_cells.end(); it++)
	{
		if ((*it)->m_crew == NULL)
		{
			if (crew->Imprison(*it) == false)
			{
				return false;
			}

			break;
		}
		else if (i == m_prison_cells.size() - 1)
		{
			return false;
		}
		else
		{
			i++;
		}
	}


	//free cell found
	m_crew[1].push_back(crew);
	crew->m_alliance = m_alliance;

	return true;
}

void Ship::UpdatePrisonerEscape(CrewMember* crew, sf::Time deltaTime)
{
	//escaping the prison cell?
	if (crew->m_tile != NULL && crew->m_tile->m_room->m_type == Room_PrisonCell && crew->m_tile->m_connexion->m_is_locked == false && m_health > PRISONER_HEALTH_MIN_TO_ESCAPE)
	{
		EscapeToRandomRoom(crew);
	}
	else if (crew->m_tile->m_room->m_type != Room_PrisonCell && crew->m_destination == NULL)//already escaped, now roaming
	{
		if (crew->m_prisoner_roaming_timer > 0)
		{
			crew->m_prisoner_roaming_timer -= deltaTime.asSeconds();
		}

		//Updating roaming
		if (crew->m_prisoner_roaming_timer <= 0)
		{
			//same room
			if (RandomizeFloatBetweenValues(0.f, 1.f) > PRISONER_CHANGING_ROOM_PROBA)
			{
				EscapeToRandomTileInRoom(crew, crew->m_tile->m_room);
			}
			else//changing room
			{
				EscapeToRandomRoom(crew);
			}
		}
	}
}

void Ship::EscapeToRandomRoom(CrewMember* crew)
{
	vector<Room*> possible_rooms;
	for (vector<Room*>::iterator it = m_rooms.begin(); it != m_rooms.end(); it++)
	{
		if ((*it)->m_type != Room_PrisonCell && (*it)->m_is_flooded == false)
		{
			possible_rooms.push_back(*it);
		}
	}

	if (possible_rooms.empty() == false)
	{
		int r = RandomizeIntBetweenValues(0, possible_rooms.size() - 1);

		EscapeToRandomTileInRoom(crew, possible_rooms[r]);
	}
}

void Ship::EscapeToRandomTileInRoom(CrewMember* crew, Room* room)
{
	vector<RoomTile*> possible_tiles;
	for (vector<RoomTile*>::iterator it = room->m_tiles.begin(); it != room->m_tiles.end(); it++)
	{
		if ((*it)->m_system == System_None && (*it)->m_crew == NULL && (*it)->m_weapon == NULL && *it != crew->m_tile)
		{
			possible_tiles.push_back(*it);
		}
	}

	if (possible_tiles.empty() == false)
	{
		int r2 = RandomizeIntBetweenValues(0, possible_tiles.size() - 1);
		crew->m_destination = possible_tiles[r2];
		crew->m_prisoner_roaming_timer = RandomizeFloatBetweenValues(PRISONER_ROAM_TIMER_MIN, PRISONER_ROAM_TIMER_MAX);
	}
}

void Ship::Reset()
{
	for (int j = 0; j < 2; j++)
	{
		for (vector<CrewMember*>::iterator it = m_crew[j].begin(); it != m_crew[j].end(); it++)
		{
			delete *it;
		}
		m_crew[j].clear();
	}

	m_speed = sf::Vector2f(0, 0);
}

bool Ship::SetSailsToWaterTile(WaterTile* tile)
{
	sf::Vector2f move_vector = tile->m_position - m_position;
	ScaleVector(&move_vector, CRUISE_SPEED);
	m_speed = move_vector;

	m_destination = tile;

	//Find path and set speed
	FindShortestPath(m_tile, m_destination);
	if (m_current_path.empty() == false)
	{
		WaterTile* waypoint = m_current_path.back();
		sf::Vector2f vec = waypoint->m_position - m_position;
		ScaleVector(&vec, CRUISE_SPEED);
		m_speed = vec;
	}

	//leaving port?
	if (m_seaport != NULL && m_destination != (WaterTile*)m_seaport)
	{
		m_seaport->RemoveShip(this);
		m_seaport = NULL;
	}

	return true;
}

int Ship::GetShortestPathLength(WaterTile* tileA, WaterTile* tileB)
{
	//save old path
	vector<WaterTile*> old_path;
	for (vector<WaterTile*>::iterator it = m_current_path.begin(); it != m_current_path.end(); it++)
	{
		old_path.push_back(*it);
	}
	m_current_path.clear();

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
	WaterTile* way_point = tileB;
	while (way_point != tileA)
	{
		m_current_path.push_back(way_point);
		way_point = way_point->m_parent;
	}

	//save size found
	int path_size = m_current_path.size();

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

	//restore old path
	m_current_path.clear();
	for (vector<WaterTile*>::iterator it = old_path.begin(); it != old_path.end(); it++)
	{
		m_current_path.push_back(*it);
	}

	//return path size found
	return path_size;
}

void Ship::FindShortestPath(WaterTile* tileA, WaterTile* tileB)
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

	//look for shortcuts
	path_size = temp_path.size();
	int index = 0;
	while (index < path_size)
	{
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

			//shortcut conditions: the rectangle is filled with only water, OR we can cast a ray to the tile without meeting islands
			bool only_water = IsOnlyWaterInsideRectangle(temp_path[index], temp_path[i]);
			if (only_water == false)
			{
				//ray-tracing should not take in account the "patch" on position but it's true position
				//WaterTile* true_tile = new WaterTile(temp_path[i]->m_coord_x, temp_path[i]->m_coord_y, temp_path[i]->m_type, temp_path[i]->m_zone, temp_path[i]->m_zone->m_coord_x, temp_path[i]->m_zone->m_coord_y);
				//if (i == path_size - 1)
				//{
				//	true_tile->m_coord_x -= patch_x;
				//	true_tile->m_coord_y -= patch_y;
				//}
				//only_water = RayTracingContainsIslandForPathfind(true_tile, temp_path[index]) == false;
				//delete true_tile;
			}

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

void Ship::IteratePathFinding(WaterTile* tileA, WaterTile* tileB)
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

bool Ship::IsOnlyWaterInsideRectangle(WaterTile* tileA, WaterTile* tileB)
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
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[j][k]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[j][k]->m_can_be_seen == false)
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

bool Ship::RayTracingContainsIsland(WaterTile* tileA, WaterTile* tileB)
{
	if (tileA == tileB)
	{
		return false;
	}

	int coord_x = tileA->m_coord_x;
	int coord_y = tileA->m_coord_y;

	bool contains_island = false;

	//Bresenham line-drawing algorithm
	int X = abs(tileA->m_coord_x - tileB->m_coord_x) + 1;
	int Y = abs(tileA->m_coord_y - tileB->m_coord_y) + 1;
	int sum = X + Y - 2;

	int tx = 1;
	int ty = 1;

	for (int i = 0; i < sum; i++)
	{
		//going horizontally is shorter?
		if (Y == 0 || 1.f * tx / X < 1.f * ty / Y)
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

void Ship::RayTracingGetPath(WaterTile* tileA, WaterTile* tileB)
{
	m_current_path.clear();

	int coord_x = tileA->m_coord_x;
	int coord_y = tileA->m_coord_y;

	//Bresenham line-drawing algorithm
	int X = abs(tileA->m_coord_x - tileB->m_coord_x) + 1;
	int Y = abs(tileA->m_coord_y - tileB->m_coord_y) + 1;
	int sum = X + Y - 2;

	int tx = 1;
	int ty = 1;

	for (int i = 0; i < sum; i++)
	{
		//going horizontally is shorter?
		if (Y == 0 || 1.f * tx / X < 1.f * ty / Y)
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
			tx++;
			ty++;
			i++;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
		}

		WaterTile* tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y];
		m_current_path.push_back(tile);
	}
}

bool Ship::RayTracingContainsIslandForPathfind(WaterTile* tileA, WaterTile* tileB)
{
	int coord_x = tileA->m_coord_x;
	int coord_y = tileA->m_coord_y;

	//Bresenham line-drawing algorithm
	int X = abs(tileA->m_coord_x - tileB->m_coord_x) + 1;
	int Y = abs(tileA->m_coord_y - tileB->m_coord_y) + 1;
	int sum = X + Y - 2;

	int tx = 1;
	int ty = 1;

	//RAY TRACING
	for (int i = 0; i < sum; i++)
	{
		//PRELIMINARY CHECKS
		if (tileA->m_coord_x < tileB->m_coord_x)//going right, checking right
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x + 1][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x + 1][coord_y]->m_can_be_seen == false)
			{
				printf("prelim check right: NOK %d, %d\n", coord_x + 1, coord_y);
				return true;
			}
		}
		if (tileA->m_coord_x > tileB->m_coord_x)//going left, checking left
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x - 1][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x - 1][coord_y]->m_can_be_seen == false)
			{
				printf("prelim check left: NOK %d, %d\n", coord_x - 1, coord_y);
				return true;
			}
		}
		if (tileA->m_coord_y < tileB->m_coord_y)//going up, checking up
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y + 1]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y + 1]->m_can_be_seen == false)
			{
				printf("prelim check up: NOK %d, %d\n", coord_x, coord_y + 1);
				return true;
			}
		}
		if (tileA->m_coord_y > tileB->m_coord_y)//going down, checking down
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y - 1]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y - 1]->m_can_be_seen == false)
			{
				printf("prelim check down: NOK %d, %d\n", coord_x, coord_y - 1);
				return true;
			}
		}

		//going horizontally is shorter?
		if (Y == 0 || 1.f * tx / X < 1.f * ty / Y)
		{
			tx++;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
			printf("going horizontal: %d, %d\n", coord_x, coord_y);

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED\n", coord_x, coord_y);
				return true;
			}

		}//going vertically is shorter?
		else if (X == 0 || 1.f * tx / X > 1.f * ty / Y)
		{
			ty++;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
			printf("going vertical: %d, %d\n", coord_x, coord_y);

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED: %d, %d\n", coord_x, coord_y);
				return true;
			}
		}
		else//perfect diagonal
		{
			tx++;
			ty++;
			i++;
			printf("going diagonal: %d, %d\n", coord_x, coord_y);
			int old_coord_x = coord_x;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED: %d, %d\n", coord_x, coord_y);
				return true;
			}

			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED: %d, %d\n", coord_x, coord_y);
				return true;
			}

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[old_coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[old_coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED: %d, %d\n", coord_x, coord_y);
				return true;
			}
		}
	}

	return false;
}


int Ship::GetDistanceToWaterTile(WaterTile* tile)
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

float Ship::GetDistanceSquaredInSecondsDMS(WaterTile* tile)
{
	float xA = m_DMS.m_minute_x * NB_WATERTILE_SUBDIVISION + m_DMS.m_second_x;
	float yA = m_DMS.m_minute_y * NB_WATERTILE_SUBDIVISION + m_DMS.m_second_y;

	float xB = tile->m_DMS.m_minute_x * NB_WATERTILE_SUBDIVISION + tile->m_DMS.m_second_x;
	float yB = tile->m_DMS.m_minute_y * NB_WATERTILE_SUBDIVISION + tile->m_DMS.m_second_y;

	float distance_squared = (xA - xB) * (xA - xB) + (yA - yB) * (yA - yB);

	return distance_squared;
}

float Ship::GetDistanceFloatToWaterTile(WaterTile* tile)
{
	int diff_x = NB_WATERTILE_SUBDIVISION * (tile->m_coord_x - m_DMS.m_minute_x) - m_DMS.m_second_x;
	int diff_y = NB_WATERTILE_SUBDIVISION * (tile->m_coord_y - m_DMS.m_minute_y) - m_DMS.m_second_y;
	float distance_f = sqrt(diff_x * diff_x + diff_y * diff_y) * 1.f / NB_WATERTILE_SUBDIVISION;

	return distance_f;
}

bool Ship::CanViewWaterTile(WaterTile* tile)
{
	if (tile->m_coord_x > m_DMS.m_minute_x + NB_WATERTILE_VIEW_RANGE + 1 || tile->m_coord_x < m_DMS.m_minute_x - NB_WATERTILE_VIEW_RANGE - 1)
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
		return true;
		//line of sight blocked by islands
		//if (RayTracingContainsIsland(m_tile, tile) == false)
		//{
		//	return true;
		//}
		//else
		//{
		//	return false;
		//}
	}
	else
	{
		return false;
	}
}

bool Ship::AddResource(ResourceType resource, int value)
{
	if (value == 0)
	{
		return false;
	}

	if (resource != Resource_Fidelity)
	{
		m_resources[resource] = Max(0, m_resources[resource] + value);
	}
	else
	{
		for (int j = 0; j < 2; j++)
		{
			for (vector<CrewMember*>::iterator it = m_crew[j].begin(); it != m_crew[j].end(); it++)
			{
				(*it)->m_fidelity += value;
				Bound((*it)->m_fidelity, sf::Vector2i(0, 100));
			}
		}
	}

	return true;
}

void Ship::PayUpkeepCost(int days)
{
	//pay upkeep cost crew member by crew member to know who starves, who wins or loses fidelity individually
	for (int k = 0; k < days; k++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (vector<CrewMember*>::iterator it = m_crew[j].begin(); it != m_crew[j].end(); it++)
			{
				bool has_lost_fidelity = false;
				for (int i = 0; i < NB_UPKEEP_COSTS; i++)
				{
					if (m_resources[i] >= (*it)->m_upkeep_cost[i])
					{
						//pay the cost of crew upkeep
						AddResource((ResourceType)i, -(*it)->m_upkeep_cost[i]);
					}
					else if (has_lost_fidelity == false)
					{
						//pay what you can...
						m_resources[i] = 0;

						//...then lose crew fidelity
						(*it)->m_fidelity = Max(0, (*it)->m_fidelity - 1);
						has_lost_fidelity = true;
					}
				}

				//cost paid fully = earn crew fidelity
				if (has_lost_fidelity == false)
				{
					(*it)->m_fidelity = Min(100, (*it)->m_fidelity + 1);
				}
			}
		}
	}
}