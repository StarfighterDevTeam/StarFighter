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

	this->bonus_armor = 0;
	this->bonus_shield = 0;
	this->bonus_shield_regen = 0;
	this->bonus_damage = 0;
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

	new_equipment->loot_credits = this->loot_credits;
	new_equipment->bonus_armor = this->bonus_armor;
	new_equipment->bonus_shield = this->bonus_shield;
	new_equipment->bonus_shield_regen = this->bonus_shield_regen;
	new_equipment->bonus_damage = this->bonus_damage;

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

Equipment* Equipment::CreateRandomArmor(int credits_)
{
	//Spending credits on the possible bonuses
	int bonus_armor_ = 0;
	int bonus_damage_ = 0;

	int loot_credits_remaining_ = credits_;
	while (loot_credits_remaining_ > 0)
	{
		int random_type_of_bonus_ = RandomizeIntBetweenValues(0, 5);

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus_)
		{
			case 0://damage
			{
				loot_credits_remaining_ --;
				bonus_damage_++;
				break;
			}

			default://armor
			{
				loot_credits_remaining_--;
				bonus_armor_++;
				break;
			}
		}
	}

	//Creating the item
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Armor, 0, 0, 0.f, 0.f, 0, 0, 0, 0, ARMOR_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Armor");

	//allocating bonuses to the weapon
	equipment->loot_credits = credits_;
	equipment->bonus_damage = bonus_damage_;
	equipment->bonus_armor = bonus_armor_;

	return equipment;
}

Equipment* Equipment::CreateRandomShield(int credits_)
{
	//Spending credits on the possible bonuses
	int bonus_shield_ = 0;
	int bonus_shield_regen_ = 0;

	int loot_credits_remaining_ = credits_;
	while (loot_credits_remaining_ > 0)
	{
		int random_type_of_bonus_ = RandomizeIntBetweenValues(0, 3);

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus_)
		{
			case 0://shield regen
			{
				loot_credits_remaining_--;
				bonus_shield_regen_++;
				break;
			}

			default://shield
			{
				loot_credits_remaining_--;
				bonus_shield_++;
				break;
			}
		}
	}

	//Creating the item
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Shield, 0, 0, 0.f, 0.f, 0, 0, 0, 0, SHIELD_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Shield");

	//allocating bonuses to the weapon
	equipment->loot_credits = credits_;
	equipment->bonus_shield = bonus_shield_;
	equipment->bonus_shield_regen = bonus_shield_regen_;

	return equipment;
}

Equipment* Equipment::CreateRandomModule(int credits_)
{
	//Spending credits on the possible bonuses
	Weapon* weapon = Weapon::CreateRandomWeapon(floor(credits_ * BOT_STATS_MULTIPLIER));

	//Initialisation
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Module, 0, 0, 0.f, 0.f, 0, 0, 0, 0, MODULE_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Module");

	Bot* bot = new Bot(sf::Vector2f(0, 0), sf::Vector2f(0, 0), BOT_FILENAME, sf::Vector2f(BOT_SIZE, BOT_SIZE));
	bot->display_name = "Bot gerard";
	bot->radius = 500;
	bot->vspeed = 300;
	bot->spread = sf::Vector2f(-50, 0);

	vector<float>* v = new vector<float>;
	//v->push_back(bot->radius); // rayon 500px
	//v->push_back(1);  // clockwise (>)
	PatternType pattern_type = PatternType::NoMovePattern;
	bot->Pattern.SetPattern(pattern_type, bot->vspeed, v); //vitesse angulaire (degres/s)

	bot->weapon = weapon;
	equipment->bot = bot;

	//allocating bonuses to the weapon
	equipment->loot_credits = credits_;

	return equipment;
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

int ShipConfig::getShipConfigArmorBonus()
{
	int new_bonus_armor = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			new_bonus_armor += equipment[i]->bonus_armor;
		}
		else
		{
			new_bonus_armor += 0;
		}
	}

	return new_bonus_armor;
}

int ShipConfig::getShipConfigShieldBonus()
{
	int new_bonus_shield = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			new_bonus_shield += equipment[i]->bonus_shield;
		}
		else
		{
			new_bonus_shield += 0;
		}
	}

	return new_bonus_shield;
}

