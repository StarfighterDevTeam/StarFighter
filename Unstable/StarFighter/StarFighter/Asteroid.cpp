#include "Asteroid.h"

extern Game* CurrentGame;

using namespace sf;

Asteroid::Asteroid(sf::Vector2i sector_index, AsteroidType asteroid_type) : Destructible()
{	
	m_hostility = Hostility_Enemy;
	m_layer = AIShipLayer;
	m_collider = DestructibleObject;
	m_asteroid_type = asteroid_type;

	string textureName;
	sf::Vector2f textureSize;

	switch (asteroid_type)
	{
		case Asteroid_XXS:
		{
			textureName = "2D/asteroid_XXS.png";
			textureSize = sf::Vector2f(30, 35);
			m_collision_damage = 1;
			m_health_max = 1;
			m_chance_of_loot = 0;
			m_speed_max = RandomizeIntBetweenValues(30, 100);
			break;
		}
		case Asteroid_XS:
		{
			textureName = "2D/asteroid_XS.png";
			textureSize = sf::Vector2f(39, 40);
			m_collision_damage = 2;
			m_health_max = 1;
			m_chance_of_loot = 0;
			m_speed_max = RandomizeIntBetweenValues(30, 100);
			break;
		}
		case Asteroid_S:
		{
			textureName = "2D/asteroid_XS.png";
			textureSize = sf::Vector2f(80, 73);
			m_collision_damage = 4;
			m_health_max = 4;
			m_chance_of_loot = 0;
			m_speed_max = RandomizeIntBetweenValues(30, 100);
			break;
		}
		case Asteroid_M:
		{
			textureName = "2D/asteroid_M.png";
			textureSize = sf::Vector2f(120, 120);
			m_collision_damage = 8;
			m_health_max = 16;
			m_chance_of_loot = 0.01;
			m_speed_max = RandomizeIntBetweenValues(20, 80);
			break;
		}
		case Asteroid_L:
		{
			textureName = "2D/asteroid_L.png";
			textureSize = sf::Vector2f(200, 224);
			m_collision_damage = 8;
			m_health_max = 32;
			m_chance_of_loot = 0.05;
			m_speed_max = RandomizeIntBetweenValues(20, 80);
			break;
		}
		case Asteroid_XL:
		{
			textureName = "2D/asteroid_XL.png";
			textureSize = sf::Vector2f(240, 300);
			m_collision_damage = 16;
			m_health_max = 64;
			m_chance_of_loot = 0.1;
			m_speed_max = RandomizeIntBetweenValues(10, 40);
			break;
		}
		case Asteroid_XXL:
		{
			textureName = "2D/asteroid_XXL.png";
			textureSize = sf::Vector2f(280, 304);
			m_collision_damage = 16;
			m_health_max = 64;
			m_chance_of_loot = 0.1;
			m_speed_max = RandomizeIntBetweenValues(10, 40);
			break;
		}
	}

	Init(m_position, m_speed, textureName, textureSize, 1, 1);

	(*CurrentGame).SetStarSectorIndex(this, sector_index);

	m_rotation_speed = RandomizeFloatBetweenValues(3, 15) * RandomizeSign();
	m_heading = RandomizeFloatBetweenValues(0, 359.9);
	//m_speed = sf::Vector2f(RandomizeFloatBetweenValues(-1, 1), RandomizeFloatBetweenValues(-1, 1));
	//ScaleVector(&m_speed, m_speed_max);

	m_health = m_health_max;
}

Asteroid::~Asteroid()
{
	
}

void Asteroid::Update(sf::Time deltaTime)
{
	m_heading += m_rotation_speed * deltaTime.asSeconds();

	Destructible::Update(deltaTime);
}

void Asteroid::GetHitByObject(GameObject* object)
{
	Destructible::GetHitByObject(object);

	if (object->m_radius > m_radius)
		m_garbageMe = true;
}

void Asteroid::Death()
{
	//create loot
	if (RandomizeFloatBetweenValues(0, 1) < m_chance_of_loot)
		CreateLoot(RandomizeIntBetweenValues(50, 100));

	Destructible::Death();
}