#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;


// ----------------SHIP MODEL ---------------
ShipModel::ShipModel(float m_max_speed, float m_acceleration, float m_deceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int m_damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name)
{
	max_speed = m_max_speed;
	deceleration = m_deceleration;
	acceleration = m_acceleration;
	armor = m_armor;
	shield = m_shield;
	shield_regen = m_shield_regen;
	damage = m_damage;
	textureName = m_textureName;
	size = m_size;
	frameNumber = m_frameNumber;
	display_name = m_display_name;
	bot = NULL;
	hasFake = false;
	hyperspeed = m_hyperspeed;
}

// ----------------EQUIPMENT ---------------

Equipment::Equipment()
{
	max_speed = 0.0f;
	deceleration = 0.0f;
	acceleration = 0.0f;
	armor = 0;
	shield = 0;
	shield_regen = 0;
	damage = 0;
	hyperspeed = 0;
	size.x = EQUIPMENT_SIZE;
	size.y = EQUIPMENT_SIZE;
	textureName = EMPTYSLOT_FILENAME;
	frameNumber = 0;
	equipmentType = EquipmentType::Armor;
	bot = NULL;
	hasFake = false;
}

Equipment::~Equipment()
{
	if (bot)
	{
		delete bot;
		bot = NULL;
	}

	level = 1;
	credits = 0;
}

Equipment* Equipment::Clone()
{
	Equipment* new_equipment = new Equipment();
	new_equipment->Init(this->equipmentType, this->max_speed, this->acceleration, this->deceleration, this->hyperspeed, this->armor, this->shield, this->shield_regen, this->damage, this->textureName, this->size, this->frameNumber, this->display_name);
	new_equipment->display_name = this->display_name;
	if (this->bot)
	{
		new_equipment->bot = this->bot->Clone();
	}
	new_equipment->level = this->level;
	new_equipment->credits = this->credits;

	return new_equipment;
}

void Equipment::Init(int m_equipmentType, float m_max_speed, float m_acceleration, float m_deceleration, float m_hyperspeed, int m_armor, int m_shield, int m_shield_regen, int m_damage, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber, std::string m_display_name)
{
	max_speed = m_max_speed;
	deceleration = m_deceleration;
	acceleration = m_acceleration;
	hyperspeed = m_hyperspeed;
	armor = m_armor;
	shield = m_shield;
	shield_regen = m_shield_regen;
	damage = damage;
	size.x = m_size.x;
	size.y = m_size.y;
	textureName = m_textureName;
	frameNumber = m_frameNumber;
	display_name = m_display_name;
	equipmentType = m_equipmentType;
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
	Weapon* weapon = Weapon::CreateRandomWeapon(floor(credits_ * BOT_STATS_MULTIPLIER), level, true);

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

float Ship::getFighterFloatStatValue(FighterStats stat)
{
	float new_stat_value = 0;
	float equipment_value = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (equipment[i])
		{
			switch (stat)
			{
				case Fighter_Hyperspeed:
				{
					equipment_value += equipment[i]->hyperspeed;
					break;
				}
				case Fighter_MaxSpeed:
				{
					equipment_value += equipment[i]->max_speed;
					break;
				}
				case Fighter_Acceleration:
				{
					equipment_value += equipment[i]->acceleration;
					break;
				}
				case Fighter_Deceleration:
				{
					equipment_value += equipment[i]->deceleration;
					break;
				}
				default:
				{
					equipment_value += 0;
				}
			}
		}
		else
		{
			equipment_value += 0;
		}
	}

	//adding ship model stats
	switch (stat)
	{
		case Fighter_Hyperspeed:
		{
			new_stat_value = ship_model->hyperspeed + equipment_value;
			break;
		}
		case Fighter_MaxSpeed:
		{
			new_stat_value = ship_model->max_speed + equipment_value;
			break;
		}
		case Fighter_Acceleration:
		{
			new_stat_value = ship_model->acceleration + equipment_value;
			break;
		}
		case Fighter_Deceleration:
		{
			new_stat_value = ship_model->deceleration + equipment_value;
			break;
		}
		default:
		{
			new_stat_value = equipment_value;
		}
	}

	return new_stat_value;
}

