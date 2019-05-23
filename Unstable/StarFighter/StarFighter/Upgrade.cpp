#include "Upgrade.h"

extern Game* CurrentGame;

Upgrade::Upgrade(string upgrade_type) : GameEntity(UI_Upgrade)
{
	for (vector<vector<string > >::iterator it = (*CurrentGame).m_upgrades_config.begin(); it != (*CurrentGame).m_upgrades_config.end(); it++)
	{
		if ((*it)[Upgrade_ID].compare(upgrade_type) == 0)
		{
			m_type = upgrade_type;
			m_cost = stoi((*it)[Upgrade_Cost]);
			m_display_name = (*it)[Upgrade_Name];
			m_display_name = StringReplace(m_display_name, "_", " ");
			m_description = (*it)[Upgrade_Description];
			m_description = StringReplace(m_description, "_", " ");
			m_texture_name = (*it)[Upgrade_TextureName];
			m_dynamic_value = stoi((*it)[Upgrade_DynamicValue]);
			m_description = StringReplace(m_description, "#1$", (*it)[Upgrade_DynamicValue]);
			m_upgrade_required = (*it)[Upgrade_UpgradeRequired];
			return;
		}
	}

	printf("ERROR: can't find upgrade %s in database\n", upgrade_type.c_str());
}

Upgrade::~Upgrade()
{
	
}