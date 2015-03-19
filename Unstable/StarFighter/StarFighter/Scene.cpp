#include "Scene.h"

extern Game* CurrentGame;

void Scene::LoadSceneFromFile(string name, int hazard_level, bool reverse_scene, bool first_scene)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Loading scene '%s'", (char*)name.c_str()));
	this->m_name = name;
	for (int i = 0; i < EnemyClass::NBVAL_EnemyClass; i++)
	{
		this->total_class_probability[i] = 0;
	}
	this->hazard_break_value = 0;
	this->generating_enemies = false;
	this->generating_boss = false;
	this->hazard_level = hazard_level;
	this->m_hazardbreak_has_occurred = false;

	int p = 0;
	int enemy_count = 0;

	try {
		//Loading the list of all scenes, contained in SCENES_FILE
		list<vector<string>> scenesConfig = *(FileLoaderUtils::FileLoader(SCENES_FILE));

		for (std::list<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
		{
			if ((*it)[ScenesData::SCENE_NAME].compare(name) == 0)
			{
				//Loading the linked scene names
				this->links[Directions::DIRECTION_UP] = (*it)[ScenesData::SCENE_LINK_UP];
				this->links[Directions::DIRECTION_DOWN] = (*it)[ScenesData::SCENE_LINK_DOWN];
				this->links[Directions::DIRECTION_RIGHT] = (*it)[ScenesData::SCENE_LINK_RIGHT];
				this->links[Directions::DIRECTION_LEFT] = (*it)[ScenesData::SCENE_LINK_LEFT];
				std::string scene_name = (*it)[ScenesData::SCENE_DISPLAYNAME];

				//Loading the particular scene that we want to load
				list<vector<string>> config = *(FileLoaderUtils::FileLoader((*it)[ScenesData::SCENE_FILENAME]));
				for (std::list<vector<string>>::iterator it = (config).begin(); it != (config).end(); it++)
				{
					if ((*it)[0].compare("bg") == 0)
					{
						this->direction = Directions::NO_DIRECTION;
						bool hub = false;

						this->vspeed = stoi((*it)[SceneDataBackground::BACKGROUND_VSPEED]);
						float w = stoi((*it)[SceneDataBackground::BACGKROUND_WIDTH]);
						float h = stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT]);

						//Assigning the right direction for the scene
						if ((*it)[SceneDataBackground::BACKGROUND_VERTICAL].compare("V") == 0)
						{
							if (!reverse_scene)
							{
								this->direction = Directions::DIRECTION_UP;
							}
							else
							{
								this->direction = Directions::DIRECTION_DOWN;
							}
						}
						else if ((*it)[SceneDataBackground::BACKGROUND_VERTICAL].compare("H") == 0)
						{
							if (!reverse_scene)
							{
								this->direction = Directions::DIRECTION_RIGHT;
							}
							else
							{
								this->direction = Directions::DIRECTION_LEFT;
							}
						}
						else
						{
							if (!first_scene)
							{
								hub = true;
								this->direction = (*CurrentGame).direction;
							}
						}

						//Setting the right initial position and speed
						float first_screen_offset = 0;
						if (first_scene)
						{
							(*CurrentGame).direction = this->direction;
							first_screen_offset = Independant::getSize_for_Direction(this->direction, sf::Vector2f(SCENE_SIZE_X, SCENE_SIZE_Y)).y;
						}

						sf::Vector2f speed = Independant::getSpeed_for_Scrolling(this->direction, this->vspeed);

						if (hub)
						{
							this->direction = Directions::NO_DIRECTION;
						}
						
						this->bg = new Background(sf::Vector2f(0, 0), speed, (*it)[SceneDataBackground::BACKGROUND_NAME], sf::Vector2f(w, h), (*CurrentGame).direction, first_screen_offset);
						this->bg->display_name = scene_name;

						//Getting the display name of the scene and loading it into the scene portals
						for (int i = 0; i < Directions::NO_DIRECTION; i++)
						{
							if (this->links[(Directions)i].compare("0") != 0)
							{
								//copying the scene name into the portal, that will be responsible for the loading of the linked scenes
								this->bg->portals[(Directions)i]->destination_name = this->links[(Directions)i];

								//Getting the string of the "display name" for a each linked scene
								for (std::list<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
								{
									if ((*it)[ScenesData::SCENE_NAME].compare(this->links[(Directions)i]) == 0)
									{
										this->bg->portals[(Directions)i]->display_name = (*it)[ScenesData::SCENE_DISPLAYNAME];
									}
								}
								//Creating portals
								(*CurrentGame).addToScene(this->bg->portals[(Directions)i], LayerType::PortalLayer, IndependantType::PortalObject);
							}
						}
					}

					//Loading enemies
					else if ((*it)[0].compare("enemy") == 0)
					{
						EnemyBase* e = FileLoader::LoadEnemyBase((*it)[SceneDataEnemy::ENEMY], stoi((*it)[SceneDataEnemy::ENEMY_PROBABILITY].c_str()), stoi((*it)[SceneDataEnemy::ENEMY_POOLSIZE]), stoi((*it)[SceneDataEnemy::ENEMY_CLASS]));
						
						//if the enemy has phases, the direction will be handled by Enemy::SetPhase(). if not, we set it here
						if (!e->enemy->hasPhases)
						{
							e->enemy->speed = Independant::getSpeed_for_Scrolling(this->direction, e->enemy->speed.y);
						}
						
						//setting probabilities of spawn within enemy class
						e->proba_min = this->total_class_probability[e->enemyclass];
						e->proba_max = e->proba_min + e->probability;
						this->total_class_probability[e->enemyclass] = e->proba_max;
						enemy_count += e->proba_max;

						this->enemies_ranked_by_class[e->enemyclass].push_back(e);
						
						//setting enemy generators
						switch (e->enemyclass)
						{
							case EnemyClass::ENEMYPOOL_ALPHA:
							{
								EnemyGenerator* generator = new EnemyGenerator(2, e->enemyclass, 0.10, 0.30);
								this->sceneEnemyGenerators->push_back(generator);
								break;
							}
							case EnemyClass::ENEMYPOOL_BETA:
							{
								EnemyGenerator* generator = new EnemyGenerator(8, e->enemyclass, 0.30, 0.10);
								this->sceneEnemyGenerators->push_back(generator);
								break;
							}
							case EnemyClass::ENEMYPOOL_DELTA:
							{
								EnemyGenerator* generator = new EnemyGenerator(20, e->enemyclass, 0.20, 0.30);
								this->sceneEnemyGenerators->push_back(generator);
								break;
							}
						}

						//hazard value automatic calculation
						hazard_break_value += e->enemy->getMoney() * e->poolsize * HAZARD_BREAK_RATIO * (1 + HAZARD_BREAK_MULTIPLIER*this->hazard_level);
					}

					//loading boss
					else if ((*it)[0].compare("boss") == 0)
					{
						EnemyBase* boss = FileLoader::LoadEnemyBase((*it)[SceneDataBoss::BOSS], 1, 1, stoi((*it)[SceneDataBoss::BOSS_CLASS]));
						if (!boss->enemy->hasPhases)
						{
							boss->enemy->speed = Independant::getSpeed_for_Scrolling(this->direction, boss->enemy->speed.y);
						}
						sf::Vector2f boss_pos = sf::Vector2f(atof((*it)[SceneDataBoss::BOSS_SPAWN_X].c_str()), atof((*it)[SceneDataBoss::BOSS_SPAWN_Y].c_str()));
						boss_pos = Independant::getPosition_for_Direction(this->direction, boss_pos);
						boss->enemy->setPosition(boss_pos);

						this->boss_list.push_back(boss);
						this->generating_boss = true;
					}

					if (enemy_count != 0 && this->direction != Directions::NO_DIRECTION)
					{
						generating_enemies = true;
						this->spawnClock.restart();
					}
				}
			}
		}
	}
	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR, ex.what());
	}
}

