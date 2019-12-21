#ifndef DOOR_H_INCLUDED
#define DOOR_H_INCLUDED

#include "Game.h"

class Door : public GameObject
{
public:
	Door(pair<int, int> tileA, pair<int, int> tileB, int frequency, int value);
	~Door(){};

	pair<int, int> m_tileA;
	pair<int, int> m_tileB;
	int m_frequency;
	int m_value;
};

#endif // DOOR_H_INCLUDED