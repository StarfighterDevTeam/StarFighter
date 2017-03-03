#include "Walker.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Walker::Walker()
{
	Init();
}

void Walker::Init()
{
	Agent::Init();

	m_display_name = "Walker";
	m_state = Walker_Undecided;
	m_standardSpeed = RandomizeFloatBetweenValues(sf::Vector2f(WALKER_MIN_SPEED, WALKER_MAX_SPEED));

	GenerateItems();
}

Walker::Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Agent(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

Walker::Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Agent(position, speed, textureName, size)
{
	Init();
}

Walker::~Walker()
{
	
}

void Walker::SetRandomIdleRoutine()
{
	m_speed.x = 0;
	m_speed.y = 0;
	m_phaseTime = RandomizeFloatBetweenValues(sf::Vector2f(WALKER_IDLE_MIN_WAIT_TIME, WALKER_IDLE_MAX_WAIT_TIME));
}

void Walker::SetRandomWalkRoutine()
{
	float speed = RandomizeFloatBetweenValues(sf::Vector2f(m_standardSpeed * WALKER_SLOW_DOWN_VARIANCE, m_standardSpeed * WALKER_SPEED_UP_VARIANCE));
	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	SetSpeedVectorFromAbsoluteSpeedAndAngle(speed, angle);
	m_phaseTime = RandomizeFloatBetweenValues(sf::Vector2f(WALKER_IDLE_MIN_WALK_TIME, WALKER_IDLE_MAX_WALK_TIME));
}

void Walker::update(sf::Time deltaTime)
{
	Agent::update(deltaTime);

	switch (m_state)
	{
		case Walker_Undecided:
		{
			m_state = Walker_Idle;
			SetRandomIdleRoutine();
			m_phaseClock.restart();
		}
		case Walker_Idle:
		{
			//phase out?
			if (m_phaseClock.getElapsedTime().asSeconds() > m_phaseTime)
			{
				m_state = Walker_Walk;
				SetRandomWalkRoutine();
				m_phaseClock.restart();
			}
			break;
		}
		case Walker_Walk:
		{
			//phase out?
			if (m_phaseClock.getElapsedTime().asSeconds() > m_phaseTime)
			{
				m_state = RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < WALKER_CHANCE_OF_WALKING ? Walker_Walk : Walker_Idle;
				switch (m_state)
				{
					case Walker_Walk:
					{
						SetRandomWalkRoutine();
						break;
					}
					case Walker_Idle:
					{
						SetRandomIdleRoutine();
						break;
					}
				}
				m_phaseClock.restart();
			}
			break;
		}
	}
}