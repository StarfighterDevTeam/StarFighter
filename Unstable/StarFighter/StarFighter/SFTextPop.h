#ifndef SFTEXTPOP_H_INCLUDED
#define SFTEXTPOP_H_INCLUDED

#include "GameObject.h"

using namespace sf;

class SFTextPop : public SFText
{
public:
	SFTextPop() = default;
	SFTextPop(SFText* text, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* target, float offset_positionY);
	~SFTextPop() {};
	void update(const float DTIME) override;

	float m_distance_not_faded;
	float m_distance_faded;
	float m_total_pop_time;
	sf::Clock m_timer_clock;
	Uint8 m_alpha;
	GameObject* m_target;
	float m_offset_positionY;
};

#endif // SFTEXTPOP_H_INCLUDED
