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
	//position
	if (m_destination == NULL)
	{
		m_position = m_tile->m_position;
	}

	//arrived at destination?
	if (m_destination != NULL)
	{
		if (GetDistanceFloatToWaterTile(m_destination) < 1.f / NB_WATERTILE_SUBDIVISION)
		{
			SetDMSCoord(m_destination->m_DMS);
			m_destination = NULL;
			m_speed = sf::Vector2f(0, 0);
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

	//UI
	ostringstream ss;
	ss << std::fixed;
	ss.precision(0);
	ss << "\n\n\n";
	ss << m_DMS.m_degree_y << "�" << m_DMS.m_minute_y << "' ";
	ss << m_DMS.m_second_y << "\"\N";
	ss << "\n";
	ss << m_DMS.m_degree_x << "�" << m_DMS.m_minute_x << "' ";
	ss << m_DMS.m_second_x << "\"\E";
	m_text.setString(ss.str());

	GameEntity::Update(deltaTime);
}

float Warship::GetAngleForVector(sf::Vector2f vector)
{
	float angle = 0.f;
	if (vector.x != 0 || vector.y != 0)
	{
		if (vector.x == 0)
		{
			angle = vector.y >= 0 ? 180.f : 0.f;
		}
		else if (vector.y == 0)
		{
			angle = vector.x >= 0 ? 90.f : 270.f;
		}
		else
		{
			if (vector.x >= 0)
			{
				angle = (atan(vector.y / vector.x) * 180.f / M_PI) + 90.f;
			}
			else
			{
				angle = (atan(vector.y / vector.x) * 180.f / M_PI) + 90.f + 180.f;
			}
		}
	}

	return angle;
}

void Warship::UpdateRotation()
{
	//find angle for speed vector
	if (m_speed.x != 0 || m_speed.y != 0)
	{
		m_angle = GetAngleForVector(m_speed);
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

	m_tile = tile;
	m_DMS = coord;
	m_zone = tile->m_zone;

	//new seaport?
	if (tile->m_seaport != NULL && tile != (WaterTile*)m_seaport)
	{
		m_seaport = tile->m_seaport;
		m_seaport->m_ships.push_back(this);
	}

	return true;
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

	float distance = GetDistanceFloatToWaterTile(tile);
	if (distance <= NB_WATERTILE_VIEW_RANGE + 0.15f || (tile->m_can_be_seen == true && m_destination != NULL && distance <= NB_WATERTILE_VIEW_RANGE + 0.99f))//custom round-up system
	{
		return true;
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

	if (CheckPathfindToWaterTile(tile) == true)
	{
		printf("Pathfind NOT ok\n");
		return false;
	}
	else
	{
		printf("Pathfind OK\n");
	}

	sf::Vector2f move_vector = tile->m_position - m_position;
	GameObject::ScaleVector(&move_vector, CRUISE_SPEED);
	m_speed = move_vector;

	m_destination = tile;

	//leaving port?
	if (m_seaport != NULL && m_destination != (WaterTile*)m_seaport)
	{
		m_seaport->RemoveShip(this);
		m_seaport = NULL;
	}

	return true;
}

bool Warship::CheckPathfindToWaterTile(WaterTile* tile)
{
	sf::Vector2f move_vector = tile->m_position - m_position;
	float angle = GetAngleForVector(move_vector) * M_PI / 180.f;

	float x = sin(angle) * WATERTILE_SIZE;
	float y = cos(angle) * WATERTILE_SIZE;

	//angle ranges from 0 to 2*PI
	float eps = 0.001;
	if (angle >= M_PI_4 - eps && angle <= 3 * M_PI_4 + eps)
	{
		x = WATERTILE_SIZE;
	}
	else if (angle >= 5 * M_PI_4 - eps && angle <= 7 * M_PI_4 + eps)
	{
		x = - WATERTILE_SIZE;
	}
	
	if (angle <= M_PI_4 + eps || angle >= 7 * M_PI_4 - eps)
	{
		y = - WATERTILE_SIZE;
	}
	else if (angle >= 3 * M_PI_4 - eps && angle <= 5 * M_PI_4 + eps)
	{
		y = WATERTILE_SIZE;
	}
	
	
	//get positions to check (1 in orthogonal move, 3 in diagonal move)
	vector<sf::Vector2f> positions_to_check;

	sf::Vector2f positionA;
	if (move_vector.x == 0 && move_vector.y < 0)
	{
		positionA = sf::Vector2f(m_position.x, m_position.y - WATERTILE_SIZE);//up
	}
	else if (move_vector.x > 0 && move_vector.y == 0)
	{
		positionA = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y);//right
	}
	else if (move_vector.x == 0 && move_vector.y > 0)
	{
		positionA = sf::Vector2f(m_position.x, m_position.y + WATERTILE_SIZE);//down
	}
	else if (move_vector.x < 0 && move_vector.y == 0)
	{
		positionA = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y);//left
	}
	else if (x >= 0 && y <= 0)//up-right
	{
		positionA = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y);//right
		sf::Vector2f positionB = sf::Vector2f(m_position.x, m_position.y - WATERTILE_SIZE);//up
		sf::Vector2f positionC = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y - WATERTILE_SIZE);//up-right
		positions_to_check.push_back(positionB);
		positions_to_check.push_back(positionC);
	}
	else if (x >= 0 && y >= 0)//down-right
	{
		positionA = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y);//right
		sf::Vector2f positionB = sf::Vector2f(m_position.x, m_position.y + WATERTILE_SIZE);//down
		sf::Vector2f positionC = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y + WATERTILE_SIZE);//down-right
		positions_to_check.push_back(positionB);
		positions_to_check.push_back(positionC);
	}
	else if (x <= 0 && y >= 0)//down-left
	{
		positionA = sf::Vector2f(m_position.x, m_position.y + WATERTILE_SIZE);//down
		sf::Vector2f positionB = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y);//left
		sf::Vector2f positionC = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y + WATERTILE_SIZE);//down-left
		positions_to_check.push_back(positionB);
		positions_to_check.push_back(positionC);
	}
	else if (x <= 0 && y <= 0)//up-left
	{
		positionA = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y);//left
		sf::Vector2f positionB = sf::Vector2f(m_position.x, m_position.y - WATERTILE_SIZE);//up
		sf::Vector2f positionC = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y - WATERTILE_SIZE);//up-left
		positions_to_check.push_back(positionB);
		positions_to_check.push_back(positionC);
	}
	//if (move_vector.x == 0 && move_vector.y < 0)
	//{
	//	positionA = sf::Vector2f(m_position.x, m_position.y - WATERTILE_SIZE);//up
	//}
	//else if (move_vector.x > 0 && move_vector.y == 0)
	//{
	//	positionA = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y);//right
	//}
	//else if (move_vector.x == 0 && move_vector.y > 0)
	//{
	//	positionA = sf::Vector2f(m_position.x, m_position.y + WATERTILE_SIZE);//down
	//}
	//else if (move_vector.x < 0 && move_vector.y == 0)
	//{
	//	positionA = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y);//left
	//}
	//else if (angle >= 0 && angle < M_PI_2)//up-right
	//{
	//	positionA = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y);//right
	//	sf::Vector2f positionB = sf::Vector2f(m_position.x, m_position.y - WATERTILE_SIZE);//up
	//	sf::Vector2f positionC = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y - WATERTILE_SIZE);//up-right
	//	positions_to_check.push_back(positionB);
	//	positions_to_check.push_back(positionC);
	//}
	//else if (angle > M_PI_2 && angle < 2 * M_PI_2)//down-right
	//{
	//	positionA = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y);//right
	//	sf::Vector2f positionB = sf::Vector2f(m_position.x, m_position.y + WATERTILE_SIZE);//down
	//	sf::Vector2f positionC = sf::Vector2f(m_position.x + WATERTILE_SIZE, m_position.y + WATERTILE_SIZE);//down-right
	//	positions_to_check.push_back(positionB);
	//	positions_to_check.push_back(positionC);
	//}
	//else if (angle > 2 * M_PI_2 && angle < 3 * M_PI_2)//down-left
	//{
	//	positionA = sf::Vector2f(m_position.x, m_position.y + WATERTILE_SIZE);//down
	//	sf::Vector2f positionB = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y);//left
	//	sf::Vector2f positionC = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y + WATERTILE_SIZE);//down-left
	//	positions_to_check.push_back(positionB);
	//	positions_to_check.push_back(positionC);
	//}
	//else if (angle > 2 * M_PI_2 && angle < 3 * M_PI_2)//up-left
	//{
	//	positionA = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y);//left
	//	sf::Vector2f positionB = sf::Vector2f(m_position.x, m_position.y - WATERTILE_SIZE);//up
	//	sf::Vector2f positionC = sf::Vector2f(m_position.x - WATERTILE_SIZE, m_position.y - WATERTILE_SIZE);//up-left
	//	positions_to_check.push_back(positionB);
	//	positions_to_check.push_back(positionC);
	//}
	positions_to_check.push_back(positionA);

	//Collision test
	bool colliding = false;
	for (vector<sf::Vector2f>::iterator it = positions_to_check.begin(); it != positions_to_check.end(); it++)
	{
		colliding = IsPositionCollidingWithIsland(*it);
		//colliding = IsPositionCollidingWithIsland(sf::Vector2f(m_position.x + x, m_position.y + y), *it);
		if (colliding == true)
		{
			break;
		}
	}

	return colliding;
}

