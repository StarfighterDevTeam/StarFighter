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
	AllianceType GetOriginAlliance() override;
	bool IsHoveredByMouse();
	void ResetColor();
	bool IsColliding(Wave* wave, float direction) override;
	Wave* CreateWaveBounce(sf::Vector2f position, float radius, float direction, Wave* wave) override;

	bool m_hovered;
	bool m_selected;

	bool m_ghost;

	vector<Node*> m_linked_nodes;

	bool m_is_terminal_node;

	bool m_radar_activated;
	float m_radar_frequency;
	float m_radar_frequency_clock;
	float m_radar_range;
	float m_radar_speed;
	float m_radar_direction;
	float m_radar_coverage;

	Wave* CreateRadarWave();
	void WaveReception(Wave* wave) override;
};

#endif // NODE_H_INCLUDED
