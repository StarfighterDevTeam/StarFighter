#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Background.h"

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
class Enemy;

static float HazardLevelsBeastBonus[HazardLevels::NB_HAZARD_LEVELS] = { 0.0, 0.5, 1.0, 1.5, 2.0 };

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
	GRAZE_LEVEL_BLUE,//2
	GRAZE_LEVEL_RED,//1
	GRAZE_LEVEL_YELLOW,//3
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
	Action_Muting,
	Action_Pausing,
	Action_Left,
	Action_Right,
	Action_Up,
	Action_Down,
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
	HUD_Trade,
	HUD_Upgrades,
};

enum ShopOptions
{
	//ShopHeal,
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
	void Draw(sf::RenderTexture& screen) override;
	void ManageDebugCommand();
	bool IsVisible();
	void ManageJumpFeedbacks();
	void ManageShieldRegen(sf::Time deltaTime, float hyperspeedMultiplier);
	void ManageAcceleration(sf::Vector2f inputs_direction);
	bool ManageFiring(sf::Time deltaTime, float hyperspeedMultiplier);
	void GetInputState(bool input_guy_boolean, PlayerActions action);
	void UpdateInputStates();
	void UpdateHUDStates();
	void ManageInputs(sf::Time deltaTime, float hyperspeedMultiplier, sf::Vector2f inputs_direction);
	void ManageImmunity(sf::Time deltaTime);
	void ManageGhost(sf::Time deltaTime);
	void ManageGrazingFeedback();
	bool ResplenishHealth();
	void RegenHealthFast(sf::Time deltaTime, bool armor, bool shield, bool hyperspeed_fuel);
	void IdleDecelleration(sf::Time deltaTime);
	void ScreenBorderConstraints();
	void SettingTurnAnimations();
	void RotateShip(float angle);
	void setGhost(bool ghost) override;

	bool setShipEquipment(Equipment* equipment, bool overwrite = false, bool no_save = false);
	bool setShipWeapon(Weapon* weapon, bool overwrite = false, bool no_save = false);
	void setShipModel(ShipModel* ship_model, bool no_save = false);
	void cleanEquipment(int equipment_type, bool no_save = false);
	void cleanWeapon(bool no_save = false);
	bool CanHyperspeed();
	bool CanJump();
	int GetNumberOfBombs();
	int GetBombsDamage();

	void CleanGarbagedEquipments();

	vector<Equipment*> m_garbageEquipments;
	vector<Weapon*> m_garbageWeapons;

	string m_currentScene_name;
	int m_currentScene_hazard;
	map<string, int> m_knownScenes;

	void RandomizeUpgrades(Shop* target_shop);
	void SetUpgrade(string upgrade_name);
	vector<string> m_upgrades;
	vector<string> m_upgrades_short;
	void SetWeapon(string weapon_name);
	void SetDrone(string drone_name, int number);
	void SetDroneWeapon(string weapon_name);
	
	void SetVisibility(bool visible);
	void Death(bool give_money = false) override;
	bool GetLoot(GameObject& object) override;
	void GetPortal(GameObject* object) override;
	void GetShop(GameObject* object) override;
	GameObject* GetFakeShip() override;
	//static void FillShopWithRandomObjets(size_t num_spawned_objects, Shop* shop, EnemyClass loot_class, int equipment_type);
	void MoveCursor(GameObject* cursor, sf::Vector2f inputs_directions, sf::Time deltaTime, SFPanel* container);
	sf::Vector2f GetShipSize();

	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	bool m_actions_states[NBVAL_PlayerActions];
	HUDStates m_HUD_state;

	bool UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition);

	bool BuyingItem_v2(GridElement* element, bool equip_directly = false);
	void SellingItem_v2(GridElement* element);
	bool DesquipItem_v2(GridElement* element);
	bool EquipItem_v2(GridElement* element);
	void ThrowingItem_v2(GridElement* element);
	bool Equip_v2(GameObject* object);
	void Desequip_v2(GameObject* object);

	void ContinueDialog();
	void Recalling();
	void Teleport(string destination_name);
	void CenterMapView(sf::Vector2f offset = sf::Vector2f(0, 0));
	void Bomb();
	void PlayStroboscopicEffect(Time effect_duration, Time time_between_poses);
	void Jump();
	void Respawn(bool no_save);

	sf::Clock m_stroboscopic_effect_clock;
	sf::Clock m_jump_clock;
	bool m_is_jumping;
	sf::Time m_ghost_timer;

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

	void GetGrazing(sf::Time deltaTime, float hyperspeedMultiplier) override;
	int getGrazeCount();
	float getShipBeastScore();
	void GetDamageFrom (GameObject& object) override;

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
	bool m_disableJump;
	bool m_disableRecall;
	bool m_disableSlowmotion;
	bool m_is_asking_scene_transition;
	SFTextPop* m_recall_text;
	bool m_release_to_fire;
	bool m_release_to_throw;

	string m_respawnSceneName;
	int m_last_hazard_level_played;

	sf::Clock m_recall_clock;

	int m_graze_count;
	int m_graze_level;
	sf::Vertex m_graze_percent_points[GRAZING_FEEDBACK_CIRCLE_POINTS * 2];
	sf::Clock m_graze_sinus_clock;
	int m_hits_taken;

	int m_combo_count;
	int m_combo_count_max;
	int m_combo_level;

	int GetFocusedPortalMaxUnlockedHazardLevel();

	int m_level;
	int m_level_max;
	int m_xp;
	int m_xp_max;
	//void gain_xp (int xp_earned_);
	//void LevelUp();
	int UpdateShipLevel();

	static int SavePlayerScenes(Ship* ship);
	static string LoadPlayerScenes(Ship* ship);
	static int SavePlayerMoneyAndHealth(Ship* ship);
	static bool LoadPlayerMoneyAndHealth(Ship* ship);
	static int SaveItems(Ship* ship);
	static bool LoadPlayerItems(Ship* ship);
	static int SavePlayerUpgrades(Ship* ship);
	static bool LoadPlayerUpgrades(Ship* ship);

	int m_hyperspeed_fuel_max;
	float m_hyperspeed_fuel;

	float m_hyperspeed;
	float m_max_speed;
	float m_acceleration;
	float m_deceleration;

	sf::Clock m_shield_recovery_clock;

	int getFighterIntStatValue(FighterStats stat) override;
	float getFighterFloatStatValue(FighterStats stat) override;
	void AddComboCount(int value) override;
	void AddDialog(Dialog* dialog) override;
	void SetInputBlocker(GameObject* blocker) override;
	GameObject* GetInputBlocker() override;
	void SetAskingPanel(SFPanelTypes type) override;
	
private:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;
};

#endif // SHIP_H_INCLUDED
