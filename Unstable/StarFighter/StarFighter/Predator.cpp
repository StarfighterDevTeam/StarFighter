#include "Predator.h"

extern Game* CurrentGame;

using namespace sf;

Predator::Predator()
{
	Predator::Init();
}

Predator::Predator(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	Predator::Init();

	//random speed, direction and color
	m_randomized_speed = RandomizeFloatBetweenValues(sf::Vector2f(PREDATOR_BASE_SPEED_MIN, PREDATOR_BASE_SPEED_MAX));
	float angle_ = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	SetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, angle_ * M_PI / 180);
	setRotation(angle_ * 180 / M_PI - 180);
	int r = RandomizeIntBetweenValues(0, 255);
	int g = RandomizeIntBetweenValues(0, 255);
	int b = RandomizeIntBetweenValues(0, 255);
	setColor(sf::Color(r, g, b, 255));
	m_prey = NULL;

	ScaleObject(PREDATOR_SCALE);
}

void Predator::Init()
{
	m_state = Predator_Swimming;
	m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(PREDATOR_MIN_CHANGE_DIR_TIME, PREDATOR_MAX_CHANGE_DIR_TIME));
}

Predator::~Predator()
{
	
}

void Predator::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);

	//bounce on screen borders
	bool bounced = false;
	if (getPosition().x - m_size.x / 2 < 0 && m_speed.x < 0)
	{
		m_speed.x *= -1;
		bounced = true;
	}
	if (getPosition().x + m_size.x / 2 > REF_WINDOW_RESOLUTION_X  && m_speed.x > 0)
	{
		m_speed.x *= -1;
		bounced = true;
	}
	if (getPosition().y - m_size.y / 2 < 0 && m_speed.y < 0)
	{
		m_speed.y *= -1;
		bounced = true;
	}
	if (getPosition().y + m_size.y / 2 > REF_WINDOW_RESOLUTION_Y && m_speed.y > 0)
	{
		m_speed.y *= -1;
		bounced = true;
	}

	if (!bounced)
	{
		if (m_prey)
		{
			//vector to prey
			sf::Vector2f chase_vector = sf::Vector2f(m_prey->getPosition().x - getPosition().x, m_prey->getPosition().y - getPosition().y);

			

			//chasing prey
			if (chase_vector != Vector2f(0, 0) && m_attack_cooldown_clock.getElapsedTime().asSeconds() > PREDATOR_ATTACK_COOLDOWN)
			{
				ScaleSpeed(&chase_vector, m_randomized_speed);
				m_speed = chase_vector;
				if (m_state == Predator_Swimming)
				{
					m_state = Predator_Chasing;
				}
			}
			else
			{
				if (m_state == Predator_Chasing)
				{
					m_state = Predator_Swimming;
				}
			}
			
			//attacking prey?
			float distance = GetDistanceBetweenPositions(getPosition(), m_prey->getPosition()) - m_size.y / 2 - m_prey->m_diag;
			if (m_state == Predator_Chasing && distance < PREDATOR_ATTACK_RADIUS && m_attack_cooldown_clock.getElapsedTime().asSeconds() > PREDATOR_ATTACK_COOLDOWN)
			{
				m_state = Predator_Attacking;
				m_attack_duration_clock.restart();
			}

			if (m_state == Predator_Attacking)
			{
				m_attack_cooldown_clock.restart();

				if (m_attack_duration_clock.getElapsedTime().asSeconds() < PREDATOR_ATTACK_DURATION)
				{
					ScaleSpeed(&m_speed, PREDATOR_ATTACK_SPEED);
				}
				else
				{
					m_state = Predator_Chasing;
					m_attack_duration_clock.restart();
				}
			}
		}
		else
		{
			m_state = Predator_Swimming;

			//Avoid Borders
			sf::Vector2f avoid_borders = AvoidBorders();

			//Change direction randomly
			sf::Vector2f change_dir = sf::Vector2f(0, 0);
			if (m_change_dir_clock.getElapsedTime().asSeconds() > m_change_dir_time)
			{
				m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(PREDATOR_MIN_CHANGE_DIR_TIME, PREDATOR_MAX_CHANGE_DIR_TIME));
				m_change_dir_clock.restart();

				int dir = RandomizeIntBetweenValues(0, 1);
				dir = dir == 0 ? -1 : 1;
				float angle_ = RandomizeFloatBetweenValues(sf::Vector2f(getRotation() + dir*PREDATOR_MIN_CHANGE_DIR_ANGLE, getRotation() + dir*PREDATOR_MAX_CHANGE_DIR_ANGLE)) - 180;

				change_dir = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, angle_ / 180 * M_PI);

				m_speed.x = change_dir.x;
				m_speed.y = change_dir.y;
			}

			m_speed.x = m_speed.x + avoid_borders.x * PREDATOR_AVOID_BORDERS_WEIGHT;
			m_speed.y = m_speed.y + avoid_borders.y * PREDATOR_AVOID_BORDERS_WEIGHT;

			ScaleSpeed(&m_speed, m_randomized_speed);
		}
	}
	
	float angle = GetAngleRadForSpeed(m_speed);
	setRotation(angle * 180 / M_PI);

	//printf("attack clock: %f | attack cooldown: %f | state: %d | speed: %f\n", m_attack_duration_clock.getElapsedTime().asSeconds(), m_attack_cooldown_clock.getElapsedTime().asSeconds(), m_state, GetAbsoluteSpeed());
}

void Predator::AddToPreys(GameObject* boid)
{
	m_prey = boid;
}

bool Predator::Eat(GameObject* prey)
{
	assert(prey != NULL);

	if (m_state == Predator_Attacking)
	{
		prey->m_GarbageMe = true;
		prey->m_visible = false;
		m_prey = NULL;
		(*CurrentGame).m_boids_eaten++;
		(*CurrentGame).m_boids_alive--;
		m_state = Predator_Swimming;
		ScaleSpeed(&m_speed, m_randomized_speed);
		printf("Boids alive: %d\n", (*CurrentGame).m_boids_alive);

		return true;
	}

	return false;
}

GameObject* Predator::GetPrey()
{
	return m_prey;
}

void Predator::SetPrey(GameObject* prey)
{
	m_prey = prey;
}