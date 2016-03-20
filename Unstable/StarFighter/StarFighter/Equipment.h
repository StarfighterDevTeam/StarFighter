#ifndef EQUIPMENT_H_INCLUDED
#define EQUIPMENT_H_INCLUDED

#include "Globals.h"
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "AnimatedSprite.hpp"
#include "TextureLoader.h"
#include "Assets.h"
#include "InputGuy.h"
#include <cmath>
#include "Bot.h"
#include "PatternBobby.h"
#include "Portal.h"
#include "Shop.h"
#include "Enemy.h"

class ShipModel
{
public:
	ShipModel(float max_speed, float acceleration, float deceleration, float hyperspeed, int armor, int shield, int shield_regen, float shield_recovery_time, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name);
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
	float m_shield_recovery_time;
	int m_damage;

	float m_deceleration;
	float m_acceleration;
	float m_max_speed;
	float m_hyperspeed;
};

class Equipment
{
public:
	void Init(int equipmentType, float max_speed, float acceleration, float deceleration, float hyperspeed, int armor, int shield, int shield_regen, float shield_recovery_time, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name);
	Equipment();
	~Equipment();
	Equipment* Clone();
	std::string m_textureName;
	sf::Vector2f m_size;
	int m_frameNumber;
	std::string m_display_name;
	int m_equipmentType;
	Bot* m_bot;

	std::string m_fake_textureName;
	sf::Vector2f m_fake_size;
	int m_fake_frameNumber;

	static Equipment* CreateRandomArmor(int credits_, int level);
	static Equipment* CreateRandomShield(int credits_, int level);
	static Equipment* CreateRandomEngine(int credits_, int level);
	static Equipment* CreateRandomModule(int credits_, int level);

	int m_armor;
	int m_shield;
	int m_shield_regen;
	float m_shield_recovery_time;
	int m_damage;
	int m_level;
	int m_credits;

	float m_hyperspeed;
	float m_max_speed;
	float m_acceleration;
	float m_deceleration;
};

#endif // EQUIPMENT_H_INCLUDED
