#include "Enemy.h"

extern Game* CurrentGame;

using namespace sf;

#define FLOCKING_PREVIOUS_SPEED_WEIGHT		5.0f
#define FLOCKING_COHESION_WEIGHT			0.8f
#define FLOCKING_ALIGNMENT_WEIGHT			0.15f
#define FLOCKING_SEPARATION_WEIGHT			2.0f
#define FLOCKING_SEPARATION_RADIUS			50.f

#define ENEMY_ROAMING_MIN_DURATION			1.f
#define ENEMY_ROAMING_MAX_DURATION			5.f
#define ENEMY_ROAMING_MIN_DISTANCE			100.f
#define ENEMY_ROAMING_MAX_DISTANCE			400.f
#define ENEMY_DASH_DISTANCE					100.f
#define ENEMY_DASH_COOLDOWN					5.f

//Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
//{
//	m_hp_max = 2;
//	m_hp = m_hp_max;
//	m_dmg = 1;
//	m_attack_cooldown = 2.f;
//	m_ref_speed = 150.f;
//
//	m_is_attacking = false;
//	//m_weapon = new Weapon(this, Weapon_Katana, sf::Color::Red);
//	m_weapon = new Weapon(this, Weapon_Shuriken, sf::Color::Red);
//	(*CurrentGame).addToScene(m_weapon, EnemyWeaponLayer, EnemyWeaponObject);
//
//	//float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
//	//SetSpeedVectorFromAbsoluteSpeedAndAngle(200, angle * M_PI / 180);
//}

Enemy::Enemy(sf::Vector2f position, EnemyType type)
{
	m_type = type;
	m_phase = EnemyPhase_Idle;
	m_is_attacking = false;

	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0.f, 360.f));
	setRotation(angle);

	string textureName;
	sf::Vector2f size;
	int frameNumber = 1;
	int animationNumber = 1;

	switch (type)
	{
		case Enemy_Wufeng_Katana:
		case Enemy_Wufeng_Spear:
		case Enemy_Wufeng_Shuriken:
		{
			m_hp_max = 2;
			m_hp = m_hp_max;
			m_dmg = 1;
			m_attack_cooldown = 2.f;
			m_ref_speed = 200.f;
			textureName = "2D/wufeng.png";
			size = sf::Vector2f(64, 114);
			m_aggro_radius = 500.f;
			break;
		}
	}

	switch (type)
	{
		case Enemy_Wufeng_Katana:
		{
			m_weapon = new Weapon(this, Weapon_Katana, sf::Color::Red);
			setColor(sf::Color::Cyan);
			break;
		}
		case Enemy_Wufeng_Spear:
		{
			m_weapon = new Weapon(this, Weapon_Spear, sf::Color::Red);
			setColor(sf::Color::Red);
			break;
		}
		case Enemy_Wufeng_Shuriken:
		{
			m_weapon = new Weapon(this, Weapon_Shuriken, sf::Color::Red);
			setColor(sf::Color::Magenta);
			break;
		}
	}

	Init(position, sf::Vector2f(0, 0), textureName, size, frameNumber, animationNumber);

	m_DontGarbageMe = true;

	if (m_weapon)
	{
		(*CurrentGame).addToScene(m_weapon, EnemyWeaponLayer, EnemyWeaponObject);
	}

	//AI
	m_roam_duration = RandomizeFloatBetweenValues(sf::Vector2f(ENEMY_ROAMING_MIN_DURATION, ENEMY_ROAMING_MAX_DURATION));
	m_destination = getPosition();
	m_arrived_at_destination = true;

	//debug
	m_aggro_radius_feedback.setRadius(m_aggro_radius);
	m_aggro_radius_feedback.setOrigin(sf::Vector2f(m_aggro_radius, m_aggro_radius));
	m_aggro_radius_feedback.setFillColor(sf::Color(0, 0, 0, 0));
	m_aggro_radius_feedback.setOutlineThickness(2);
	m_aggro_radius_feedback.setOutlineColor(sf::Color(255, 0, 0, 80));
	m_aggro_radius_feedback.setPosition(getPosition());


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

