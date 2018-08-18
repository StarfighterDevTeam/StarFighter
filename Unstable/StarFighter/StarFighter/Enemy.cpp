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

#define ENEMY_DASH_SPEED					800.f

#define ENEMY_SUMMONING_RADIUS				100.f

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
	m_state = EnemyState_Idle;
	m_is_attacking = false;
	m_weapon = NULL;
	m_enemy_summoned = NBVAL_ENEMYTYPES;
	m_dash_range = 0.f;
	m_hit_feedback_timer = HIT_FEEDBACK_DURATION;

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
		case Enemy_Wufeng_Summoner:
		{
			m_hp_max = 2;
			m_hp = m_hp_max;
			m_dmg = 1;
			m_attack_cooldown = 2.f;
			m_ref_speed = 200.f;
			textureName = "2D/wufeng.png";
			size = sf::Vector2f(64, 114);
			m_aggro_radius = 500.f;

			m_dash_range = 200.f;
			m_dash_cooldown = 2.f;
			m_dash_cooldown_timer = m_dash_cooldown;

			break;
		}
		case Enemy_Ghost:
		{
			m_hp_max = 1;
			m_hp = m_hp_max;
			m_dmg = 1;
			m_attack_cooldown = 1.f;
			m_ref_speed = 400.f;
			textureName = "2D/ghost.png";
			size = sf::Vector2f(32, 57);
			m_aggro_radius = 8888.f;
			break;
		}
	}

	switch (type)
	{
		case Enemy_Wufeng_Katana:
		{
			m_weapon = new Weapon(this, Weapon_Katana, sf::Color::Red);
			setColor(sf::Color::Cyan, true);
			break;
		}
		case Enemy_Wufeng_Spear:
		{
			m_weapon = new Weapon(this, Weapon_Spear, sf::Color::Red);
			setColor(sf::Color::Blue, true);
			break;
		}
		case Enemy_Wufeng_Shuriken:
		{
			m_weapon = new Weapon(this, Weapon_Shuriken, sf::Color::Red);
			setColor(sf::Color::Magenta, true);
			break;
		}
		case Enemy_Wufeng_Summoner:
		{
			m_weapon = new Weapon(this, Weapon_Katana, sf::Color::Red);
			setColor(sf::Color::Yellow, true);

			//summoning
			m_enemy_summoned = Enemy_Ghost;
			m_summoning_cooldown_range = sf::Vector2f(3.f, 8.f);
			m_summoning_cooldown = RandomizeFloatBetweenValues(m_summoning_cooldown_range);
			m_summoning_cooldown_timer = m_summoning_cooldown;
			m_summoning_duration = 1.f;
			m_summoning_timer = m_summoning_duration;
			break;
		}
		case Enemy_Ghost:
		{
			setColor(sf::Color::Black, true);
			break;
		}
	}

	Init(position, sf::Vector2f(0, 0), textureName, size, frameNumber, animationNumber);

	m_DontGarbageMe = true;

	if (m_weapon)
	{
		(*CurrentGame).addToScene(m_weapon, EnemyWeaponLayer, EnemyWeaponObject);
	}

	//TIMER
	m_attack_cooldown_timer = m_attack_cooldown;

	//AI
	m_roam_duration = RandomizeFloatBetweenValues(sf::Vector2f(ENEMY_ROAMING_MIN_DURATION, ENEMY_ROAMING_MAX_DURATION));
	m_roaming_timer = m_roam_duration;
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
	if (m_state == EnemyState_Dash)
	{
		return;
	}

	GameObject* player = (GameObject*)(*CurrentGame).m_playerShip;

	float dist_squared_to_player = GetDistanceSquaredBetweenPositions(getPosition(), player->getPosition());
	if (!player->m_ghost && player->m_visible && dist_squared_to_player < m_aggro_radius * m_aggro_radius)
	{
		if (m_state == EnemyState_Idle)
		{
			m_state = EnemyState_FollowTarget;
		}
	}
	else
	{
		if (m_state == EnemyState_FollowTarget)
		{
			m_state = EnemyState_Idle;

			m_arrived_at_destination = true;
			m_speed = sf::Vector2f(0, 0);
		}
	}

	if (m_state == EnemyState_Summoning)
	{
		if (Summon())
		{
			m_state = EnemyState_Idle;
			m_attack_cooldown_timer = 0.f;
			m_roaming_timer = 0.f;
		}
	}

	if (m_state == EnemyState_Idle)
	{
		Roam(deltaTime);
	}

	if (m_state == EnemyState_FollowTarget)
	{
		//1. DEFENSE

		//any emergency to dodge?
		GameObject* target = CanDodge();
		if (target != NULL && DashAwayFromTarget(target))
		{
			m_state = EnemyState_Dash;
		}
		//any emergency target to parry?
		else
		{
			target = CanParry();
			if (target != NULL)
			{
				AttackTarget(target);
			}
			//2. ATTACK
			else
			{
				target = player;
				//follow the player
				if (TrySummoning())
				{
					m_state = EnemyState_Summoning;
					m_speed = sf::Vector2f(0, 0);
				}
				else
				{
					if (FollowTarget(target))
					{
						AttackTarget(target);
					}
				}
			}
		}
	}
}

