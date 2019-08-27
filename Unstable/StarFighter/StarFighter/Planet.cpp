#include "Planet.h"

extern Game* CurrentGame;

using namespace sf;

Planet::Planet(PlanetType planet_type, sf::Vector2i sector_index, HostilityLevel hostility)
{
	m_planet_type = planet_type;
	m_hostility = hostility;

	ColliderType collider = BackgroundObject;

	SetStarSectorIndex(sector_index);

	Init(m_position, sf::Vector2f(0, 0) , "2D/planet.png", sf::Vector2f(138, 138), 1, 17);
	SetAnimationLine((int)planet_type);
	m_heading = 0;

	//update position and rotation "manually" because they won't be updated during the frame of their creation
	setPosition(sf::Vector2f(m_position.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_position.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
	setRotation(m_heading);

	m_marker = new Marker(hostility != Hostility_Ally ? Marker_Enemy : Marker_Ally, this);
	(*CurrentGame).m_playerShip->MarkThis(this);
}

Planet::~Planet()
{
	
}