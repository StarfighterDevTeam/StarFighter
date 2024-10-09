#ifndef SFTEXTPOP_H_INCLUDED
#define SFTEXTPOP_H_INCLUDED

#include "GameObject.h"

using namespace sf;

class SFTextPop : public SFText
{
public:
	SFTextPop() {};
	SFTextPop(SFText* text, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* target, float offset_positionY);
	~SFTextPop() {};
	void Update(Time deltaTime) override;

	float m_distance_not_faded = 0.f;
	float m_distance_faded = 0.f;
	float m_total_pop_time = 0.f;
	sf::Clock m_timer_clock;
	Uint8 m_alpha = 0;
	GameObject* m_target = nullptr;
	float m_offset_positionY = 0.f;
};

#endif // SFTEXTPOP_H_INCLUDED
