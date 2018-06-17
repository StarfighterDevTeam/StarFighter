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
	float speed_ = RandomizeFloatBetweenValues(sf::Vector2f(PREDATOR_BASE_SPEED_MIN, PREDATOR_BASE_SPEED_MAX));
	float angle_ = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	SetSpeedVectorFromAbsoluteSpeedAndAngle(speed_, angle_ * M_PI / 180);
	m_previous_speed = m_speed;
	setRotation(angle_ * 180 / M_PI - 180);
	int r = RandomizeIntBetweenValues(0, 255);
	int g = RandomizeIntBetweenValues(0, 255);
	int b = RandomizeIntBetweenValues(0, 255);
	setColor(sf::Color(r, g, b, 255));

	float predator_scale = 5.f;
	setScale(sf::Vector2f(predator_scale, predator_scale));
	m_size.x *= predator_scale;
	m_size.y *= predator_scale;
	m_diag *= predator_scale;
}

void Predator::Init()
{
	m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(PREDATOR_MIN_CHANGE_DIR_TIME, PREDATOR_MAX_CHANGE_DIR_TIME));
}

Predator::~Predator()
{
	
}

void Predator::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);

	//UpdateThreats();

	//Sum up vectors and normalize speed
	m_previous_speed.x = m_speed.x;
	m_previous_speed.y = m_speed.y;

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

			change_dir = GetSpeedVectorFromAbsoluteSpeedAndAngle(GetAbsoluteSpeed(), angle_ / 180 * M_PI);

			m_speed.x = change_dir.x;
			m_speed.y = change_dir.y;
		}
		
		//printf("speed: (%f, %f) | avoid (%f, %f)", m_speed.x, m_speed.y, avoid_borders.x * AVOID_BORDERS_WEIGHT, avoid_borders.y * AVOID_BORDERS_WEIGHT);

		
			m_speed.x = m_speed.x + avoid_borders.x * PREDATOR_AVOID_BORDERS_WEIGHT;
			m_speed.y = m_speed.y + avoid_borders.y * PREDATOR_AVOID_BORDERS_WEIGHT;
		
		//printf(" new speed: (%f, %f)\n", m_speed.x, m_speed.y);
		

		NormalizeSpeed(&m_speed, PREDATOR_MAX_SPEED);

		//printf(" norm speed: (%f, %f)\n", m_speed.x, m_speed.y);
	}
	
	float angle = GetAngleRadForSpeed(m_speed);
	setRotation(angle * 180 / M_PI);
}