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

	m_radar_homing_range = 0;
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
			m_acceleration = 800;
			m_radar_homing_range = 600;
			m_turn_speed = 40;
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
		const float radar_homing_angle = 180;

		//target still in homing radar?
		if (m_locked_target != NULL)
		{
			const float dx = m_locked_target->getPosition().x - getPosition().x;
			const float dy = m_locked_target->getPosition().y - getPosition().y;
			if (dx*dx + dy*dy > m_radar_homing_range * m_radar_homing_range || GetAngleDegToTargetPosition(getPosition(), m_heading, m_locked_target->getPosition()) > radar_homing_angle)
				m_locked_target = NULL;
		}

		//find a new target?
		if (m_radar_homing_range > 0 && m_locked_target == NULL)
			m_locked_target = (GameObject*)(*CurrentGame).m_playerShip->GetTargetableEnemyShip(this, m_radar_homing_range, radar_homing_angle);

		//track target
		if (m_locked_target != NULL)
		{
			//get ride of deleted target
			if (m_locked_target->m_garbageMe == false)
			{
				float delta_angle = GetAngleDegToTargetPosition(getPosition(), m_heading, m_locked_target->getPosition());

				if (abs(delta_angle) <= m_turn_speed * deltaTime.asSeconds())
					m_heading += delta_angle;
				else if (delta_angle > 0)
					m_heading += m_turn_speed * deltaTime.asSeconds();
				else 
					m_heading -= m_turn_speed * deltaTime.asSeconds();
			}
			else
			{
				m_locked_target = NULL;
			}
		}

		//sf::Vector2f acceleration_vector = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_acceleration, m_heading * M_PI / 180);
		//m_speed -= acceleration_vector * deltaTime.asSeconds();
		m_speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_acceleration, (m_heading + 180) * M_PI / 180);
		NormalizeVector(&m_speed, m_speed_max);

		GameObject::Update(deltaTime);
	}

}

void Ammo::Draw(RenderTarget& screen)
{
	screen.draw(*this);

	//Debug radar homing
	if (m_radar_homing_range > 0)
	{
		sf::Vector2f vector = m_speed;
		ScaleVector(&vector, m_radar_homing_range);
		sf::Vertex line[2];

		line[0].position = getPosition();
		line[1].position = sf::Vector2f(getPosition().x + vector.x, getPosition().y - vector.y);

		line[0].color = sf::Color::Red;
		line[1].color = sf::Color::Red;

		screen.draw(line, 2, sf::Lines);
	}
}