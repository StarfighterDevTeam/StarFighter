#include "Ammo.h"

extern Game* CurrentGame;

using namespace sf;

Ammo::Ammo(SpatialObject* owner, AmmoType ammo_type, sf::Vector2f position, float heading, float range, int damage)
{
	m_owner = owner;
	m_ammo_type = ammo_type;
	m_damage = damage;
	m_locked_target = NULL;
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
			m_FX_type = FX_Hit;
			break;
		}
		case Ammo_LaserRed:
		{
			m_speed_min = 1000;
			m_speed_max = 1000;
			m_acceleration = 0;
			textureSize = sf::Vector2f(6, 32);
			textureName = "2D/laser_red.png";
			m_FX_type = FX_Hit;
			break;
		}
		case Ammo_Missile:
		{
			m_speed_min = 10;
			m_speed_max = 2000;
			m_acceleration = 2000;
			m_radar_homing_range = 900;
			m_radar_homing_angle_max = 45;
			m_turn_speed = 50;
			textureSize = sf::Vector2f(24, 42);
			textureName = "2D/missile.png";
			break;
		}
	}

	m_speed = GetVectorFromLengthAndAngle(m_speed_min, (heading + 180) * M_PI / 180);
	m_lifespan = range / m_speed_max;

	Init(position, m_speed, textureName, textureSize, frameNumber, animationNumber);
	
	m_heading = heading;
	
	//update position and rotation "manually" because they won't be updated during the frame of their creation
	setPosition(sf::Vector2f(m_position.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_position.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
	setRotation(m_heading);
}

Ammo::~Ammo()
{
	
}

void Ammo::Update(sf::Time deltaTime)
{
	if (m_owner != NULL && m_owner->m_garbageMe == true)
		m_owner = NULL;

	m_lifespan -= deltaTime.asSeconds();

	if (m_lifespan <= 0)
	{
		m_garbageMe = true;
		m_visible = false;
	}
	else
	{
		//target still in homing radar?
		if (m_locked_target != NULL)
		{
			if (m_locked_target->m_garbageMe == true)
				m_locked_target = NULL;
			else
			{
				const float dx = m_locked_target->m_position.x - m_position.x;
				const float dy = m_locked_target->m_position.y - m_position.y;
				if (dx*dx + dy*dy > m_radar_homing_range * m_radar_homing_range || abs(GetAngleDegToTargetPosition(m_position, m_heading, m_locked_target->m_position)) > m_radar_homing_angle_max)
					m_locked_target = NULL;
			}
		}

		//find a new target?
		if (m_radar_homing_range > 0 && m_locked_target == NULL)
			m_locked_target = (*CurrentGame).GetTargetableEnemyShip(this, m_radar_homing_range, m_radar_homing_angle_max);

		//track target
		const float speed = GetVectorLength(m_speed) + m_acceleration * deltaTime.asSeconds();
		if (m_locked_target != NULL)
		{
			//get rid of deleted target
			if (m_locked_target->m_garbageMe == false)
			{
				const float turn_speed = m_turn_speed * speed / m_speed_max;
				const float delta_angle = GetAngleDegToTargetPosition(m_position, m_heading, m_locked_target->m_position);
				
				if (delta_angle != 0)
				{
					if (abs(delta_angle) <= m_turn_speed * deltaTime.asSeconds())
						m_heading -= delta_angle;
					else if (delta_angle > 0)
						m_heading -= m_turn_speed * deltaTime.asSeconds();
					else
						m_heading += m_turn_speed * deltaTime.asSeconds();						
				}
			}
			else
			{
				m_locked_target = NULL;
			}
		}

		m_speed = GetVectorFromLengthAndAngle(speed, (m_heading + 180) * M_PI / 180);
		NormalizeVector(&m_speed, m_speed_max);

		GameObject::Update(deltaTime);
	}
}

void Ammo::Draw(RenderTarget& screen)
{
	screen.draw(*this);

	//Debug radar homing
	if (m_locked_target != NULL)
	{
		sf::Vector2f vector = m_speed;
		ScaleVector(&vector, m_radar_homing_range);

		DebugDrawSegment(getPosition(), sf::Vector2f(getPosition().x + vector.x, getPosition().y - vector.y), sf::Color::Red, (*CurrentGame).m_mainScreen);
	}
}

void Ammo::Bounce(GameObject* target)
{
	const float dx = m_position.x - target->m_position.x;
	const float dy = m_position.y - target->m_position.y;

	float incoming_angle = 180 + m_heading;
	BoundAngle(incoming_angle, 360);

	const float radius_angle = GetAngleRadFromVector(sf::Vector2f(dx, dy)) * 180 / M_PI;;

	const int cross_product = dx * m_speed.x + dy * m_speed.y > 0 ? -1 : 1;

	m_heading = radius_angle + cross_product * (radius_angle - incoming_angle);

	//instant repositioning at collision point
	m_position = target->m_position + GetVectorFromLengthAndAngle(target->GetRadius(true) + m_size.y * 0.5, (radius_angle - 180) * M_PI / 180);
	SetPosition(sf::Vector2f(m_position.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_position.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
	setRotation(m_heading);
}

bool Ammo::IsAmmoOwnedByPlayer()
{
	return m_owner == (*CurrentGame).m_playerShip;
}