#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Equipment.h"
#include "SFTextPop.h"

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
class Dialog;
class ShipModel;

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

enum PlayerInputStates
{
	Input_Release,//0
	Input_Tap,//1
	Input_Hold,//2
};

enum PlayerActions
{
	Action_Idle,
	Action_Firing,
	Action_Braking,
	Action_Hyperspeeding,
	Action_Slowmotion,
	Action_Recalling,
	Action_OpeningHud,
	Action_ChangingResolution,
	Action_AutomaticFire,
	Action_DebugCommand,
	NBVAL_PlayerActions,
};

enum HUDStates
{
	HUD_Idle,
	HUD_PortalInteraction,
	HUD_OpeningEquipment,
	HUD_ShopMainMenu,
	HUD_ShopBuyMenu,
	HUD_ShopSellMenu,
	HUD_ShopStellarMap,
	HUD_Dialog,
};

enum ShopOptions
{
	ShopHeal,
	ShopBuy,
	ShopStellarMap,
	NBVAL_ShopOptions,
};

class Ship : public GameObject
{
public :
	Ship(ShipModel* ship_model);
	~Ship();
	void Init();
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void updatePostCollision() override;
	void ManageDebugCommand();
	bool ManageVisibility();
	void ManageShieldRegen(sf::Time deltaTime, float hyperspeedMultiplier);
	void ManageAcceleration(sf::Vector2f inputs_direction);
	void ManageFiring(sf::Time deltaTime, float hyperspeedMultiplier);
	void GetInputState(bool input_guy_boolean, PlayerActions action);
	void UpdateInputStates();
	void UpdateHUDStates();
	void ManageInputs(sf::Time deltaTime, float hyperspeedMultiplier, sf::Vector2f inputs_direction);
	void ManageImmunity();
	bool ResplenishHealth();
	void ManageFeedbackExpiration(sf::Time deltaTime);
	void IdleDecelleration(sf::Time deltaTime);
	void ScreenBorderContraints();
	void SettingTurnAnimations();
	void RotateShip(float angle);

	void Respawn() override;
	bool setShipEquipment(Equipment* equipment, bool overwrite = false, bool no_save = false);
	bool setShipWeapon(Weapon* weapon, bool overwrite = false, bool no_save = false);
	void setShipModel(ShipModel* ship_model, bool no_save = false);
	void cleanEquipment(int equipment_type, bool no_save = false);
	void cleanWeapon(bool no_save = false);
	static GameObject* CloneEquipmentIntoGameObject(Equipment* new_equipment);
	static GameObject* CloneWeaponIntoGameObject(Weapon* new_weapon);

	void CleanGarbagedEquipments();

	vector<Equipment*> m_garbageEquipments;
	vector<Weapon*> m_garbageWeapons;

	string m_currentScene_name;
	int m_currentScene_hazard;
	map<string, int> m_knownScenes;
	
	void SetVisibility(bool visible);
	void Death() override;
	bool GetLoot(GameObject& object) override;
	void GetPortal(GameObject* object) override;
	void GetShop(GameObject* object) override;
	static void FillShopWithRandomObjets(size_t num_spawned_objects, Shop* shop, EnemyClass loot_class);
	void MoveCursor(GameObject* cursor, sf::Vector2f inputs_directions, sf::Time deltaTime, SFPanel* container);
	void ForceCursorOnEquivalentObjectInGrid(GameObject* focused_object, ObjectGrid* grid);

	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	bool m_actions_states[NBVAL_PlayerActions];
	HUDStates m_HUD_state;

	bool UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition);

	void BuyingItem();
	void SellingItem();
	void GarbagingItem();
	void EquipItem();
	void DesequipItem();
	void ContinueDialog();
	void Recalling();
	void Teleport(string destination_name);
	void CenterMapView(sf::Vector2f offset = sf::Vector2f(0, 0));

	Portal* m_targetPortal;
	Shop* m_targetShop;
	vector<Dialog*> m_targetDialogs;
	GameObject* m_input_blocker;
	InteractionType m_previouslyCollidingWithInteractiveObject;

	SFPanel* m_SFTargetPanel;
	SFPanelTypes m_is_asking_SFPanel;
	SFPanel* m_SFHudPanel;
	string m_is_asking_teleportation;

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
	void SetBotsVisibility(bool visible);
	void DestroyBots();
	FakeShip* m_fake_ship;
	bool m_automatic_fire;

	bool m_disable_bots;
	bool m_disable_inputs;
	Aura* m_combo_aura;
	Aura* m_trail;
	bool m_disableHyperspeed;
	bool m_disableRecall;
	bool m_disableSlowmotion;
	bool m_is_asking_scene_transition;
	SFTextPop* m_recall_text;

	int getFighterIntStatValue(FighterStats stat) override;
	float getFighterFloatStatValue(FighterStats stat) override;

	string m_respawnSceneName;
	int m_last_hazard_level_played;

	sf::Clock m_recall_clock;

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

	static int SavePlayerMoney(string file, Ship* ship);
	static bool LoadPlayerMoney(string file, Ship* ship);
	static int SaveItems(string file, Ship* ship);
	static bool LoadPlayerItems(string file, Ship* ship);
	static Equipment* LoadEquipmentFromLine(string line);
	static Weapon* LoadWeaponFromLine(string line);
	static void SaveEquipmentData(ofstream& data, Equipment* equipment, bool skip_type);
	static void SaveWeaponData(ofstream& data, Weapon* weapon, bool skip_type, bool skip_level = false);

	int m_hyperspeed_fuel_max;
	float m_hyperspeed_fuel;

	float m_hyperspeed;
	float m_max_speed;
	float m_acceleration;
	float m_deceleration;

	sf::Clock m_shield_recovery_clock;
	
private:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;

};

#endif // SHIP_H_INCLUDED
