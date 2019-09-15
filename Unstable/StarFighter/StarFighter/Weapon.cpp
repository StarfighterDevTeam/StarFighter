#include "Weapon.h"

extern Game* CurrentGame;

using namespace sf;

Weapon::Weapon(SpatialObject* owner, WeaponType weapon_type, AmmoType ammo_type, ColliderType collider, LayerType layer, sf::Vector2f weapon_offset, float heading_offset)
{
	m_owner = owner;
	m_weapon_type = weapon_type;
	m_ammo_type = ammo_type;
	m_collider = collider;
	m_layer = layer;
	m_weapon_offset = weapon_offset;
	m_heading_offset = heading_offset;
	m_rate_of_fire_timer = 0;

	m_locked_target = NULL;
	m_locking_target_clock = 0;

	m_lock_rectangle.setOutlineColor(sf::Color::Green);
	m_lock_rectangle.setOutlineThickness(-2);
	m_lock_rectangle.setFillColor(sf::Color(0, 0, 0, 0));

	switch (weapon_type)
	{
		case Weapon_Laser:
		{
			m_rate_of_fire = 0.2;
			m_range = REF_WINDOW_RESOLUTION_X * 0.4;
			m_damage = 1;
			m_energy_cost = 1;
			break;
		}
		case Weapon_Missile:
		{
			m_rate_of_fire = 0.8;
			m_range = REF_WINDOW_RESOLUTION_X * 4;
			m_damage = 5;
			m_energy_cost = 2;
			break;
		}
	}
}

Weapon::~Weapon()
{

}

void Weapon::Fire()
{
	Ammo* ammo = new Ammo(m_ammo_type, m_position, m_heading, m_range, m_damage, m_collider);
	(*CurrentGame).addToScene(ammo, m_layer, m_collider, true);

	m_rate_of_fire_timer = m_rate_of_fire;

	//position offset
	ammo->m_position.x += ammo->m_size.y * 0.5 * cos((m_owner->m_heading + m_heading_offset) * M_PI / 180 + M_PI_2);
	ammo->m_position.y -= ammo->m_size.y * 0.5 * sin((m_owner->m_heading + m_heading_offset) * M_PI / 180 + M_PI_2);
}

void Weapon::Update(sf::Time deltaTime)
{
	//position offset
	m_position.x = m_owner->m_position.x + cos(-m_owner->m_heading * M_PI / 180 + M_PI_2) * m_weapon_offset.y + sin(-m_owner->m_heading * M_PI / 180 + M_PI_2) * m_weapon_offset.x;
	m_position.y = m_owner->m_position.y + sin(-m_owner->m_heading * M_PI / 180 + M_PI_2) * m_weapon_offset.y - cos(-m_owner->m_heading * M_PI / 180 + M_PI_2) * m_weapon_offset.x;;

	//heading
	m_heading = m_owner->m_heading + m_heading_offset;

	//rate of fire
	if (m_rate_of_fire_timer > 0)
		m_rate_of_fire_timer -= deltaTime.asSeconds();
	else	
		m_rate_of_fire_timer = 0;
}

bool Weapon::IsReadyToFire()
{
	bool rate_of_fire = m_rate_of_fire_timer <= 0;

	return rate_of_fire;
}

bool Weapon::IsTargetAligned(SpatialObject* target, float angle_tolerance)
{
	const float dx = target->m_position.x - m_position.x;
	const float dy = target->m_position.y - m_position.y;
	if (dx*dx + dy*dy <= m_range * m_range && abs(GetAngleDegToTargetPosition(m_position, m_heading, target->m_position)) <= angle_tolerance)
		return true;
	else
		return false;
}