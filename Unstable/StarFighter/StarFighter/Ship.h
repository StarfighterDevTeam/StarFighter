#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "AnimatedSprite.hpp"
#include "TextureLoader.h"
#include "Assets.h"
#include "Hud.h"
#include "Independant.h"
#include <cmath>

#define SHIP_START_X                0
#define SHIP_START_Y                0
#define SHIP_ACCELERATION_X         30.0f
#define SHIP_ACCELERATION_Y         30.0f
#define SHIP_DECCELERATION_COEF		1000.0f
#define SHIP_MAX_SPEED_X            500.0f
#define SHIP_MAX_SPEED_Y            500.0f
#define SHIP_MIN_SPEED_X            50.0f
#define SHIP_MIN_SPEED_Y            50.0f
#define SHIP_ARMOR					600
#define SHIP_SHIELD					1000
#define SHIP_SHIELD_REGEN			1

#define SHIP_SPRITE_RATE_SEC        0.2f

enum EquipmentType {
	Empty,
	Airbrake,
	Engine,
	Stabs,
	Armor,
	Shield,
	NBVAL_EQUIPMENT
};

class ShipModel
{
public:
	ShipModel();
	sf::Vector2f getShipModelMaxSpeed();
	float getShipModelDecceleration();
	int getShipModelArmor();
	int getShipModelShield();
	int getShipModelShieldRegen();
private:
	float decceleration;
	sf::Vector2f max_speed;
	int armor;
	int shield;
	int shield_regen;
};

class Equipment
{
public:
	void Init(EquipmentType m_equipmentType, sf::Vector2f m_max_speed, float m_decceleration , int m_armor, int m_shield, int m_shield_regen, std::string m_textureName, sf::Vector2f m_size = sf::Vector2f(256,256));
	Equipment();
	std::string textureName;
	sf::Vector2f size;
	EquipmentType equipmentType;
	sf::Vector2f getEquipmentMaxSpeed();
	float getEquipmentDecceleration();
	int getEquipmentArmor();
	int getEquipmentShield();
	int getEquipmentShieldRegen();
private:
	float decceleration;
	sf::Vector2f max_speed;
	int armor;
	int shield;
	int shield_regen;
};

class ShipConfig
{
public:
	void Init(sf::Vector2f m_max_speed, float decceleration , std::string m_textureName, sf::Vector2f m_size, int m_frameNumber);
	ShipConfig();
	std::string textureName;
	sf::Vector2f size;
	
	int frameNumber;
	void setEquipment(Equipment* m_equipment);
	sf::Vector2f getShipConfigMaxSpeed();
	float getShipConfigDecceleration();
	int getShipConfigArmor();
	int getShipConfigShield();
	int getShipConfigShieldRegen();
	Equipment* equipment[NBVAL_EQUIPMENT];
	ShipModel* ship_model;
	Weapon* weapon;
private:
	sf::Vector2f max_speed;
	float decceleration;
	int armor;
	int shield;
	int shield_regen;
};

class Ship : public Independant
{
public :
	Ship(Vector2f position, ShipConfig m_ship_config);
	void Init(int x, int y);
	void update(sf::Time deltaTime) override;
	void setShipConfig(ShipConfig m_ship_config);
	ShipConfig ship_config;
	PlayerHud ship_hud;
	void Respawn() override;

private:
	bool moving;
};


#endif // SHIP_H_INCLUDED
