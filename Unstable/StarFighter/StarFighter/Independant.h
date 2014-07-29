#ifndef INDEPENDANT_H_INCLUDED
#define INDEPENDANT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "AnimatedSprite.hpp"
#include "TextUtils.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "MovePattern.h"
#include "PatternBobby.h"
#include "Globals.h"

class Equipment;
class Weapon;

enum IndependantType {
	Background,
	LootObject,
	PlayerShip,
	FriendlyFire,
	Neutral,
	EnemyFire,
	EnemyObject,
	NBVAL_Independant
};

class Independant : public AnimatedSprite
{
public:
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	~Independant();

	virtual void update(sf::Time deltaTime);
	virtual void updateAnimation(sf::Time deltaTime);
	bool visible;
	bool isOnScene;
	bool GarbageMe;
	bool DontGarbageMe;
	IndependantType collider_type;
	void setVisible(bool visible);
	virtual void damage_from (Independant& independant);
	sf::Vector2f m_size;
	sf::Vector2f getIndependantSpeed();
	int getIndependantDamage();
	int getIndependantArmor();
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
	void get_money_from (Independant& independant);
	void get_money_from (Independant& independant, int loot_value);
	virtual void GetLoot (Independant& independant);

	bool hasEquipmentLoot;
	void get_equipment_from (Independant& independant);
	void setEquipmentLoot (Equipment* equipment);
	void releaseEquipmentLoot();
	Equipment* getEquipmentLoot();

	bool hasWeaponLoot;
	void get_weapon_from (Independant& independant);
	void setWeaponLoot (Weapon* weapon);
	void releaseWeaponLoot();
	Weapon* getWeaponLoot();

	void Independant::GetPolarMovement(sf::Vector2f* np);
	//sf::Vector2f setMovePattern(float angle_rad, float radius, float triggerY, int pattern_id=MovePatternType::NoMove);
	int damage;
	int armor;
	int shield;
	int shield_max;
	int shield_regen;
	sf::Vector2f speed;
	PatternBobby Pattern;
	float diag;
	std::string display_name;

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