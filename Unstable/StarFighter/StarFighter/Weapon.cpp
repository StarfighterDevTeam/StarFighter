#include "Weapon.h"

extern Game* CurrentGame;

//Weapon::Weapon(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, sf::Time duration, int animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2), frameNumber, animationNumber)
//{
//
//}

#define MELEE_KATANA_RANGE_X			100.f
#define MELEE_KATANA_RANGE_Y			70.f
#define MELEE_KATANA_DURATION			0.1f

#define MELEE_SPEAR_RANGE_X				230.f
#define MELEE_SPEAR_RANGE_Y				40.f
#define MELEE_SPEAR_DURATION			0.5f

#define RANGED_SHURIKEN_SPEED			500.f
#define RANGED_SHURIKEN_COOLDOWN		0.1f

Weapon::Weapon(GameObject* owner, WeaponType type, sf::Color color)
{
	m_owner = owner;
	m_type = type;
	m_color = color;
	m_bullet = new Ammo(owner, sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/range_shuriken.png", sf::Vector2f(32, 32), sf::Vector2f(16, 16), 1, 1);
	LayerType layer = m_owner->m_layer == PlayerShipLayer ? PlayerBulletLayer : EnemyBulletLayer;
	GameObjectType collider_type = m_owner->m_collider_type == PlayerShip ? PlayerBulletObject : EnemyBulletObject;
	m_bullet->m_layer = layer;
	m_bullet->m_collider_type = collider_type;

	switch (type)
	{
		case Weapon_Katana:
		{
			m_range = sf::Vector2f(MELEE_KATANA_RANGE_X, MELEE_KATANA_RANGE_Y);
			m_attack_duration = MELEE_KATANA_DURATION;
			m_attack_cooldown = PLAYER_ATTACK_COOLDOWN;
			m_dmg = 1;
			m_is_piercing = false;
			m_is_ranged = false;
			m_can_be_parried = true;

			Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), sf::Color(255, 255, 255,255), m_range);
			break;
		}

		case Weapon_Spear:
		{	 
			m_range = sf::Vector2f(MELEE_SPEAR_RANGE_X, MELEE_SPEAR_RANGE_Y);
			m_attack_duration = MELEE_SPEAR_DURATION;
			m_attack_cooldown = PLAYER_ATTACK_COOLDOWN;
			m_dmg = 1;
			m_is_piercing = true;
			m_is_ranged = false;
			m_can_be_parried = false;

			Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), sf::Color(255, 255, 255, 255), m_range);
			break;
		}

		case Weapon_Shuriken:
		{
			m_range = sf::Vector2f(MELEE_SPEAR_RANGE_X, MELEE_SPEAR_RANGE_Y);
			m_attack_duration = 0.f;
			m_attack_cooldown = RANGED_SHURIKEN_COOLDOWN;
			m_dmg = 1;
			m_is_piercing = false;
			m_is_ranged = true;

			m_bullet->m_textureName = "2D/range_shuriken.png";
			m_bullet->m_size = sf::Vector2f(32, 32);
			m_bullet->m_frameNumber = 1;
			m_bullet_speed = RANGED_SHURIKEN_SPEED;
			m_bullet_is_following_target = false;
			m_bullet_is_unique = false;
			m_can_be_parried = true;

			Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/range_shuriken.png", sf::Vector2f(32, 32), 1, 1);
			break;
		}
	}

	m_attack_timer = m_attack_duration;
	m_DontGarbageMe = true;
	setColor(color);
	m_visible = false;
	Extend(sf::Vector2f(0.f, 1.f));
}

Weapon::~Weapon()
{
	if (m_bullet)
	{
		delete m_bullet;
		m_bullet = NULL;
	}
}

void Weapon::update(sf::Time deltaTime)
{
	//update timers
	m_attack_timer += deltaTime.asSeconds();

	AnimatedSprite::update(deltaTime);
}

void Weapon::Draw(sf::RenderTexture& screen)
{
	screen.draw(*this);
}

void Weapon::CollisionWithEnemy(GameObject* enemy)
{
	if (m_is_piercing == false && m_enemies_tagged.size() > 0)//piercing is the ability to hit several targets with one stroke
	{
		return;
	}

	std::vector<GameObject*>::iterator it = find(m_enemies_tagged.begin(), m_enemies_tagged.end(), enemy);
	if (it == m_enemies_tagged.end())
	{
		m_enemies_tagged.push_back(enemy);
		
		enemy->DealDamage(m_dmg);
	}
}

