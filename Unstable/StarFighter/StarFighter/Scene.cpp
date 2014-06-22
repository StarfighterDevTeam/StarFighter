#include "Scene.h"

extern Game* CurrentGame;


void Scene::LoadSceneFromFile(string name)
{
	LOGGER_WRITE(Logger::Priority::DEBUG,"Loading Scene");
	scrolling_direction.x = 1;
	scrolling_direction.y = 0;
	vspeed = + 10;
	hazard_break_value = 0;
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
					this->bg = new Independant(sf::Vector2f(0, -stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])),sf::Vector2f(ApplyScrollingDirectionOnSpeed(vspeed)),(*it)[SceneDataBackground::BACKGROUND_NAME],Vector2f(stoi((*it)[SceneDataBackground::BACGKROUND_WIDTH]),stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])),Vector2f(0,0));
					bg->setVisible(true);
				}

				if((*it)[0].compare("hub") == 0)
				{
					this->hub = new Independant(sf::Vector2f(0,0),sf::Vector2f(0,vspeed),(*it)[SceneDataBackground::BACKGROUND_NAME],Vector2f(stoi((*it)[SceneDataBackground::BACGKROUND_WIDTH]),stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT])),Vector2f(0,0));
					hub->setVisible(true);
				}

				if((*it)[0].compare("enemy") == 0)
				{
					EnemyBase* e = LoadEnemy((*it)[SceneDataEnemy::ENEMY],atof((*it)[SceneDataEnemy::ENEMY_PROBABILITY].c_str()),stoi((*it)[SceneDataEnemy::ENEMY_POOLSIZE]), stoi((*it)[SceneDataEnemy::ENEMY_CLASS]));
					this->sceneIndependantsLayered[e->enemyclass].push_back(e);
					//legacy, to delete when pools are done
					this->enemies.push_back(*e);
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
	hazardBar.setPosition(760, 10);

	//hazardBarMax.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, hazard_break_value));
	hazardBarMax.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, HAZARD_BAR_SIZE_Y));
	hazardBarMax.setFillColor(sf::Color(0, 0, 0));//black
	hazardBarMax.setOutlineThickness(4);
	hazardBarMax.setOutlineColor(sf::Color(255, 255, 255));
	hazardBarMax.setOrigin(0, 0);
	hazardBarMax.setPosition(760, 10);

}

Scene::Scene(string name)
{
	transitionDestination = TransitionList::NO_TRANSITION;

	for (int b=0; b<SceneBooleans::NBVAL_SceneBooleans; b++)
	{
		phaseShifter[b] = false;
	}

	LoadSceneFromFile(name);

	LOGGER_WRITE(Logger::Priority::DEBUG,"Loading Ship");

	try 
	{
		this->shipConfig = *(FileLoader(SHIP_FILE));

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
		this->hazardBreakText->setPosition(700,HAZARD_BAR_SIZE_Y+20);

		this->hazardBreakScore = new sf::Text("", *font2, 15);
		this->hazardBreakScore->setColor(sf::Color::Red);
		this->hazardBreakScore->setStyle(sf::Text::Regular);
		this->hazardBreakScore->setPosition(700,HAZARD_BAR_SIZE_Y+46);
	}

	catch( const std::exception & ex ) 
	{

		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}

	//Player ship
	//this->playerShip = new Ship(Vector2f(400,500), *shipConf);
	this->playerShip = new Ship(Vector2f(ApplyScrollingDirectionOnPosition(Vector2f(WINDOW_RESOLUTION_X*STARTSCENE_X_RATIO,WINDOW_RESOLUTION_Y*STARTSCENE_Y_RATIO))), *LoadShipConfig("default"));
}

