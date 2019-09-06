#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"

#include "Planet.h"

class Ship : public SpatialObject
{
public :
	Ship();
	virtual ~Ship();

	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);

	static int Ship::SaveShip(Ship* ship);
	static bool Ship::LoadShip(Ship* ship);

	sf::Clock m_stroboscopic_effect_clock;

	//Star Hunter
	void ApplyFlightModel(sf::Time deltaTime, sf::Vector2f inputs_direction);
	void Update(sf::Time deltaTime) override;
	bool GetHitByAmmo(GameObject* ammo) override;
	void UpdateOrbit(sf::Time deltaTime);
	void UpdateWeaponRangeAndAngleCoverage();

	float m_acceleration;
	float m_acceleration_max;
	float m_speed_max;
	float m_turn_speed;
	float m_braking_max;
	float m_idle_decelleration;
	vector<Weapon*> m_weapons;
	float m_range_max;
	float m_angle_coverage_max;

	float m_hit_feedback_timer;
	Planet* m_isOrbiting;
	float m_orbit_angle;
	int m_orbit_cw;
};

#endif // SHIP_H_INCLUDED
