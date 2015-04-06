#include "FileLoader.h"

ShipConfig* FileLoader::LoadShipConfig(string name)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship config file");
	try
	{
		vector<vector<string>> shipConfig = *(FileLoaderUtils::FileLoader(SHIP_FILE));

		for (std::vector<vector<string>>::iterator it = (shipConfig).begin(); it != (shipConfig).end(); it++)
		{
			if((*it)[ShipConfigData::SHIPCONFIG_NAME].compare(name) == 0)
			{
				ShipConfig* shipC = new ShipConfig();

				//Loading Ship Model
				LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship model\n");
				shipC->setShipModel(FileLoader::LoadShipModel((*it)[ShipConfigData::SHIPCONFIG_SHIPMODEL]));

				//Loading equipment
				LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship equipment\n");
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_AIRBRAKE]), false);
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_ENGINE]), false);
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_MODULE]), false);
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_ARMOR]), false);
				//shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_SHIELD]), false);//false because of shipC->Init() below that will recompute the ship config stats

				//Loading FX
				shipC->FX_death = FileLoader::LoadFX((*it)[ShipConfigData::SHIPCONFIG_DEATH_FX]);

				//Loading weapon
				if ((*it)[ShipConfigData::SHIPCONFIG_WEAPON].compare("0") != 0)
				{
					LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship weapon\n");
					shipC->setShipWeapon(FileLoader::LoadWeapon((*it)[ShipConfigData::SHIPCONFIG_WEAPON], -1, FileLoader::LoadAmmo((*it)[ShipConfigData::SHIPCONFIG_AMMO])), false);//false because of shipC->Init() below that will recompute the ship config stats
				}

				//Computing the ship config
				shipC->Init();

				return shipC;
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
			base->enemy = new Enemy(sf::Vector2f(0, 0), sf::Vector2f(0, stoi((*it)[EnemyData::ENEMY_SPEED])), (*it)[EnemyData::ENEMY_IMAGE_NAME], sf::Vector2f(stoi((*it)[EnemyData::ENEMY_WIDTH]), stoi((*it)[EnemyData::ENEMY_HEIGHT])), LoadFX((*it)[EnemyData::ENEMY_FX_DEATH]), stoi((*it)[EnemyData::ENEMY_FRAMES]));
			base->probability = m_probability;
			base->enemyclass = m_enemyClass;
			base->enemy->enemy_class = (EnemyClass)m_enemyClass;

			((Independant*)base->enemy)->armor = stoi((*it)[EnemyData::ENEMY_ARMOR]);
			((Independant*)base->enemy)->armor_max = stoi((*it)[EnemyData::ENEMY_ARMOR]);
			((Independant*)base->enemy)->shield = stoi((*it)[EnemyData::ENEMY_SHIELD]);
			((Independant*)base->enemy)->shield_max = stoi((*it)[EnemyData::ENEMY_SHIELD]);
			((Independant*)base->enemy)->shield_regen = stoi((*it)[EnemyData::ENEMY_SHIELD_REGEN]);
			((Independant*)base->enemy)->damage = stoi((*it)[EnemyData::ENEMY_DAMAGE]);
			((Independant*)base->enemy)->setMoney(stoi((*it)[EnemyData::ENEMY_VALUE]));
			((Independant*)base->enemy)->display_name = (*it)[EnemyData::ENEMY_NAME];

			if ((*it)[EnemyData::ENEMY_WEAPON].compare("0") != 0)
			{
				base->enemy->weapons_list.push_back(FileLoader::LoadWeapon((*it)[EnemyData::ENEMY_WEAPON], 1, FileLoader::LoadAmmo((*it)[EnemyData::ENEMY_AMMO])));
			}
			if ((*it)[EnemyData::ENEMY_WEAPON_2].compare("0") != 0)
			{
				base->enemy->weapons_list.push_back(FileLoader::LoadWeapon((*it)[EnemyData::ENEMY_WEAPON_2], 1, FileLoader::LoadAmmo((*it)[EnemyData::ENEMY_AMMO_2])));
			}
			if ((*it)[EnemyData::ENEMY_WEAPON_3].compare("0") != 0)
			{
				base->enemy->weapons_list.push_back(FileLoader::LoadWeapon((*it)[EnemyData::ENEMY_WEAPON_3], 1, FileLoader::LoadAmmo((*it)[EnemyData::ENEMY_AMMO_3])));
			}

			PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), EnemyData::ENEMY_PATTERN);
			base->enemy->Pattern.SetPattern(m_bobby->currentPattern, m_bobby->patternSpeed, m_bobby->patternParams);

			base->enemy->rotation_speed = stoi((*it)[EnemyData::ENEMY_ROTATION_SPEED]);

			//Loading phases
			if ((*it)[EnemyData::ENEMY_PHASE].compare("0") != 0)
			{
				base->enemy->hasPhases = true;
				base->enemy->setPhase((*it)[EnemyData::ENEMY_PHASE]);
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
				i->bot = LoadBot((*it)[EquipmentData::EQUIPMENT_BOT]);
				i->hasBot = true;
			}

			if(!(*it)[EquipmentData::EQUIPMENT_FAKE_TEXTURE].compare("0") == 0 && !(*it)[EquipmentData::EQUIPMENT_FAKE_WIDTH].compare("0") == 0
				&& !(*it)[EquipmentData::EQUIPMENT_FAKE_HEIGHT].compare("0") == 0 && !(*it)[EquipmentData::EQUIPMENT_FAKE_FRAMES].compare("0") == 0)
			{
				i->fake_textureName = (*it)[EquipmentData::EQUIPMENT_FAKE_TEXTURE];
				i->fake_size = sf::Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_FAKE_WIDTH]), stoi((*it)[EquipmentData::EQUIPMENT_FAKE_HEIGHT]));
				i->fake_frameNumber = stoi((*it)[EquipmentData::EQUIPMENT_FAKE_FRAMES]);
				i->hasFake = true;
			}

			//if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("airbrake") == 0)
			//	i->equipmentType = EquipmentType::Airbrake;
			if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("engine") == 0)
				i->equipmentType = EquipmentType::Engine;
			else if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("armor") == 0)
				i->equipmentType = EquipmentType::Armor;
			else if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("shield") == 0)
				i->equipmentType = EquipmentType::Shield;
			else if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("module") == 0)
				i->equipmentType = EquipmentType::Module;
			else 
				LOGGER_WRITE(Logger::Priority::DEBUG,"Equipment config file error: cannot find a valid equipment type for: '%s'. Please check the config file",name);

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
					s->bot = LoadBot((*it)[EquipmentData::EQUIPMENT_BOT]);
					s->hasBot = true;
				}

				if(!(*it)[EquipmentData::EQUIPMENT_FAKE_TEXTURE].compare("0") == 0 && !(*it)[EquipmentData::EQUIPMENT_FAKE_WIDTH].compare("0") == 0
					&& !(*it)[EquipmentData::EQUIPMENT_FAKE_HEIGHT].compare("0") == 0 && !(*it)[EquipmentData::EQUIPMENT_FAKE_FRAMES].compare("0") == 0)
				{
					s->fake_textureName = (*it)[EquipmentData::EQUIPMENT_FAKE_TEXTURE];
					s->fake_size = sf::Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_FAKE_WIDTH]), stoi((*it)[EquipmentData::EQUIPMENT_FAKE_HEIGHT]));
					s->fake_frameNumber = stoi((*it)[EquipmentData::EQUIPMENT_FAKE_FRAMES]);
					s->hasFake = true;
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

			((Independant*)bot)->display_name = (*it)[BotData::BOT_NAME];
			((Independant*)bot)->armor = stoi((*it)[BotData::BOT_ARMOR]);
			((Independant*)bot)->armor_max = stoi((*it)[BotData::BOT_ARMOR]);
			((Independant*)bot)->shield = stoi((*it)[BotData::BOT_SHIELD]);
			((Independant*)bot)->shield_max = stoi((*it)[BotData::BOT_SHIELD]);
			((Independant*)bot)->shield_regen = stoi((*it)[BotData::BOT_SHIELD_REGEN]);
			((Independant*)bot)->damage = stoi((*it)[BotData::BOT_DAMAGE]);
			bot->spread = Vector2f(stoi((*it)[BotData::BOT_XSPREAD]), stoi((*it)[BotData::BOT_YSPREAD]));

			PatternBobby* m_bobby = PatternBobby::PatternLoader((*it), BotData::BOT_PATTERN);
			bot->Pattern.SetPattern(m_bobby->currentPattern, m_bobby->patternSpeed, m_bobby->patternParams);

			bot->rotation_speed = stoi((*it)[BotData::BOT_ROTATION_SPEED]);

			if ((*it)[BotData::BOT_WEAPON].compare("0") != 0)
			{
				bot->weapon = FileLoader::LoadWeapon((*it)[BotData::BOT_WEAPON], -1, FileLoader::LoadAmmo((*it)[BotData::BOT_AMMO]));
			}

			return bot;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Bot '%s'. Please check the config file",name));
}