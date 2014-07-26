#include "Scene.h"

extern Game* CurrentGame;


void Scene::LoadSceneFromFile(string name)
{
	LOGGER_WRITE(Logger::Priority::DEBUG,"Loading Scene");
	scrolling_direction.x = 0;
	scrolling_direction.y = 1;
	vspeed = + 10;
	hazard_break_value = 0;
	for (int i=0; i<EnemyClass::NBVAL_EnemyClass;i++)
	{
		this->total_class_probability[i] = 0;
	}
	try {

		this->config = *(FileLoaderUtils::FileLoader(name));
		this->enemyConfig = *(FileLoaderUtils::FileLoader(ENEMY_FILE));
		this->weaponConfig = *(FileLoaderUtils::FileLoader(WEAPON_FILE));
		this->ammoConfig = *(FileLoaderUtils::FileLoader(AMMO_FILE));
		this->enemypoolConfig = *(FileLoaderUtils::FileLoader(ENEMYPOOL_FILE));
		this->FXConfig = *(FileLoaderUtils::FileLoader(FX_FILE));
		this->shipConfig = *(FileLoaderUtils::FileLoader(SHIP_FILE));
		this->botConfig = *(FileLoaderUtils::FileLoader(BOT_FILE));

		int p = 0;
		//enemies
		for (std::list<vector<string>>::iterator it = (this->config).begin(); it != (this->config).end(); it++)
		{
			if((*it)[0].compare("bg") == 0)
			{
				this->bg = new Independant(sf::Vector2f(0, -stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])),sf::Vector2f(0, vspeed),(*it)[SceneDataBackground::BACKGROUND_NAME],Vector2f(stoi((*it)[SceneDataBackground::BACGKROUND_WIDTH]),stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])),Vector2f(0,0));
				bg->setVisible(true);
			}

			if((*it)[0].compare("hub") == 0)
			{
				this->hub = new Independant(sf::Vector2f(0,0),sf::Vector2f(0,vspeed),(*it)[SceneDataBackground::BACKGROUND_NAME],Vector2f(stoi((*it)[SceneDataBackground::BACGKROUND_WIDTH]),stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])),Vector2f(0,0));
				hub->setVisible(true);
			}

			if((*it)[0].compare("enemy") == 0)
			{
				EnemyBase* e = LoadEnemy((*it)[SceneDataEnemy::ENEMY],stoi((*it)[SceneDataEnemy::ENEMY_PROBABILITY].c_str()),stoi((*it)[SceneDataEnemy::ENEMY_POOLSIZE]), stoi((*it)[SceneDataEnemy::ENEMY_CLASS]));
				this->sceneIndependantsLayered[e->enemyclass].push_back(e);

				//giving intervall of hit values for dice rolls
				p++;
				e->proba_min = p;
				p += (e->probability - 1);
				e->proba_max = p;
				this->total_class_probability[e->enemyclass] = e->proba_max;

				//this->enemies.push_back(*e);//legacy, to delete when pools are done ?
				this->enemies_ranked_by_class[e->enemyclass].push_back(*e);
				//hazard value automatic calculation
				hazard_break_value += e->enemy->getMoney() * e->poolsize * HAZARD_BREAK_RATIO;
			}
			printf("Hazard Break to reach: %d\n", hazard_break_value);
		}
	}
	catch( const std::exception & ex ) 
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}

	//Hazard feature (scoring system)
	hazardBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 0));
	hazardBar.setFillColor(sf::Color(250, 0, 50));//red
	hazardBar.setOutlineThickness(4);
	hazardBar.setOutlineColor(sf::Color(255, 255, 255));
	hazardBar.setOrigin(0, 0);
	hazardBar.setPosition(WINDOW_RESOLUTION_X-40, 10);

	//hazardBarMax.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, hazard_break_value));
	hazardBarMax.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, HAZARD_BAR_SIZE_Y));
	hazardBarMax.setFillColor(sf::Color::Transparent);//black
	hazardBarMax.setOutlineThickness(4);
	hazardBarMax.setOutlineColor(sf::Color(255, 255, 255));
	hazardBarMax.setOrigin(0, 0);
	hazardBarMax.setPosition(WINDOW_RESOLUTION_X-40, 10);

}