void Scene::StartGame(sf::RenderWindow*	window)
{
	this->mainWindow = window;
	(*CurrentGame).init(window);

	//bg
	bg->setPosition(0,bg->getPosition().y + WINDOW_RESOLUTION_Y); // this is an exception for the 1st level of the game only
	hub->setPosition(0,hub->getPosition().y + WINDOW_RESOLUTION_Y);
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

	hazardBar.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, ((*CurrentGame).getHazard()*HAZARD_BAR_SIZE_Y)/hazard_break_value));
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
			
			shipC->weapon = LoadWeapon((*it)[ShipModelData::SHIPMODEL_WEAPON], -1, LoadAmmo((*it)[ShipModelData::SHIPMODEL_AMMO]));

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
			base->enemy = new Enemy(sf::Vector2f(0,0),sf::Vector2f(ApplyScrollingDirectionOnSpeed(stoi((*it)[EnemyData::ENEMY_SPEED]))),(*it)[EnemyData::ENEMY_IMAGE_NAME],sf::Vector2f(stoi((*it)[EnemyData::ENEMY_WIDTH]),stoi((*it)[EnemyData::ENEMY_HEIGHT])), LoadFX((*it)[EnemyData::ENEMY_FX_DEATH]));
			base->probability = probability;
			base->poolsize = poolSize;
			base->enemyclass = enemyClass;

			((Independant*)base->enemy)->armor = stoi((*it)[EnemyData::ENEMY_ARMOR]);
			((Independant*)base->enemy)->shield = ((Independant*)base->enemy)->shield_max = stoi((*it)[EnemyData::ENEMY_SHIELD]);
			((Independant*)base->enemy)->shield_regen = stoi((*it)[EnemyData::ENEMY_SHIELD_REGEN]);
			((Independant*)base->enemy)->damage = stoi((*it)[EnemyData::ENEMY_DAMAGE]);
			((Independant*)base->enemy)->setMoney(stoi((*it)[EnemyData::ENEMY_VALUE]));

			base->enemy->weapon = LoadWeapon((*it)[EnemyData::WEAPON], 1, LoadAmmo((*it)[EnemyData::AMMO]));

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
			weapon->multishot = stoi((*it)[WeaponData::WEAPON_MULTISHOT]);
			weapon->xspread = stoi((*it)[WeaponData::WEAPON_XSPREAD]);
			weapon->alternate = (bool)(stoi((*it)[WeaponData::WEAPON_ALTERNATE]));
			weapon->dispersion = stoi((*it)[WeaponData::WEAPON_DISPERSION]);

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
				n->setPosition(ApplyScrollingDirectionOnPosition(sf::Vector2f(rand() % WINDOW_RESOLUTION_X,-n->m_size.y*2)));
				n->setVisible(true);
				it->poolsize--;

				n->movePattern = MovePatternType::StarFish;

				LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("spawning enemy '%s' (x=%.1f) [pool=%d] (class=%d)",it->enemy->getName().c_str(),n->getPosition().x,it->poolsize, it->enemyclass));

				(*CurrentGame).addToScene((Independant*)n,LayerType::EnemyObjectLayer,IndependantType::EnemyObject);
			}
		}
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
		hub->setPosition(sf::Vector2f(bg->getPosition().x,bg->getPosition().y - WINDOW_RESOLUTION_Y));

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
		playerShip->speed.x = transition_DOWN * ((WINDOW_RESOLUTION_X/2) - playerShip->getPosition().x) / WINDOW_RESOLUTION_Y;
		playerShip->speed.y = transition_DOWN * ((WINDOW_RESOLUTION_Y/2) - playerShip->getPosition().y) / WINDOW_RESOLUTION_Y;
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
				bg->setPosition(sf::Vector2f(hub->getPosition().x,hub->getPosition().y - bg->m_size.y - WINDOW_RESOLUTION_Y));

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
		//playerShip->speed.x = transition_speed_DOWN * ((WINDOW_RESOLUTION_X*STARTSCENE_X_RATIO) - playerShip->getPosition().x) / WINDOW_RESOLUTION_Y;
		playerShip->speed.y = transition_speed_DOWN * ((WINDOW_RESOLUTION_Y*STARTSCENE_Y_RATIO) - playerShip->getPosition().y) / WINDOW_RESOLUTION_Y;
		phaseShifter[SceneBooleans::EXITHUB_PHASE2] = false;
		phaseShifter[SceneBooleans::EXITHUB_PHASE3] = true;
	}

	if (!phaseShifter[SceneBooleans::EXITHUB_PHASE2] && phaseShifter[SceneBooleans::EXITHUB_PHASE3] && bg->getPosition().y > - bg->m_size.y + WINDOW_RESOLUTION_Y)
	{
		bg->setPosition(0, - bg->m_size.y + WINDOW_RESOLUTION_Y);
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

/*
void Scene::hubExitPhase1(float transition_speed_DOWN, int transitionDestination)
{
if (phaseShifter[SceneBooleans::EXITHUB_PHASE1] && !phaseShifter[SceneBooleans::EXITHUB_PHASE2])
{
//LIGNE DE GROS HACK POUR LE CAS "UP"

switch (transitionDestination)
{
playerShip->disable_inputs = true;



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
phaseShifter[SceneBooleans::EXITHUB_PHASE2] = true;
phaseShifter[SceneBooleans::ENDSCENE_PHASE1] = false;
}

*/


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
		y = WINDOW_RESOLUTION_Y - position.y;
	}
	
	if (scrolling_direction.y == 0)
		{
			if (scrolling_direction.x == 1)
			{
				x = (WINDOW_RESOLUTION_Y - position.y) * WINDOW_RESOLUTION_X/WINDOW_RESOLUTION_Y;
				y = position.x * WINDOW_RESOLUTION_Y/WINDOW_RESOLUTION_X;
			}
			if (scrolling_direction.x == -1)
			{
				x= WINDOW_RESOLUTION_X - ((WINDOW_RESOLUTION_Y - position.y) * WINDOW_RESOLUTION_X/WINDOW_RESOLUTION_Y);
				y= position.x * WINDOW_RESOLUTION_Y/WINDOW_RESOLUTION_X;
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

//float ApplyScrollingDirectionOnRotation()
//{
//	return 
//}