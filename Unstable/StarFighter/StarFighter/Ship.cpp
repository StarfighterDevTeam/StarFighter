#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;


// ----------------SHIP MODEL ---------------
ShipModel::ShipModel(float m_max_speed, float m_acceleration, float m_decceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int m_damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name)
{
	this->max_speed = m_max_speed;
	this->decceleration = m_decceleration;
	this->acceleration = m_acceleration;
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
float ShipModel::getShipModelMaxSpeed()
{
	return this->max_speed;
}

float ShipModel::getShipModelAcceleration()
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
	this->max_speed = 0.0f;
	this->decceleration = 0.0f;
	this->acceleration = 0.0f;
	this->armor = 0;
	this->shield = 0;
	this->shield_regen = 0;
	this->damage = 0;
	this->hyperspeed = 0;
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

	level = 1;
	credits = 0;
}

Equipment* Equipment::Clone()
{
	Equipment* new_equipment = new Equipment();
	new_equipment->Init(this->equipmentType, this->max_speed, this->acceleration, this->decceleration, this->hyperspeed, this->armor, this->shield, this->shield_regen, this->damage, this->textureName, this->size, this->frameNumber, this->display_name);
	new_equipment->display_name = this->display_name;
	new_equipment->hasBot = this->hasBot;
	if (this->hasBot)
	{
		new_equipment->bot = this->bot->Clone();
	}
	new_equipment->level = this->level;
	new_equipment->credits = this->credits;

	return new_equipment;
}

void Equipment::Init(int m_equipmentType, float m_max_speed, float m_acceleration, float m_decceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int m_damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name)
{
	this->max_speed = m_max_speed;
	this->decceleration = m_decceleration;
	this->acceleration = m_acceleration;
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

float Equipment::getEquipmentMaxSpeed()
{
	return this->max_speed;
}

float Equipment::getEquipmentDecceleration()
{
	return this->decceleration;
}

float Equipment::getEquipmentAcceleration()
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

Equipment* Equipment::CreateRandomArmor(int credits_, int level)
{
	credits_ += credits_ == 0 ? LOOT_CREDITS_DEFAULT_BONUS : 0;

	//Spending credits on the possible bonuses
	int bonus_armor = 0;
	int bonus_damage = 0;

	int loot_credits_remaining = credits_;
	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = RandomizeIntBetweenValues(0, 5);

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus)
		{
			case 0://damage
			{
				loot_credits_remaining--;
				bonus_damage++;
				break;
			}

			default://armor
			{
				loot_credits_remaining--;
				bonus_armor++;
				break;
			}
		}
	}

	//Creating the item
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Armor, 0, 0, 0.f, 0.f, 0, 0, 0, 0, ARMOR_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Armor");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->armor = FIRST_LEVEL_ARMOR * multiplier_;
	equipment->damage = FIRST_LEVEL_COLLISION_DAMAGE * multiplier_;

	//allocating bonuses to the weapon
	equipment->armor += bonus_armor * FIRST_LEVEL_ARMOR * 0.01;
	equipment->damage += bonus_damage * FIRST_LEVEL_COLLISION_DAMAGE * 0.01;

	//saving level and credits used
	equipment->level = level;
	equipment->credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return equipment;
}

Equipment* Equipment::CreateRandomShield(int credits_, int level)
{
	credits_ += credits_ == 0 ? LOOT_CREDITS_DEFAULT_BONUS : 0;

	//Spending credits on the possible bonuses
	int bonus_shield = 0;
	int bonus_shield_regen = 0;

	int loot_credits_remaining = credits_;

	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = RandomizeIntBetweenValues(0, 3);

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus)
		{
			case 0://shield regen
			{
				loot_credits_remaining--;
				bonus_shield_regen++;
				break;
			}

			default://shield
			{
				loot_credits_remaining--;
				bonus_shield++;
				break;
			}
		}
	}

	//Creating the item
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Shield, 0, 0, 0.f, 0.f, 0, 0, 0, 0, SHIELD_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Shield");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->shield = FIRST_LEVEL_SHIELD * multiplier_;
	equipment->shield_regen = FIRST_LEVEL_SHIELD_REGEN * multiplier_;

	//allocating bonuses to the weapon
	equipment->shield += bonus_shield * FIRST_LEVEL_SHIELD * 0.01;
	equipment->shield_regen += ceil(bonus_shield_regen * FIRST_LEVEL_SHIELD_REGEN * 0.01);

	//saving level and credits used
	equipment->level = level;
	equipment->credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return equipment;
}

