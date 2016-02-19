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
#include "Shop.h"
#include "Enemy.h"

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
	ShipModel(float m_max_speed, float m_acceleration, float m_deceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name);
	float getShipModelMaxSpeed();
	float getShipModelAcceleration();
	float getShipModelDeceleration();
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

	int armor;
	int shield;
	int shield_regen;
	int damage;

private:
	float deceleration;
	float acceleration;
	float max_speed;
	float hyperspeed;
	
};

class Equipment
{
public:
	void Init(int m_equipmentType, float m_max_speed, float m_acceleration, float m_deceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int m_damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name);
	Equipment();
	~Equipment();
	Equipment* Clone();
	std::string textureName;
	sf::Vector2f size;
	int frameNumber;
	std::string display_name;
	int equipmentType;
	float getEquipmentMaxSpeed();
	float getEquipmentAcceleration();
	float getEquipmentDeceleration();
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

	static Equipment* CreateRandomArmor(int credits_, int level);
	static Equipment* CreateRandomShield(int credits_, int level);
	static Equipment* CreateRandomEngine(int credits_, int level);
	static Equipment* CreateRandomModule(int credits_, int level);

	int armor;
	int shield;
	int shield_regen;
	int damage;
	int level;
	int credits;

private:
	float max_speed;
	float acceleration;
	float deceleration;
	float hyperspeed;
};

//class ShipConfig
//{
//public:
//	void Init();
//	ShipConfig();
//	std::string textureName;
//	sf::Vector2f size;
//
//	int frameNumber;
//	float getShipConfigMaxSpeed();
//	float getShipConfigAcceleration();
//	float getShipConfigDeceleration();
//	float getShipConfigHyperspeed();
//	int getShipConfigArmor();
//	int getShipConfigShield();
//	int getShipConfigShieldRegen();
//	int getShipConfigDamage();
//	bool setEquipment(Equipment* m_equipment, bool recomputing_stats = true, bool overwrite = false);
//	bool setShipModel(ShipModel* m_ship_model);
//	bool setShipWeapon(Weapon* m_weapon, bool recomputing_stats = true, bool overwrite = false);
//	Equipment* equipment[NBVAL_Equipment];
//	ShipModel* ship_model;
//	Weapon* weapon;
//	vector<Bot*> bot_list;
//	FX* FX_death;
//	void GenerateBots(GameObject* m_target);
//	void DestroyBots();
//	void GenerateFakeShip(GameObject* m_target);
//	FakeShip* m_fake_ship;
//	bool automatic_fire;
//
//private:
//	float max_speed;
//	float acceleration;
//	float deceleration;
//	float hyperspeed;
//	int armor;
//	int shield;
//	int shield_regen;
//	int damage;
//};

class Ship : public GameObject
{
public :
	Ship(ShipModel* ship_model);
	void Init();
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void updatePostCollision() override;
	void ManageDebugCommand();
	bool ManageVisibility();
	void ManageShieldRegen(sf::Time deltaTime, float hyperspeedMultiplier);
	void ManageAcceleration(sf::Vector2f inputs_direction);
	void ManageBraking();
	void ManageSlowMotion();
	void ManageHyperspeed();
	void ManageFiring(sf::Time deltaTime, float hyperspeedMultiplier);
	void ManageInteractions(sf::Vector2f input_directions);
	void ManageInputs(sf::Time deltaTime, float hyperspeedMultiplier, sf::Vector2f inputs_direction);
	void ManageOpeningHud();
	void ManageImmunity();
	bool ResplenishHealth();
	void ManageHudControls(sf::Vector2f inputs_directions);
	void TestingInputsRelease();
	void ManageFeedbackExpiration(sf::Time deltaTime);
	void IdleDecelleration(sf::Time deltaTime);
	void ScreenBorderContraints();
	void SettingTurnAnimations();
	PlayerHud ship_hud;

	void Respawn() override;
	bool setEquipment(Equipment* equipment, bool overwrite = false, bool no_save = false);
	bool setShipWeapon(Weapon* weapon, bool overwrite = false, bool no_save = false);
	void setShipModel(ShipModel* ship_model, bool no_save = false);
	void cleanEquipment(int equipment_type, bool no_save = false);
	void cleanWeapon(bool no_save = false);
	static GameObject* CloneEquipmentIntoGameObject(Equipment* new_equipment);
	static GameObject* CloneWeaponIntoGameObject(Weapon* new_weapon);
	
	void Death() override;
	bool GetLoot(GameObject& object) override;
	void GetPortal(GameObject* object) override;
	void GetShop(GameObject* object) override;
	Portal* targetPortal;
	Shop* targetShop;
	InteractionType previouslyCollidingWithInteractiveObject;
	bool wasHyperspeedingButtonPressed;
	bool isFiringButtonPressed;
	bool wasBrakingButtonPressed;
	bool isBrakingButtonHeldPressed;
	sf::Clock brakingHoldingClock;
	GameObject* previously_focused_item;

	void GetGrazing() override;
	int getGrazeCount();
	float getShipBeastScore();
	void damage_from (GameObject& object) override;

	Equipment* equipment[NBVAL_Equipment];
	ShipModel* ship_model;
	Weapon* weapon;
	vector<Bot*> bot_list;
	FX* FX_death;
	void GenerateBots(GameObject* m_target);
	void DestroyBots();
	void GenerateFakeShip(GameObject* m_target);
	FakeShip* m_fake_ship;
	bool automatic_fire;

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
	InteractionType m_interactionType;

	float getShipMaxSpeed();
	float getShipAcceleration();
	float getShipDeceleration();
	float getShipHyperspeed();
	int getShipArmor();
	int getShipShield();
	int getShipShieldRegen();
	int getShipDamage();

	bool isFocusedOnHud;

	string respawnSceneName;

	int graze_count;
	int graze_level;
	sf::Vector2f m_ship_size;

	int GetFocusedPortalMaxUnlockedHazardLevel();

	int level;
	int level_max;
	int xp;
	int xp_max;
	//void gain_xp (int xp_earned_);
	//void LevelUp();
	int UpdateShipLevel();

	static int SaveItems(string file);
	static bool LoadPlayerItems(string file);
	static Equipment* LoadEquipmentFromLine(string line);
	static Weapon* LoadWeaponFromLine(string line);
	static void SaveEquipmentData(ofstream& data, Equipment* equipment, bool skip_type);
	static void SaveWeaponData(ofstream& data, Weapon* weapon, bool skip_type, bool skip_level = false);

	float hyperspeed;
	
private:
	bool moving;
	bool movingX;
	bool movingY;
	float max_speed;
	float acceleration;
	float deceleration;

};

#endif // SHIP_H_INCLUDED