void Enemy::UpdateAI(sf::Time deltaTime)
{
	GameObject* player = (GameObject*)(*CurrentGame).m_playerShip;

	float dist_squared_to_player = GetDistanceSquaredBetweenPositions(getPosition(), player->getPosition());
	if (dist_squared_to_player < m_aggro_radius * m_aggro_radius)
	{
		if (m_phase != EnemyPhase_FollowPlayer)
		{
			m_phase = EnemyPhase_FollowPlayer;
		}
	}
	else
	{
		if (m_phase != EnemyPhase_Idle)
		{
			m_phase = EnemyPhase_Idle;

			m_arrived_at_destination = true;
			m_speed = sf::Vector2f(0, 0);
			//m_roaming_clock.restart();
			printf("GO TO ROAMING\n");
		}
	}

	if (m_phase == EnemyPhase_Idle)
	{
		Roam(deltaTime);
	}

	if (m_phase == EnemyPhase_FollowPlayer)
	{
		if (FollowPlayer(player))
		{
			AttackPlayer(player);
		}
	}
}
void Enemy::update(sf::Time deltaTime)
{
	//bounce on screen borders
	bool bounced = BounceOnBorders((*CurrentGame).m_map_size);

	if (m_weapon->m_owner == NULL)
	{
		printf("BUG\n");//chasing a potential bug
	}

	//Flocking?
	//sf::Vector2f flocking_vector = m_flocking ? Flocking() : sf::Vector2f(0, 0);

	//AI behaviour
	if (m_is_attacking)//reset
	{
		if (m_weapon && m_weapon->m_attack_clock.getElapsedTime().asSeconds() > m_weapon->m_attack_duration)
		{
			m_weapon->m_visible = false;
			m_weapon->Extend(sf::Vector2f(0.f, 1.f));
			m_weapon->m_enemies_tagged.clear();
			m_weapon->setColor(m_weapon->m_color);

			if (m_weapon->m_is_ranged && m_weapon->m_bullet_is_unique && m_weapon->GetFiredBulletsCount() > 0)
			{
				//do nothing
			}
			else
			{
				m_is_attacking = false;
				m_attack_cooldown_clock.restart();
			}
		}
	}

	// # # #
	UpdateAI(deltaTime);
	// # # #

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

	//debug
	m_aggro_radius_feedback.setPosition(getPosition());
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

bool Enemy::FollowPlayer(GameObject* player)
{
	float range = m_weapon ? m_weapon->m_range.x + m_size.x / 2 : player->m_size.x + m_size.x / 2;
	if (GetDistanceSquaredBetweenPositions(this->getPosition(), player->getPosition()) > range * range)
	{
		SetSpeedForConstantSpeedToDestination(player->getPosition(), m_ref_speed);
		return false;
	}
	else
	{
		m_speed = sf::Vector2f(0, 0);
		return true;
	}
}

bool Enemy::AttackPlayer(GameObject* player)
{
	if (m_attack_cooldown_clock.getElapsedTime().asSeconds() > m_attack_cooldown)//condition to start attack
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

	return m_is_attacking;
}

void Enemy::Draw(sf::RenderTexture& screen)
{
	GameObject::Draw(screen);

	if (m_visible)
	{
		screen.draw(m_aggro_radius_feedback);
	}
}

bool Enemy::Roam(sf::Time deltaTime)
{
	//arrived?
	float dist_squared_to_destination = GetDistanceSquaredBetweenPositions(getPosition(), m_destination);
	if (!m_arrived_at_destination)
	{
		if (dist_squared_to_destination < 16 * 16)
		{
			m_arrived_at_destination = true;
			m_speed = sf::Vector2f(0, 0);
			setPosition(m_destination);
			m_roaming_clock.restart();
		}
		else
		{
			SetSpeedForConstantSpeedToDestination(m_destination, m_ref_speed);
		}
	}

	//arrived = roam to a new destination
	if (m_arrived_at_destination && m_roaming_clock.getElapsedTime().asSeconds() > m_roam_duration)
	{
		m_roam_duration = RandomizeFloatBetweenValues(sf::Vector2f(ENEMY_ROAMING_MIN_DURATION, ENEMY_ROAMING_MAX_DURATION));

		float distance = RandomizeFloatBetweenValues(sf::Vector2f(ENEMY_ROAMING_MIN_DISTANCE, ENEMY_ROAMING_MAX_DISTANCE));
		float angle = RandomizeFloatBetweenValues(sf::Vector2f(0.f, 2 * M_PI));

		sf::Vector2f move = GetVectorFromLengthAndAngle(distance, angle);

		if (IsInsideArea(getPosition() + move, (*CurrentGame).m_map_size))
		{
			m_destination = getPosition() + move;
		}
		else
		{
			move = - move;
			m_destination = getPosition() + move;
		}

		SetSpeedForConstantSpeedToDestination(m_destination, m_ref_speed);
		m_arrived_at_destination = false;

		return true;
	}
	
	return false;
}