Equipment* Equipment::CreateRandomEngine(int credits_, int level)
{
	credits_ += credits_ == 0 ? LOOT_CREDITS_DEFAULT_BONUS : 0;

	//Spending credits on the possible bonuses
	int bonus_hyperspeed = credits_;

	//Creating the item
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Engine, 0, 0, 0.f, 0.f, 0, 0, 0, 0, THRUSTER_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Engine");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->hyperspeed = FIRST_LEVEL_HYPERSPEED * multiplier_;

	//allocating bonuses to the weapon
	equipment->hyperspeed += bonus_hyperspeed * FIRST_LEVEL_HYPERSPEED * 0.01;

	//saving level and credits used
	equipment->level = level;
	equipment->credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return equipment;
}

Equipment* Equipment::CreateRandomModule(int credits_, int level)
{
	credits_ += credits_ == 0 ? LOOT_CREDITS_DEFAULT_BONUS : 0;

	//Spending credits on the possible bonuses
	Weapon* weapon = Weapon::CreateRandomWeapon(floor(credits_ * BOT_STATS_MULTIPLIER), level);

	//Initialisation
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Module, 0, 0, 0.f, 0.f, 0, 0, 0, 0, MODULE_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Module");

	Bot* bot = new Bot(sf::Vector2f(0, 0), sf::Vector2f(0, 0), BOT_FILENAME, sf::Vector2f(BOT_SIZE, BOT_SIZE));
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
	equipment->hasBot = true;

	//saving level and credits used
	equipment->level = level;
	equipment->credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return equipment;
}

#define EQUIPMENT_DECCELERATION_MULTIPLIER		10
#define EQUIPMENT_ACCELERATION_MULTIPLIER		10
#define EQUIPMENT_MAXSPEED_MULTIPLIER			10
#define EQUIPMENT_ARMOR_MULTIPLIER				10
#define EQUIPMENT_SHIELD_MULTIPLIER				10
#define EQUIPMENT_SHIELD_REGEN_MULTIPLIER		10
#define EQUIPMENT_DAMAGE_MULTIPLIER				10

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

float ShipConfig::getShipConfigMaxSpeed()
{
	float new_max_speed = 0;
	float equipment_max_speed = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_max_speed += equipment[i]->getEquipmentMaxSpeed();
		}
		else
		{
			equipment_max_speed += 0;
		}
	}

	new_max_speed = ship_model->getShipModelMaxSpeed() + equipment_max_speed;

	//cancelling negative equipment values
	if (new_max_speed < ship_model->getShipModelMaxSpeed())
		new_max_speed = ship_model->getShipModelMaxSpeed();

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

float ShipConfig::getShipConfigAcceleration()
{
	float new_acceleration = 0;
	float equipment_acceleration = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (this->equipment[i] != NULL)
		{
			equipment_acceleration += equipment[i]->getEquipmentAcceleration();
		}
		else
		{
			equipment_acceleration += 0;
		}
	}

	new_acceleration = ship_model->getShipModelAcceleration() + equipment_acceleration;

	//cancelling negative equipment values
	if (new_acceleration < ship_model->getShipModelAcceleration())
		new_acceleration = ship_model->getShipModelAcceleration();

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
	this->hyperspeed = 1.0f;
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

	this->Init();
}

void Ship::Init()
{
	this->armor_max = this->ship_config.getShipConfigArmor();
	if (this->armor > this->armor_max)
	{
		this->armor = this->armor_max;
	}
	this->shield_max = this->ship_config.getShipConfigShield();
	if (this->shield > this->shield_max)
	{
		this->shield = this->shield_max;
	}
	this->shield_regen = this->ship_config.getShipConfigShieldRegen();
	this->damage = this->ship_config.getShipConfigDamage();
	this->hyperspeed = this->ship_config.getShipConfigHyperspeed();

	this->m_size = this->ship_config.ship_model->size;
	this->textureName = this->ship_config.ship_model->textureName;
	this->transparent = this->ship_config.ship_model->hasFake;

	UpdateShipLevel();
}