Scene::Scene(string name)
{
	transitionDestination = TransitionList::NO_TRANSITION;

	for (int b=0; b<SceneBooleans::NBVAL_SceneBooleans; b++)
	{
		phaseShifter[b] = false;
	}

	LoadSceneFromFile(name);

	LOGGER_WRITE(Logger::Priority::DEBUG,"Loading ship config file");

	try 
	{
		this->FXConfig = *(FileLoaderUtils::FileLoader(FX_FILE));
		this->equipmentConfig = *(FileLoaderUtils::FileLoader(EQUIPMENT_FILE));
		this->weaponConfig = *(FileLoaderUtils::FileLoader(WEAPON_FILE));
		this->ammoConfig = *(FileLoaderUtils::FileLoader(AMMO_FILE));
		this->botConfig = *(FileLoaderUtils::FileLoader(BOT_FILE));
		this->shipConfig = *(FileLoaderUtils::FileLoader(SHIP_FILE));

		//Loading font for framerate
		//TODO : refactor this
		sf::Font* font = new sf::Font();
		if (!font->loadFromFile("Assets/Fonts/arial.ttf"))
		{
			// error
			//TODO: font loader
		}

		this->framerate = new sf::Text("00", *font, 15);
		this->framerate->setColor(sf::Color::Yellow);
		this->framerate->setStyle(sf::Text::Bold);
		this->framerate->setPosition(WINDOW_RESOLUTION_X-80,WINDOW_RESOLUTION_Y-30);

		sf::Font* font2 = new sf::Font();
		if (!font2->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
		{
			// error
			//TODO: font loader
		}
		this->hazardBreakText = new sf::Text("Hazard\nBreak", *font2, 12);
		this->hazardBreakText->setColor(sf::Color::Red);
		this->hazardBreakText->setStyle(sf::Text::Bold);
		this->hazardBreakText->setPosition(WINDOW_RESOLUTION_X-100,HAZARD_BAR_SIZE_Y+20);

		this->hazardBreakScore = new sf::Text("", *font2, 15);
		this->hazardBreakScore->setColor(sf::Color::Red);
		this->hazardBreakScore->setStyle(sf::Text::Regular);
		this->hazardBreakScore->setPosition(SCENE_SIZE_X-100,HAZARD_BAR_SIZE_Y+46);
	}

	catch( const std::exception & ex ) 
	{

		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}

	//Player ship
	//this->playerShip = new Ship(Vector2f(400,500), *shipConf);
	this->playerShip = new Ship(Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO,SCENE_SIZE_Y*STARTSCENE_Y_RATIO), *LoadShipConfig("default"));
	
	printf("DEBUG: SHIP LOADED.\n");
	
}

void Scene::StartGame(sf::RenderWindow*	window)
{
	this->mainWindow = window;
	(*CurrentGame).init(window);

	//bg
	bg->setPosition(0,bg->getPosition().y + SCENE_SIZE_Y); // this is an exception for the 1st level of the game only
	hub->setPosition(0,hub->getPosition().y + SCENE_SIZE_Y);
	(*CurrentGame).addToScene(bg,LayerType::BackgroundLayer,IndependantType::Background);
	(*CurrentGame).addToScene(hub,LayerType::BackgroundLayer,IndependantType::Background);

	//ship
	(*CurrentGame).addToScene(playerShip,LayerType::PlayerShipLayer, IndependantType::PlayerShip);
	playerShip->GenerateBots(playerShip);

}

