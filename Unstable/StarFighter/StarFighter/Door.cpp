#include "Door.h"

extern Game* CurrentGame;

Door::Door(pair<int, int> tileA, pair<int, int> tileB, int frequency, int offset)
{
	m_tileA = tileA;
	m_tileB = tileB;
	m_frequency = frequency;
	m_offset = offset;
	m_open_ratio = 0;

	m_cooldown = 4.f / SONG_BPM * 60;
	m_cooldown_current = m_cooldown + (m_cooldown / (frequency / 4.f) * (m_offset - 1)) + SONG_OFFSET;   // 0.5f * (m_offset - 1) * m_cooldown + m_cooldown + SONG_OFFSET;
	m_door_state = Door_Close;

	switch (frequency)
	{
		case -1:
		{
			m_color = sf::Color::Green;
			break;
		}
		case 0:
		{
			m_color = sf::Color::Black;
			break;
		}
		case 1:
		case 2:
		case 3:
		case 4:
		{
			m_color = sf::Color::Red;
			break;
		}
		case 8:
		{
			m_color = sf::Color::Blue;
			break;
		}
		case 16:
		{
			m_color = sf::Color::Yellow;
			break;
		}
		case 12:
		case 24:
		{
			m_color = sf::Color::Magenta;
			break;
		}	
		case 64:
		{
			m_color = sf::Color(255, 128, 255, 255);//orange
			break;
		}
	}

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), m_color, sf::Vector2f(TILE_SIZE, DOOR_WIDTH));

	//horizontal connexion = vertical door
	if (m_tileA.second == m_tileB.second)
	{
		setRotation(90);
		setPosition(START_X + (0.5 + (MinBetweenIntValues(m_tileA.first, m_tileB.first))) * TILE_SIZE, START_Y - (m_tileB.second * TILE_SIZE));
	}
	else//vertical connexion = horizontal door
	{
		setRotation(0);
		setPosition(START_X + (m_tileA.first * TILE_SIZE), START_Y - (0.5 + (MinBetweenIntValues(m_tileA.second, m_tileB.second))) * TILE_SIZE);
	}

	//visual doors
	m_door_UL.setFillColor(sf::Color(m_color.r, m_color.g, m_color.b, 128));
	m_door_UL.setOutlineThickness(2);
	m_door_UL.setOutlineColor(sf::Color(m_color));

	m_door_DR.setFillColor(sf::Color(m_color.r, m_color.g, m_color.b, 128));
	m_door_DR.setOutlineThickness(2);
	m_door_DR.setOutlineColor(sf::Color(m_color));

	//m_door_UL = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), m_color, sf::Vector2f(TILE_SIZE * 0.5, DOOR_WIDTH), 2);
	//m_door_DR = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), m_color, sf::Vector2f(TILE_SIZE * 0.5, DOOR_WIDTH), 2);

	UpdateDoorsPosition();

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

void Door::UpdateDoorsPosition()
{
	float door_size = Lerp(m_open_ratio, 0, 1, TILE_SIZE * 0.5, DOOR_SIZE_MIN);

	//horizontal connexion = vertical door
	if (m_tileA.second == m_tileB.second)
	{
		//visual doors
		m_door_UL.setSize(sf::Vector2f(DOOR_WIDTH, door_size));
		m_door_UL.setOrigin(m_door_UL.getSize().x * 0.5, m_door_UL.getSize().y * 0.5);
		m_door_UL.setPosition(getPosition().x, getPosition().y - TILE_SIZE * 0.5 + door_size * 0.5);

		m_door_DR.setSize(sf::Vector2f(DOOR_WIDTH, door_size));
		m_door_DR.setOrigin(m_door_DR.getSize().x * 0.5, m_door_DR.getSize().y * 0.5);
		m_door_DR.setPosition(getPosition().x, getPosition().y + TILE_SIZE * 0.5 - door_size * 0.5);
	}
	else//vertical connexion = horizontal door
	{
		//visual doors
		m_door_UL.setSize(sf::Vector2f(door_size, DOOR_WIDTH));
		m_door_UL.setOrigin(m_door_UL.getSize().x * 0.5, m_door_UL.getSize().y * 0.5);
		m_door_UL.setPosition(getPosition().x - TILE_SIZE * 0.5 + door_size * 0.5, getPosition().y);

		m_door_DR.setSize(sf::Vector2f(door_size, DOOR_WIDTH));
		m_door_DR.setOrigin(m_door_DR.getSize().x * 0.5, m_door_DR.getSize().y * 0.5);
		m_door_DR.setPosition(getPosition().x + TILE_SIZE * 0.5 - door_size * 0.5, getPosition().y);
	}
}

Door::~Door()
{
	//delete m_door_UL;
	//delete m_door_DR;
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

	//CLOSE -> OPENING
	if (m_door_state == Door_Close && m_cooldown_current <= 0)//OPENING_DURATION)
	{
		m_door_state = Door_Opening;

		m_open_ratio = 1;
	}

	//OPENING -> CLOSING
	if (m_door_state == Door_Opening && m_cooldown_current <= -OPENING_DURATION * 0.5)//0)
	{
		m_door_state = Door_Closing;

		m_open_ratio = 1;
	}

	// CLOSING -> CLOSE
	if (m_door_state == Door_Closing && m_cooldown_current <= -OPENING_DURATION)
	{
		m_cooldown_current += m_cooldown;
		m_door_state = Door_Close;

		m_open_ratio = 1;
	}

	//open ratio
	switch (m_door_state)
	{
		case Door_Close:
			m_open_ratio = 0;
			break;
		case Door_Open:
			m_open_ratio = 1;
			break;
		case Door_Opening:
			m_open_ratio = Lerp(m_cooldown_current, -OPENING_DURATION * 0.5, 0, 1, 0);
			break;
		case Door_Closing:
			m_open_ratio = Lerp(m_cooldown_current, -OPENING_DURATION, -OPENING_DURATION * 0.5, 0, 1);
			break;
	}

	//color
	if (m_door_state == Door_Opening || m_door_state == Door_Closing)
		setColor(sf::Color(255, 255, 255, 0));
	else
		setColor(sf::Color(255, 255, 255, 255));

	 //visual doors
	UpdateDoorsPosition();
}

void Door::Draw(RenderTarget& screen)
{
	if (m_frequency <= 0)
		GameObject::Draw(screen);
	else
	{
		screen.draw(m_door_UL);
		screen.draw(m_door_DR);
		screen.draw(m_text);
	}
}