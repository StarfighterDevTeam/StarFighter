#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;


// ----------------SHIP MODEL ---------------
ShipModel::ShipModel()
{
	this->max_speed.x = SHIP_MAX_SPEED_X;
	this->max_speed.y = SHIP_MAX_SPEED_Y;
	this->decceleration = SHIP_DECCELERATION_COEF;
	this->acceleration.x = SHIP_ACCELERATION_X;
	this->acceleration.y = SHIP_ACCELERATION_Y;
	this->armor = SHIP_ARMOR;
	this->shield = SHIP_SHIELD;
	this->shield_regen = SHIP_SHIELD_REGEN;
}

//various "get" functions to enter private members of ShipModel, Equipment, and ShipConfig
sf::Vector2f ShipModel::getShipModelMaxSpeed()
{
	return this->max_speed;
}

float ShipModel::getShipModelDecceleration()
{
	return this->decceleration;
}

sf::Vector2f ShipModel::getShipModelAcceleration()
{
	return this->acceleration;
}

int ShipModel::getShipModelArmor()
{
	return this->armor;
}

int ShipModel::getShipModelShield()
{
	return this->shield;
}

int ShipModel::getShipModelShieldRegen()
{
	return this->shield_regen;
}

// ----------------EQUIPMENT ---------------

Equipment::Equipment()
{
	this->max_speed.x = 0.0f;
	this->max_speed.y = 0.0f;
	this->decceleration = 0.0f;
	this->acceleration.x = 0.0f;
	this->acceleration.y = 0.0f;
	this->armor = 0;
	this->shield = 0;
	this->shield_regen = 0;
	this->size.x = SLOT_WIDTH;
	this->size.y = SLOT_HEIGHT;
	this->textureName = EMPTYSLOT_FILENAME;
	this->equipmentType = EquipmentType::Empty;
}

void Equipment::Init(EquipmentType m_equipmentType, sf::Vector2f m_max_speed, float m_decceleration , sf::Vector2f m_acceleration, int m_armor, int m_shield, int m_shield_regen, std::string m_textureName, sf::Vector2f m_size)
{
	this->max_speed.x = m_max_speed.x;
	this->max_speed.y = m_max_speed.y;
	this->decceleration = m_decceleration;
	this->acceleration.x = m_acceleration.x;
	this->acceleration.y = m_acceleration.y;
	this->armor = m_armor;
	this->shield = m_shield;
	this->shield_regen = m_shield_regen;
	this->size.x = m_size.x;
	this->size.y = m_size.y;
	this->textureName = m_textureName;
	this->equipmentType = m_equipmentType;
};

sf::Vector2f Equipment::getEquipmentMaxSpeed()
{
	return this->max_speed;
}

float Equipment::getEquipmentDecceleration()
{
	return this->decceleration;
}

sf::Vector2f Equipment::getEquipmentAcceleration()
{
	return this->acceleration;
}

int Equipment::getEquipmentArmor()
{
	return this->armor;
}

int Equipment::getEquipmentShield()
{
	return this->shield;
}

int Equipment::getEquipmentShieldRegen()
{
	return this->shield_regen;
}

// ----------------SHIP CONFIG ---------------

ShipConfig::ShipConfig()
{

}