float Enemy::RangeToInterceptTarget(GameObject* target)
{
	float range = m_weapon ? (m_size.y / 2) + m_weapon->m_range.x + (target->m_size.y / 2) : (m_size.y / 2) + (target->m_size.y / 2);
	if (target->m_collider_type == PlayerBulletObject)
	{
		Ammo* bullet = (Ammo*)target;
		range += bullet->m_ref_speed * m_weapon->m_attack_duration;
	}
	else if (target->m_collider_type == PlayerWeaponObject)
	{
		Weapon* weapon = (Weapon*)target;
		range = (m_size.y / 2) + weapon->m_range.x; 
	}

	return range;
}

float Enemy::RangeToDodgeTarget(GameObject* target)
{
	float range = m_size.y / 2;
	if (target->m_collider_type == PlayerBulletObject)
	{
		Ammo* bullet = (Ammo*)target;
		range += bullet->m_ref_speed * (bullet->m_size.y / 2 + m_size.y / 2) / ENEMY_DASH_SPEED;
	}
	else if (target->m_collider_type == PlayerWeaponObject)
	{
		Weapon* weapon = (Weapon*)target;
		range += weapon->m_range.x;
	}

	return range;
}


GameObject* Enemy::CanParry()
{
	if (m_weapon == NULL || m_attack_cooldown_timer < m_attack_cooldown || m_is_attacking)//must be able to attack in order to parry
	{
		return NULL;
	}

	//get player weapon
	GameObject* closest_target = (*CurrentGame).GetClosestObjectTyped(this, PlayerWeaponObject);

	//get player bullets and choose what's closest (and visible)
 	GameObject* closest_bullet = (*CurrentGame).GetClosestObjectTypedIncoming(this, PlayerBulletObject, 30.f);//it is useless to parry bullets whose trajectory is off
	if (closest_bullet)
	{
		if (closest_target == NULL || GetDistanceSquaredBetweenPositions(this->getPosition(), closest_bullet->getPosition()) < GetDistanceSquaredBetweenPositions(this->getPosition(), closest_target->getPosition()))
		{
			closest_target = closest_bullet;
		}
	}

	//target the selected object, if it exists and is in weapon range
	if (closest_target && closest_target->m_visible)
	{
		float range = RangeToInterceptTarget(closest_target);

		if (GetDistanceSquaredBetweenPositions(this->getPosition(), closest_target->getPosition()) < range * range)
		{
			//melee weapon can parry anything ; but ranged weapon can only parry bullets, not weapons
			if (m_weapon->m_is_ranged == false || closest_target->m_collider_type != PlayerWeaponObject)
			{
				//N.B: some weapons cannot be parried (m_can_be_parried) but we do not evaluate it, so AI can make kind of mistakes
				return closest_target;
			}
		}
	}
	
	return NULL;
}

GameObject* Enemy::CanDodge()
{
	if (m_dash_range == 0 || m_dash_cooldown_timer < m_dash_cooldown)
	{
		return NULL;
	}
	
	//get player weapon
	GameObject* closest_target = (*CurrentGame).GetClosestObjectTyped(this, PlayerWeaponObject);

	//get player bullets and choose what's closest (and visible)
	GameObject* closest_bullet = (*CurrentGame).GetClosestObjectTypedIncoming(this, PlayerBulletObject, 30.f);//it is useless to parry bullets whose trajectory is off
	if (closest_bullet)
	{
		if (closest_target == NULL || GetDistanceSquaredBetweenPositions(this->getPosition(), closest_bullet->getPosition()) < GetDistanceSquaredBetweenPositions(this->getPosition(), closest_target->getPosition()))
		{
			closest_target = closest_bullet;
		}
	}

	//target the selected object, if it exists and is in weapon range
	if (closest_target && closest_target->m_visible)
	{
		float range = RangeToDodgeTarget(closest_target);

		if (GetDistanceSquaredBetweenPositions(this->getPosition(), closest_target->getPosition()) < range * range)
		{
			return closest_target;
		}
	}

	return NULL;
}

