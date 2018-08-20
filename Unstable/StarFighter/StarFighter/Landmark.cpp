#include "Landmark.h"

extern Game* CurrentGame;

using namespace sf;

Landmark::Landmark(LandmarkType type, sf::Vector2f position)
{
	m_type = type;
	m_activated = false;

	switch (type)
	{
		case Landmark_Gong:
		{
			Init(position, sf::Vector2f(0, 0), "2D/gong.png", sf::Vector2f(307, 303), 1, 1);

			m_fake_landmark = new Aura(this, "2D/gong_fake.png", sf::Vector2f(498, 564), 1, 1);
			(*CurrentGame).addToScene(m_fake_landmark, LandmarkFakeLayer, LandmarkFakeObject);
			break;
		}
	}
}

Landmark::~Landmark()
{
	if (m_fake_landmark)
	{
		m_fake_landmark->m_GarbageMe = true;
		m_fake_landmark->m_visible = false;
	}
}

void Landmark::update(sf::Time deltaTime)
{
	//debug only:
	if ((*CurrentGame).GetEnemyRatings() <= 3)
	{
		m_activated = false;
		setGhost(false);
	}

	AnimatedSprite::update(deltaTime);
}

bool Landmark::ActivateLandmark()
{
	if (!m_activated)
	{
		switch (m_type)
		{
			case Landmark_Gong:
			{
				(*CurrentGame).PlaySFX(SFX_Gong);
				while ((*CurrentGame).GetEnemyRatings() < 9)
				{
					SpawnRandomEnemy();
				}
				break;
			}
			default:
				return false;
		}

		m_activated = true;
		setGhost(true);
		return true;
	}

	return false;
}

bool Landmark::SpawnRandomEnemy()
{
	EnemyType random_type = (EnemyType)(RandomizeIntBetweenValues(Enemy_Wufeng_Katana, Enemy_Wufeng_Summoner));

	float random_position_x = RandomizeFloatBetweenValues(sf::Vector2f(100.f, (*CurrentGame).m_map_size.x - 100.f));
	float random_position_y = RandomizeFloatBetweenValues(sf::Vector2f(100.f, (*CurrentGame).m_map_size.y - 100.f));

	Enemy* wufeng = new Enemy(random_type, sf::Vector2f(random_position_x, random_position_y));
	(*CurrentGame).addToScene(wufeng, EnemyObjectLayer, EnemyObject);

	return true;
}

void Landmark::CollisionWithWeapon(GameObject* enemy_weapon)
{
	Weapon* weapon = (Weapon*)enemy_weapon;

	std::vector<GameObject*>::iterator it = find(weapon->m_enemies_tagged.begin(), weapon->m_enemies_tagged.end(), this);
	if (it == weapon->m_enemies_tagged.end())
	{
		ActivateLandmark();
	}
}

void Landmark::CollisionWithBullet(GameObject* enemy_bullet)
{
	Ammo* bullet = (Ammo*)enemy_bullet;

	bullet->m_GarbageMe = true;
	bullet->m_visible = false;

	ActivateLandmark();
}