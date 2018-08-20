#ifndef LANDMARK_H_INCLUDED
#define LANDMARK_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "Enemy.h"// > Loot > Weapon > Ammo > FX

enum LandmarkType
{
	Landmark_Gong,
	NBVAL_LANDMARK_TYPES,
};

class Landmark : public GameObject
{
public :
	Landmark(LandmarkType type, sf::Vector2f position);
	~Landmark();
	void update(sf::Time deltaTime) override;
	Aura* m_fake_landmark;

	LandmarkType m_type;
	bool m_activated;

	void CollisionWithWeapon(GameObject* enemy_weapon) override;
	void CollisionWithBullet(GameObject* enemy_bullet) override;
	bool ActivateLandmark();
	bool SpawnRandomEnemy();
};

#endif // LANDMARK_H_INCLUDED
