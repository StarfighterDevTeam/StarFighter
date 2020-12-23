#ifndef INDEPENDANT_H_INCLUDED
#define INDEPENDANT_H_INCLUDED

#include "Dialog.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using namespace sf;

class Equipment;
class Weapon;
class Loot;

enum SFPanelTypes
{
	SFPanel_None,
	SFPanel_ItemStats,
	SFPanel_HUD,
	SFPanel_Action,
	SFPanel_Portal,
	SFPanel_Shop,
	SFPanel_Dialog,
	SFPanel_DialogNext,
	SFPanel_Map,
	SFPanel_MapInfo,
	SFPanel_Trade,
	SFPanel_Upgrades,
};

enum FighterStats
{
	Fighter_Armor,
	Fighter_ArmorMax,
	Fighter_Shield,
	Fighter_ShieldMax,
	Fighter_ShieldRegen,
	Fighter_ShieldRecovery,
	Fighter_ContactDamage,
	Fighter_MaxSpeed,
	Fighter_Acceleration,
	Fighter_Deceleration,
	Fighter_Hyperspeed,
	Fighter_HyperspeedFuel,
	Fighter_Credits,
	Fighter_Level,
	NBVAL_FighterStats,
};

enum LayerType 
{
	BackgroundLayer,
	PortalLayer,
	FriendlyBeamLayer,
	EnemyBeamLayer,
	ExplosionLayer,
	EnemyObjectLayer,
	AuraLayer,
	FeedbacksLayer,
	BotLayer,
	FriendlyFireLayer,
	FakeShipLayer,
	LootLayer,
	PlayerStroboscopicLayer,
	PlayerShipLayer,
	EnemyFireLayer,
	BlackStripesLayer,
	PanelLayer,
	NBVAL_Layer
};

enum WeaponData
{
	WEAPON_NAME,
	WEAPON_DISPLAY_NAME,
	WEAPON_IMAGE_NAME,
	WEAPON_WIDTH,
	WEAPON_HEIGHT,
	WEAPON_FRAMES,
	WEAPON_AMMO,
	WEAPON_DAMAGE,
	WEAPON_SPEED,
	WEAPON_RANGE,
	WEAPON_RATE_OF_FIRE,
	WEAPON_RAFALE,
	WEAPON_RAFALE_COOLDOWN,
	WEAPON_RAFALE_LOCKING,
	WEAPON_MULTISHOT,
	WEAPON_XSPREAD,
	WEAPON_DISPERSION,
	WEAPON_ALTERNATE,
	WEAPON_ANGLE_OFFSET,
	WEAPON_TARGET_HOMING,
	WEAPON_PATTERN,
	WEAPON_PATTERN_SPEED,
	WEAPON_PATTERN_CLOCKWISE,
	WEAPON_PATTERN_STARTING_POINT,
	WEAPON_PATTERN_WIDTH,
	WEAPON_PATTERN_HEIGHT,
};

enum TargetHoming
{
	NO_HOMING,
	SEMI_HOMING,//aim once every rafale
	HOMING,//aim once every bullet
	//SUPER_HOMING,//bullet follows target
};

enum ShotMode
{
	NoShotMode,//0
	AlternateShotMode,//1
	AscendingShotMode,//2
	DescendingShotMode,//3
	Ascending2ShotMode,//4 - back and forth
	Descending2ShotMode,//5 - back and forth
};

enum AmmoData
{
	AMMO_NAME,//0
	AMMO_IMAGE_NAME,//1
	AMMO_WIDTH,//2
	AMMO_HEIGHT,//3
	AMMO_FRAMES,//4
	AMMO_SKIN,//5
	AMMO_NB_SKINS,//6
	AMMO_FX,//7
	AMMO_SOUND,//8
	AMMO_ROTATION_SPEED,//9
	AMMO_MISSILE_MODEL,//10
	AMMO_AREA_OF_EFFECT,//11
};

enum FXData
{
	FX_TYPE,//0
	FX_NAME,//1
	FX_FILENAME,//2
	FX_WIDTH,//3
	FX_HEIGHT,//4
	FX_FRAMES,//5
	FX_DURATION,//6
};

enum BotData
{
	BOT_NAME,//0
	BOT_ARMOR,//1
	BOT_SHIELD,//2
	BOT_SHIELD_REGEN,//3
	BOT_DAMAGE,//4
	BOT_WEAPON,//5
	BOT_IMAGE_NAME,//6
	BOT_WIDTH,//7
	BOT_HEIGHT,//8
	BOT_FRAMES,//9
	BOT_FX_DEATH,//10
	BOT_PATTERN,//11
	BOT_PATTERN_SPEED,//12
	BOT_PATTERN_CLOCKWISE,//13
	BOT_PATTERN_STARTING_POINT,//14
	BOT_PATTERN_WIDTH,//15
	BOT_PATTERN_HEIGHT,//16
	BOT_ROTATION_SPEED,//17
	BOT_XSPREAD,//18
	BOT_YSPREAD,//19
};

