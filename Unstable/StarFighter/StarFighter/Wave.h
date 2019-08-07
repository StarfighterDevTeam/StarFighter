#ifndef WAVE_H_INCLUDED
#define WAVE_H_INCLUDED

#include "Game.h"

class Wave : public CircleObject
{
public :
	Wave(sf::Vector2f position, AllianceType alliance, float radius, float expansion_speed, float lifespan);
	~Wave();

	void update(sf::Time deltaTime) override;
	Wave* CreateWaveBounce(sf::Vector2f position, float radius, sf::Vector2f vector, Node* bounced_node) override;
	AllianceType GetOriginAlliance() override;
	void UpdateCirclePoints();
	void Draw(RenderTarget& screen) override;

	float m_expansion_speed;
	float m_lifespan;

	Node* m_emitter_node;
	Node* m_bounced_node;

	sf::Vertex m_points[64*2];
};

#endif // WAVE_H_INCLUDED
