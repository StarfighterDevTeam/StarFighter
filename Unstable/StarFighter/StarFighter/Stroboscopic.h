#ifndef STROBOSCOPIC_H_INCLUDED
#define STROBOSCOPIC_H_INCLUDED

#include "Game.h"

class Stroboscopic : public GameObject
{
public :
	Stroboscopic(sf::Time decade_time, GameObject* parent, int max_alpha);
	virtual ~Stroboscopic();
	void update(sf::Time deltaTime, float hyperspeedMultiplier = 1) override;

	Uint8 m_alpha;
	sf::Time m_decay_time;

private:
	Animation m_frozenAnimation; //owns the single frozen frame shown while decaying; a value member so no manual new/delete is needed
};

#endif // STROBOSCOPIC_H_INCLUDED
