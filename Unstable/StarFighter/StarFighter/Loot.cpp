#include "Loot.h"

extern Game* CurrentGame;

using namespace sf;

Loot::Loot(int money) : SpatialObject()
{
	m_money = money;
	m_layer = AIShipLayer;
	m_collider = LootObject;
	m_speed_max = 200;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/crystal.png", sf::Vector2f(32, 32), 1, 1);
}

Loot::~Loot()
{
	
}

void Loot::Update(sf::Time deltaTime)
{
	SpatialObject::Update(deltaTime);
}

void Loot::GetHitByObject(GameObject* object)
{
	
}