void Scene::Update(Time deltaTime)
{
	// end scene animation
	EndSceneAnimation(ENDSCENE_TRANSITION_SPEED_UP, ENDSCENE_TRANSITION_SPEED_DOWN);

	//Hub roaming
	hubRoaming();

	//Exit hub
	ExitHubTransition(ENDSCENE_TRANSITION_SPEED_UP, ENDSCENE_TRANSITION_SPEED_DOWN);

	//Random enemy generation
	if (!phaseShifter[SceneBooleans::ENDSCENE_PHASE1])
	{
		this->GenerateEnemies(deltaTime);
	}

	(*CurrentGame).updateScene(deltaTime);
	mainWindow->clear();
	(*CurrentGame).drawScene();

	//TODO: refactor these
	mainWindow->draw(this->playerShip->ship_hud.armorBar);
	mainWindow->draw(this->playerShip->ship_hud.shieldBar);
	mainWindow->draw(this->playerShip->ship_hud.HazardScore);

	if (hazard_break_value != 0)
		hazardBar.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, ((*CurrentGame).getHazard()*HAZARD_BAR_SIZE_Y)/hazard_break_value));
	else
		printf("DEBUG: <error> HazardBar cannot be computed because hazard break value = 0)\n");
	ostringstream ss;
	ostringstream ss2;
	ss << (*CurrentGame).getHazard();
	ss2 << hazard_break_value;

	if ((*CurrentGame).getHazard() > hazard_break_value) // max constraint
	{
		hazardBar.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, HAZARD_BAR_SIZE_Y));
		ss << hazard_break_value;
	}

	hazardBreakScore->setString(ss.str() + "/" + ss2.str());

	mainWindow->draw(hazardBarMax);
	mainWindow->draw(hazardBar);
	mainWindow->draw(*(this->hazardBreakText));
	mainWindow->draw(*(this->hazardBreakScore));
	if ((*CurrentGame).getHazard() > hazard_break_value - 1)//hazard break event
	{
		HazardBreakEvent();
	}

	//Show framerate
	this->framerate->setString(TextUtils::format("fps=%.0f", 1 / (deltaTime.asMilliseconds() * 0.001)));
	mainWindow->draw(*(this->framerate));

	mainWindow->display();
}

Ship* Scene::GetPlayerShip()
{
	return this->playerShip;
}


ShipConfig* Scene::LoadShipConfig(string name)
{
	for (std::list<vector<string>>::iterator it = (this->shipConfig).begin(); it != (this->shipConfig).end(); it++)
	{
		if((*it)[ShipConfigData::SHIPCONFIG_NAME].compare(name) == 0)
		{
			ShipConfig* shipC = new ShipConfig();

			//Loading Ship Model
			printf("DEBUG: Loading ship model\n");
			shipC->setShipModel(LoadShipModel((*it)[ShipConfigData::SHIPCONFIG_SHIPMODEL]));

			//Loading equipment
			printf("DEBUG: Loading ship equipment\n");
			shipC->setEquipment(LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_AIRBRAKE]));
			shipC->setEquipment(LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_ENGINE]));
			shipC->setEquipment(LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_MODULE]));
			shipC->setEquipment(LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_ARMOR]));
			shipC->setEquipment(LoadEquipment((*it)[ShipConfigData::SHIPCONFIG_SHIELD]));

			//Loading bot
			shipC->bot = LoadBot(shipC->equipment[EquipmentType::Module]->botName);

			//Loading weapon
			printf("DEBUG: Loading ship weapon\n");
			shipC->setShipWeapon(LoadWeapon((*it)[ShipConfigData::SHIPCONFIG_WEAPON], -1, LoadAmmo((*it)[ShipConfigData::SHIPCONFIG_AMMO])));

			//Computing the ship config
			shipC->Init();

			return shipC;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file",name));
}

