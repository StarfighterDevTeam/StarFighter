#include "Scene.h"

extern Game* CurrentGame;

void Scene::LoadSceneFromFile(string name, int hazard_level, bool reverse_scene, bool first_scene)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Loading scene '%s'", (char*)name.c_str()));
	srand(time(NULL));
	m_name = name;
	for (int i = 0; i < EnemyClass::NBVAL_EnemyClass; i++)
	{
		m_total_class_probability[i] = 0;
	}
	m_generating_enemies = false;
	m_generating_boss = false;
	m_hazard_level = hazard_level;
	m_hazardbreak_has_occurred = false;
	m_canHazardBreak = false;

	int p = 0;
	int enemy_count = 0;

	m_textHazardBreak.setFont(*(*CurrentGame).m_hud.font2);
	m_textHazardBreak.setCharacterSize(30);
	m_textHazardBreak.setColor(sf::Color(255, 255, 255, 255));

	try {
		//Loading the list of all scenes, contained in SCENES_FILE
		vector<vector<string>> scenesConfig = *(FileLoaderUtils::FileLoader(SCENES_FILE));

		for (std::vector<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
		{
			if ((*it)[SCENE_NAME].compare(name) == 0)
			{
				//Loading the linked scene names
				m_links[Directions::DIRECTION_UP] = (*it)[SCENE_LINK_UP];
				m_links[Directions::DIRECTION_DOWN] = (*it)[SCENE_LINK_DOWN];
				m_links[Directions::DIRECTION_RIGHT] = (*it)[SCENE_LINK_RIGHT];
				m_links[Directions::DIRECTION_LEFT] = (*it)[SCENE_LINK_LEFT];

				m_canHazardBreak = ((*it)[SCENE_HAZARD_BREAK].compare("1") == 0) ? true : false;

				std::string scene_name = (*it)[SCENE_DISPLAYNAME];

				//Loading the particular scene that we want to load
				vector<vector<string>> config = *(FileLoaderUtils::FileLoader((*it)[SCENE_FILENAME]));
				for (std::vector<vector<string>>::iterator it = (config).begin(); it != (config).end(); it++)
				{
					if ((*it)[0].compare("bg") == 0)
					{
						m_direction = Directions::NO_DIRECTION;
						bool hub = false;

						m_vspeed = stoi((*it)[BACKGROUND_VSPEED]);
						float w = stoi((*it)[BACGKROUND_WIDTH]);
						float h = stoi((*it)[BACKGROUND_HEIGHT]);

						//Assigning the right direction for the scene
						if ((*it)[BACKGROUND_VERTICAL].compare("V") == 0)
						{
							if (!reverse_scene)
							{
								m_direction = Directions::DIRECTION_UP;
							}
							else
							{
								m_direction = Directions::DIRECTION_DOWN;
							}
						}
						else if ((*it)[BACKGROUND_VERTICAL].compare("H") == 0)
						{
							if (!reverse_scene)
							{
								m_direction = Directions::DIRECTION_RIGHT;
							}
							else
							{
								m_direction = Directions::DIRECTION_LEFT;
							}
						}
						else
						{
							if (!first_scene)
							{
								hub = true;
								m_direction = (*CurrentGame).m_direction;
							}
						}

						//Setting the right initial position and speed
						float first_screen_offset = 0;
						if (first_scene)
						{
							(*CurrentGame).m_direction = m_direction;
							first_screen_offset = GameObject::getSize_for_Direction(m_direction, sf::Vector2f(SCENE_SIZE_X, SCENE_SIZE_Y)).y;
						}

						sf::Vector2f speed = GameObject::getSpeed_for_Scrolling(m_direction, m_vspeed);

						if (hub)
						{
							m_direction = Directions::NO_DIRECTION;
						}
						
						m_bg = new Background(sf::Vector2f(0, 0), speed, (*it)[BACKGROUND_NAME], sf::Vector2f(w, h), (*CurrentGame).m_direction, first_screen_offset);
						m_bg->m_display_name = scene_name;
						(*CurrentGame).addToScene(m_bg, LayerType::BackgroundLayer, GameObjectType::BackgroundObject);

						//Getting the display name of the scene and loading it into the scene portals
						for (int i = 0; i < Directions::NO_DIRECTION; i++)
						{
							if (m_links[(Directions)i].compare("0") != 0)
							{
								//CREATING THE PORTAL
								m_bg->m_portals[(Directions)i] = new Portal(sf::Vector2f(0, 0), speed, PORTAL_TEXTURE_NAME, sf::Vector2f(PORTAL_WIDTH, PORTAL_HEIGHT), sf::Vector2f(PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2), PORTAL_FRAMES, PORTAL_ANIMATIONS);
								sf::Vector2f bg_size = GameObject::getSize_for_Direction((Directions)i, sf::Vector2f(w, h));
								//applying offset respect to the center of the background, depending on the direction
								m_bg->m_portals[(Directions)i]->m_offset = GameObject::getSpeed_for_Scrolling((Directions)i, (-bg_size.y / 2) + (PORTAL_HEIGHT / 2));
								m_bg->m_portals[(Directions)i]->setPosition(m_bg->getPosition().x + m_bg->m_portals[(Directions)i]->m_offset.x, m_bg->getPosition().y + m_bg->m_portals[(Directions)i]->m_offset.y);

								//rotation
								m_bg->m_portals[(Directions)i]->setRotation(GameObject::getRotation_for_Direction((Directions)i));

								//direction
								m_bg->m_portals[(Directions)i]->m_direction = (Directions)i;

								//copying the scene name into the portal, that will be responsible for the loading of the linked scenes
								m_bg->m_portals[(Directions)i]->m_destination_name = m_links[(Directions)i];

								//Getting the string of the "display name" for a each linked scene
								for (std::vector<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
								{
									if ((*it)[SCENE_NAME].compare(m_links[(Directions)i]) == 0)
									{
										m_bg->m_portals[(Directions)i]->m_display_name = (*it)[SCENE_DISPLAYNAME];
									}
								}

								//Displaying the portals
								(*CurrentGame).addToScene(m_bg->m_portals[(Directions)i], LayerType::PortalLayer, GameObjectType::PortalObject);
							}
						}

						if (first_scene)
						{
							m_bg->SetPortalsState(PortalState::PortalOpen);
						}
						else
						{
							m_bg->SetPortalsState(PortalState::PortalGhost);
						}
					}
					else if ((*it)[0].compare("shop") == 0)
					{
						//creating the shop
						float w_ = stoi((*it)[SHOP_WIDTH]);
						float h_ = stoi((*it)[SHOP_HEIGHT]);
						m_bg->m_shop = new Shop(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2), sf::Vector2f(0, 0), (*it)[SHOP_TEXTURE_NAME], sf::Vector2f(w_, h_), sf::Vector2f(w_ / 2, h_ / 2));
						m_bg->m_shop->m_level = stoi((*it)[SHOP_LEVEL]);
						m_bg->m_shop->m_visible = true;
						m_bg->m_shop->m_display_name = m_bg->m_display_name + " Shop";
						(*CurrentGame).addToScene(m_bg->m_shop, PortalLayer, ShopObject);

						//creating shop content
						Ship::FillShopWithRandomObjets(NUMBER_OF_OBJECTS_GENERATED_IN_SHOP, m_bg->m_shop, ENEMYPOOL_ALPHA);
						Ship::FillShopWithRandomObjets(NUMBER_OF_RARE_OBJECTS_GENERATED_IN_SHOP, m_bg->m_shop, ENEMYPOOL_DELTA);
						LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Filling scene '%s' shop with new items.\n", (char*)name.c_str()));

					}
					//Loading enemies
					else if ((*it)[0].compare("enemy") == 0)
					{
						EnemyBase* e = FileLoader::LoadEnemyBase((*it)[ENEMY], stoi((*it)[ENEMY_PROBABILITY]), stoi((*it)[ENEMY_CLASS]));
						e->m_enemy->m_level = stoi((*it)[ENEMY_CLASS_LEVEL]);
						e->m_enemy->ApplyLevelModifiers();

						//if the enemy has phases, the direction will be handled by Enemy::SetPhase(). if not, we set it here
						if (e->m_enemy->m_phases.empty())
						{
							e->m_enemy->m_speed = GameObject::getSpeed_for_Scrolling(m_direction, e->m_enemy->m_speed.y);
						}
						
						//setting enemy generators: we need to create one generator per class
						if (m_total_class_probability[e->m_enemyclass] == 0)
						{
							float l_spawnCost = stof((*it)[ENEMY_CLASS_SPAWNCOST]) / spawnCostMultiplierTable[this->getSceneHazardLevelValue()];
							EnemyGenerator* generator = new EnemyGenerator(l_spawnCost, e->m_enemyclass, stof((*it)[ENEMY_CLASS_REPEAT_CHANCE]), stof((*it)[ENEMY_CLASS_MISS_CHANCE]));
							generator->m_spawnCostCollateralMultiplier = spawnCostCollateralMultiplierTable[this->getSceneHazardLevelValue()];
							m_sceneEnemyGenerators.push_back(generator);
						}

						//setting probabilities of spawn within enemy class
						e->m_proba_min = m_total_class_probability[e->m_enemyclass];
						e->m_proba_max = e->m_proba_min + e->m_probability;
						m_total_class_probability[e->m_enemyclass] = e->m_proba_max;
						enemy_count += e->m_proba_max;

						m_enemies_ranked_by_class[e->m_enemyclass].push_back(e);
					}

					//loading boss
					else if ((*it)[0].compare("boss") == 0)
					{
						EnemyBase* boss = FileLoader::LoadEnemyBase((*it)[BOSS], 1, stoi((*it)[BOSS_CLASS]));
						boss->m_enemy->m_level = stoi((*it)[BOSS_LEVEL]);

						if (boss->m_enemy->m_phases.empty())
						{
							boss->m_enemy->m_speed = GameObject::getSpeed_for_Scrolling(m_direction, boss->m_enemy->m_speed.y);
						}
						sf::Vector2f boss_pos = sf::Vector2f(atof((*it)[BOSS_SPAWN_X].c_str()) * SCENE_SIZE_X, atof((*it)[BOSS_SPAWN_Y].c_str()) * SCENE_SIZE_Y);
						boss_pos = GameObject::getPosition_for_Direction(m_direction, boss_pos);
						boss->m_enemy->setPosition(boss_pos);

						m_boss_list.push_back(boss);
						m_generating_boss = true;
					}

					if (enemy_count != 0 && m_direction != Directions::NO_DIRECTION)
					{
						m_generating_enemies = true;
						m_spawnClock.restart();
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
	m_name = name;
	try {
		//Loading the list of all scenes, contained in SCENES_FILE
		vector<vector<string>> scenesConfig = *(FileLoaderUtils::FileLoader(SCENES_FILE));

		for (std::vector<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
		{
			if ((*it)[SCENE_NAME].compare(name) == 0)
			{
				//Loading the linked scene names
				m_links[Directions::DIRECTION_UP] = (*it)[SCENE_LINK_UP];
				m_links[Directions::DIRECTION_DOWN] = (*it)[SCENE_LINK_DOWN];
				m_links[Directions::DIRECTION_RIGHT] = (*it)[SCENE_LINK_RIGHT];
				m_links[Directions::DIRECTION_LEFT] = (*it)[SCENE_LINK_LEFT];
				std::string scene_name = (*it)[SCENE_DISPLAYNAME];

				//Loading the particular scene that we want to load
				vector<vector<string>> config = *(FileLoaderUtils::FileLoader((*it)[SCENE_FILENAME]));
				for (std::vector<vector<string>>::iterator it = (config).begin(); it != (config).end(); it++)
				{
					if ((*it)[0].compare("bg") == 0)
					{
						m_bg = new Background(sf::Vector2f(0, 0), sf::Vector2f(0, 0), (*it)[BACKGROUND_NAME], sf::Vector2f(0, 0), m_direction);
						m_bg->m_display_name = scene_name;
					}
				}
			}

			//Drawing link zones and texts
			for (int i = 0; i < Directions::NO_DIRECTION; i++)
			{
				if (m_links[(Directions)i].compare("0") != 0)
				{
					//Getting the string of the "display name" for a each linked scene
					for (std::vector<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
					{
						if ((*it)[SCENE_NAME].compare(m_links[(Directions)i]) == 0)
						{
							//Getting the name
							m_bg->m_portals[(Directions)i]->m_display_name = (*it)[SCENE_DISPLAYNAME];
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
	LoadSceneFromFile(name, hazard_level, reverse_scene, first_scene);

	LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Scene '%s' loaded.", (char*)name.c_str()));
}

void Scene::DisplayDestructions(bool hazard_break)
{
	ostringstream ss;
	stringstream ratio;
	ratio.precision(4);
	ratio << 100.0f * (*CurrentGame).getHazard() / (*CurrentGame).m_hazardSpawned;

	ss << "Destructions: " << (*CurrentGame).getHazard() << " / " << (*CurrentGame).m_hazardSpawned << " [" << ratio.str() << "%]";
	if (hazard_break)
	{
		ss << "\n\n          HAZARD BREAK!!!";
	}
	m_textHazardBreak.setString(ss.str());

	if (m_direction != DIRECTION_DOWN)
	{
		
		m_textHazardBreak.setPosition(sf::Vector2f((SCENE_SIZE_X / 2) - (m_textHazardBreak.getLocalBounds().width / 2), ENDSCENE_SCORE_DISPLAY_POSITION_Y));
	}
	else
	{
		m_textHazardBreak.setPosition(sf::Vector2f((SCENE_SIZE_X / 2) - (m_textHazardBreak.getLocalBounds().width / 2), SCENE_SIZE_Y - ENDSCENE_SCORE_DISPLAY_POSITION_Y));
	}

	(*CurrentGame).addToFeedbacks(&this->m_textHazardBreak);
}

void Scene::DestroyScene()
{
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		if (m_bg->m_portals[(Directions)i] != NULL)
		{
			m_bg->m_portals[(Directions)i]->m_GarbageMe = true;
		}
	}

	if (m_bg->m_shop != NULL)
	{
		m_bg->m_shop->m_GarbageMe = true;
	}

	m_bg->m_GarbageMe = true;

	m_boss_list.clear();

	for (int i = 0; i < EnemyClass::NBVAL_EnemyClass; i++)
	{
		m_enemies_ranked_by_class[i].clear();
	}
	m_sceneEnemyGenerators.clear();
}

void Scene::GenerateEnemiesv2(Time deltaTime)
{
	for (std::vector<EnemyGenerator*>::iterator it = m_sceneEnemyGenerators.begin(); it != m_sceneEnemyGenerators.end(); ++it)
	{
		//SOURCES OF INCREMENTATION
		//Time
		sf::Time adjusted_time = deltaTime * (*CurrentGame).m_hyperspeedMultiplier;
		(*it)->m_spawnResource += adjusted_time.asSeconds();

		//SPAWN
		if ((*it)->m_spawnResource > (*it)->m_spawnCost)
		{
			//Randomization of the event
			float p = RandomizeFloatBetweenValues(sf::Vector2f(0, 1));
			if (p < (*it)->m_spawnRepeatProbability)
			{
				//Critical event: spawn enemy, pay discount cost
				float r = RandomizeFloatBetweenValues(sf::Vector2f(SPAWN_REPEAT_MINIMUM_RESOURCE, SPAWN_REPEAT_MAXIMUM_RESOURCE));
				(*it)->m_spawnResource *= r;
				SpawnEnemy((*it)->m_enemyClass);
				CollateralSpawnCost((*it)->m_spawnCost, (*it)->m_spawnCostCollateralMultiplier, (*it)->m_enemyClass);
				//printf("CRITICAL REPEAT class %d\n", (*it)->enemyClass);
			}
			else if (p > 1 - (*it)->m_spawnMissProbability)
			{
				//Critical event: don't spawn enemy, but still pay a (discount) cost
				float m = RandomizeFloatBetweenValues(sf::Vector2f(SPAWN_MISS_MINIMUM_RESOURCE, SPAWN_MISS_MAXIMUM_RESOURCE));
				(*it)->m_spawnResource *= m;
				//printf("CRITICAL MISS class %d\n", (*it)->enemyClass);
			}
			else
			{
				//Normal event: spawn enemy, pay total cost
				SpawnEnemy((*it)->m_enemyClass);
				float n = RandomizeFloatBetweenValues(sf::Vector2f(SPAWN_NORMAL_MINIMUM_RESOURCE, SPAWN_NORMAL_MAXIMUM_RESOURCE));
				(*it)->m_spawnResource = 0 + (n*(*it)->m_spawnCost);
				CollateralSpawnCost((*it)->m_spawnCost, (*it)->m_spawnCostCollateralMultiplier, (*it)->m_enemyClass);
			}
		}

		//DEBUG
		if ((*it)->m_enemyClass == 1)
		{
			//printf("RESSOURCES: %f / %f\n", (*it)->spawnResource, (*it)->spawnCost);
		}
	}
	
	m_spawnClock.restart();
}

void Scene::CollateralSpawnCost(float collatefal_cost, float collateral_multiplier, int below_enemy_class)
{
	for (std::vector<EnemyGenerator*>::iterator it = m_sceneEnemyGenerators.begin(); it != m_sceneEnemyGenerators.end(); ++it)
	{
		if ((*it)->m_enemyClass < below_enemy_class)
		{
			if (collateral_multiplier > 0)
			{
				(*it)->m_spawnResource -= (collatefal_cost * collateral_multiplier);
			}
			else
			{
				(*it)->m_spawnResource = 0;
			}
		}
	}
}

void Scene::SpawnEnemy(int enemy_class)
{
	Enemy* enemy = NULL;
	//Attention si total class probability vaut 0 �a va crasher - division par z�ro oblige. du coup il faut v�rifier que ce n'est pas �gal � 0.
	//int dice_roll = (rand() % (this->total_class_probability[enemy_class])) + 1;
	int dice_roll = RandomizeIntBetweenValues(0, m_total_class_probability[enemy_class]);
	
	for (std::vector<EnemyBase*>::iterator it = m_enemies_ranked_by_class[enemy_class].begin(); it != m_enemies_ranked_by_class[enemy_class].end(); ++it)
	{
		if (dice_roll >= (*it)->m_proba_min && dice_roll <= (*it)->m_proba_max)
		{
			enemy = (*it)->m_enemy->Clone();
			ApplyHazardLevelModifiers(getSceneHazardLevelValue(), *enemy);
			break;
		}
	}
	assert(enemy != NULL);
	enemy->setRotation(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	enemy->RotateFeedbacks(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));

	//RANDOM POSITION
	sf::Vector2f pos = enemy->getRandomXSpawnPosition((*CurrentGame).m_direction, enemy->m_size);
	enemy->setPosition(pos);
	(*CurrentGame).addToScene(enemy, EnemyObjectLayer, EnemyObject);

	//counting spawned enemies
	(*CurrentGame).m_hazardSpawned += enemy->m_money;
}

void Scene::GenerateBoss()
{
	for (std::vector<EnemyBase*>::iterator it = m_boss_list.begin(); it != m_boss_list.end(); ++it)
	{
		Enemy* m_boss = (*it)->m_enemy->Clone();
		m_boss->m_enemy_class = (EnemyClass)((*it)->m_enemyclass);
		(*CurrentGame).addToScene(m_boss, EnemyObjectLayer, EnemyObject);

		m_boss->setRotation(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
		m_boss->RotateFeedbacks(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));

		//counting spawned enemies
		(*CurrentGame).m_hazardSpawned += m_boss->m_money;
	}
}

void Scene::ApplyHazardLevelModifiers(int hazard_level, Enemy& enemy)
{
	enemy.m_speed.x *= enemySpeedModifierTable[hazard_level];
	enemy.m_speed.y *= enemySpeedModifierTable[hazard_level];
	for (std::vector<Weapon*>::iterator it = enemy.m_weapons_list.begin(); it != enemy.m_weapons_list.end(); it++)
	{
		(*it)->m_ammunition->m_speed.y *= ammoSpeedModifierTable[hazard_level];
	}
}

void Scene::GenerateEnemies(Time deltaTime)
{
	if (m_spawnClock.getElapsedTime() > sf::seconds(4))
	{
		m_spawnClock.restart();
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
		int dice_roll = (rand() % (m_total_class_probability[EnemyClass::ENEMYPOOL_ALPHA])) + 1;

		for (int i = 0; i < EnemyClass::NBVAL_EnemyClass; i++)
		{
			for (std::vector<EnemyBase*>::iterator it = m_enemies_ranked_by_class[i].begin(); it != m_enemies_ranked_by_class[i].end(); ++it)
			{
				if (dice_roll >= (*it)->m_proba_min && dice_roll <= (*it)->m_proba_max)
				{
					random_enemy_within_class[i] = (*it)->m_enemy;
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
			if (e->m_enemy->m_size.x > max_enemy_size.x)
			{
				max_enemy_size.x = e->m_enemy->m_size.x;
			}
			if (e->m_enemy->m_size.y > max_enemy_size.y)
			{
				max_enemy_size.y = e->m_enemy->m_size.y;

				cluster->push_back(e);
			}

			sf::Vector2f size = sf::Vector2f(((nb_rows - 1) * xspread) + max_enemy_size.x, ((nb_lines - 1) * yspread) + max_enemy_size.y);

			//RANDOM POSITION TO SPAWN THE CLUSTER
			sf::Vector2f pos = cluster->front()->m_enemy->getRandomXSpawnPosition((*CurrentGame).m_direction, max_enemy_size, size);

			//generating the cluster at the given coordinates
			EnemyPool* generated_cluster = new EnemyPool(pos, nb_lines, nb_rows, xspread, yspread, cluster);
		}
	}
}

float HazardLevelsBeastBonus[HazardLevels::NB_HAZARD_LEVELS] = { 0.0, 0.5, 1.0, 1.5, 2.0 };

void Scene::HazardBreak()
{
	if (m_hazard_level_unlocked < HazardLevels::NB_HAZARD_LEVELS - 1)
	{
		m_hazard_level_unlocked++;
	}
	
	LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Hazard level up: %d/5\n", m_hazard_level + 1));
}

float Scene::getSceneBeastScore()
{
	float bonus = 0;
	if (m_hazard_level < HazardLevels::NB_HAZARD_LEVELS && m_hazard_level >= 0)
	{
		bonus = HazardLevelsBeastBonus[m_hazard_level];
	}
	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error, The scene has a 'hazard_level' (%d) beyond existing values\n", m_hazard_level);
		m_hazard_level = 0;
	}

	return bonus;
}

int Scene::getSceneHazardLevelUnlockedValue()
{
	return m_hazard_level_unlocked;
}

int Scene::getSceneHazardLevelValue()
{
	return m_hazard_level;
}

