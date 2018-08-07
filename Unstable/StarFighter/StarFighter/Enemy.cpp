#include "Enemy.h"

extern Game* CurrentGame;

using namespace sf;

#define FLOCKING_PREVIOUS_SPEED_WEIGHT		5.0f
#define FLOCKING_COHESION_WEIGHT			0.8f
#define FLOCKING_ALIGNMENT_WEIGHT			0.15f
#define FLOCKING_SEPARATION_WEIGHT			2.0f
#define FLOCKING_SEPARATION_RADIUS			50.f

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_hp_max = 2;
	m_hp = m_hp_max;
	m_dmg = 1;
	m_melee_cooldown = 2.f;
	m_ref_speed = 150.f;

	m_is_attacking = false;
	//m_weapon = new Weapon(this, Weapon_Katana, sf::Color::Red);
	m_weapon = new Weapon(this, Weapon_Shuriken, sf::Color::Red);
	(*CurrentGame).addToScene(m_weapon, EnemyWeaponLayer, EnemyWeaponObject);

	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	//SetSpeedVectorFromAbsoluteSpeedAndAngle(200, angle * M_PI / 180);
}

Enemy::~Enemy()
{
	GameObject* playerShip = (GameObject*)(*CurrentGame).m_playerShip;
	if (playerShip->GetDashEnemy() == this)
	{
		playerShip->SetDashEnemy(NULL);
	}

	if (m_weapon)
	{
		m_weapon->m_GarbageMe = true;
		m_weapon->m_visible = false;
	}
}

sf::Vector2f Enemy::FlockingGetAveragePosition()
{
	int size = m_flocking_neighbours.size();
	float sum_x = 0;
	float sum_y = 0;
	
	if (m_flocking_neighbours.empty())
	{
		return getPosition();
	}

	for (GameObject* neighbour : m_flocking_neighbours)
	{
		if (neighbour)
		{
			sum_x += neighbour->getPosition().x;
			sum_y += neighbour->getPosition().y;
		}
	}

	sum_x /= 1.0f * size;
	sum_y /= 1.0f * size;

	return sf::Vector2f(sum_x, sum_y);
}

sf::Vector2f Enemy::FlockingGetAverageSpeed()
{
	int size = m_flocking_neighbours.size();
	float sum_x = 0;
	float sum_y = 0;

	if (m_flocking_neighbours.empty())
	{
		return m_speed;
	}

	for (GameObject* neighbour : m_flocking_neighbours)
	{
		if (neighbour)
		{
			sum_x += neighbour->m_speed.x;
			sum_y += neighbour->m_speed.y;
		}
	}

	sum_x /= 1.0f * size;
	sum_y /= 1.0f * size;

	return sf::Vector2f(sum_x, sum_y);
}

sf::Vector2f Enemy::FlockingSeparate()
{
	if (m_flocking_neighbours.empty())
	{
		return m_speed;
	}

	int count = 0;
	float sum_x = 0;
	float sum_y = 0;

	for (GameObject* neighbour : m_flocking_neighbours)
	{
		if (neighbour)
		{
			if (GetDistanceBetweenObjects(this, neighbour) < FLOCKING_SEPARATION_RADIUS)
			{
				sf::Vector2f vector = sf::Vector2f(neighbour->getPosition().x - getPosition().x, neighbour->getPosition().y - getPosition().y);
				sum_x -= vector.x;
				sum_y -= vector.y;
				count++;
			}
		}
	}

	if (count == 0)
	{
		return m_speed;
	}

	sum_x /= 1.f * count;
	sum_y /= 1.f * count;

	return sf::Vector2f(sum_x, sum_y);
}

