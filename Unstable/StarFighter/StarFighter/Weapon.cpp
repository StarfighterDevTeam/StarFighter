#include "Weapon.h"

extern Game* CurrentGame;

//Weapon::Weapon(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, sf::Time duration, int animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2), frameNumber, animationNumber)
//{
//
//}

#define MELEE_RANGE_X				130.f
#define MELEE_RANGE_Y				70.f
#define MELEE_DURATION				0.1f

Weapon::Weapon(WeaponTypes type)
{
	switch (type)
	{
		case Weapon_Katana:
		{
			InitWeapon(sf::Vector2f(MELEE_RANGE_X, MELEE_RANGE_Y), sf::Color(255, 255, 0, 255));
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
		pixels[i] = color.r;		// R
		pixels[i + 1] = color.g;	// G
		pixels[i + 2] = color.b;	// B
		pixels[i + 3] = color.a;	// A
	}

	ostringstream ss;
	ss << "weapon_" << (int)size.x << "x" << (int)size.y << "_r" << (int)color.r << "_g" << (int)color.g << "_b" << (int)color.b << "_a" << (int)color.a;
	
	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), ss.str(), sf::Vector2f(W, H), 1, 1, pixels);


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
	enemy->DealDamage(m_dmg);

	ostringstream ss;
	ss << "-" << m_dmg;
	(*CurrentGame).CreateSFTextPop(ss.str(), Font_Arial, 30, sf::Color::Blue, enemy->getPosition(), PlayerBlue, 100, 50, 3, NULL, -m_size.y / 2);
}

void Weapon::Extend(sf::Vector2f ratio)
{
	sf::Vector2f scale;
	scale.x = 1.f / m_size.x * m_melee_range.x;
	scale.y = 1.f / m_size.y * m_melee_range.y;

	setScale(sf::Vector2f(ratio.x * scale.x, ratio.y * scale.y));
}