int Ship::getFighterIntStatValue(FighterStats stat)
{
	int new_stat_value = 0;
	int equipment_value = 0;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (equipment[i])
		{
			switch (stat)
			{
				case Fighter_ArmorMax:
				{
					equipment_value += equipment[i]->armor;
					break;
				}
				case Fighter_ShieldMax:
				{
					equipment_value += equipment[i]->shield;
					break;
				}
				case Fighter_ShieldRegen:
				{
					equipment_value += equipment[i]->shield_regen;
					break;
				}
				case Fighter_ContactDamage:
				{
					equipment_value += equipment[i]->damage;
					break;
				}
				case Fighter_Credits:
				{
					equipment_value += equipment[i]->credits;
					break;
				}
				case Fighter_Level:
				{
					equipment_value += equipment[i]->level;
					break;
				}
				default:
				{
					equipment_value += 0;
				}
			}
		}
		else
		{
			equipment_value += 0;
		}
	}

	//adding ship model stats
	switch (stat)
	{
		case Fighter_ArmorMax:
		{
			new_stat_value = ship_model->armor + equipment_value;
			break;
		}
		case Fighter_ShieldMax:
		{
			new_stat_value = ship_model->shield + equipment_value;
			break;
		}
		case Fighter_ShieldRegen:
		{
			new_stat_value = ship_model->shield_regen + equipment_value;
			break;
		}
		case Fighter_ContactDamage:
		{
			new_stat_value = ship_model->damage + equipment_value;
			break;
		}
		case Fighter_Credits:
		{
			new_stat_value = equipment_value;
			break;
		}
		case Fighter_Level:
		{
			new_stat_value = equipment_value;
			break;
		}
		default:
		{
			new_stat_value = equipment_value;
		}
	}

	return new_stat_value;
}

void Ship::GenerateBots(GameObject* m_target)
{
	for (std::vector<Bot*>::iterator it = (this->bot_list.begin()); it != (this->bot_list.end()); it++)
	{
		Bot* m_bot = (*it)->Clone();
		m_bot->automatic_fire = this->automatic_fire;
		m_bot->spread = GameObject::getSize_for_Direction((*CurrentGame).direction, m_bot->spread);
		m_bot->setTarget(m_target);
		m_bot->rotate(GameObject::getRotation_for_Direction((*CurrentGame).direction));
		(*CurrentGame).addToScene(m_bot, LayerType::BotLayer, GameObjectType::Neutral);
	}
}

void Ship::DestroyBots()
{
	this->bot_list.clear();
	(*CurrentGame).garbageLayer(LayerType::BotLayer);
}

void Ship::GenerateFakeShip(GameObject* m_target)
{
	assert(this->ship_model != NULL);
	if (this->ship_model->hasFake)
	{
		m_fake_ship = new FakeShip(m_target, this->ship_model->fake_textureName, this->ship_model->fake_size, this->ship_model->fake_frameNumber, ShipAnimations::NB_ShipAnimations);
		(*CurrentGame).addToScene(m_fake_ship, LayerType::FakeShipLayer, GameObjectType::FakePlayerShip);
	}
}

// ----------------SHIP ---------------

