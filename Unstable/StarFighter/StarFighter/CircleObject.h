#ifndef CIRCLEOBJECT_H_INCLUDED
#define CIRCLEOBJECT_H_INCLUDED

#include "GameObject.h"

class L16Entity;
class Wave;

#define CIRCLE_POINTS_COUNT			128

enum CircleType
{
	Circle_L16Entity_Air,
	Circle_L16Entity_Ground,
	Circle_L16Entity_MultiDomain,
	Circle_Wave,
	NB_CIRCLE_TYPES,
};

class CircleObject : public sf::CircleShape
{
public:
	CircleObject(AllianceType alliance);
	~CircleObject();

	virtual void update(sf::Time deltaTime);

	virtual Wave* CreateWaveBounce(sf::Vector2f position, float radius, float direction, Wave* wave);
	virtual void WaveReception(Wave* wave);
	virtual AllianceType GetOriginAlliance();
	virtual void Draw(RenderTarget& screen);
	virtual bool IsColliding(Wave* wave, float direction);
	virtual bool HasBouncedOnEntity(L16Entity* entity);
	virtual bool IsEvadedEntity(L16Entity* entity);
	virtual void AddToEvadedEntities(L16Entity* entity);

	AllianceType m_alliance;
	sf::Color m_color;

	sf::Vector2f m_previous_speed;
	sf::Vector2f m_speed;
	CircleType m_type;

	bool m_garbageMe;
	bool m_visible;
};

#endif // CIRCLEOBJECT_H_INCLUDED