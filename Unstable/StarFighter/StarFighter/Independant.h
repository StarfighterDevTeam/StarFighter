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

enum LayerType {
	BackgroundLayer,
	PortalLayer,
	ExplosionLayer,
	EnemyObjectLayer,
	AuraLayer,
	BotLayer,
	FriendlyFireLayer,
	EnemyFireLayer,
	FakeShipLayer,
	PlayerShipLayer,
	HudObject,
	HudCursor,
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
	WEAPON_TARGET_SEAKING,
};

enum TargetSeaking
{
	NO_SEAKING,
	SEMI_SEAKING,//aim once every rafale
	SEAKING,//aim once every bullt
	SUPER_SEAKING,//bullet follows target
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
	AMMO_ROTATION_SPEED,//13
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

enum IndependantType {
	BackgroundObject,
	PortalObject,
	LootObject,
	PlayerShip,
	FakePlayerShip,
	FriendlyFire,
	Neutral,
	EnemyFire,
	EnemyObject,
	NBVAL_Independant
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
	GREATHER_THAN,
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



class Independant : public AnimatedSprite
{
public:
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber = 1, int m_animationNumber = 1);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	Independant();

	~Independant();

	virtual void update(sf::Time deltaTime, float hyperspeedMultiplier = 1);
	virtual void updateAnimation(sf::Time deltaTime);
	bool visible;
	bool isOnScene;
	bool GarbageMe;
	bool DontGarbageMe;
	IndependantType collider_type;
	LayerType layer;
	virtual void damage_from(Independant& independant);
	sf::Vector2f m_size;
	sf::Vector2f getIndependantSpeed();
	int getIndependantDamage();
	int getIndependantArmor();
	int getIndependantArmorMax();
	int getIndependantShield();
	int getIndependantShieldMax();
	int getIndependantShieldRegen();
	string getName();
	virtual void Respawn();
	Independant* Clone();
	virtual void Death();
	virtual void Destroy();
	virtual void GenerateLoot();

	int getMoney();
	void addMoney(int loot_value);
	void setMoney(int loot_value);
	bool get_money_from(Independant& independant);
	bool get_money_from(Independant& independant, int loot_value);
	virtual bool GetLoot(Independant& independant);
	virtual void GetPortal(Independant* independant);
	void setGhost(bool m_ghost);
	void setAnimationLine(int m_animation_line, bool keep_frame_index = false);

	bool get_equipment_from(Independant& independant);
	bool setEquipmentLoot(Equipment* equipment);
	void releaseEquipmentLoot();
	Equipment* getEquipmentLoot();

	bool get_weapon_from(Independant& independant);
	bool setWeaponLoot(Weapon* weapon);
	void releaseWeaponLoot();
	Weapon* getWeaponLoot();
	virtual void CreateRandomLoot(float BeastScaleBonus = 0);

	virtual void GetGrazing();
	void GetPolarMovement(sf::Vector2f* np);
	int damage;
	int armor;
	int armor_max;
	int shield;
	int shield_max;
	int shield_regen;
	sf::Vector2f speed;
	PatternBobby Pattern;
	float diag;
	std::string display_name;
	bool transparent;
	bool ghost;
	bool disable_fire;
	bool wake_up;
	float rotation_speed;
	int animationNumber;
	int frameNumber;
	int currentAnimationIndex;
	bool isUsingPortal;

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

	FloatCompare compare_posY_withTarget_for_Direction(Directions direction, sf::Vector2f target_position);
	FloatCompare compare_posX_withTarget_for_Direction(Directions direction, sf::Vector2f target_position);

	sf::Vector2f getRandomXSpawnPosition(Directions direction, sf::Vector2f max_enemy_size, sf::Vector2f cluster_size = sf::Vector2f(0, 0));
	sf::Vector2f setPosition_Y_for_Direction(Directions direction, sf::Vector2f target_position, bool centered = false);

	int money;
	Equipment* equipment_loot;
	Weapon* weapon_loot;
	std::string textureName;
	bool immune;
protected:
	sf::Vector2f initial_position;
	bool startPattern;
	
	Animation defaultAnimation;
	Animation* currentAnimation;

	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int m_frameNumber, int m_animationNumber = 1);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber = 1, int m_animationNumber = 1);
	
	sf::Clock immunityTimer;
	
};

#endif // INDEPENDANT_H_INCLUDED