Scene::Scene(string name)
{
	this->m_name = name;
	try {
		//Loading the list of all scenes, contained in SCENES_FILE
		list<vector<string>> scenesConfig = *(FileLoaderUtils::FileLoader(SCENES_FILE));

		for (std::list<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
		{
			if ((*it)[ScenesData::SCENE_NAME].compare(name) == 0)
			{
				//Loading the linked scene names
				this->links[Directions::DIRECTION_UP] = (*it)[ScenesData::SCENE_LINK_UP];
				this->links[Directions::DIRECTION_DOWN] = (*it)[ScenesData::SCENE_LINK_DOWN];
				this->links[Directions::DIRECTION_RIGHT] = (*it)[ScenesData::SCENE_LINK_RIGHT];
				this->links[Directions::DIRECTION_LEFT] = (*it)[ScenesData::SCENE_LINK_LEFT];
				std::string scene_name = (*it)[ScenesData::SCENE_DISPLAYNAME];

				//Loading the particular scene that we want to load
				list<vector<string>> config = *(FileLoaderUtils::FileLoader((*it)[ScenesData::SCENE_FILENAME]));
				for (std::list<vector<string>>::iterator it = (config).begin(); it != (config).end(); it++)
				{
					if ((*it)[0].compare("bg") == 0)
					{
						this->bg = new Background(sf::Vector2f(0, 0), sf::Vector2f(0, 0), (*it)[SceneDataBackground::BACKGROUND_NAME], sf::Vector2f(0, 0), this->direction);
						this->bg->display_name = scene_name;
					}
				}
			}

			//Drawing link zones and texts
			for (int i = 0; i < Directions::NO_DIRECTION; i++)
			{
				if (this->links[(Directions)i].compare("0") != 0)
				{
					//Getting the string of the "display name" for a each linked scene
					for (std::list<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
					{
						if ((*it)[ScenesData::SCENE_NAME].compare(this->links[(Directions)i]) == 0)
						{
							//Getting the name
							this->bg->portals[(Directions)i]->display_name = (*it)[ScenesData::SCENE_DISPLAYNAME];
						}
					}
				}
			}
		}
	}
	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR, ex.what());
	}
}

