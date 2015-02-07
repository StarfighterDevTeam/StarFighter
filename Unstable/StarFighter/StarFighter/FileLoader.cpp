#include "FileLoader.h"

ShipConfig* FileLoader::LoadShipConfig(string name)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship config file");
	try
	{
		list<vector<string>> shipConfig = *(FileLoaderUtils::FileLoader(SHIP_FILE));

		for (std::list<vector<string>>::iterator it = (shipConfig).begin(); it != (shipConfig).end(); it++)
		{
			if((*it)[ShipConfigData::SHIPCONFIG_NAME].compare(name) == 0)
			{
				ShipConfig* shipC = new ShipConfig();

				//Loading Ship Model
				LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship model\n");
				shipC->setShipModel(FileLoader::LoadShipModel((*it)[ShipConfigData::SHIPCONFIG_SHIPMODEL]));

				//Loading equipment
				LOGGER_WRITE(Logger::Priority::DEBUG, "Loading ship equipment\n");
				shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_AIRBRAKE]), false);
				shipC->setEquipment(FileLoader::LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_ENGINE]), false);
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
	
	list<vector<string>> enemypoolConfig = *(FileLoaderUtils::FileLoader(ENEMYPOOL_FILE));

	for (std::list<vector<string>>::iterator it = (enemypoolConfig).begin(); it != (enemypoolConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			//TODO
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find EnemyPool '%s'. Please check the config file",name));
}

EnemyBase* FileLoader::LoadEnemy(string name, int probability, int poolSize, int enemyClass)
{
	list<vector<string>> enemyConfig = *(FileLoaderUtils::FileLoader(ENEMY_FILE));

	for (std::list<vector<string>>::iterator it = (enemyConfig).begin(); it != (enemyConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			EnemyBase* base = new EnemyBase;
			base->enemy = new Enemy(sf::Vector2f(0,0),sf::Vector2f(0, stoi((*it)[EnemyData::ENEMY_SPEED])),(*it)[EnemyData::ENEMY_IMAGE_NAME],sf::Vector2f(stoi((*it)[EnemyData::ENEMY_WIDTH]),stoi((*it)[EnemyData::ENEMY_HEIGHT])), LoadFX((*it)[EnemyData::ENEMY_FX_DEATH]));
			base->probability = probability;
			base->poolsize = poolSize;
			base->enemyclass = enemyClass;

			((Independant*)base->enemy)->armor = stoi((*it)[EnemyData::ENEMY_ARMOR]);
			((Independant*)base->enemy)->shield = ((Independant*)base->enemy)->shield_max = stoi((*it)[EnemyData::ENEMY_SHIELD]);
			((Independant*)base->enemy)->shield_regen = stoi((*it)[EnemyData::ENEMY_SHIELD_REGEN]);
			((Independant*)base->enemy)->damage = stoi((*it)[EnemyData::ENEMY_DAMAGE]);
			((Independant*)base->enemy)->setMoney(stoi((*it)[EnemyData::ENEMY_VALUE]));
			((Independant*)base->enemy)->display_name = (*it)[EnemyData::ENEMY_NAME];
			
			base->enemy->angspeed = stoi((*it)[EnemyData::ENEMY_ANGSPEED]);
			base->enemy->radius = stoi((*it)[EnemyData::ENEMY_RADIUS]);
			

			if ((*it)[EnemyData::ENEMY_WEAPON].compare("0") != 0)
			{
				base->enemy->weapon = FileLoader::LoadWeapon((*it)[EnemyData::ENEMY_WEAPON], 1, FileLoader::LoadAmmo((*it)[EnemyData::ENEMY_AMMO]));
				base->enemy->hasWeapon = true;
			}

			//Loading movement pattern
			vector<float>* v = new vector<float>;
			v->push_back(base->enemy->radius); // rayon
			v->push_back(1);  // clockwise (>)
			
			PatternType pattern_type = PatternType::NoMovePattern;
			if ((*it)[EnemyData::ENEMY_PATTERN].compare("circle") == 0)
				pattern_type = PatternType::Circle_;
			if ((*it)[EnemyData::ENEMY_PATTERN].compare("oscillator") == 0)
				pattern_type = PatternType::Oscillator;
			
			base->enemy->Pattern.SetPattern(pattern_type, base->enemy->angspeed, v); //vitesse angulaire (degres/s)

			return base;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Enemy '%s'. Please check the config file",name));
}

Weapon* FileLoader::LoadWeapon(string name, int fire_direction, Ammo* ammo)
{
	
	list<vector<string>> weaponConfig = *(FileLoaderUtils::FileLoader(WEAPON_FILE));

	for (std::list<vector<string>>::iterator it = (weaponConfig).begin(); it != (weaponConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			Weapon* weapon = new Weapon(ammo);
			weapon->display_name = (*it)[WeaponData::WEAPON_NAME];
			weapon->fire_direction = Vector2i(0,fire_direction);
			weapon->rate_of_fire = atof((*it)[WeaponData::WEAPON_RATE_OF_FIRE].c_str());
			weapon->multishot = stoi((*it)[WeaponData::WEAPON_MULTISHOT]);
			weapon->xspread = stoi((*it)[WeaponData::WEAPON_XSPREAD]);
			weapon->alternate = (bool)(stoi((*it)[WeaponData::WEAPON_ALTERNATE]));
			weapon->dispersion = stoi((*it)[WeaponData::WEAPON_DISPERSION]);
			weapon->rafale = stoi((*it)[WeaponData::WEAPON_RAFALE]);
			if (weapon->rafale != 0)
				weapon->rafale_cooldown = atof((*it)[WeaponData::WEAPON_RAFALE_COOLDOWN].c_str());

			weapon->textureName = (*it)[WeaponData::WEAPON_IMAGE_NAME];
			weapon->size = sf::Vector2f(stoi((*it)[WeaponData::WEAPON_WIDTH]), stoi((*it)[WeaponData::WEAPON_HEIGHT]));
			weapon->frameNumber = stoi((*it)[WeaponData::WEAPON_FRAMES]);
			weapon->target_seaking = (bool)(stoi((*it)[WeaponData::WEAPON_TARGET_SEAKING]));

			return weapon;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Weapon '%s'. Please check the config file",name));

}

Ammo* FileLoader::LoadAmmo(string name)
{
	list<vector<string>> ammoConfig = *(FileLoaderUtils::FileLoader(AMMO_FILE));

	for (std::list<vector<string>>::iterator it = (ammoConfig).begin(); it != (ammoConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			Ammo* new_ammo = new Ammo(Vector2f(0,0), Vector2f(0,stoi((*it)[AmmoData::AMMO_SPEED])), (*it)[AmmoData::AMMO_IMAGE_NAME], 
				Vector2f(stoi((*it)[AmmoData::AMMO_WIDTH]),stoi((*it)[AmmoData::AMMO_HEIGHT])), stoi((*it)[AmmoData::AMMO_DAMAGE]), LoadFX((*it)[AmmoData::AMMO_FX]));
			new_ammo->display_name = (*it)[AmmoData::AMMO_NAME];
			new_ammo->radius = stoi((*it)[AmmoData::AMMO_RADIUS]);
			new_ammo->angspeed = stoi((*it)[AmmoData::AMMO_ANGSPEED]);

			//Loading movement pattern
			vector<float>* v = new vector<float>;
			v->push_back(new_ammo->radius); // rayon
			v->push_back(1);  // clockwise (>)

			PatternType pattern_type = PatternType::NoMovePattern;
			if ((*it)[AmmoData::AMMO_PATTERN].compare("circle") == 0)
				pattern_type = PatternType::Circle_;
			if ((*it)[AmmoData::AMMO_PATTERN].compare("oscillator") == 0)
				pattern_type = PatternType::Oscillator;

			new_ammo->Pattern.SetPattern(pattern_type, new_ammo->angspeed, v); //vitesse angulaire (degres/s)

			return new_ammo;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file",name));
}

FX* FileLoader::LoadFX(string name)
{
	list<vector<string>>FXConfig = *(FileLoaderUtils::FileLoader(FX_FILE));

	for (std::list<vector<string>>::iterator it = (FXConfig).begin(); it != (FXConfig).end(); it++)
	{
		if((*it)[FXData::FX_TYPE].compare("explosion") == 0)
		{
			if((*it)[FXData::FX_NAME].compare(name) == 0)
			{
				float duration = atof(((*it)[FXData::FX_DURATION]).c_str());
				FX* myFX = new FX(Vector2f(0,0), Vector2f(0,0), (*it)[FXData::FX_FILENAME], Vector2f(stoi((*it)[FXData::FX_WIDTH]),stoi((*it)[FXData::FX_HEIGHT])), stoi((*it)[FXData::FX_FRAMES]), sf::seconds(duration));
				myFX->display_name = (*it)[FXData::FX_NAME];

				return myFX;
			}
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find FX '%s'. Please check the config file",name));

}

Equipment* FileLoader::LoadEquipment(string name)
{
	list<vector<string>>equipmentConfig = *(FileLoaderUtils::FileLoader(EQUIPMENT_FILE));

	for (std::list<vector<string>>::iterator it = (equipmentConfig).begin(); it != (equipmentConfig).end(); it++)
	{
		if((*it)[EquipmentData::EQUIPMENT_NAME].compare(name) == 0)
		{
			Equipment* i = new Equipment();

			i-> Init(EquipmentType::Airbrake, Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED_X]),stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED_Y])), 
				stoi((*it)[EquipmentData::EQUIPMENT_DECCELERATION]), Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION_X]), stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION_Y])),
				stoi((*it)[EquipmentData::EQUIPMENT_ARMOR]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD_REGEN]),
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

			if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("airbrake") == 0)
				i->equipmentType = EquipmentType::Airbrake;
			else if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("engine") == 0)
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
	list<vector<string>> equipmentConfig = *(FileLoaderUtils::FileLoader(EQUIPMENT_FILE));

	for (std::list<vector<string>>::iterator it = (equipmentConfig).begin(); it != (equipmentConfig).end(); it++)
	{
		if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("shipmodel") == 0)
		{
			if((*it)[EquipmentData::EQUIPMENT_NAME].compare(name) == 0)
			{
				ShipModel* s = new ShipModel(Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED_X]),stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED_Y])), 
					Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION_X]), stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION_Y])),stoi((*it)[EquipmentData::EQUIPMENT_DECCELERATION]), 
					stoi((*it)[EquipmentData::EQUIPMENT_ARMOR]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD_REGEN]),
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
	list<vector<string>>botConfig = *(FileLoaderUtils::FileLoader(BOT_FILE));

	for (std::list<vector<string>>::iterator it = (botConfig).begin(); it != (botConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			Bot* bot = new Bot(Vector2f(0,0), Vector2f(0,0),(*it)[BotData::BOT_IMAGE_NAME],sf::Vector2f(stoi((*it)[BotData::BOT_WIDTH]),stoi((*it)[BotData::BOT_HEIGHT])));

			((Independant*)bot)->display_name = (*it)[BotData::BOT_NAME];
			((Independant*)bot)->armor = stoi((*it)[BotData::BOT_ARMOR]);
			((Independant*)bot)->shield = ((Independant*)bot)->shield_max = stoi((*it)[BotData::BOT_SHIELD]);
			((Independant*)bot)->shield_regen = stoi((*it)[BotData::BOT_SHIELD_REGEN]);
			((Independant*)bot)->damage = stoi((*it)[BotData::BOT_DAMAGE]);
			bot->radius = stoi((*it)[BotData::BOT_RADIUS]);
			bot->angspeed = stoi((*it)[BotData::BOT_ANGSPEED]);
			bot->spread = Vector2f(stoi((*it)[BotData::BOT_XSPREAD]), stoi((*it)[BotData::BOT_YSPREAD]));

			//Loading movement pattern
			vector<float>* v = new vector<float>;
			v->push_back(bot->radius); // rayon
			v->push_back(1);  // clockwise (>)

			PatternType pattern_type = PatternType::NoMovePattern;
			if((*it)[BotData::BOT_PATTERN].compare("circle") == 0)
				pattern_type = PatternType::Circle_;
			if((*it)[BotData::BOT_PATTERN].compare("oscillator") == 0)
				pattern_type = PatternType::Oscillator;

			bot->Pattern.SetPattern(pattern_type,bot->angspeed,v); //vitesse angulaire (degres/s)

			if ((*it)[BotData::BOT_WEAPON].compare("0") != 0)
			{
				bot->weapon = FileLoader::LoadWeapon((*it)[BotData::BOT_WEAPON], -1, FileLoader::LoadAmmo((*it)[BotData::BOT_AMMO]));
				bot->hasWeapon=true;
			}

			return bot;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Bot '%s'. Please check the config file",name));
}