bool Enemy::DashAwayFromTarget(GameObject* target)
{
	float dash_angle = M_PI_2 * RandomizeSign();
	GameObject* player = (GameObject*)(*CurrentGame).m_playerShip;
	float angle_to_player = GetAngleRadBetweenObjects(this, player);
	float angle = angle_to_player + dash_angle;
	sf::Vector2f offset = GetVectorFromLengthAndAngle(m_dash_range, angle);
	sf::Vector2f future_position = getPosition() + offset;

	//ensure we end up in a position inside the map bounds
	if (this->IsInsideArea(future_position, (*CurrentGame).m_map_size) == false)
	{
		dash_angle *= -1;
		angle = angle_to_player + dash_angle;
		offset = GetVectorFromLengthAndAngle(m_dash_range, angle);
		future_position = getPosition() + offset;
	}
	if (this->IsInsideArea(future_position, (*CurrentGame).m_map_size) == false)
	{
		//still outside bounds? -> abort
		return false;
	}

	ScaleVector(&offset, ENEMY_DASH_SPEED);
	m_speed = offset;
	m_dash_target = future_position;

	m_ghost = true;

	return true;
}

void Enemy::update(sf::Time deltaTime)
{
	//update timers
	if (m_attack_cooldown_timer < m_attack_cooldown)
		m_attack_cooldown_timer += deltaTime.asSeconds();

	if (m_roaming_timer < m_roam_duration)
		m_roaming_timer += deltaTime.asSeconds();

	if (m_summoning_cooldown_timer < m_summoning_cooldown)
		m_summoning_cooldown_timer += deltaTime.asSeconds();
	
	if (m_summoning_timer < m_summoning_duration)
		m_summoning_timer += deltaTime.asSeconds();

	if (m_dash_cooldown_timer < m_dash_cooldown)
		m_dash_cooldown_timer += deltaTime.asSeconds();

	if (m_stroboscopic_effect_timer < STROBOSCOPIC_EFFECT_FREQUENCY)
		m_stroboscopic_effect_timer += deltaTime.asSeconds();

	if (m_hit_feedback_timer < HIT_FEEDBACK_DURATION)
	{
		m_hit_feedback_timer += deltaTime.asSeconds();
		if (m_hit_feedback_timer >= HIT_FEEDBACK_DURATION && m_hp <= 0)
		{
			Death();
		}
	}

	//reset color
	setColor(m_color);

	//bounce on screen borders
	bool bounced = BounceOnBorders((*CurrentGame).m_map_size);

	//end of dash?
	if (m_state == EnemyState_Dash)
	{
		float dist_to_target = GetDistanceBetweenPositions(getPosition(), m_dash_target);
		if (dist_to_target > ENEMY_DASH_SPEED * deltaTime.asSeconds())
		{
			sf::Vector2f dash_vector = m_dash_target - getPosition();
			ScaleVector(&dash_vector, ENEMY_DASH_SPEED);
			m_speed = dash_vector;
		}
		else
		{
			m_speed = sf::Vector2f(0, 0);
			setPosition(m_dash_target);
			m_dash_cooldown_timer = 0.f;
			m_ghost = false;
			m_state = EnemyState_Idle;
		}
	}
	
	if (m_is_attacking)//reset
	{
		if (m_weapon && m_weapon->m_attack_timer >= m_weapon->m_attack_duration)
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
				m_attack_cooldown_timer = 0.f;
			}
		}
		else if (!m_weapon)
		{
			m_is_attacking = false;
			m_attack_cooldown_timer = 0.f;
		}
	}

	//AI strategy
	UpdateAI(deltaTime);
	// # # #

	if (m_is_attacking)//update
	{
		if (m_weapon)
		{
			float ratio = m_weapon->m_attack_timer / m_weapon->m_attack_duration;
			if (ratio > 1.0f)
			{
				ratio = 1.0f;
			}
			m_weapon->Extend(sf::Vector2f(ratio, 1.f));

			UpdateWeaponPosition(m_weapon);
		}
	}

	if (m_hit_feedback_timer < HIT_FEEDBACK_DURATION)
	{
		setColor(sf::Color::Red);
	}

	if (m_state == EnemyState_Dash)
	{
		PlayStroboscopicEffect(sf::seconds(STROBOSCOPIC_EFFECT_DURATION), sf::seconds(STROBOSCOPIC_EFFECT_FREQUENCY));
		setColor(sf::Color(200, 200, 200, 180));
	}

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
		m_hit_feedback_timer = 0.f;
		(*CurrentGame).PlaySFX(SFX_GruntEnemy);
	}

	return m_hp <= 0;
}

