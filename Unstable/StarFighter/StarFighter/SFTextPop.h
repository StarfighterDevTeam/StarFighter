#ifndef SFTEXTPOP_H_INCLUDED
#define SFTEXTPOP_H_INCLUDED

#include "GameEntity.h"

using namespace sf;

class SFTextPop : public SFText
{
public:
	SFTextPop();
	SFTextPop(sf::Text* text, float distance_not_faded, float distance_faded, float total_pop_time, GameEntity* target, float offset_positionY);
	~SFTextPop();
	void Update(Time deltaTime);
	static SFTextPop* CreateSFTextPop(const Font& font, unsigned int size, Uint32 style, const Color& color, string text_string, float distance_not_faded, float distance_faded, float total_pop_time, GameEntity* target, float offset_positionY);
	float m_distance_not_faded;
	float m_distance_faded;
	float m_total_pop_time;
	sf::Clock m_timer_clock;
	Uint8 m_alpha;
	GameEntity* m_target;
	float m_offset_positionY;
};

#endif // SFTEXTPOP_H_INCLUDED
