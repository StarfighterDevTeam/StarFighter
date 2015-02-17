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

enum ShotMode
{
	NoShotMode,//0
	AlternateShotMode,//1
	AscendingShotMode,//2
	DescendingShotMode,//3
};

class Weapon : public sf::Sprite
{

public:
	Weapon(Ammo* Ammunition);

	std::string textureName;
	sf::Vector2f size;
	int frameNumber;
	std::string display_name;
	sf::Vector2f speed;
	bool firing_ready;
	void Fire(IndependantType m_collider_type);

	void CreateBullet(IndependantType m_collider_type, float offsetX=0, float dispersion=0);
	bool isFiringReady();
	void FireSingleShot(IndependantType m_collider_type);
	void FireMultiShot(IndependantType m_collider_type);
	void FireAlternateShot(IndependantType m_collider_type);
	void FireAscendingShot(IndependantType m_collider_type);
	void FireDescendingShot(IndependantType m_collider_type);
	sf::Vector2f AngleShot(float angle, float m_ref_speed);
	sf::Vector2i fire_direction;
	float rate_of_fire;
	sf::Clock deltaClock;
	Ammo* ammunition;

	TargetSeaking target_seaking;
	void SeakTarget(IndependantType m_collider_type);

	//special weapon abilities
	int multishot;
	int xspread;
	ShotMode shot_mode;
	int shot_index;
	float angle;
	float dispersion;
	int rafale;
	int rafale_index;
	float rafale_cooldown;
	float angle_offset;

	sf::Vector2f weaponOffset;
	
	sf::Vector2i getFireDirection_for_Direction (Directions direction);
	void AddWeaponProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void AddBotWeaponProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	Weapon* Clone();
};

#endif // WEAPON_H_INCLUDED