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

Room::Room(int upcorner_x, int upcorner_y, int width, int height, RoomType type)
{
	m_type = type;

	m_position.x = ROOMTILE_OFFSET_X + (2.f * upcorner_x + width - 1) * 0.5f * ROOMTILE_SIZE;
	m_position.y = ROOMTILE_OFFSET_Y + (2.f * upcorner_y + height - 1) * 0.5f * ROOMTILE_SIZE;

	//create tiles
	for (int y = upcorner_y; y < upcorner_y + height; y++)
	{
		for (int x = upcorner_x; x < upcorner_x + width; x++)
		{
			(*CurrentGame).m_tiles.push_back(RoomTile(x, y, this));
		}
	}

	//UI
	m_shape_container.setPosition(m_position);
	m_shape_container.setSize(sf::Vector2f(ROOMTILE_SIZE * width, ROOMTILE_SIZE * height));
	m_shape_container.setOrigin(sf::Vector2f(ROOMTILE_SIZE * width * 0.5f, ROOMTILE_SIZE * height * 0.5f));
	m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));
	m_shape_container.setOutlineThickness(2.f);
	m_shape_container.setOutlineColor(sf::Color::Magenta);

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setColor(sf::Color::White);
	m_text.setString(dico_rooms[type]);
	m_text.SetPosition(m_position);
}

Room::~Room()
{
	
}