void Ship::setShipConfig(ShipConfig m_ship_config)
{
	this->ship_config = m_ship_config;
	this->Init();
}

bool Ship::setEquipment(Equipment* m_equipment, bool overwrite_existing, bool no_save)
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
	
	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);

	return result;
}

bool Ship::setShipWeapon(Weapon* m_weapon, bool overwrite_existing, bool no_save)
{
	//this->ship_config.DestroyBots();
	bool result = this->ship_config.setShipWeapon(m_weapon, true, overwrite_existing);
	if (result)
	{
		this->Init();
	}
	//this->ship_config.GenerateBots(this);

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);
	
	return result;
}

void Ship::cleanEquipment(int equipment_type, bool no_save)
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

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);
}

void Ship::cleanWeapon(bool no_save)
{
	if (this->ship_config.weapon != NULL)
	{
		this->ship_config.weapon->~Weapon();
		delete this->ship_config.weapon;
		this->ship_config.weapon = NULL;

		this->ship_config.Init();
		this->Init();
	}

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);
}

void Ship::setShipModel(ShipModel* m_ship_model, bool no_save)
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

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);
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
				(*CurrentGame).hyperspeedMultiplier = 1.0f / this->hyperspeed;
				printf("");
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
	if (shield < shield_max)
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
		if (shield > shield_max)
		{
			shield = shield_max;
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
				(*CurrentGame).hyperspeedMultiplier = 1.0f / this->hyperspeed;
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
		(*CurrentGame).hyperspeedMultiplier = this->hyperspeed;
	}
	else if (!this->isSlowMotion)
	{
		(*CurrentGame).hyperspeedMultiplier = 1.0f;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	speed.x += inputs_direction.x*ship_config.getShipConfigAcceleration();
	speed.y += inputs_direction.y*ship_config.getShipConfigAcceleration();

	//max speed constraints
	Independant::NormalizeSpeed(&this->speed, this->ship_config.getShipConfigMaxSpeed());
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
								this->cleanWeapon(true);
							}
							else
							{
								this->cleanEquipment(equip_type, true);
							}
						}
						//Save items
						SaveItems(ITEMS_SAVE_FILE);

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
	//int death_xp_penalty_ = floor(this->xp_max * XP_DEATH_MALUS_PERCENTAGE);
	//if (this->xp < death_xp_penalty_)
	//{
	//	this->xp = 0;
	//}
	//else
	//{
	//	this->xp -= death_xp_penalty_;
	//}
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

//void Ship::gain_xp(int xp_earned_)
//{
//	this->xp += xp_earned_;
//
//	//no level down
//	if (this->xp < 0)
//	{
//		this->xp = 0;
//	}
//
//	while (this->xp >= this->xp_max && this->level < this->level_max)
//	{
//		this->LevelUp();
//	}
//
//	//max level reached?
//	if (this->xp > this->xp_max && this->level >= this->level_max)
//	{
//		this->xp = this->xp_max;
//	}
//}

//void Ship::LevelUp()
//{
//	this->level++;
//	this->xp -= this->xp_max;
//	this->xp_max = floor(this->xp_max * (1 + XP_MAX_INCREASE_PER_LEVEL));
//}

int Ship::UpdateShipLevel()
{
	int level_ = 1;
	int credits_ = 0;

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (ship_config.equipment[i])
		{
			credits_ += ship_config.equipment[i]->credits;
		}
	}
	if (ship_config.weapon)
	{
		credits_ += ship_config.weapon->credits;
	}

	while (credits_ >= ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1))
	{
		credits_ -= ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1);
		level_++;
	}

	this->level = level_;
	this->xp = credits_;
	this->xp_max = ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1);

	return level_;
}

