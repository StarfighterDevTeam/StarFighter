#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "AnimatedSprite.hpp"
#include "TextureLoader.h"
#include "Assets.h"
#include "Hud.h"
#include "Independant.h"
#include "InputGuy.h"
#include <cmath>
#include "Bot.h"
#include "PatternBobby.h"

#define SHIP_START_X                0
#define SHIP_START_Y                0
#define SHIP_ACCELERATION_X         20.0f
#define SHIP_ACCELERATION_Y         20.0f
#define SHIP_DECCELERATION_COEF		1000.0f
#define SHIP_MAX_SPEED_X            500.0f
#define SHIP_MAX_SPEED_Y            500.0f
#define SHIP_MIN_SPEED_X            50.0f
#define SHIP_MIN_SPEED_Y            50.0f
#define SHIP_ARMOR					600
#define SHIP_SHIELD					1000
#define SHIP_SHIELD_REGEN			1
#define SHIP_SPRITE_RATE_SEC        0.2f

class Loot;

enum GrazeLevels
{
	GRAZE_LEVEL_NONE,//0
	GRAZE_LEVEL_RED,//1
	GRAZE_LEVEL_BLUE,//2
	GRAZE_LEVEL_WHITE,//3
	NB_GRAZE_LEVELS,//4
};

class ShipModel : public sf::Sprite
{
public:
	ShipModel(sf::Vector2f m_max_speed, sf::Vector2f m_acceleration, float m_decceleration, float m_armor, float m_shield, float m_shield_regen, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name);
	sf::Vector2f getShipModelMaxSpeed();
	float getShipModelDecceleration();
	sf::Vector2f getShipModelAcceleration();
	int getShipModelArmor();
	int getShipModelShield();
	int getShipModelShieldRegen();
	std::string textureName;
	sf::Vector2f size;
	int frameNumber;
	std::string display_name;
	Bot* bot;
	bool hasBot;
	
	bool hasFake;
	std::string fake_textureName;
	sf::Vector2f fake_size;
	int fake_frameNumber;

private:
	float decceleration;
	sf::Vector2f acceleration;
	sf::Vector2f max_speed;
	int armor;
	int shield;
	int shield_regen;
};

class Equipment : public sf::Sprite
{
public:
	void Init(int m_equipmentType, sf::Vector2f m_max_speed, float m_decceleration , sf::Vector2f m_acceleration, int m_armor, int m_shield, int m_shield_regen, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name);
	Equipment();
	std::string textureName;
	sf::Vector2f size;
	int frameNumber;
	std::string display_name;
	int equipmentType;
	sf::Vector2f getEquipmentMaxSpeed();
	float getEquipmentDecceleration();
	sf::Vector2f getEquipmentAcceleration();
	int getEquipmentArmor();
	int getEquipmentShield();
	int getEquipmentShieldRegen();
	Bot* bot;
	bool hasBot;

	bool hasFake;
	std::string fake_textureName;
	sf::Vector2f fake_size;
	int fake_frameNumber;


	void Equipment::AddAirbrakeProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void Equipment::AddEngineProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void Equipment::AddArmorProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void Equipment::AddShieldProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void Equipment::AddModuleProperty(int chosen_property, int value, sf::Vector2f BeastScale);

private:
	float decceleration;
	sf::Vector2f acceleration;
	sf::Vector2f max_speed;
	int armor;
	int shield;
	int shield_regen;
};

class ShipConfig
{
public:
	void Init();
	ShipConfig();
	std::string textureName;
	sf::Vector2f size;

	int frameNumber;
	sf::Vector2f getShipConfigMaxSpeed();
	float getShipConfigDecceleration();
	sf::Vector2f getShipConfigAcceleration();
	int getShipConfigArmor();
	int getShipConfigShield();
	int getShipConfigShieldRegen();
	void setEquipment(Equipment* m_equipment, bool recomputing_stats = true);
	void setShipModel(ShipModel* m_ship_model);
	void setShipWeapon(Weapon* m_weapon, bool recomputing_stats = true);
	Equipment* equipment[NBVAL_EQUIPMENT];
	ShipModel* ship_model;
	Weapon* weapon;
	Bot* bot;
	vector<Bot*> bot_list;
	bool hasWeapon;
	bool hasEquipment[NBVAL_EQUIPMENT];
	FX* FX_death;
	void GenerateBots(Independant* m_target);
	void DestroyBots();
	void GenerateFakeShip(Independant* m_target);
	bool automatic_fire;
	

private:
	sf::Vector2f max_speed;
	float decceleration;
	sf::Vector2f acceleration;
	int armor;
	int shield;
	int shield_regen;
};

class Ship : public Independant
{
public :
	Ship(Vector2f position, ShipConfig m_ship_config);
	void Init();
	void update(sf::Time deltaTime) override;
	void setShipConfig(ShipConfig m_ship_config);
	ShipConfig ship_config;
	PlayerHud ship_hud;
	vector<Loot*> stash;

	void Respawn() override;
	void setEquipment(Equipment* m_equipment);
	void setShipModel(ShipModel* m_ship_model);
	void setShipWeapon(Weapon* m_weapon);
	
	void Death() override;
	void GetLoot(Independant& independant) override;
	void GetGrazing() override;
	int getGrazeCount();
	float getShipBeastScore();
	void Ship::damage_from (Independant& independant) override;

	bool disable_inputs;
	bool disable_fire;
	Aura* combo_aura[GrazeLevels::NB_GRAZE_LEVELS];
	bool key_repeat;

	
private:
	bool moving;
	bool movingX;
	bool movingY;
	int graze_count;
	int graze_level;
};

#endif // SHIP_H_INCLUDED
