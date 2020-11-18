#include "Scene.h"

extern Game* CurrentGame;

void Scene::LoadSceneFromFile(string name, int hazard_level, bool reverse_scene, bool first_scene)
{
	LOGGER_WRITE(Logger::DEBUG, TextUtils::format("Loading scene '%s'", (char*)name.c_str()));
	srand(time(NULL));
	m_name = name;
	for (int i = 0; i < NBVAL_EnemyClass; i++)
	{
		m_total_class_probability[i] = 0;
	}
	m_generating_enemies = false;
	m_generating_boss = false;
	m_hazard_level = hazard_level;
	m_hazardbreak_has_occurred = false;
	m_canHazardBreak = false;

	for (int i = 0; i < NBVAL_SceneScripts; i++)
	{
		m_scripts[i] = false;
	}

	int p = 0;
	int enemy_count = 0;
	
	//Loading the linked scene names
	m_links[DIRECTION_UP] = (*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_UP];
	m_links[DIRECTION_DOWN] = (*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_DOWN];
	m_links[DIRECTION_RIGHT] = (*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_RIGHT];
	m_links[DIRECTION_LEFT] = (*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_LEFT];

	m_canHazardBreak = ((*CurrentGame).m_generalScenesConfig[name][SCENE_HAZARD_BREAK].compare("1") == 0) ? true : false;
	m_level = stoi((*CurrentGame).m_generalScenesConfig[name][SCENE_LEVEL]);// +hazard_level;

	std::string scene_name = (*CurrentGame).m_generalScenesConfig[name][SCENE_DISPLAYNAME];

	//Loading the particular scene that we want to load
	size_t sceneVectorSize = (*CurrentGame).m_sceneConfigs[name].size();
	for (size_t i = 0; i < sceneVectorSize; i++)
	{
		if ((*CurrentGame).m_sceneConfigs[name][i][0].compare("bg") == 0)
		{
			m_direction = NO_DIRECTION;
			bool hub = false;

			m_vspeed = stoi((*CurrentGame).m_sceneConfigs[name][i][BACKGROUND_VSPEED]);
			float w = stoi((*CurrentGame).m_sceneConfigs[name][i][BACGKROUND_WIDTH]);
			float h = stoi((*CurrentGame).m_sceneConfigs[name][i][BACKGROUND_HEIGHT]);

			//Assigning the right direction for the scene
			if ((*CurrentGame).m_sceneConfigs[name][i][BACKGROUND_VERTICAL].compare("V") == 0)
			{
				if (!reverse_scene)
				{
					m_direction = DIRECTION_UP;
				}
				else
				{
					m_direction = DIRECTION_DOWN;
				}
			}
			else if ((*CurrentGame).m_sceneConfigs[name][i][BACKGROUND_VERTICAL].compare("H") == 0)
			{
				if (!reverse_scene)
				{
					m_direction = DIRECTION_RIGHT;
				}
				else
				{
					m_direction = DIRECTION_LEFT;
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
				m_direction = NO_DIRECTION;
			}

			m_bg = new Background(sf::Vector2f(0, 0), speed, (*CurrentGame).m_sceneConfigs[name][i][BACKGROUND_NAME], sf::Vector2f(w, h), (*CurrentGame).m_direction, first_screen_offset);
			m_bg->m_display_name = scene_name;
			(*CurrentGame).addToScene(m_bg, true);

			//Getting the display name of the scene and loading it into the scene portals
			for (int i = 0; i < NO_DIRECTION; i++)
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
					m_bg->m_portals[(Directions)i]->m_display_name = (*CurrentGame).m_generalScenesConfig[m_links[(Directions)i]][SCENE_DISPLAYNAME];

					//Getting the level of each linked scene
					m_bg->m_portals[(Directions)i]->m_level = stoi((*CurrentGame).m_generalScenesConfig[m_links[(Directions)i]][SCENE_LEVEL]);

					//Displaying the portals
					(*CurrentGame).addToScene(m_bg->m_portals[(Directions)i], true);
				}
			}

			if (first_scene)
			{
				m_bg->SetPortalsState(PortalOpen);
			}
			else
			{
				m_bg->SetPortalsState(PortalGhost);
			}
		}
		else if ((*CurrentGame).m_sceneConfigs[name][i][0].compare("shop") == 0)
		{
			//creating the shop
			float w_ = stoi((*CurrentGame).m_sceneConfigs[name][i][SHOP_WIDTH]);
			float h_ = stoi((*CurrentGame).m_sceneConfigs[name][i][SHOP_HEIGHT]);
			m_bg->m_shop = new Shop(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2), sf::Vector2f(0, 0), (*CurrentGame).m_sceneConfigs[name][i][SHOP_TEXTURE_NAME], sf::Vector2f(w_, h_), sf::Vector2f(w_ / 2, h_ / 2));
			m_bg->m_shop->m_level = m_level;// = stoi((*CurrentGame).m_sceneConfigs[name][i][SHOP_LEVEL]);
			m_bg->m_shop->m_visible = true;
			ostringstream ss;
			ss << m_bg->m_display_name << " Shop (Level " << m_bg->m_shop->m_level << ")";
			m_bg->m_shop->m_display_name = ss.str();
			(*CurrentGame).addToScene(m_bg->m_shop, true);

			//creating shop content
			//Ship::FillShopWithRandomObjets(NUMBER_OF_OBJECTS_GENERATED_IN_SHOP, m_bg->m_shop, ENEMYPOOL_ALPHA, -1);
			//Ship::FillShopWithRandomObjets(NUMBER_OF_RARE_OBJECTS_GENERATED_IN_SHOP, m_bg->m_shop, ENEMYPOOL_BETA, -1);
			//LOGGER_WRITE(Logger::DEBUG, TextUtils::format("Filling scene '%s' shop with new items.\n", (char*)name.c_str()));
			Ship::FillShopWithRandomObjets(1, m_bg->m_shop, ENEMYPOOL_ALPHA, (int)Engine);
			Ship::FillShopWithRandomObjets(1, m_bg->m_shop, ENEMYPOOL_ALPHA, (int)Armor);
			Ship::FillShopWithRandomObjets(1, m_bg->m_shop, ENEMYPOOL_ALPHA, (int)Shield);
			Ship::FillShopWithRandomObjets(1, m_bg->m_shop, ENEMYPOOL_ALPHA, (int)Module);
			Ship::FillShopWithRandomObjets(1, m_bg->m_shop, ENEMYPOOL_ALPHA, (int)NBVAL_Equipment);
		}
		//Loading enemies
		else if ((*CurrentGame).m_sceneConfigs[name][i][0].compare("enemy") == 0)
		{
			EnemyBase* e = FileLoader::LoadEnemyBase((*CurrentGame).m_sceneConfigs[name][i][ENEMY], stoi((*CurrentGame).m_sceneConfigs[name][i][ENEMY_PROBABILITY]), stoi((*CurrentGame).m_sceneConfigs[name][i][ENEMY_CLASS]));
			if (e)
			{
				e->m_enemy->m_level = m_level;//stoi((*CurrentGame).m_sceneConfigs[name][i][ENEMY_CLASS_LEVEL]);
				e->m_enemy->ApplyLevelModifiers();

				//if the enemy has phases, the direction will be handled by Enemy::SetPhase(). if not, we set it here
				if (e->m_enemy->m_phases.empty())
				{
					e->m_enemy->m_speed = GameObject::getSpeed_for_Scrolling(m_direction, e->m_enemy->m_speed.y);
				}

				//setting enemy generators: we need to create one generator per class
				if (m_total_class_probability[e->m_enemyclass] == 0)
				{
					float l_spawnCost = stof((*CurrentGame).m_sceneConfigs[name][i][ENEMY_CLASS_SPAWNCOST]) / spawnCostMultiplierTable[this->getSceneHazardLevelValue()];
					EnemyGenerator* generator = new EnemyGenerator(l_spawnCost, e->m_enemyclass, stof((*CurrentGame).m_sceneConfigs[name][i][ENEMY_CLASS_REPEAT_CHANCE]), stof((*CurrentGame).m_sceneConfigs[name][i][ENEMY_CLASS_MISS_CHANCE]));
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
		}
		//Loading boss
		else if ((*CurrentGame).m_sceneConfigs[name][i][0].compare("boss") == 0)
		{
			EnemyBase* boss = FileLoader::LoadEnemyBase((*CurrentGame).m_sceneConfigs[name][i][BOSS], 1, stoi((*CurrentGame).m_sceneConfigs[name][i][BOSS_CLASS]));
			if (boss)
			{
				boss->m_enemy->m_level = m_level;//stoi((*CurrentGame).m_sceneConfigs[name][i][BOSS_LEVEL]);

				if (boss->m_enemy->m_phases.empty())
				{
					boss->m_enemy->m_speed = GameObject::getSpeed_for_Scrolling(m_direction, boss->m_enemy->m_speed.y);
				}
				sf::Vector2f boss_pos = sf::Vector2f(atof((*CurrentGame).m_sceneConfigs[name][i][BOSS_SPAWN_X].c_str()) * SCENE_SIZE_X, atof((*CurrentGame).m_sceneConfigs[name][i][BOSS_SPAWN_Y].c_str()) * SCENE_SIZE_Y);
				boss_pos = GameObject::getPosition_for_Direction(m_direction, boss_pos);
				boss->m_enemy->setPosition(boss_pos);
				boss->m_enemy->setRotation(stoi((*CurrentGame).m_sceneConfigs[name][i][BOSS_SPAWN_ROTATION]));

				m_boss_list.push_back(boss);
				m_generating_boss = true;
			}
		}
		//Loading optional scripts
		else if ((*CurrentGame).m_sceneConfigs[name][i][0].compare("script") == 0)
		{
			for (int j = 0; j < NBVAL_SceneScripts; j++)
			{
				m_scripts[j] = (bool)stoi((*CurrentGame).m_sceneConfigs[name][i][j+1]);
			}

			if (m_scripts[SceneScript_PortalOpenDuringBoss])
			{
				m_bg->SetPortalsState(PortalOpen);
			}
		}
		//Loading dialogs
		else if ((*CurrentGame).m_sceneConfigs[name][i][0].compare("dialog") == 0)
		{
			Dialog* dialog = Enemy::LoadDialog((*CurrentGame).m_sceneConfigs[name][i][SCENE_DIALOG_NAME]);
			vector<Dialog*> chained_dialogs;
			chained_dialogs.push_back(dialog);
			while (!dialog->m_next_dialog_name.empty() && dialog->m_next_dialog_name.compare("0") != 0)
			{
				dialog = Enemy::LoadDialog(dialog->m_next_dialog_name);
				chained_dialogs.push_back(dialog);
			}
			//m_dialogs.insert(map<float, vector<Dialog*> >::value_type(stoi((*CurrentGame).m_sceneConfigs[name][i][SCENE_DIALOG_TIME]), chained_dialogs));
			m_dialogs.push_back(std::make_pair(stoi((*CurrentGame).m_sceneConfigs[name][i][SCENE_DIALOG_TIME]), chained_dialogs));
		}
		//Loading specific music
		else if ((*CurrentGame).m_sceneConfigs[name][i][0].compare("music") == 0)
		{
			m_scene_music = (*CurrentGame).m_sceneConfigs[name][i][1];
		}

		if (enemy_count != 0 && m_direction != NO_DIRECTION)
		{
			m_generating_enemies = true;
			m_spawnClock.restart();
		}
	}
}

