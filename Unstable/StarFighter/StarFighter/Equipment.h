#ifndef EQUIPMENT_H_INCLUDED
#define EQUIPMENT_H_INCLUDED

#include "Bot.h"

class ShipModel
{
public:
	ShipModel(float max_speed, float acceleration, float deceleration, float hyperspeed, int hyperspeed_fuel, int armor, int shield, int shield_regen, float shield_recovery_time, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name);
	~ShipModel();
	std::string m_textureName;
	sf::Vector2f m_size;
	int m_frameNumber;
	std::string m_display_name;
	Bot* m_bot;
	
	std::string m_fake_textureName;
	sf::Vector2f m_fake_size;
	int m_fake_frameNumber;

	int m_armor;
	int m_shield;
	int m_shield_regen;
	int m_hyperspeed_fuel;
	int m_damage;

	float m_deceleration;
	float m_acceleration;
	float m_max_speed;
	float m_hyperspeed;
	float m_shield_recovery_time;

	bool m_can_hyperspeed;
	bool m_can_jump;
	int m_bombs;
};

class Equipment
{
public:
	void Init(int equipmentType, float max_speed, float acceleration, float deceleration, float hyperspeed, int m_hyperspeed_fuel, int armor, int shield, int shield_regen, float shield_recovery_time, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name);
	Equipment();
	~Equipment();
	Equipment* Clone();
	std::string m_textureName;
	sf::Vector2f m_size;
	int m_frameNumber;
	std::string m_display_name;
	int m_equipmentType;
	vector<Bot*> m_bots;

	std::string m_fake_textureName;
	sf::Vector2f m_fake_size;
	int m_fake_frameNumber;

	int m_armor;
	int m_shield;
	int m_shield_regen;
	int m_hyperspeed_fuel;
	int m_damage;
	int m_level;
	int m_credits;

	float m_quality;
	float m_shield_recovery_time;
	float m_hyperspeed;
	float m_max_speed;
	float m_acceleration;
	float m_deceleration;

	bool m_can_hyperspeed;
	bool m_can_jump;
	int m_bombs;
};

#endif // EQUIPMENT_H_INCLUDED
