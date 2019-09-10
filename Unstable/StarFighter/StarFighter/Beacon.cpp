#include "Beacon.h"

extern Game* CurrentGame;

using namespace sf;

Beacon::Beacon(sf::Vector2i sector_index) : SpatialObject()
{
	(*CurrentGame).SetStarSectorIndex(this, sector_index);
	m_visible = false;
	m_collider = BeaconObject;
	m_layer = BackgroundLayer;
	m_size = sf::Vector2f(138, 138);
}

Beacon::~Beacon()
{
	for (AIShip* ship : m_ships_to_create)
		delete ship;
}