int ShipConfig::getShipConfigShieldRegenBonus()
{
	int new_bonus_shield_regen = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			new_bonus_shield_regen += equipment[i]->bonus_shield_regen;
		}
		else
		{
			new_bonus_shield_regen += 0;
		}
	}

	return new_bonus_shield_regen;
}

int ShipConfig::getShipConfigDamageBonus()
{
	int new_bonus_damage = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			new_bonus_damage += equipment[i]->bonus_damage;
		}
		else
		{
			new_bonus_damage += 0;
		}
	}

	return new_bonus_damage;
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
		m_fake_ship = new FakeShip(m_target, this->ship_model->fake_textureName, this->ship_model->fake_size, this->ship_model->fake_frameNumber, ShipAnimations::NB_ShipAnimations);
		(*CurrentGame).addToScene(m_fake_ship, LayerType::FakeShipLayer, IndependantType::FakePlayerShip);
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

	this->level = 1;
	this->level_max = FIRST_LEVEL_MAX;
	this->xp = 0;
	this->xp_max = XP_MAX_FIRST_LEVEL;

	this->m_combo_aura[GrazeLevels::GRAZE_LEVEL_RED] = new Aura(this, "Assets/2D/FX/Aura_RedGlow.png", sf::Vector2f(150, 150), 3);
	this->m_combo_aura[GrazeLevels::GRAZE_LEVEL_BLUE] = new Aura(this, "Assets/2D/FX/Aura_BlueGlow.png", sf::Vector2f(150, 150), 3);
	this->m_combo_aura[GrazeLevels::GRAZE_LEVEL_WHITE] = new Aura(this, "Assets/2D/FX/Aura_WhiteGlow.png", sf::Vector2f(150, 150), 3);
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
	this->m_interactionType = No_Interaction;
	this->isFiringButtonPressed = true;//will be updated to false in the update function if button released
	this->wasBrakingButtonPressed = true;
	this->isBrakingButtonHeldPressed = false;
	this->wasHyperspeedingButtonPressed = true;
	this->targetPortal = NULL;
	this->targetShop = NULL;
	this->equipment_loot = NULL;
	this->weapon_loot = NULL;
	this->isFocusedOnHud = false;
	this->previously_focused_item = NULL;

	this->previouslyCollindingWithInteractiveObject = No_Interaction;
	this->isCollindingWithInteractiveObject = No_Interaction;

	this->ship_base_stat_multiplier = (*CurrentGame).GetPlayerStatsMultiplierForLevel(this->level);

	this->Init();
}