Ship::Ship(ShipModel* ship_model) : GameObject(Vector2f(0, 0), Vector2f(0, 0), ship_model->textureName, ship_model->size, Vector2f((ship_model->size.x / 2), (ship_model->size.y / 2)), ship_model->frameNumber)
{
	this->ship_model = ship_model;

	automatic_fire = false;

	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		equipment[i] = NULL;
	}
	weapon = NULL;

	collider_type = GameObjectType::PlayerShip;
	moving = false;
	movingX = movingY = false;
	visible = true;
	damage = 0;
	hyperspeed = 1.0f;
	armor = 1;
	shield = 0;
	shield_regen = 0;
	disable_inputs = false;
	disable_fire = false;
	isBraking = false;
	isHyperspeeding = false;
	isSlowMotion = false;
	disabledHyperspeed = false;
	graze_count = 0;
	graze_level = 0;

	level = 1;
	level_max = FIRST_LEVEL_MAX;
	xp = 0;
	xp_max = XP_MAX_FIRST_LEVEL;

	m_combo_aura[GrazeLevels::GRAZE_LEVEL_RED] = new Aura(this, "Assets/2D/FX/Aura_RedGlow.png", sf::Vector2f(150, 150), 3);
	m_combo_aura[GrazeLevels::GRAZE_LEVEL_BLUE] = new Aura(this, "Assets/2D/FX/Aura_BlueGlow.png", sf::Vector2f(150, 150), 3);
	m_combo_aura[GrazeLevels::GRAZE_LEVEL_WHITE] = new Aura(this, "Assets/2D/FX/Aura_WhiteGlow.png", sf::Vector2f(150, 150), 3);
	trail = new Aura(this, "Assets/2D/FX/Aura_HyperspeedTrail.png", sf::Vector2f(70, 34), 3, 1);
	trail->visible = false;
	if (ship_model->hasFake)
	{
		trail->offset = sf::Vector2f(0, (ship_model->fake_size.y / 2) + (this->trail->m_size.y / 2));
	}
	else
	{
		trail->offset = sf::Vector2f(0, (ship_model->size.y / 2) + (this->trail->m_size.y / 2));
	}
	(*CurrentGame).addToScene(this->trail, LayerType::FakeShipLayer, GameObjectType::Neutral);
	fire_key_repeat = false;
	slowmo_key_repeat = false;
	hud_key_repeat = false;
	m_interactionType = No_Interaction;
	isFiringButtonPressed = true;//will be updated to false in the update function if button released
	wasBrakingButtonPressed = true;
	isBrakingButtonHeldPressed = false;
	wasHyperspeedingButtonPressed = true;
	targetPortal = NULL;
	targetShop = NULL;
	equipment_loot = NULL;
	weapon_loot = NULL;
	isFocusedOnHud = false;
	previously_focused_item = NULL;

	previouslyCollidingWithInteractiveObject = No_Interaction;
	isCollidingWithInteractiveObject = No_Interaction;

	Init();
}

void Ship::Init()
{
	UpdateShipLevel();

	max_speed = getFighterFloatStatValue(Fighter_MaxSpeed);
	deceleration = getFighterFloatStatValue(Fighter_Deceleration);
	acceleration = getFighterFloatStatValue(Fighter_Acceleration);
	hyperspeed = getFighterFloatStatValue(Fighter_Hyperspeed);
	armor_max = getFighterIntStatValue(Fighter_ArmorMax);
	if (armor > armor_max)
	{
		armor = armor_max;
	}
	shield_max = getFighterIntStatValue(Fighter_ShieldMax);
	if (shield > shield_max)
	{
		shield = shield_max;
	}
	shield_regen = getFighterIntStatValue(Fighter_ShieldRegen);
	damage = getFighterIntStatValue(Fighter_ContactDamage);
	m_size = ship_model->size;
	textureName = ship_model->textureName;
	frameNumber = ship_model->frameNumber;

	//fake texture
	transparent = ship_model->hasFake;
	m_ship_size = ship_model->hasFake ? ship_model->fake_size : m_size;

	//Loading bots
	bot_list.clear();
	if (ship_model->bot)
	{
		bot_list.push_back(ship_model->bot);
	}
	for (int i = 0; i < EquipmentType::NBVAL_Equipment; i++)
	{
		if (equipment[i] != NULL)
		{
			if (equipment[i]->bot)
			{
				bot_list.push_back(equipment[i]->bot);
			}
		}
	}
}

