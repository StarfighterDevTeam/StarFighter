#include "Weapon.h"

extern Game* CurrentGame;

//Weapon::Weapon(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, sf::Time duration, int animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2), frameNumber, animationNumber)
//{
//
//}

#define MELEE_KATANA_RANGE_X			130.f
#define MELEE_KATANA_RANGE_Y			70.f
#define MELEE_KATANA_DURATION			0.2f

#define MELEE_SPEAR_RANGE_X				230.f
#define MELEE_SPEAR_RANGE_Y				40.f
#define MELEE_SPEAR_DURATION			0.5f

#define RANGED_SHURIKEN_SPEED			1000.f
#define RANGED_SHURIKEN_COOLDOWN		0.5f

Weapon::Weapon(GameObject* owner, WeaponTypes type, sf::Color color)
{
	m_owner = owner;
	m_type = type;
	m_color = color;
	m_visible = false;
	m_bullet = new Ammo(this->m_owner, sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/range_shuriken.png", sf::Vector2f(32, 32), sf::Vector2f(16, 16), 1, 1);
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
			m_dmg = 1;
			m_is_piercing = false;
			m_is_ranged = false;

			InitWeapon(m_range, color);
			break;
		}

		case Weapon_Spear:
		{	 
			m_range = sf::Vector2f(MELEE_SPEAR_RANGE_X, MELEE_SPEAR_RANGE_Y);
			m_attack_duration = MELEE_SPEAR_DURATION;
			m_dmg = 1;
			m_is_piercing = true;
			m_is_ranged = false;

			InitWeapon(m_range, color);
			break;
		}

		case Weapon_Shuriken:
		{
			m_range = sf::Vector2f(MELEE_SPEAR_RANGE_X, MELEE_SPEAR_RANGE_Y);
			m_attack_duration = RANGED_SHURIKEN_COOLDOWN;
			m_dmg = 1;
			m_is_piercing = true;
			m_is_ranged = true;
			m_bullet->m_textureName = "2D/range_shuriken.png";
			m_bullet->m_size = sf::Vector2f(32, 32);
			m_bullet->m_frameNumber = 1;
			m_bullet_speed = RANGED_SHURIKEN_SPEED;
			m_bullet_is_following_target = false;
			m_bullet_is_unique = false;

			Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/range_shuriken.png", sf::Vector2f(32, 32), 1, 1);
			break;
		}
	}
}

Weapon::~Weapon()
{
	if (m_bullet)
	{
		delete m_bullet;
		m_bullet = NULL;
	}
}

void Weapon::InitWeapon(sf::Vector2f size, sf::Color color)// : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/smiley.png", sf::Vector2f(200, 200), sf::Vector2f(100, 100))
{
	//TEXTURE CREATION
	const int W = size.x;
	const int H = size.y;

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];

	for (int i = 0; i < W * H * 4; i += 4)
	{
		//pixels[i] = color.r;		// R
		//pixels[i + 1] = color.g;	// G
		//pixels[i + 2] = color.b;	// B
		//pixels[i + 3] = color.a;	// A
		pixels[i]		= 255;	// R
		pixels[i + 1]	= 255;	// G
		pixels[i + 2]	= 255;	// B
		pixels[i + 3]	= 255;	// A
	}

	ostringstream ss;
	//ss << "weapon_" << (int)size.x << "x" << (int)size.y << "_r" << (int)color.r << "_g" << (int)color.g << "_b" << (int)color.b << "_a" << (int)color.a;
	ss << "weapon_" << (int)size.x << "x" << (int)size.y;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), ss.str(), sf::Vector2f(W, H), 1, 1, pixels);
	
	setColor(color);
	Extend(sf::Vector2f(0.f, 1.f));
	//setScale(sf::Vector2f(0.f, m_range.y));
	//setScale(sf::Vector2f(0.f, 1.f));
}

void Weapon::update(sf::Time deltaTime)
{
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

void Weapon::CollisionBetweenWeapons(GameObject* enemy_weapon)
{
	Weapon* weapon = (Weapon*)enemy_weapon;

	std::vector<GameObject*>::iterator it = find(m_enemies_tagged.begin(), m_enemies_tagged.end(), weapon->m_owner);
	std::vector<GameObject*>::iterator it2 = find(weapon->m_enemies_tagged.begin(), weapon->m_enemies_tagged.end(), m_owner);

	//tagging the enemy weapon's owner so it won't be affected by further damage by this weapon (= parry mechanic)
	//once an enemy is hit, he cannot be parried until our next stroke
	if (it == m_enemies_tagged.end() && it2 == weapon->m_enemies_tagged.end())
	{
		m_enemies_tagged.push_back(weapon->m_owner);
		setColor(sf::Color::White);

		weapon->m_enemies_tagged.push_back(m_owner);
		weapon->setColor(sf::Color::White);

		(*CurrentGame).PlaySFX(SFX_Parry);
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
	Ammo* bullet = new Ammo(this->m_owner, m_bullet_speed, m_bullet->m_textureName, m_bullet->m_size, m_bullet->m_size * 0.5f, m_bullet->m_frameNumber, 1);
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
		
		bullet->SetSpeedForConstantSpeedToDestination(target->getPosition(), m_bullet_speed);

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

size_t Weapon::GetBulletFiredCount()
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