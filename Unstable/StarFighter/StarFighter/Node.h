#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Game.h"
#include "Wave.h"

class Link;

class Node : public CircleObject
{
public :
	Node(sf::Vector2f position, AllianceType alliance);
	Node(sf::Vector2f position, AllianceType alliance, float radius);
	~Node();

	void update(sf::Time deltaTime) override;
	bool IsHoveredByMouse();
	void ResetColor();

	bool m_hovered;
	bool m_selected;

	vector<Node*> m_linked_nodes;

	bool m_is_terminal_node;

	bool m_radar_activated;
	float m_radar_frequency;
	float m_radar_frequency_clock;
	float m_radar_range;
	float m_radar_speed;

	Wave* CreateRadarWave();
	void WaveReception(Wave* wave) override;
};

#endif // NODE_H_INCLUDED
