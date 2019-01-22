#include "Room.h"

extern Game* CurrentGame;

string dico_rooms[NB_ROOM_TYPES] = {
	"",					//Room_Empty,
	"Crew Quarters",	//Room_Crewquarter,
	"Navigation",		//Room_Navigation,
	"Weapon",			//Room_Weapon,
	"Gold",				//Room_Gold,
	"Fishing",			//Room_Fishing,
	"Kitchen",			//Room_Kitchen,
	"Relic",			//Room_Relic,
	"Sword",			//Room_Sword,
	"Life boats",		//Room_Lifeboat,
	"Ammo",				//Room_Ammo,
	"Engine",			//Room_Engine,
	"Mecha",			//Room_Mecha
};

Room::Room(int upcorner_x, int upcorner_y, int width, int height, RoomType type, bool minimized) : GameEntity(UI_Room)
{
	m_upcorner_x = upcorner_x;
	m_upcorner_y = upcorner_y;
	m_width = width;
	m_height = height;
	m_type = type;
	m_UI_type = UI_Room;
	m_nb_crew_max = width * height / 4;
	m_is_flooded = false;

	float size = minimized == false ? ROOMTILE_SIZE : ROOMTILE_MINI_SIZE;
	float offset_x = minimized == false ? ROOMTILE_OFFSET_X : ROOMTILE_MINI_OFFSET_X;
	float offset_y = minimized == false ? ROOMTILE_OFFSET_Y : ROOMTILE_MINI_OFFSET_Y;

	m_size = sf::Vector2f(size * width, size * height);
	m_position.x = offset_x + (-0.5f + upcorner_x + width * 0.5f) * size;
	m_position.y = offset_y + (-0.5f + upcorner_y + height * 0.5f) * size;

	//create tiles
	for (int y = upcorner_y; y < upcorner_y + height; y++)
	{
		for (int x = upcorner_x; x < upcorner_x + width; x++)
		{
			RoomTile* tile = new RoomTile(x, y, this, size, minimized);
			m_tiles.push_back(tile);

			if (minimized == false)
			{
				(*CurrentGame).m_tiles.push_back(tile);
			}
			else
			{
				(*CurrentGame).m_enemy_tiles.push_back(tile);
			}
		}
	}

	//init crew count
	for (int i = 0; i < NB_CREW_TYPES; i++)
	{
		m_nb_crew[i] = 0;
		m_nb_crew_working[i] = 0;
	}

	//UI
	m_shape_container.setPosition(m_position);//264, 244
	m_shape_container.setSize(m_size);
	m_shape_container.setOrigin(sf::Vector2f(m_size.x * 0.5f, m_size.y * 0.5f));
	m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));
	m_shape_container.setOutlineThickness(2.f);
	m_shape_container.setOutlineColor(sf::Color::White);
	m_default_color = sf::Color::White;

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setColor(sf::Color::White);
	m_text.setString(dico_rooms[type]);
	m_text.SetPosition(m_position);
}

Room::~Room()
{
	for (vector<RoomTile*>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
	{
		delete *it;
	}
}

void Room::Update(Time deltaTime)
{
	ostringstream ss;
	ss << dico_rooms[m_type] << "\n(" << m_nb_crew[Crew_All] << "/" << m_nb_crew_max << ")";
	m_text.setString(ss.str());

	GameEntity::Update(deltaTime);
}

bool Room::IsConnectedToRoom(Room* room)
{
	for (vector<RoomConnexion*>::iterator it = m_connexions.begin(); it != m_connexions.end(); it++)
	{
		if (((*it)->m_tiles.first->m_room == this && ((*it)->m_tiles.second->m_room == room)) || ((*it)->m_tiles.first->m_room == room && ((*it)->m_tiles.second->m_room == this)))
		{
			return true;
		}
	}

	return false;
}

bool Room::IsConnectedToRoomTile(RoomTile* tileA, RoomTile* tileB)
{
	const int tileA_x = tileA->m_coord_x;
	const int tileA_y = tileA->m_coord_y;
	const int tileB_x = tileB->m_coord_x;
	const int tileB_y = tileB->m_coord_y;

	//adjacent
	if ((abs(tileA_x - tileB_x) == 1 && abs(tileA_y - tileB_y) == 0) || (abs(tileA_x - tileB_x) == 0 && abs(tileA_y - tileB_y) == 1))
	{
		//same room ...
		if (tileA->m_room == tileB->m_room)
		{
			return true;
		}

		//...or connected by an unlocked door
		for (vector<RoomConnexion*>::iterator it = tileA->m_room->m_connexions.begin(); it != tileA->m_room->m_connexions.end(); it++)
		{
			if ((*it)->m_locked == false && (((*it)->m_tiles.first == tileA && ((*it)->m_tiles.second == tileB)) || ((*it)->m_tiles.first == tileB && ((*it)->m_tiles.second == tileA))))
			{
				return true;
			}
		}
	}

	return false;
}