void Weapon::CollisionWithWeapon(GameObject* enemy_weapon)
{
	Weapon* weapon = (Weapon*)enemy_weapon;

	std::vector<GameObject*>::iterator it = find(m_enemies_tagged.begin(), m_enemies_tagged.end(), weapon->m_owner);
	std::vector<GameObject*>::iterator it2 = find(weapon->m_enemies_tagged.begin(), weapon->m_enemies_tagged.end(), m_owner);

	//tagging the enemy weapon's owner so it won't be affected by further damage by this weapon (= parry mechanic)
	//once an enemy is hit, he cannot be parried until our next stroke
	if (it == m_enemies_tagged.end() && it2 == weapon->m_enemies_tagged.end())
	{
		if (m_can_be_parried)
		{
			m_enemies_tagged.push_back(weapon->m_owner);
			setColor(sf::Color::White);
		}

		if (weapon->m_can_be_parried)
		{
			weapon->m_enemies_tagged.push_back(m_owner);
			weapon->setColor(sf::Color::White);
		}

		if (m_can_be_parried || weapon->m_can_be_parried)
		{
			(*CurrentGame).PlaySFX(SFX_Parry);
		}
	}
}

void Weapon::CollisionWithBullet(GameObject* enemy_bullet)
{
	Ammo* bullet = (Ammo*)enemy_bullet;

	if (bullet->m_can_be_parried)//deflecting enemy bullets and turning them into own bullets
	{
		if (m_is_piercing == false && m_enemies_tagged.size() > 0)//piercing is the ability to hit several targets with one stroke
		{
			return;
		}

		if (bullet->m_parry_timer >= PARRY_BULLET_COOLDOWN)
		{
			std::vector<GameObject*>::iterator it = find(m_enemies_tagged.begin(), m_enemies_tagged.end(), enemy_bullet);
			if (it == m_enemies_tagged.end())
			{
				m_enemies_tagged.push_back(bullet);

				//bullet->m_speed = -bullet->m_speed;
				bullet->m_speed = GetVectorFromLengthAndAngle(bullet->m_ref_speed, (getRotation() - 90) * M_PI / 180.f);
				bullet->m_parry_timer = 0.f;

				LayerType layer = bullet->m_layer == EnemyBulletLayer ? PlayerBulletLayer : EnemyBulletLayer;
				GameObjectType type = bullet->m_collider_type == EnemyBulletObject ? PlayerBulletObject : EnemyBulletObject;
				(*CurrentGame).changeObjectTypeAndLayer(bullet, layer, type);

				(*CurrentGame).PlaySFX(SFX_Parry);
			}
		}
	}
}

void Weapon::Extend(sf::Vector2f ratio)
{
	if (m_is_ranged == false)
	{
		sf::Vector2f scale;
		scale.x = 1.f / m_size.x * m_range.x;
		scale.y = 1.f / m_size.y * m_range.y;

		setScale(sf::Vector2f(ratio.x * scale.x, ratio.y * scale.y));
	}
}

Ammo* Weapon::Shoot()
{
	Ammo* bullet = new Ammo(m_owner, m_bullet_speed, m_dmg, m_is_piercing, m_can_be_parried, m_bullet->m_textureName, m_bullet->m_size, m_bullet->m_size * 0.5f, m_bullet->m_frameNumber, 1);
	(*CurrentGame).addToScene(bullet, m_bullet->m_layer, m_bullet->m_collider_type);

	bullet->m_visible = true;

	bullet->setPosition(getPosition());
	bullet->setRotation(getRotation());

	return bullet;
}

void Weapon::Shoot(float angle_rad)
{
	Ammo* bullet = Shoot();

	bullet->SetSpeedVectorFromAbsoluteSpeedAndAngle(m_bullet_speed, angle_rad);
}

void Weapon::Shoot(GameObject* target)
{
	if (target)
	{
		Ammo* bullet = Shoot();
		
		bullet->SetConstantSpeedToDestination(target->getPosition(), m_bullet_speed);

		if (m_bullet_is_following_target)
		{
			bullet->m_target = target;
		}
	}
	else
	{
		Shoot((getRotation() - 90.f) * M_PI / 180.f);
	}
}

size_t Weapon::GetFiredBulletsCount()
{
	size_t count = 0;

	vector<GameObject*> bullets = (*CurrentGame).GetSceneGameObjectsTyped(m_bullet->m_collider_type);

	size_t bulletsVectorSize = bullets.size();
	for (size_t i = 0; i < bulletsVectorSize; i++)
	{
		if (bullets[i] == NULL)
			continue;

		if (bullets[i]->m_visible == false)
			continue;

		Ammo* bullet = (Ammo*)bullets[i];
		if (bullet->m_owner = this->m_owner)
		{
			count++;
		}
	}

	return count;
}