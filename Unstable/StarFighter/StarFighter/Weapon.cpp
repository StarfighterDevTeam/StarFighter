#include "Weapon.h"

extern Game* CurrentGame;

using namespace sf;

Weapon::Weapon(GameObject* owner, WeaponType weapon_type, AmmoType ammo_type, ColliderType collider, LayerType layer, sf::Vector2f polar_offset, float heading_offset)
{
	m_owner = owner;
	m_weapon_type = weapon_type;
	m_ammo_type = ammo_type;
	m_collider = collider;
	m_layer = layer;
	m_polar_offset = polar_offset;
	m_heading_offset = heading_offset;
	m_rate_of_fire_timer = 0;

	m_locked_target = NULL;
	m_locking_target_clock = 0;
	m_locking_angle_coverage = 60;

	m_lock_rectangle.setOutlineColor(sf::Color::Green);
	m_lock_rectangle.setOutlineThickness(-2);
	m_lock_rectangle.setFillColor(sf::Color(0, 0, 0, 0));

	m_range = REF_WINDOW_RESOLUTION_X * 3;

	switch (weapon_type)
	{
		case Weapon_Laser:
		{
			m_rate_of_fire = 0.1;
			break;
		}
	}
}

Weapon::~Weapon()
{

}

void Weapon::Fire()
{
	Ammo* ammo = new Ammo(m_ammo_type, m_position, m_heading, m_range, m_collider, m_locked_target);
	(*CurrentGame).addToScene(ammo, m_layer, m_collider);

	m_rate_of_fire_timer = m_rate_of_fire;

	//position offset
	ammo->m_position.x += ammo->m_size.y * 0.5 * cos(m_owner->m_heading * M_PI / 180 + M_PI_2);
	ammo->m_position.y -= ammo->m_size.y * 0.5 * sin(m_owner->m_heading * M_PI / 180 + M_PI_2);
}

void Weapon::Update(sf::Time deltaTime)
{
	//position offset
	m_position.x = m_owner->m_position.x + cos(- m_owner->m_heading * M_PI / 180 + M_PI_2) * m_owner->m_size.y * 0.5;
	m_position.y = m_owner->m_position.y + sin(-m_owner->m_heading * M_PI / 180 + M_PI_2) * m_owner->m_size.y * 0.5;

	//heading
	m_heading = m_owner->m_heading + m_heading_offset;

	//rate of fire
	if (m_rate_of_fire_timer > 0)
		m_rate_of_fire_timer -= deltaTime.asSeconds();
	else	
		m_rate_of_fire_timer = 0;
	

	//locking target
	if (m_locked_target == NULL)
	{
		ColliderType collider = m_collider == PlayerFire ? EnemyShip : PlayerShipObject;
		GameObject* locked_target = (*CurrentGame).GetClosestObjectTyped(m_position, collider, m_range, m_locking_angle_coverage / 2);

		if (locked_target != NULL)
		{
			m_locking_target_clock += deltaTime.asSeconds();
			if (m_locking_target_clock >= 1)
			{
				//lock target
				m_locked_target = locked_target;

				m_lock_rectangle.setSize(sf::Vector2f((m_locked_target->m_radius * 2) + 2, (m_locked_target->m_radius * 2) + 2));
				m_lock_rectangle.setOrigin(sf::Vector2f(m_locked_target->m_radius + 1, m_locked_target->m_radius + 1));
				m_lock_rectangle.setPosition(m_locked_target->getPosition());
			}
		}
		else
		{
			m_locking_target_clock = 0;
		}
	}
	else
	{
		m_lock_rectangle.setPosition(m_locked_target->getPosition());

		if (CanStayLocked(m_locked_target) == false)
		{
			m_locked_target = NULL;
			m_locking_target_clock = 0;
		}
	}
}

bool Weapon::IsReadyToFire()
{
	bool rate_of_fire = m_rate_of_fire_timer <= 0;

	return rate_of_fire;
}

bool Weapon::CanStayLocked(GameObject* object)
{
	if (object->m_visible == false || object->m_garbageMe == true)
	{
		return false;
	}

	float angle_delta = GetAngleDegToTargetPosition(m_position, m_heading, object->getPosition());
	if (abs(angle_delta) > m_locking_angle_coverage)
	{
		return false;
	}

	float dist_squared = GetDistanceSquaredBetweenPositions(m_position, object->getPosition());
	if (dist_squared > m_range * m_range)
	{
		return false;
	}

	return true;
}