void ShipConfig::Init(std::string m_textureName, sf::Vector2f m_size, int m_frameNumber)
{
	this->max_speed = getShipConfigMaxSpeed();
	this->decceleration = getShipConfigDecceleration();
	this->acceleration = getShipConfigAcceleration();
	this->armor = getShipConfigArmor();
	this->shield = getShipConfigShield();
	this->shield_regen = getShipConfigShieldRegen();
	this->size.x = m_size.x;
	this->size.y = m_size.y;
	this->textureName = m_textureName;
	this->frameNumber = m_frameNumber;

	Ammo* player_ammo;
	player_ammo = new Ammo(Vector2f(0,0), Vector2f(0,-500), LASERBLUE_FILENAME, Vector2f(LASERBLUE_WIDTH,LASERBLUE_HEIGHT), 150, new FX(Vector2f(0,0), Vector2f(0,0), FX_EXPLOSION_FILENAME, Vector2f(FX_EXPLOSION_WIDTH,FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(FX_LITTLE_EXPLOSION_DURATION)));
	weapon = new Weapon(player_ammo);
	weapon->rate_of_fire = 0.2f;
	weapon->fire_direction = sf::Vector2i(0, 1);

}

void ShipConfig::setEquipment(Equipment* m_equipment)
{
	// TODO : pas exact, on peut avoir des types d'armes identiques dans différents slots... mais bon pour l'instant.
	this->equipment[m_equipment->equipmentType] = m_equipment;
	this->max_speed = getShipConfigMaxSpeed();
	this->decceleration = getShipConfigDecceleration();
	this->acceleration = getShipConfigAcceleration();
	this->armor = getShipConfigArmor();
	this->shield = getShipConfigShield();
	this->shield_regen = getShipConfigShieldRegen();
}

int ShipConfig::getShipConfigArmor()
{
	int new_armor = 0;
	int equipment_armor = 0;

	for (int i=0; i<NBVAL_EQUIPMENT; i++)
	{
		equipment_armor += equipment[i]->getEquipmentArmor();
	}

	new_armor = ship_model->getShipModelArmor() + equipment_armor;
	return new_armor;
}

int ShipConfig::getShipConfigShield()
{
	int new_shield = 0;
	int equipment_shield = 0.;

	for (int i=0; i<NBVAL_EQUIPMENT; i++)
	{
		equipment_shield += equipment[i]->getEquipmentShield();
	}

	new_shield = ship_model->getShipModelShield() + equipment_shield;
	return new_shield;
}

int ShipConfig::getShipConfigShieldRegen()
{
	int new_shield_regen = 0;
	int equipment_shield_regen = 0;

	for (int i=0; i<NBVAL_EQUIPMENT; i++)
	{
		equipment_shield_regen += equipment[i]->getEquipmentShieldRegen();
	}

	new_shield_regen = ship_model->getShipModelShieldRegen() + equipment_shield_regen;
	return new_shield_regen;
}

sf::Vector2f ShipConfig::getShipConfigMaxSpeed()
{
	sf::Vector2f new_max_speed = sf::Vector2f(0,0);
	sf::Vector2f equipment_max_speed = sf::Vector2f(0,0);

	for (int i=0; i<NBVAL_EQUIPMENT-1; i++)
	{
		equipment_max_speed.x += equipment[i]->getEquipmentMaxSpeed().x;
		equipment_max_speed.y += equipment[i]->getEquipmentMaxSpeed().y;
	}

	new_max_speed.x = ship_model->getShipModelMaxSpeed().x + equipment_max_speed.x;
	new_max_speed.y = ship_model->getShipModelMaxSpeed().y + equipment_max_speed.y;

	return new_max_speed;
}

float ShipConfig::getShipConfigDecceleration()
{
	float new_decceleration = 0.0f;
	float equipment_decceleration = 0.0f;

	for (int i=0; i<NBVAL_EQUIPMENT-1; i++)
	{
		equipment_decceleration += equipment[i]->getEquipmentDecceleration();
	}

	new_decceleration = ship_model->getShipModelDecceleration() + equipment_decceleration;
	return new_decceleration;
}

sf::Vector2f ShipConfig::getShipConfigAcceleration()
{
	sf::Vector2f new_acceleration = sf::Vector2f(0,0);
	sf::Vector2f equipment_acceleration = sf::Vector2f(0,0);

	for (int i=0; i<NBVAL_EQUIPMENT-1; i++)
	{
		equipment_acceleration.x += equipment[i]->getEquipmentAcceleration().x;
		equipment_acceleration.y += equipment[i]->getEquipmentAcceleration().y;
	}

	new_acceleration.x = ship_model->getShipModelAcceleration().x + equipment_acceleration.x;
	new_acceleration.y = ship_model->getShipModelAcceleration().y + equipment_acceleration.y;
	return new_acceleration;
}

// ----------------SHIP ---------------

Ship::Ship(Vector2f position, ShipConfig m_ship_config) : Independant(position, Vector2f(0,0), m_ship_config.textureName, Vector2f(m_ship_config.size.x, m_ship_config.size.y), Vector2f((m_ship_config.size.x/2),(m_ship_config.size.y/2)), m_ship_config.frameNumber)
{
	this->collider_type =  IndependantType::PlayerShip;
	this->ship_config = m_ship_config;
	moving = false;	
	this->visible = true;
	this->damage = 0;
	this->armor = 1;
	this->shield = 1;
	this->armor = ship_config.getShipConfigArmor();
	this->shield = ship_config.getShipConfigShield();
	this->shield_regen = ship_config.getShipConfigShieldRegen();
	ship_hud.Init(this->ship_config.getShipConfigArmor(), this->ship_config.getShipConfigShield());
}

void Ship::setShipConfig(ShipConfig m_ship_config)
{
	this->ship_config = m_ship_config;
}

void Ship::update(sf::Time deltaTime, sf::Vector2f polarOffset[MovePatternType::NBVAL_MovePattern])
{
	Ship::update(deltaTime);
}
void Ship::update(sf::Time deltaTime)
{
	static double shield_regen_buffer = 0;
	//immunity frames after death
	if (immune)
	{
		if (immunityTimer.getElapsedTime() > sf::seconds(2))
		{
			immune = false;
		}
	}

	//sheld regen if not maximum
	if (shield < ship_config.getShipConfigShield())
	{
		shield_regen_buffer += shield_regen*deltaTime.asSeconds();
		if(shield_regen_buffer > 1)
		{
			double intpart;
			shield_regen_buffer = modf (shield_regen_buffer , &intpart);
			shield += intpart;
		}

		//canceling over-regen
		if (shield > ship_config.getShipConfigShield())
		{
			shield = ship_config.getShipConfigShield();
		}
	}
	this->ship_hud.update(armor/3, shield/3);//will do for now... but we'll need to scale it to the max value later

	sf::Vector2f directions = InputGuy::getDirections();
	moving = directions.x !=0 || directions.y !=0;
	speed.x += directions.x*ship_config.getShipConfigAcceleration().x;
	speed.y += directions.y*ship_config.getShipConfigAcceleration().y;

	if(InputGuy::isFiring())
	{
		ship_config.weapon->setPosition(this->getPosition().x, (this->getPosition().y - (ship_config.size.y/2)) );
		ship_config.weapon->Fire(FriendlyFire);
	}

	//max speed constraints
	if(abs(speed.x) > this->ship_config.getShipConfigMaxSpeed().x)
	{
		speed.x = speed.x > 0 ?  this->ship_config.getShipConfigMaxSpeed().x : - this->ship_config.getShipConfigMaxSpeed().x;
	}
	if(abs(speed.y) >  this->ship_config.getShipConfigMaxSpeed().y)
	{
		speed.y = speed.y > 0 ?  this->ship_config.getShipConfigMaxSpeed().y : - this->ship_config.getShipConfigMaxSpeed().y;
	}

	//screen borders contraints	
	if (this->getPosition().x < ship_config.size.x/2)
	{
		this->setPosition(ship_config.size.x/2, this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().x > WINDOW_RESOLUTION_X - (ship_config.size.x/2))
	{
		this->setPosition(WINDOW_RESOLUTION_X-(ship_config.size.x/2), this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().y < ship_config.size.y/2)
	{
		this->setPosition(this->getPosition().x, ship_config.size.y/2);
		speed.y = 0;
	}

	if (this->getPosition().y > WINDOW_RESOLUTION_Y-(ship_config.size.y/2))
	{
		this->setPosition(this->getPosition().x, WINDOW_RESOLUTION_Y-(ship_config.size.y/2));
		speed.y = 0;
	}

	//moving stuff
	//this->setPosition(this->getPosition().x + (speed.x)*deltaTime.asSeconds(), this->getPosition().y + (speed.y)*deltaTime.asSeconds());

	//idle decceleration
	if(!moving)
	{
		speed.x -= (speed.x)*deltaTime.asSeconds()*(ship_config.getShipConfigDecceleration()/100);
		speed.y -= (speed.y)*deltaTime.asSeconds()*(ship_config.getShipConfigDecceleration()/100);

		if(abs(speed.x) < SHIP_MIN_SPEED_X)
			speed.x = 0;

		if(abs(speed.y) < SHIP_MIN_SPEED_Y)
			speed.y = 0;
	}

	Independant::update(deltaTime);
}

void Ship::Respawn()
{
	armor = ship_config.getShipConfigArmor();
	shield = ship_config.getShipConfigShield();
	speed.x = 0;
	speed.y = 0;
	this->setVisible(true);
	isOnScene = true;
	this->setPosition(400, 500);

	immune = true;
	immunityTimer.restart();
}