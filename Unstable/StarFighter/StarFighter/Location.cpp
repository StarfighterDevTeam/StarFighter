#include "Location.h"

extern Game* CurrentGame;

Location::Location(LocationType type, WaterTile* tile) : GameEntity(sf::Vector2f(WATERTILE_SIZE, WATERTILE_SIZE), UI_Location)
{
	m_tile = tile;
	m_type = type;
	m_visited_countdown = 0;
}

Location::~Location()
{

}