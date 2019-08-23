#ifndef MARKER_H_INCLUDED
#define MARKER_H_INCLUDED

#include "Game.h"

class Ship;

class Marker : public GameObject
{
public:
	Marker(GameObject* target);
	~Marker();
	
	void Update(sf::Time deltaTime);

	GameObject* m_target;
};

#endif // MARKER_H_INCLUDED
