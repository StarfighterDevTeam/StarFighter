#ifndef PLANCTON_H_INCLUDED
#define PLANCTON_H_INCLUDED

#include "LivingThing.h"

class Plancton : public LivingThing
{
public :
	Plancton();
	Plancton(sf::Vector2f position);
	void Init();

	virtual ~Plancton();
	void update(sf::Time deltaTime) override;

	float m_change_dir_time;
	sf::Clock m_change_dir_clock;
};

#endif // PLANCTON_H_INCLUDED
