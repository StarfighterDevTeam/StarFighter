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
	this->hazard_level = hazard_level;

	this->m_hazardbreak_has_occurred = false;
	int p = 0;
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
							first_screen_offset = Independant::getSize_for_Direction(this->direction, sf::Vector2f(SCENE_SIZE_X, SCENE_SIZE_Y)).y;
						}

						sf::Vector2f speed = Independant::getSpeed_for_Scrolling(this->direction, this->vspeed);

						if (hub)
						{
							this->direction = Directions::NO_DIRECTION;
						}
						this->bg = new Independant(sf::Vector2f(0, 0), speed, (*it)[SceneDataBackground::BACKGROUND_NAME], sf::Vector2f(w, h));

						sf::Vector2f size_for_dir = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(w, h));
						this->bg->setPosition_Y_for_Direction((*CurrentGame).direction, sf::Vector2f(size_for_dir.x / 2, (-size_for_dir.y / 2) + first_screen_offset), true);
						this->bg->setVisible(true);
						this->bg->isOnScene = true;
						this->bg->display_name = scene_name;

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
										this->links_displayname[(Directions)i] = (*it)[ScenesData::SCENE_DISPLAYNAME];
									}
									//And drawing the rect and text accordingly
									this->SetLinkZone((Directions)i);
								}
							}
						}
					}

					//Loading enemies
					int enemy_count = 0;
					if ((*it)[0].compare("enemy") == 0)
					{
						EnemyBase* e = FileLoader::LoadEnemy((*it)[SceneDataEnemy::ENEMY], stoi((*it)[SceneDataEnemy::ENEMY_PROBABILITY].c_str()), stoi((*it)[SceneDataEnemy::ENEMY_POOLSIZE]), stoi((*it)[SceneDataEnemy::ENEMY_CLASS]));
						e->enemy->speed = Independant::getSpeed_for_Scrolling(this->direction, e->enemy->speed.y);
						this->sceneIndependantsLayered[e->enemyclass].push_back(e);

						//giving intervall of hit values for dice rolls
						p++;
						e->proba_min = p;
						p += (e->probability - 1);
						e->proba_max = p;
						this->total_class_probability[e->enemyclass] = e->proba_max;
						enemy_count += e->proba_max;
						//this->enemies.push_back(*e);//legacy, to delete when pools are done ?
						this->enemies_ranked_by_class[e->enemyclass].push_back(*e);
						//hazard value automatic calculation
						hazard_break_value += e->enemy->getMoney() * e->poolsize * HAZARD_BREAK_RATIO * (1 + HAZARD_BREAK_MULTIPLIER*this->hazard_level);
					}

					if (enemy_count != 0 && this->direction != Directions::NO_DIRECTION)
					{
						generating_enemies = true;
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

void Scene::SetLinkZone(Directions direction)
{
	//Rect
	float size_x = Independant::getSize_for_Direction(direction, sf::Vector2f(SCENE_SIZE_X, SCENE_SIZE_Y)).x - (2 * HUB_EXIT_X_MIN_RATIO * SCENE_SIZE_X);//circa 550
	float size_y = HUB_EXIT_X_MIN_RATIO * SCENE_SIZE_X;//150
	this->link_zone[direction].rect.setSize(Independant::getSize_for_Direction(direction, sf::Vector2f(size_x, size_y)));
	this->link_zone[direction].rect.setFillColor(sf::Color(0, 128, 128, 128));
	this->link_zone[direction].rect.setOutlineThickness(1);
	this->link_zone[direction].rect.setOrigin(this->link_zone[direction].rect.getSize().x / 2, this->link_zone[direction].rect.getSize().y / 2);
	this->link_zone[direction].rect.setOutlineColor(sf::Color(128, 0, 0));

	float first_scene_offset = Independant::getSize_for_Direction(direction, sf::Vector2f(SCENE_SIZE_X, SCENE_SIZE_Y)).y;
	sf::Vector2f bg_size = Independant::getSize_for_Direction(direction, this->bg->m_size);

	this->link_zone[direction].setPosition_Y_for_Direction(direction, sf::Vector2f(bg_size.x / 2, -bg_size.y + (size_y / 2) + first_scene_offset), true);
	this->link_zone[direction].rect.setPosition(this->link_zone[direction].getPosition());

	//Title
	sf::Font* font = new sf::Font();
	if (!font->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
	{
		// error
		//TODO: font loader
	}
	this->link_zone[direction].title.setFont(*font);
	this->link_zone[direction].title.setCharacterSize(16);
	this->link_zone[direction].title.setColor(sf::Color::White);
	ostringstream ss;
	ss << this->links_displayname[direction];
	this->link_zone[direction].title.setString(ss.str());
	this->link_zone[direction].title.setRotation(0.0);

	float w = this->link_zone[direction].title.getGlobalBounds().width;
	float h = this->link_zone[direction].title.getGlobalBounds().height;
	this->link_zone[direction].title.setOrigin(sf::Vector2f(w / 2, h / 2));
	this->link_zone[direction].title_offset = Independant::getSpeed_for_Scrolling(direction, -HUB_EXIT_X_MIN_RATIO * SCENE_SIZE_X * HUB_LINK_NAME_OFFSET_RATIO);
	this->link_zone[direction].title.setPosition(this->link_zone[direction].getPosition().x, this->link_zone[direction].getPosition().y);

	if (direction == Directions::DIRECTION_RIGHT || direction == Directions::DIRECTION_LEFT)
	{
		this->link_zone[direction].title.setRotation(Independant::getRotation_for_Direction(direction));
		//this->link_zone[direction].title.setRotation(270.0);
	}

	//(*CurrentGame).addToScene(&this->link_zone[direction], LayerType::LinkZoneLayer, IndependantType::LinkZone);
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
						this->bg = new Independant(sf::Vector2f(0, 0), sf::Vector2f(0, 0), (*it)[SceneDataBackground::BACKGROUND_NAME], sf::Vector2f(0, 0));
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
							this->links_displayname[(Directions)i] = (*it)[ScenesData::SCENE_DISPLAYNAME];
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
		this->GenerateEnemies(deltaTime);
	}
	if ((*CurrentGame).getHazard() > hazard_break_value - 1 && hazard_break_value > 0 && !m_hazardbreak_has_occurred)
	{
		HazardBreakEvent();
	}

	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		if (this->links[(Directions)i].compare("0") != 0)
		{
			this->link_zone[i].speed = this->bg->speed;
			this->link_zone[i].update(deltaTime);
		}
	}
}

