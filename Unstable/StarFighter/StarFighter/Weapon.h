#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Assets.h"
#include "Ammo.h"

#define LASER_RATE_OF_FIRE			0.4f
#define LASER_DAMAGE				100
#define LASERFAST_RATE_OF_FIRE			0.2f
#define LASERFAST_DAMAGE				50

class Weapon : public sf::Sprite
{

public:
	Weapon(Ammo* Ammunition);
	~Weapon();

	std::string textureName;
	sf::Vector2f size;
	int frameNumber;
	std::string display_name;
	sf::Vector2f speed;
	bool firing_ready;
	void Fire(IndependantType m_collider_type, sf::Time deltaTime = sf::seconds(0), float hyperspeedMultiplier = 1.0f);
	bool isFiringReady(sf::Time deltaTime, float hyperspeedMultiplier);

	void CreateBullet(IndependantType m_collider_type, float offsetX=0, float dispersion=0);
	
	void FireSingleShot(IndependantType m_collider_type);
	void FireMultiShot(IndependantType m_collider_type);
	void FireAlternateShot(IndependantType m_collider_type);
	void FireAscendingShot(IndependantType m_collider_type);
	void FireDescendingShot(IndependantType m_collider_type);
	sf::Vector2i fire_direction;
	float rate_of_fire;
	sf::Time readyFireTimer;
	Ammo* ammunition;
	bool m_isReadyToFire;

	TargetSeaking target_seaking;
	void SeakTarget(IndependantType m_collider_type);

	//special weapon abilities
	int multishot;
	int xspread;
	ShotMode shot_mode;
	int shot_index;
	float dispersion;
	int rafale;
	int rafale_index;
	float rafale_cooldown;
	float angle_offset;
	float delay;
	sf::Vector2f weaponOffset;
	sf::Vector2f weapon_current_offset;
	float shot_angle;
	bool face_target;

	int level;
	int credits;
	
	sf::Vector2i getFireDirection_for_Direction (Directions direction);
	Weapon* Clone();

	static Weapon* CreateRandomWeapon(int credits_, int level);

private:
	bool fire_pattern_return;
};

#endif // WEAPON_H_INCLUDED