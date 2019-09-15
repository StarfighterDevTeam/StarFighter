#include "EscortShip.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
EscortShip::EscortShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe, AIShip* escorted_ship, sf::Vector2f escort_offset) : AIShip(ship_type, sector_index, heading, hostility, roe)
{
	m_escorted_ship = escorted_ship;
	m_escorted_offset = escort_offset;

	m_scripted_destination = new sf::Vector2f(0, 0);

}

EscortShip::~EscortShip()
{

}

void EscortShip::Update(sf::Time deltaTime)
{
	if (m_escorted_ship != NULL)
	{
		//position offset to escorted ship
		sf::Vector2f offset_vector;
		offset_vector.x = cos(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.y + sin(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.x;
		offset_vector.y = sin(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.y - cos(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.x;;

		//sf::Vector2f destination_vector = sf::Vector2f(m_escorted_ship->m_move_destination.x - m_escorted_ship->m_position.x, m_escorted_ship->m_move_destination.y - m_escorted_ship->m_position.y);
		//sf::Vector2f offset_vector = sf::Vector2f(destination_vector.y, -destination_vector.x);
		//ScaleVector(&offset_vector, 200);
		*m_scripted_destination = m_escorted_ship->m_position + offset_vector;
	}

	AIShip::Update(deltaTime);
}

void EscortShip::SetROE(RuleOfEngagement roe)
{
	SpatialObject::SetROE(roe);
}