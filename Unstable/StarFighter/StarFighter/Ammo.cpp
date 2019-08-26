#include "Ammo.h"

extern Game* CurrentGame;

using namespace sf;

Ammo::Ammo(AmmoType ammo_type, sf::Vector2f position, float heading, float range, ColliderType collider, GameObject* locked_target)
{
	m_ammo_type = ammo_type;
	m_locked_target = locked_target;
	string textureName;
	sf::Vector2f textureSize;
	int frameNumber = 1;
	int animationNumber = 1;

	switch (m_ammo_type)
	{
		case Ammo_LaserGreen:
		{
			m_speed_min = 3000;
			m_speed_max = 3000;
			m_acceleration = 0;
			textureSize = sf::Vector2f(6, 32);
			textureName = "2D/laser_green.png";
			break;
		}
		case Ammo_LaserRed:
		{
			m_speed_min = 400;
			m_speed_max = 400;
			m_acceleration = 0;
			textureSize = sf::Vector2f(6, 32);
			textureName = "2D/laser_red.png";
			break;
		}
		case Ammo_Missile:
		{
			m_speed_min = 10;
			m_speed_max = 3000;
			m_acceleration = 600;
			textureSize = sf::Vector2f(6, 32);
			textureName = "2D/missile.png";
			break;
		}
	}

	m_speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_speed_min, (heading + 180) * M_PI / 180);
	m_lifespan = range / m_speed_max;

	Init(position, m_speed, textureName, textureSize, frameNumber, animationNumber);

	m_heading = heading;
}

Ammo::~Ammo()
{

}

void Ammo::Update(sf::Time deltaTime)
{
	m_lifespan -= deltaTime.asSeconds();

	if (m_lifespan <= 0)
	{
		m_garbageMe = true;
		m_visible = false;
	}
	else
	{
		//get target
		/*
		if (m_locked_target != NULL)
		{
			if (m_locked_target->m_garbageMe == false)
			{
				float delta_angle = GetAngleDegToTargetPosition(getPosition(), m_heading, m_locked_target->getPosition());
				m_heading += delta_angle;
			}
			else
			{
				m_locked_target = NULL;
			}
		}
		*/

		//m_heading += inputs_direction.x * m_turn_speed * deltaTime.asSeconds();

		sf::Vector2f acceleration_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_acceleration, m_heading * M_PI / 180);
		m_speed -= acceleration_vector * deltaTime.asSeconds();
		NormalizeVector(&m_speed, m_speed_max);

		//apply speed
		NormalizeVector(&m_speed, m_speed_max);

		GameObject::Update(deltaTime);
	}
}