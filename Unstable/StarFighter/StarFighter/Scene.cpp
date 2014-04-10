#include "Scene.h"

extern Game* CurrentGame;

//Scene::Scene(string name, ShipConfig* shipConf)
Scene::Scene(string name)
{

	vspeed = +200;
	transitionDestination = TransitionList::NO_TRANSITION;
	endingPhase1isOver = false;
	endingPhase2isOver = false;
	endingPhase3isOver = false;
	endingPhase4isOver = false;
	exitHubPhase1isOver = false;
	exitHubPhase2isOver = false;

	LOGGER_WRITE(Logger::Priority::DEBUG,"Loading Scene");

	try {

		this->config = *(FileLoader(name));
		this->enemyConfig = *(FileLoader(ENEMY_FILE));
		this->weaponConfig = *(FileLoader(WEAPON_FILE));
		this->ammoConfig = *(FileLoader(AMMO_FILE));
		this->enemypoolConfig = *(FileLoader(ENEMYPOOL_FILE));
		this->FXConfig = *(FileLoader(FX_FILE));
		this->shipConfig = *(FileLoader(SHIP_FILE));

		//enemies
		for (std::list<vector<string>>::iterator it = (this->config).begin(); it != (this->config).end(); it++)
		{
			if((*it)[0].compare("bg") == 0)
			{
				this->bg = new Independant(sf::Vector2f(0,0),sf::Vector2f(0,vspeed),(*it)[SceneDataBackground::BACKGROUND_NAME],Vector2f(stoi((*it)[SceneDataBackground::BACGKROUND_WIDTH]),stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])),Vector2f(0,stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])-WINDOW_RESOLUTION_Y));
				bg->setVisible(true);
			}

			if((*it)[0].compare("hub") == 0)
			{
				this->hub = new Independant(sf::Vector2f(0,0),sf::Vector2f(0,vspeed),(*it)[SceneDataBackground::BACKGROUND_NAME],Vector2f(stoi((*it)[SceneDataBackground::BACGKROUND_WIDTH]),stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])),Vector2f(0,bg->m_size.y));
				hub->setVisible(true);
			}

			if((*it)[0].compare("enemy") == 0)
			{
				EnemyBase* e = LoadEnemy((*it)[SceneDataEnemy::ENEMY],atof((*it)[SceneDataEnemy::ENEMY_PROBABILITY].c_str()),stoi((*it)[SceneDataEnemy::ENEMY_POOLSIZE]), stoi((*it)[SceneDataEnemy::ENEMY_CLASS]));
				this->sceneIndependantsLayered[e->enemyclass].push_back(e);
				//legacy, to delete when pools are done
				this->enemies.push_back(*e);
			}
		}

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

	}
	catch( const std::exception & ex ) {

		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}

	//Player ship
	//this->playerShip = new Ship(Vector2f(400,500), *shipConf);
	this->playerShip = new Ship(Vector2f(WINDOW_RESOLUTION_X*STARTSCENE_X_RATIO,WINDOW_RESOLUTION_Y*STARTSCENE_Y_RATIO), *LoadShipConfig("default"));
}

void Scene::StartGame(sf::RenderWindow*	window)
{
	this->mainWindow = window;
	(*CurrentGame).init(window);

	//bg
	(*CurrentGame).addToScene(bg,LayerType::BackgroundLayer,IndependantType::Background);
	(*CurrentGame).addToScene(hub,LayerType::BackgroundLayer,IndependantType::Background);

	//ship
	(*CurrentGame).addToScene(playerShip,LayerType::PlayerShipLayer, IndependantType::PlayerShip);
	
}

void Scene::Update(Time deltaTime)
{
	// end scene animation
	EndSceneAnimation(ENDSCENE_TRANSITION_SPEED_UP, ENDSCENE_TRANSITION_SPEED_DOWN);

	//Hub roaming
	hubRoaming();

	//Exit hub
	ExitHubTransition();

	/*
	//exit HUB transition
	if (transitionDestination != TransitionList::NO_TRANSITION && !exitHubPhase1isOver)
	{
		hubExitPhase1(ENDSCENE_TRANSITION_SPEED_DOWN, transitionDestination);
	}

	//end of exit HUB transition
	if (exitHubPhase1isOver && !exitHubPhase2isOver)
	{
		hubExitPhase2();//player regains control
	}
	*/

	//Random enemy generation
	if (!endingPhase1isOver)
	{
		//this->GenerateEnemies(deltaTime);
	}

	(*CurrentGame).updateScene(deltaTime);
	mainWindow->clear();
	(*CurrentGame).drawScene();

	//TODO: refactor these
	mainWindow->draw(this->playerShip->ship_hud.armorBar);
	mainWindow->draw(this->playerShip->ship_hud.shieldBar);


	//Show framerate
	this->framerate->setString(TextUtils::format("fps=%.0f", 1 / (deltaTime.asMilliseconds() * 0.001)));
	mainWindow->draw(*(this->framerate));

	mainWindow->display();
}

