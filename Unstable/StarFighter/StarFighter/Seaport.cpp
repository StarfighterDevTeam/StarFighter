#include "Seaport.h"

extern Game* CurrentGame;

Seaport::Seaport(int coord_x, int coord_y, int zone_coord_x, int zone_coord_y, SeaportType seaport_type) : Location(Location_Seaport, (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[coord_x][coord_y])
{
	m_tile->m_location = this;
	m_known = true;
	m_seaport_type = seaport_type;

	sf::Texture* texture;
	switch (seaport_type)
	{
		case Seaport_Small:
		{
			m_choicesID[0] = 1;
			m_choicesID[1] = 2;
			m_choicesID[2] = 3;
			m_choicesID[3] = 13;

			texture = TextureLoader::getInstance()->loadTexture("2D/seaportS_icon.png", (int)WATERTILE_SIZE, (int)WATERTILE_SIZE);
			break;
		}
		case Seaport_Medium:
		{
			m_choicesID[0] = 1;
			m_choicesID[1] = 2;
			m_choicesID[2] = 3;
			m_choicesID[3] = 13;

			texture = TextureLoader::getInstance()->loadTexture("2D/seaportM_icon.png", (int)WATERTILE_SIZE, (int)WATERTILE_SIZE);
			break;
		}
		case Seaport_Large:
		{
			m_choicesID[0] = 1;
			m_choicesID[1] = 14;
			m_choicesID[2] = 3;
			m_choicesID[3] = 13;

			texture = TextureLoader::getInstance()->loadTexture("2D/seaportL_icon.png", (int)WATERTILE_SIZE, (int)WATERTILE_SIZE);
			break;
		}
	}
	setAnimation(texture, 1, 1);
	
	//UI
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setColor(sf::Color::Black);

	//get a name at random in the list
	m_display_name = (*CurrentGame).GetRandomNameFromDico((*CurrentGame).m_dico_islands_names, (*CurrentGame).m_dico_islands_names_used);
	m_text.setString(m_display_name);
}

Seaport::~Seaport()
{
	delete m_island;
}

void Seaport::RemoveShip(Ship* ship)
{
	vector<Ship*> old_vector;

	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		old_vector.push_back(ship);
	}

	m_ships.clear();

	for (vector<Ship*>::iterator it = old_vector.begin(); it != old_vector.end(); it++)
	{
		if (*it != ship)
		{
			m_ships.push_back(*it);
		}
	}
}

Island* Seaport::AddIsland(int upcorner_x, int upcorner_y, int width, int height, int skin)
{
	m_island = new Island(upcorner_x, upcorner_y, width, height, m_tile->m_DMS.m_degree_x, m_tile->m_DMS.m_degree_y, skin, this);

	return m_island;
}