#include "Equipment.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP MODEL ---------------
ShipModel::ShipModel(float max_speed, float acceleration, float deceleration, float hyperspeed, int armor, int shield, int shield_regen, float shield_recovery_time, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name)
{
	m_max_speed = max_speed;
	m_deceleration = deceleration;
	m_acceleration = acceleration;
	m_armor = armor;
	m_shield = shield;
	m_shield_regen = shield_regen;
	m_shield_recovery_time = shield_recovery_time;
	m_damage = damage;
	m_textureName = textureName;
	m_size = size;
	m_frameNumber = frameNumber;
	m_display_name = display_name;
	m_bot = NULL;
	m_hyperspeed = hyperspeed;
}

ShipModel::~ShipModel()
{
	m_bot = NULL;
}

// ----------------EQUIPMENT ---------------

Equipment::Equipment()
{
	m_max_speed = 0.0f;
	m_deceleration = 0.0f;
	m_acceleration = 0.0f;
	m_armor = 0;
	m_shield = 0;
	m_shield_regen = 0;
	m_damage = 0;
	m_hyperspeed = 0;
	m_size.x = EQUIPMENT_SIZE;
	m_size.y = EQUIPMENT_SIZE;
	m_textureName = EMPTYSLOT_FILENAME;
	m_frameNumber = 0;
	m_equipmentType = EquipmentType::Armor;
	m_bot = NULL;
}

Equipment::~Equipment()
{
	if (m_bot)
	{
		//delete m_bot; -> garbaged
		m_bot = NULL;
	}

	m_level = 1;
	m_credits = 0;
}

Equipment* Equipment::Clone()
{
	Equipment* new_equipment = new Equipment();
	new_equipment->Init(m_equipmentType, m_max_speed, m_acceleration, m_deceleration, m_hyperspeed, m_armor, m_shield, m_shield_regen, m_shield_recovery_time, m_damage, m_textureName, m_size, m_frameNumber, m_display_name);
	new_equipment->m_display_name = m_display_name;
	if (m_bot)
	{
		new_equipment->m_bot = m_bot->Clone();
	}
	new_equipment->m_level = m_level;
	new_equipment->m_credits = m_credits;

	return new_equipment;
}

void Equipment::Init(int equipmentType, float max_speed, float acceleration, float deceleration, float hyperspeed, int armor, int shield, int shield_regen, float shield_recovery_time, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name)
{
	m_max_speed = max_speed;
	m_deceleration = deceleration;
	m_acceleration = acceleration;
	m_hyperspeed = hyperspeed;
	m_armor = armor;
	m_shield = shield;
	m_shield_regen = shield_regen;
	m_shield_recovery_time = shield_recovery_time;
	m_damage = damage;
	m_size.x = size.x;
	m_size.y = size.y;
	m_textureName = textureName;
	m_frameNumber = frameNumber;
	m_display_name = display_name;
	m_equipmentType = equipmentType;
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
	equipment->Init((int)EquipmentType::Armor, 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0, ARMOR_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Armor");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->m_armor = FIRST_LEVEL_ARMOR * multiplier_;
	equipment->m_damage = FIRST_LEVEL_COLLISION_DAMAGE * multiplier_;

	//allocating bonuses to the weapon
	equipment->m_armor += bonus_armor * FIRST_LEVEL_ARMOR * 0.01;
	equipment->m_damage += bonus_damage * FIRST_LEVEL_COLLISION_DAMAGE * 0.01;

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return equipment;
}

Equipment* Equipment::CreateRandomShield(int credits_, int level)
{
	credits_ += credits_ == 0 ? LOOT_CREDITS_DEFAULT_BONUS : 0;

	//Spending credits on the possible bonuses
	int bonus_shield = 0;
	int bonus_shield_regen = 0;
	int bonus_shield_recovery = 0;

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
			case 1://shield regen
			{
				loot_credits_remaining--;
				bonus_shield_recovery++;
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
	equipment->Init((int)EquipmentType::Shield, 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0, SHIELD_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Shield");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->m_shield = FIRST_LEVEL_SHIELD * multiplier_;
	equipment->m_shield_regen = FIRST_LEVEL_SHIELD_REGEN * multiplier_;
	equipment->m_shield_recovery_time = 0;

	//allocating bonuses to the weapon
	equipment->m_shield += bonus_shield * FIRST_LEVEL_SHIELD * 0.01;
	equipment->m_shield_regen += ceil(bonus_shield_regen * FIRST_LEVEL_SHIELD_REGEN * 0.01);
	equipment->m_shield_regen += ceil(bonus_shield_regen * FIRST_LEVEL_SHIELD_REGEN * 0.01);
	equipment->m_shield_recovery_time -= bonus_shield_recovery * FIRST_LEVEL_SHIELD_RECOVERY_TIME * 0.01;

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return equipment;
}

Equipment* Equipment::CreateRandomEngine(int credits_, int level)
{
	credits_ += credits_ == 0 ? LOOT_CREDITS_DEFAULT_BONUS : 0;

	//Spending credits on the possible bonuses
	int bonus_hyperspeed = credits_;

	//Creating the item
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Engine, 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0, THRUSTER_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Engine");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->m_hyperspeed = FIRST_LEVEL_HYPERSPEED * multiplier_;

	//allocating bonuses to the weapon
	equipment->m_hyperspeed += bonus_hyperspeed * FIRST_LEVEL_HYPERSPEED * 0.01;

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return equipment;
}

Equipment* Equipment::CreateRandomModule(int credits_, int level)
{
	credits_ += credits_ == 0 ? LOOT_CREDITS_DEFAULT_BONUS : 0;

	//Spending credits on the possible bonuses
	Weapon* weapon = Weapon::CreateRandomWeapon(floor(credits_ * BOT_STATS_MULTIPLIER), level, true);

	//Initialisation
	Equipment* equipment = new Equipment();
	equipment->Init((int)EquipmentType::Module, 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0, MODULE_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Module");

	Bot* bot = new Bot(sf::Vector2f(0, 0), sf::Vector2f(0, 0), BOT_FILENAME, sf::Vector2f(BOT_SIZE, BOT_SIZE));
	bot->m_radius = 500;
	bot->m_vspeed = 300;
	bot->m_spread = sf::Vector2f(-50, 0);

	vector<float>* v = new vector<float>;
	//v->push_back(bot->radius); // rayon 500px
	//v->push_back(1);  // clockwise (>)
	PatternType pattern_type = PatternType::NoMovePattern;
	bot->m_Pattern.SetPattern(pattern_type, bot->m_vspeed, v); //vitesse angulaire (degres/s)

	bot->m_weapon = weapon;
	equipment->m_bot = bot;

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_ + ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);

	return equipment;
}

#define EQUIPMENT_DECCELERATION_MULTIPLIER		10
#define EQUIPMENT_ACCELERATION_MULTIPLIER		10
#define EQUIPMENT_MAXSPEED_MULTIPLIER			10
#define EQUIPMENT_ARMOR_MULTIPLIER				10
#define EQUIPMENT_SHIELD_MULTIPLIER				10
#define EQUIPMENT_SHIELD_REGEN_MULTIPLIER		10
#define EQUIPMENT_DAMAGE_MULTIPLIER				10