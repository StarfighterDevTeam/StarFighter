#ifndef UPGRADE_H_INCLUDED
#define UPGRADE_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

#define UPGRADE_ICON_SIZE				64

enum UpgradeData
{
	Upgrade_ID,
	Upgrade_Name,
	Upgrade_TextureName,
	Upgrade_Cost,
	Upgrade_Description,
	Upgrade_DynamicValue,
	Upgrade_UpgradeRequired,
	Upgrade_Stat,
	NB_UPGRADE_DATA,
};

struct Upgrade : GameEntity
{
public:
	Upgrade(string upgrade_type);
	~Upgrade();
	
	string m_type;
	int m_cost;
	string m_display_name;
	string m_description;
	string m_texture_name;
	int m_dynamic_value;
	string m_upgrade_required;
};

#endif //UPGRADE_H_INCLUDED