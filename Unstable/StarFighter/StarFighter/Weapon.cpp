#include "Weapon.h"

extern Game* CurrentGame;

//Weapon::Weapon(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, sf::Time duration, int animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2), frameNumber, animationNumber)
//{
//
//}

#define MELEE_RANGE_X				130.f
#define MELEE_RANGE_Y				70.f
#define MELEE_DURATION				0.3f

Weapon::Weapon(GameObject* owner, WeaponTypes type, sf::Color color)
{
	m_owner = owner;

	switch (type)
	{
		case Weapon_Katana:
		{
			InitWeapon(sf::Vector2f(MELEE_RANGE_X, MELEE_RANGE_Y), color);
			break;
		}
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
	m_color = color;

	// INIT
	m_melee_range = sf::Vector2f(MELEE_RANGE_X, MELEE_RANGE_Y);
	m_melee_duration = MELEE_DURATION;
	m_dmg = 1;

	m_visible = false;
	Extend(sf::Vector2f(0.f, 1.f));
	//setScale(sf::Vector2f(0.f, m_melee_range.y));
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
	std::vector<GameObject*>::iterator it = find(m_enemies_tagged.begin(), m_enemies_tagged.end(), enemy);
	if (it == m_enemies_tagged.end())
	{
		m_enemies_tagged.push_back(enemy);

		enemy->DealDamage(m_dmg);

		ostringstream ss;
		ss << "-" << m_dmg;
		sf::Color color = enemy->m_collider_type == PlayerShip ? sf:: Color::Red : sf::Color::Blue;
		(*CurrentGame).CreateSFTextPop(ss.str(), Font_Arial, 30, color, enemy->getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y / 2);
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
	}
}

void Weapon::Extend(sf::Vector2f ratio)
{
	sf::Vector2f scale;
	scale.x = 1.f / m_size.x * m_melee_range.x;
	scale.y = 1.f / m_size.y * m_melee_range.y;

	setScale(sf::Vector2f(ratio.x * scale.x, ratio.y * scale.y));
}