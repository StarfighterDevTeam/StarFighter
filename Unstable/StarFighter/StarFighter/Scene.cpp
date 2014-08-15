#include "Scene.h"

extern Game* CurrentGame;


void Scene::LoadSceneFromFile(string name, bool reverse_scene, bool first_scene)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Loading scene '%s'", (char*)name.c_str()));
	this->hazard_break_value = 0;
	this->generating_enemies = false;

	for (int i = 0; i < EnemyClass::NBVAL_EnemyClass; i++)
	{
		this->total_class_probability[i] = 0;
	}
	try {
		list<vector<string>> scenesConfig = *(FileLoaderUtils::FileLoader(SCENES_FILE));

		for (std::list<vector<string>>::iterator it = (scenesConfig).begin(); it != (scenesConfig).end(); it++)
		{
			if ((*it)[ScenesData::SCENE_NAME].compare(name) == 0)
			{
				this->links[Directions::DIRECTION_UP] = (*it)[ScenesData::SCENE_LINK_UP];
				this->links[Directions::DIRECTION_DOWN] = (*it)[ScenesData::SCENE_LINK_DOWN];
				this->links[Directions::DIRECTION_RIGHT] = (*it)[ScenesData::SCENE_LINK_RIGHT];
				this->links[Directions::DIRECTION_LEFT] = (*it)[ScenesData::SCENE_LINK_LEFT];

				list<vector<string>> config = *(FileLoaderUtils::FileLoader((*it)[ScenesData::SCENE_FILENAME]));

				int p = 0;
				//enemies
				for (std::list<vector<string>>::iterator it = (config).begin(); it != (config).end(); it++)
				{
					if ((*it)[0].compare("bg") == 0)
					{
						this->direction = Directions::NO_DIRECTION;
						bool hub = false;
						
						this->vspeed = stoi((*it)[SceneDataBackground::BACKGROUND_VSPEED]);
						float w = stoi((*it)[SceneDataBackground::BACGKROUND_WIDTH]);
						float h = stoi((*it)[SceneDataBackground::BACKGROUND_HEIGHT]);

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

						sf::Vector2f pos = Independant::getCoordinates_for_Spawn(first_scene, this->direction, sf::Vector2f(w / 2, h / 2), true, true);
						sf::Vector2f speed = Independant::getSpeed_for_Scrolling(this->direction, this->vspeed);

						if (hub)
						{
							this->direction = Directions::NO_DIRECTION;
						}
						this->bg = new Independant(pos, speed, (*it)[SceneDataBackground::BACKGROUND_NAME], sf::Vector2f(w, h));
						this->bg->display_name = (*it)[SceneDataBackground::BACKGROUND_DISPLAYNAME];
						this->bg->setVisible(true);
						this->bg->isOnScene = true;
					}

					int enemy_count = 0;
					if ((*it)[0].compare("enemy") == 0)
					{
						EnemyBase* e = FileLoader::LoadEnemy((*it)[SceneDataEnemy::ENEMY], stoi((*it)[SceneDataEnemy::ENEMY_PROBABILITY].c_str()), stoi((*it)[SceneDataEnemy::ENEMY_POOLSIZE]), stoi((*it)[SceneDataEnemy::ENEMY_CLASS]));
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
						hazard_break_value += e->enemy->getMoney() * e->poolsize * HAZARD_BREAK_RATIO;
					}
					//printf("Hazard Break to reach: %d\n", hazard_break_value);
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

Scene::Scene(string name, bool reverse_scene, bool first_scene)
{
	hazard_level = 0;
	LoadSceneFromFile(name, reverse_scene, first_scene);
}

void Scene::Update(Time deltaTime)
{
	if (this->generating_enemies)
	{
		this->GenerateEnemies(deltaTime);
	}

	if ((*CurrentGame).getHazard() > hazard_break_value - 1)//hazard break event
	{
		HazardBreakEvent();
	}
}

void Scene::GenerateEnemies(Time deltaTime)
{
	static double timer = 0;
	timer += deltaTime.asSeconds();
	if (timer > 2)
	{
		double intpart;
		timer = modf(timer, &intpart);

		double random_number = ((double)rand() / (RAND_MAX));

		// A PASSER EN .CSV :
		int nb_rows = 1;
		int nb_lines = 1;
		float xspread = 200;
		float yspread = 50;
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
		vector<EnemyPoolElement*>* cluster = new vector<EnemyPoolElement*>;
		for (int i = 0; i < nb_rows*nb_lines; i++)
		{
			//VALEURS A CONF EN .CSV
			// arg0 = enemy class
			// arg1 = move pattern
			//if arg0 != VOID
			EnemyPoolElement* e = new EnemyPoolElement(random_enemy_within_class[EnemyClass::ENEMYPOOL_ALPHA], EnemyClass::ENEMYPOOL_ALPHA, PatternType::NoMovePattern);

			enemies_ranked_by_class[EnemyClass::ENEMYPOOL_ALPHA].begin()->poolsize--;

			//test de loot d'ennemi hardcodé
			//Equipment* loot = LoadEquipment("module_gerard");
			//((Independant*)e->enemy)->setEquipmentLoot(loot);

			//test de loot d'ennemi hardcodé
			//Weapon* loot = LoadWeapon("laser_player", -1, LoadAmmo("laser_blue"));
			//((Independant*)e->enemy)->setWeaponLoot(loot);

			cluster->push_back(e);
		}

		int size_x = (int)((nb_rows + 1) * xspread);//let's take some margin : +1 row
		float size_y = (nb_lines + 1) * yspread;//let's take some margin : +1 line
		if (size_x >= SCENE_SIZE_X) size_x = SCENE_SIZE_X - 1;//without this, rand() % SCENE_SIZE_X - size_x may crash if size_x = SCENE_SIZE_X
		if (size_y >= SCENE_SIZE_Y) size_y = SCENE_SIZE_Y - 1;

		EnemyPool* generated_cluster = new EnemyPool(sf::Vector2f(rand() % (SCENE_SIZE_X - size_x), -size_y), nb_lines, nb_rows, xspread, yspread, cluster);

		//sf::Vector2f pos = Independant::getCoordinates_for_Spawn(false, (*CurrentGame).direction, sf::Vector2f(xspread, yspread), true, false, false, sf::Vector2f(0, 0), true, sf::Vector2f(xspread, yspread));
		//EnemyPool* generated_cluster = new EnemyPool(pos, nb_lines, nb_rows, xspread, yspread, cluster);

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
	(*CurrentGame).resetHazard((*CurrentGame).getHazard() - hazard_break_value);
	printf("DEBUG: HAZARD BREAK!!!\n");
	hazard_break_value *= (1 + HAZARD_BREAK_MULTIPLIER);
	if (hazard_level < HazardLevels::NB_HAZARD_LEVELS - 1)
		hazard_level++;
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
/*
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
			x = (SCENE_SIZE_Y - position.y) * SCENE_SIZE_X / SCENE_SIZE_Y;
			y = position.x * SCENE_SIZE_Y / SCENE_SIZE_X;
		}
		if (scrolling_direction.x == -1)
		{
			x = SCENE_SIZE_X - ((SCENE_SIZE_Y - position.y) * SCENE_SIZE_X / SCENE_SIZE_Y);
			y = position.x * SCENE_SIZE_Y / SCENE_SIZE_X;
		}
	}

	return sf::Vector2f(x, y);
}

sf::Vector2f Scene::ApplyScrollingDirectionOnSpeed(float vspeed)
{
	float x = 0;
	float y = vspeed;

	if (scrolling_direction.y == -1)
	{
		x = 0;
		y *= (-1);
	}


	if (scrolling_direction.y == 0)
	{
		if (scrolling_direction.x == 1)
		{
			x = -vspeed;
			y = 0;
		}
		if (scrolling_direction.x == -1)
		{
			x = vspeed;
			y = 0;
		}
	}

	return sf::Vector2f(x, y);
}
*/

int Scene::getSceneHazardBreakValue()
{
	return this->hazard_break_value;
}

int Scene::getSceneHazardLevelValue()
{
	return this->hazard_level;
}