void Scene::Draw(sf::RenderWindow* window)
{
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		if (this->links[(Directions)i].compare("0") != 0)
		{
			this->link_zone[i].Draw(window);
		}
	}
}

void Scene::GenerateEnemies(Time deltaTime)
{
	static double timer = 0;
	timer += deltaTime.asSeconds();
	if (timer > 5)
	{
		double intpart;
		timer = modf(timer, &intpart);

		double random_number = ((double)rand() / (RAND_MAX));

		// A PASSER EN .CSV :
		int nb_rows = 1;
		int nb_lines = 1;
		float xspread = 150;
		float yspread = 200;

		// liste de classes d'ennemis : alpha, alpha, alpha, alpha, alpha
		// liste de patterns associés : 0, 0, 0, 0, 0, 

		//chosing a random enemy within a class of enemies
		Enemy* random_enemy_within_class[EnemyClass::NBVAL_EnemyClass];

		//Attention si total class probability vaut 0 ça va crasher - division par zéro oblige. du coup il faut vérifier que ce n'est pas égal à 0.
		int dice_roll = (rand() % (total_class_probability[EnemyClass::ENEMYPOOL_ALPHA])) + 1;

		for (int i = 0; i < EnemyClass::NBVAL_EnemyClass; i++)
		{
			for (std::list<EnemyBase>::iterator it = enemies_ranked_by_class[i].begin(); it != enemies_ranked_by_class[i].end(); ++it)
			{
				if (dice_roll >= it->proba_min && dice_roll <= it->proba_max)
				{
					random_enemy_within_class[i] = (*it).enemy;
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

			enemies_ranked_by_class[EnemyClass::ENEMYPOOL_ALPHA].begin()->poolsize--;

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
	}
	m_hazardbreak_has_occurred = true;
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