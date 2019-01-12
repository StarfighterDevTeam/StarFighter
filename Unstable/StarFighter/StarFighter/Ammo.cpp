#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(AmmoType type) : GameEntity(UI_None)
{
	m_type = type;
	m_angle = 90.f;
	m_speed = sf::Vector2f(0, 0);
	m_ref_speed = CANNONBALL_SPEED;

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/cannonball.png", (int)CANNONBALL_SIZE, (int)CANNONBALL_SIZE);

	setAnimation(texture, 1, 1);
}

void Ammo::Update(Time deltaTime)
{
	//apply movement
	m_position.x += m_speed.x * deltaTime.asSeconds();
	m_position.y += m_speed.y * deltaTime.asSeconds();

	GameEntity::Update(deltaTime);
}