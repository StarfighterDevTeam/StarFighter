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

enum WeaponData
{
	WEAPON_NAME,//0
	WEAPON_RATE_OF_FIRE,//1
	WEAPON_IMAGE_NAME,//2
	WEAPON_WIDTH,//3
	WEAPON_HEIGHT,//4
	WEAPON_FRAMES,//5
	WEAPON_SOUND,//6
	WEAPON_MULTISHOT,//7
	WEAPON_XSPREAD,//8
	WEAPON_ALTERNATE,//9
	WEAPON_DISPERSION,//10
	WEAPON_RAFALE,//11
	WEAPON_RAFALE_COOLDOWN,//12
	WEAPON_TARGET_SEAKING,//13
};

enum TargetSeaking
{
	NO_SEAKING,
	SEMI_SEAKING,//aim once every rafale
	SEAKING,//aim once every bullt
	SUPER_SEAKING,//bullet follows target
};

enum AmmoData
{
	AMMO_NAME,//0
	AMMO_DAMAGE,//1
	AMMO_SPEED,//2
	AMMO_IMAGE_NAME,//3
	AMMO_WIDTH,//4
	AMMO_HEIGHT,//5
	AMMO_FRAMES,//6
	AMMO_FX,//7
	AMMO_PATTERN,//8
	AMMO_ANGSPEED,//9
	AMMO_RADIUS,//10
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
	Background,
	LinkZone,
	LootObject,
	PlayerShip,
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
	ENEMYPOOL_BETA,//2
	ENEMYPOOL_DELTA,//3
	ENEMYPOOL_GAMMA,//4
	ENEMYPOOL_ZETA,//5
	NBVAL_EnemyClass,
};

enum EquipmentType {
	Airbrake,//0
	Engine,//1
	Armor,//2
	Shield,//3
	Module,//4
	NBVAL_EQUIPMENT//5
};



class Independant : public AnimatedSprite
{
public:
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	Independant();

	~Independant();

	virtual void update(sf::Time deltaTime);
	virtual void updateAnimation(sf::Time deltaTime);
	bool visible;
	bool isOnScene;
	bool GarbageMe;
	bool DontGarbageMe;
	IndependantType collider_type;
	void setVisible(bool visible);
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
	virtual void GenerateLoot();

	int getMoney();
	void addMoney(int loot_value);
	void setMoney(int loot_value);
	void get_money_from(Independant& independant);
	void get_money_from(Independant& independant, int loot_value);
	virtual void GetLoot(Independant& independant);
	void setGhost(bool m_ghost);

	bool hasEquipmentLoot;
	void get_equipment_from(Independant& independant);
	void setEquipmentLoot(Equipment* equipment);
	void releaseEquipmentLoot();
	Equipment* getEquipmentLoot();

	bool hasWeaponLoot;
	void get_weapon_from(Independant& independant);
	void setWeaponLoot(Weapon* weapon);
	void releaseWeaponLoot();
	Weapon* getWeaponLoot();
	virtual void CreateRandomLoot(float BeastScaleBonus = 0);

	virtual void GetGrazing();
	void Independant::GetPolarMovement(sf::Vector2f* np);
	//sf::Vector2f setMovePattern(float angle_rad, float radius, float triggerY, int pattern_id=MovePatternType::NoMove);
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
	
	static sf::Vector2f getPosition_for_Direction(Directions direction, sf::Vector2f position, bool rescale = true);
	FloatCompare compare_posY_withTarget_for_Direction(Directions direction, sf::Vector2f target_position);
	FloatCompare Independant::compare_posX_withTarget_for_Direction(Directions direction, sf::Vector2f target_position);
	void setPosition_Y_for_Direction(Directions direction, sf::Vector2f target_position, bool centered = false);

	static float Independant::getRotation_for_Direction(Directions direction);

protected:
	sf::Vector2f initial_position;
	bool startPattern;
	std::string textureName;
	Animation defaultAnimation;
	Animation* currentAnimation;
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber = 1);

	bool immune;
	sf::Clock immunityTimer;
	int money;
	Equipment* equipment_loot;
	Weapon* weapon_loot;

};

#endif // INDEPENDANT_H_INCLUDED