Scene::Scene(string name)
{
	m_name = name;
	try {
		//Loading the list of all scenes, contained in SCENES_FILE
		vector<vector<string> > scenesConfig = *(FileLoaderUtils::FileLoader(SCENES_FILE));

		for (std::vector<vector<string> >::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
		{
			if ((*it)[SCENE_NAME].compare(name) == 0)
			{
				//Loading the linked scene names
				m_links[DIRECTION_UP] = (*it)[SCENE_LINK_UP];
				m_links[DIRECTION_DOWN] = (*it)[SCENE_LINK_DOWN];
				m_links[DIRECTION_RIGHT] = (*it)[SCENE_LINK_RIGHT];
				m_links[DIRECTION_LEFT] = (*it)[SCENE_LINK_LEFT];
				std::string scene_name = (*it)[SCENE_DISPLAYNAME];

				//Loading the particular scene that we want to load
				vector<vector<string> > config = *(FileLoaderUtils::FileLoader((*it)[SCENE_FILENAME]));
				for (std::vector<vector<string> >::iterator it = (config).begin(); it != (config).end(); it++)
				{
					if ((*it)[0].compare("bg") == 0)
					{
						m_bg = new Background(sf::Vector2f(0, 0), sf::Vector2f(0, 0), (*it)[BACKGROUND_NAME], sf::Vector2f(0, 0), m_direction);
						m_bg->m_display_name = scene_name;
					}
				}
			}

			//Drawing link zones and texts
			for (int i = 0; i < NO_DIRECTION; i++)
			{
				if (m_links[(Directions)i].compare("0") != 0)
				{
					//Getting the string of the "display name" for a each linked scene
					for (std::vector<vector<string> >::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
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
		LOGGER_WRITE(Logger::LERROR, ex.what());
	}
}

Scene::Scene(string name, int hazard_level, bool reverse_scene, bool first_scene)
{
	LoadSceneFromFile(name, hazard_level, reverse_scene, first_scene);

	LOGGER_WRITE(Logger::DEBUG, TextUtils::format("Scene '%s' loaded.", (char*)name.c_str()));
}

Scene::~Scene()
{
	if (m_bg != NULL)
	{
		for (int i = 0; i < NO_DIRECTION; i++)
			if (m_bg->m_portals[i] != NULL)
				m_bg->m_portals[i]->Death();

		if (m_bg->m_shop != NULL)
			m_bg->m_shop->Death();

		m_bg->Death();
	}

	for (int i = 0; i < NBVAL_EnemyClass; i++)
		for (EnemyBase* enemy : m_enemies_ranked_by_class[i])
			delete enemy;

	for (EnemyBase* boss : m_boss_list)
		delete boss;

	for (EnemyGenerator* generator : m_sceneEnemyGenerators)
		delete generator;
}

void Scene::PlayTitleFeedback()
{
	//feedback
	sf::Color _white = sf::Color::Color(255, 255, 255, 255);//white
	sf::Vector2f position = sf::Vector2f(SCENE_SIZE_X / 2, SCENE_TITLE_OFFSET_Y);
	position = m_direction == DIRECTION_DOWN ? GameObject::getPosition_for_Direction(m_direction, position) : position;
	SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 30, _white, position);
	ostringstream ss;
	ss << this->m_name;
	text_feedback->setString(ReplaceAll(ss.str(), "_", " "));
	SFTextPop* pop_feedback = new SFTextPop(text_feedback, SCENE_TILE_FADE_IN_TIME, SCENE_TILE_NOT_FADED_TIME, SCENE_TILE_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
	pop_feedback->setPosition(sf::Vector2f(position.x - pop_feedback->getGlobalBounds().width / 2, position.y));
	delete text_feedback;
	(*CurrentGame).addToFeedbacks(pop_feedback);
}

bool Scene::CheckHazardBreakConditions()
{
	//Score = destruction rank + graze rank
	//RANKS: 0 = D, 1 = C, 2 = B, 3 = A, 4 = S, 5 = SS

	//Destruction
	if ((*CurrentGame).getHazard() == (*CurrentGame).m_hazardSpawned)
	{
		m_score_destruction = 4;//S
	}
	else if (1.0f * (*CurrentGame).getHazard() / (*CurrentGame).m_hazardSpawned > 0.95)
	{
		m_score_destruction = 3;//A
	}
	else if (1.0f * (*CurrentGame).getHazard() / (*CurrentGame).m_hazardSpawned > 0.8)
	{
		m_score_destruction = 2;//B
	}
	else
	{
		m_score_destruction = 1;//C
	}

	//Graze
	//m_score_graze = (*CurrentGame).m_playerShip->m_graze_level + 1;

	//Hits taken
	if ((*CurrentGame).m_playerShip->m_hits_taken == 0)//untouched
	{
		m_score_graze = 4;//S
	}
	else if ((*CurrentGame).m_playerShip->m_hits_taken < 6)//1-5 hits taken
	{
		m_score_graze = 3;//A
	}
	else if ((*CurrentGame).m_playerShip->m_hits_taken < 16)//6-15 hits taken
	{
		m_score_graze = 2;//B
	}
	else//>10 hits taken
	{
		m_score_graze = 1;//C
	}

	//Total
	m_score_total = (m_score_destruction + m_score_graze) / 2;
	if ((m_score_destruction + m_score_graze) % 2 == 1 && (m_score_destruction + m_score_graze) < 8)
	{
		m_score_total++;//because we want to round up A + S = S
	}
	if (m_score_destruction == 1 && m_score_graze == 1)
	{
		m_score_total = 0;//C + C = D
	}
	if (m_score_destruction == 4 && m_score_graze == 4)
	{
		m_score_total = 5;//S + S = SS
	}

	//return m_score_total >= 4;//S rank minimum for hazard break
	return true;
}

void Scene::DisplayDestructions(bool hazard_break)//OLD: to remove
{
	ostringstream ss;
	ss.precision(1);
	ss << fixed;
	ss << "Destructions: " << (*CurrentGame).getHazard() << " / " << (*CurrentGame).m_hazardSpawned << " [" << 100.0f * (*CurrentGame).getHazard() / (*CurrentGame).m_hazardSpawned << "%]";
	//feedback
	sf::Color _white = sf::Color::Color(255, 255, 255, 255);//white
	sf::Vector2f position = sf::Vector2f(SCENE_SIZE_X / 2, DESTRUCTIONS_DISPLAY_OFFSET_Y);
	SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 24, _white, position);
	text_feedback->setString(ss.str());
	SFTextPop* pop_feedback = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
	pop_feedback->setPosition(sf::Vector2f(position.x - pop_feedback->getGlobalBounds().width / 2, position.y));
	(*CurrentGame).addToFeedbacks(pop_feedback);

	if (hazard_break && m_hazard_level == m_hazard_level_unlocked && m_hazard_level < NB_HAZARD_LEVELS - 1)
	{
		text_feedback->setString("HAZARD BREAK!!!");
		SFTextPop* pop_feedback2 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
		pop_feedback2->setPosition(sf::Vector2f(position.x - pop_feedback2->getGlobalBounds().width / 2, position.y + pop_feedback->getGlobalBounds().height + INTERACTION_INTERBLOCK));
		(*CurrentGame).addToFeedbacks(pop_feedback2);
	}
	else if (!hazard_break && m_hazard_level == m_hazard_level_unlocked && m_hazard_level < NB_HAZARD_LEVELS - 1)
	{
		text_feedback->setString("Score 100% to unlock next hazard level");
		text_feedback->setCharacterSize(18);
		SFTextPop* pop_feedback2 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
		pop_feedback2->setPosition(sf::Vector2f(position.x - pop_feedback2->getGlobalBounds().width / 2, position.y + pop_feedback->getGlobalBounds().height + INTERACTION_INTERBLOCK));
		(*CurrentGame).addToFeedbacks(pop_feedback2);
	}
	
	delete text_feedback;
}

void Scene::DisplayScore(bool hazard_break)
{
	//Display
	ostringstream ss_destruction;
	ss_destruction.precision(1);
	ss_destruction << fixed;
	//ss << "Destructions: " << (*CurrentGame).getHazard() << " / " << (*CurrentGame).m_hazardSpawned << " [" << 100.0f * (*CurrentGame).getHazard() / (*CurrentGame).m_hazardSpawned << "%]";
	ss_destruction << "Destructions: " << 100.0f * (*CurrentGame).getHazard() / (*CurrentGame).m_hazardSpawned << "%";
	switch (m_score_destruction)
	{
		case 1:
		{
			ss_destruction << " -> Rank C";
			break;
		}
		case 2:
		{
			ss_destruction << " -> Rank B";
			break;
		}
		case 3:
		{
			ss_destruction << " -> Rank A";
			break;
		}
		case 4:
		{
			ss_destruction << " -> Rank S";
			break;
		}
	}

	ostringstream ss_graze;
	//ss_graze << "Graze: " << (*CurrentGame).m_playerShip->m_graze_level << " / " << NB_GRAZE_LEVELS - 1;
	ss_graze << "Hits taken: " << (*CurrentGame).m_playerShip->m_hits_taken;
	switch (m_score_graze)
	{
		case 1:
		{
			ss_graze << " -> Rank C";
			break;
		}
		case 2:
		{
			ss_graze << " -> Rank B";
			break;
		}
		case 3:
		{
			ss_graze << " -> Rank A";
			break;
		}
		case 4:
		{
			ss_graze << " -> Rank S";
			break;
		}
	}

	ostringstream ss_total;
	ss_total << "Score total: ";
	switch (m_score_total)
	{
		case 0:
		{
			ss_total << " Rank D";
			break;
		}
		case 1:
		{
			ss_total << " Rank C";
			break;
		}
		case 2:
		{
			ss_total << " Rank B";
			break;
		}
		case 3:
		{
			ss_total << " Rank A";
			break;
		}
		case 4:
		{
			ss_total << " Rank S";
			break;
		}
		case 5:
		{
			ss_total << " Rank S+";
			break;
		}
	}

	float text_height = 0;

	//destruction
	sf::Color _white = sf::Color::Color(255, 255, 255, 255);//white
	sf::Vector2f position = sf::Vector2f(SCENE_SIZE_X / 2, DESTRUCTIONS_DISPLAY_OFFSET_Y);
	SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 24, _white, position);
	text_feedback->setString(ss_destruction.str());
	SFTextPop* pop_feedback = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
	pop_feedback->setPosition(sf::Vector2f(position.x - pop_feedback->getGlobalBounds().width / 2, position.y));
	text_height = position.y;
	(*CurrentGame).addToFeedbacks(pop_feedback);

	//graze
	text_feedback->setString(ss_graze.str());
	text_height += text_feedback->getGlobalBounds().height + INTERACTION_INTERBLOCK;
	SFTextPop* pop_feedback2 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
	pop_feedback2->setPosition(sf::Vector2f(position.x - pop_feedback2->getGlobalBounds().width / 2, text_height));
	(*CurrentGame).addToFeedbacks(pop_feedback2);

	//total
	text_feedback->setString(ss_total.str());
	sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow
	text_feedback->setColor(_yellow);
	text_height += text_feedback->getGlobalBounds().height + 2*INTERACTION_INTERBLOCK;
	SFTextPop* pop_feedback3 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
	pop_feedback3->setPosition(sf::Vector2f(position.x - pop_feedback3->getGlobalBounds().width / 2, text_height));
	(*CurrentGame).addToFeedbacks(pop_feedback3);

	if (hazard_break && m_hazard_level == m_hazard_level_unlocked && m_hazard_level < NB_HAZARD_LEVELS - 1)
	{
		text_feedback->setString("STAGE CLEARED! Next hazard level unlocked!!");
		text_feedback->setCharacterSize(18);
		sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow
		text_feedback->setColor(_yellow);
		text_height += text_feedback->getGlobalBounds().height + 2*INTERACTION_INTERBLOCK;
		SFTextPop* pop_feedback4 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
		pop_feedback4->setPosition(sf::Vector2f(position.x - pop_feedback4->getGlobalBounds().width / 2, text_height));
		(*CurrentGame).addToFeedbacks(pop_feedback4);
	}
	else if (!hazard_break && m_hazard_level == m_hazard_level_unlocked && m_hazard_level < NB_HAZARD_LEVELS - 1)
	{
		text_feedback->setString("STAGE CLEARED! Get a total S Rank to unlock next hazard level");
		text_feedback->setCharacterSize(18);
		text_height += text_feedback->getGlobalBounds().height + 2*INTERACTION_INTERBLOCK;
		SFTextPop* pop_feedback4 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
		pop_feedback4->setPosition(sf::Vector2f(position.x - pop_feedback4->getGlobalBounds().width / 2, text_height));
		(*CurrentGame).addToFeedbacks(pop_feedback4);
	}
	else
	{
		text_feedback->setString("STAGE CLEARED! Hazard level already beaten");
		text_feedback->setCharacterSize(18);
		text_height += text_feedback->getGlobalBounds().height + 2*INTERACTION_INTERBLOCK;
		SFTextPop* pop_feedback4 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
		pop_feedback4->setPosition(sf::Vector2f(position.x - pop_feedback4->getGlobalBounds().width / 2, text_height));
		(*CurrentGame).addToFeedbacks(pop_feedback4);
	}
	//else if (m_hazard_level < m_hazard_level_unlocked)
	//{
	//	text_feedback->setString("Hazard level already beaten");
	//	text_feedback->setCharacterSize(18);
	//	text_height += text_feedback->getGlobalBounds().height + INTERACTION_INTERBLOCK;
	//	SFTextPop* pop_feedback4 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
	//	pop_feedback4->setPosition(sf::Vector2f(position.x - pop_feedback4->getGlobalBounds().width / 2, text_height));
	//	(*CurrentGame).addToFeedbacks(pop_feedback4);
	//}
	//else if (m_hazard_level == NB_HAZARD_LEVELS - 1)
	//{
	//	text_feedback->setString("Max hazard level reached");
	//	text_feedback->setCharacterSize(18);
	//	text_height += text_feedback->getGlobalBounds().height + INTERACTION_INTERBLOCK;
	//	SFTextPop* pop_feedback4 = new SFTextPop(text_feedback, DESTRUCTIONS_DISPLAY_FADE_IN_TIME, DESTRUCTIONS_DISPLAY_NOT_FADED_TIME, DESTRUCTIONS_DISPLAY_FADE_OUT_TIME, NULL, 0, sf::Vector2f(0, 0));
	//	pop_feedback4->setPosition(sf::Vector2f(position.x - pop_feedback4->getGlobalBounds().width / 2, text_height));
	//	(*CurrentGame).addToFeedbacks(pop_feedback4);
	//}

	delete text_feedback;
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
	//Attention si total class probability vaut 0 ça va crasher - division par zéro oblige. du coup il faut vérifier que ce n'est pas égal à 0.
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
	enemy->UpdateHealthBars();//update health bar position
	(*CurrentGame).addToScene(enemy, true);

	//counting spawned enemies
	(*CurrentGame).m_hazardSpawned += enemy->m_money;
}

void Scene::GenerateBoss()
{
	for (std::vector<EnemyBase*>::iterator it = m_boss_list.begin(); it != m_boss_list.end(); ++it)
	{
		Enemy* boss = (*it)->m_enemy->Clone();
		boss->m_enemy_class = (EnemyClass)((*it)->m_enemyclass);
		(*CurrentGame).addToScene(boss, true);

		boss->setRotation(GameObject::getRotation_for_Direction((*CurrentGame).m_direction) + boss->getRotation());
		boss->RotateFeedbacks(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));

		//counting spawned enemies
		(*CurrentGame).m_hazardSpawned += boss->m_money;
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
		// liste de patterns associés : 0, 0, 0, 0, 0, 

		//chosing a random enemy within a class of enemies
		//for each sceneEnemyClassesAvailable[i]
		Enemy* random_enemy_within_class[NBVAL_EnemyClass];

		//Attention si total class probability vaut 0 ça va crasher - division par zéro oblige. du coup il faut vérifier que ce n'est pas égal à 0.
		int dice_roll = (rand() % (m_total_class_probability[ENEMYPOOL_ALPHA])) + 1;

		for (int i = 0; i < NBVAL_EnemyClass; i++)
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
			EnemyPoolElement* e = new EnemyPoolElement(random_enemy_within_class[ENEMYPOOL_ALPHA], ENEMYPOOL_ALPHA, NoMovePattern);
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

void Scene::HazardBreak()
{
	if (m_hazard_level_unlocked < HazardLevels::NB_HAZARD_LEVELS - 1)
	{
		m_hazard_level_unlocked++;
	}
	
	LOGGER_WRITE(Logger::DEBUG, TextUtils::format("Hazard level up: %d/5\n", m_hazard_level + 1));
}

float Scene::getSceneBeastScore(int for_hazard_level)
{
	float bonus = 0;
	if (for_hazard_level < HazardLevels::NB_HAZARD_LEVELS && for_hazard_level >= 0)
	{
		bonus = HazardLevelsBeastBonus[for_hazard_level];
	}
	else
	{
		LOGGER_WRITE(Logger::DEBUG, TextUtils::format("<!> Error, The scene has a 'hazard_level' (%d) beyond existing values\n", for_hazard_level));
		for_hazard_level = 0;
	}

	return bonus;
}

int Scene::getSceneHazardLevelUnlockedValue()
{
	return m_hazard_level_unlocked;
}

void Scene::setSceneHazardLevelUnlockedValue(int hazard_unlocked_value)
{
	m_hazard_level_unlocked = hazard_unlocked_value;
}

int Scene::getSceneHazardLevelValue()
{
	return m_hazard_level;
}

