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
	ShipModel(float max_speed, float acceleration, float deceleration, float hyperspeed, int armor, int shield, int shield_regen, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name);
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
	int m_damage;

	float m_deceleration;
	float m_acceleration;
	float m_max_speed;
	float m_hyperspeed;
};

class Equipment
{
public:
	void Init(int equipmentType, float max_speed, float acceleration, float deceleration, float hyperspeed, int armor, int shield, int shield_regen, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name);
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
	int m_damage;
	int m_level;
	int m_credits;

	float m_hyperspeed;
	float m_max_speed;
	float m_acceleration;
	float m_deceleration;
};

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

	Portal* m_targetPortal;
	Shop* m_targetShop;
	InteractionType m_previouslyCollidingWithInteractiveObject;

	bool m_wasHyperspeedingButtonPressed;
	bool m_isFiringButtonPressed;
	bool m_wasBrakingButtonPressed;
	bool m_isBrakingButtonHeldPressed;
	sf::Clock m_brakingHoldingClock;
	GameObject* m_previously_focused_item;

	void GetGrazing() override;
	int getGrazeCount();
	float getShipBeastScore();
	void damage_from (GameObject& object) override;

	Equipment* m_equipment[NBVAL_Equipment];
	ShipModel* m_ship_model;
	Weapon* m_weapon;
	vector<Bot*> m_bot_list;
	FX* m_FX_death;
	void GenerateBots(GameObject* target);
	void DestroyBots();
	void GenerateFakeShip(GameObject* target);
	FakeShip* m_fake_ship;
	bool m_automatic_fire;

	bool m_disable_inputs;
	Aura* m_combo_aura[GrazeLevels::NB_GRAZE_LEVELS];
	Aura* m_trail;
	bool m_fire_key_repeat;
	bool m_slowmo_key_repeat;
	bool m_hud_key_repeat;
	bool m_isBraking;
	bool m_isHyperspeeding;
	bool m_isSlowMotion;
	bool m_disabledHyperspeed;
	InteractionType m_interactionType;

	int getFighterIntStatValue(FighterStats stat) override;
	float getFighterFloatStatValue(FighterStats stat) override;

	bool m_isFocusedOnHud;

	string m_respawnSceneName;
	int m_last_hazard_level_played;

	int m_graze_count;
	int m_graze_level;

	int GetFocusedPortalMaxUnlockedHazardLevel();

	int m_level;
	int m_level_max;
	int m_xp;
	int m_xp_max;
	//void gain_xp (int xp_earned_);
	//void LevelUp();
	int UpdateShipLevel();

	static int SaveItems(string file);
	static bool LoadPlayerItems(string file);
	static Equipment* LoadEquipmentFromLine(string line);
	static Weapon* LoadWeaponFromLine(string line);
	static void SaveEquipmentData(ofstream& data, Equipment* equipment, bool skip_type);
	static void SaveWeaponData(ofstream& data, Weapon* weapon, bool skip_type, bool skip_level = false);

	float m_hyperspeed;
	float m_max_speed;
	float m_acceleration;
	float m_deceleration;
	
private:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;

};

#endif // SHIP_H_INCLUDED
