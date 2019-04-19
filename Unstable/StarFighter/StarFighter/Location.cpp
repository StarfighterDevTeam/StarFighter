#include "Location.h"

extern Game* CurrentGame;

Location::Location(LocationType type, WaterTile* tile) : GameEntity(sf::Vector2f(WATERTILE_SIZE, WATERTILE_SIZE), UI_Location)
{
	m_tile = tile;
	m_type = type;
	m_visited_countdown = 0;

	//Add scenariis (choices)
	switch (type)
	{
		case Location_Seaport:
		{
			m_choicesID[0] = 1;
			m_choicesID[1] = 2;
			m_choicesID[2] = 3;
			m_choicesID[3] = -1;
			break;
		}

		case Location_SeaMonster:
		case Location_Wreck:
		{
			m_choicesID[0] = 4;
			m_choicesID[1] = 5;
			m_choicesID[2] = -1;
			m_choicesID[3] = -1;
			break;
		}
	}
	
}

Location::~Location()
{

}