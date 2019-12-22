#include "Door.h"

extern Game* CurrentGame;

Door::Door(pair<int, int> tileA, pair<int, int> tileB, int frequency, int offset)
{
	m_tileA = tileA;
	m_tileB = tileB;
	m_frequency = frequency;
	m_offset = offset;

	m_cooldown = 4.f * 4.f / frequency / BPM * 60;
	m_cooldown_current = 0.5f * (m_offset - 1) * m_cooldown + m_cooldown + SONG_OFFSET;
	m_door_state = Door_Close;

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

	//debug offset
	if (m_frequency > 0)
	{
		m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_text.setCharacterSize(18);
		m_text.setColor(sf::Color::Black);
		m_text.setString(to_string(offset));
		m_text.setPosition(getPosition().x, getPosition().y - 20);
	}
}

bool Door::AddDoor(pair<int, int> tileA, pair<int, int> tileB, int frequency, int value, bool erase_current_door)
{
	if (erase_current_door == true)
		EraseDoor(tileA, tileB);

	if (tileA.first > NB_TILES_X || tileB.first > NB_TILES_X || tileA.second > NB_TILES_Y || tileB.second > NB_TILES_Y)
		return false;

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

bool Door::OffsetDoor(pair<int, int> tileA, pair<int, int> tileB)
{
	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[DoorObject])
	{
		Door* door = (Door*)object;
		if (door->m_tileA == tileA && door->m_tileB == tileB && door->m_visible == true)
		{
			door->m_offset++;
			door->m_offset %= 1 + (door->m_frequency / 4);
			if (door->m_offset == 0)
				door->m_offset = 1;
			door->m_text.setString(to_string(door->m_offset));
			return true;
		}
	}

	return false;
}

void Door::update(Time deltaTime)
{
	if (m_frequency <= 0)
		return;

	m_cooldown_current -= deltaTime.asSeconds();

	if (m_door_state == Door_Close && m_cooldown_current <= 0)//OPENING_DURATION)
	{
		m_door_state = Door_Opening;
	}

	if (m_door_state == Door_Opening && m_cooldown_current <= -OPENING_DURATION * 0.5)//0)
	{
		m_door_state = Door_Closing;
	}

	if (m_door_state == Door_Closing && m_cooldown_current <= -OPENING_DURATION)
	{
		m_cooldown_current += m_cooldown;
		m_door_state = Door_Close;
	}

	if (m_door_state == Door_Opening || m_door_state == Door_Closing)
		setColor(sf::Color(255, 255, 255, 0));
	else
		setColor(sf::Color(255, 255, 255, 255));
}

void Door::Draw(RenderTarget& screen)
{
	GameObject::Draw(screen);
	screen.draw(m_text);
}