//SAVING AND LOADING ITEMS
void Ship::SaveEquipmentData(ofstream& data, Equipment* equipment, bool skip_type)
{
	if (equipment)
	{
		if (!skip_type)
		{
			switch (equipment->equipmentType)
			{
			case Engine:
			{
						   data << "Engine ";
						   break;
			}
			case Armor:
			{
						  data << "Armor ";
						  break;
			}
			case Shield:
			{
						   data << "Shield ";
						   break;
			}
			case Module:
			{
						   data << "Module ";
						   break;
			}
			default:
			{
					   data << "Unknown ";
					   break;
			}
			}
		}

		data << equipment->display_name << " ";
		data << equipment->level << " ";
		data << equipment->credits << " ";
		data << equipment->textureName << " ";
		data << equipment->size.x << " ";
		data << equipment->size.y << " ";
		data << equipment->frameNumber << " ";
		data << equipment->getEquipmentMaxSpeed() << " ";
		data << equipment->getEquipmentDecceleration() << " ";
		data << equipment->getEquipmentAcceleration() << " ";
		data.precision(3);
		data << equipment->getEquipmentHyperspeed() << " ";
		data.precision(0);
		data << equipment->getEquipmentArmor() << " ";
		data << equipment->getEquipmentShield() << " ";
		data << equipment->getEquipmentShieldRegen() << " ";
		data << equipment->getEquipmentDamage() << " ";

		if (equipment->hasBot)
		{
			data << equipment->bot->display_name << " ";
			data << equipment->bot->textureName << " ";
			data << equipment->bot->m_size.x << " ";
			data << equipment->bot->m_size.y << " ";
			data << equipment->bot->frameNumber << " ";
			data << equipment->bot->spread.x << " ";
			data << equipment->bot->spread.y << " ";
			data << equipment->bot->rotation_speed << " ";
			data << equipment->bot->Pattern.currentPattern << " ";
			switch (equipment->bot->Pattern.currentPattern)
			{
			case NoMovePattern:
			{
								  break;
			}
			case Line_:
			{
						  data << equipment->bot->Pattern.patternSpeed << " ";
						  data << &equipment->bot->Pattern.patternParams[1] << " ";
						  break;
			}
			default:
			{
					   data << equipment->bot->Pattern.patternSpeed << " ";
					   data << &equipment->bot->Pattern.patternParams[0] << " ";
					   data << &equipment->bot->Pattern.patternParams[1] << " ";
					   break;
			}
			}

			if (equipment->bot->weapon)
			{
				Ship::SaveWeaponData(data, equipment->bot->weapon, true, true);
				return;
			}
			else
			{
				data << "0";
			}
		}
		else
		{
			data << "0";
		}
	}
	else
	{
		data << "0";
	}

	data << endl;
}

void Ship::SaveWeaponData(ofstream& data, Weapon* weapon, bool skip_type, bool skip_level)
{
	if (!skip_type)
	{
		data << "Weapon ";
	}

	if (weapon)
	{
		data << weapon->display_name << " ";
		if (!skip_level)
		{
			data << weapon->level << " ";
			data << weapon->credits << " ";
		}
		data << weapon->textureName << " ";
		data << weapon->size.x << " ";
		data << weapon->size.y << " ";
		data << weapon->frameNumber << " ";
		data.precision(3);
		data << weapon->rate_of_fire << " ";
		data.precision(0);
		data << weapon->rafale << " ";
		data.precision(3);
		data << weapon->rafale_cooldown << " ";
		data.precision(0);

		data << weapon->multishot << " ";
		data << weapon->xspread << " ";
		data << weapon->dispersion << " ";
		data << (int)weapon->shot_mode << " ";
		data << (int)weapon->target_seaking << " ";

		data << weapon->ammunition->display_name << " ";
		data << weapon->ammunition->damage << " ";
		data << weapon->ammunition->speed.y << " ";
		data << weapon->ammunition->range << " ";
		data << weapon->ammunition->textureName << " ";
		data << weapon->ammunition->m_size.x << " ";
		data << weapon->ammunition->m_size.y << " ";
		data << weapon->ammunition->frameNumber << " ";
		data << weapon->ammunition->explosion->display_name << " ";
		data << (int)weapon->ammunition->Pattern.currentPattern;
		if (weapon->ammunition->Pattern.currentPattern == Line_)
		{
			data << " " << weapon->ammunition->Pattern.patternSpeed << " ";
			data << &weapon->ammunition->Pattern.patternParams[1];
		}
		else if (weapon->ammunition->Pattern.currentPattern > NoMovePattern)
		{
			data << " " << weapon->ammunition->Pattern.patternSpeed << " ";
			data << &weapon->ammunition->Pattern.patternParams[0] << " ";
			data << &weapon->ammunition->Pattern.patternParams[1];
		}
	}
	else
	{
		data << "0";
	}

	data << endl;
}

