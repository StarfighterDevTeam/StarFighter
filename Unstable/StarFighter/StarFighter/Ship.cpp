#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;


// ----------------SHIP MODEL ---------------
ShipModel::ShipModel(float m_max_speed, float m_acceleration, float m_decceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int m_damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name)
{
	this->max_speed.x = m_max_speed;
	this->max_speed.y = m_max_speed;
	this->decceleration = m_decceleration;
	this->acceleration.x = m_acceleration;
	this->acceleration.y = m_acceleration;
	this->armor = m_armor;
	this->shield = m_shield;
	this->shield_regen = m_shield_regen;
	this->damage = m_damage;
	this->textureName = m_textureName;
	this->size = m_size;
	this->frameNumber = m_frameNumber;
	this->display_name = m_display_name;
	this->hasBot = false;
	this->hasFake = false;
	this->hyperspeed = m_hyperspeed;
}

//various "get" functions to enter private members of ShipModel, Equipment, and ShipConfig
sf::Vector2f ShipModel::getShipModelMaxSpeed()
{
	return this->max_speed;
}

sf::Vector2f ShipModel::getShipModelAcceleration()
{
	return this->acceleration;
}

float ShipModel::getShipModelDecceleration()
{
	return this->decceleration;
}

float ShipModel::getShipModelHyperspeed()
{
	return this->hyperspeed;
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

int ShipModel::getShipModelDamage()
{
	return this->damage;
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
	this->damage = 0;
	this->size.x = EQUIPMENT_SIZE;
	this->size.y = EQUIPMENT_SIZE;
	this->textureName = EMPTYSLOT_FILENAME;
	this->frameNumber = 0;
	this->equipmentType = EquipmentType::Armor;
	this->hasBot = false;
	this->hasFake = false;
}

Equipment::~Equipment()
{
	if (this->hasBot)
	{
		this->bot->~Bot();
		this->hasBot = false;
	}
}

Equipment* Equipment::Clone()
{
	Equipment* new_equipment = new Equipment();
	new_equipment->Init(this->equipmentType, this->max_speed.x, this->acceleration.x, this->decceleration, this->hyperspeed, this->armor, this->shield, this->shield_regen, this->damage, this->textureName, this->size, this->frameNumber, this->display_name);
	new_equipment->display_name = this->display_name;
	new_equipment->hasBot = this->hasBot;
	if (this->hasBot)
	{
		new_equipment->bot = this->bot->Clone();
	}

	return new_equipment;
}

void Equipment::Init(int m_equipmentType, float  m_max_speed, float m_acceleration, float m_decceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int m_damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name)
{
	this->max_speed.x = m_max_speed;
	this->max_speed.y = m_max_speed;
	this->decceleration = m_decceleration;
	this->acceleration.x = m_acceleration;
	this->acceleration.y = m_acceleration;
	this->hyperspeed = m_hyperspeed;
	this->armor = m_armor;
	this->shield = m_shield;
	this->shield_regen = m_shield_regen;
	this->damage = damage;
	this->size.x = m_size.x;
	this->size.y = m_size.y;
	this->textureName = m_textureName;
	this->frameNumber = m_frameNumber;
	this->display_name = m_display_name;
	this->equipmentType = m_equipmentType;
}

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

float Equipment::getEquipmentHyperspeed()
{
	return this->hyperspeed;
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

int Equipment::getEquipmentDamage()
{
	return this->damage;
}

#define EQUIPMENT_DECCELERATION_MULTIPLIER		10
#define EQUIPMENT_ACCELERATION_MULTIPLIER		10
#define EQUIPMENT_MAXSPEED_MULTIPLIER			10
#define EQUIPMENT_ARMOR_MULTIPLIER				10
#define EQUIPMENT_SHIELD_MULTIPLIER				10
#define EQUIPMENT_SHIELD_REGEN_MULTIPLIER		10
#define EQUIPMENT_DAMAGE_MULTIPLIER				10

void Equipment::AddAirbrakeProperty(int chosen_property, int value, sf::Vector2f BeastScale)
{
	switch (chosen_property) // 1 case
	{
	case 0:
	{
			  float log_multiplier = EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_BONUS * (log(value * EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_X));

			  float e_decceleration = EQUIPMENT_MIN_DECCELLERATION_VALUE * RandomizeFloatBetweenValues(BeastScale);
			  if (log_multiplier > 1)
				  e_decceleration *= log_multiplier;
			  else
				  e_decceleration = ProrataBetweenThreshold(value, sf::Vector2f(0, e_decceleration));

			  this->decceleration += e_decceleration;
			  this->decceleration = floor(this->decceleration);
			  break;
	}

	default:
	{
			   printf("DEBUG: error: trying to add Airbrake property that does not exit.\n<!> Check that the chosen property for this Airbrake match with the existing properties in the AddAibrakeProperty function.\n");
			   break;
	}
	}
}

void Equipment::AddEngineProperty(int chosen_property, int value, sf::Vector2f BeastScale)
{
	switch (chosen_property) // 2 case
	{
	case 0:
	{
			  float log_multiplier = EQUIPMENT_ACCELLERATION_LN_MULTIPLIER_BONUS * (log(value * EQUIPMENT_ACCELLERATION_LN_MULTIPLIER_X));

			  float e_acceleration = EQUIPMENT_MIN_ACCELLERATION_VALUE * RandomizeFloatBetweenValues(BeastScale);
			  if (log_multiplier > 1)
				  e_acceleration *= log_multiplier;
			  else
				  e_acceleration = ProrataBetweenThreshold(value, sf::Vector2f(0, e_acceleration));

			  this->acceleration.x += e_acceleration;
			  this->acceleration.y += e_acceleration;
			  this->acceleration.x = floor(this->acceleration.x);
			  this->acceleration.y = floor(this->acceleration.y);
			  break;
	}
	case 1:
	{
			  //this->max_speed.x += value * EQUIPMENT_MAXSPEED_MULTIPLIER * RandomizeFloatBetweenValues(BeastScale);
			  //this->max_speed.y += value * EQUIPMENT_MAXSPEED_MULTIPLIER * RandomizeFloatBetweenValues(BeastScale);
			  float log_multiplier = EQUIPMENT_MAXSPEED_LN_MULTIPLIER_BONUS * (log(value * EQUIPMENT_MAXSPEED_LN_MULTIPLIER_X));

			  float e_maxspeed = EQUIPMENT_MIN_MAXSPEED_VALUE * RandomizeFloatBetweenValues(BeastScale);
			  if (log_multiplier > 1)
				  e_maxspeed *= log_multiplier;
			  else
				  e_maxspeed = ProrataBetweenThreshold(value, sf::Vector2f(0, e_maxspeed));

			  this->max_speed.x += e_maxspeed;
			  this->max_speed.y += e_maxspeed;
			  this->max_speed.x = floor(this->max_speed.x);
			  this->max_speed.y = floor(this->max_speed.y);
			  break;
	}
	default:
	{
			   printf("DEBUG: error: trying to add Engine property that does not exit.\n<!> Check that the chosen property for this Engine match with the existing properties in the AddEngineProperty function.\n");
			   break;
	}
	}
}

void Equipment::AddArmorProperty(int chosen_property, int value, sf::Vector2f BeastScale)
{
	switch (chosen_property) // 1 case
	{
	case 0:
	{
			  //this->armor += value * EQUIPMENT_ARMOR_MULTIPLIER * RandomizeFloatBetweenValues(BeastScale);
			  float log_multiplier = EQUIPMENT_ARMOR_LN_MULTIPLIER_BONUS * (log(value * EQUIPMENT_ARMOR_LN_MULTIPLIER_X));

			  float e_armor = EQUIPMENT_MIN_ARMOR_VALUE * RandomizeFloatBetweenValues(BeastScale);
			  if (log_multiplier > 1)
				  e_armor *= log_multiplier;
			  else
				  e_armor = ProrataBetweenThreshold(value, sf::Vector2f(0, e_armor));

			  this->armor += e_armor;
			  this->armor = floor(this->armor);
			  break;
	}
	default:
	{
			   printf("DEBUG: error: trying to add Armor property that does not exit.\n<!> Check that the chosen property for this Armor match with the existing properties in the AddArmorProperty function.\n");
			   break;
	}
	}
}

void Equipment::AddShieldProperty(int chosen_property, int value, sf::Vector2f BeastScale)
{
	switch (chosen_property) // 2 case
	{
	case 0:
	{
			  //this->shield += value * EQUIPMENT_SHIELD_MULTIPLIER;
			  float log_multiplier = EQUIPMENT_SHIELD_LN_MULTIPLIER_BONUS * (log(value * EQUIPMENT_SHIELD_LN_MULTIPLIER_X));

			  float e_shield = EQUIPMENT_MIN_SHIELD_VALUE * RandomizeFloatBetweenValues(BeastScale);
			  if (log_multiplier > 1)
				  e_shield *= log_multiplier;
			  else
				  e_shield = ProrataBetweenThreshold(value, sf::Vector2f(0, e_shield));

			  this->shield += e_shield;
			  this->shield = floor(this->shield);
			  break;
	}

	case 1:
	{
			  //this->shield_regen += value * EQUIPMENT_SHIELD_REGEN_MULTIPLIER * RandomizeFloatBetweenValues(BeastScale);
			  float log_multiplier = EQUIPMENT_SHIELD_REGEN_LN_MULTIPLIER_BONUS * (log(value * EQUIPMENT_SHIELD_REGEN_LN_MULTIPLIER_X));

			  float e_shield_regen = EQUIPMENT_MIN_SHIELD_REGEN_VALUE * RandomizeFloatBetweenValues(BeastScale);
			  if (log_multiplier > 1)
				  e_shield_regen *= log_multiplier;
			  else
				  e_shield_regen = ProrataBetweenThreshold(value, sf::Vector2f(0, e_shield_regen));

			  this->shield_regen += e_shield_regen;
			  this->shield_regen = floor(this->shield_regen);
			  break;
	}

	default:
	{
			   printf("DEBUG: error: trying to add Shield property that does not exit.\n<!> Check that the chosen property for this Shield match with the existing properties in the AddShieldProperty function.\n");
			   break;
	}
	}
}

void Equipment::AddModuleProperty(int chosen_property, int value, sf::Vector2f BeastScale)
{
	switch (chosen_property)
	{
	case 0://adding bot
	{
			   this->hasBot = true;
			   this->bot->weapon->AddBotWeaponProperty(chosen_property, value, BeastScale);
			   break;
	}
	case 1://adding bot
	{
			   this->hasBot = true;
			   this->bot->weapon->AddBotWeaponProperty(chosen_property, value, BeastScale);
			   break;
	}
	case 2://adding bot
	{
			   this->hasBot = true;
			   this->bot->weapon->AddBotWeaponProperty(chosen_property, value, BeastScale);
			   break;
	}
	case 3://adding bot
	{
			   this->hasBot = true;
			   this->bot->weapon->AddBotWeaponProperty(chosen_property, value, BeastScale);
			   break;
	}
	default:
	{
			   printf("DEBUG: error: trying to add Module property that does not exit.\n<!> Check that the chosen property for this Module match with the existing properties in the AddModuleProperty function.\n");
			   break;
	}
	}
}

// ----------------SHIP CONFIG ---------------

ShipConfig::ShipConfig()
{
	this->ship_model = new ShipModel(0, 0, 0.0f, 0.0f, 0, 0, 0, 0, EMPTYSLOT_FILENAME, sf::Vector2f(64, 64), 1, "default");
	this->automatic_fire = false;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		this->equipment[i] = NULL;
	}

	this->weapon = NULL;
}

void ShipConfig::Init()
{
	this->max_speed = getShipConfigMaxSpeed();
	this->decceleration = getShipConfigDecceleration();
	this->acceleration = getShipConfigAcceleration();
	this->hyperspeed = getShipConfigHyperspeed();
	this->armor = getShipConfigArmor();
	this->shield = getShipConfigShield();
	this->shield_regen = getShipConfigShieldRegen();
	this->damage = getShipConfigDamage();
	this->size.x = ship_model->size.x;
	this->size.y = ship_model->size.y;
	this->textureName = ship_model->textureName;
	this->frameNumber = ship_model->frameNumber;

	//Loading bots
	this->bot_list.clear();
	if (this->ship_model->hasBot)
	{
		this->bot_list.push_back(this->ship_model->bot);
	}
	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			if (this->equipment[i]->hasBot)
			{
				this->bot_list.push_back(this->equipment[i]->bot);
			}
		}
	}


}