EnemyPool* Scene::LoadEnemyPool(string name)
{
	for (std::list<vector<string>>::iterator it = (this->enemypoolConfig).begin(); it != (this->enemypoolConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			//TODO
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find EnemyPool '%s'. Please check the config file",name));
}

EnemyBase* Scene::LoadEnemy(string name, int probability, int poolSize, int enemyClass)
{
	for (std::list<vector<string>>::iterator it = (this->enemyConfig).begin(); it != (this->enemyConfig).end(); it++)
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

			base->enemy->weapon = LoadWeapon((*it)[EnemyData::ENEMY_WEAPON], 1, LoadAmmo((*it)[EnemyData::ENEMY_AMMO]));

			return base;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Enemy '%s'. Please check the config file",name));
}

Weapon* Scene::LoadWeapon(string name, int fire_direction, Ammo* ammo)
{
	for (std::list<vector<string>>::iterator it = (this->weaponConfig).begin(); it != (this->weaponConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			Weapon* weapon = new Weapon(ammo);
			weapon->fire_direction = Vector2i(0,fire_direction);
			weapon->rate_of_fire = atof((*it)[WeaponData::WEAPON_RATE_OF_FIRE].c_str());
			weapon->multishot = stoi((*it)[WeaponData::WEAPON_MULTISHOT]);
			weapon->xspread = stoi((*it)[WeaponData::WEAPON_XSPREAD]);
			weapon->alternate = (bool)(stoi((*it)[WeaponData::WEAPON_ALTERNATE]));
			weapon->dispersion = stoi((*it)[WeaponData::WEAPON_DISPERSION]);
			weapon->rafale = stoi((*it)[WeaponData::WEAPON_RAFALE]);
			if (weapon->rafale != 0)
				weapon->rafale_cooldown = stoi((*it)[WeaponData::WEAPON_RAFALE_COOLDOWN]);

			return weapon;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Weapon '%s'. Please check the config file",name));

}

Ammo* Scene::LoadAmmo(string name)
{
	for (std::list<vector<string>>::iterator it = (this->ammoConfig).begin(); it != (this->ammoConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			return new Ammo(Vector2f(0,0), Vector2f(0,stoi((*it)[AmmoData::AMMO_SPEED])), (*it)[AmmoData::AMMO_IMAGE_NAME], Vector2f(stoi((*it)[AmmoData::AMMO_WIDTH]),stoi((*it)[AmmoData::AMMO_HEIGHT])), stoi((*it)[AmmoData::AMMO_DAMAGE]), LoadFX((*it)[AmmoData::AMMO_FX]));
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file",name));
}

FX* Scene::LoadFX(string name)
{
	for (std::list<vector<string>>::iterator it = (this->FXConfig).begin(); it != (this->FXConfig).end(); it++)
	{
		if((*it)[0].compare("explosion") == 0)
		{
			return new FX(Vector2f(0,0), Vector2f(0,0), (*it)[FXData::FX_FILENAME], Vector2f(stoi((*it)[FXData::FX_WIDTH]),stoi((*it)[FXData::FX_HEIGHT])), stoi((*it)[FXData::FX_FRAMES]), sf::seconds(stoi((*it)[FXData::FX_DURATION])));
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find FX '%s'. Please check the config file",name));

}

Equipment* Scene::LoadEquipment(string name)
{
	for (std::list<vector<string>>::iterator it = (this->equipmentConfig).begin(); it != (this->equipmentConfig).end(); it++)
	{
		if((*it)[EquipmentData::EQUIPMENT_NAME].compare(name) == 0)
		{
			Equipment* i = new Equipment();

			i-> Init(EquipmentType::Empty, Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED_X]),stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED_Y])), 
				stoi((*it)[EquipmentData::EQUIPMENT_DECCELERATION]), Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION_X]), stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION_Y])),
				stoi((*it)[EquipmentData::EQUIPMENT_ARMOR]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD_REGEN]),
				(*it)[EquipmentData::EQUIPMENT_IMAGE_NAME], Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_WIDTH]), stoi((*it)[EquipmentData::EQUIPMENT_HEIGHT])),
				stoi((*it)[EquipmentData::EQUIPMENT_FRAMES]));

			if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("airbrake") == 0)
				i->equipmentType = EquipmentType::Airbrake;
			if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("engine") == 0)
				i->equipmentType = EquipmentType::Engine;
			if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("armor") == 0)
				i->equipmentType = EquipmentType::Armor;
			if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("shield") == 0)
				i->equipmentType = EquipmentType::Shield;
			if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("module") == 0)
			{
				i->equipmentType = EquipmentType::Module;
				i->botName = (*it)[EquipmentData::EQUIPMENT_BOT];
			}
			if (i->equipmentType == EquipmentType::Empty)
				LOGGER_WRITE(Logger::Priority::DEBUG,"Equipment config file error: cannot find a valid equipment type for: '%s'. Please check the config file",name);

			

			return i;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Equipment '%s'. Please check the config file",name));
}

