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
	m_can_hyperspeed = false;
	m_can_jump = false;
	m_bombs = 0;
}

ShipModel::~ShipModel()
{
	
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
	m_can_hyperspeed = false;
	m_can_jump = false;
	m_bombs = 0;
}

Equipment::~Equipment()
{
	//bots are destroyed with the method DestroyBots()
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
	new_equipment->m_bombs = m_bombs;
	new_equipment->m_can_hyperspeed = m_can_hyperspeed;
	new_equipment->m_can_jump = m_can_jump;

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