enum EquipmentData
{
	EQUIPMENT_COMPARE,
	EQUIPMENT_NAME,
	EQUIPMENT_DISPLAY_NAME,
	EQUIPMENT_IMAGE_NAME,
	EQUIPMENT_WIDTH,
	EQUIPMENT_HEIGHT,
	EQUIPMENT_FRAMES,
	EQUIPMENT_MAXSPEED,
	EQUIPMENT_DECCELERATION,
	EQUIPMENT_ACCELERATION,
	EQUIPMENT_HYPERSPEED,
	EQUIPMENT_HYPERSPEED_FUEL,
	EQUIPMENT_ARMOR,
	EQUIPMENT_SHIELD,
	EQUIPMENT_SHIELD_REGEN,
	EQUIPMENT_SHIELD_RECOVERY,
	EQUIPMENT_DAMAGE,
	EQUIPMENT_BOT,
	EQUIPMENT_FAKE_TEXTURE,
	EQUIPMENT_FAKE_WIDTH,
	EQUIPMENT_FAKE_HEIGHT,
	EQUIPMENT_FAKE_FRAMES,
};

enum UpgradeData
{
	UPGRADE_NAME,//0
	UPGRADE_CATEGORY,//1
	UPGRADE_PERMANENT,//2
	UPGRADE_PRICE,//3
	UPGRADE_UNLOCKED_BY,//4
	UPGRADE_FORBIDS_1,//5
	UPGRADE_FORBIDS_2,//6
	UPGRADE_IMAGE,//7
	UPGRADE_DISPLAY_NAME,//8
	UPGRADE_DESCRIPTION,//9
	UPGRADE_CONDITION,//10
	UPGRADE_CONDITION_OP,//11
	UPGRADE_CONDITION_VALUE,//12
};

enum GameObjectType {
	BackgroundObject,
	PortalObject,
	ShopObject,
	LootObject,
	PlayerShip,
	FakePlayerShip,
	FriendlyFire,
	EnemyFire,
	EnemyObject,
	NBVAL_GameObject
};

enum InteractionType
{
	No_Interaction,
	PortalInteraction,
	ShopInteraction,
};

enum Directions
{
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_RIGHT,
	DIRECTION_LEFT,
	NO_DIRECTION,
};

enum FloatCompare
{
	LESSER_THAN,
	EQUAL_TO,
	GREATER_THAN,
	ERROR_COMPARE,
};

enum EnemyClass
{
	ENEMYPOOL_VOID,//0
	ENEMYPOOL_ALPHA,//1
	ENEMYPOOL_SIGMA,//2
	ENEMYPOOL_BETA,//3
	ENEMYPOOL_GAMMA,//4
	ENEMYPOOL_DELTA,//5
	ENEMYPOOL_KAPPA,//6
	ENEMYPOOL_OMEGA,//7
	ENEMYPOOL_ZETA,//8
	NBVAL_EnemyClass,//9
};

enum EquipmentType {
	//Airbrake,//0
	Engine,//
	Armor,//1
	Shield,//2
	Module,//3
	NBVAL_Equipment//4
};

enum EquipmentQuality
{
	ItemQuality_Poor,
	ItemQuality_Medium,
	ItemQuality_Good,
	ItemQuality_Epic,
	NBVAL_ItemQuality,
};

