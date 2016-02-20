#include "FileLoader.h"

Ship* FileLoader::LoadShipConfig(string name)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship config file");
	try
	{
		vector<vector<string>> shipConfig = *(FileLoaderUtils::FileLoader(SHIP_FILE));

		for (std::vector<vector<string>>::iterator it = (shipConfig).begin(); it != (shipConfig).end(); it++)
		{
			if((*it)[ShipConfigData::SHIPCONFIG_NAME].compare(name) == 0)
			{
				//Loading Ship Model
				LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship model\n");
				ShipModel* ship_model = FileLoader::LoadShipModel((*it)[ShipConfigData::SHIPCONFIG_SHIPMODEL]);

				Ship* ship = new Ship(ship_model);

				//Loading equipment
				LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship equipment\n");
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_AIRBRAKE]), false);
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_ENGINE]), false);
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_MODULE]), false);
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_ARMOR]), false);
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_SHIELD]), false);//false because of shipC->Init() below that will recompute the ship config stats

				//Loading FX
				ship->m_FX_death = FileLoader::LoadFX((*it)[ShipConfigData::SHIPCONFIG_DEATH_FX]);

				//Loading weapon
				if ((*it)[ShipConfigData::SHIPCONFIG_WEAPON].compare("0") != 0)
				{
					LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship weapon\n");
					ship->setShipWeapon(FileLoader::LoadWeapon((*it)[ShipConfigData::SHIPCONFIG_WEAPON], -1, FileLoader::LoadAmmo((*it)[ShipConfigData::SHIPCONFIG_AMMO])), false);//false because of shipC->Init() below that will recompute the ship config stats
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
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file",name));
}

