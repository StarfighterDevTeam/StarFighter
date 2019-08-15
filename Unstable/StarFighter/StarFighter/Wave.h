#ifndef WAVE_H_INCLUDED
#define WAVE_H_INCLUDED

#include "Game.h"

class Wave : public CircleObject
{
public :
	Wave(sf::Vector2f position, AllianceType alliance, float radius, float expansion_speed, float lifespan, float angle_coverage, float angle_direction);
	~Wave();

	void update(sf::Time deltaTime) override;
	void UpdateCirclePoints();
	void Draw(RenderTarget& screen) override;
	bool HasBouncedOnEntity(L16Entity* entity) override;
	bool IsEvadedEntity(L16Entity* entity) override;
	void AddToEvadedEntities(L16Entity* entity) override;
	void RemoveEntity(L16Entity* entity) override;
	void SetColor(sf::Color color);

	float m_expansion_speed;
	float m_lifespan;

	float m_angle_coverage;//in degrees
	float m_angle_direction;//in degress

	L16Entity* m_emitter_entity;
	L16Entity* m_bounced_entity;
	vector<L16Entity*> m_bounced_entities;
	vector<L16Entity*> m_evaded_entities;

	sf::Vertex m_points[CIRCLE_POINTS_COUNT*2];
};

#endif // WAVE_H_INCLUDED
