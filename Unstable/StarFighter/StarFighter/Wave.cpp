#include "Wave.h"

extern Game* CurrentGame;

using namespace sf;

Wave::Wave(sf::Vector2f position, AllianceType alliance, float radius, float expansion_speed, float lifespan, float angle_coverage, float angle_direction) : CircleObject(alliance, Circle_Wave)
{
	BoundAngle(angle_direction, 360);
	BoundAngle(angle_coverage, 360);

	setRadius(radius);
	setOrigin(sf::Vector2f(radius, radius));
	setPosition(position);
	m_expansion_speed = expansion_speed;
	m_lifespan = lifespan;
	m_angle_coverage = angle_coverage;
	m_angle_direction = angle_direction;

	setFillColor(sf::Color(0, 0, 0, 0));
	setOutlineColor(m_color);
	setOutlineThickness(4);

	for (int i = 0; i < CIRCLE_POINTS_COUNT; i++)
	{
		float ang = i * 360.f / (CIRCLE_POINTS_COUNT - 1);
		m_points[i * 2].color = IsInsideAngleCoverage(ang, angle_coverage, angle_direction) ? m_color : sf::Color(0, 0, 0, 0);
		m_points[i * 2 + 1].color = m_points[i * 2].color;
	}
}

void Wave::UpdateCirclePoints()
{
	sf::Vector2f position = getPosition();
	float radius = getRadius();
	float thickness = getOutlineThickness();
	sf::Vector2f origin = getOrigin();
	
	for (int i = 0; i < CIRCLE_POINTS_COUNT*2; i++)
	{
		m_points[i].position.x = position.x + (radius + (i % 2) * thickness) * cos((i / 2) * 2.f * M_PI / (CIRCLE_POINTS_COUNT - 1));
		m_points[i].position.y = position.y - (radius + (i % 2) * thickness) * sin((i / 2) * 2.f * M_PI / (CIRCLE_POINTS_COUNT - 1));
	}
}

Wave::~Wave()
{

}

void Wave::update(sf::Time deltaTime)
{
	//lifespan
	m_lifespan -= deltaTime.asSeconds();

	if (m_lifespan < 1)
	{
		Uint8 alpha = (Uint8)(m_lifespan * 255);
		setOutlineColor(sf::Color(m_color.r, m_color.g, m_color.b, alpha));
	}

	if (m_lifespan <= 0)
	{
		m_garbageMe = true;
	}

	//update expansion
	setRadius(getRadius() + m_expansion_speed * deltaTime.asSeconds());
	setOrigin(sf::Vector2f(getRadius(), getRadius()));
	UpdateCirclePoints();

	

	//points
	
}

AllianceType Wave::GetOriginAlliance()
{
	CircleObject* object = (CircleObject*)m_emitter_entity;
	return object->GetOriginAlliance();
}

void Wave::Draw(RenderTarget& screen)
{
	screen.draw(m_points, CIRCLE_POINTS_COUNT*2, sf::TrianglesStrip);
}

bool Wave::HasBouncedOnEntity(L16Entity* entity)
{
	for (vector<L16Entity*>::iterator it = m_bounced_entities.begin(); it != m_bounced_entities.end(); it++)
	{
		if (entity == *it)
			return true;
	}

	return false;
}

bool Wave::IsEvadedEntity(L16Entity* entity)
{
	for (vector<L16Entity*>::iterator it = m_evaded_entities.begin(); it != m_evaded_entities.end(); it++)
	{
		if (entity == *it)
			return true;
	}

	return false;
}

void Wave::AddToEvadedEntities(L16Entity* entity)
{
	m_evaded_entities.push_back(entity);
}