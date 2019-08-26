#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Weapon.h";
#include "Marker.h"

enum HostilityLevel
{
	Hostility_Ally,
	Hostility_HoldFire,
	Hostility_ReturnFire,
	Hostility_FireAtWill,
	NB_HOSTILITY_LEVELS,
};

class Ship : public GameObject
{
public :
	Ship(){};
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Ship();

	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);

	static int Ship::SaveShip(Ship* ship);
	static bool Ship::LoadShip(Ship* ship);

	sf::Clock m_stroboscopic_effect_clock;

	//Star Hunter
	void ApplyFlightModel(sf::Time deltaTime, sf::Vector2f inputs_direction);
	void Update(sf::Time deltaTime) override;
	bool GetHitByAmmo(GameObject* ammo) override;

	float m_acceleration;
	float m_speed_max;
	float m_max_acceleration;
	float m_turn_speed;
	float m_max_braking;
	float m_idle_decelleration;
	vector<Weapon*> m_weapons;

	float m_hit_feedback_timer;

	Marker* m_marker;
	HostilityLevel m_hostility;
};

#endif // SHIP_H_INCLUDED
