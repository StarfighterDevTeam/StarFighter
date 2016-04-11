#include "FileLoader.h"

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
					ship->m_weapon = FileLoader::LoadWeapon((*it)[SHIPCONFIG_WEAPON], -1, FileLoader::LoadAmmo((*it)[SHIPCONFIG_AMMO]));//false because of shipC->Init() below that will recompute the ship config stats
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
	vector<vector<string> > enemyConfig = *(FileLoaderUtils::FileLoader(ENEMY_FILE));

	for (std::vector<vector<string> >::iterator it = (enemyConfig).begin(); it != (enemyConfig).end(); it++)
	{
		if ((*it)[0].compare(name) == 0)
		{
			EnemyBase* base = new EnemyBase;
			base->m_enemy = new Enemy(sf::Vector2f(0, 0), sf::Vector2f(0, stoi((*it)[ENEMY_SPEED])), (*it)[ENEMY_IMAGE_NAME], sf::Vector2f(stoi((*it)[ENEMY_WIDTH]), stoi((*it)[ENEMY_HEIGHT])), LoadFX((*it)[ENEMY_FX_DEATH]), stoi((*it)[ENEMY_FRAMES]));
			base->m_probability = probability;
			base->m_enemyclass = enemyClass;
			base->m_enemy->m_enemy_class = (EnemyClass)enemyClass;

			((GameObject*)base->m_enemy)->m_armor = stoi((*it)[ENEMY_ARMOR]);
			((GameObject*)base->m_enemy)->m_armor_max = stoi((*it)[ENEMY_ARMOR]);
			((GameObject*)base->m_enemy)->m_shield = stoi((*it)[ENEMY_SHIELD]);
			((GameObject*)base->m_enemy)->m_shield_max = stoi((*it)[ENEMY_SHIELD]);
			((GameObject*)base->m_enemy)->m_shield_regen = stoi((*it)[ENEMY_SHIELD_REGEN]);
			((GameObject*)base->m_enemy)->m_damage = stoi((*it)[ENEMY_DAMAGE]);
			((GameObject*)base->m_enemy)->setMoney(stoi((*it)[ENEMY_VALUE]));
			((GameObject*)base->m_enemy)->m_display_name = (*it)[ENEMY_NAME];

			//Loading phases
			if ((*it)[ENEMY_PHASE].compare("0") != 0)
			{
				vector<string> l_loadedPhases;
				vector<string> l_phasesToBeLoaded;

				l_phasesToBeLoaded.push_back((*it)[ENEMY_PHASE]);

				while (!l_phasesToBeLoaded.empty())
				{
					//loading phase
					Phase* phase = Enemy::LoadPhase(l_phasesToBeLoaded.front());
					base->m_enemy->m_phases.push_back(phase);
					l_loadedPhases.push_back(phase->m_name);
					l_phasesToBeLoaded.erase(l_phasesToBeLoaded.begin());

					//Do we have other phases to load that we have not loaded already?
					for (vector<ConditionTransition*>::iterator it = (phase->m_transitions_list).begin(); it != (phase->m_transitions_list).end(); it++)
					{
						vector<string>::iterator nextPhase = find(l_loadedPhases.begin(), l_loadedPhases.end(), (*it)->m_nextPhase_name);
						if (nextPhase == l_loadedPhases.end())
						{
							l_phasesToBeLoaded.push_back((*it)->m_nextPhase_name);
						}
					}
				}

				//setting the starting phase
				base->m_enemy->setPhase(base->m_enemy->m_phases.front());
			}
			else
			{
				if ((*it)[ENEMY_WEAPON].compare("0") != 0)
				{
					base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*it)[ENEMY_WEAPON], 1, FileLoader::LoadAmmo((*it)[ENEMY_AMMO])));
				}
				if ((*it)[ENEMY_WEAPON_2].compare("0") != 0)
				{
					base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*it)[ENEMY_WEAPON_2], 1, FileLoader::LoadAmmo((*it)[ENEMY_AMMO_2])));
				}
				if ((*it)[ENEMY_WEAPON_3].compare("0") != 0)
				{
					base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*it)[ENEMY_WEAPON_3], 1, FileLoader::LoadAmmo((*it)[ENEMY_AMMO_3])));
				}

				PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), ENEMY_PATTERN);
				base->m_enemy->m_Pattern.SetPattern(m_bobby->currentPattern, m_bobby->patternSpeed, m_bobby->patternParams);

				base->m_enemy->m_rotation_speed = stoi((*it)[ENEMY_ROTATION_SPEED]);
			}

			return base;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Enemy '%s'. Please check the config file", (char*)name.c_str()));
}

