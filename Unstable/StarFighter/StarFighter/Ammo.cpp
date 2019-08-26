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

	m_radar_homing = false;
	m_turn_speed = 0;

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
			m_speed_max = 2000;
			m_acceleration = 600;
			m_radar_homing = true;
			m_turn_speed = 5000;
			textureSize = sf::Vector2f(24, 42);
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
		const float radar_homing_range = 1200;
		const float radar_homing_angle = 90;

		//target still in homing radar?
		if (m_locked_target != NULL)
		{
			const float dx = m_locked_target->getPosition().x - getPosition().x;
			const float dy = m_locked_target->getPosition().y - getPosition().y;
			if (dx*dx + dy*dy > radar_homing_range * radar_homing_range || GetAngleDegToTargetPosition(getPosition(), m_heading, m_locked_target->getPosition()) > radar_homing_angle)
				m_locked_target = NULL;
		}

		//find a new target?
		if (m_radar_homing == true && m_locked_target == NULL)
			m_locked_target = (GameObject*)(*CurrentGame).m_playerShip->GetTargetableEnemyShip(this, radar_homing_range, radar_homing_angle);

		//track target
		if (m_locked_target != NULL)
		{
			//get ride of deleted target
			if (m_locked_target->m_garbageMe == false)
			{
				float delta_angle = GetAngleDegToTargetPosition(getPosition(), m_heading, m_locked_target->getPosition());
				BoundAbsoluteValue(delta_angle, m_turn_speed);
				m_heading += delta_angle * deltaTime.asSeconds();

				//printf("target pos: %f, %f, ammo pos: %f, %f, delta_angle: %f\n", m_locked_target->getPosition().x, m_locked_target->getPosition().y, getPosition().x, getPosition().y, delta_angle);
			}
			else
			{
				m_locked_target = NULL;
			}
		}

		sf::Vector2f acceleration_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_acceleration, m_heading * M_PI / 180);
		m_speed -= acceleration_vector * deltaTime.asSeconds();
		NormalizeVector(&m_speed, m_speed_max);

		GameObject::Update(deltaTime);
	}
}