Scene::Scene(string name, int hazard_level, bool reverse_scene, bool first_scene)
{
	this->LoadSceneFromFile(name, hazard_level, reverse_scene, first_scene);

	LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Scene '%s' loaded.", (char*)name.c_str()));
}

void Scene::Update(Time deltaTime)
{
	if (this->generating_enemies)
	{
		//this->GenerateEnemies(deltaTime);
		this->GenerateEnemiesv2();
	}

	if ((*CurrentGame).getHazard() > hazard_break_value - 1 && hazard_break_value > 0 && !m_hazardbreak_has_occurred)
	{
		HazardBreakEvent();
	}
}

void Scene::GenerateBoss()
{
	for (std::list<EnemyBase*>::iterator it = boss_list.begin(); it != boss_list.end(); ++it)
	{
		Enemy* m_boss = (*it)->enemy->Clone();
		m_boss->enemy_class = (EnemyClass)((*it)->enemyclass);
		(*CurrentGame).addToScene(m_boss, LayerType::EnemyObjectLayer, IndependantType::EnemyObject);
	}
}

void Scene::Draw(sf::RenderWindow* window)
{
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		if (this->links[(Directions)i].compare("0") != 0)
		{
			//this->link_zone[i].Draw(window);
		}
	}
}

void Scene::GenerateEnemiesv2()
{
	for (std::vector<EnemyGenerator*>::iterator it = sceneEnemyGenerators->begin(); it != sceneEnemyGenerators->end(); ++it)
	{
		//SOURCES OF INCREMENTATION
		//Time
		(*it)->spawnResource += this->spawnClock.getElapsedTime().asSeconds();

		//SPAWN
		if ((*it)->spawnResource > (*it)->spawnCost)
		{
			//Randomization of the event
			float p = RandomizeFloatBetweenValues(sf::Vector2f(0, 1));
			if (p < (*it)->spawnRepeatProbability)
			{
				//Critical event: spawn enemy, pay discount cost
				float r = RandomizeFloatBetweenValues(sf::Vector2f(SPAWN_REPEAT_MINIMUM_RESOURCE, SPAWN_REPEAT_MAXIMUM_RESOURCE));
				(*it)->spawnResource *= r;
				this->SpawnEnemy((*it)->enemyClass);
				this->CollateralSpawnCost((*it)->spawnCost, COLLATERAL_SPAWN_COST_MULTIPLIER, (*it)->enemyClass);
				//printf("CRITICAL REPEAT class %d\n", (*it)->enemyClass);
			}
			else if (p > 1 - (*it)->spawnMissProbability)
			{
				//Critical event: don't spawn enemy, but still pay a (discount) cost
				float m = RandomizeFloatBetweenValues(sf::Vector2f(SPAWN_MISS_MINIMUM_RESOURCE, SPAWN_MISS_MAXIMUM_RESOURCE));
				(*it)->spawnResource *= m;
				//printf("CRITICAL MISS class %d\n", (*it)->enemyClass);
			}
			else
			{
				//Normal event: spawn enemy, pay total cost
				this->SpawnEnemy((*it)->enemyClass);
				(*it)->spawnResource = 0;
				this->CollateralSpawnCost((*it)->spawnCost, COLLATERAL_SPAWN_COST_MULTIPLIER, (*it)->enemyClass);
			}
		}

		//DEBUG
		if ((*it)->enemyClass == 1)
		{
			//printf("RESSOURCES: %f\n", (*it)->spawnResource);
		}
	}
	
	this->spawnClock.restart();
}

