#include "Walker.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Walker::Walker()
{
	Walker::Init();
}

void Walker::Init()
{
	m_display_name = "Walker";
	m_state = Walker_Undecided;
	m_awareness = Walker_Unaware;
	m_standardSpeed = RandomizeFloatBetweenValues(sf::Vector2f(WALKER_MIN_SPEED, WALKER_MAX_SPEED));
	m_vision_range = 300;
	m_vision_angle = 90;
	m_angular_speed = WALKER_ANGULAR_SPEED;

	m_vision_cone = (*CurrentGame).m_vision_cone_90->Clone();
	m_vision_cone->m_DontGarbageMe = true;
	m_vision_cone->setPosition(getPosition());
	m_vision_cone->setColor(sf::Color(75, 195, 10, GHOST_ALPHA_VALUE));
	(*CurrentGame).addToScene(m_vision_cone, VisionConeLayer, VisionConeObject);

	GenerateItems();
}

Walker::Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Agent(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Walker::Init();
}

Walker::Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Agent(position, speed, textureName, size)
{
	Walker::Init();
}

Walker::~Walker()
{
	m_vision_cone->m_GarbageMe = true;
	m_vision_cone->m_visible = false;
	m_vision_cone = NULL;
}

void Walker::SetRandomIdleRoutine()
{
	printf("idle routine | \n");
	m_speed.x = 0;
	m_speed.y = 0;
	m_phaseTime = RandomizeFloatBetweenValues(sf::Vector2f(WALKER_IDLE_MIN_WAIT_TIME, WALKER_IDLE_MAX_WAIT_TIME));
}

void Walker::SetRandomPivotRoutine()
{
	printf("pivot routine | \n");
	m_speed.x = 0;
	m_speed.y = 0;
	m_desired_angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
}

void Walker::SetRandomWalkRoutine()
{
	m_desired_speed = RandomizeFloatBetweenValues(sf::Vector2f(m_standardSpeed * WALKER_SLOW_DOWN_VARIANCE, m_standardSpeed * WALKER_SPEED_UP_VARIANCE));
	
	//m_desired_angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	printf("walk routine | ");
	
	//SetSpeedVectorFromAbsoluteSpeedAndAngle(speed, angle);
	//setRotation(angle * 180 / M_PI - 180);
	//m_desired_angle = angle * 180 / M_PI - 180;
	//m_speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_desired_speed, m_desired_angle / 180 * M_PI - M_PI);
	m_phaseTime = RandomizeFloatBetweenValues(sf::Vector2f(WALKER_MIN_WALK_TIME, WALKER_MAX_WALK_TIME));
}

void Walker::update(sf::Time deltaTime)
{
	Agent::update(deltaTime);

	//update vision cone
	if (m_vision_cone)
	{
		m_vision_cone->setPosition(getPosition());
		m_vision_cone->setRotation(getRotation());

		//who is the most suspicious player?
		m_max_current_awareness = (*CurrentGame).GetCurrentMaxAwareness(this);
		if (m_max_current_awareness < WALKER_AWARENESS_THRESHOLD_CASUAL)
		{
			m_awareness = Walker_Unaware;
		}
		else if (m_max_current_awareness < WALKER_AWARENESS_THRESHOLD_SUSPICIOUS)
		{
			m_awareness = Walker_Casual;
		}
		else if (m_max_current_awareness < WALKER_AWARENESS_THRESHOLD_ANGRY)
		{
			m_awareness = Walker_Suspicious;
		}
		else if (m_max_current_awareness < WALKER_AWARENESS_THRESHOLD_MAD)
		{
			m_awareness = Walker_Angry;
		}
		else
		{
			m_awareness = Walker_Mad;
		}

		switch (m_awareness)
		{
			case Walker_Unaware:
			{
				m_vision_cone->setColor(sf::Color(75, 195, 10, GHOST_ALPHA_VALUE));
				break;
			}
			case Walker_Casual:
			{
				m_vision_cone->setColor(sf::Color(255, 242, 0, GHOST_ALPHA_VALUE));
				break;
			}
			case Walker_Suspicious:
			{
				m_vision_cone->setColor(sf::Color(255, 109, 21, GHOST_ALPHA_VALUE));
				break;
			}
			case Walker_Angry:
			{
				m_vision_cone->setColor(sf::Color(255, 44, 32, GHOST_ALPHA_VALUE));
				break;
			}
			case Walker_Mad:
			{
				m_vision_cone->setColor(sf::Color(0, 0, 0, GHOST_ALPHA_VALUE));
				break;
			}
		}
	}

	//update movement
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
				m_state = RandomizeFloatBetweenValues(sf::Vector2f(0, 1)) < WALKER_CHANCE_OF_WALKING ? Walker_Walk : Walker_Pivot;
				switch (m_state)
				{
					case Walker_Pivot:
					{
						SetRandomPivotRoutine();
						break;
					}
					case Walker_Walk:
					{
						SetRandomWalkRoutine();
						break;
					}
				}
				m_phaseClock.restart();
			}
			break;
		}
		case Walker_Pivot:
		{
			if (!TurnToDesiredAngle(deltaTime))
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
		case Walker_Walk:
		{
			//time out?
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
			else
			{
				//do we have a destination? if not, wait for UpdatePathfind() to get us one.
				if (!m_current_path.empty())
				{
					GoToWaypoint(m_current_path.front());

					//rotation finished? let's move on to destination
					if (!TurnToDesiredAngle(deltaTime))
					{
						SetSpeedVectorFromAbsoluteSpeedAndAngle(m_desired_speed, m_desired_angle / 180 * M_PI - M_PI);

						float remaining_distance_squared = GameObject::GetDistanceSquaredBetweenPositions(getPosition(), m_desired_destination);

						//arrived at destination?
						if (remaining_distance_squared <= m_desired_speed*m_desired_speed * deltaTime.asSeconds() * deltaTime.asSeconds())
						{
							//what is the next waypoint?
							m_current_path.pop_front();
							if (!m_current_path.empty())
							{
								GoToWaypoint(m_current_path.front());
								SetSpeedVectorFromAbsoluteSpeedAndAngle(m_desired_speed, m_desired_angle / 180 * M_PI - M_PI);
								printf("current waypoint: %d (blocking: %d)\n", m_current_path.front(), (int)(*CurrentGame).IsTileBlocking(m_current_path.front()));
							}
							else//arrived at destination
							{
								setPosition(m_desired_destination);
								m_speed = sf::Vector2f(0, 0);

								//next phase
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
						}
						else
						{
							SetSpeedVectorFromAbsoluteSpeedAndAngle(m_desired_speed, m_desired_angle / 180 * M_PI - M_PI);
							//printf("desired angle: %f, current angle: %f, movX: %f, movY:%f\n", m_desired_angle, getRotation(), m_speed.x, m_speed.y);
						}
					}
					else
					{
						m_phaseClock.restart();
					}
				}
			}
			break;
		}
	}
}