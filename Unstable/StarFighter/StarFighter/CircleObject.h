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
	Circle_L16Ballistic_Air,
	Circle_L16Ballistic_MultiDomain,
	Circle_Wave,//must be performed after L16Entities
	Circle_WaveBounce,
	NB_CIRCLE_TYPES,
};

class CircleObject : public sf::CircleShape
{
public:
	CircleObject(AllianceType alliance, CircleType circle_type);
	~CircleObject();

	virtual void update(sf::Time deltaTime);

	virtual Wave* CreateWaveBounce(sf::Vector2f position, float radius, float direction, Wave* wave);
	virtual void WaveReception(Wave* wave);
	virtual void Draw(RenderTarget& screen);
	virtual bool IsColliding(Wave* wave, float direction);
	virtual bool HasBouncedOnEntity(L16Entity* entity);
	virtual bool IsEvadedEntity(L16Entity* entity);
	virtual void AddToEvadedEntities(L16Entity* entity);
	virtual void RemoveEntity(L16Entity* entity);
	virtual void SelectEntity();
	virtual void HoverEntity();

	AllianceType m_alliance;
	sf::Color m_color;

	sf::Vector2f m_previous_speed;
	sf::Vector2f m_speed;
	CircleType m_circle_type;

	bool m_garbageMe;
	bool m_visible;
};

#endif // CIRCLEOBJECT_H_INCLUDED