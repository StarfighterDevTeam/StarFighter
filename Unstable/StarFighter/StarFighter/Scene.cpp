#include "Scene.h"

extern Game* CurrentGame;

void Scene::LoadSceneFromFile(string name, int hazard_level, bool reverse_scene, bool first_scene)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Loading scene '%s'", (char*)name.c_str()));
	srand(time(NULL));
	this->m_name = name;
	for (int i = 0; i < EnemyClass::NBVAL_EnemyClass; i++)
	{
		this->total_class_probability[i] = 0;
	}
	this->generating_enemies = false;
	this->generating_boss = false;
	this->hazard_level = hazard_level;
	this->m_hazardbreak_has_occurred = false;
	this->sceneEnemyGenerators->clear();

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
						EnemyBase* e = FileLoader::LoadEnemyBase((*it)[SceneDataEnemy::ENEMY], stoi((*it)[SceneDataEnemy::ENEMY_PROBABILITY].c_str()), stoi((*it)[SceneDataEnemy::ENEMY_CLASS]));
						
						//if the enemy has phases, the direction will be handled by Enemy::SetPhase(). if not, we set it here
						if (!e->enemy->hasPhases)
						{
							e->enemy->speed = Independant::getSpeed_for_Scrolling(this->direction, e->enemy->speed.y);
						}
						
						//setting enemy generators: we need to create on generator per class
						if (this->total_class_probability[e->enemyclass] == 0)
						{
							EnemyGenerator* generator = new EnemyGenerator(stof((*it)[SceneDataEnemy::ENEMY_CLASS_SPAWNCOST]), e->enemyclass, 
								stof((*it)[SceneDataEnemy::ENEMY_CLASS_REPEAT_CHANCE]), stof((*it)[SceneDataEnemy::ENEMY_CLASS_MISS_CHANCE]));
							generator->spawnCostCollateralMultiplier = spawnCostCollateralMultiplierTable[this->getSceneHazardLevelValue()];
							this->sceneEnemyGenerators->push_back(generator);
						}

						//setting probabilities of spawn within enemy class
						e->proba_min = this->total_class_probability[e->enemyclass];
						e->proba_max = e->proba_min + e->probability;
						this->total_class_probability[e->enemyclass] = e->proba_max;
						enemy_count += e->proba_max;

						this->enemies_ranked_by_class[e->enemyclass].push_back(e);
					}

					//loading boss
					else if ((*it)[0].compare("boss") == 0)
					{
						EnemyBase* boss = FileLoader::LoadEnemyBase((*it)[SceneDataBoss::BOSS], 1, stoi((*it)[SceneDataBoss::BOSS_CLASS]));
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

void Scene::GenerateEnemiesv2(Time deltaTime)
{
	for (std::vector<EnemyGenerator*>::iterator it = sceneEnemyGenerators->begin(); it != sceneEnemyGenerators->end(); ++it)
	{
		//SOURCES OF INCREMENTATION
		//Time
		(*it)->spawnResource += deltaTime.asSeconds();

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
				this->CollateralSpawnCost((*it)->spawnCost, (*it)->spawnCostCollateralMultiplier, (*it)->enemyClass);
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
				float n = RandomizeFloatBetweenValues(sf::Vector2f(SPAWN_NORMAL_MINIMUM_RESOURCE, SPAWN_NORMAL_MAXIMUM_RESOURCE));
				(*it)->spawnResource = 0 + (n*(*it)->spawnCost);
				this->CollateralSpawnCost((*it)->spawnCost, (*it)->spawnCostCollateralMultiplier, (*it)->enemyClass);
			}
		}

		//DEBUG
		if ((*it)->enemyClass == 1)
		{
			//printf("RESSOURCES: %f / %f\n", (*it)->spawnResource, (*it)->spawnCost);
		}
	}
	
	this->spawnClock.restart();
}

void Scene::CollateralSpawnCost(float collatefal_cost, float collateral_multiplier, int below_enemy_class)
{
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
	//Attention si total class probability vaut 0 �a va crasher - division par z�ro oblige. du coup il faut v�rifier que ce n'est pas �gal � 0.
	//int dice_roll = (rand() % (this->total_class_probability[enemy_class])) + 1;
	int dice_roll = RandomizeIntBetweenValues(0, this->total_class_probability[enemy_class]);
	
	for (std::vector<EnemyBase*>::iterator it = enemies_ranked_by_class[enemy_class].begin(); it != enemies_ranked_by_class[enemy_class].end(); ++it)
	{
		if (dice_roll >= (*it)->proba_min && dice_roll <= (*it)->proba_max)
		{
			enemy = (*it)->enemy->Clone();
			enemy->setRotation(Independant::getRotation_for_Direction((*CurrentGame).direction));
			
			break;
		}
	}
	assert(enemy != NULL);

	//RANDOM POSITION
	sf::Vector2f pos = enemy->getRandomXSpawnPosition((*CurrentGame).direction, enemy->m_size);
	enemy->setPosition(pos);
	(*CurrentGame).addToScene(enemy, LayerType::EnemyObjectLayer, IndependantType::EnemyObject);

	//counting spawned enemies
	(*CurrentGame).hazardSpawned += enemy->getMoney();
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
		// liste de patterns associ�s : 0, 0, 0, 0, 0, 

		//chosing a random enemy within a class of enemies
		//for each sceneEnemyClassesAvailable[i]
		Enemy* random_enemy_within_class[EnemyClass::NBVAL_EnemyClass];

		//Attention si total class probability vaut 0 �a va crasher - division par z�ro oblige. du coup il faut v�rifier que ce n'est pas �gal � 0.
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

				cluster->push_back(e);
			}

			sf::Vector2f size = sf::Vector2f(((nb_rows - 1) * xspread) + max_enemy_size.x, ((nb_lines - 1) * yspread) + max_enemy_size.y);

			//RANDOM POSITION TO SPAWN THE CLUSTER
			sf::Vector2f pos = cluster->front()->enemy->getRandomXSpawnPosition((*CurrentGame).direction, max_enemy_size, size);

			//generating the cluster at the given coordinates
			EnemyPool* generated_cluster = new EnemyPool(pos, nb_lines, nb_rows, xspread, yspread, cluster);
		}
	}
}

float HazardLevelsBeastBonus[HazardLevels::NB_HAZARD_LEVELS] = { 0.0, 0.5, 1.0, 1.5, 2.0 };

void Scene::HazardBreak()
{
	if (hazard_level < HazardLevels::NB_HAZARD_LEVELS - 1)
	{
		hazard_level++;
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

int Scene::getSceneHazardLevelValue()
{
	return this->hazard_level;
}