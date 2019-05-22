#ifndef UPGRADE_H_INCLUDED
#define UPGRADE_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

#define UPGRADE_ICON_SIZE				64

enum UpgradeType
{
	Upgrade_None,
	Upgrade_SonarI,
	Upgrade_SonarII,
	Upgrade_Lifeboat,
	Upgrade_DivingSuit,
	Upgrade_Repair,
	NB_UPGRADE_TYPES,
};

struct Upgrade : GameEntity
{
public:
	Upgrade(UpgradeType type);
	~Upgrade();
	
	UpgradeType m_type;
	int m_value;
	string m_display_name;
	string m_description;
	string m_texture_name;
};

#endif //UPGRADE_H_INCLUDED