Weapon* FileLoader::LoadWeapon(string name, int fire_direction, Ammo* ammo)
{
	return Enemy::LoadWeapon(name, fire_direction, ammo);
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
	vector<vector<string> >equipmentConfig = *(FileLoaderUtils::FileLoader(EQUIPMENT_FILE));

	for (std::vector<vector<string> >::iterator it = (equipmentConfig).begin(); it != (equipmentConfig).end(); it++)
	{
		if((*it)[EQUIPMENT_NAME].compare(name) == 0)
		{
			Equipment* i = new Equipment();

			i-> Init(NBVAL_Equipment, stoi((*it)[EQUIPMENT_MAXSPEED]), stoi((*it)[EQUIPMENT_ACCELERATION]), stoi((*it)[EQUIPMENT_DECCELERATION]), 
				stoi((*it)[EQUIPMENT_HYPERSPEED]), stoi((*it)[EQUIPMENT_HYPERSPEED_FUEL]), stoi((*it)[EQUIPMENT_ARMOR]), stoi((*it)[EQUIPMENT_SHIELD]), stoi((*it)[EQUIPMENT_SHIELD_REGEN]),
				atof((*it)[EQUIPMENT_SHIELD_RECOVERY].c_str()), stoi((*it)[EQUIPMENT_DAMAGE]),
				(*it)[EQUIPMENT_IMAGE_NAME], Vector2f(stoi((*it)[EQUIPMENT_WIDTH]), stoi((*it)[EQUIPMENT_HEIGHT])),
				stoi((*it)[EQUIPMENT_FRAMES]), (*it)[EQUIPMENT_NAME]);

			if ((*it)[EQUIPMENT_BOT].compare("0") != 0)
			{
				i->m_bot = LoadBot((*it)[EQUIPMENT_BOT]);
			}

			if(!(*it)[EQUIPMENT_FAKE_TEXTURE].compare("0") == 0 && !(*it)[EQUIPMENT_FAKE_WIDTH].compare("0") == 0
				&& !(*it)[EQUIPMENT_FAKE_HEIGHT].compare("0") == 0 && !(*it)[EQUIPMENT_FAKE_FRAMES].compare("0") == 0)
			{
				i->m_fake_textureName = (*it)[EQUIPMENT_FAKE_TEXTURE];
				i->m_fake_size = sf::Vector2f(stoi((*it)[EQUIPMENT_FAKE_WIDTH]), stoi((*it)[EQUIPMENT_FAKE_HEIGHT]));
				i->m_fake_frameNumber = stoi((*it)[EQUIPMENT_FAKE_FRAMES]);
			}

			//if((*it)[EQUIPMENT_COMPARE].compare("airbrake") == 0)
			//	i->equipmentType = Airbrake;
			if((*it)[EQUIPMENT_COMPARE].compare("engine") == 0)
				i->m_equipmentType = Engine;
			else if((*it)[EQUIPMENT_COMPARE].compare("armor") == 0)
				i->m_equipmentType = Armor;
			else if((*it)[EQUIPMENT_COMPARE].compare("shield") == 0)
				i->m_equipmentType = Shield;
			else if((*it)[EQUIPMENT_COMPARE].compare("module") == 0)
				i->m_equipmentType = Module;
			else 
				LOGGER_WRITE(Logger::DEBUG, ("Equipment config file error: cannot find a valid equipment type for: '%s'. Please check the config file", (char*)name.c_str()));

			i->m_display_name = (*it)[EQUIPMENT_DISPLAY_NAME];

			return i;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Equipment '%s'. Please check the config file", (char*)name.c_str()));
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
				ShipModel* s = new ShipModel(stoi((*it)[EQUIPMENT_MAXSPEED]), stoi((*it)[EQUIPMENT_ACCELERATION]), stoi((*it)[EQUIPMENT_DECCELERATION]), 
					stoi((*it)[EQUIPMENT_HYPERSPEED]), stoi((*it)[EQUIPMENT_HYPERSPEED_FUEL]), stoi((*it)[EQUIPMENT_ARMOR]), stoi((*it)[EQUIPMENT_SHIELD]), stoi((*it)[EQUIPMENT_SHIELD_REGEN]),
					atof((*it)[EQUIPMENT_SHIELD_RECOVERY].c_str()), stoi((*it)[EQUIPMENT_DAMAGE]),
					(*it)[EQUIPMENT_IMAGE_NAME], Vector2f(stoi((*it)[EQUIPMENT_WIDTH]), stoi((*it)[EQUIPMENT_HEIGHT])), 
					stoi((*it)[EQUIPMENT_FRAMES]), (*it)[EQUIPMENT_NAME]);

				if ((*it)[EQUIPMENT_BOT].compare("0") != 0)
				{
					s->m_bot = LoadBot((*it)[EQUIPMENT_BOT]);
				}

				if(!(*it)[EQUIPMENT_FAKE_TEXTURE].compare("0") == 0 && !(*it)[EQUIPMENT_FAKE_WIDTH].compare("0") == 0
					&& !(*it)[EQUIPMENT_FAKE_HEIGHT].compare("0") == 0 && !(*it)[EQUIPMENT_FAKE_FRAMES].compare("0") == 0)
				{
					s->m_fake_textureName = (*it)[EQUIPMENT_FAKE_TEXTURE];
					s->m_fake_size = sf::Vector2f(stoi((*it)[EQUIPMENT_FAKE_WIDTH]), stoi((*it)[EQUIPMENT_FAKE_HEIGHT]));
					s->m_fake_frameNumber = stoi((*it)[EQUIPMENT_FAKE_FRAMES]);
				}

				return s;
			}	
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find ShipModel '%s'. Please check the config file", (char*)name.c_str()));
}

Bot* FileLoader::LoadBot(string name)
{
	vector<vector<string> >botConfig = *(FileLoaderUtils::FileLoader(BOT_FILE));

	for (std::vector<vector<string> >::iterator it = (botConfig).begin(); it != (botConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			Bot* bot = new Bot(Vector2f(0,0), Vector2f(0,0),(*it)[BOT_IMAGE_NAME],sf::Vector2f(stoi((*it)[BOT_WIDTH]),stoi((*it)[BOT_HEIGHT])));

			((GameObject*)bot)->m_display_name = (*it)[BOT_NAME];
			((GameObject*)bot)->m_armor = stoi((*it)[BOT_ARMOR]);
			((GameObject*)bot)->m_armor_max = stoi((*it)[BOT_ARMOR]);
			((GameObject*)bot)->m_shield = stoi((*it)[BOT_SHIELD]);
			((GameObject*)bot)->m_shield_max = stoi((*it)[BOT_SHIELD]);
			((GameObject*)bot)->m_shield_regen = stoi((*it)[BOT_SHIELD_REGEN]);
			((GameObject*)bot)->m_damage = stoi((*it)[BOT_DAMAGE]);
			bot->m_spread = Vector2f(stoi((*it)[BOT_XSPREAD]), stoi((*it)[BOT_YSPREAD]));

			PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), BOT_PATTERN);
			bot->m_Pattern.SetPattern(m_bobby->currentPattern, m_bobby->patternSpeed, m_bobby->patternParams);

			bot->m_rotation_speed = stoi((*it)[BOT_ROTATION_SPEED]);

			if ((*it)[BOT_WEAPON].compare("0") != 0)
			{
				bot->m_weapon = FileLoader::LoadWeapon((*it)[BOT_WEAPON], -1, FileLoader::LoadAmmo((*it)[BOT_AMMO]));
			}

			return bot;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Bot '%s'. Please check the config file", (char*)name.c_str()));
}