int ShipConfig::getShipConfigArmor()
{
	int new_armor = 0;
	int equipment_armor = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_armor += equipment[i]->getEquipmentArmor();
		}
		else
		{
			equipment_armor += 0;
		}
	}

	new_armor = ship_model->getShipModelArmor() + equipment_armor;

	//cancelling negative equipment values
	if (new_armor < ship_model->getShipModelArmor())
		new_armor = ship_model->getShipModelArmor();

	return new_armor;
}

int ShipConfig::getShipConfigShield()
{
	int new_shield = 0;
	int equipment_shield = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_shield += equipment[i]->getEquipmentShield();
		}
		else
		{
			equipment_shield += 0;
		}
	}

	new_shield = ship_model->getShipModelShield() + equipment_shield;

	//cancelling negative equipment values
	if (new_shield < ship_model->getShipModelShield())
		new_shield = ship_model->getShipModelShield();

	return new_shield;
}

int ShipConfig::getShipConfigShieldRegen()
{
	int new_shield_regen = 0;
	int equipment_shield_regen = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_shield_regen += equipment[i]->getEquipmentShieldRegen();

		}
		else
		{
			equipment_shield_regen += 0;
		}
	}

	new_shield_regen = ship_model->getShipModelShieldRegen() + equipment_shield_regen;

	//cancelling negative equipment values
	if (new_shield_regen < ship_model->getShipModelShieldRegen())
		new_shield_regen = ship_model->getShipModelShieldRegen();

	return new_shield_regen;
}