void Ship::Init()
{
	float multiplier_ = 1.0f * this->ship_base_stat_multiplier / 100;

	this->armor_max = ceil(multiplier_ * this->ship_config.getShipConfigArmor() * (1 + (1.0f * this->ship_config.getShipConfigArmorBonus() / 100)));
	if (this->armor > this->armor_max)
	{
		this->armor = this->armor_max;
	}
	this->shield_max = ceil(multiplier_ * this->ship_config.getShipConfigShield() * (1 + (1.0f * this->ship_config.getShipConfigShieldBonus() / 100)));
	if (this->shield > this->shield_max)
	{
		this->shield = this->shield_max;
	}
	this->shield_regen = ceil(multiplier_ * this->ship_config.getShipConfigShieldRegen() * (1 + (1.0f * this->ship_config.getShipConfigShieldRegenBonus() / 100)));
	this->damage = ceil(multiplier_ * this->ship_config.getShipConfigDamage() * (1 + (1.0f * this->ship_config.getShipConfigDamageBonus() / 100)));

	this->ship_config.weapon->ammunition->damage = ceil(multiplier_ * this->ship_config.weapon->ammunition->damage * (1 + (1.0f * this->ship_config.weapon->bonus_damage / 100))
		* (1 + (1.0f * CREDITS_COST_PER_ONE_MULTISHOT * (this->ship_config.weapon->multishot - MIN_VALUE_OF_MULTISHOT) / 100)));
	this->ship_config.weapon->rate_of_fire = FIRST_LEVEL_RATE_OF_FIRE * (1 - (1.0f * ship_config.weapon->bonus_rate_of_fire / 100));
	this->ship_config.weapon->multishot = MIN_VALUE_OF_MULTISHOT + this->ship_config.weapon->bonus_multishot;

	for (std::vector<Bot*>::iterator it = (this->ship_config.bot_list.begin()); it != (this->ship_config.bot_list.end()); it++)
	{
		(*it)->weapon->ammunition->damage = ceil(multiplier_ * FIRST_LEVEL_AMMO_DAMAGE * (1 + (1.0f * (*it)->weapon->bonus_damage / 100))
			* (1 + (1.0f * CREDITS_COST_PER_ONE_MULTISHOT * ((*it)->weapon->multishot - MIN_VALUE_OF_MULTISHOT) / 100)));
		this->ship_config.weapon->rate_of_fire = FIRST_LEVEL_RATE_OF_FIRE * (1 - (1.0f * (*it)->weapon->bonus_rate_of_fire / 100));
		this->ship_config.weapon->multishot = ceil(MIN_VALUE_OF_MULTISHOT * BOT_STATS_MULTIPLIER) + this->ship_config.weapon->bonus_multishot;
	}
	
	

	printf("\nLEVEL %d. Stats:\n", level);
	printf("armor: %d\n", armor_max);
	printf("shield: %d\n", shield_max);
	printf("shield_regen: %d\n", shield_regen);
	printf("ammo damage: %d\n", ship_config.weapon->ammunition->damage);

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

void Ship::ManageDebugCommand()
{
	if (InputGuy::isUsingDebugCommand())
	{
		(*CurrentGame).killIndependantLayer(EnemyObject);
	}
}

void Ship::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (ManageVisibility())
	{
		ManageDebugCommand();

		ManageImmunity();

		ManageShieldRegen(deltaTime, hyperspeedMultiplier);

		sf::Vector2f directions = InputGuy::getDirections();

		ManageInputs(deltaTime, hyperspeedMultiplier, directions);
		
		previouslyCollindingWithInteractiveObject = isCollindingWithInteractiveObject;
		isCollindingWithInteractiveObject = No_Interaction;

		this->trail->visible = (hyperspeedMultiplier > 1.0f);

		Independant::update(deltaTime, hyperspeedMultiplier);

		ScreenBorderContraints();

		SettingTurnAnimations();

		ManageFeedbackExpiration(deltaTime);		
	}
}

