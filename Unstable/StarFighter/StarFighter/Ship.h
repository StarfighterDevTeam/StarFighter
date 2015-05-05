#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "AnimatedSprite.hpp"
#include "TextureLoader.h"
#include "Assets.h"
#include "Hud.h"
#include "InputGuy.h"
#include <cmath>
#include "Bot.h"
#include "PatternBobby.h"
#include "Portal.h"
//#include "Independant.h"

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

enum ShipAnimations
{
	ShipIdle,//0
	ShipTurningRight,//1
	ShipTurningLeft,//2
	NB_ShipAnimations,//3
};

enum GrazeLevels
{
	GRAZE_LEVEL_NONE,//0
	GRAZE_LEVEL_RED,//1
	GRAZE_LEVEL_BLUE,//2
	GRAZE_LEVEL_WHITE,//3
	NB_GRAZE_LEVELS,//4
};

class ShipModel
{
public:
	ShipModel(float m_max_speed, float m_acceleration, float m_decceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name);
	sf::Vector2f getShipModelMaxSpeed();
	sf::Vector2f getShipModelAcceleration();
	float getShipModelDecceleration();
	float getShipModelHyperspeed();
	int getShipModelArmor();
	int getShipModelShield();
	int getShipModelShieldRegen();
	int getShipModelDamage();
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
	float hyperspeed;
	int armor;
	int shield;
	int shield_regen;
	int damage;
};

class Equipment
{
public:
	void Init(int m_equipmentType, float m_max_speed, float m_acceleration, float m_decceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int m_damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name);
	Equipment();
	~Equipment();
	Equipment* Clone();
	std::string textureName;
	sf::Vector2f size;
	int frameNumber;
	std::string display_name;
	int equipmentType;
	sf::Vector2f getEquipmentMaxSpeed();
	sf::Vector2f getEquipmentAcceleration();
	float getEquipmentDecceleration();
	float getEquipmentHyperspeed();
	int getEquipmentArmor();
	int getEquipmentShield();
	int getEquipmentShieldRegen();
	int getEquipmentDamage();
	Bot* bot;
	bool hasBot;

	bool hasFake;
	std::string fake_textureName;
	sf::Vector2f fake_size;
	int fake_frameNumber;

	void AddAirbrakeProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void AddEngineProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void AddArmorProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void AddShieldProperty(int chosen_property, int value, sf::Vector2f BeastScale);
	void AddModuleProperty(int chosen_property, int value, sf::Vector2f BeastScale);

private:
	sf::Vector2f max_speed;
	sf::Vector2f acceleration;
	float decceleration;
	float hyperspeed;
	int armor;
	int shield;
	int shield_regen;
	int damage;
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
	sf::Vector2f getShipConfigAcceleration();
	float getShipConfigDecceleration();
	float getShipConfigHyperspeed();
	int getShipConfigArmor();
	int getShipConfigShield();
	int getShipConfigShieldRegen();
	int getShipConfigDamage();
	bool setEquipment(Equipment* m_equipment, bool recomputing_stats = true, bool overwrite = false);
	bool setShipModel(ShipModel* m_ship_model);
	bool setShipWeapon(Weapon* m_weapon, bool recomputing_stats = true, bool overwrite = false);
	Equipment* equipment[NBVAL_Equipment];
	ShipModel* ship_model;
	Weapon* weapon;
	vector<Bot*> bot_list;
	FX* FX_death;
	void GenerateBots(Independant* m_target);
	void DestroyBots();
	void GenerateFakeShip(Independant* m_target);
	FakeShip* m_fake_ship;
	bool automatic_fire;

private:
	sf::Vector2f max_speed;
	sf::Vector2f acceleration;
	float decceleration;
	float hyperspeed;
	int armor;
	int shield;
	int shield_regen;
	int damage;
};

class Ship : public Independant
{
public :
	Ship(Vector2f position, ShipConfig m_ship_config);
	void Init();
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void setShipConfig(ShipConfig m_ship_config);
	ShipConfig ship_config;
	PlayerHud ship_hud;

	void Respawn() override;
	bool setEquipment(Equipment* m_equipment, bool overwrite = false);
	bool setShipWeapon(Weapon* m_weapon, bool overwrite = false);
	void setShipModel(ShipModel* m_ship_model);
	void cleanEquipment(int equipment_type);
	void cleanWeapon();
	static Independant* CloneEquipmentIntoIndependant(Equipment* new_equipment);
	static Independant* CloneWeaponIntoIndependant(Weapon* new_weapon);
	
	void Death() override;
	bool GetLoot(Independant& independant) override;
	void GetPortal(Independant* independant) override;
	Portal* targetPortal;
	bool isCollindingWithPortal;
	bool isFiringButtonPressed;
	bool wasBrakingButtonPressed;
	bool isBrakingButtonHeldPressed;
	sf::Clock brakingHoldingClock;
	Independant* previously_focused_item;

	void GetGrazing() override;
	int getGrazeCount();
	float getShipBeastScore();
	void damage_from (Independant& independant) override;

	bool disable_inputs;
	Aura* m_combo_aura[GrazeLevels::NB_GRAZE_LEVELS];
	Aura* trail;
	bool fire_key_repeat;
	bool slowmo_key_repeat;
	bool hud_key_repeat;
	bool isBraking;
	bool isHyperspeeding;
	bool isSlowMotion;
	bool disabledHyperspeed;

	bool isFocusedOnHud;

	int graze_count;
	int graze_level;
	
private:
	bool moving;
	bool movingX;
	bool movingY;
};

#endif // SHIP_H_INCLUDED
