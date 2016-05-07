#ifndef INDEPENDANT_H_INCLUDED
#define INDEPENDANT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "AnimatedSprite.hpp"
#include "TextUtils.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "PatternBobby.h"
#include "Globals.h"

using namespace std;
using namespace sf;

class Equipment;
class Weapon;
class Loot;

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
	WEAPON_SOUND,
	WEAPON_RATE_OF_FIRE,
	WEAPON_RAFALE,
	WEAPON_RAFALE_COOLDOWN,
	WEAPON_MULTISHOT,
	WEAPON_XSPREAD,
	WEAPON_DISPERSION,
	WEAPON_ALTERNATE,
	WEAPON_ANGLE_OFFSET,
	WEAPON_TARGET_HOMING,
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
	AMMO_DAMAGE,//1
	AMMO_SPEED,//2
	AMMO_RANGE,//3
	AMMO_IMAGE_NAME,//4
	AMMO_WIDTH,//5
	AMMO_HEIGHT,//6
	AMMO_FRAMES,//7
	AMMO_FX,//8
	AMMO_PATTERN,//9
	AMMO_ANGSPEED,//10
	AMMO_RADIUS,//11
	AMMO_CLOCKWISE,//12
	AMMO_CENTERED,//13
	AMMO_ROTATION_SPEED,//14
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

enum GameObjectType {
	BackgroundObject,
	PortalObject,
	ShopObject,
	LootObject,
	PlayerShip,
	FakePlayerShip,
	FriendlyFire,
	Neutral,
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

	bool m_visible;
	bool m_isOnScene;
	bool m_GarbageMe;
	bool m_DontGarbageMe;
	GameObjectType m_collider_type;
	LayerType m_layer;
	sf::Vector2f m_size;

	virtual void GetDamageFrom(GameObject& object);
	virtual void GetDamage(int damage);
	string getName();
	virtual void Respawn();
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

	InteractionType m_isCollidingWithInteractiveObject;
	InteractionType m_previouslyCollidingWithInteractiveObject;

	virtual void GetGrazing(sf::Time deltaTime, float hyperspeedMultiplier);
	void GetPolarMovement(sf::Vector2f* np);

	int m_damage;
	int m_armor;
	int m_armor_max;
	int m_shield;
	int m_shield_max;
	int m_shield_regen;
	float m_shield_recovery_time;
	sf::Vector2f m_speed;
	PatternBobby m_Pattern;
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

	//TIPS:
	// direction = the scene border you refer too
	// offset = the (positive) distance to that border
	// outside_screen = true if the sprite is on the outside (example: if direction is UP, if the sprite is northern than the border)
	// player_side = true if the sprite is a playership

	static sf::Vector2f getSpeed_to_LocationWhileSceneSwap(Directions current_direction, Directions future_direction, float vspeed, sf::Vector2f sprite_position);

	static sf::Vector2i getDirectionMultiplier(Directions direction);
	static sf::Vector2f getSize_for_Direction(Directions direction, sf::Vector2f size);
	static sf::Vector2i getSize_for_Direction(Directions direction, sf::Vector2i size);
	static sf::Vector2f getSpeed_for_Scrolling(Directions direction, float vspeed);
	static sf::Vector2f getSpeed_for_Direction(Directions direction, sf::Vector2f speed);
	static float getRotation_for_Direction(Directions direction);
	static sf::Vector2f getPosition_for_Direction(Directions direction, sf::Vector2f position, bool rescale = true);
	static float GetDistanceBetweenObjects(GameObject* object1, GameObject* object2);
	static float GetAbsoluteSpeed(sf::Vector2f speed_);

	FloatCompare compare_posY_withTarget_for_Direction(Directions direction, sf::Vector2f target_position);
	FloatCompare compare_posX_withTarget_for_Direction(Directions direction, sf::Vector2f target_position);

	sf::Vector2f getRandomXSpawnPosition(Directions direction, sf::Vector2f max_enemy_size, sf::Vector2f cluster_size = sf::Vector2f(0, 0));
	sf::Vector2f setPosition_Y_for_Direction(Directions direction, sf::Vector2f target_position, bool centered = false);

	static sf::Vector2f ApplyScreenBordersConstraints(Directions direction, sf::Vector2f position, sf::Vector2f size);

	//utilitary methods
	static bool NormalizeSpeed(sf::Vector2f* vector, float max_value);
	static void ScaleSpeed(sf::Vector2f* vector, float target_value);

	int m_money;
	Equipment* m_equipment_loot;
	Weapon* m_weapon_loot;
	std::string m_textureName;
	bool m_immune;

	virtual float getFighterFloatStatValue(FighterStats stat);
	virtual int getFighterIntStatValue(FighterStats stat);

protected:
	sf::Vector2f m_initial_position;
	bool m_startPattern;
	sf::Clock m_immunityTimer;
	
	Animation m_defaultAnimation;
	Animation* m_currentAnimation;

	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int m_frameNumber, int m_animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber = 1, int m_animationNumber = 1);
};

#endif // INDEPENDANT_H_INCLUDED