ShipModel* Scene::LoadShipModel(string name)
{
	for (std::list<vector<string>>::iterator it = (this->equipmentConfig).begin(); it != (this->equipmentConfig).end(); it++)
	{
		if((*it)[EquipmentData::EQUIPMENT_COMPARE].compare("shipmodel") == 0)
		{
			if((*it)[EquipmentData::EQUIPMENT_NAME].compare(name) == 0)
			{
				ShipModel* s = new ShipModel(Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED_X]),stoi((*it)[EquipmentData::EQUIPMENT_MAXSPEED_Y])), 
					Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION_X]), stoi((*it)[EquipmentData::EQUIPMENT_ACCELERATION_Y])),stoi((*it)[EquipmentData::EQUIPMENT_DECCELERATION]), 
					stoi((*it)[EquipmentData::EQUIPMENT_ARMOR]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD]), stoi((*it)[EquipmentData::EQUIPMENT_SHIELD_REGEN]),
					(*it)[EquipmentData::EQUIPMENT_IMAGE_NAME], Vector2f(stoi((*it)[EquipmentData::EQUIPMENT_WIDTH]), stoi((*it)[EquipmentData::EQUIPMENT_HEIGHT])), stoi((*it)[EquipmentData::EQUIPMENT_FRAMES]));

				return s;

			}	
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find ShipModel '%s'. Please check the config file",name));
}

