#include "FileLoader.h"

extern Game* CurrentGame;

EnemyPool* FileLoader::LoadEnemyPool(string name)
{
	vector<vector<string> > enemypoolConfig = *(FileLoaderUtils::FileLoader(ENEMYPOOL_FILE));

	for (std::vector<vector<string> >::iterator it = (enemypoolConfig).begin(); it != (enemypoolConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			//TODO
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find EnemyPool '%s'. Please check the config file", (char*)name.c_str()));
}

EnemyBase* FileLoader::LoadEnemyBase(string name, int probability, int enemyClass)
{
	if ((*CurrentGame).m_enemiesConfig[name].empty())
	{
		LOGGER_WRITE(Logger::DEBUG, ("\n<!>Enemy name cannot be found in (*CurrentGame).m_enemiesConfig: '%s'. Please check the ENEMY config file", (char*)name.c_str()));
		return NULL;
	}

	EnemyBase* base = new EnemyBase;
	base->m_enemy = new Enemy(sf::Vector2f(0, 0), sf::Vector2f(0, (float)stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_SPEED])), (*CurrentGame).m_enemiesConfig[name][ENEMY_IMAGE_NAME], sf::Vector2f((float)stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_WIDTH]), (float)stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_HEIGHT])), LoadFX((*CurrentGame).m_enemiesConfig[name][ENEMY_FX_DEATH]), stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_FRAMES]), stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_NB_SKINS]));
	base->m_enemy->setAnimationLine(stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_SKIN]) - 1);//the skin counts starts at 1
	base->m_probability = probability;
	base->m_enemyclass = enemyClass;
	base->m_enemy->m_enemy_class = (EnemyClass)enemyClass;

	((GameObject*)base->m_enemy)->m_armor = stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_ARMOR]);
	((GameObject*)base->m_enemy)->m_armor_max = stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_ARMOR]);
	((GameObject*)base->m_enemy)->m_shield = stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_SHIELD]);
	((GameObject*)base->m_enemy)->m_shield_max = stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_SHIELD]);
	((GameObject*)base->m_enemy)->m_shield_regen = stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_SHIELD_REGEN]);
	((GameObject*)base->m_enemy)->m_damage = stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_DAMAGE]);
	((GameObject*)base->m_enemy)->setMoney(stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_VALUE]));
	((GameObject*)base->m_enemy)->m_display_name = (*CurrentGame).m_enemiesConfig[name][ENEMY_NAME];

	//Loading phases
	if ((*CurrentGame).m_enemiesConfig[name][ENEMY_PHASE].compare("0") != 0)
	{
		vector<string> l_loadedPhases;
		vector<string> l_phasesToBeLoaded;

		l_phasesToBeLoaded.push_back((*CurrentGame).m_enemiesConfig[name][ENEMY_PHASE]);

		while (!l_phasesToBeLoaded.empty())
		{
			//loading phase
			Phase* phase = Enemy::LoadPhase(l_phasesToBeLoaded.front());
			base->m_enemy->m_phases.push_back(phase);
			l_loadedPhases.push_back(phase->m_name);
			l_phasesToBeLoaded.erase(l_phasesToBeLoaded.begin());

			//Do we have other phases to load that we have not loaded already?
			for (vector<ConditionTransition*>::iterator it = phase->m_transitions_list.begin(); it != phase->m_transitions_list.end(); it++)
			{
				vector<string>::iterator nextPhase = find(l_loadedPhases.begin(), l_loadedPhases.end(), (*it)->m_nextPhase_name);
				if (nextPhase == l_loadedPhases.end())
				{
					//already flagged as a phase to be loaded?
					vector<string>::iterator nextPhaseNotAlreadyLoading = find(l_phasesToBeLoaded.begin(), l_phasesToBeLoaded.end(), (*it)->m_nextPhase_name);
					if (nextPhaseNotAlreadyLoading == l_phasesToBeLoaded.end())
					{
						l_phasesToBeLoaded.push_back((*it)->m_nextPhase_name);
					}
				}
			}
		}

		//setting the starting phase
		//base->m_enemy->m_currentPhase = base->m_enemy->m_phases.front();
	}
	else
	{
		if ((*CurrentGame).m_enemiesConfig[name][ENEMY_WEAPON].compare("0") != 0)
			base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*CurrentGame).m_enemiesConfig[name][ENEMY_WEAPON], 1));
		
		if ((*CurrentGame).m_enemiesConfig[name][ENEMY_WEAPON_2].compare("0") != 0)
			base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*CurrentGame).m_enemiesConfig[name][ENEMY_WEAPON_2], 1));
		
		if ((*CurrentGame).m_enemiesConfig[name][ENEMY_WEAPON_3].compare("0") != 0)
			base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*CurrentGame).m_enemiesConfig[name][ENEMY_WEAPON_3], 1));

		base->m_enemy->m_rotation_speed = (float)stoi((*CurrentGame).m_enemiesConfig[name][ENEMY_ROTATION_SPEED]);
	}

	return base;
		
}

Weapon* FileLoader::LoadWeapon(string name, int fire_direction)
{
	return Enemy::LoadWeapon(name, fire_direction);
}

Ammo* FileLoader::LoadAmmo(string name)
{
	return Enemy::LoadAmmo(name);
}

FX* FileLoader::LoadFX(string name)
{
	return Enemy::LoadFX(name);
}

Bot* FileLoader::LoadBot(string name)
{
	return Enemy::LoadBot(name);
}