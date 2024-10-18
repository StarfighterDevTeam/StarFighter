#ifndef STROBOSCOPIC_H_INCLUDED
#define STROBOSCOPIC_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Stroboscopic : public GameObject
{
public :
	Stroboscopic(sf::Time decade_time, GameObject* parent);
	virtual ~Stroboscopic();
	void update(const float DTIME) override;

	Uint8 m_alpha;
	sf::Time m_decay_time;
};

#endif // STROBOSCOPIC_H_INCLUDED