class GameObject : public AnimatedSprite
{
public:
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	GameObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	GameObject(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	GameObject();

	~GameObject();

	virtual void update(sf::Time deltaTime, float hyperspeedMultiplier = 1);
	virtual void updateAnimation(sf::Time deltaTime);
	virtual void updatePostCollision();
	virtual void Draw(sf::RenderTexture& screen);
	void GarbageWhenOutOfScreen();

	bool m_visible;
	bool m_isOnScene;
	bool m_garbageMe;
	bool m_DontGarbageMe;
	GameObjectType m_collider_type;
	LayerType m_layer;
	sf::Vector2f m_size;

	float m_damage_feedbackTimer;

	virtual void GetDamageFrom(GameObject& object);
	virtual void GetDamage(int damage);
	string getName();
	GameObject* Clone();
	virtual void Death();
	virtual void GenerateLoot();
	void ShieldRegen(sf::Time deltaTime, float hyperspeedMultiplier);

	void addMoney(int loot_value);
	void setMoney(int loot_value);
	bool get_money_from(GameObject& object);
	bool get_money_from(GameObject& object, int loot_value);
	virtual bool GetLoot(GameObject& object);
	virtual void GetPortal(GameObject* object);
	virtual void GetShop(GameObject* object);
	virtual void setGhost(bool ghost);
	void setAnimationLine(int animation_line, bool keep_frame_index = false);

	bool get_equipment_from(GameObject& object);
	bool setEquipmentLoot(Equipment* equipment);
	Equipment* getEquipmentLoot();

	bool get_weapon_from(GameObject& object);
	bool setWeaponLoot(Weapon* weapon);
	Weapon* getWeaponLoot();
	
	static int GetPrice(int credits, float quality);
	
	bool clearLoots();
	virtual void AddComboCount(int value);

	InteractionType m_isCollidingWithInteractiveObject;
	InteractionType m_previouslyCollidingWithInteractiveObject;

	virtual void GetGrazing(sf::Time deltaTime, float hyperspeedMultiplier);
	virtual GameObject* GetFakeShip();

	int m_damage;
	int m_armor;
	int m_armor_max;
	int m_shield;
	int m_shield_max;
	int m_shield_regen;
	float m_shield_recovery_time;
	sf::Vector2f m_speed;
	GeometryPattern m_pattern;
	float m_diag;
	std::string m_display_name;
	bool m_transparent;
	bool m_ghost;
	bool m_disable_fire;
	bool m_wake_up;
	float m_rotation_speed;
	int m_animationNumber;
	int m_frameNumber;
	int m_currentAnimationIndex;
	float m_collision_timer;

	//TIPS:
	// direction = the scene border you refer too
	// offset = the (positive) distance to that border
	// outside_screen = true if the sprite is on the outside (example: if direction is UP, if the sprite is northern than the border)
	// player_side = true if the sprite is a playership

	static sf::Vector2f getSpeed_to_LocationWhileSceneSwap(Directions current_direction, Directions future_direction, float vspeed, sf::Vector2f sprite_position, float sprite_sizeY);

	static sf::Vector2i getDirectionMultiplier(Directions direction);
	static sf::Vector2f getSize_for_Direction(Directions direction, sf::Vector2f size);
	static sf::Vector2i getSize_for_Direction(Directions direction, sf::Vector2i size);
	static sf::Vector2f getSpeed_for_Scrolling(Directions direction, float vspeed);
	static sf::Vector2f getSpeed_for_Direction(Directions direction, sf::Vector2f speed);
	static float getRotation_for_Direction(Directions direction);
	static sf::Vector2f getPosition_for_Direction(Directions direction, sf::Vector2f position, bool rescale = true);
	static float GetDistanceBetweenObjects(GameObject* object1, GameObject* object2);
	static float GetDistanceBetweenPositions(sf::Vector2f position1, sf::Vector2f position2);
	static float GetVectorLength(sf::Vector2f vector);
	static float GetAngleRadForVector(sf::Vector2f vector);
	static float GetAngleRadBetweenObjects(GameObject* ref_object, GameObject* object2);
	static float GetAngleRadBetweenPositions(sf::Vector2f ref_position, sf::Vector2f position2);
	static float GetAngleDegToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position);
	static float GetDeltaAngleToTargetAngle(float ref_rotation_in_deg, float target_rotation_in_deg);
	sf::Vector2f GetVectorFromLengthAndAngle(const float length, const float angle);
	bool TurnToTargetAngle(float target_angle, float rotation_speed, sf::Time deltaTime, float hyperspeedMultiplier);//in degress

	FloatCompare compare_posY_withTarget_for_Direction(Directions direction, sf::Vector2f target_position);
	FloatCompare compare_posX_withTarget_for_Direction(Directions direction, sf::Vector2f target_position);

	sf::Vector2f getRandomXSpawnPosition(sf::Vector2f max_enemy_size, sf::Vector2f cluster_size = sf::Vector2f(0, 0));
	sf::Vector2f setPosition_Y_for_Direction(Directions direction, sf::Vector2f target_position, bool centered = false);

	static sf::Vector2f ApplyScreenBordersConstraints(Directions direction, sf::Vector2f position, sf::Vector2f size);

	//utilitary methods
	static bool NormalizeVector(sf::Vector2f* vector, float max_value);
	static void ScaleVector(sf::Vector2f* vector, float target_value);

	int m_money;
	Equipment* m_equipment_loot;
	Weapon* m_weapon_loot;
	std::string m_textureName;
	bool m_immune;

	virtual float getFighterFloatStatValue(FighterStats stat);
	virtual int getFighterIntStatValue(FighterStats stat);
	virtual bool CheckCondition();
	virtual void AddDialog(Dialog* dialog);
	virtual void SetInputBlocker(GameObject* blocker);
	virtual GameObject* GetInputBlocker();
	virtual void SetAskingPanel(SFPanelTypes type);
	virtual bool ClearTargetIfGarbage();

protected:
	sf::Vector2f m_initial_position;
	
	Animation m_defaultAnimation;
	Animation* m_currentAnimation;

	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int m_frameNumber, int m_animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber = 1, int m_animationNumber = 1);
};

#endif // INDEPENDANT_H_INCLUDED