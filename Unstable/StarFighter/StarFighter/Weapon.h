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

enum WeaponType {
	Laser,
	LaserFast,
	NBVAL_Weapon
};

struct Weapon : public sf::Sprite
{

public:
	Weapon(Ammo* Ammunition);

	sf::Vector2f speed;
	bool firing_ready;
	void Fire(IndependantType m_collider_type);

	void CreateBullet(IndependantType m_collider_type, float offsetX=0, float dispersion=0);
	bool isFiringReady();
	void FireSingleShot(IndependantType m_collider_type);
	void FireMultiShot(IndependantType m_collider_type);
	void FireAlternateShot(IndependantType m_collider_type);
	sf::Vector2f AngleShot(float angle, float m_ref_speed);
	sf::Vector2i fire_direction;
	float rate_of_fire;
	sf::Clock deltaClock;
	Ammo* ammunition;

	//special weapon abilities
	int multishot;
	int xspread;
	bool alternate;
	int shot_index;
	float angle;
	float dispersion;
	int rafale;
	int rafale_index;
	float rafale_cooldown;

	sf::Vector2f weaponOffset;

	sf::Vector2f OffsetWeapon(float angle);

	Weapon* Clone();
};


#endif // WEAPON_H_INCLUDED