#ifndef DOOR_H_INCLUDED
#define DOOR_H_INCLUDED

#include "Game.h"

enum DoorState
{
	Door_Close,
	Door_Opening,
	Door_Open,
	Door_Closing,
};

class Door : public GameObject
{
public:
	Door(pair<int, int> tileA, pair<int, int> tileB, int frequency, int offset, float song_offset);
	~Door();
	void update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;

	static bool AddDoor(pair<int, int> tileA, pair<int, int> tileB, int frequency, int offset, bool erase_current_door, float song_offset);
	static void EraseDoor(pair<int, int> tileA, pair<int, int> tileB);
	static bool OffsetDoor(pair<int, int> tileA, pair<int, int> tileB);
	float GetDoorSize();

	pair<int, int> m_tileA;
	pair<int, int> m_tileB;
	int m_frequency;
	int m_offset;

	float m_cooldown;
	float m_cooldown_current;
	DoorState m_door_state;
	float m_open_ratio;

	void UpdateDoorsPosition();

	RectangleShape m_door_UL;//up and left
	RectangleShape m_door_DR;//don and right

	Text m_text;
};

#endif // DOOR_H_INCLUDED