#include "Door.h"

extern Game* CurrentGame;

Door::Door(pair<int, int> tileA, pair<int, int> tileB, int frequency, int offset)
{
	m_tileA = tileA;
	m_tileB = tileB;
	m_frequency = frequency;
	m_offset = offset;

	sf::Color color;
	switch (frequency)
	{
		case -1:
		{
			color = sf::Color::Green;
			break;
		}
		case 0:
		{
			color = sf::Color::Black;
			break;
		}
		case 1:
		case 2:
		case 3:
		case 4:
		{
			color = sf::Color::Red;
			break;
		}
		case 8:
		{
			color = sf::Color::Blue;
			break;
		}
		case 16:
		{
			color = sf::Color::Yellow;
			break;
		}
		case 12:
		case 24:
		{
			color = sf::Color::Magenta;
			break;
		}	
	}

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), color, sf::Vector2f(TILE_SIZE, DOOR_WIDTH));

	//horizontal connexion = vertical door
	if (m_tileA.second == m_tileB.second)
	{
		rotate(90);
		setPosition(START_X + (0.5 + (MinBetweenIntValues(tileA.first, tileB.first))) * TILE_SIZE, START_Y - (tileB.second * TILE_SIZE));
	}
	else
	{
		setPosition(START_X + (tileA.first * TILE_SIZE), START_Y - (0.5 + (MinBetweenIntValues(tileA.second, tileB.second))) * TILE_SIZE);
	}
}

bool Door::AddDoor(pair<int, int> tileA, pair<int, int> tileB, int frequency, int value, bool erase_current_door)
{
	if (erase_current_door == true)
		EraseDoor(tileA, tileB);

	Door* door = new Door(tileA, tileB, frequency, value);
	(*CurrentGame).addToScene(door, DoorLayer, DoorObject, true);

	return true;
}

bool Door::EraseDoor(pair<int, int> tileA, pair<int, int> tileB)
{
	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[DoorObject])
	{
		Door* door = (Door*)object;
		if (door->m_tileA == tileA && door->m_tileB == tileB && door->m_visible == true)
		{
			door->m_visible = false;
			return true;
		}
	}

	return false;
}