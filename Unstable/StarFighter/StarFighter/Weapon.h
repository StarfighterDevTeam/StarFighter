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

	std::string m_textureName;
	sf::Vector2f m_size;
	int m_frameNumber;
	std::string m_display_name;
	sf::Vector2f m_speed;
	bool m_firing_ready;

	void Fire(GameObjectType m_collider_type, sf::Time deltaTime = sf::seconds(0), float hyperspeedMultiplier = 1.0f);
	bool isFiringReady(sf::Time deltaTime, float hyperspeedMultiplier);
	void CreateBullet(GameObjectType collider_type, float offsetX=0, float dispersion=0);

	void FireSingleShot(GameObjectType collider_type);
	void FireMultiShot(GameObjectType collider_type);
	void FireAlternateShot(GameObjectType collider_type);
	void FireAscendingShot(GameObjectType collider_type);
	void FireDescendingShot(GameObjectType collider_type);

	sf::Vector2i m_fire_direction;
	float m_rate_of_fire;
	sf::Time m_readyFireTimer;
	Ammo* m_ammunition;
	bool m_isReadyToFire;
	TargetSeaking m_target_seaking;

	void SeakTarget(GameObjectType collider_type);

	//special weapon abilities
	int m_multishot;
	int m_xspread;
	ShotMode m_shot_mode;
	int m_shot_index;
	float m_dispersion;
	int m_rafale;
	int m_rafale_index;
	float m_rafale_cooldown;
	float m_angle_offset;
	float m_delay;
	sf::Vector2f m_weaponOffset;
	sf::Vector2f m_weapon_current_offset;
	float m_shot_angle;
	bool m_face_target;

	int m_level;
	int m_credits;
	
	sf::Vector2i getFireDirection_for_Direction (Directions direction);
	Weapon* Clone();

	static Weapon* CreateRandomWeapon(int credits_, int level, bool is_bot);

private:
	bool m_fire_pattern_return;
};

#endif // WEAPON_H_INCLUDED