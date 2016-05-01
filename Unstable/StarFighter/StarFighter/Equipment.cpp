#include "Equipment.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP MODEL ---------------
ShipModel::ShipModel(float max_speed, float acceleration, float deceleration, float hyperspeed, int hyperspeed_fuel, int armor, int shield, int shield_regen, float shield_recovery_time, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name)
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
	m_hyperspeed_fuel = hyperspeed_fuel;
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
	m_equipmentType = Armor;
	m_level = 1;
	m_credits = 0;
	m_quality = 0;
}

Equipment::~Equipment()
{
	m_bots.clear();

	m_level = 1;
	m_credits = 0;
}

Equipment* Equipment::Clone()
{
	Equipment* new_equipment = new Equipment();
	new_equipment->Init(m_equipmentType, m_max_speed, m_acceleration, m_deceleration, m_hyperspeed, m_hyperspeed_fuel, m_armor, m_shield, m_shield_regen, m_shield_recovery_time, m_damage, m_textureName, m_size, m_frameNumber, m_display_name);
	new_equipment->m_display_name = m_display_name;
	if (!m_bots.empty())
	{
		size_t botsVectorSize = m_bots.size();
		for (size_t i = 0; i < botsVectorSize; i++)
		{
			new_equipment->m_bots.push_back(m_bots[i]->Clone());
		}
	}
	new_equipment->m_level = m_level;
	new_equipment->m_credits = m_credits;
	new_equipment->m_quality = m_quality;

	return new_equipment;
}