void Enemy::Death()
{
	m_GarbageMe = true;
	m_visible = false;

	Loot* loot = new Loot(Loot_DashAmmo, getPosition());
	(*CurrentGame).addToScene(loot, LootObjectLayer, LootObject);
}

int Enemy::GetRating()
{
	int rating = 0;

	rating += m_hp_max;
	rating += m_dmg;

	return rating;
}

bool Enemy::FollowTarget(GameObject* target)
{
	  if (m_weapon && m_weapon->m_attack_timer < m_weapon->m_attack_duration)
	{
		return false;
	}
	
	if (!m_weapon && m_attack_cooldown_timer < m_attack_cooldown)
	{
		return false;
	}

	float range = m_weapon ? (m_size.y / 2) + m_weapon->m_range.x : (m_size.y / 2);
	
	if (GetDistanceSquaredBetweenPositions(this->getPosition(), target->getPosition()) > range * range)
	{
		SetSpeedForConstantSpeedToDestination(target->getPosition(), m_ref_speed);
		return false;
	}
	else
	{
		m_speed = sf::Vector2f(0, 0);
		float angle_deg = GetAngleDegToTargetPosition(this->getPosition(), this->getRotation(), target->getPosition());
		rotate(angle_deg);
		return true;
	}
}

bool Enemy::AttackTarget(GameObject* target)
{
	if (m_attack_cooldown_timer >= m_attack_cooldown && !m_is_attacking)//condition to start attack
	{
		if (m_weapon )
		{
			if (m_weapon->m_is_ranged == false)
			{
				m_weapon->m_visible = true;
				(*CurrentGame).PlaySFX(SFX_Melee);
			}
			else
			{
				m_weapon->Shoot(target);//shoot an enemy. If no enemy found, it will shoot towards current rotation.
			}

			m_weapon->m_attack_timer = 0.f;
		}

		m_is_attacking = true;
		m_attack_cooldown_timer = 0.f;
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
			m_roaming_timer = 0.f;
		}
		else
		{
			SetSpeedForConstantSpeedToDestination(m_destination, m_ref_speed);
		}
	}

	//arrived = roam to a new destination
	if (m_arrived_at_destination && m_roaming_timer >= m_roam_duration)
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

bool Enemy::TrySummoning()
{
	if (m_enemy_summoned != NBVAL_ENEMYTYPES)
	{
		if (m_summoning_cooldown_timer >= m_summoning_cooldown)
		{
			m_summoning_timer = 0.f;
			return true;
		}
	}

	return false;
}

bool Enemy::Summon()
{
	if (m_summoning_timer >= m_summoning_duration)
	{
		m_summoning_cooldown = RandomizeFloatBetweenValues(m_summoning_cooldown_range);
		m_summoning_cooldown_timer = 0.f;

		float random_angle = RandomizeFloatBetweenValues(sf::Vector2f(M_PI_2, 1.5 * M_PI_2));
		random_angle *= RandomizeSign();
		GameObject* player = (GameObject*)(*CurrentGame).m_playerShip;
		float angle_to_player = GetAngleRadBetweenObjects(this, player);
		float angle = angle_to_player + random_angle;
		sf::Vector2f offset = GetVectorFromLengthAndAngle(m_diag + ENEMY_SUMMONING_RADIUS, angle);
		sf::Vector2f random_position = getPosition() + offset;

 		Enemy* invocation = new Enemy(random_position, m_enemy_summoned);
		
		//ensure invocation is summoned inside the map bounds
		if (invocation->IsInsideArea(random_position, (*CurrentGame).m_map_size) == false)
		{
			random_angle *= -1;
			angle = angle_to_player + random_angle;
			offset = GetVectorFromLengthAndAngle(m_diag + ENEMY_SUMMONING_RADIUS, angle);
			random_position = getPosition() + offset;
		}
		if (invocation->IsInsideArea(random_position, (*CurrentGame).m_map_size) == false)
		{
			//still outside bounds? -> abort
			return false;
		}

		setColor(m_color);

		(*CurrentGame).addToScene(invocation, EnemyObjectLayer, EnemyObject);

		(*CurrentGame).PlaySFX(SFX_Summon);

		return true;
	}
	else
	{
		setColor(sf::Color::White);
	}
	
	return false;
}

void Enemy::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (m_stroboscopic_effect_timer >= time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		m_stroboscopic_effect_timer = 0.f;
	}
}