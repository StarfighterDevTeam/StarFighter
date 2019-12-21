#ifndef DOOR_H_INCLUDED
#define DOOR_H_INCLUDED

#include "Game.h"

class Door : public GameObject
{
public:
	Door(pair<int, int> tileA, pair<int, int> tileB, int frequency, int offset);
	~Door(){};


	static bool AddDoor(pair<int, int> tileA, pair<int, int> tileB, int frequency, int offset, bool erase_current_door);
	static bool EraseDoor(pair<int, int> tileA, pair<int, int> tileB);

	pair<int, int> m_tileA;
	pair<int, int> m_tileB;
	int m_frequency;
	int m_offset;
};

#endif // DOOR_H_INCLUDED