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
	offset_vector.y = sin(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.y - cos(-m_escorted_ship->m_heading * M_PI / 180 + M_PI_2) * m_escorted_offset.x;;
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

		sf::Vector2f inputs_direction = sf::Vector2f(0, 0);

		float speed_max = MaxBetweenValues(20, GetVectorLength(m_speed));
		if (GetDistanceSquaredBetweenPositions(m_position, *m_scripted_destination) < speed_max * speed_max)
		{
			printf("speed: %f, TURN TO\n", GetVectorLength(m_speed));
			TurnTo(*m_escorted_ship->m_scripted_destination + offset_vector, deltaTime, inputs_direction);
		}
		else
		{
			printf("speed: %f, GOTO\n", GetVectorLength(m_speed));
			GoTo(*m_scripted_destination, deltaTime, inputs_direction);
		}

		//apply inputs
		ApplyFlightModel(deltaTime, inputs_direction);

		//update marking
		if (m_hostility == Hostility_Ally && m_escorted_ship->m_marker_mission != NULL && m_marker_target == NULL)
			(*CurrentGame).m_playerShip->MarkThis(this, false);
		else if (m_hostility == Hostility_Ally && m_escorted_ship->m_marker_mission == NULL && m_marker_target != NULL)
			(*CurrentGame).m_playerShip->UnmarkThis(this, false);
	}

	Ship::Update(deltaTime);
}

void EscortShip::SetROE(RuleOfEngagement roe)
{
	SpatialObject::SetROE(roe);
}