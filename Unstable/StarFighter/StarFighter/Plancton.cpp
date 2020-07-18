#include "Plancton.h"

extern Game* CurrentGame;

using namespace sf;

Plancton::Plancton()
{
	Plancton::Init();
}

Plancton::Plancton(sf::Vector2f position)
{
	GameObject::Init(position, sf::Vector2f(0, 0), "2D/boid.png", sf::Vector2f(32, 32));
	Plancton::Init();

	ScaleObject(PLANCTON_SCALE);
}

void Plancton::Init()
{
	m_layer = PlanctonLayer;
	m_collider_type = PlanctonObject;

	//random speed, direction and color
	m_randomized_speed = RandomizeFloatBetweenValues(sf::Vector2f(FLOCKING_BASE_SPEED_MIN, FLOCKING_BASE_SPEED_MAX));
	float angle_ = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	SetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, angle_ * M_PI / 180);

	setRotation(angle_ * 180 / M_PI - 180);

	int r = 30;// RandomizeIntBetweenValues(50, 200);
	int g = 170;// RandomizeIntBetweenValues(50, 255);
	int b = 70;// RandomizeIntBetweenValues(50, 255);
	setColor(sf::Color(r, g, b, 255));
	m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(BOID_MIN_CHANGE_DIR_TIME, BOID_MAX_CHANGE_DIR_TIME));
}

Plancton::~Plancton()
{
	
}

void Plancton::update(sf::Time deltaTime)
{
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

	/*
	if (!bounced)
	{
		//Change direction randomly
		sf::Vector2f change_dir = sf::Vector2f(0, 0);
		m_change_dir_time = RandomizeFloatBetweenValues(sf::Vector2f(BOID_MIN_CHANGE_DIR_TIME, BOID_MAX_CHANGE_DIR_TIME));
		m_change_dir_clock.restart();

		int dir = RandomizeIntBetweenValues(0, 1);
		dir = dir == 0 ? -1 : 1;
		float angle_increment = RandomizeFloatBetweenValues(sf::Vector2f(dir*BOID_MIN_CHANGE_DIR_ANGLE, dir*BOID_MAX_CHANGE_DIR_ANGLE));

		//correction to avoid screen borders
		float border_size = 200.0f;
		if (getPosition().x < border_size)
		{
			if (m_speed.y < 0)
			{
				if (angle_increment < 0)
				{
					angle_increment = -angle_increment;
				}
			}
			if (m_speed.y > 0)
			{
				if (angle_increment > 0)
				{
					angle_increment = -angle_increment;
				}
			}
		}

		if (getPosition().x > REF_WINDOW_RESOLUTION_X - border_size)
		{
			if (m_speed.y < 0)
			{
				if (angle_increment > 0)
				{
					angle_increment = -angle_increment;
				}
			}
			if (m_speed.y > 0)
			{
				if (angle_increment < 0)
				{
					angle_increment = -angle_increment;
				}
			}
		}

		if (getPosition().y < border_size)
		{
			if (m_speed.x < 0)
			{
				if (angle_increment > 0)
				{
					angle_increment = -angle_increment;
				}
			}
			if (m_speed.x > 0)
			{
				if (angle_increment < 0)
				{
					angle_increment = -angle_increment;
				}
			}
		}

		if (getPosition().y > REF_WINDOW_RESOLUTION_Y - border_size)
		{
			if (m_speed.x < 0)
			{
				if (angle_increment < 0)
				{
					angle_increment = -angle_increment;
				}
			}
			if (m_speed.x > 0)
			{
				if (angle_increment > 0)
				{
					angle_increment = -angle_increment;
				}
			}
		}
					
		float angle_ = getRotation() + angle_increment;
		change_dir = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_randomized_speed, angle_ / 180 * M_PI);

		m_speed.x = change_dir.x;
		m_speed.y = change_dir.y;
	}*/
	
	UpdateRotation();

	GameObject::update(deltaTime);
}