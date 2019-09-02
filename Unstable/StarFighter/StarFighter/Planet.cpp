#include "Planet.h"

extern Game* CurrentGame;

using namespace sf;

Planet::Planet(sf::Vector2i sector_index, HostilityLevel hostility, int nb_missions)
{
	m_hostility = hostility;
	m_nb_missions = nb_missions;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/planet.png", sf::Vector2f(138, 138), 1, NB_PLANET_TYPES);
	SetAnimationLine(RandomizeIntBetweenValues(0, NB_PLANET_TYPES - 1));
	m_heading = 0;

	m_collider = PlanetObject;
	m_layer = Planet_Layer;

	(*CurrentGame).SetStarSectorIndex(this, sector_index);//after Init to keep m_removeMe = true if needed

	//update position and rotation "manually" because they won't be updated during the frame of their creation
	setPosition(sf::Vector2f(m_position.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_position.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
	setRotation(m_heading);

	m_marker = new Marker(hostility != Hostility_Ally ? Marker_Enemy : Marker_Ally, this);
	(*CurrentGame).m_playerShip->MarkThis(this);

	//gravity
	m_gravity_range = 200;
	m_gravity_period = 8;

	m_orbit_circle.setRadius(m_gravity_range);
	m_orbit_circle.setOrigin(sf::Vector2f(m_gravity_range, m_gravity_range));
	m_orbit_circle.setOutlineColor(sf::Color(128, 128, 128, 40));
	m_orbit_circle.setOutlineThickness(2);
	m_orbit_circle.setFillColor(sf::Color::Transparent);
	m_orbit_circle.setPointCount(128);
}

Planet::~Planet()
{
	
}

void Planet::Draw(RenderTarget& screen)
{
	SpatialObject::Draw(screen);

	if (m_visible == true)
		screen.draw(m_orbit_circle);
}

void Planet::SetHostility(HostilityLevel hostility)
{
	SpatialObject::SetHostility(hostility);

	m_marker->SetMarkerType(hostility == Hostility_Ally ? Marker_Ally : Marker_Enemy);
}

void Planet::SetPosition(sf::Vector2f position)
{
	GameObject::SetPosition(position);

	m_orbit_circle.setPosition(getPosition());
}