void Equipment::Init(int equipmentType, float max_speed, float acceleration, float deceleration, float hyperspeed, int hyperspeed_fuel, int armor, int shield, int shield_regen, float shield_recovery_time, int damage, std::string textureName, sf::Vector2f size, int frameNumber, std::string display_name)
{
	m_max_speed = max_speed;
	m_deceleration = deceleration;
	m_acceleration = acceleration;
	m_hyperspeed = hyperspeed;
	m_hyperspeed_fuel = hyperspeed_fuel;
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

Equipment* Equipment::CreateRandomArmor(int level, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

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
	equipment->Init((int)Armor, 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0, 0, ARMOR_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Armor");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->m_armor = FIRST_LEVEL_ARMOR * multiplier_;
	equipment->m_damage = FIRST_LEVEL_COLLISION_DAMAGE * multiplier_;

	//allocating bonuses to the weapon
	equipment->m_armor += bonus_armor * FIRST_LEVEL_ARMOR * 0.01;
	equipment->m_damage += bonus_damage * FIRST_LEVEL_COLLISION_DAMAGE * 0.01;

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_;
	equipment->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	return equipment;
}

Equipment* Equipment::CreateRandomShield(int level, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

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
	equipment->Init((int)Shield, 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0, 0, SHIELD_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Shield");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->m_shield = FIRST_LEVEL_SHIELD * multiplier_;
	equipment->m_shield_regen = FIRST_LEVEL_SHIELD_REGEN * multiplier_;
	equipment->m_shield_recovery_time = 0;

	//allocating bonuses to the weapon
	equipment->m_shield += bonus_shield * FIRST_LEVEL_SHIELD * 0.01;
	equipment->m_shield_regen += ceil(bonus_shield_regen * FIRST_LEVEL_SHIELD_REGEN * 0.01);
	equipment->m_shield_recovery_time = FIRST_LEVEL_SHIELD_RECOVERY_TIME / (1 + bonus_shield_recovery * 0.01);

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_;
	equipment->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	return equipment;
}

Equipment* Equipment::CreateRandomEngine(int level, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

	//Spending credits on the possible bonuses
	int loot_credits_remaining = credits_;

	int bonus_hyperspeed = 0;
	int bonus_fuel = 0;

	while (loot_credits_remaining > 0)
	{
		int random_type_of_bonus = RandomizeIntBetweenValues(0, 2);

		//spending the credits in the chosen bonus
		switch (random_type_of_bonus)
		{
			case 0://hyperspeed
			{
				loot_credits_remaining--;
				bonus_hyperspeed++;
				break;
			}
			default://fuel
			{
				loot_credits_remaining--;
				bonus_fuel++;
				break;
			}
		}
	}

	//Creating the item
	Equipment* equipment = new Equipment();
	equipment->Init((int)Engine, 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0, 0, THRUSTER_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Engine");

	//Base item stats
	float multiplier_ = (*CurrentGame).GetPlayerStatsMultiplierForLevel(level) * 0.01;
	equipment->m_hyperspeed = FIRST_LEVEL_HYPERSPEED * multiplier_;
	equipment->m_hyperspeed_fuel = FIRST_LEVEL_FUEL * multiplier_;

	//allocating bonuses to the weapon
	equipment->m_hyperspeed += bonus_hyperspeed * FIRST_LEVEL_HYPERSPEED * 0.01;
	equipment->m_hyperspeed_fuel += bonus_fuel * FIRST_LEVEL_FUEL * 0.01;

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_;
	equipment->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	return equipment;
}

Equipment* Equipment::CreateRandomModule(int level, float beastScore)
{
	int credits_ = ((*CurrentGame).GetPlayerStatsMultiplierForLevel(level) - 100);
	credits_ += ceil(beastScore / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(level + 1));

	int cost_per_bot = floor(CREDITS_COST_PER_ONE_ADDITIONAL_BOT * pow((1 + COST_PER_ONE_BOT_MULTIPLIER_PER_LEVEL), level - 1));

	//buying additional bots?
	int loot_credits_remaining = credits_;
	int number_of_bots = 0;
	bool can_buy_additional_bot = true;
	while (can_buy_additional_bot)
	{
		number_of_bots++;
		loot_credits_remaining -= number_of_bots == 0 ? 0 : cost_per_bot;
		can_buy_additional_bot = loot_credits_remaining > cost_per_bot && number_of_bots < MAX_NUMBER_OF_BOTS_PER_ITEM;
	}
	loot_credits_remaining = credits_ / number_of_bots;

	//Spending credits on the possible bonuses
	Weapon* weapon = Weapon::CreateRandomWeapon(level, true, beastScore);

	//Initialisation
	Equipment* equipment = new Equipment();
	equipment->Init((int)Module, 0, 0, 0.f, 0.f, 0, 0, 0, 0, 0, 0, MODULE_FILENAME, sf::Vector2f(EQUIPMENT_SIZE, EQUIPMENT_SIZE), 1, "Module");

	Bot* bot = new Bot(sf::Vector2f(0, 0), sf::Vector2f(0, 0), BOT_FILENAME, sf::Vector2f(BOT_SIZE, BOT_SIZE));
	bot->m_radius = 500;
	bot->m_vspeed = 300;
	bot->m_spread = sf::Vector2f(-50, 0);

	vector<float> patternParams;
	//v->push_back(bot->radius); // rayon 500px
	//v->push_back(1);  // clockwise (>)
	PatternType pattern_type = NoMovePattern;
	bot->m_Pattern.SetPattern(pattern_type, bot->m_vspeed, patternParams); //vitesse angulaire (degres/s)

	bot->m_weapon = weapon;

	for (int i = 0; i < number_of_bots; i++)
	{
		if (i == 0)
		{
			equipment->m_bots.push_back(bot);
		}
		else
		{
			equipment->m_bots.push_back(bot->Clone());
		}
	}

	//saving level and credits used
	equipment->m_level = level;
	equipment->m_credits = credits_;
	equipment->m_quality = beastScore * 100 / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES);

	return equipment;
}

#define EQUIPMENT_DECCELERATION_MULTIPLIER		10
#define EQUIPMENT_ACCELERATION_MULTIPLIER		10
#define EQUIPMENT_MAXSPEED_MULTIPLIER			10
#define EQUIPMENT_ARMOR_MULTIPLIER				10
#define EQUIPMENT_SHIELD_MULTIPLIER				10
#define EQUIPMENT_SHIELD_REGEN_MULTIPLIER		10
#define EQUIPMENT_DAMAGE_MULTIPLIER				10