int Ship::SaveItems(string file)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, "Saving items in profile.\n");

	ofstream data(file.c_str(), ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		// instructions
		for (int i = 0; i < NBVAL_Equipment; i++)
		{
			switch (i)
			{
				case Engine:
				{
					data << "Engine ";
					break;
				}
				case Armor:
				{
					data << "Armor ";
					break;
				}
				case Shield:
				{
					data << "Shield ";
					break;
				}
				case Module:
				{
					data << "Module ";
					break;
				}
				default:
				{
					data << "Unknown ";
					break;
				}
			}

			Ship::SaveEquipmentData(data, (*CurrentGame).playerShip->ship_config.equipment[i], true);
		}

		data << "Weapon ";
		Ship::SaveWeaponData(data, (*CurrentGame).playerShip->ship_config.weapon, true);

		for (size_t i = 0; i < EQUIPMENT_GRID_NB_LINES; i++)
		{
			for (size_t j = 0; j < EQUIPMENT_GRID_NB_ROWS; j++)
			{
				if ((*CurrentGame).hud.equipmentGrid.grid[i][j])
				{
					if ((*CurrentGame).hud.equipmentGrid.grid[i][j]->equipment_loot)
					{
						Ship::SaveEquipmentData(data, (*CurrentGame).hud.equipmentGrid.grid[i][j]->equipment_loot, false);
					}
					else if ((*CurrentGame).hud.equipmentGrid.grid[i][j]->weapon_loot)
					{
						Ship::SaveWeaponData(data, (*CurrentGame).hud.equipmentGrid.grid[i][j]->weapon_loot, false);
					}
					else
					{
						data << "0";
						data << endl;
					}
				}
				else
				{
					data << "0";
					data << endl;
				}
			}
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "Failed to open PLAYER SAVE FILE !" << endl;
	}

	return 0;
}

