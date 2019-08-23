#include "Enemy.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Ship(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	
}

Enemy::Enemy(EnemyType enemy_type, sf::Vector2i sector_index, float heading)
{
	m_enemy_type = enemy_type;
	
	string textureName;
	sf::Vector2f textureSize;
	int frameNumber = 1;
	int animationNumber = 1;
	switch (m_enemy_type)
	{
		case Enemy_Alpha:
		{
			m_speed_max = 100;
			m_max_acceleration = 2000;
			m_turn_speed = 160;
			m_max_braking = 3000;
			m_idle_decelleration = 1000;

			textureName = "2D/V_Alpha2_red.png";
			textureSize = sf::Vector2f(68*3, 84);
			frameNumber = 3;

			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, EnemyFire, EnemyFireLayer, sf::Vector2f(0, m_size.y * 0.5)));
			break;
		}
	}

	SetStarSectorIndex(sector_index);
	m_speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_speed_max, (heading + 180) * M_PI / 180);

	Init(m_position, m_speed, textureName, textureSize, frameNumber, animationNumber);

	m_heading = heading;
}

void Enemy::Update(sf::Time deltaTime)
{
	ApplyFlightModel(deltaTime, sf::Vector2f(1, -1));

	for (Weapon* weapon : m_weapons)
	{
		weapon->Update(deltaTime);

		if (weapon->IsReadyToFire() == true)
		{
			//weapon->Fire();
		}	
	}

	Ship::Update(deltaTime);
}