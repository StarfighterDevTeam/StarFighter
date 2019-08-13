#ifndef L16ENTITY_H_INCLUDED
#define L16ENTITY_H_INCLUDED

#include "Game.h"
#include "Wave.h"

class Link;

enum L16EntityType
{
	L16Entity_Ballistic,
	L16Entity_Wing,
	L16Entity_Node,
};

enum L16MissionType
{
	L16Mission_Idle,
};

class L16Entity : public CircleObject
{
public :
	L16Entity(sf::Vector2f position, AllianceType alliance, float radius);
	~L16Entity();

	void update(sf::Time deltaTime) override;
	AllianceType GetOriginAlliance() override;
	bool IsHoveredByMouse();
	void ResetColor();
	bool IsColliding(Wave* wave, float direction) override;
	Wave* CreateWaveBounce(sf::Vector2f position, float radius, float direction, Wave* wave) override;

	L16EntityType m_type;
	L16MissionType m_mission;

	bool m_hovered;
	bool m_selected;

	bool m_radar_activated;
	float m_radar_frequency;
	float m_radar_frequency_clock;
	float m_radar_range;
	float m_radar_wavespeed;
	float m_radar_heading;
	float m_radar_coverage;

	float m_radar_bounce_feedback_clock;

	Wave* CreateRadarWave();
	void WaveReception(Wave* wave) override;
	void RevealEntity();
};

#endif // L16ENTITY_H_INCLUDED