Equipment* Ship::LoadEquipmentFromLine(string line)
{
	string equipment_type;
	string display_name;

	std::istringstream ss(line);
	ss >> equipment_type >> display_name;
	//ss.str(line.substr(ss.tellg()));

	Equipment* equipment = new Equipment();

	string texture_name;
	int level;
	int credits;
	int width;
	int height;
	int frames;
	float max_speed;
	float deceleration;
	float acceleration;
	float hyperspeed;
	int armor;
	int shield;
	int shield_regen;
	int damage;

	string bot_name;
	string bot_texture_name;
	int bot_width;
	int bot_height;
	int bot_frames;
	float bot_spread_x;
	float bot_spread_y;
	float bot_rotation_speed;
	float bot_pattern_speed;
	int bot_pattern_type;
	float bot_pattern_arg1;
	float bot_pattern_arg2;

	string bot_weapon_name;
	string bot_weapon_texture_name;
	int bot_weapon_width;
	int bot_weapon_height;
	int bot_weapon_frames;
	float bot_weapon_rate_of_fire;
	int bot_weapon_rafale;
	float bot_rafale_cooldown;
	int bot_weapon_multishot;
	float bot_weapon_xspread;
	float bot_weapon_dispersion;
	int bot_weapon_shot_mode;
	int bot_weapon_target_seaking;

	string bot_ammo_name;
	int bot_ammo_damage;
	float bot_ammo_speed;
	float bot_ammo_range;
	string bot_ammo_texture_name;
	int bot_ammo_width;
	int bot_ammo_height;
	int bot_ammo_frames;
	int bot_ammo_pattern_type;
	float bot_ammo_pattern_speed;
	float bot_ammo_pattern_arg1;
	float bot_ammo_pattern_arg2;
	string bot_ammo_explosion_name;

	if (display_name.compare("0") == 0)
	{
		//do nothing
	}
	else
	{
		ss >> level >> credits >> texture_name >> width >> height >> frames >> max_speed >> deceleration >> acceleration >> hyperspeed >> armor >> shield >> shield_regen >> damage >> bot_name;

		if (bot_name.compare("0") == 0)
		{
			//do nothing
		}
		else
		{
			ss >> bot_texture_name >> bot_width >> bot_height >> bot_frames >> bot_spread_x >> bot_spread_y >> bot_rotation_speed >> bot_pattern_type;
			if (bot_pattern_type == Line_)
			{
				ss >> bot_pattern_speed >> bot_pattern_arg2;
			}
			else if (bot_pattern_type > NoMovePattern)
			{
				ss >> bot_pattern_speed >> bot_pattern_arg1 >> bot_pattern_arg2;
			}

			ss >> bot_weapon_name;
			if (bot_weapon_name.compare("0") == 0)
			{
				//do nothing
			}
			else
			{
				ss >> bot_weapon_texture_name >> bot_weapon_width >> bot_weapon_height >> bot_weapon_frames >> bot_weapon_rate_of_fire >>
					bot_weapon_rafale >> bot_rafale_cooldown >> bot_weapon_multishot >> bot_weapon_xspread >> bot_weapon_dispersion >> bot_weapon_shot_mode
					>> bot_weapon_target_seaking;

				ss >> bot_ammo_name >> bot_ammo_damage >> bot_ammo_speed >> bot_ammo_range >> bot_ammo_texture_name >> bot_ammo_width >> bot_ammo_height >>
					bot_ammo_frames >> bot_ammo_explosion_name >> bot_ammo_pattern_type;

				if (bot_ammo_pattern_type == Line_)
				{
					ss >> bot_ammo_pattern_speed >> bot_ammo_pattern_arg2;
				}
				else if (bot_ammo_pattern_type >> NoMovePattern)
				{
					ss >> bot_ammo_pattern_speed >> bot_ammo_pattern_arg1 >> bot_ammo_pattern_arg2;
				}
			}
		}
	}

	EquipmentType type = NBVAL_Equipment;
	if (equipment_type.compare("Shield") == 0)
	{
		type = Shield;
	}
	else if (equipment_type.compare("Armor") == 0)
	{
		type = Armor;
	}
	else if (equipment_type.compare("Engine") == 0)
	{
		type = Engine;
	}
	else if (equipment_type.compare("Module") == 0)
	{
		type = Module;
	}

	equipment->Init(type, max_speed, acceleration, deceleration, hyperspeed, armor, shield, shield_regen, damage, texture_name, sf::Vector2f(width, height), frames, display_name);
	equipment->level = level;
	equipment->credits = credits;
	if (bot_name.compare("0") == 0)
	{
		equipment->hasBot = false;
	}
	else
	{
		equipment->hasBot = true;
		Bot* bot = new Bot(Vector2f(0, 0), Vector2f(0, 0), bot_texture_name, sf::Vector2f(bot_width, bot_height));

		bot->display_name = bot_name;
		bot->spread = sf::Vector2f(bot_spread_x, bot_spread_y);
		bot->rotation_speed = bot_rotation_speed;
		bot->Pattern.currentPattern = (PatternType)bot_pattern_type;
		if (bot->Pattern.currentPattern == Line_)
		{
			bot->Pattern.patternSpeed = bot_pattern_speed;
			bot->Pattern.patternParams->push_back(bot_pattern_arg2);
		}
		else if (bot->Pattern.currentPattern > NoMovePattern)
		{
			bot->Pattern.patternSpeed = bot_pattern_speed;
			bot->Pattern.patternParams->push_back(bot_pattern_arg1);
			bot->Pattern.patternParams->push_back(bot_pattern_arg2);
		}

		if (bot_weapon_name.compare("0") == 0)
		{
			//do nothing
		}
		else
		{
			Ammo* ammo = new Ammo(Vector2f(0, 0), sf::Vector2f(0, bot_ammo_speed), bot_ammo_texture_name, sf::Vector2f(bot_ammo_width, bot_ammo_height), bot_ammo_damage, Enemy::LoadFX(bot_ammo_explosion_name));
			ammo->display_name = bot_ammo_name;
			ammo->range = bot_ammo_range;
			ammo->Pattern.currentPattern = (PatternType)bot_ammo_pattern_type;
			if (ammo->Pattern.currentPattern == Line_)
			{
				ammo->Pattern.patternSpeed = bot_ammo_pattern_speed;
				ammo->Pattern.patternParams->push_back(bot_ammo_pattern_arg2);
			}
			else if (ammo->Pattern.currentPattern > NoMovePattern)
			{
				ammo->Pattern.patternSpeed = bot_ammo_pattern_speed;
				ammo->Pattern.patternParams->push_back(bot_ammo_pattern_arg1);
				ammo->Pattern.patternParams->push_back(bot_ammo_pattern_arg2);
			}

			Weapon* weapon = new Weapon(ammo);
			weapon->display_name = bot_weapon_name;
			weapon->fire_direction = Vector2i(0, -1);
			weapon->rate_of_fire = bot_weapon_rate_of_fire;
			weapon->shot_mode = (ShotMode)bot_weapon_shot_mode;

			weapon->multishot = bot_weapon_multishot;
			weapon->xspread = bot_weapon_xspread;
			weapon->dispersion = bot_weapon_dispersion;
			weapon->rafale = bot_weapon_rafale;
			weapon->rafale_cooldown = bot_rafale_cooldown;

			weapon->textureName = bot_weapon_texture_name;
			weapon->size = sf::Vector2f(bot_weapon_width, bot_weapon_height);
			weapon->frameNumber = bot_weapon_frames;
			weapon->target_seaking = (TargetSeaking)bot_weapon_target_seaking;

			bot->weapon = weapon;
		}

		equipment->bot = bot;
	}

	return equipment;
}