int ShipConfig::getShipConfigDamage()
{
	int new_damage = 0;
	int equipment_damage = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_damage += equipment[i]->getEquipmentDamage();
		}
		else
		{
			equipment_damage += 0;
		}
	}

	new_damage = ship_model->getShipModelDamage() + equipment_damage;

	//cancelling negative equipment values
	if (new_damage < ship_model->getShipModelDamage())
		new_damage = ship_model->getShipModelDamage();

	return new_damage;
}

sf::Vector2f ShipConfig::getShipConfigMaxSpeed()
{
	sf::Vector2f new_max_speed = sf::Vector2f(0, 0);
	sf::Vector2f equipment_max_speed = sf::Vector2f(0, 0);

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_max_speed.x += equipment[i]->getEquipmentMaxSpeed().x;
			equipment_max_speed.y += equipment[i]->getEquipmentMaxSpeed().y;
		}
		else
		{
			equipment_max_speed.x += 0;
			equipment_max_speed.y += 0;
		}
	}

	new_max_speed.x = ship_model->getShipModelMaxSpeed().x + equipment_max_speed.x;
	new_max_speed.y = ship_model->getShipModelMaxSpeed().y + equipment_max_speed.y;

	//cancelling negative equipment values
	if (new_max_speed.x < ship_model->getShipModelMaxSpeed().x)
		new_max_speed.x = ship_model->getShipModelMaxSpeed().x;
	if (new_max_speed.y < ship_model->getShipModelMaxSpeed().y)
		new_max_speed.y = ship_model->getShipModelMaxSpeed().y;

	return new_max_speed;
}

float ShipConfig::getShipConfigDecceleration()
{
	float new_decceleration = 0.0f;
	float equipment_decceleration = 0.0f;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_decceleration += equipment[i]->getEquipmentDecceleration();
		}
		else
		{
			equipment_decceleration += 0;
		}
	}

	new_decceleration = ship_model->getShipModelDecceleration() + equipment_decceleration;

	//cancelling negative equipment values
	if (new_decceleration < ship_model->getShipModelDecceleration())
		new_decceleration = ship_model->getShipModelDecceleration();

	return new_decceleration;
}

sf::Vector2f ShipConfig::getShipConfigAcceleration()
{
	sf::Vector2f new_acceleration = sf::Vector2f(0, 0);
	sf::Vector2f equipment_acceleration = sf::Vector2f(0, 0);

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_acceleration.x += equipment[i]->getEquipmentAcceleration().x;
			equipment_acceleration.y += equipment[i]->getEquipmentAcceleration().y;
		}
		else
		{
			equipment_acceleration.x += 0;
			equipment_acceleration.y += 0;
		}
	}

	new_acceleration.x = ship_model->getShipModelAcceleration().x + equipment_acceleration.x;
	new_acceleration.y = ship_model->getShipModelAcceleration().y + equipment_acceleration.y;

	//cancelling negative equipment values
	if (new_acceleration.x < ship_model->getShipModelAcceleration().x)
		new_acceleration.x = ship_model->getShipModelAcceleration().x;
	if (new_acceleration.y < ship_model->getShipModelAcceleration().y)
		new_acceleration.y = ship_model->getShipModelAcceleration().y;

	return new_acceleration;
}

float ShipConfig::getShipConfigHyperspeed()
{
	float new_hyperspeed = 0.0f;
	float equipment_hyperspeed = 0.0f;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_hyperspeed += equipment[i]->getEquipmentHyperspeed();
		}
		else
		{
			equipment_hyperspeed += 0;
		}
	}

	new_hyperspeed = ship_model->getShipModelHyperspeed() + equipment_hyperspeed;

	//cancelling negative equipment values
	if (new_hyperspeed < ship_model->getShipModelHyperspeed())
		new_hyperspeed = ship_model->getShipModelHyperspeed();

	return new_hyperspeed;
}

