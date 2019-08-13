#include "Node.h"

extern Game* CurrentGame;

using namespace sf;

Node::Node(sf::Vector2f position, AllianceType alliance, float radius) : L16Entity(position, alliance, radius)
{
	m_type = L16Entity_Node;
	m_is_terminal_node = false;

	m_radar_frequency = 0.5;
	m_radar_range = 100;
	m_radar_wavespeed = 200;
	m_radar_direction = alliance == PlayerAlliance ? 0 : 180;
	m_radar_coverage = 60;

	//m_visible = true;// alliance == PlayerAlliance;
}

Node::~Node()
{

}

void Node::update(sf::Time deltaTime)
{
	//radar
	if (m_is_terminal_node == true)
	{
		m_radar_activated = true;
	}
	else
	{
		m_radar_activated = false;
		for (vector<L16Entity*>::iterator it = m_linked_entities.begin(); it != m_linked_entities.end(); it++)
		{
			if ((*it)->m_radar_activated == true)
			{
				m_radar_activated = true;
				break;
			}
		}
	}

	L16Entity::update(deltaTime);
}

Wing* Node::CreateWing()
{
	Wing* wing = new Wing(getPosition(), m_alliance);
	(*CurrentGame).AddCircleObject(wing, Circle_L16Entity);
	(*CurrentGame).m_wings.push_back(wing);

	return wing;
}