EnemyPool* FileLoader::LoadEnemyPool(string name)
{
	
	vector<vector<string>> enemypoolConfig = *(FileLoaderUtils::FileLoader(ENEMYPOOL_FILE));

	for (std::vector<vector<string>>::iterator it = (enemypoolConfig).begin(); it != (enemypoolConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			//TODO
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find EnemyPool '%s'. Please check the config file",name));
}

EnemyBase* FileLoader::LoadEnemyBase(string m_name, int m_probability, int m_enemyClass)
{
	vector<vector<string>> enemyConfig = *(FileLoaderUtils::FileLoader(ENEMY_FILE));

	for (std::vector<vector<string>>::iterator it = (enemyConfig).begin(); it != (enemyConfig).end(); it++)
	{
		if ((*it)[0].compare(m_name) == 0)
		{
			EnemyBase* base = new EnemyBase;
			base->m_enemy = new Enemy(sf::Vector2f(0, 0), sf::Vector2f(0, stoi((*it)[EnemyData::ENEMY_SPEED])), (*it)[EnemyData::ENEMY_IMAGE_NAME], sf::Vector2f(stoi((*it)[EnemyData::ENEMY_WIDTH]), stoi((*it)[EnemyData::ENEMY_HEIGHT])), LoadFX((*it)[EnemyData::ENEMY_FX_DEATH]), stoi((*it)[EnemyData::ENEMY_FRAMES]));
			base->m_probability = m_probability;
			base->m_enemyclass = m_enemyClass;
			base->m_enemy->m_enemy_class = (EnemyClass)m_enemyClass;

			((GameObject*)base->m_enemy)->m_armor = stoi((*it)[EnemyData::ENEMY_ARMOR]);
			((GameObject*)base->m_enemy)->m_armor_max = stoi((*it)[EnemyData::ENEMY_ARMOR]);
			((GameObject*)base->m_enemy)->m_shield = stoi((*it)[EnemyData::ENEMY_SHIELD]);
			((GameObject*)base->m_enemy)->m_shield_max = stoi((*it)[EnemyData::ENEMY_SHIELD]);
			((GameObject*)base->m_enemy)->m_shield_regen = stoi((*it)[EnemyData::ENEMY_SHIELD_REGEN]);
			((GameObject*)base->m_enemy)->m_damage = stoi((*it)[EnemyData::ENEMY_DAMAGE]);
			((GameObject*)base->m_enemy)->setMoney(stoi((*it)[EnemyData::ENEMY_VALUE]));
			((GameObject*)base->m_enemy)->m_display_name = (*it)[EnemyData::ENEMY_NAME];

			//Loading phases
			if ((*it)[EnemyData::ENEMY_PHASE].compare("0") != 0)
			{
				vector<string> l_loadedPhases;
				vector<string> l_phasesToBeLoaded;

				l_phasesToBeLoaded.push_back((*it)[EnemyData::ENEMY_PHASE]);

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
				if ((*it)[EnemyData::ENEMY_WEAPON].compare("0") != 0)
				{
					base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*it)[EnemyData::ENEMY_WEAPON], 1, FileLoader::LoadAmmo((*it)[EnemyData::ENEMY_AMMO])));
				}
				if ((*it)[EnemyData::ENEMY_WEAPON_2].compare("0") != 0)
				{
					base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*it)[EnemyData::ENEMY_WEAPON_2], 1, FileLoader::LoadAmmo((*it)[EnemyData::ENEMY_AMMO_2])));
				}
				if ((*it)[EnemyData::ENEMY_WEAPON_3].compare("0") != 0)
				{
					base->m_enemy->m_weapons_list.push_back(FileLoader::LoadWeapon((*it)[EnemyData::ENEMY_WEAPON_3], 1, FileLoader::LoadAmmo((*it)[EnemyData::ENEMY_AMMO_3])));
				}

				PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), EnemyData::ENEMY_PATTERN);
				base->m_enemy->m_Pattern.SetPattern(m_bobby->currentPattern, m_bobby->patternSpeed, m_bobby->patternParams);

				base->m_enemy->m_rotation_speed = stoi((*it)[EnemyData::ENEMY_ROTATION_SPEED]);
			}

			return base;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Enemy '%s'. Please check the config file", m_name));
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
	vector<vector<string>>equipmentConfig = *(FileLoaderUtils::FileLoader(EQUIPMENT_FILE));

	for (std::vector<vector<string>>::iterator it = (equipmentConfig).begin(); it != (equipmentConfig).end(); it++)
	{
		if((*it)[EquipmentData::EQUIPMENT_NAME].compare(name) == 0)
		{
			Equipment* i = new Equipment();

			i-> Init(EquipmentType::NBVAL_Equipment, stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED]), stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION]), stoi((*it)[EquipmentData::EQUIPMENT_DECCELERATION]), 
				stoi((*it)[EquipmentData::EQUIPMENT_HYPERSPEED]), stoi((*it)[EquipmentData::EQUIPMENT_ARMOR]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD_REGEN]), stoi((*it)[EquipmentData::EQUIPMENT_DAMAGE]),
				(*it)[EquipmentData::EQUIPMENT_IMAGE_NAME], Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_WIDTH]), stoi((*it)[EquipmentData::EQUIPMENT_HEIGHT])),
				stoi((*it)[EquipmentData::EQUIPMENT_FRAMES]), (*it)[EquipmentData::EQUIPMENT_NAME]);

			if ((*it)[EquipmentData::EQUIPMENT_BOT].compare("0") != 0)
			{
				i->m_bot = LoadBot((*it)[EquipmentData::EQUIPMENT_BOT]);
			}

			if(!(*it)[EquipmentData::EQUIPMENT_FAKE_TEXTURE].compare("0") == 0 && !(*it)[EquipmentData::EQUIPMENT_FAKE_WIDTH].compare("0") == 0
				&& !(*it)[EquipmentData::EQUIPMENT_FAKE_HEIGHT].compare("0") == 0 && !(*it)[EquipmentData::EQUIPMENT_FAKE_FRAMES].compare("0") == 0)
			{
				i->m_fake_textureName = (*it)[EquipmentData::EQUIPMENT_FAKE_TEXTURE];
				i->m_fake_size = sf::Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_FAKE_WIDTH]), stoi((*it)[EquipmentData::EQUIPMENT_FAKE_HEIGHT]));
				i->m_fake_frameNumber = stoi((*it)[EquipmentData::EQUIPMENT_FAKE_FRAMES]);
			}

			//if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("airbrake") == 0)
			//	i->equipmentType = EquipmentType::Airbrake;
			if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("engine") == 0)
				i->m_equipmentType = EquipmentType::Engine;
			else if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("armor") == 0)
				i->m_equipmentType = EquipmentType::Armor;
			else if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("shield") == 0)
				i->m_equipmentType = EquipmentType::Shield;
			else if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("module") == 0)
				i->m_equipmentType = EquipmentType::Module;
			else 
				LOGGER_WRITE(Logger::Priority::DEBUG,("Equipment config file error: cannot find a valid equipment type for: '%s'. Please check the config file",name));

			i->m_display_name = (*it)[EquipmentData::EQUIPMENT_DISPLAY_NAME];

			return i;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Equipment '%s'. Please check the config file",name));
}

