#ifndef SFTEXTPOP_H_INCLUDED
#define SFTEXTPOP_H_INCLUDED

#include "GameObject.h"
#include "SFText.h"
#include "Game.h"

using namespace sf;

class SFTextPop : public SFText
{
public:
	SFTextPop();
	~SFTextPop(){};
	SFTextPop(SFText* text, float time_fade_in, float time_not_faded, float time_fade_out, GameObject* target, float speed_y, sf::Vector2f offset);
	void update(Time deltaTime, float hyperspeedMultiplier) override;

	float m_time_fade_in;
	float m_time_not_faded;
	float m_time_fade_out;
	sf::Time m_time;
	Uint8 m_alpha;
	GameObject* m_target;
	sf::Vector2f m_offset;
	float m_speed_y;
};

#endif // SFTEXTPOP_H_INCLUDED