bool Warship::IsPositionCollidingWithIsland(sf::Vector2f position)
{
	DMS_Coord dms = GetDMSCoord(position);
	int coord_x = dms.m_minute_x;
	int coord_y = dms.m_minute_y;

	//out of bounds?
	if (coord_x < 0 || coord_x > NB_WATERTILE_X - 1 || coord_y < 0 || coord_y > NB_WATERTILE_Y - 1)
	{
		return false;
	}

	//TEST all 8 tiles around
	vector<WaterTile*> candidates;
	WaterTile* tileA = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x][coord_y];
	if (tileA->m_type != Water_Empty)
	{
		return true;
		candidates.push_back(tileA);
	}
	else
	{
		return false;
	}

	////right
	//if (coord_x < NB_WATERTILE_Y - 1)
	//{
	//	WaterTile* tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x + 1][coord_y];
	//	if (tile->m_type != Water_Empty)
	//	{
	//		candidates.push_back(tile);
	//	}
	//
	//	//up-right
	//	if (coord_y < NB_WATERTILE_Y - 1)
	//	{
	//		WaterTile* tile2 = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x + 1][coord_y + 1];
	//		if (tile2->m_type != Water_Empty)
	//		{
	//			candidates.push_back(tile2);
	//		}
	//	}
	//
	//	//down-right
	//	if (coord_y > 0)
	//	{
	//		WaterTile* tile2 = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x + 1][coord_y - 1];
	//		if (tile2->m_type != Water_Empty)
	//		{
	//			candidates.push_back(tile2);
	//		}
	//	}
	//}
	////left
	//if (coord_x > 0)
	//{
	//	WaterTile* tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x - 1][coord_y];
	//	if (tile->m_type != Water_Empty)
	//	{
	//		candidates.push_back(tile);
	//	}
	//
	//	//up-left
	//	if (coord_y < NB_WATERTILE_Y - 1)
	//	{
	//		WaterTile* tile2 = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x - 1][coord_y + 1];
	//		if (tile2->m_type != Water_Empty)
	//		{
	//			candidates.push_back(tile2);
	//		}
	//	}
	//
	//	//down-left
	//	if (coord_y > 0)
	//	{
	//		WaterTile* tile2 = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x - 1][coord_y - 1];
	//		if (tile2->m_type != Water_Empty)
	//		{
	//			candidates.push_back(tile2);
	//		}
	//	}
	//}
	////up
	//if (coord_y < NB_WATERTILE_Y - 1)
	//{
	//	WaterTile* tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x][coord_y + 1];
	//	if (tile->m_type != Water_Empty)
	//	{
	//		candidates.push_back(tile);
	//	}
	//}
	////down
	//if (coord_y > 0)
	//{
	//	WaterTile* tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_x]->m_watertiles[coord_x][coord_y - 1];
	//	if (tile->m_type != Water_Empty)
	//	{
	//		candidates.push_back(tile);
	//	}
	//}

	//collision test (square into square?)
	vector<WaterTile*> collisions;
	for (vector<WaterTile*>::iterator it = candidates.begin(); it != candidates.end(); it++)
	{
		float xA = position.x - WATERTILE_SIZE * 0.5f;
		float yA = position.y - WATERTILE_SIZE * 0.5f;
		float xA2 = position.x + WATERTILE_SIZE * 0.5f;
		float yA2 = position.y + WATERTILE_SIZE * 0.5f;
		float xB = (*it)->m_position.x - WATERTILE_SIZE * 0.5f;
		float yB = (*it)->m_position.y - WATERTILE_SIZE * 0.5f;
		float xB2 = (*it)->m_position.x + WATERTILE_SIZE * 0.5f;
		float yB2 = (*it)->m_position.y + WATERTILE_SIZE * 0.5f;

		if ((xA > xB && xA < xB2 && yA > yB && yA < yB2)
			|| (xA > xB && xA < xB2 && yA2 > yB && yA2 < yB2)
			|| (xA2 > xB && xA2 < xB2 && yA > yB && yA < yB2)
			|| (xA2 > xB && xA2 < xB2 && yA2 > yB && yA2 < yB2)
			|| (xA == xB && xA2 == xB2 && yA == yB))
		{
			collisions.push_back(*it);
			//return true;
		}
	}

	return (collisions.empty() == false);
}