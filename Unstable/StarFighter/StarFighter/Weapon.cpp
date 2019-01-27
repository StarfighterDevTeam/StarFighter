#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(WeaponType type, bool is_enemy) : GameEntity(UI_Weapon)
{
	m_type = type;
	m_angle = is_enemy == false ? 90.f : -90.f;
	m_healt_max = CANNON_HEALTH_MAX;
	m_health = m_healt_max;
	m_ammo_type = Ammo_CannonBall;
	m_rof = CANNON_RATE_OF_FIRE;
	m_rof_timer = CANNON_RATE_OF_FIRE;
	m_angle_speed = CANNON_ANGLESPEED;
	m_target_room = NULL;
	m_tile = NULL;
	m_ship = NULL;

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/cannon_icon.png", (int)ROOMTILE_SIZE, (int)ROOMTILE_SIZE * 2);

	setAnimation(texture, 1, 2);
	setAnimationLine(is_enemy == false ? 0 : 1);

	//UI
	m_default_color = sf::Color(0, 0, 0, 0);

	m_shape_container.setSize(sf::Vector2f(m_size));
	m_shape_container.setOrigin(sf::Vector2f(m_size.x * 0.5f, m_size.y * 0.5f));
	m_shape_container.setOutlineThickness(1.f);
	m_shape_container.setOutlineColor(m_default_color);
	m_shape_container.setFillColor(m_default_color);

	//weapon stats
	m_direct_fire = true;
}

Weapon::~Weapon()
{
	
}

void Weapon::Update(Time deltaTime)
{
	UpdateRof(deltaTime);

	GameEntity::Update(deltaTime);
}

void Weapon::UpdateRof(Time deltaTime)
{
	if (m_rof_timer > 0)
	{
		m_rof_timer -= deltaTime.asSeconds();
	}
}

RoomTile* Weapon::GetFreeRoomTile(Room* room)
{
	for (vector<RoomTile*>::iterator it = room->m_tiles.begin(); it != room->m_tiles.end(); it++)
	{
		if (((*it)->m_crew == NULL && (*it)->m_weapon == NULL) || (*it)->m_weapon == this)
		{
			return *it;
		}
	}

	return NULL;
}

bool Weapon::Fire(float angle, float distance_combat, Ship* target_ship, RoomTile* target_tile)
{
	//reset rate of fire cooldown
	m_rof_timer = m_rof;

	//Fire from room tile
	Ammo* new_ammo = new Ammo(m_ammo_type, m_position, angle, distance_combat, target_ship, target_tile);
	(*CurrentGame).m_bullets.push_back(new_ammo);

	return true;
}

RoomTile* Weapon::GetFreeWeaponTile(Room* room, bool is_enemy)
{
	for (vector<RoomTile*>::iterator it = room->m_tiles.begin(); it != room->m_tiles.end(); it++)
	{
		if (is_enemy == false && (*it)->m_hull == Hull_Right && (*it)->m_crew == NULL && (*it)->m_weapon == NULL)
		{
			return *it;
		}
		else if (is_enemy == true && (*it)->m_hull == Hull_Left && (*it)->m_crew == NULL && (*it)->m_weapon == NULL)
		{
			return *it;
		}
	}

	for (vector<RoomTile*>::iterator it = room->m_tiles.begin(); it != room->m_tiles.end(); it++)
	{
		if (((*it)->m_hull == Hull_Left || (*it)->m_hull == Hull_Right) && (*it)->m_crew == NULL && (*it)->m_weapon == NULL)
		{
			return *it;
		}
	}

	return NULL;
}