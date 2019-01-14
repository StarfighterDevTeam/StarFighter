#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(WeaponType type) : GameEntity(UI_Weapon)
{
	m_type = type;
	m_angle = 0.f;

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/cannon_icon.png", (int)ROOMTILE_SIZE, (int)ROOMTILE_SIZE * 2);

	setAnimation(texture, 1, 2);
	setRotation(-90.f);

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

bool Weapon::Fire(Time deltaTime, sf::Vector2f ship_position, float ship_angle, float distance_combat, Ship* target_ship, RoomTile* target_tile)
{
	//Fire from room tile
	Ammo* new_ammo = new Ammo(Ammo_CannonBall, m_position, m_angle, distance_combat, target_ship, target_tile);
	(*CurrentGame).m_bullets.push_back(new_ammo);

	return true;
}