Ship* Scene::GetPlayerShip()
{
	return this->playerShip;
}

/*MovePattern* Scene::LoadMovePattern(string name, float radius, float triggerY)
{
	for (std::list<vector<string>>::iterator it = (this->movepatternConfig).begin(); it != (this->movepatternConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			MovePattern* mpattern = new MovePattern(stoi((*it)[MovePatternData::MOVEPATTERN_RADIUS]), stoi((*it)[MovePatternData::MOVEPATTERN_TRIGGERY]));
			return mpattern;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find MovePattern '%s'. Please check the config file",name));
}
*/


ShipConfig* Scene::LoadShipConfig(string name)
{
	for (std::list<vector<string>>::iterator it = (this->shipConfig).begin(); it != (this->shipConfig).end(); it++)
	{
		if((*it)[0].compare("default") == 0)
		{
			ShipConfig* shipC = new ShipConfig();

			shipC->ship_model = new ShipModel(sf::Vector2f(stoi((*it)[ShipModelData::SHIPMODEL_MAXSPEED_X]),stoi((*it)[ShipModelData::SHIPMODEL_MAXSPEED_X])), sf::Vector2f(stoi((*it)[ShipModelData::SHIPMODEL_ACCELERATION_X]),stoi((*it)[ShipModelData::SHIPMODEL_ACCELERATION_Y])), stoi((*it)[ShipModelData::SHIPMODEL_DECCELERATION]), stoi((*it)[ShipModelData::SHIPMODEL_ARMOR]), stoi((*it)[ShipModelData::SHIPMODEL_SHIELD]), stoi((*it)[ShipModelData::SHIPMODEL_SHIELD_REGEN]));
			//TODO: LoadEquipment
			
			shipC->Init((*it)[ShipModelData::SHIPMODEL_IMAGE_NAME], sf::Vector2f(stoi((*it)[ShipModelData::SHIPMODEL_WIDTH]),stoi((*it)[ShipModelData::SHIPMODEL_HEIGHT])), stoi((*it)[ShipModelData::SHIPMODEL_FRAMES]));
			
			shipC->weapon = LoadWeapon((*it)[ShipModelData::SHIPMODEL_WEAPON], 1, LoadAmmo((*it)[ShipModelData::SHIPMODEL_AMMO]));

			printf("DEBUG: Loaded player ship config.\n");
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

EnemyBase* Scene::LoadEnemy(string name, float probability, int poolSize, int enemyClass)
{
	for (std::list<vector<string>>::iterator it = (this->enemyConfig).begin(); it != (this->enemyConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			EnemyBase* base = new EnemyBase;
			base->enemy = new Enemy(sf::Vector2f(0,0),sf::Vector2f(0,stoi((*it)[EnemyData::ENEMY_SPEED])),(*it)[EnemyData::ENEMY_IMAGE_NAME],sf::Vector2f(stoi((*it)[EnemyData::ENEMY_WIDTH]),stoi((*it)[EnemyData::ENEMY_HEIGHT])), LoadFX((*it)[EnemyData::ENEMY_FX_DEATH]));
			base->probability = probability;
			base->poolsize = poolSize;
			base->enemyclass = enemyClass;

			((Independant*)base->enemy)->armor = stoi((*it)[EnemyData::ENEMY_ARMOR]);
			((Independant*)base->enemy)->shield = ((Independant*)base->enemy)->shield_max = stoi((*it)[EnemyData::ENEMY_SHIELD]);
			((Independant*)base->enemy)->shield_regen = stoi((*it)[EnemyData::ENEMY_SHIELD_REGEN]);
			((Independant*)base->enemy)->damage = stoi((*it)[EnemyData::ENEMY_DAMAGE]);

			base->enemy->weapon = LoadWeapon((*it)[EnemyData::WEAPON], -1, LoadAmmo((*it)[EnemyData::AMMO]));

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
			weapon->rate_of_fire = atof((*it)[WeaponData::RATE_OF_FIRE].c_str());

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

list<vector<string>>* Scene::FileLoader(string name)
{

	list<vector<string>>* fileConfig = new list<vector<string>>;

	std::ifstream  data(name);

	std::string line;
	while(std::getline(data,line))
	{

		std::stringstream  lineStream(line);
		std::string        cell;

		std::vector<string> parameters;
		while(std::getline(lineStream,cell,','))
		{
			if(cell[0] == '#')
				goto loop_end;

			parameters.push_back(cell);
		}

		//TODO: check if not empty
		fileConfig->push_back(parameters);
loop_end:;
	}
	data.close();

	return fileConfig;
}

void Scene::GenerateEnemies(Time deltaTime)
{
	static double timer = 0;
	timer += deltaTime.asSeconds();
	if(timer > 1)
	{
		double intpart;
		timer = modf (timer, &intpart);

		double random_number = ((double) rand() / (RAND_MAX));

		for (std::list<EnemyBase>::iterator it = (this->enemies).begin(); it != (this->enemies).end(); it++)
		{
			if(it->probability > random_number && it->poolsize > 0)
			{
				//spawn (where on screen ?)
				Enemy* n = it->enemy->Clone();
				n->setPosition(rand() % WINDOW_RESOLUTION_X,-n->m_size.y*2);
				n->setVisible(true);
				it->poolsize--;

				LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("spawning enemy '%s' (x=%.1f) [pool=%d] (class=%d)",it->enemy->getName().c_str(),n->getPosition().x,it->poolsize, it->enemyclass));

				(*CurrentGame).addToScene((Independant*)n,LayerType::EnemyObjectLayer,IndependantType::EnemyObject);
			}
		}
	}
}

void Scene::EndSceneAnimation(float transition_UP, float transition_DOWN)
{
	if (!endingPhase1isOver && !endingPhase4isOver && bg->getPosition().y > bg->m_size.y - WINDOW_RESOLUTION_Y)
	{
		bg->speed.y = 0;
		hub->speed.y = 0;
		endingPhase1isOver = true;
		endingPhase2isOver = false;
		endingPhase3isOver = false;
		endingPhase4isOver = false;
	}

	//end scene Phase 2: scene is cleared, the player is set on rails towards the top of the screen
	if (endingPhase1isOver && !endingPhase2isOver && !endingPhase4isOver && (*CurrentGame).getNumberOfIndeIndependantsAlive() < 4) // ship, bg and hub are the 3 only alive independants allowed before ending the scene
	{
		playerShip->disable_inputs = true;
		playerShip->speed.x = 0;
		playerShip->speed.y = -transition_UP;
		endingPhase2isOver = true;//speed of translation towards UP
	}

	//end scene Phase 3: the playership reached the top of the screen, now background and HUB are switching
	if (endingPhase2isOver && !endingPhase3isOver && !endingPhase4isOver && playerShip->getPosition().y < playerShip->ship_config.size.y) //playership reaches the top of the screen
	{
		bg->speed.y = transition_DOWN;
		hub->speed.y = transition_DOWN;

		//center the player at the required speed i.e. proportionally to the speed at which the background fades out
		playerShip->speed.x = transition_DOWN * ((WINDOW_RESOLUTION_X/2) - playerShip->getPosition().x) / WINDOW_RESOLUTION_Y;
		playerShip->speed.y = transition_DOWN * ((WINDOW_RESOLUTION_Y/2) - playerShip->getPosition().y) / WINDOW_RESOLUTION_Y;
		endingPhase3isOver = true;
	}

	//end scene Phase 4: the HUB is now full screen, the players are given back the control of their ship
	if (endingPhase3isOver && !endingPhase4isOver && hub->getPosition().y > bg->m_size.y)
	{
		
		hub->speed.y = 0;
		bg->speed.y = 0;
		
		
		playerShip->disable_inputs = false;
		endingPhase4isOver = true;
		endingPhase1isOver = false;
		endingPhase2isOver = false;
		endingPhase3isOver = false;
		//printf("hub pos: %f / %f \n", hubClone->getPosition().x, hubClone->getPosition().y);
	}
}

void Scene::hubRoaming()
{
	if (endingPhase4isOver)
	{
		float x = playerShip->getPosition().x;
		float y = playerShip->getPosition().y;

		float X_min = WINDOW_RESOLUTION_X*HUB_EXIT_X_MIN_RATIO;
		float X_max = WINDOW_RESOLUTION_X*HUB_EXIT_X_MAX_RATIO;
		float Y_min = WINDOW_RESOLUTION_Y*HUB_EXIT_Y_MIN_RATIO;
		float Y_max = WINDOW_RESOLUTION_Y*HUB_EXIT_Y_MAX_RATIO;

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
				endingPhase4isOver=false;
				bg->setPosition(sf::Vector2f(hub->getPosition().x,hub->getPosition().y - bg->m_size.y - WINDOW_RESOLUTION_Y));
				bg->speed.y = vspeed;
				hub->speed.y=vspeed;

				//to do better:
				hub->setPosition(sf::Vector2f(bg->getPosition().x,bg->getPosition().y));
				
			}

			if (x<X_min && timer.asSeconds() > HUB_EXIT_TIMER)
			{
				//go LEFT
				printf("DEBUG: Travel LEFT !\n");
				this->transitionDestination = TransitionList::TRANSITION_LEFT;
				endingPhase4isOver=true;
			}

			if (x>X_max && timer.asSeconds() > HUB_EXIT_TIMER)
			{
				//go RIGHT
				printf("DEBUG: Travel RIGHT !\n");
				this->transitionDestination = TransitionList::TRANSITION_RIGHT;
				endingPhase4isOver=false;
			}

			if (y>Y_max && timer.asSeconds() > HUB_EXIT_TIMER)
			{
				//go DOWN
				printf("DEBUG: Travel DOWN !\n");
				this->transitionDestination = TransitionList::TRANSITION_DOWN;
				endingPhase4isOver=false;
			}
		}
	}
}

void Scene::ExitHubTransition()
{
	if (transitionDestination = TransitionList::TRANSITION_UP)
	{
		
	}
}

/*
void Scene::hubExitPhase1(float transition_speed_DOWN, int transitionDestination)
{
	if (transitionDestination != TransitionList::NO_TRANSITION)
	{
		//LIGNE DE GROS HACK POUR LE CAS "UP"
		bg->setPosition(sf::Vector2f(0, hub->getPosition().y+WINDOW_RESOLUTION_Y));

		switch (transitionDestination)
		{
			playerShip->disable_inputs = true;
			exitHubPhase1isOver = true;
			exitHubPhase2isOver = false;

			case TransitionList::TRANSITION_UP:
			{
				playerShip->speed.x = -transition_speed_DOWN * ((WINDOW_RESOLUTION_X*STARTSCENE_X_RATIO) - playerShip->getPosition().x) / WINDOW_RESOLUTION_Y;
				playerShip->speed.y = -transition_speed_DOWN * ((WINDOW_RESOLUTION_Y*STARTSCENE_Y_RATIO) - playerShip->getPosition().y) / WINDOW_RESOLUTION_Y;
				hub->speed.y = -transition_speed_DOWN;
				bg->speed.y = -transition_speed_DOWN;
			}
			case TransitionList::TRANSITION_DOWN:
			{
				playerShip->speed.x = transition_speed_DOWN * ((WINDOW_RESOLUTION_X*(1-STARTSCENE_X_RATIO)) - playerShip->getPosition().x) / WINDOW_RESOLUTION_Y;
				playerShip->speed.y = transition_speed_DOWN * ((WINDOW_RESOLUTION_Y*(1-STARTSCENE_Y_RATIO)) - playerShip->getPosition().y) / WINDOW_RESOLUTION_Y;
				hub->speed.y = - transition_speed_DOWN;
				bg->speed.y = - transition_speed_DOWN;
			}
			case TransitionList::TRANSITION_RIGHT:
			{
				playerShip->speed.x = transition_speed_DOWN * ((WINDOW_RESOLUTION_X*(1-STARTSCENE_Y_RATIO)) - playerShip->getPosition().x) / WINDOW_RESOLUTION_Y;
				playerShip->speed.y = transition_speed_DOWN * ((WINDOW_RESOLUTION_Y*(1-STARTSCENE_X_RATIO)) - playerShip->getPosition().y) / WINDOW_RESOLUTION_Y;
				hub->speed.x = transition_speed_DOWN;
				bg->speed.x = transition_speed_DOWN;
			}
			case TransitionList::TRANSITION_LEFT:
			{
				playerShip->speed.x = transition_speed_DOWN * ((WINDOW_RESOLUTION_X*STARTSCENE_Y_RATIO) - playerShip->getPosition().x) / WINDOW_RESOLUTION_Y;
				playerShip->speed.y = transition_speed_DOWN * ((WINDOW_RESOLUTION_Y*STARTSCENE_X_RATIO) - playerShip->getPosition().y) / WINDOW_RESOLUTION_Y;
				hub->speed.x = - transition_speed_DOWN;
				bg->speed.x = - transition_speed_DOWN;
			}
		}
	}
		
}

void Scene::hubExitPhase2()
{
	playerShip->speed.x = 0;
	playerShip->speed.y = 0;
	playerShip->disable_inputs = false;
	bg->speed.y = vspeed;
	hub->speed.y = vspeed;
	exitHubPhase2isOver = true;
	endingPhase1isOver = false;
}
*/