bool Ship::setEquipment(Equipment* equipment, bool overwrite_existing, bool no_save)
{
	assert(equipment != NULL);

	if (!overwrite_existing && this->equipment[equipment->equipmentType])
	{
		return false;
	}

	if (overwrite_existing && this->equipment[equipment->equipmentType] && this->equipment[equipment->equipmentType]->bot)
	{
		DestroyBots();
	}

	if (overwrite_existing && this->equipment[equipment->equipmentType])
	{
		delete this->equipment[equipment->equipmentType];
	}

	this->equipment[equipment->equipmentType] = equipment;
	
	Init();

	GenerateBots(this);
	
	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);

	return true;
}

bool Ship::setShipWeapon(Weapon* weapon, bool overwrite_existing, bool no_save)
{
	assert(weapon != NULL);

	if (!overwrite_existing && weapon)
	{
		return false;
	}

	if (overwrite_existing && weapon)
	{
		delete this->weapon;
	}
		
	this->weapon = weapon;
		
	this->Init();

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);
	
	return true;
}

void Ship::cleanEquipment(int equipment_type, bool no_save)
{
	if (equipment[equipment_type])
	{
		if (equipment[equipment_type]->bot)
		{
			DestroyBots();
		}
		delete equipment[equipment_type];
		equipment[equipment_type] = NULL;

		this->Init();
	}

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);
}

void Ship::cleanWeapon(bool no_save)
{
	if (weapon != NULL)
	{
		delete weapon;
		weapon = NULL;

		Init();
	}

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);
}

void Ship::setShipModel(ShipModel* ship_model, bool no_save)
{
	assert(ship_model != NULL);

	if (ship_model->bot)
	{
		DestroyBots();
	}

	this->ship_model = ship_model;
	this->Init();

	if (ship_model->bot)
	{
		GenerateBots(this);
	}

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE);
}