Bot* Scene::LoadBot(string name)
{
	for (std::list<vector<string>>::iterator it = (this->botConfig).begin(); it != (this->botConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			Bot* bot = new Bot(Vector2f(0,0), Vector2f(0,0),(*it)[BotData::BOT_IMAGE_NAME],sf::Vector2f(stoi((*it)[BotData::BOT_WIDTH]),stoi((*it)[BotData::BOT_HEIGHT])));

			((Independant*)bot)->armor = stoi((*it)[BotData::BOT_ARMOR]);
			((Independant*)bot)->shield = ((Independant*)bot)->shield_max = stoi((*it)[BotData::BOT_SHIELD]);
			((Independant*)bot)->shield_regen = stoi((*it)[BotData::BOT_SHIELD_REGEN]);
			((Independant*)bot)->damage = stoi((*it)[BotData::BOT_DAMAGE]);
			bot->radius = stoi((*it)[BotData::BOT_RADIUS]);
			bot->vspeed = stoi((*it)[BotData::BOT_SPEED]);

			vector<float>* v = new vector<float>;
			v->push_back(bot->radius); // rayon 500px
			v->push_back(1);  // clockwise (>)
			bot->Pattern.SetPattern(PatternType::Circle_,bot->vspeed,v); //vitesse angulaire (degres/s)

			bot->weapon = LoadWeapon((*it)[BotData::BOT_WEAPON], -1, LoadAmmo((*it)[BotData::BOT_AMMO]));
			bot->hasWeapon=true;
			
			return bot;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Bot '%s'. Please check the config file",name));
}

void Scene::GenerateEnemies(Time deltaTime)
{
	static double timer = 0;
	timer += deltaTime.asSeconds();
	if(timer > 3)
	{
		double intpart;
		timer = modf (timer, &intpart);

		double random_number = ((double) rand() / (RAND_MAX));

		// A PASSER EN .CSV :
		int nb_rows = 3;
		int nb_lines = 2;
		float xspread = 200;
		float yspread = 50;
		// liste de classes d'ennemis : alpha, alpha, alpha, alpha, alpha
		// liste de patterns associés : 0, 0, 0, 0, 0, 

		//chosing a random enemy within a class of enemies
		Enemy* random_enemy_within_class[EnemyClass::NBVAL_EnemyClass];


		int dice_roll = (rand() % (total_class_probability[EnemyClass::ENEMYPOOL_ALPHA]))+1;
		for (int i=0; i<EnemyClass::NBVAL_EnemyClass; i++)
		{
			for (std::list<EnemyBase>::iterator it = enemies_ranked_by_class[i].begin() ; it != enemies_ranked_by_class[i].end(); ++it)
			{
				if (dice_roll >= it->proba_min && dice_roll <= it->proba_max)
				{
					random_enemy_within_class[i] = (*it).enemy;
				}
			}
		}


		//preparing the list of enemies to put in the cluster
		vector<EnemyPoolElement*>* cluster = new vector<EnemyPoolElement*>;
		for (int i=0 ; i< nb_rows*nb_lines; i++)
		{
			//VALEURS A CONF EN .CSV
			// arg0 = enemy class
			// arg1 = move pattern
			//if arg0 != VOID
			EnemyPoolElement* e = new EnemyPoolElement(random_enemy_within_class[EnemyClass::ENEMYPOOL_ALPHA], EnemyClass::ENEMYPOOL_ALPHA,PatternType::NoMovePattern);

			enemies_ranked_by_class[EnemyClass::ENEMYPOOL_ALPHA].begin()->poolsize --;

			cluster->push_back(e);
		}

		int size_x = (int)((nb_rows+1) * xspread);//let's take some margin : +1 row
		float size_y = (nb_lines+1) * yspread;//let's take some margin : +1 line
		if (size_x >= SCENE_SIZE_X) size_x = SCENE_SIZE_X-1;//without this, rand() % SCENE_SIZE_X - size_x may crash if size_x = SCENE_SIZE_X
		if (size_y >= SCENE_SIZE_Y) size_y = SCENE_SIZE_Y-1;

		EnemyPool* generated_cluster = new EnemyPool(sf::Vector2f(rand() % (SCENE_SIZE_X - size_x), - size_y), nb_lines, nb_rows, xspread, yspread, cluster);

		// OLD MAIS FONCTIONNEL = BACKUP
		/*
		for (std::list<EnemyBase>::iterator it = (this->enemies).begin(); it != (this->enemies).end(); it++)
		{
		if(it->probability > random_number && it->poolsize > 0)
		{
		//spawn (where on screen ?)

		Enemy* n = it->enemy->Clone();
		n->setPosition(rand() % SCENE_SIZE_X,-n->m_size.y*2);
		n->setVisible(true);

		//Pattern definition snipet
		//vector<float>* v = new vector<float>;
		//v->push_back(60); // rayon 60px
		//v->push_back(1);  // clockwise (>)
		//n->Pattern.SetPattern(PatternType::Circle_, 100,v); //vitesse angulaire (degres/s)

		it->poolsize--;

		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("spawning enemy '%s' (x=%.1f) [pool=%d] (class=%d)",it->enemy->getName().c_str(),n->getPosition().x,it->poolsize, it->enemyclass));

		(*CurrentGame).addToScene((Independant*)n,LayerType::EnemyObjectLayer,IndependantType::EnemyObject);
		}
		}
		*/
	}
}

void Scene::EndSceneAnimation(float transition_UP, float transition_DOWN)
{
	if (!phaseShifter[SceneBooleans::ENDSCENE_PHASE1] && !phaseShifter[SceneBooleans::ENDSCENE_PHASE4] && bg->getPosition().y > 0)
	{
		bg->setPosition(sf::Vector2f(0, 0));

		bg->speed.y = 0;
		hub->speed.y = 0;

		phaseShifter[SceneBooleans::ENDSCENE_PHASE1] = true;
		phaseShifter[SceneBooleans::ENDSCENE_PHASE2] = false;
		phaseShifter[SceneBooleans::ENDSCENE_PHASE3] = false;
		phaseShifter[SceneBooleans::ENDSCENE_PHASE4] = false;
		//to do better:
		hub->setPosition(sf::Vector2f(bg->getPosition().x,bg->getPosition().y - SCENE_SIZE_Y));

	}

	//end scene Phase 2: scene is cleared, the player is set on rails towards the top of the screen
	if (phaseShifter[SceneBooleans::ENDSCENE_PHASE1] && !phaseShifter[SceneBooleans::ENDSCENE_PHASE2] && !phaseShifter[SceneBooleans::ENDSCENE_PHASE4] && (*CurrentGame).isLastEnemyDead())
	{
		playerShip->disable_inputs = true;
		playerShip->disable_fire = true;
		playerShip->speed.x = 0;
		playerShip->speed.y = -transition_UP;
		phaseShifter[SceneBooleans::ENDSCENE_PHASE2] = true;//speed of translation towards UP
	}

	//end scene Phase 3: the playership reached the top of the screen, now background and HUB are switching
	if (phaseShifter[SceneBooleans::ENDSCENE_PHASE2] && !phaseShifter[SceneBooleans::ENDSCENE_PHASE3] && !phaseShifter[SceneBooleans::ENDSCENE_PHASE4] && playerShip->getPosition().y < playerShip->ship_config.size.y) //playership reaches the top of the screen
	{
		bg->speed.y = transition_DOWN;
		hub->speed.y = transition_DOWN;

		playerShip->setPosition(playerShip->getPosition().x, playerShip->ship_config.size.y);

		//center the player at the required speed i.e. proportionally to the speed at which the background fades out
		playerShip->speed.x = transition_DOWN * ((SCENE_SIZE_X/2) - playerShip->getPosition().x) / SCENE_SIZE_Y;
		playerShip->speed.y = transition_DOWN * ((SCENE_SIZE_Y/2) - playerShip->getPosition().y) / SCENE_SIZE_Y;
		phaseShifter[SceneBooleans::ENDSCENE_PHASE3] = true;
	}

	//end scene Phase 4: the HUB is now full screen, the players are given back the control of their ship
	if (phaseShifter[SceneBooleans::ENDSCENE_PHASE3] && !phaseShifter[SceneBooleans::ENDSCENE_PHASE4] && hub->getPosition().y > 0)
	{
		hub->speed.y = 0;
		bg->speed.y = 0;
		hub->setPosition(sf::Vector2f(0,0));

		playerShip->disable_inputs = false;
		phaseShifter[SceneBooleans::ENDSCENE_PHASE4] = true;
		//printf("hub pos: %f / %f \n", hubClone->getPosition().x, hubClone->getPosition().y);
	}
}

void Scene::hubRoaming()
{
	if (phaseShifter[SceneBooleans::ENDSCENE_PHASE4])
	{
		float x = playerShip->getPosition().x;
		float y = playerShip->getPosition().y;

		float X_min = SCENE_SIZE_X*HUB_EXIT_X_MIN_RATIO;
		float X_max = SCENE_SIZE_X*HUB_EXIT_X_MAX_RATIO;
		float Y_min = SCENE_SIZE_Y*HUB_EXIT_Y_MIN_RATIO;
		float Y_max = SCENE_SIZE_Y*HUB_EXIT_Y_MAX_RATIO;

		if ((x > X_min && x < X_max && y > Y_min && y < Y_max) || (x<X_min && y<Y_min) || (x<X_min && y >Y_max) || (x>X_max && y >Y_max) || (x>X_max && y <Y_min))
		{
			clockHubExit.restart();
			this->transitionDestination = TransitionList::NO_TRANSITION;
		}
		else
		{
			sf::Time timer = clockHubExit.getElapsedTime();

			if (y<Y_min && timer.asSeconds() > HUB_EXIT_TIMER)
			{
				//go UP
				printf("DEBUG: Travel UP !\n");
				this->transitionDestination = TransitionList::TRANSITION_UP;
				bg->setPosition(sf::Vector2f(hub->getPosition().x,hub->getPosition().y - bg->m_size.y - SCENE_SIZE_Y));

				phaseShifter[SceneBooleans::ENDSCENE_PHASE1] = false;
				phaseShifter[SceneBooleans::ENDSCENE_PHASE2] = false;
				phaseShifter[SceneBooleans::ENDSCENE_PHASE3] = false;
				phaseShifter[SceneBooleans::ENDSCENE_PHASE4] = false;
				phaseShifter[SceneBooleans::EXITHUB_PHASE1] = true;

			}

			if (x<X_min && timer.asSeconds() > HUB_EXIT_TIMER)
			{
				//go LEFT
				printf("DEBUG: Travel LEFT !\n");
				this->transitionDestination = TransitionList::TRANSITION_LEFT;
				phaseShifter[SceneBooleans::ENDSCENE_PHASE4]=false;
			}

			if (x>X_max && timer.asSeconds() > HUB_EXIT_TIMER)
			{
				//go RIGHT
				printf("DEBUG: Travel RIGHT !\n");
				this->transitionDestination = TransitionList::TRANSITION_RIGHT;
				phaseShifter[SceneBooleans::ENDSCENE_PHASE4]=false;
			}

			if (y>Y_max && timer.asSeconds() > HUB_EXIT_TIMER)
			{
				//go DOWN
				printf("DEBUG: Travel DOWN !\n");
				this->transitionDestination = TransitionList::TRANSITION_DOWN;
				phaseShifter[SceneBooleans::ENDSCENE_PHASE4]=false;
			}
		}
	}
}

void Scene::ExitHubTransition (float transition_speed_UP, float transition_speed_DOWN)
{
	if (phaseShifter[SceneBooleans::EXITHUB_PHASE1] && !phaseShifter[SceneBooleans::EXITHUB_PHASE2])
	{
		playerShip->disable_inputs = true;
		playerShip->speed.x = 0;
		playerShip->speed.y = - transition_speed_UP;
		phaseShifter[SceneBooleans::EXITHUB_PHASE1] = false;
		phaseShifter[SceneBooleans::EXITHUB_PHASE2] = true;
		phaseShifter[SceneBooleans::EXITHUB_PHASE3] = false;

	}

	if (!phaseShifter[SceneBooleans::EXITHUB_PHASE1] && phaseShifter[SceneBooleans::EXITHUB_PHASE2] && !phaseShifter[SceneBooleans::EXITHUB_PHASE3] && playerShip->getPosition().y < playerShip->ship_config.size.y)
	{
		playerShip->setPosition(playerShip->getPosition().x, playerShip->ship_config.size.y);

		bg->setPosition(0, - bg->m_size.y);
		bg->speed.y = transition_speed_DOWN;
		hub->speed.y = transition_speed_DOWN;
		//playerShip->speed.x = transition_speed_DOWN * ((SCENE_SIZE_X*STARTSCENE_X_RATIO) - playerShip->getPosition().x) / SCENE_SIZE_Y;
		playerShip->speed.y = transition_speed_DOWN * ((SCENE_SIZE_Y*STARTSCENE_Y_RATIO) - playerShip->getPosition().y) / SCENE_SIZE_Y;
		phaseShifter[SceneBooleans::EXITHUB_PHASE2] = false;
		phaseShifter[SceneBooleans::EXITHUB_PHASE3] = true;
	}

	if (!phaseShifter[SceneBooleans::EXITHUB_PHASE2] && phaseShifter[SceneBooleans::EXITHUB_PHASE3] && bg->getPosition().y > - bg->m_size.y + SCENE_SIZE_Y)
	{
		bg->setPosition(0, - bg->m_size.y + SCENE_SIZE_Y);
		bg->speed.y = vspeed;
		hub->speed.y = vspeed;
		phaseShifter[SceneBooleans::EXITHUB_PHASE3] = false;
		playerShip->speed.x = 0;
		playerShip->speed.y = 0;
		playerShip->disable_inputs = false;
		playerShip->disable_fire = false;
	}
}

bool Scene:: getPhaseShifter(int index)
{
	if (index < SceneBooleans::NBVAL_SceneBooleans)
		return phaseShifter[index];
	else
	{
		printf("\n/!\ERROR: trying to access a Scene Boolean index that does not exist...\n");
		return false;
	}

}

void Scene::setPhaseShifter(int index, bool b)
{
	if (index < SceneBooleans::NBVAL_SceneBooleans)
		phaseShifter[index] = b;
	else
	{
		printf("\n/!\ERROR: trying to write in a Scene Boolean index that does not exist...\n");
	}
}


void Scene::HazardBreakEvent()
{
	(*CurrentGame).resetHazard((*CurrentGame).getHazard() - hazard_break_value);
	printf("DEBUG: HAZARD BREAK!!!\n");
	hazard_break_value *= (1+ HAZARD_BREAK_MULTIPLIER);
}

sf::Vector2f Scene::ApplyScrollingDirectionOnPosition(sf::Vector2f position)
{
	float x = position.x;
	float y = position.y;

	if (scrolling_direction.y == -1)
	{
		x = position.x;
		y = SCENE_SIZE_Y - position.y;
	}

	if (scrolling_direction.y == 0)
	{
		if (scrolling_direction.x == 1)
		{
			x = (SCENE_SIZE_Y - position.y) * SCENE_SIZE_X/SCENE_SIZE_Y;
			y = position.x * SCENE_SIZE_Y/SCENE_SIZE_X;
		}
		if (scrolling_direction.x == -1)
		{
			x= SCENE_SIZE_X - ((SCENE_SIZE_Y - position.y) * SCENE_SIZE_X/SCENE_SIZE_Y);
			y= position.x * SCENE_SIZE_Y/SCENE_SIZE_X;
		}
	}

	return sf::Vector2f (x,y);
}

sf::Vector2f Scene::ApplyScrollingDirectionOnSpeed(float vspeed)
{
	float x = 0;
	float y = vspeed;

	if (scrolling_direction.y == -1)
	{
		x=0;
		y *= (-1);
	}


	if (scrolling_direction.y == 0)
	{
		if (scrolling_direction.x == 1)
		{
			x = - vspeed;
			y = 0;
		}
		if (scrolling_direction.x == -1)
		{
			x = vspeed;
			y = 0;
		}
	}

	return sf::Vector2f (x,y);
}