#include "Upgrade.h"

extern Game* CurrentGame;

Upgrade::Upgrade(UpgradeType type) : GameEntity(UI_Upgrade)
{
	m_type = type;
	m_value = 0;

	switch (type)
	{
		case Upgrade_SonarI:
		{
			m_value = 500;
			m_display_name = "Sonar I";
			m_description = "Allows to detect underwater elements down to 50 meters of depth.";
			m_texture_name = "2D/upgrade_sonar_1.png";
			break;
		}
		case Upgrade_SonarII:
		{
			m_value = 1500;
			m_display_name = "Sonar II";
			m_description = "Allows to detect underwater elements down to 100 meters of depth.";
			m_texture_name = "2D/upgrade_sonar_2.png";
			break;
		}
		case Upgrade_Lifeboat:
		{
			m_value = 1000;
			m_display_name = "Lifeboat";
			m_description = "Allows to unboard crew members on islands.";
			m_texture_name = "2D/upgrade_sonar_1.png";
			break;
		}
		case Upgrade_DivingSuit:
		{
			m_value = 1000;
			m_display_name = "Lifeboat";
			m_description = "Allows to send crew members diving underneath the sea.";
			m_texture_name = "2D/upgrade_sonar_1.png";
			break;
		}
		case Upgrade_Repair:
		{
			m_value = 100;
			m_display_name = "Repair";
			m_description = "Restore 100 health to your ship.";
			m_texture_name = "2D/upgrade_sonar_1.png";
			break;
		}
	}
}

Upgrade::~Upgrade()
{
	
}