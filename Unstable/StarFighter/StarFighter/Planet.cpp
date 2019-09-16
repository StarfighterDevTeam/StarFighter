#include "Planet.h"

extern Game* CurrentGame;

using namespace sf;

Planet::Planet(sf::Vector2i sector_index, Hostility hostility, int nb_missions_to_create)
{
	m_collider = PlanetObject;
	m_layer = Planet_Layer;

	SetHostility(hostility);
	m_nb_missions_to_create = nb_missions_to_create;

	//set a unique planet id
	do
	{
		m_planet_id = RandomizeIntBetweenValues(1001, 9999);
	} while ((*CurrentGame).m_planet_ids[m_planet_id] != NULL);
	(*CurrentGame).m_planet_ids[m_planet_id] = this;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/planet.png", sf::Vector2f(PLANET_RADIUS * 2, PLANET_RADIUS * 2), 1, NB_PLANET_TYPES);
	SetAnimationLine(RandomizeIntBetweenValues(0, NB_PLANET_TYPES - 1));
	m_heading = 0;

	(*CurrentGame).SetStarSectorIndex(this, sector_index);//after Init to keep m_removeMe = true if needed

	//update position and rotation "manually" because they won't be updated during the frame of their creation
	setPosition(sf::Vector2f(m_position.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_position.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
	setRotation(m_heading);

	//gravity
	m_gravity_range = PLANET_ORBIT_RANGE;
	m_gravity_period = 8;

	m_orbit_circle.setRadius(m_gravity_range);
	m_orbit_circle.setOrigin(sf::Vector2f(m_gravity_range, m_gravity_range));
	m_orbit_circle.setOutlineColor(sf::Color(128, 128, 128, 40));
	m_orbit_circle.setOutlineThickness(2);
	m_orbit_circle.setFillColor(sf::Color::Transparent);
	m_orbit_circle.setPointCount(128);

	//id
	m_id_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_id_text.setStyle(sf::Text::Bold);
	m_id_text.setCharacterSize(18);
	m_id_text.setColor(sf::Color::White);
	m_id_text.setString("[" + to_string(m_planet_id) + "]");
}

Planet::~Planet()
{
	
}

void Planet::Update(sf::Time deltaTime)
{
	if (m_frameNumber > 1)
		AnimatedSprite::Update(deltaTime);
}

void Planet::Draw(RenderTarget& screen)
{
	GameObject::Draw(screen);

	if (m_visible == true)
	{
		screen.draw(m_orbit_circle);
		screen.draw(m_id_text);
	}
}

void Planet::SetHostility(Hostility hostility)
{
	SpatialObject::SetHostility(hostility);

	if (m_marker_target != NULL)
		m_marker_target->SetMarkerType(hostility == Hostility_Ally ? Marker_Ally : Marker_Enemy);
}

void Planet::SetPosition(sf::Vector2f position)
{
	GameObject::SetPosition(position);

	m_orbit_circle.setPosition(getPosition());
	m_id_text.setPosition(sf::Vector2f(getPosition().x - m_id_text.getGlobalBounds().width * 0.5, getPosition().y - m_id_text.getCharacterSize() * 0.65));
}

bool Planet::CheckMarkingConditions()
{
	return false;
}