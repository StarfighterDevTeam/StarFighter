#ifndef WAVE_H_INCLUDED
#define WAVE_H_INCLUDED

#include "Game.h"

class Wave : public CircleObject
{
public :
	Wave(sf::Vector2f position, AllianceType alliance, float radius, float expansion_speed, float lifespan, float angle_coverage, float angle_direction);
	~Wave();

	void update(sf::Time deltaTime) override;
	Wave* CreateWaveBounce(sf::Vector2f position, float radius, float direction, Node* bounced_node) override;
	AllianceType GetOriginAlliance() override;
	void UpdateCirclePoints();
	void Draw(RenderTarget& screen) override;
	bool HasBouncedOnNode(Node* node) override;

	float m_expansion_speed;
	float m_lifespan;

	float m_angle_coverage;//in degrees
	float m_angle_direction;//in degress

	Node* m_emitter_node;
	Node* m_bounced_node;
	vector<Node*> m_bounced_nodes;

	sf::Vertex m_points[64*2];
};

#endif // WAVE_H_INCLUDED
