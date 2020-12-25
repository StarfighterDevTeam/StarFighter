#include "FileLoader.h"

extern Game* CurrentGame;

Ship* FileLoader::LoadShipConfig(string name)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading ship config file");
	try
	{
		vector<vector<string> > shipConfig = *(FileLoaderUtils::FileLoader(SHIP_FILE));

		for (std::vector<vector<string> >::iterator it = (shipConfig).begin(); it != (shipConfig).end(); it++)
		{
			if((*it)[SHIPCONFIG_NAME].compare(name) == 0)
			{
				//Loading Ship Model
				LOGGER_WRITE(Logger::DEBUG, "Loading ship model\n");
				ShipModel* ship_model = FileLoader::LoadShipModel((*it)[SHIPCONFIG_SHIPMODEL]);

				Ship* ship = new Ship(ship_model);

				//Loading equipment
				LOGGER_WRITE(Logger::DEBUG, "Loading ship equipment\n");
				if ((*it)[SHIPCONFIG_ENGINE].compare("0") != 0)
					ship->setShipEquipment(FileLoader::LoadEquipment((*it)[SHIPCONFIG_ENGINE]), false, true);
				if ((*it)[SHIPCONFIG_MODULE].compare("0") != 0)
					ship->setShipEquipment(FileLoader::LoadEquipment((*it)[SHIPCONFIG_MODULE]), false, true);
				if ((*it)[SHIPCONFIG_ARMOR].compare("0") != 0)
					ship->setShipEquipment(FileLoader::LoadEquipment((*it)[SHIPCONFIG_ARMOR]), false, true);
				if ((*it)[SHIPCONFIG_SHIELD].compare("0") != 0)
					ship->setShipEquipment(FileLoader::LoadEquipment((*it)[SHIPCONFIG_SHIELD]), false, true);//false because of shipC->Init() below that will recompute the ship config stats

				//Loading FX
				if ((*it)[SHIPCONFIG_DEATH_FX].compare("0") != 0)
					ship->m_FX_death = FileLoader::LoadFX((*it)[SHIPCONFIG_DEATH_FX]);
	
				//Loading weapon
				if ((*it)[SHIPCONFIG_WEAPON].compare("0") != 0)
				{
					LOGGER_WRITE(Logger::DEBUG, "Loading ship weapon\n");
					ship->m_weapon = FileLoader::LoadWeapon((*it)[SHIPCONFIG_WEAPON], -1);//false because of shipC->Init() below that will recompute the ship config stats
				}

				//Computing the ship config
				ship->Init();

				return ship;
			}
		}
	}
	catch( const std::exception & ex ) 
	{
		//An error occured
		LOGGER_WRITE(Logger::LERROR,ex.what());
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ship config '%s'. Please check the config file", (char*)name.c_str()));
}

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

Equipment* FileLoader::LoadEquipment(string name)
{
	return Enemy::LoadEquipment(name);
}

Bot* FileLoader::LoadBot(string name)
{
	return Enemy::LoadBot(name);
}

ShipModel* FileLoader::LoadShipModel(string name)
{
	vector<vector<string> > equipmentConfig = *(FileLoaderUtils::FileLoader(EQUIPMENT_FILE));

	for (std::vector<vector<string> >::iterator it = (equipmentConfig).begin(); it != (equipmentConfig).end(); it++)
	{
		if((*it)[EQUIPMENT_COMPARE].compare("shipmodel") == 0)
		{
			if((*it)[EQUIPMENT_NAME].compare(name) == 0)
			{
				ShipModel* s = new ShipModel((float)stoi((*it)[EQUIPMENT_MAXSPEED]), (float)stoi((*it)[EQUIPMENT_ACCELERATION]), (float)stoi((*it)[EQUIPMENT_DECCELERATION]),
					stoi((*it)[EQUIPMENT_HYPERSPEED]), stoi((*it)[EQUIPMENT_HYPERSPEED_FUEL]), stoi((*it)[EQUIPMENT_ARMOR]), stoi((*it)[EQUIPMENT_SHIELD]), stoi((*it)[EQUIPMENT_SHIELD_REGEN]),
					atof((*it)[EQUIPMENT_SHIELD_RECOVERY].c_str()), stoi((*it)[EQUIPMENT_DAMAGE]),
					(*it)[EQUIPMENT_IMAGE_NAME], Vector2f((float)stoi((*it)[EQUIPMENT_WIDTH]), (float)stoi((*it)[EQUIPMENT_HEIGHT])),
					stoi((*it)[EQUIPMENT_FRAMES]), (*it)[EQUIPMENT_NAME]);

				if ((*it)[EQUIPMENT_BOT].compare("0") != 0)
				{
					s->m_bot = LoadBot((*it)[EQUIPMENT_BOT]);
				}

				if(!(*it)[EQUIPMENT_FAKE_TEXTURE].compare("0") == 0 && !(*it)[EQUIPMENT_FAKE_WIDTH].compare("0") == 0
					&& !(*it)[EQUIPMENT_FAKE_HEIGHT].compare("0") == 0 && !(*it)[EQUIPMENT_FAKE_FRAMES].compare("0") == 0)
				{
					s->m_fake_textureName = (*it)[EQUIPMENT_FAKE_TEXTURE];
					s->m_fake_size = sf::Vector2f((float)stoi((*it)[EQUIPMENT_FAKE_WIDTH]), (float)stoi((*it)[EQUIPMENT_FAKE_HEIGHT]));
					s->m_fake_frameNumber = stoi((*it)[EQUIPMENT_FAKE_FRAMES]);
				}

				return s;
			}	
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find ShipModel '%s'. Please check the config file", (char*)name.c_str()));
}