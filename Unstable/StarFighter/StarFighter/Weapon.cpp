#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(WeaponType type) : GameEntity(UI_Weapon)
{
	m_type = type;
	m_angle = 90.f;

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/cannon_icon.png", (int)ROOMTILE_SIZE, (int)ROOMTILE_SIZE * 2);

	setAnimation(texture, 1, 2);

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
	GameEntity::Update(deltaTime);
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

bool Weapon::Fire(Time deltaTime)
{
	//Create bullet
	Ammo* new_ammo = new Ammo(Ammo_CannonBall);
	m_angle = 90.f;
	new_ammo->m_angle = m_angle;
	new_ammo->setRotation(m_angle);

	float angle_rad = m_angle * M_PI / 180.f;
	new_ammo->m_speed = sf::Vector2f(new_ammo->m_ref_speed * sin(angle_rad), new_ammo->m_ref_speed * cos(angle_rad));

	//Fire from cannon
	float weapon_offset_x = (ROOMTILE_SIZE + new_ammo->m_size.x) * 0.5f * sin(angle_rad);
	float weapon_offset_y = (ROOMTILE_SIZE + new_ammo->m_size.y) * 0.5f * cos(angle_rad);
	new_ammo->m_position = getPosition() + sf::Vector2f(weapon_offset_x, weapon_offset_y);
	
	(*CurrentGame).m_bullets.push_back(new_ammo);

	//Fire from map
	//float map_offset_x = (ROOMTILE_SIZE * 0.5f) * sin(angle_rad) + new_ammo->m_size.x * 0.5f * sin(angle_rad);
	//float map_offset_y = (ROOMTILE_SIZE * 0.5f) * cos(angle_rad) + new_ammo->m_size.y * 0.5f * cos(angle_rad);
	//new_ammo->m_position = getPosition() + sf::Vector2f(bullet_offset_x, bullet_offset_y);

	//(*CurrentGame).m_bullets.push_back(new_ammo);


	return new_ammo;
}