#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"

#include "AsteroidField.h"

class Ship : public Destructible
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
	void Draw(RenderTarget& screen) override;
	void SetPosition(sf::Vector2f position) override;
	void GetHitByAmmo(GameObject* ammo) override;
	void GetHitByObject(GameObject* object) override;
	void HitWithGravitation(GameObject* object) override;
	void UpdateOrbit(sf::Time deltaTime);
	float GetRadius(bool include_shield) const override;
	float GetGravitationRange() override;
	void SetROE(RuleOfEngagement roe) override;
	bool HasWeapons() override;
	void AddAmmoToShotsFired(Ammo* ammo) override;

	sf::Vector2f m_inputs_direction;
	float m_acceleration;
	float m_acceleration_max;
	float m_turn_speed;
	float m_braking_max;
	float m_idle_decelleration;
	vector<Weapon*> m_weapons;
	vector<Ammo*> m_shots_fired;
	float m_range_max;
	float m_shield_range;
	int m_shield;
	int m_shield_max;
	float m_shield_regen;
	double m_shield_regen_buffer;
	double m_energy_regen_buffer;
	int m_energy;
	int m_energy_max;
	float m_energy_regen;
	float m_gravitation_range;
	float m_gravitation_strength;
	CircleDisplay* m_shield_circle;
	CircleDisplay* m_gravitation_circle;

	sf::RectangleShape m_health_rect;
	sf::RectangleShape m_health_container_rect;
	sf::RectangleShape m_shield_rect;
	sf::RectangleShape m_shield_container_rect;
	sf::RectangleShape m_energy_rect;
	sf::RectangleShape m_energy_container_rect;

	void UpdateShieldRegen(sf::Time deltaTime);
	void UpdateEnergyRegen(sf::Time deltaTime);
	void InitShip();

	bool m_isReflectingShots;

	Planet* m_isOrbiting;
	float m_orbit_angle;
	int m_orbit_cw;
};

#endif // SHIP_H_INCLUDED