void Scene::CollateralSpawnCost(float collatefal_cost, float collateral_multiplier, int below_enemy_class)
{
	//TODO: refactoriser pour passer le max de fonction dans la classe EnemyGenerator
	for (std::vector<EnemyGenerator*>::iterator it = sceneEnemyGenerators->begin(); it != sceneEnemyGenerators->end(); ++it)
	{
		if ((*it)->enemyClass < below_enemy_class)
		{
			if (collateral_multiplier > 0)
			{
				(*it)->spawnResource -= (collatefal_cost * collateral_multiplier);
			}
			else
			{
				(*it)->spawnResource = 0;
			}
		}
	}
}

void Scene::SpawnEnemy(int enemy_class)
{
	Enemy* enemy = NULL;
	//Attention si total class probability vaut 0 ça va crasher - division par zéro oblige. du coup il faut vérifier que ce n'est pas égal à 0.
	int dice_roll = (rand() % (this->total_class_probability[enemy_class])) + 1;

	for (std::vector<EnemyBase*>::iterator it = enemies_ranked_by_class[enemy_class].begin(); it != enemies_ranked_by_class[enemy_class].end(); ++it)
	{
		if (dice_roll >= (*it)->proba_min && dice_roll <= (*it)->proba_max)
		{
			enemy = (*it)->enemy->Clone();
			break;
		}
	}
	assert(enemy != NULL);

	//RANDOM POSITION
	sf::Vector2f rand_coordinates_min = sf::Vector2f(enemy->m_size.x / 2, -enemy->m_size.y / 2);
	rand_coordinates_min = Independant::getPosition_for_Direction((*CurrentGame).direction, rand_coordinates_min, false);
	
	//length of the allowed spread
	int i_ = Independant::getDirectionMultiplier((*CurrentGame).direction).y;
	float allowed_spread = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(i_*(SCENE_SIZE_X - enemy->m_size.x / 2), i_*(SCENE_SIZE_Y - enemy->m_size.x / 2))).x;
	
	//cutting clusters bigger than the scene (+ debug message)
	if ((allowed_spread*Independant::getDirectionMultiplier((*CurrentGame).direction).y) < 0)
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("ERROR: Error in calculation of 'allowed_spread' value in enemy generation. This value leads out of screen.\n"));
		return;
	}
	
	//random value inside the allowed spread
	float random_posX = RandomizeFloatBetweenValues(sf::Vector2f(0, allowed_spread));
	
	//getting position coordinates (min + random value)
	float pos_x = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(rand_coordinates_min.x + random_posX, rand_coordinates_min.x)).x;
	float pos_y = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(rand_coordinates_min.y, rand_coordinates_min.y + random_posX)).x;
	sf::Vector2f pos = sf::Vector2f(pos_x, pos_y);
	
	//spawning enemy at the chosen random position
	enemy->setPosition(pos);
	(*CurrentGame).addToScene(enemy, LayerType::EnemyObjectLayer, IndependantType::EnemyObject);
}

