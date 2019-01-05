#include "Seaport.h"

extern Game* CurrentGame;

Seaport::Seaport(int coord_x, int coord_y, int zone_coord_x, int zone_coord_y, SeaportType type) : GameEntity(sf::Vector2f(WATERTILE_SIZE, WATERTILE_SIZE), UI_Seaport)
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;
	m_type = type;

	m_tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[coord_x][coord_y];
	m_tile->m_seaport = this;

	//UI
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	Texture* texture = loader->getTexture("2D/seaport_icon.png");
	setAnimation(texture, 1, 1);

	//m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	//m_text.setCharacterSize(20);
	//m_text.setColor(sf::Color::Black);
	//m_text.setString("Small port");
}

Seaport::~Seaport()
{
	
}

void Seaport::RemoveShip(Warship* ship)
{
	vector<Warship*> old_vector;

	for (vector<Warship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		old_vector.push_back(ship);
	}

	m_ships.clear();

	for (vector<Warship*>::iterator it = old_vector.begin(); it != old_vector.end(); it++)
	{
		if (*it != ship)
		{
			m_ships.push_back(*it);
		}
	}
}