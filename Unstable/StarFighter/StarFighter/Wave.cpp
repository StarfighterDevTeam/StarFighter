#include "Wave.h"

extern Game* CurrentGame;

using namespace sf;

Wave::Wave(sf::Vector2f position, AllianceType alliance, float radius, float expansion_speed, float lifespan, float angle_coverage, float angle_direction) : CircleObject(alliance)
{
	Bound(angle_direction, 0, 360);
	Bound(angle_coverage, 0, 360);

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

	for (int i = 0; i < 64*2; i++)
	{
		float ang = (i / 2) * 360.f / 63;
		m_points[i].color = IsInsideAngleCoverage(ang, angle_coverage, angle_direction) ? m_color : sf::Color(0, 0, 0, 0);
	}
}

void Wave::UpdateCirclePoints()
{
	sf::Vector2f position = getPosition();
	float radius = getRadius();
	float thickness = getOutlineThickness();
	sf::Vector2f origin = getOrigin();
	
	for (int i = 0; i < 64*2; i++)
	{
		m_points[i].position.x = position.x + (radius + (i % 2) * thickness) * cos((i / 2) * 2.f * M_PI / 63);
		m_points[i].position.y = position.y - (radius + (i % 2) * thickness) * sin((i / 2) * 2.f * M_PI / 63);
	}
}

Wave::~Wave()
{

}

void Wave::update(sf::Time deltaTime)
{
	//update expansion
	setRadius(getRadius() + m_expansion_speed * deltaTime.asSeconds());
	setOrigin(sf::Vector2f(getRadius(), getRadius()));

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

	//points
	UpdateCirclePoints();
}

AllianceType Wave::GetOriginAlliance()
{
	CircleObject* object = (CircleObject*)m_emitter_node;
	return object->GetOriginAlliance();
}

void Wave::Draw(RenderTarget& screen)
{
	screen.draw(m_points, 64*2, sf::TrianglesStrip);
}

bool Wave::HasBouncedOnNode(Node* node)
{
	for (vector<Node*>::iterator it = m_bounced_nodes.begin(); it != m_bounced_nodes.end(); it++)
	{
		if (node == *it)
			return true;
	}

	return false;
}