ShipModel* FileLoader::LoadShipModel(string name)
{
	vector<vector<string>> equipmentConfig = *(FileLoaderUtils::FileLoader(EQUIPMENT_FILE));

	for (std::vector<vector<string>>::iterator it = (equipmentConfig).begin(); it != (equipmentConfig).end(); it++)
	{
		if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("shipmodel") == 0)
		{
			if((*it)[EquipmentData::EQUIPMENT_NAME].compare(name) == 0)
			{
				ShipModel* s = new ShipModel(stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED]), stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION]), stoi((*it)[EquipmentData::EQUIPMENT_DECCELERATION]), 
					stoi((*it)[EquipmentData::EQUIPMENT_HYPERSPEED]), stoi((*it)[EquipmentData::EQUIPMENT_ARMOR]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD_REGEN]), stoi((*it)[EquipmentData::EQUIPMENT_DAMAGE]),
					(*it)[EquipmentData::EQUIPMENT_IMAGE_NAME], Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_WIDTH]), stoi((*it)[EquipmentData::EQUIPMENT_HEIGHT])), 
					stoi((*it)[EquipmentData::EQUIPMENT_FRAMES]), (*it)[EquipmentData::EQUIPMENT_NAME]);

				if ((*it)[EquipmentData::EQUIPMENT_BOT].compare("0") != 0)
				{
					s->m_bot = LoadBot((*it)[EquipmentData::EQUIPMENT_BOT]);
				}

				if(!(*it)[EquipmentData::EQUIPMENT_FAKE_TEXTURE].compare("0") == 0 && !(*it)[EquipmentData::EQUIPMENT_FAKE_WIDTH].compare("0") == 0
					&& !(*it)[EquipmentData::EQUIPMENT_FAKE_HEIGHT].compare("0") == 0 && !(*it)[EquipmentData::EQUIPMENT_FAKE_FRAMES].compare("0") == 0)
				{
					s->m_fake_textureName = (*it)[EquipmentData::EQUIPMENT_FAKE_TEXTURE];
					s->m_fake_size = sf::Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_FAKE_WIDTH]), stoi((*it)[EquipmentData::EQUIPMENT_FAKE_HEIGHT]));
					s->m_fake_frameNumber = stoi((*it)[EquipmentData::EQUIPMENT_FAKE_FRAMES]);
				}

				return s;
			}	
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find ShipModel '%s'. Please check the config file",name));
}

Bot* FileLoader::LoadBot(string name)
{
	vector<vector<string>>botConfig = *(FileLoaderUtils::FileLoader(BOT_FILE));

	for (std::vector<vector<string>>::iterator it = (botConfig).begin(); it != (botConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			Bot* bot = new Bot(Vector2f(0,0), Vector2f(0,0),(*it)[BotData::BOT_IMAGE_NAME],sf::Vector2f(stoi((*it)[BotData::BOT_WIDTH]),stoi((*it)[BotData::BOT_HEIGHT])));

			((GameObject*)bot)->m_display_name = (*it)[BotData::BOT_NAME];
			((GameObject*)bot)->m_armor = stoi((*it)[BotData::BOT_ARMOR]);
			((GameObject*)bot)->m_armor_max = stoi((*it)[BotData::BOT_ARMOR]);
			((GameObject*)bot)->m_shield = stoi((*it)[BotData::BOT_SHIELD]);
			((GameObject*)bot)->m_shield_max = stoi((*it)[BotData::BOT_SHIELD]);
			((GameObject*)bot)->m_shield_regen = stoi((*it)[BotData::BOT_SHIELD_REGEN]);
			((GameObject*)bot)->m_damage = stoi((*it)[BotData::BOT_DAMAGE]);
			bot->m_spread = Vector2f(stoi((*it)[BotData::BOT_XSPREAD]), stoi((*it)[BotData::BOT_YSPREAD]));

			PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), BotData::BOT_PATTERN);
			bot->m_Pattern.SetPattern(m_bobby->currentPattern, m_bobby->patternSpeed, m_bobby->patternParams);

			bot->m_rotation_speed = stoi((*it)[BotData::BOT_ROTATION_SPEED]);

			if ((*it)[BotData::BOT_WEAPON].compare("0") != 0)
			{
				bot->m_weapon = FileLoader::LoadWeapon((*it)[BotData::BOT_WEAPON], -1, FileLoader::LoadAmmo((*it)[BotData::BOT_AMMO]));
			}

			return bot;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Bot '%s'. Please check the config file",name));
}