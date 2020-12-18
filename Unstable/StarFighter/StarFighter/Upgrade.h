#ifndef UPGRADE_H_INCLUDED
#define UPGRADE_H_INCLUDED

#include "Portal.h"

enum UpgradesCategory
{
	Upgrade_Health,
	Upgrade_Shield,
	Upgrade_Weapon,
	NB_UPGRADES_CATEGORY,
};

class Upgrade : public GameObject
{
public:
	Upgrade(string name);
	
	string m_upgrade_name;

};

#endif //UPGRADE_H_INCLUDED