void Ship::ManageOpeningHud()
{
	if (InputGuy::isOpeningHud())
	{
		if (!this->hud_key_repeat)
		{
			isFocusedOnHud = !isFocusedOnHud;
			hud_key_repeat = true;

			(*CurrentGame).hud.has_focus = isFocusedOnHud;
			disable_fire = isFocusedOnHud;
			if ((*CurrentGame).direction == NO_DIRECTION)
			{
				disable_fire = true;
			}

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
}

bool Ship::ManageVisibility()
{
	//no update on invisible ship (=dead)
	bool l_visible = false;
	if (this->visible)
	{
		l_visible = true;
	}
	if (this->ship_config.m_fake_ship != NULL)
	{
		if (this->ship_config.m_fake_ship->visible)
		{
			l_visible = true;
		}
	}

	return l_visible;
}

void Ship::ManageShieldRegen(sf::Time deltaTime, float hyperspeedMultiplier)
{
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
}

void Ship::ManageFiring(sf::Time deltaTime, float hyperspeedMultiplier)
{
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

	//Fire function
	if (this->ship_config.weapon != NULL)
	{
		if (ship_config.weapon->isFiringReady(deltaTime, hyperspeedMultiplier))
		{
			if (!disable_fire && (isCollindingWithInteractiveObject == No_Interaction) && !isHyperspeeding)
			{
				if ((InputGuy::isFiring() || this->ship_config.automatic_fire))
				{
					//calculating the angle we want to face, if any
					float target_angle = this->getRotation();
					if (ship_config.weapon->target_seaking != NO_SEAKING || (ship_config.weapon->target_seaking == SEMI_SEAKING && ship_config.weapon->rafale_index == 0))
					{
						target_angle = fmod(Independant::getRotation_for_Direction((*CurrentGame).direction) - (*CurrentGame).GetAngleToNearestIndependant(IndependantType::EnemyObject, this->getPosition()), 360);
					}

					float current_angle = this->getRotation();
					float delta = current_angle - target_angle;
					if (delta > 180)
						delta -= 360;
					else if (delta < -180)
						delta += 360;

					//float theta = (this->getRotation() - delta) / 180 * M_PI;
					float theta = this->getRotation() / 180 * M_PI;
					if (ship_config.weapon->target_seaking != NO_SEAKING)
					{
						theta -= delta / 180 * M_PI;
					}

					float sizeX = this->m_size.x;
					float sizeY = this->m_size.y;
					if (this->ship_config.ship_model->hasFake)
					{
						if (this->ship_config.ship_model->fake_size.x > sizeX)
						{
							sizeX = this->ship_config.ship_model->fake_size.x;
						}
						if (this->ship_config.ship_model->fake_size.y > sizeY)
						{
							sizeY = this->ship_config.ship_model->fake_size.y;
						}
					}

					if (ship_config.weapon->target_seaking == SEMI_SEAKING && ship_config.weapon->rafale_index > 0 && ship_config.weapon->rafale_index < ship_config.weapon->rafale)
					{
						//semi-seaking and rafale not ended = no update of target or weapon position
					}
					else
					{
						ship_config.weapon->weapon_current_offset.x = ship_config.weapon->weaponOffset.x + sizeX / 2 * sin(theta);
						ship_config.weapon->weapon_current_offset.y = ship_config.weapon->weaponOffset.y - sizeY / 2 * cos(theta);

						//transmitting the angle to the weapon, which will pass it to the bullets
						ship_config.weapon->shot_angle = theta;
					}

					ship_config.weapon->setPosition(this->getPosition().x + ship_config.weapon->weapon_current_offset.x, this->getPosition().y + ship_config.weapon->weapon_current_offset.y);
					ship_config.weapon->Fire(FriendlyFire, deltaTime, hyperspeedMultiplier);

					
				}
			}
		}
		//speed malus when shooting
		if ((InputGuy::isFiring() || this->ship_config.automatic_fire))
		{
			if (!disable_fire && (isCollindingWithInteractiveObject == No_Interaction) && !isHyperspeeding)
			{
				if (!this->isBraking)
				{
					speed.x *= SHIP_BRAKING_MALUS_SPEED;
					speed.y *= SHIP_BRAKING_MALUS_SPEED;
				}
				this->isBraking = true;
			}
		}
	}
}

void Ship::ManageInputs(sf::Time deltaTime, float hyperspeedMultiplier, sf::Vector2f inputs_direction)
{
	if (!disable_inputs)
	{
		moving = inputs_direction.x != 0 || inputs_direction.y != 0;
		movingX = inputs_direction.x != 0;
		movingY = inputs_direction.y != 0;

		if (!isFocusedOnHud)
		{
			if ((*CurrentGame).GetShopMenu() == ShopMainMenu)
			{
				ManageOpeningHud();

				ManageAcceleration(inputs_direction);

				ManageSlowMotion();

				ManageHyperspeed();

				ManageInteractions(inputs_direction);

				ManageFiring(deltaTime, hyperspeedMultiplier);

				ManageBraking();
			}
			else if ((*CurrentGame).GetShopMenu() == ShopBuyMenu)
			{
				ManageInteractions(inputs_direction);
			}
		}
		//HUD controls
		else
		{
			ManageOpeningHud();

			ManageHudControls(inputs_direction);
		}

		TestingInputsRelease();

		IdleDecelleration(deltaTime);
	}
	else
	{
		isFocusedOnHud = false;
	}
}

void Ship::ManageSlowMotion()
{
	//Slow_motion function
	if (InputGuy::isSlowMotion() && !disable_fire && (isCollindingWithInteractiveObject == No_Interaction))
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
}

void Ship::ManageHyperspeed()
{
	this->isHyperspeeding = false;
	if (InputGuy::isHyperspeeding() && !this->disabledHyperspeed && !this->isHyperspeeding && !this->isBraking &&!this->isSlowMotion && (isCollindingWithInteractiveObject == No_Interaction))
	{
		this->isHyperspeeding = true;
		(*CurrentGame).hyperspeedMultiplier = this->ship_config.getShipConfigHyperspeed();
	}
	else if (!this->isSlowMotion)
	{
		(*CurrentGame).hyperspeedMultiplier = 1.0f;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	speed.x += inputs_direction.x*ship_config.getShipConfigAcceleration().x;
	speed.y += inputs_direction.y*ship_config.getShipConfigAcceleration().y;

	//max speed constraints
	if (abs(speed.x) > this->ship_config.getShipConfigMaxSpeed().x)
	{
		speed.x = speed.x > 0 ? this->ship_config.getShipConfigMaxSpeed().x : -this->ship_config.getShipConfigMaxSpeed().x;
	}
	if (abs(speed.y) > this->ship_config.getShipConfigMaxSpeed().y)
	{
		speed.y = speed.y > 0 ? this->ship_config.getShipConfigMaxSpeed().y : -this->ship_config.getShipConfigMaxSpeed().y;
	}
}

void Ship::ManageBraking()
{
	//Braking function
	this->isBraking = false;
	if (InputGuy::isBraking() && !this->isBraking && !this->isHyperspeeding && (isCollindingWithInteractiveObject == No_Interaction))
	{
		speed.x *= SHIP_BRAKING_MALUS_SPEED;
		speed.y *= SHIP_BRAKING_MALUS_SPEED;
		this->isBraking = true;
	}
}

void Ship::ManageImmunity()
{
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
}

void Ship::ManageFeedbackExpiration(sf::Time deltaTime)
{
	//damage feedback expires?
	if (ship_config.ship_model->hasFake)
	{
		assert(ship_config.m_fake_ship != NULL);
		if (ship_config.m_fake_ship->m_color_timer > sf::seconds(0))
		{
			ship_config.m_fake_ship->m_color_timer -= deltaTime;
			ship_config.m_fake_ship->setColor(ship_config.m_fake_ship->m_color);
			if (ship_config.m_fake_ship->m_color_timer < sf::seconds(0))
			{
				ship_config.m_fake_ship->setColor(Color(255, 255, 255, 255));
			}
		}
	}
	if (m_color_timer > sf::seconds(0))
	{
		m_color_timer -= deltaTime;
		setColor(m_color);
		if (m_color_timer < sf::seconds(0))
		{
			setColor(Color(255, 255, 255, 255));
		}
	}
}

void Ship::TestingInputsRelease()
{
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

	if (InputGuy::isHyperspeeding())
	{
		wasHyperspeedingButtonPressed = true;
	}
	else
	{
		wasHyperspeedingButtonPressed = false;
	}

	if (InputGuy::isSlowMotion())
	{
		slowmo_key_repeat = true;
	}
	else
	{
		slowmo_key_repeat = false;
	}
}

void Ship::ManageHudControls(sf::Vector2f inputs_directions)
{
	//movement
	Independant* l_cursor = (*CurrentGame).hud.hud_cursor;
	l_cursor->speed.x = inputs_directions.x * HUD_CURSOR_SPEED;
	l_cursor->speed.y = inputs_directions.y * HUD_CURSOR_SPEED;

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

	l_cursor = NULL;
}

void Ship::SettingTurnAnimations()
{
	//setting animation
	const sf::Vector2f f = (sf::Vector2f)Independant::getDirectionMultiplier((*CurrentGame).direction);
	const float x = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(this->speed.x * f.x, this->speed.y * f.y)).x;

	if (this->ship_config.ship_model->hasFake)
	{
		if (x > 0 && this->currentAnimationIndex != ShipAnimations::ShipTurningRight && !this->disable_inputs)
		{
			this->currentAnimationIndex = ShipAnimations::ShipTurningRight;
		}

		else if (x < 0 && this->currentAnimationIndex != ShipAnimations::ShipTurningLeft && !this->disable_inputs)
		{
			this->currentAnimationIndex = ShipAnimations::ShipTurningLeft;
		}

		else if ((x == 0 && this->currentAnimationIndex != ShipAnimations::ShipIdle) || this->disable_inputs)
		{
			this->currentAnimationIndex = ShipAnimations::ShipIdle;
		}
	}
}

void Ship::ScreenBorderContraints()
{
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
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle decceleration
	if (!movingX || isFocusedOnHud == true)
	{
		speed.x -= (speed.x) * deltaTime.asSeconds()*(ship_config.getShipConfigDecceleration() / 100);

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

void Ship::ManageInteractions(sf::Vector2f input_directions)
{
	//using portals and shops
	m_interactionType = No_Interaction;

	if (this->isCollindingWithInteractiveObject != No_Interaction)
	{
		if (this->isCollindingWithInteractiveObject == PortalInteraction)
		{
			assert(this->targetPortal != NULL);
			if (this->targetPortal->state == PortalOpen)
			{
				//Updating interaction panel informations
				(*CurrentGame).SetSelectedDirection(this->targetPortal->direction);
				assert(this->targetPortal->destination_name.compare("0") != 0);
				(*CurrentGame).SetSelectedDestination(this->targetPortal->display_name);
				//default value = max
				if (previouslyCollindingWithInteractiveObject != PortalInteraction)
				{
					(*CurrentGame).SetSelectedIndex(this->targetPortal->max_unlocked_hazard_level);
				}

				//interaction: select
				if (InputGuy::isFiring() && !isFiringButtonPressed)
				{
					if (this->targetPortal->currentAnimationIndex == (int)(PortalAnimation::PortalOpenIdle))
					{
						this->m_interactionType = PortalInteraction;
					}
					isFiringButtonPressed = true;
				}
			}
			else
			{
				this->isCollindingWithInteractiveObject = No_Interaction;
			}
		}
		else if (this->isCollindingWithInteractiveObject == ShopInteraction)
		{
			assert(this->targetShop != NULL);

			(*CurrentGame).SetSelectedDestination(this->targetShop->display_name);
			//default value = first choice
			if (previouslyCollindingWithInteractiveObject != ShopInteraction)
			{
				(*CurrentGame).SetSelectedIndex(0);
			}

			switch ((*CurrentGame).GetShopMenu())
			{
				case ShopMainMenu:
				{
					//interaction: select
					if (InputGuy::isFiring() && !isFiringButtonPressed)
					{
						this->m_interactionType = ShopInteraction;
						isFiringButtonPressed = true;
						switch ((*CurrentGame).m_interactionPanel->m_selected_index)
						{
						case ShopHeal:
							ResplenishHealth();
							break;
						case ShopBuy:
							(*CurrentGame).SetShopMenu(ShopBuyMenu);
							(*CurrentGame).m_interactionPanel->InitCursorOnGrid();
							break;
						}
					}
					break;
				}
				case ShopBuyMenu:
				{
					//cursor control
					//movement
					Independant* l_cursor = (*CurrentGame).m_interactionPanel->m_cursor;
					l_cursor->speed.x = input_directions.x * HUD_CURSOR_SPEED;
					l_cursor->speed.y = input_directions.y * HUD_CURSOR_SPEED;

					//exit
					if (InputGuy::isSlowMotion() && !slowmo_key_repeat)
					{
						(*CurrentGame).SetShopMenu(ShopMainMenu);
					}
					break;
				}
			}
		}

		//controls up and down selected object
		if (this->isCollindingWithInteractiveObject == PortalInteraction && this->m_interactionType != PortalInteraction)
		{
			//interaction: decreasing
			if (InputGuy::isHyperspeeding() && !wasHyperspeedingButtonPressed)
			{
				if ((*CurrentGame).GetSelectedIndex() > 0)
				{
					(*CurrentGame).SetSelectedIndex((*CurrentGame).GetSelectedIndex() - 1);
				}
			}
			//interaction: increasing
			else if (InputGuy::isBraking() && !wasBrakingButtonPressed)
			{
				if ((*CurrentGame).GetSelectedIndex() < this->targetPortal->max_unlocked_hazard_level)
				{
					(*CurrentGame).SetSelectedIndex((*CurrentGame).GetSelectedIndex() + 1);
				}
			}
		}
		else if (this->isCollindingWithInteractiveObject == ShopInteraction  && this->m_interactionType != ShopInteraction)
		{
			//interaction: decreasing
			if (InputGuy::isHyperspeeding() && !wasHyperspeedingButtonPressed)
			{
				if ((*CurrentGame).GetSelectedIndex() > 0)
				{
					(*CurrentGame).SetSelectedIndex((*CurrentGame).GetSelectedIndex() - 1);
				}
			}
			//interaction: increasing
			else if (InputGuy::isBraking() && !wasBrakingButtonPressed)
			{
				if ((*CurrentGame).GetSelectedIndex() < NBVAL_ShopOptions - 1)
				{
					(*CurrentGame).SetSelectedIndex((*CurrentGame).GetSelectedIndex() + 1);
				}
			}
		}
	}
}

bool Ship::ResplenishHealth()
{
	bool hasHealthToResplenish = false;
	if (this->armor < this->armor_max)
	{
		this->armor = this->armor_max;
		hasHealthToResplenish = true;
	}
	if (this->shield < this->shield_max)
	{
		this->shield = this->shield_max;
		hasHealthToResplenish = true;
	}
	return hasHealthToResplenish;
}

void Ship::Respawn()
{
	armor = ship_config.getShipConfigArmor();
	shield = ship_config.getShipConfigShield();
	speed.x = 0;
	speed.y = 0;
	this->visible = true;
	if (this->ship_config.ship_model->hasFake)
	{
		this->ship_config.m_fake_ship->visible = true;
	}
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

	this->visible = false;
	if (this->ship_config.m_fake_ship != NULL)
	{
		this->ship_config.m_fake_ship->visible = false;
	}
	(*CurrentGame).garbageLayer(AuraLayer);

	//losing xp
	int death_xp_penalty_ = floor(this->xp_max * XP_DEATH_MALUS_PERCENTAGE);
	if (this->xp < death_xp_penalty_)
	{
		this->xp = 0;
	}
	else
	{
		this->xp -= death_xp_penalty_;
	}
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
	this->isCollindingWithInteractiveObject = PortalInteraction;
}

void Ship::GetShop(Independant* independant)
{
	this->targetShop = (Shop*)(independant);
	this->isCollindingWithInteractiveObject = ShopInteraction;
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
					Aura* combo_aura = this->m_combo_aura[GRAZE_LEVEL_RED]->Clone();
					(*CurrentGame).addToScene(combo_aura, AuraLayer, Neutral);
					break;
				}
				case GRAZE_LEVEL_BLUE:
				{
					(*CurrentGame).garbageLayer(AuraLayer);
					Aura* combo_aura = this->m_combo_aura[GRAZE_LEVEL_BLUE]->Clone();
					(*CurrentGame).addToScene(combo_aura, AuraLayer, Neutral);
					break;
				}
				case GRAZE_LEVEL_WHITE:
				{
					(*CurrentGame).garbageLayer(AuraLayer);
					Aura* combo_aura = this->m_combo_aura[GRAZE_LEVEL_WHITE]->Clone();
					(*CurrentGame).addToScene(combo_aura, AuraLayer, Neutral);
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
		if (ship_config.ship_model->hasFake)
		{
			assert(ship_config.m_fake_ship != NULL);
			ship_config.m_fake_ship->setColor(Color(255, 0, 0, 255), sf::seconds(DAMAGE_FEEDBACK_TIME));
		}
		this->setColor(Color(255, 0, 0, 255), sf::seconds(DAMAGE_FEEDBACK_TIME));

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


int Ship::GetFocusedPortalMaxUnlockedHazardLevel()
{
	if (this->targetPortal == NULL)
	{
		return -1;
	}
	else
	{
		return this->targetPortal->max_unlocked_hazard_level;
	}
}

void Ship::gain_xp(int xp_earned_)
{
	this->xp += xp_earned_;

	//no level down
	if (this->xp < 0)
	{
		this->xp = 0;
	}

	while (this->xp >= this->xp_max && this->level < this->level_max)
	{
		this->LevelUp();
	}

	//max level reached?
	if (this->xp > this->xp_max && this->level >= this->level_max)
	{
		this->xp = this->xp_max;
	}
}

void Ship::LevelUp()
{
	this->level++;
	this->xp -= this->xp_max;
	this->xp_max = floor(this->xp_max * (1 + XP_MAX_INCREASE_PER_LEVEL));

	//updating ship stats
	this->ship_base_stat_multiplier = (*CurrentGame).GetPlayerStatsMultiplierForLevel(this->level);

	//increasing enemies level
	(*CurrentGame).level = this->level;
	(*CurrentGame).enemy_base_stat_multiplier = (*CurrentGame).GetEnemiesStatsMultiplierForLevel((*CurrentGame).level);

	//increasing loot level
	(*CurrentGame).loot_on_par_stat_multiplier = (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel((*CurrentGame).level);
	
	//applying level modifiers
	this->Init();
	(*CurrentGame).ApplyLevelModifiers();
}