Weapon* Ship::LoadWeaponFromLine(string line)
{
	string equipment_type;
	string display_name;

	//std::istringstream(line) >> equipment_type >> display_name;

	std::istringstream ss(line);
	ss >> equipment_type >> display_name;

	Equipment* equipment = new Equipment();

	string weapon_texture_name;
	int weapon_level;
	int weapon_credits;
	int weapon_width;
	int weapon_height;
	int weapon_frames;
	float weapon_rate_of_fire;
	int weapon_rafale;
	float rafale_cooldown;
	int weapon_multishot;
	float weapon_xspread;
	float weapon_dispersion;
	int weapon_shot_mode;
	int weapon_target_seaking;

	string ammo_name;
	int ammo_damage;
	float ammo_speed;
	float ammo_range;
	string ammo_texture_name;
	int ammo_width;
	int ammo_height;
	int ammo_frames;
	int ammo_pattern_type;
	float ammo_pattern_speed;
	float ammo_pattern_arg1;
	float ammo_pattern_arg2;
	string ammo_explosion_name;

	ss >> weapon_level >> weapon_credits >> weapon_texture_name >> weapon_width >> weapon_height >> weapon_frames >> weapon_rate_of_fire >>
		weapon_rafale >> rafale_cooldown >> weapon_multishot >> weapon_xspread >> weapon_dispersion >> weapon_shot_mode
		>> weapon_target_seaking >> ammo_name >> ammo_damage >> ammo_speed >> ammo_range >> ammo_texture_name >> ammo_width >> ammo_height >>
		ammo_frames >> ammo_explosion_name >> ammo_pattern_type;
	if (ammo_pattern_type == Line_)
	{
		ss >> ammo_pattern_speed >> ammo_pattern_arg2;
	}
	else if (ammo_pattern_type > NoMovePattern)
	{
		ss >> ammo_pattern_speed >> ammo_pattern_arg1 >> ammo_pattern_arg2;
	}

	EquipmentType type = NBVAL_Equipment;

	Ammo* ammo = new Ammo(Vector2f(0, 0), sf::Vector2f(0, ammo_speed), ammo_texture_name, sf::Vector2f(ammo_width, ammo_height), ammo_damage, Enemy::LoadFX(ammo_explosion_name));
	ammo->display_name = ammo_name;
	ammo->range = ammo_range;
	ammo->Pattern.currentPattern = (PatternType)ammo_pattern_type;
	if (ammo->Pattern.currentPattern == Line_)
	{
		ammo->Pattern.patternSpeed = ammo_pattern_speed;
		ammo->Pattern.patternParams->push_back(ammo_pattern_arg2);
	}
	else if (ammo->Pattern.currentPattern > NoMovePattern)
	{
		ammo->Pattern.patternSpeed = ammo_pattern_speed;
		ammo->Pattern.patternParams->push_back(ammo_pattern_arg1);
		ammo->Pattern.patternParams->push_back(ammo_pattern_arg2);
	}

	Weapon* weapon = new Weapon(ammo);
	weapon->display_name = display_name;
	weapon->level = weapon_level;
	weapon->credits = weapon_credits;
	weapon->fire_direction = Vector2i(0, -1);
	weapon->rate_of_fire = weapon_rate_of_fire;
	weapon->shot_mode = (ShotMode)weapon_shot_mode;

	weapon->multishot = weapon_multishot;
	weapon->xspread = weapon_xspread;
	weapon->dispersion = weapon_dispersion;
	weapon->rafale = weapon_rafale;
	weapon->rafale_cooldown = rafale_cooldown;

	weapon->textureName = weapon_texture_name;
	weapon->size = sf::Vector2f(weapon_width, weapon_height);
	weapon->frameNumber = weapon_frames;
	weapon->target_seaking = (TargetSeaking)weapon_target_seaking;

	return weapon;
}