sf::Vector2f Enemy::Flocking()
{
	//Flocking
	//1. cohesion
	sf::Vector2f avg_position = FlockingGetAveragePosition();
	sf::Vector2f cohesion_vector = sf::Vector2f(avg_position.x - getPosition().x, avg_position.y - getPosition().y);
	NormalizeVector(&cohesion_vector, m_ref_speed);

	//2. alignment
	sf::Vector2f avg_speed = FlockingGetAverageSpeed();
	sf::Vector2f alignment_vector = sf::Vector2f(avg_speed.x - m_speed.x, avg_speed.y - m_speed.y);
	NormalizeVector(&alignment_vector, m_ref_speed);

	//3. separation
	sf::Vector2f separation_vector = FlockingSeparate();
	NormalizeVector(&separation_vector, m_ref_speed);

	//TOTAL
	sf::Vector2f final_vector;
	final_vector = m_speed * FLOCKING_PREVIOUS_SPEED_WEIGHT + cohesion_vector * FLOCKING_COHESION_WEIGHT + alignment_vector * FLOCKING_ALIGNMENT_WEIGHT + separation_vector * FLOCKING_SEPARATION_WEIGHT;

	NormalizeVector(&final_vector, m_ref_speed);

	return final_vector;
}

void Enemy::update(sf::Time deltaTime)
{
	//bounce on screen borders
	bool bounced = BounceOnBorders((*CurrentGame).m_map_size);

	if (m_weapon->m_owner == NULL)
	{
		printf("BUG\n");//chasing a potential bug
	}

	//Chase player
	GameObject* player = (GameObject*)(*CurrentGame).m_playerShip;

	//Flocking?
	sf::Vector2f flocking_vector = m_flocking ? Flocking() : sf::Vector2f(0, 0);

	//Follow player
	SetSpeedForConstantSpeedToDestination(player->getPosition() + flocking_vector, m_ref_speed);

	//Melee
	if (m_is_attacking)//reset
	{
		if (m_weapon && m_weapon->m_attack_clock.getElapsedTime().asSeconds() > m_weapon->m_attack_duration)
		{
			m_weapon->m_visible = false;
			m_weapon->Extend(sf::Vector2f(0.f, 1.f));
			m_weapon->m_enemies_tagged.clear();
			m_weapon->setColor(m_weapon->m_color);

			if (m_weapon->m_is_ranged && m_weapon->m_bullet_is_unique && m_weapon->GetBulletFiredCount() > 0)
			{
				//do nothing
			}
			else
			{
				m_is_attacking = false;
				m_melee_cooldown_clock.restart();
			}
		}
	}

	if (m_melee_cooldown_clock.getElapsedTime().asSeconds() > m_melee_cooldown)//condition to start attack
	{
		if (m_weapon && !m_is_attacking)
		{
			if (m_weapon->m_is_ranged == false)
			{
				m_weapon->m_visible = true;
				(*CurrentGame).PlaySFX(SFX_Melee);
			}
			else
			{
				m_weapon->Shoot(player);//shoot an enemy. If no enemy found, it will shoot towards current rotation.
			}

			m_is_attacking = true;
			m_weapon->m_attack_clock.restart();
		}
	}

	if (m_is_attacking)//update
	{
		float ratio = m_weapon->m_attack_clock.getElapsedTime().asSeconds() / m_weapon->m_attack_duration;
		if (ratio > 1.0f)
		{
			ratio = 1.0f;
		}
		m_weapon->Extend(sf::Vector2f(ratio, 1.f));
	}

	UpdateWeaponPosition(m_weapon);

	GameObject::update(deltaTime);

	UpdateRotation();
}

bool Enemy::DealDamage(int dmg)
{
	m_hp -= dmg;

	ostringstream ss;
	ss << "-" << dmg;
	(*CurrentGame).CreateSFTextPop(ss.str(), Font_Arial, 30, sf::Color::Blue, getPosition(), PlayerBlue, 100, 50, 2, NULL, -m_size.y*getScale().y / 2);

	if (dmg > 0)
	{
		(*CurrentGame).PlaySFX(SFX_GruntEnemy);
	}

	if (m_hp <= 0)
	{
		Death();
		return true;
	}

	return false;
}

void Enemy::Death()
{
	m_GarbageMe = true;
	m_visible = false;
}

int Enemy::GetRating()
{
	int rating = 0;

	rating += m_hp_max;
	rating += m_dmg;

	return rating;
}