bool ShipConfig::setEquipment(Equipment* m_equipment, bool recomputing_stats, bool overwrite)
{
	if (this->equipment[m_equipment->equipmentType] == NULL || overwrite)
	{
		if (overwrite)
		{
			delete this->equipment[m_equipment->equipmentType];
		}
		this->equipment[m_equipment->equipmentType] = m_equipment;
		if (recomputing_stats)
		{
	 		this->Init();
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool ShipConfig::setShipWeapon(Weapon* m_weapon, bool recomputing_stats, bool overwrite)
{
	if (this->weapon == NULL || overwrite)
	{
		if (overwrite)
		{
			delete this->weapon;
		}
		this->weapon = m_weapon;
		if (recomputing_stats)
		{
			this->Init();
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool ShipConfig::setShipModel(ShipModel* m_ship_model)
{
	assert(m_ship_model != NULL);

	if (m_ship_model != NULL)
	{
		this->ship_model = m_ship_model;

		this->Init();
		return true;
	}
	else
	{
		return false;
	}
}

void ShipConfig::GenerateBots(Independant* m_target)
{
	for (std::vector<Bot*>::iterator it = (this->bot_list.begin()); it != (this->bot_list.end()); it++)
	{
		Bot* m_bot = (*it)->Clone();
		m_bot->automatic_fire = this->automatic_fire;
		m_bot->spread = Independant::getSize_for_Direction((*CurrentGame).direction, m_bot->spread);
		m_bot->setTarget(m_target);
		m_bot->rotate(Independant::getRotation_for_Direction((*CurrentGame).direction));
		(*CurrentGame).addToScene(m_bot, LayerType::BotLayer, IndependantType::Neutral);
	}
}

void ShipConfig::DestroyBots()
{
	this->bot_list.clear();
	(*CurrentGame).garbageLayer(LayerType::BotLayer);
}

void ShipConfig::GenerateFakeShip(Independant* m_target)
{
	assert(this->ship_model != NULL);
	if (this->ship_model->hasFake)
	{
		FakeShip* fake_ship_ = new FakeShip(m_target, this->ship_model->fake_textureName, this->ship_model->fake_size, this->ship_model->fake_frameNumber, ShipAnimations::NB_ShipAnimations);
		(*CurrentGame).addToScene(fake_ship_, LayerType::FakeShipLayer, IndependantType::Neutral);
	}

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			if (this->equipment[i]->hasFake)
			{
				FakeShip* fake_ship = new FakeShip(m_target, this->equipment[i]->fake_textureName, this->equipment[i]->fake_size, this->equipment[i]->fake_frameNumber, ShipAnimations::NB_ShipAnimations);
				(*CurrentGame).addToScene(fake_ship, LayerType::FakeShipLayer, IndependantType::Neutral);
			}
		}
	}
}

// ----------------SHIP ---------------

Ship::Ship(Vector2f position, ShipConfig m_ship_config) : Independant(position, Vector2f(0, 0), m_ship_config.textureName, Vector2f(m_ship_config.size.x, m_ship_config.size.y), Vector2f((m_ship_config.size.x / 2), (m_ship_config.size.y / 2)), m_ship_config.frameNumber)
{
	this->collider_type = IndependantType::PlayerShip;
	this->ship_config = m_ship_config;
	this->moving = false;
	this->movingX = movingY = false;
	this->visible = true;
	this->damage = 0;
	this->shield = m_ship_config.getShipConfigShield();;
	this->disable_inputs = false;
	this->disable_fire = false;
	this->isBraking = false;
	this->isHyperspeeding = false;
	this->isSlowMotion = false;
	this->disabledHyperspeed = false;
	this->graze_count = 0;
	this->graze_level = 0;
	this->combo_aura[GrazeLevels::GRAZE_LEVEL_RED] = new Aura(this, "Assets/2D/FX/Aura_RedGlow.png", sf::Vector2f(150, 150), 3);
	this->combo_aura[GrazeLevels::GRAZE_LEVEL_BLUE] = new Aura(this, "Assets/2D/FX/Aura_BlueGlow.png", sf::Vector2f(150, 150), 3);
	this->combo_aura[GrazeLevels::GRAZE_LEVEL_WHITE] = new Aura(this, "Assets/2D/FX/Aura_WhiteGlow.png", sf::Vector2f(150, 150), 3);
	this->trail = new Aura(this, "Assets/2D/FX/Aura_HyperspeedTrail.png", sf::Vector2f(70, 34), 3, 1);
	this->trail->visible = false;
	if (this->ship_config.ship_model->hasFake)
	{
		this->trail->offset = sf::Vector2f(0, (this->ship_config.ship_model->fake_size.y / 2) + (this->trail->m_size. y / 2));
	}
	else
	{
		this->trail->offset = sf::Vector2f(0, (this->ship_config.ship_model->size.y / 2 ) + (this->trail->m_size.y / 2));
	}
	(*CurrentGame).addToScene(this->trail, LayerType::FakeShipLayer, IndependantType::Neutral);
	this->fire_key_repeat = false;
	this->slowmo_key_repeat = false;
	this->hud_key_repeat = false;
	this->isCollindingWithPortal = false;
	this->isUsingPortal = false;
	this->isFiringButtonPressed = true;//will be updated to false in the update function if button released
	this->wasBrakingButtonPressed = true;
	this->isBrakingButtonHeldPressed = false;
	this->targetPortal = NULL;
	this->equipment_loot = NULL;
	this->weapon_loot = NULL;
	this->isFocusedOnHud = false;
	this->previously_focused_item = NULL;

	this->Init();
}

void Ship::Init()
{
	this->armor = this->ship_config.getShipConfigArmor();
	this->armor_max = this->ship_config.getShipConfigArmor();
	this->shield_max = this->ship_config.getShipConfigShield();
	this->shield_regen = this->ship_config.getShipConfigShieldRegen();
	this->damage = this->ship_config.getShipConfigDamage();
	this->m_size = this->ship_config.ship_model->size;
	this->textureName = this->ship_config.ship_model->textureName;
	this->transparent = this->ship_config.ship_model->hasFake;
}

void Ship::setShipConfig(ShipConfig m_ship_config)
{
	this->ship_config = m_ship_config;
	this->Init();
}

bool Ship::setEquipment(Equipment* m_equipment, bool overwrite_existing)
{
	if (m_equipment->hasBot && (overwrite_existing || ship_config.equipment[m_equipment->equipmentType] == NULL))
	{
		this->ship_config.DestroyBots();
	}

	bool result = this->ship_config.setEquipment(m_equipment, true, overwrite_existing);
	
	if (result)
	{
		this->Init();
		if (m_equipment->hasBot)
		{
			this->ship_config.GenerateBots(this);
		}
	}
	
	return result;
}

bool Ship::setShipWeapon(Weapon* m_weapon, bool overwrite_existing)
{
	//this->ship_config.DestroyBots();
	bool result = this->ship_config.setShipWeapon(m_weapon, true, overwrite_existing);
	if (result)
	{
		this->Init();
	}
	//this->ship_config.GenerateBots(this);

	return result;
}

void Ship::cleanEquipment(int equipment_type)
{
	if (this->ship_config.equipment[equipment_type] != NULL)
	{
		bool delete_bots = this->ship_config.equipment[equipment_type]->hasBot;
		if (delete_bots)
		{
			this->ship_config.DestroyBots();
		}
		this->ship_config.equipment[equipment_type]->~Equipment();
		delete this->ship_config.equipment[equipment_type];
		this->ship_config.equipment[equipment_type] = NULL;

		this->ship_config.Init();
		this->Init();
		if (delete_bots)
		{
			this->ship_config.GenerateBots(this);
		}
	}
}

void Ship::cleanWeapon()
{
	if (this->ship_config.weapon != NULL)
	{
		this->ship_config.weapon->~Weapon();
		delete this->ship_config.weapon;
		this->ship_config.weapon = NULL;

		this->ship_config.Init();
		this->Init();
	}
}

void Ship::setShipModel(ShipModel* m_ship_model)
{
	if (m_ship_model->hasBot)
	{
		this->ship_config.DestroyBots();
	}

	this->ship_config.setShipModel(m_ship_model);
	this->Init();

	if (m_ship_model->hasBot)
	{
		this->ship_config.GenerateBots(this);
	}
}

void Ship::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	this->isBraking = false;
	this->isHyperspeeding = false;

	//immunity frames after death
	if (immune)
	{
		if (immunityTimer.getElapsedTime() > sf::seconds(2))
		{
			immune = false;
		}
	}

	//CHEAT
	//this->immune = true;

	//sheld regen if not maximum
	static double shield_regen_buffer = 0;
	if (shield < ship_config.getShipConfigShield())
	{
		if (hyperspeedMultiplier < 1.0f)
		{
			shield_regen_buffer += shield_regen * deltaTime.asSeconds() * hyperspeedMultiplier;
		}
		else
		{
			shield_regen_buffer += shield_regen * deltaTime.asSeconds();
		}
		
		if (shield_regen_buffer > 1)
		{
			double intpart;
			shield_regen_buffer = modf(shield_regen_buffer, &intpart);
			shield += intpart;
		}

		//canceling over-regen
		if (shield > ship_config.getShipConfigShield())
		{
			shield = ship_config.getShipConfigShield();
		}
	}

	sf::Vector2f directions = InputGuy::getDirections();

	//switching controls to the HUD
	//Ship controls
 	if (!disable_inputs)
	{
		if (InputGuy::isOpeningHud())
		{
			if (!this->hud_key_repeat)
			{
				isFocusedOnHud = !isFocusedOnHud;
				hud_key_repeat = true;

				(*CurrentGame).hud.has_focus = isFocusedOnHud;
				disable_fire = isFocusedOnHud;

				if (isFocusedOnHud && !isSlowMotion)
				{
					(*CurrentGame).hyperspeedMultiplier = 1.0f / this->ship_config.getShipConfigHyperspeed();
				}
				else if (!isFocusedOnHud && !isSlowMotion)
				{
					(*CurrentGame).hyperspeedMultiplier = 1.0f;
				}
			}
		}
		else
		{
			hud_key_repeat = false;
		}

		moving = directions.x != 0 || directions.y != 0;
		movingX = directions.x != 0;
		movingY = directions.y != 0;

		if (!isFocusedOnHud)
		{
			speed.x += directions.x*ship_config.getShipConfigAcceleration().x;
			speed.y += directions.y*ship_config.getShipConfigAcceleration().y;

			//max speed constraints
			if (abs(speed.x) > this->ship_config.getShipConfigMaxSpeed().x)
			{
				speed.x = speed.x > 0 ? this->ship_config.getShipConfigMaxSpeed().x : -this->ship_config.getShipConfigMaxSpeed().x;
			}
			if (abs(speed.y) > this->ship_config.getShipConfigMaxSpeed().y)
			{
				speed.y = speed.y > 0 ? this->ship_config.getShipConfigMaxSpeed().y : -this->ship_config.getShipConfigMaxSpeed().y;
			}

			//Slow_motion function
			if (InputGuy::isSlowMotion() && !disable_fire)
			{
				if (!this->slowmo_key_repeat)
				{
					this->isSlowMotion = !this->isSlowMotion;
					this->slowmo_key_repeat = true;
					if (this->isSlowMotion)
					{
						(*CurrentGame).hyperspeedMultiplier = 1.0f / this->ship_config.getShipConfigHyperspeed();
					}
					else
					{
						(*CurrentGame).hyperspeedMultiplier = 1.0f;
					}
				}
			}
			else
			{
				this->slowmo_key_repeat = false;
			}

			//Hyperspeed function
			if (InputGuy::isHyperspeeding() && !this->disabledHyperspeed && !this->isHyperspeeding && !this->isBraking &&!this->isSlowMotion)
			{
				this->isHyperspeeding = true;
				(*CurrentGame).hyperspeedMultiplier = this->ship_config.getShipConfigHyperspeed();
			}
			else if (!this->isSlowMotion)
			{
				(*CurrentGame).hyperspeedMultiplier = 1.0f;
			}

			//auto fire option (F key)
			if (InputGuy::setAutomaticFire() && !disable_fire)
			{
				if (!this->fire_key_repeat)
				{
					this->ship_config.automatic_fire = !this->ship_config.automatic_fire;
					this->fire_key_repeat = true;
				}
			}
			else
			{
				this->fire_key_repeat = false;
			}

			//portals: required to release fire and then press fire while colliding with a portal
			isUsingPortal = false;
			if (!isFiringButtonPressed)
			{
				if (isCollindingWithPortal)
				{
					if (InputGuy::isFiring())
					{
						if (this->targetPortal != NULL)
						{
							if (this->targetPortal->currentAnimationIndex == (int)(PortalAnimation::PortalOpenIdle))
							{
								isUsingPortal = true;

								if (this->targetPortal->destination_name.compare("0") == 0)
								{
									LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error, entering a portal that has an empty destination name\n");
								}
							}
						}
						else
						{
							LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error, Ship::targetPortal still NULL when it should be initialized.\n");
						}
					}
				}
			}
			//Fire function
			if (this->ship_config.weapon != NULL)
			{
				if (!disable_fire && !isUsingPortal && !isHyperspeeding)
				{
					if ((InputGuy::isFiring() || this->ship_config.automatic_fire))
					{
						//calculating the angle we want to face, if any
						float target_angle = fmod(Independant::getRotation_for_Direction((*CurrentGame).direction) - (*CurrentGame).GetAngleToNearestIndependant(IndependantType::EnemyObject, this->getPosition()), 360);

						float current_angle = this->getRotation();
						float delta = current_angle - target_angle;
						if (delta > 180)
							delta -= 360;
						else if (delta < -180)
							delta += 360;

						float theta = (this->getRotation() - delta) / 180 * M_PI;

						float sizeY = this->m_size.y;
						if (this->ship_config.ship_model->hasFake)
						{
							if (this->ship_config.ship_model->fake_size.y > sizeY)
							{
								sizeY = this->ship_config.ship_model->fake_size.y;
							}
						}

						float x_weapon_offset = sizeY / 2 * sin(theta);
						float y_weapon_offset = -sizeY / 2 * cos(theta);

						ship_config.weapon->setPosition(this->getPosition().x + x_weapon_offset, this->getPosition().y + y_weapon_offset);
						ship_config.weapon->shot_angle = theta;
						ship_config.weapon->Fire(FriendlyFire, deltaTime, hyperspeedMultiplier);

						//speed malus when shooting
						if (!this->isBraking)
						{
							speed.x *= SHIP_BRAKING_MALUS_SPEED;
							speed.y *= SHIP_BRAKING_MALUS_SPEED;
						}
						this->isBraking = true;
					}
					else
					{
						//even if we don't shoot, the weapon has to keep reloading
						ship_config.weapon->isFiringReady(deltaTime, hyperspeedMultiplier);
					}
				}
				else
				{
					//even if we don't shoot, the weapon has to keep reloading
					ship_config.weapon->isFiringReady(deltaTime, hyperspeedMultiplier);
				}
			}
			
				
			isCollindingWithPortal = false;

			//Braking function
			if (InputGuy::isBraking() && !this->isBraking && !this->isHyperspeeding)
			{
				speed.x *= SHIP_BRAKING_MALUS_SPEED;
				speed.y *= SHIP_BRAKING_MALUS_SPEED;
				this->isBraking = true;
			}
		}

		//HUD controls
		else
		{
			//movement
			Independant* cursor_ = (*CurrentGame).hud.hud_cursor;
			cursor_->speed.x = directions.x * HUD_CURSOR_SPEED;
			cursor_->speed.y = directions.y * HUD_CURSOR_SPEED;

			//focus
			bool has_changed_focused_item_ = true;
			if ((*CurrentGame).getHudFocusedItem() == this->previously_focused_item)
			{
				has_changed_focused_item_ = false;
			}
			else
			{
				this->previously_focused_item = (*CurrentGame).getHudFocusedItem();
			}

			if (!isFiringButtonPressed)
			{
				if (InputGuy::isFiring())
				{
					if (!fire_key_repeat)
					{
						//interaction
						if ((*CurrentGame).getHudFocusedItem() != NULL)
						{
							if ((*CurrentGame).getHudFocusedGridAndIndex().x == (int)HudGrid_EquipmentGrid)
							{
								Independant* tmp_ptr = (*CurrentGame).getHudFocusedItem();
								int equip_index_ = (*CurrentGame).getHudFocusedGridAndIndex().y;

								if (tmp_ptr->getEquipmentLoot() != NULL)
								{
									int ship_index_ = tmp_ptr->getEquipmentLoot()->equipmentType;

									//if there is no item we don't need to swap items, just equip it. Otherwise, we do a swap between the grids
									if ((*CurrentGame).SwapEquipObjectInShipGrid(ship_index_, equip_index_))
									{
										//if this succeeds, we can actually equip the item
										Equipment* new_equipment = (*CurrentGame).hud.shipGrid.getCellPointerFromIntIndex(ship_index_)->getEquipmentLoot()->Clone();
										this->setEquipment(new_equipment, true);
										new_equipment = NULL;
									}
								}
								else if (tmp_ptr->getWeaponLoot() != NULL)
								{
									int ship_index_ = NBVAL_Equipment;

									//if there is no item we don't need to swap items, just equip it. Otherwise, we do a swap between the grids
									if ((*CurrentGame).SwapEquipObjectInShipGrid(ship_index_, equip_index_))
									{
										//if this succeeds, we can actually equip the item
										Weapon* new_weapon = (*CurrentGame).hud.shipGrid.getCellPointerFromIntIndex(ship_index_)->getWeaponLoot()->Clone();
										this->setShipWeapon(new_weapon, true);
										new_weapon = NULL;
									}
								}
								else
								{
									LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: trying to swap an item that has no equipment or weapon.\n");
								}

								tmp_ptr = NULL;
							}
						}

						fire_key_repeat = true;
					}
				}
				else
				{
					fire_key_repeat = false;
				}
			}

			if (!wasBrakingButtonPressed || isBrakingButtonHeldPressed)
			{
				if (InputGuy::isBraking())
				{
					if (!isBrakingButtonHeldPressed)
					{
						brakingHoldingClock.restart();
						isBrakingButtonHeldPressed = true;
						(*CurrentGame).hud.has_prioritary_cursor_feedback = true;
					}
					else
					{
						if (has_changed_focused_item_)
						{
							brakingHoldingClock.restart();
							isBrakingButtonHeldPressed = false;
							(*CurrentGame).hud.has_prioritary_cursor_feedback = false;
						}
						else if ((*CurrentGame).getHudFocusedItem() != NULL)
						{
							if (brakingHoldingClock.getElapsedTime() > sf::seconds(HUD_HOLD_TIME_BEFORE_REMOVE_ITEM))
							{
								(*CurrentGame).hud.hud_cursor->setAnimationLine(Cursor_Focus1_8);
								if (brakingHoldingClock.getElapsedTime().asSeconds() < HUD_HOLD_TIME_BEFORE_REMOVE_ITEM / 8)
									(*CurrentGame).hud.hud_cursor->setAnimationLine(Cursor_Focus1_8);

								int equip_type = NBVAL_Equipment;
								if ((*CurrentGame).getHudFocusedItem()->getEquipmentLoot() != NULL)
								{
									equip_type = (*CurrentGame).getHudFocusedItem()->getEquipmentLoot()->equipmentType;
								}
								//garbage in hud
								int grid_id_ = (*CurrentGame).getHudFocusedGridAndIndex().x;
								int index_ = (*CurrentGame).getHudFocusedGridAndIndex().y;
								(*CurrentGame).GarbageObjectInGrid(grid_id_, index_);
								//garbage for real
								if (grid_id_ == (int)HudGrid_ShipGrid)
								{
									if (equip_type == NBVAL_Equipment)
									{
										this->cleanWeapon();
									}
									else
									{
										this->cleanEquipment(equip_type);
									}
								}

								brakingHoldingClock.restart();
								isBrakingButtonHeldPressed = false;
								(*CurrentGame).hud.has_prioritary_cursor_feedback = false;
							}
							else
							{
								for (int k = 0; k < HUD_CURSOR_HOLDING_FRACTIONS; k++)
								{
									if (brakingHoldingClock.getElapsedTime().asSeconds() < (1.0f * HUD_HOLD_TIME_BEFORE_REMOVE_ITEM / HUD_CURSOR_HOLDING_FRACTIONS) * (k + 1))
									{
										(*CurrentGame).setRemovingCursorAnimation((CursorFeedbackStates)(Cursor_Focus1_8 + k));
										(*CurrentGame).hud.has_prioritary_cursor_feedback = true;
										break;
									}
								}
							}
						}	
					}
				}
				else
				{
					isBrakingButtonHeldPressed = false;
					brakingHoldingClock.restart();
					(*CurrentGame).hud.has_prioritary_cursor_feedback = false;
				}
			}
			
			cursor_ = NULL;
		}

		//testing button release
		if (InputGuy::isFiring())
		{
			isFiringButtonPressed = true;
		}
		else
		{
			isFiringButtonPressed = false;
		}

		if (InputGuy::isBraking())
		{
			wasBrakingButtonPressed = true;
		}
		else
		{
			wasBrakingButtonPressed = false;
		}

		//idle decceleration
		if (!movingX || isFocusedOnHud == true)
		{
			speed.x -= (speed.x)*deltaTime.asSeconds()*(ship_config.getShipConfigDecceleration() / 100);

			if (abs(speed.x) < SHIP_MIN_SPEED_X)
				speed.x = 0;
		}

		if (!movingY || isFocusedOnHud == true)
		{
			speed.y -= (speed.y)*deltaTime.asSeconds()*(ship_config.getShipConfigDecceleration() / 100);

			if (abs(speed.y) < SHIP_MIN_SPEED_Y)
				speed.y = 0;
		}
	}
	else
	{
		isFocusedOnHud = false;
	}

	this->trail->visible = (hyperspeedMultiplier > 1.0f);

	Independant::update(deltaTime, hyperspeedMultiplier);

	//screen borders contraints	correction
	if (this->ship_config.ship_model->hasFake)
	{
		if (this->getPosition().x < ship_config.ship_model->fake_size.x / 2)
		{
			this->setPosition(ship_config.ship_model->fake_size.x / 2, this->getPosition().y);
			speed.x = 0;
		}

		if (this->getPosition().x > SCENE_SIZE_X - (ship_config.ship_model->fake_size.x / 2))
		{
			this->setPosition(SCENE_SIZE_X - (ship_config.ship_model->fake_size.x / 2), this->getPosition().y);
			speed.x = 0;
		}

		if (this->getPosition().y < ship_config.ship_model->fake_size.y / 2)
		{
			this->setPosition(this->getPosition().x, ship_config.ship_model->fake_size.y / 2);
			speed.y = 0;
		}

		if (this->getPosition().y > SCENE_SIZE_Y - (ship_config.ship_model->fake_size.y / 2))
		{
			this->setPosition(this->getPosition().x, SCENE_SIZE_Y - (ship_config.ship_model->fake_size.y / 2));
			speed.y = 0;
		}
	}
	else
	{
		if (this->getPosition().x < ship_config.size.x / 2)
		{
			this->setPosition(ship_config.size.x / 2, this->getPosition().y);
			speed.x = 0;
		}

		if (this->getPosition().x > SCENE_SIZE_X - (ship_config.size.x / 2))
		{
			this->setPosition(SCENE_SIZE_X - (ship_config.size.x / 2), this->getPosition().y);
			speed.x = 0;
		}

		if (this->getPosition().y < ship_config.size.y / 2)
		{
			this->setPosition(this->getPosition().x, ship_config.size.y / 2);
			speed.y = 0;
		}

		if (this->getPosition().y > SCENE_SIZE_Y - (ship_config.size.y / 2))
		{
			this->setPosition(this->getPosition().x, SCENE_SIZE_Y - (ship_config.size.y / 2));
			speed.y = 0;
		}
	}

	//setting animation
	const sf::Vector2f f = (sf::Vector2f)Independant::getDirectionMultiplier((*CurrentGame).direction);
	const float x = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(this->speed.x * f.x, this->speed.y * f.y)).x;

	if (x > 0 && this->currentAnimationIndex != ShipAnimations::ShipTurningRight && !this->disable_inputs)
	{
		if (!this->ship_config.ship_model->hasFake)
		{
			this->setAnimationLine(ShipAnimations::ShipTurningRight, true);
		}
		else
		{
			this->currentAnimationIndex = ShipAnimations::ShipTurningRight;
		}
	}

	else if (x < 0 && this->currentAnimationIndex != ShipAnimations::ShipTurningLeft && !this->disable_inputs)
	{
		if (!this->ship_config.ship_model->hasFake)
		{
			this->setAnimationLine(ShipAnimations::ShipTurningLeft, true);
		}
		else
		{
			this->currentAnimationIndex = ShipAnimations::ShipTurningLeft;
		}
	}

	else if ((x == 0 && this->currentAnimationIndex != ShipAnimations::ShipIdle) || this->disable_inputs)
	{
		if (!this->ship_config.ship_model->hasFake)
		{
			this->setAnimationLine(ShipAnimations::ShipIdle, true);
		}
		else
		{
			this->currentAnimationIndex = ShipAnimations::ShipIdle;
		}
	}
}

void Ship::Respawn()
{
	armor = ship_config.getShipConfigArmor();
	shield = ship_config.getShipConfigShield();
	speed.x = 0;
	speed.y = 0;
	this->visible = true;
	isOnScene = true;
	sf::Vector2f pos = sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO);
	pos = Independant::getPosition_for_Direction((*CurrentGame).direction, pos);
	this->setPosition(pos);

	immune = true;
	immunityTimer.restart();
}

void Ship::Death()
{
	FX* myFX = this->ship_config.FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	(*CurrentGame).addToScene(myFX, LayerType::ExplosionLayer, IndependantType::Neutral);
}

Independant* Ship::CloneEquipmentIntoIndependant(Equipment* new_equipment)
{
	assert(new_equipment != NULL);
	Independant* capsule = new Independant(sf::Vector2f(0, 0), sf::Vector2f(0, 0), new_equipment->textureName, new_equipment->size, sf::Vector2f(new_equipment->size.x / 2, new_equipment->size.y / 2), new_equipment->frameNumber);
	capsule->setEquipmentLoot(new_equipment->Clone());

	return capsule;
}

Independant* Ship::CloneWeaponIntoIndependant(Weapon* new_weapon)
{
	assert(new_weapon != NULL);
	Independant* capsule = new Independant(new_weapon->getPosition(), sf::Vector2f(0, 0), new_weapon->textureName, new_weapon->size, sf::Vector2f(new_weapon->size.x / 2, new_weapon->size.y / 2), new_weapon->frameNumber);
	capsule->setWeaponLoot(new_weapon->Clone());

	return capsule;
}

bool Ship::GetLoot(Independant& independant)
{
	//EQUIPMENT
	if (independant.getEquipmentLoot() != NULL)
	{
		Independant* capsule = CloneEquipmentIntoIndependant(independant.getEquipmentLoot());
		if (this->setEquipment(independant.getEquipmentLoot()))
		{
			//if the ship config does not have any equipment of this type on, we equip it and update the HUD
			
			(*CurrentGame).InsertObjectInShipGrid(*capsule, independant.getEquipmentLoot()->equipmentType);
		}
		else
		{
			//...else we put it in the stash
			(*CurrentGame).InsertObjectInEquipmentGrid(*capsule);
		}
		//independant.releaseEquipmentLoot();
		//independant.releaseWeaponLoot();
		return true;
	}

	//WEAPON
	if (independant.getWeaponLoot() != NULL)
	{
		Independant* capsule = CloneWeaponIntoIndependant(independant.getWeaponLoot());
		if (this->setShipWeapon(independant.getWeaponLoot()))
		{
			//if the ship config does not have a weapon already, we equip it and update the HUD
			(*CurrentGame).InsertObjectInShipGrid(*capsule, NBVAL_Equipment);
		}
		else
		{
			//...else we put it in the stash
			(*CurrentGame).InsertObjectInEquipmentGrid(*capsule);
		}
		//independant.releaseEquipmentLoot();
		//independant.releaseWeaponLoot();
		return true;
	}

	//MONEY
	return this->get_money_from(independant);
}

void Ship::GetPortal(Independant* independant)
{
	this->targetPortal = (Portal*)(independant);
	this->isCollindingWithPortal = true;
}

static int GrazeLevelsThresholds[GrazeLevels::NB_GRAZE_LEVELS] = { 0, 10, 40, 70 };
static float GrazeLevelsBeastBonus[GrazeLevels::NB_GRAZE_LEVELS] = { 0.0f, 0.2f, 0.4f, 0.6f };

void Ship::GetGrazing()
{
	graze_count++;

	if (graze_level < GrazeLevels::NB_GRAZE_LEVELS - 1)
	{
		if (graze_count >= GrazeLevelsThresholds[graze_level + 1])
		{
			//Graze level up
			graze_level++;
			switch (graze_level)
			{
			case GRAZE_LEVEL_RED:
			{
				(*CurrentGame).garbageLayer(AuraLayer);
				Aura* m_combo_aura = this->combo_aura[GRAZE_LEVEL_RED]->Clone();
				(*CurrentGame).addToScene(m_combo_aura, AuraLayer, Neutral);
				break;
			}
			case GRAZE_LEVEL_BLUE:
			{
				(*CurrentGame).garbageLayer(AuraLayer);
				Aura* m_combo_aura = this->combo_aura[GRAZE_LEVEL_BLUE]->Clone();
				(*CurrentGame).addToScene(m_combo_aura, AuraLayer, Neutral);
				break;
			}
			case GRAZE_LEVEL_WHITE:
			{
				(*CurrentGame).garbageLayer(AuraLayer);
				Aura* m_combo_aura = this->combo_aura[GRAZE_LEVEL_WHITE]->Clone();
				(*CurrentGame).addToScene(m_combo_aura, AuraLayer, Neutral);
				break;
			}
			default:
			{
					   graze_level = 0;
					   LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error, entering a GrazeLevels case that does not exist. Combo aura cannot be generated\n");
					   break;
			}
			}
		}
	}
}

int Ship::getGrazeCount()
{
	return this->graze_count;
}

float Ship::getShipBeastScore()
{
	float bonus = 0;
	if (graze_level < NB_GRAZE_LEVELS && this->graze_level >= 0)
	{
		bonus = GrazeLevelsBeastBonus[graze_level];
	}
	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error, The ship has a 'graze_level' (%d) beyond existing values\n", this->graze_level);
		this->graze_count = 0;
		this->graze_level = 0;
	}

	return bonus;
}

void Ship::damage_from(Independant& independant)
{
	if (!immune)
	{
		if (independant.damage > shield)
		{
			armor -= (independant.damage - shield);
			shield = 0;
		}
		else
		{
			shield -= independant.damage;
		}
	}
	graze_count = 0;
	graze_level = GRAZE_LEVEL_NONE;
	(*CurrentGame).garbageLayer(AuraLayer);
}