bool Ship::LoadPlayerItems(string file)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading items from profile.\n");

	std::ifstream  data(file, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		int i = 0;
		while (std::getline(data, line))
		{
			string equipment_type;
			string display_name;

			//Loading equipment
			if (i < NBVAL_Equipment)
			{
				std::istringstream(line) >> equipment_type >> display_name;
				if (display_name.compare("0") != 0)
				{
					(*CurrentGame).playerShip->setEquipment(Ship::LoadEquipmentFromLine(line), true, true);
				}
			}
			//Loading weapon
			else if (i == NBVAL_Equipment)
			{
				std::istringstream(line) >> equipment_type >> display_name;
				if (display_name.compare("0") != 0)
				{
					(*CurrentGame).playerShip->setShipWeapon(Ship::LoadWeaponFromLine(line), true, true);
				}
			}
			//Loading stash
			else //if (i > NBVAL_Equipment)
			{
				std::istringstream(line) >> equipment_type;

				int index = i - NBVAL_Equipment - 1;
				int r = index % (*CurrentGame).hud.equipmentGrid.squares.y;
				int l = index / (*CurrentGame).hud.equipmentGrid.squares.y;

				if ((*CurrentGame).hud.equipmentGrid.grid[l][r])
				{
					(*CurrentGame).GarbageObjectInGrid(HudGrid_ShipGrid, index);
				}

				if (equipment_type.compare("0") != 0)
				{
					if ((*CurrentGame).hud.equipmentGrid.grid[l][r])
					{
						delete (*CurrentGame).hud.equipmentGrid.grid[l][r];
					}
					if (equipment_type.compare("Weapon") == 0)
					{
						Weapon* weapon = Ship::LoadWeaponFromLine(line);
						Independant* capsule = (*CurrentGame).playerShip->CloneWeaponIntoIndependant(weapon);
						(*CurrentGame).InsertObjectInEquipmentGrid(*capsule, index);
						delete weapon;
					}
					else
					{
						Equipment* equipment = Ship::LoadEquipmentFromLine(line);
						Independant* capsule = (*CurrentGame).playerShip->CloneEquipmentIntoIndependant(equipment);
						(*CurrentGame).InsertObjectInEquipmentGrid(*capsule, index);
						delete equipment;
					}
				}
			}

			i++;
		}

		data.close();  // on ferme le fichier

		return i == NBVAL_Equipment + 1 + EQUIPMENT_GRID_NB_LINES*EQUIPMENT_GRID_NB_ROWS;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "Failed to open ITEMS SAVE FILE !" << endl;
		return false;
	}
}