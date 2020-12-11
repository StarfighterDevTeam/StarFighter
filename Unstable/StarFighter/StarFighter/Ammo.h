#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "FX.h"

enum MissilePhase
{
	Missile_SlowDown,
	Missile_TrackTarget,
	Missile_FinalHeading,
};

class Ammo : public GameObject
{
public:
	Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int damage, FX* explosion, bool is_missile_model);
	Ammo* Clone();
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void Draw(sf::RenderTexture& screen) override;

	FX* m_explosion;
	float m_radius;
	float m_angspeed;
	float m_ref_speed;
	float m_range;
	float m_current_range;
	float m_shot_angle;
	float m_offset_x;//offset respect to weapon on firing
	bool m_isBeam;
	string m_sound_name;
	bool m_is_missile_model;

	//missile motion model
	float m_missile_turn_speed;
	float m_missile_turn_speed_track;
	float m_missile_acceleration;
	float m_missile_deceleration;
	float m_missile_speed_min;
	float m_missile_speed_max;
	float m_missile_speed_unlocking_target;
	MissilePhase m_missile_phase;
	sf::Vector2f m_missile_target_position;//for feedback only
	GameObject* m_missile_target_object;
};

#endif // AMMO_H_INCLUDED