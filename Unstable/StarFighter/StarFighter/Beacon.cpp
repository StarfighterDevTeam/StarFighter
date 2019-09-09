#include "Beacon.h"

extern Game* CurrentGame;

using namespace sf;

Beacon::Beacon(sf::Vector2i sector_index) : SpatialObject()
{
	(*CurrentGame).SetStarSectorIndex(this, sector_index, false);
	m_visible = false;
	m_collider = Beacon_Object;
	m_layer = BackgroundLayer;
	m_size = sf::Vector2f(138, 138);
}

Beacon::~Beacon()
{

}