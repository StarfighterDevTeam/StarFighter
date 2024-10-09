#include "Asteroid.h"

extern Game* CurrentGame;

using namespace sf;

Asteroid::Asteroid(sf::Vector2i sector_index, AsteroidType asteroid_type) : Destructible()
{	
	m_hostility = Hostility_Enemy;
	m_layer = AsteroidLayer;
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
			m_speed_max = RandomizeFloatBetweenValues(30.f, 100.f);
			break;
		}
		case Asteroid_XS:
		{
			textureName = "2D/asteroid_XS.png";
			textureSize = sf::Vector2f(39, 40);
			m_collision_damage = 2;
			m_health_max = 1;
			m_chance_of_loot = 0;
			m_speed_max = RandomizeFloatBetweenValues(30.f, 100.f);
			break;
		}
		case Asteroid_S:
		{
			textureName = "2D/asteroid_XS.png";
			textureSize = sf::Vector2f(80, 73);
			m_collision_damage = 4;
			m_health_max = 4;
			m_chance_of_loot = 0;
			m_speed_max = RandomizeFloatBetweenValues(30.f, 100.f);
			break;
		}
		case Asteroid_M:
		{
			textureName = "2D/asteroid_M.png";
			textureSize = sf::Vector2f(120.f, 120.f);
			m_collision_damage = 8;
			m_health_max = 16;
			m_chance_of_loot = 0.01f;
			m_speed_max = RandomizeFloatBetweenValues(20.f, 80.f);
			break;
		}
		case Asteroid_L:
		{
			textureName = "2D/asteroid_L.png";
			textureSize = sf::Vector2f(200.f, 224.f);
			m_collision_damage = 8;
			m_health_max = 32;
			m_chance_of_loot = 0.05f;
			m_speed_max = RandomizeFloatBetweenValues(20.f, 80.f);
			break;
		}
		case Asteroid_XL:
		{
			textureName = "2D/asteroid_XL.png";
			textureSize = sf::Vector2f(240.f, 300.f);
			m_collision_damage = 16;
			m_health_max = 64;
			m_chance_of_loot = 0.1f;
			m_speed_max = RandomizeFloatBetweenValues(10.f, 40.f);
			break;
		}
		case Asteroid_XXL:
		{
			textureName = "2D/asteroid_XXL.png";
			textureSize = sf::Vector2f(280.f, 304.f);
			m_collision_damage = 16;
			m_health_max = 64;
			m_chance_of_loot = 0.1f;
			m_speed_max = RandomizeFloatBetweenValues(10.f, 40.f);
			break;
		}
	}

	Init(m_position, m_speed, textureName, textureSize, 1, 1);

	(*CurrentGame).SetStarSectorIndex(this, sector_index);

	m_rotation_speed = RandomizeFloatBetweenValues(3.f, 15.f) * RandomizeSign();
	m_heading = RandomizeFloatBetweenValues(0.f, 359.9f);
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