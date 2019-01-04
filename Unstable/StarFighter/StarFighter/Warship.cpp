#include "Warship.h"

extern Game* CurrentGame;

Warship::Warship() : GameEntity(UI_Warship)
{
	m_angle = 90.f;

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

void Warship::InitWarship()
{
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

	//crew
	CrewMember* crew = new CrewMember(Crew_Pirate);
	AddCrewMember(crew, room);

	CrewMember* crew2 = new CrewMember(Crew_Civilian);
	AddCrewMember(crew2, room2);

	//crew->MoveToRoom(room2);
	//crew2->MoveToRoom(room);

	//assign to world map
	SetDMSCoord(DMS_Coord(0, 6, 0, 0, 6, 0));
}

void Warship::Update(Time deltaTime)
{
	//rotation
	UpdateRotation();

	GameEntity::Update(deltaTime);
}

void Warship::UpdateRotation()
{
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
	m_position = sf::Vector2f(m_tile->m_position.x - WATERTILE_SIZE * 0.5f, m_tile->m_position.y - WATERTILE_SIZE * 0.5f);

	return true;
}

int Warship::GetDistanceToWaterTile(WaterTile* tile)
{
	int diff_x = tile->m_coord_x - m_DMS.m_minute_x;
	int diff_y = tile->m_coord_y - m_DMS.m_minute_y;
	float distance_f = sqrt(diff_x * diff_x + diff_y * diff_y);

	int distance_i = (int)distance_f;

	if (distance_f - distance_i > 0.15f)//custom round-up rule
	{
		distance_i++;
	}

	return distance_i;
}

bool Warship::CanViewWaterTile(WaterTile* tile)
{
	if (tile->m_zone != m_zone)
	{
		return false;
	}

	int distance = GetDistanceToWaterTile(tile);

	return distance <= NB_WATERTILE_VIEW_RANGE;
}