void Scene::GenerateEnemies(Time deltaTime)
{
	if (spawnClock.getElapsedTime() > sf::seconds(4))
	{
		spawnClock.restart();
		double random_number = ((double)rand() / (RAND_MAX));

		// A PASSER EN .CSV :
		int nb_rows = 1;
		int nb_lines = 1;
		float xspread = 150;
		float yspread = 200;

		// liste de classes d'ennemis : alpha, alpha, alpha, alpha, alpha
		// liste de patterns associés : 0, 0, 0, 0, 0, 

		//chosing a random enemy within a class of enemies
		//for each sceneEnemyClassesAvailable[i]
		Enemy* random_enemy_within_class[EnemyClass::NBVAL_EnemyClass];

		//Attention si total class probability vaut 0 ça va crasher - division par zéro oblige. du coup il faut vérifier que ce n'est pas égal à 0.
		int dice_roll = (rand() % (total_class_probability[EnemyClass::ENEMYPOOL_ALPHA])) + 1;

		for (int i = 0; i < EnemyClass::NBVAL_EnemyClass; i++)
		{
			for (std::vector<EnemyBase*>::iterator it = enemies_ranked_by_class[i].begin(); it != enemies_ranked_by_class[i].end(); ++it)
			{
				if (dice_roll >= (*it)->proba_min && dice_roll <= (*it)->proba_max)
				{
					random_enemy_within_class[i] = (*it)->enemy;
				}
			}
		}

		//preparing the list of enemies to put in the cluster
		sf::Vector2f max_enemy_size = sf::Vector2f(0, 0);

		vector<EnemyPoolElement*>* cluster = new vector<EnemyPoolElement*>;
		for (int i = 0; i < nb_rows*nb_lines; i++)
		{
			//VALEURS A CONF EN .CSV
			// arg0 = enemy class
			// arg1 = move pattern
			//if arg0 != VOID
			EnemyPoolElement* e = new EnemyPoolElement(random_enemy_within_class[EnemyClass::ENEMYPOOL_ALPHA], EnemyClass::ENEMYPOOL_ALPHA, PatternType::NoMovePattern);
			if (e->enemy->m_size.x > max_enemy_size.x)
			{
				max_enemy_size.x = e->enemy->m_size.x;
			}
			if (e->enemy->m_size.y > max_enemy_size.y)
			{
				max_enemy_size.y = e->enemy->m_size.y;
			}

			enemies_ranked_by_class[EnemyClass::ENEMYPOOL_ALPHA].front()->poolsize--;

			cluster->push_back(e);
		}

		sf::Vector2f size = sf::Vector2f(((nb_rows - 1) * xspread) + max_enemy_size.x, ((nb_lines - 1) * yspread) + max_enemy_size.y);

		//CALCULATION OF A RANDOM POSITION TO SPAWN THE CLUSTER 
		//min position
		sf::Vector2f rand_coordinates_min = sf::Vector2f(max_enemy_size.x/2, -size.y/2);
		rand_coordinates_min = Independant::getPosition_for_Direction((*CurrentGame).direction, rand_coordinates_min, false);

		//length of the allowed spread
		int i_ = Independant::getDirectionMultiplier((*CurrentGame).direction).y;
		float allowed_spread = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(i_*(SCENE_SIZE_X - size.x), i_*(SCENE_SIZE_Y - size.x))).x;
		//printf("Allowed spread: %f. Cluster size: %f. Max neemy size: %f\n", allowed_spread, size.x, max_enemy_size.x);

		//cutting clusters bigger than the scene (+ debug message)
		if ((allowed_spread*Independant::getDirectionMultiplier((*CurrentGame).direction).y) < 0)
		{
			LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("ERROR: No possibility to spawn this enemy cluster within scene size. Spawn skipped. Please check the configuration of enemy sizes and cluster spread values.\n"));
			return;
		}
		
		//random value inside the allowed spread
		float random_posX = RandomizeFloatBetweenValues(sf::Vector2f(0, allowed_spread));

		//getting position coordinates (min + random value)
		float pos_x = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(rand_coordinates_min.x + random_posX, rand_coordinates_min.x)).x;
		float pos_y = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(rand_coordinates_min.y, rand_coordinates_min.y + random_posX)).x;
		sf::Vector2f pos = sf::Vector2f(pos_x, pos_y);
		//printf("Random posX: %f. Spawn pos: %f, %f.\n\n", random_posX, pos_x, pos_y);

		//generating the cluster at the given coordinates
		EnemyPool* generated_cluster = new EnemyPool(pos, nb_lines, nb_rows, xspread, yspread, cluster);

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

float HazardLevelsBeastBonus[HazardLevels::NB_HAZARD_LEVELS] = { 0.0, 0.5, 1.0, 1.5, 2.0 };

void Scene::HazardBreakEvent()
{
	//(*CurrentGame).resetHazard((*CurrentGame).getHazard() - hazard_break_value);
	//hazard_break_value *= (1 + HAZARD_BREAK_MULTIPLIER);
	if (hazard_level < HazardLevels::NB_HAZARD_LEVELS - 1)
	{
		hazard_level++;
		m_hazardbreak_has_occurred = true;
	}
	
	LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Hazard level up: %d/5\n", this->hazard_level + 1));
}

float Scene::getSceneBeastScore()
{
	float bonus = 0;
	if (this->hazard_level < HazardLevels::NB_HAZARD_LEVELS && this->hazard_level >= 0)
	{
		bonus = HazardLevelsBeastBonus[this->hazard_level];
	}
	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error, The scene has a 'hazard_level' (%d) beyond existing values\n", this->hazard_level);
		this->hazard_level = 0;
	}

	return bonus;
}

int Scene::getSceneHazardBreakValue()
{
	return this->hazard_break_value;
}

int Scene::getSceneHazardLevelValue()
{
	return this->hazard_level;
}