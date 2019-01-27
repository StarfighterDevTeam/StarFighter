#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(WeaponType type, bool is_enemy) : GameEntity(UI_Weapon)
{
	m_type = type;
	m_angle = is_enemy == false ? 90.f : -90.f;
	m_health_max = CANNON_HEALTH_MAX;
	m_health = m_health_max;
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

	m_lifebar = new GameEntity(UI_None);
	m_lifebar->m_shape_container.setSize(sf::Vector2f(CREWMEMBER_LIFEBAR_SIZE_X, CREWMEMBER_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape_container.setOrigin(sf::Vector2f(CREWMEMBER_LIFEBAR_SIZE_X * 0.5f, CREWMEMBER_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_lifebar->m_shape_container.setOutlineThickness(1.f);
	m_lifebar->m_shape_container.setOutlineColor(sf::Color::Black);

	m_lifebar->m_shape.setSize(sf::Vector2f(CREWMEMBER_LIFEBAR_SIZE_X, CREWMEMBER_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape.setOrigin(sf::Vector2f(CREWMEMBER_LIFEBAR_SIZE_X * 0.5f, CREWMEMBER_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape.setFillColor(sf::Color::Green);

	m_rofbar = new GameEntity(UI_None);
	m_rofbar->m_shape_container.setSize(sf::Vector2f(CREWMEMBER_LIFEBAR_SIZE_X, CREWMEMBER_LIFEBAR_SIZE_Y));
	m_rofbar->m_shape_container.setOrigin(sf::Vector2f(CREWMEMBER_LIFEBAR_SIZE_X * 0.5f, CREWMEMBER_LIFEBAR_SIZE_Y * 0.5f));
	m_rofbar->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_rofbar->m_shape_container.setOutlineThickness(1.f);
	m_rofbar->m_shape_container.setOutlineColor(sf::Color::Black);

	m_rofbar->m_shape.setSize(sf::Vector2f(CREWMEMBER_LIFEBAR_SIZE_X, CREWMEMBER_LIFEBAR_SIZE_Y));
	m_rofbar->m_shape.setOrigin(sf::Vector2f(CREWMEMBER_LIFEBAR_SIZE_X * 0.5f, CREWMEMBER_LIFEBAR_SIZE_Y * 0.5f));
	m_rofbar->m_shape.setFillColor(sf::Color(255, 242, 39, 255));//yellow "rof");

	//weapon stats
	m_direct_fire = true;
}

Weapon::~Weapon()
{
	delete m_lifebar;
	delete m_rofbar;
}

void Weapon::Update(Time deltaTime)
{
	UpdateRof(deltaTime);

	GameEntity::Update(deltaTime);

	//UI lifebar
	m_lifebar->m_shape_container.setPosition(m_position.x, m_position.y - m_size.y * 0.5f - CREWMEMBER_LIFEBAR_OFFSET_Y);
	m_lifebar->m_shape.setPosition(m_position.x, m_position.y - m_size.y * 0.5f - CREWMEMBER_LIFEBAR_OFFSET_Y);

	int health = m_health;
	Bound(health, sf::Vector2i(0, m_health_max));

	float life_ratio = 1.0f * health / m_health_max;
	m_lifebar->m_shape.setSize(sf::Vector2f(life_ratio * CREWMEMBER_LIFEBAR_SIZE_X, CREWMEMBER_LIFEBAR_SIZE_Y));

	float threshold[3] = { 0.7, 0.5, 0.2 };
	if (life_ratio >= threshold[1])
	{
		m_lifebar->m_shape.setFillColor(sf::Color::Green);
	}
	else if (life_ratio >= threshold[2])
	{
		m_lifebar->m_shape.setFillColor(sf::Color(255, 127, 39, 255));//orange "damaged"
	}
	else
	{
		m_lifebar->m_shape.setFillColor(sf::Color::Red);
	}

	//UI rofbar
	m_rofbar->m_shape_container.setPosition(m_position.x, m_position.y - m_size.y * 0.5f - CREWMEMBER_LIFEBAR_OFFSET_Y - (CREWMEMBER_LIFEBAR_SIZE_Y * 0.5f * 2));
	m_rofbar->m_shape.setPosition(m_position.x, m_position.y - m_size.y * 0.5f - CREWMEMBER_LIFEBAR_OFFSET_Y - (CREWMEMBER_LIFEBAR_SIZE_Y * 0.5f * 2));

	float rof = m_rof - m_rof_timer;
	Bound(rof, sf::Vector2f(0, m_rof));

	float rof_ratio = 1.0f * rof / m_rof;
	m_rofbar->m_shape.setSize(sf::Vector2f(rof_ratio * CREWMEMBER_LIFEBAR_SIZE_X, CREWMEMBER_LIFEBAR_SIZE_Y));
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