void Ship::ManageDebugCommand()
{
	if (InputGuy::isUsingDebugCommand())
	{
		(*CurrentGame).killGameObjectLayer(EnemyObject);
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

		this->trail->visible = (hyperspeedMultiplier > 1.0f);

		GameObject::update(deltaTime, hyperspeedMultiplier);

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
	if (m_fake_ship)
	{
		if (m_fake_ship->visible)
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
			automatic_fire = !automatic_fire;
			fire_key_repeat = true;
		}
	}
	else
	{
		this->fire_key_repeat = false;
	}

	//Fire function
	if (weapon)
	{
		if (weapon->isFiringReady(deltaTime, hyperspeedMultiplier))
		{
			if (!disable_fire && (isCollidingWithInteractiveObject == No_Interaction) && !isHyperspeeding)
			{
				if ((InputGuy::isFiring() || automatic_fire))
				{
					//calculating the angle we want to face, if any
					float target_angle = getRotation();
					if (weapon->target_seaking != NO_SEAKING || (weapon->target_seaking == SEMI_SEAKING && weapon->rafale_index == 0))
					{
						target_angle = fmod(GameObject::getRotation_for_Direction((*CurrentGame).direction) - (*CurrentGame).GetAngleToNearestGameObject(GameObjectType::EnemyObject, getPosition()), 360);
					}

					float current_angle = getRotation();
					float delta = current_angle - target_angle;
					if (delta > 180)
						delta -= 360;
					else if (delta < -180)
						delta += 360;

					//float theta = (this->getRotation() - delta) / 180 * M_PI;
					float theta = getRotation() / 180 * M_PI;
					if (weapon->target_seaking != NO_SEAKING)
					{
						theta -= delta / 180 * M_PI;
					}

					if (weapon->target_seaking == SEMI_SEAKING && weapon->rafale_index > 0 && weapon->rafale_index < weapon->rafale)
					{
						//semi-seaking and rafale not ended = no update of target or weapon position
					}
					else
					{
						weapon->weapon_current_offset.x = weapon->weaponOffset.x + m_ship_size.x / 2 * sin(theta);
						weapon->weapon_current_offset.y = weapon->weaponOffset.y - m_ship_size.y / 2 * cos(theta);

						//transmitting the angle to the weapon, which will pass it to the bullets
						weapon->shot_angle = theta;
					}

					weapon->setPosition(getPosition().x + weapon->weapon_current_offset.x, getPosition().y + weapon->weapon_current_offset.y);
					weapon->Fire(FriendlyFire, deltaTime, hyperspeedMultiplier);

					
				}
			}
		}
		//speed malus when shooting
		if ((InputGuy::isFiring() || automatic_fire))
		{
			if (!disable_fire && (isCollidingWithInteractiveObject == No_Interaction) && !isHyperspeeding)
			{
				if (!isBraking)
				{
					speed.x *= SHIP_BRAKING_MALUS_SPEED;
					speed.y *= SHIP_BRAKING_MALUS_SPEED;
				}
				isBraking = true;
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
	if (InputGuy::isSlowMotion() && !disable_fire && (isCollidingWithInteractiveObject == No_Interaction))
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
	if (InputGuy::isHyperspeeding() && !this->disabledHyperspeed && !this->isHyperspeeding && !this->isBraking &&!this->isSlowMotion && (isCollidingWithInteractiveObject == No_Interaction))
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
	speed.x += inputs_direction.x*getFighterFloatStatValue(Fighter_Acceleration);
	speed.y += inputs_direction.y*getFighterFloatStatValue(Fighter_Acceleration);

	//max speed constraints
	GameObject::NormalizeSpeed(&speed, getFighterFloatStatValue(Fighter_MaxSpeed));
}

void Ship::ManageBraking()
{
	//Braking function
	isBraking = false;
	if (InputGuy::isBraking() && !isBraking && !isHyperspeeding && (isCollidingWithInteractiveObject == No_Interaction))
	{
		speed.x *= SHIP_BRAKING_MALUS_SPEED;
		speed.y *= SHIP_BRAKING_MALUS_SPEED;
		isBraking = true;
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
	if (ship_model->hasFake)
	{
		assert(m_fake_ship != NULL);

		if (m_fake_ship->m_color_timer > sf::seconds(0))
		{
			m_fake_ship->m_color_timer -= deltaTime;
			m_fake_ship->setColor(m_fake_ship->m_color);
			if (m_fake_ship->m_color_timer < sf::seconds(0))
			{
				m_fake_ship->setColor(Color(255, 255, 255, 255));
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
	GameObject* l_cursor = (*CurrentGame).hud.hud_cursor;
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
						GameObject* tmp_ptr = (*CurrentGame).getHudFocusedItem();
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
	const sf::Vector2f f = (sf::Vector2f)GameObject::getDirectionMultiplier((*CurrentGame).direction);
	const float x = GameObject::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(speed.x * f.x, speed.y * f.y)).x;

	if (ship_model->hasFake)
	{
		if (x > 0 && currentAnimationIndex != ShipAnimations::ShipTurningRight && !disable_inputs)
		{
			currentAnimationIndex = ShipAnimations::ShipTurningRight;
		}

		else if (x < 0 && currentAnimationIndex != ShipAnimations::ShipTurningLeft && !disable_inputs)
		{
			currentAnimationIndex = ShipAnimations::ShipTurningLeft;
		}

		else if ((x == 0 && currentAnimationIndex != ShipAnimations::ShipIdle) || disable_inputs)
		{
			currentAnimationIndex = ShipAnimations::ShipIdle;
		}
	}
}

void Ship::ScreenBorderContraints()
{
	//screen borders contraints	correction
	if (this->getPosition().x < m_ship_size.x / 2)
	{
		this->setPosition(m_ship_size.x / 2, this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().x > SCENE_SIZE_X - (m_ship_size.x / 2))
	{
		this->setPosition(SCENE_SIZE_X - (m_ship_size.x / 2), this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().y < m_ship_size.y / 2)
	{
		this->setPosition(this->getPosition().x, m_ship_size.y / 2);
		speed.y = 0;
	}

	if (this->getPosition().y > SCENE_SIZE_Y - (m_ship_size.y / 2))
	{
		this->setPosition(this->getPosition().x, SCENE_SIZE_Y - (m_ship_size.y / 2));
		speed.y = 0;
	}
	
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle deceleration
	if (!movingX || isFocusedOnHud == true)
	{
		speed.x -= (speed.x) * deltaTime.asSeconds()*(getFighterFloatStatValue(Fighter_Deceleration) / 100);

		if (abs(speed.x) < SHIP_MIN_SPEED_X)
			speed.x = 0;
	}

	if (!movingY || isFocusedOnHud == true)
	{
		speed.y -= (speed.y)*deltaTime.asSeconds()*(getFighterFloatStatValue(Fighter_Deceleration) / 100);

		if (abs(speed.y) < SHIP_MIN_SPEED_Y)
			speed.y = 0;
	}
}

void Ship::ManageInteractions(sf::Vector2f input_directions)
{
	//using portals and shops
	m_interactionType = No_Interaction;

	if (this->isCollidingWithInteractiveObject != No_Interaction)
	{
		if (this->isCollidingWithInteractiveObject == PortalInteraction)
		{
			assert(this->targetPortal != NULL);
			if (this->targetPortal->state == PortalOpen)
			{
				//Updating interaction panel informations
				(*CurrentGame).SetSelectedDirection(this->targetPortal->direction);
				assert(this->targetPortal->destination_name.compare("0") != 0);
				(*CurrentGame).SetSelectedDestination(this->targetPortal->display_name);
				//default value = max
				if (previouslyCollidingWithInteractiveObject != PortalInteraction)
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
				this->isCollidingWithInteractiveObject = No_Interaction;
			}
		}
		else if (this->isCollidingWithInteractiveObject == ShopInteraction)
		{
			assert(this->targetShop != NULL);

			(*CurrentGame).SetSelectedDestination(this->targetShop->display_name);
			//default value = first choice
			if (previouslyCollidingWithInteractiveObject != ShopInteraction)
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
					GameObject* l_cursor = (*CurrentGame).m_interactionPanel->m_cursor;
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
		if (this->isCollidingWithInteractiveObject == PortalInteraction && this->m_interactionType != PortalInteraction)
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
		else if (this->isCollidingWithInteractiveObject == ShopInteraction  && this->m_interactionType != ShopInteraction)
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
	armor = getFighterIntStatValue(Fighter_ArmorMax);
	shield = getFighterIntStatValue(Fighter_ShieldMax);
	speed.x = 0;
	speed.y = 0;
	this->visible = true;
	if (ship_model->hasFake)
	{
		m_fake_ship->visible = true;
	}
	isOnScene = true;
	sf::Vector2f pos = sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO);
	pos = GameObject::getPosition_for_Direction((*CurrentGame).direction, pos);
	this->setPosition(pos);

	immune = true;
	immunityTimer.restart();
}

void Ship::Death()
{
	FX* myFX = FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	(*CurrentGame).addToScene(myFX, LayerType::ExplosionLayer, GameObjectType::Neutral);

	this->visible = false;
	if (m_fake_ship != NULL)
	{
		m_fake_ship->visible = false;
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

GameObject* Ship::CloneEquipmentIntoGameObject(Equipment* new_equipment)
{
	assert(new_equipment != NULL);
	GameObject* capsule = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), new_equipment->textureName, new_equipment->size, sf::Vector2f(new_equipment->size.x / 2, new_equipment->size.y / 2), new_equipment->frameNumber);
	capsule->setEquipmentLoot(new_equipment->Clone());

	return capsule;
}

GameObject* Ship::CloneWeaponIntoGameObject(Weapon* new_weapon)
{
	assert(new_weapon != NULL);
	GameObject* capsule = new GameObject(new_weapon->getPosition(), sf::Vector2f(0, 0), new_weapon->textureName, new_weapon->size, sf::Vector2f(new_weapon->size.x / 2, new_weapon->size.y / 2), new_weapon->frameNumber);
	capsule->setWeaponLoot(new_weapon->Clone());

	return capsule;
}

bool Ship::GetLoot(GameObject& object)
{
	//EQUIPMENT
	if (object.getEquipmentLoot() != NULL)
	{
		GameObject* capsule = CloneEquipmentIntoGameObject(object.getEquipmentLoot());
		if (this->setEquipment(object.getEquipmentLoot()))
		{
			//if the ship config does not have any equipment of this type on, we equip it and update the HUD
			(*CurrentGame).InsertObjectInShipGrid(*capsule, object.getEquipmentLoot()->equipmentType);
		}
		else
		{
			//...else we put it in the stash
			(*CurrentGame).InsertObjectInEquipmentGrid(*capsule);
		}
		//object.releaseEquipmentLoot();
		//object.releaseWeaponLoot();
		return true;
	}

	//WEAPON
	if (object.getWeaponLoot() != NULL)
	{
		GameObject* capsule = CloneWeaponIntoGameObject(object.getWeaponLoot());
		if (this->setShipWeapon(object.getWeaponLoot()))
		{
			//if the ship config does not have a weapon already, we equip it and update the HUD
			(*CurrentGame).InsertObjectInShipGrid(*capsule, NBVAL_Equipment);
		}
		else
		{
			//...else we put it in the stash
			(*CurrentGame).InsertObjectInEquipmentGrid(*capsule);
		}
		//object.releaseEquipmentLoot();
		//object.releaseWeaponLoot();
		return true;
	}

	//MONEY
	return this->get_money_from(object);
}

void Ship::GetPortal(GameObject* object)
{
	this->targetPortal = (Portal*)(object);
	this->isCollidingWithInteractiveObject = PortalInteraction;
}

void Ship::GetShop(GameObject* object)
{
	this->targetShop = (Shop*)(object);
	this->isCollidingWithInteractiveObject = ShopInteraction;
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

void Ship::damage_from(GameObject& object)
{
	if (!immune)
	{
		if (ship_model->hasFake)
		{
			assert(m_fake_ship != NULL);
			m_fake_ship->setColor(Color(255, 0, 0, 255), sf::seconds(DAMAGE_FEEDBACK_TIME));
		}
		this->setColor(Color(255, 0, 0, 255), sf::seconds(DAMAGE_FEEDBACK_TIME));

		if (object.damage > shield)
		{
			armor -= (object.damage - shield);
			shield = 0;
		}
		else
		{
			shield -= object.damage;
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
		if (equipment[i])
		{
			credits_ += equipment[i]->credits;
		}
	}
	if (weapon)
	{
		credits_ += weapon->credits;
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
		data << equipment->max_speed << " ";
		data << equipment->deceleration << " ";
		data << equipment->acceleration << " ";
		data.precision(3);
		data << equipment->hyperspeed << " ";
		data.precision(0);
		data << equipment->armor << " ";
		data << equipment->shield << " ";
		data << equipment->shield_regen << " ";
		data << equipment->damage << " ";

		if (equipment->bot)
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

			Ship::SaveEquipmentData(data, (*CurrentGame).playerShip->equipment[i], true);
		}

		data << "Weapon ";
		Ship::SaveWeaponData(data, (*CurrentGame).playerShip->weapon, true);

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
	if (bot_name.compare("0") != 0)
	{
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
						GameObject* capsule = (*CurrentGame).playerShip->CloneWeaponIntoGameObject(weapon);
						(*CurrentGame).InsertObjectInEquipmentGrid(*capsule, index);
						delete weapon;
					}
					else
					{
						Equipment* equipment = Ship::LoadEquipmentFromLine(line);
						GameObject* capsule = (*CurrentGame).playerShip->CloneEquipmentIntoGameObject(equipment);
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

void Ship::updatePostCollision()
{
	previouslyCollidingWithInteractiveObject = isCollidingWithInteractiveObject;
	isCollidingWithInteractiveObject = No_Interaction;
}