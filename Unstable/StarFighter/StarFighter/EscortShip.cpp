#include "EscortShip.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
EscortShip::EscortShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe, AIShip* escorted_ship, sf::Vector2f escort_offset) : AIShip(ship_type, sector_index, heading, hostility, roe)
{
	m_escorted_ship = escorted_ship;
	m_escorted_offset = escort_offset;

	sf::Vector2f offset_vector;
	offset_vector.x = cos(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.y + sin(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.x;
	offset_vector.y = sin(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.y - cos(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.x;
	m_scripted_destination = new sf::Vector2f(m_escorted_ship->m_position + offset_vector);
	m_position = *m_scripted_destination;
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

		*m_scripted_destination = m_escorted_ship->m_position + offset_vector;
		m_move_destination = *m_scripted_destination;

		float speed_max = MaxBetweenValues(20, GetVectorLength(m_speed));
		if (abs(m_position.x - m_move_destination.x) > 32 || abs(m_position.y - m_move_destination.y) > 32)
		{
			GoTo(m_move_destination, deltaTime, m_inputs_direction);
			TurnTo(m_move_destination, deltaTime, m_inputs_direction);//look to a projection of the scripted position, rather than the position around the escorted ship
		}
		else
		{
			TurnTo(*m_escorted_ship->m_scripted_destination + offset_vector, deltaTime, m_inputs_direction);//look to a projection of the scripted position, rather than the position around the escorted ship
		}

		//escorted ship is dying?
		if (m_escorted_ship->m_garbageMe == true)
			m_escorted_ship = NULL;
	}

	AIShip::Update(deltaTime);
}

void EscortShip::SetROE(RuleOfEngagement roe)
{
	AIShip::SetROE(roe);

	if (m_escorted_ship != NULL)
		if (roe == ROE_FireAtWill)
			m_escorted_ship->SetROE(ROE_Freeze);
}

AIShip* EscortShip::GetEscortedShip()
{
	return m_escorted_ship;
}

bool EscortShip::CheckMarkingConditions()
{
	return m_escorted_ship != NULL && m_escorted_ship->m_marker_mission != NULL;
}