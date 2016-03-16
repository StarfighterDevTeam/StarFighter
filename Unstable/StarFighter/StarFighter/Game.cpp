#include "Game.h"
#define stringify(x)  #x

const char* GameObjectTypeValues[] =
{
	stringify(BackgroundObject),
	stringify(PlayerShip),
	stringify(FriendlyFire),
	stringify(EnemyFire),
	stringify(EnemyObject)
};

void Game::init(RenderWindow* window)
{
	m_window = window;
	m_mainScreen.create(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y, false);
	m_mainScreen.setSmooth(true);

	m_sceneChronometer.restart();
	m_scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	m_screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	resetHazard();;//initalisation of the scoring system
	m_BeastScoreBonus = 0;
	m_direction = Directions::NO_DIRECTION;
	m_hyperspeedMultiplier = 1.0f;

	playerShip = NULL;
	m_currentScene = NULL;
	m_waiting_for_dialog_validation = false;

	try
	{
		m_font[Font_Arial] = new sf::Font();
		if (!m_font[Font_Arial]->loadFromFile("Assets/Fonts/arial.ttf"))
		{
			
		}
		m_font[Font_Terminator] = new sf::Font();
		if (!m_font[Font_Terminator]->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
		{

		}
	}
	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR, ex.what());
	}
}

sf::RenderWindow* Game::getMainWindow()
{
	return m_window;
}

void Game::SetPlayerShip(Ship* m_playerShip)
{
	this->playerShip = m_playerShip;
}

void Game::addToScene(GameObject *object, LayerType m_layer, GameObjectType type)
{
	object->m_layer = m_layer;
	object->m_collider_type = type;

	//Window resolution adjustements
	//object->setScale(scale_factor.x, scale_factor.y);

	if (((int)m_layer >= 0 && (int)m_layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_GameObject))
	{
		AddGameObjectToVector(object, &m_sceneGameObjectsTyped[(int)type]);
		AddGameObjectToVector(object, &m_sceneGameObjectsLayered[(int)m_layer]);
		AddGameObjectToVector(object, &m_sceneGameObjects);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add GameObject '%s' to layer '%d'", object->getName().c_str(), (int)m_layer));
	}
}

void Game::addToFeedbacks(RectangleShape* feedback)
{
	m_sceneFeedbackBars.push_back(feedback);
}

void Game::addToFeedbacks(SFPanel* panel)
{
	m_sceneSFPanels.push_back(panel);
}

void Game::addToFeedbacks(Text* text)
{
	m_sceneFeedbackTexts.push_back(text);
}

void Game::removeFromFeedbacks(RectangleShape* feedback)
{
	m_sceneFeedbackBars.remove(feedback);
}

void Game::removeFromFeedbacks(SFPanel* panel)
{
	m_sceneSFPanels.remove(panel);
}

void Game::removeFromFeedbacks(Text* text)
{
	m_sceneFeedbackTexts.remove(text);
}

void Game::updateScene(Time deltaTime)
{
	//printf("OnScene: %d / Collected: %d\n", this->sceneGameObjects.size(), this->garbage.size());

	//TODO: Updating screen resolution
	m_scale_factor.x = 1.0f * m_screen_size.x / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * m_screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Clean garbage
	cleanGarbage();

	size_t sceneGameObjectsSize = m_sceneGameObjects.size();

	for (int i = 0; i < sceneGameObjectsSize; i++)
	{
		if (m_sceneGameObjects[i] == NULL)
			continue;

		m_sceneGameObjects[i]->update(deltaTime, m_hyperspeedMultiplier);
	}

	for (int i = 0; i < sceneGameObjectsSize; i++)
	{
		if (m_sceneGameObjects[i] == NULL)
			continue;

		m_sceneGameObjects[i]->updatePostCollision();
	}

	//Checking colisions
	colisionChecksV2();

	//Collect the dust
	collectGarbage();
}

void Game::killGameObjectLayer(GameObjectType m_layer)
{
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[m_layer].begin(); it != m_sceneGameObjectsTyped[m_layer].end(); it++)
	{
		if ((*it) != NULL)
		{
			if ((*it)->m_isOnScene)
			{
				(*it)->Death();
			}
		}
	}
}

void Game::drawScene()
{
	m_mainScreen.clear();

	//adding black stripes on the left and right
	float black_stripe = (REF_WINDOW_RESOLUTION_X - (SCENE_SIZE_X * 4.0f / 3)) / 2;
	sf::RectangleShape blackStripeLeft, blackStripeRight;
	blackStripeLeft.setSize(sf::Vector2f(m_scale_factor.x * black_stripe, m_scale_factor.y * REF_WINDOW_RESOLUTION_Y));
	blackStripeRight.setSize(sf::Vector2f(m_scale_factor.x * black_stripe, m_scale_factor.y * REF_WINDOW_RESOLUTION_Y));
	blackStripeLeft.setFillColor(sf::Color(0, 0, 0, 255));
	blackStripeRight.setFillColor(sf::Color(0, 0, 0, 255));
	blackStripeLeft.setOrigin(0, 0);
	blackStripeRight.setOrigin(0, 0);
	blackStripeLeft.setPosition(0, 0);
	blackStripeRight.setPosition(sf::Vector2f(m_scale_factor.x * (REF_WINDOW_RESOLUTION_X - black_stripe), 0));
	m_window->draw(blackStripeLeft);
	m_window->draw(blackStripeRight);

	//drawing stuff
	for (int i = 0; i < NBVAL_Layer; i++)
	{
		if (i == FeedbacksLayer)
		{
			for (std::list<RectangleShape*>::iterator it = m_sceneFeedbackBars.begin(); it != m_sceneFeedbackBars.end(); it++)
			{
				m_mainScreen.draw(*(*it));
			}
			for (std::list<Text*>::iterator it = m_sceneFeedbackTexts.begin(); it != m_sceneFeedbackTexts.end(); it++)
			{
				m_mainScreen.draw(*(*it));
			}
		}
		else if (i == PanelLayer)
		{
			for (std::list<SFPanel*>::iterator it = m_sceneSFPanels.begin(); it != m_sceneSFPanels.end(); it++)
			{
				(*(*it)).Draw(m_mainScreen);
			}
		}
		else
		{
			for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsLayered[i].begin(); it != m_sceneGameObjectsLayered[i].end(); it++)
			{
				if (*it == NULL)
					continue;

				if ((*(*it)).m_visible)
				{
					if (!(*(*it)).m_transparent)
					{
						m_mainScreen.draw((*(*it)));
					}
				}
			}
		}
	}

	m_mainScreen.display();
	sf::Sprite temp(m_mainScreen.getTexture());
	temp.scale(m_scale_factor.x, m_scale_factor.y);
	
	temp.setPosition(sf::Vector2f(m_scale_factor.x * black_stripe, 0));
	m_window->draw(temp);

	
}

void Game::colisionChecksV2()
{
	sf::Clock dt;
	dt.restart();

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[GameObjectType::PlayerShip].begin(); it1 != m_sceneGameObjectsTyped[GameObjectType::PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Enemy bullets hitting the player
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[GameObjectType::EnemyFire].begin(); it2 != m_sceneGameObjectsTyped[GameObjectType::EnemyFire].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::IsGrazing((*it1), (*it2)))
			{
				(*it1)->GetGrazing();

				if (SimpleCollision::AreColliding((*it1), (*it2)))
				{
					//Do something (like, kill ship) -> OK
					(*it1)->damage_from(*(*it2));
					//explosion
					(*it2)->Death();

					//FX* explosion = new FX (sf::Vector2f((*it2)->getPosition().x, (*it2)->getPosition().y),sf::Vector2f(0,0), FX_EXPLOSION_FILENAME, sf::Vector2f(FX_EXPLOSION_WIDTH, FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(FX_MEDIUM_EXPLOSION_DURATION));
					//this->addToScene(explosion, LayerType::ExplosionLayer, GameObjectType::Neutral);
					//hide destroyed item
					if ((*it1)->m_armor <= 0)
					{
						(*it1)->Death();
						//we all deserve another chance...
						//(*it1)->Respawn();
					}
				}
			}
		}

		//Enemy objects
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[GameObjectType::EnemyObject].begin(); it2 != m_sceneGameObjectsTyped[GameObjectType::EnemyObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something (like, kill ship)
				(*it1)->damage_from(*(*it2));
				//explosion
				//TO DO : explosion impact enemy vs ship

				//death of player?
				if ((*it1)->m_armor <= 0)
				{
					(*it1)->Death();
					//we all deserve another chance...
					//(*it1)->Respawn();
				}

				//player may also deal contact damage to enemy ships
				(*it2)->damage_from(*(*it1));

				//TODO: display contact feedback (small explosion?)

				//death of enemy ship?
				if ((*it2)->m_armor <= 0)
				{
					(*it2)->Death();
					
				}
			}
		}

		//Loot
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[GameObjectType::LootObject].begin(); it2 != m_sceneGameObjectsTyped[GameObjectType::LootObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something (like, take the loot)
				if ((*it1)->GetLoot((*(*it2))))
				{
					(*it2)->m_visible = false;
					(*it2)->m_GarbageMe = true;
				}
			}
		}

		//Portal
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[GameObjectType::PortalObject].begin(); it2 != m_sceneGameObjectsTyped[GameObjectType::PortalObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				(*it1)->GetPortal((*it2));
			}
		}

		//Shop
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[GameObjectType::ShopObject].begin(); it2 != m_sceneGameObjectsTyped[GameObjectType::ShopObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				(*it1)->GetShop((*it2));
			}
		}
	}

	//Then, check if any allied bullet collide with any enemy
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[GameObjectType::EnemyObject].begin(); it1 != m_sceneGameObjectsTyped[GameObjectType::EnemyObject].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Player bullets on enemy
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[GameObjectType::FriendlyFire].begin(); it2 != m_sceneGameObjectsTyped[GameObjectType::FriendlyFire].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			//Bullets are invisible after impact
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something (like, kill the enemy ship ?)
				(*it1)->damage_from(*(*it2));
				//explosion
				(*it2)->Death();

				//death
				if ((*it1)->m_armor <= 0)
				{
					(*it1)->Death();
				}
			}
		}
	}

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[GameObjectType::FakePlayerShip].begin(); it1 != m_sceneGameObjectsTyped[GameObjectType::FakePlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[GameObjectType::LootObject].begin(); it2 != m_sceneGameObjectsTyped[GameObjectType::LootObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something (like, take the loot)
				if ((*it1)->GetLoot((*(*it2))))
				{
					(*it2)->m_visible = false;
					(*it2)->m_isOnScene = false;
					(*it2)->m_GarbageMe = true;
				}
			}
		}
	}
	//printf("| Collision: %d \n",dt.getElapsedTime().asMilliseconds());
}

void Game::cleanGarbage()
{
	sf::Clock dt;
	dt.restart();

	// On "cache" les size, pour éviter d'appeler des fonctions à chaque itération
	const size_t garbageSize = m_garbage.size();
	const size_t sceneGameObjectsSize = m_sceneGameObjects.size();
	//Size layer
	size_t sceneGameObjectsLayeredSize[NBVAL_Layer];
	for (int layer = 0; layer < NBVAL_Layer; layer++)
	{
		sceneGameObjectsLayeredSize[layer] = m_sceneGameObjectsLayered[layer].size();
	}
	//Size ind type
	size_t m_sceneGameObjectsTypedSize[NBVAL_GameObject];
	for (int layer = 0; layer < NBVAL_GameObject; layer++)
	{
		m_sceneGameObjectsTypedSize[layer] = m_sceneGameObjectsTyped[layer].size();
	}

	//Scene objects
	for (size_t i = 0; i < garbageSize; i++)
	{
		GameObject*    pCurGameObject = m_garbage[i];

		// On remet à NULL lorsqu'on a trouvé un élément à dégager
		for (size_t j = 0; j < sceneGameObjectsSize; j++)
		{
			if (m_sceneGameObjects[j] == pCurGameObject)
			{
				m_sceneGameObjects[j] = NULL;
				break;
			}
		}

		// "layered"...
		const int layer = pCurGameObject->m_layer;
		for (size_t j = 0; j < sceneGameObjectsLayeredSize[layer]; j++)
		{
			if (m_sceneGameObjectsLayered[layer][j] == pCurGameObject)
			{
				m_sceneGameObjectsLayered[layer][j] = NULL;
				break;
			}
		}

		// "typed"
		const int type = pCurGameObject->m_collider_type;
		for (size_t j = 0; j < m_sceneGameObjectsTypedSize[type]; j++)
		{
			if (m_sceneGameObjectsTyped[type][j] == pCurGameObject)
			{
				m_sceneGameObjectsTyped[type][j] = NULL;
				break;
			}
		}

		pCurGameObject->Destroy();//destructor function

		// A la fin, on delete l'élément
		delete pCurGameObject;
	}

	//printf("| Clean: %d ",dt.getElapsedTime().asMilliseconds());
}

void Game::AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] == NULL)
		{
			(*vector)[i] = pGameObject;
			return; // ayé, on a trouvé un free slot, inséré, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas où on n'a pas trouvé de free slot => on rajoute à la fin
	vector->push_back(pGameObject);
}

void Game::collectGarbage()
{
	sf::Clock dt;
	dt.restart();

	m_garbage.clear();

	for (std::vector<GameObject*>::iterator it = (m_sceneGameObjects).begin(); it != (m_sceneGameObjects).end(); it++)
	{
		if (*it == NULL)
			continue;

		//Content flagged for deletion
		if ((**it).m_GarbageMe)
		{
			m_garbage.push_back(*it);
			continue;
		}

		if (!(**it).m_isOnScene)
		{
			//objects that are spawning out of screen are not deleted
			if (((**it).getPosition().x + ((**it).m_size.x) / 2 >= 0 && (**it).getPosition().x - ((**it).m_size.x) / 2 <= SCENE_SIZE_X) && ((**it).getPosition().y + ((**it).m_size.y) / 2 >= 0 && (**it).getPosition().y - ((**it).m_size.y) / 2 <= SCENE_SIZE_Y))
			{
				(**it).m_isOnScene = true;
			}
		}

		//Content that went on scene and then exited have to be deleted
		if (!(**it).m_DontGarbageMe && (**it).m_isOnScene)
		{
			if ((**it).getPosition().x + ((**it).m_size.x) / 2 < 0 || (**it).getPosition().x - ((**it).m_size.x) / 2 > SCENE_SIZE_X
				|| (**it).getPosition().y + ((**it).m_size.y) / 2 < 0 || (**it).getPosition().y - ((**it).m_size.y) / 2 > SCENE_SIZE_Y)
			{
				m_garbage.push_back(*it);
				continue;
			}
		}
	}

	//printf("| Collect: %d ",dt.getElapsedTime().asMilliseconds());

}

void Game::garbageLayer(LayerType m_layer, bool only_offscene)
{
	int clear_count = 0;
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsLayered[m_layer].begin(); it != m_sceneGameObjectsLayered[m_layer].end(); it++)
	{
		if (*it == NULL)
			continue;

		if (only_offscene)
		{
			if (!(*it)->m_isOnScene)
			{
				(*it)->m_GarbageMe = true;
				clear_count++;
				//don't count them as "spawned" enemies if we cut them off this way
				if (m_layer == EnemyObjectLayer)
				{
					m_hazardSpawned -= (*it)->m_money;
				}
			}
		}
		else
		{
			(*it)->m_visible = false;
			(*it)->m_isOnScene = false;
			(*it)->m_GarbageMe = true;
			//don't count them as "spawned" enemies if we cut them off this way
			if (m_layer == EnemyObjectLayer)
			{
				m_hazardSpawned -= (*it)->m_money;
			}
		}
	}
}

void Game::SetLayerRotation(LayerType m_layer, float angle)
{
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsLayered[m_layer].begin(); it != m_sceneGameObjectsLayered[m_layer].end(); it++)
	{
		if (*it == NULL)
			continue;

		(*it)->setRotation(angle);
	}
}

bool Game::isVectorEmpty(vector <GameObject*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] != NULL)
		{
			return false;
		}
	}

	return true;
}

bool Game::isLastEnemyDead()
{
	if (!isVectorEmpty(&m_sceneGameObjectsTyped[EnemyFire]))
		return false;
	else if (!isVectorEmpty(&m_sceneGameObjectsTyped[EnemyObject]))
		return false;
	
	return true;
}

int Game::getHazard()
{
	return m_hazard;
}

void Game::resetHazard(int hazard_overkill)
{
	m_hazard = hazard_overkill;
	m_hazardSpawned = 0;
}

void Game::GetBeastScoreBonus(float m_playerShipBeastScore, float m_sceneBeastScore)
{
	m_BeastScoreBonus = m_playerShipBeastScore + m_sceneBeastScore;
}

TargetScan Game::FoundNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range)
{
	sf::Vector2f pos;
	float shortest_distance = -1;
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[type].begin(); it != m_sceneGameObjectsTyped[type].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->m_isOnScene && !(*it)->m_ghost)
		{
			const float a = ref_position.x - (*it)->getPosition().x;
			const float b = ref_position.y - (*it)->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				pos = (*it)->getPosition();
			}
		}
	}
	if (shortest_distance > 0)
	{
		if (range > 0 && (range * range) <= shortest_distance)
		{
			return TargetScan::TARGET_OUT_OF_RANGE;
		}
		else
		{
			return TargetScan::TARGET_IN_RANGE;
		}

	}
	else
	{
		return TargetScan::NO_TARGET_FOUND;
	}
}

float Game::GetAngleToNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range)
{
	float angle = 0.f;
	sf::Vector2f pos;
	float shortest_distance = -1;
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[type].begin(); it != m_sceneGameObjectsTyped[type].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->m_isOnScene && !(*it)->m_ghost)
		{
			const float a = ref_position.x - (*it)->getPosition().x;
			const float b = ref_position.y - (*it)->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				pos = (*it)->getPosition();
			}
		}
	}
	if (shortest_distance > 0)
	{
		if (range > 0 && (range * range) < shortest_distance)//shortest_distance is already squared
		{
			//do nothing: range is too short to reach the target, therefore we don't aim for it.
		}
		else
		{
			shortest_distance = sqrtf(shortest_distance);
			//angle = acos((ref_position.y - pos.y) / shortest_distance);
			sf::Vector2f diff_position = GameObject::getSize_for_Direction(m_direction, (sf::Vector2f((ref_position.y - pos.y), (ref_position.x - pos.x))));
			diff_position.x *= GameObject::getDirectionMultiplier(m_direction).x;
			angle = acos(diff_position.x / shortest_distance);
			angle = angle * 180 / M_PI;

			diff_position.y *= GameObject::getDirectionMultiplier(m_direction).y;

			//if (ref_position.x < pos.x)
			if (diff_position.y < 0)
			{
				angle = -angle;
			}
		}
	}
	//if no target found, the default value for enemies is 180 degrees (firing straight ahead)
	else if (type == PlayerShip)
	{
		return 180;
	}

	return angle;
}

void Game::WakeUpEnemiesWithName(string m_display_name)
{
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[GameObjectType::EnemyObject].begin(); it != m_sceneGameObjectsTyped[GameObjectType::EnemyObject].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->m_display_name == m_display_name)
		{
			(*it)->m_wake_up = true;
		}
	}
}

int Game::GetPlayerStatsMultiplierForLevel(int level_)
{
	int result_ = round(100 * (pow((1 + PLAYER_LEVELING_STATS_MULTIPLIER), (level_ - 1))));
	return result_;
}

int Game::GetEnemiesStatsMultiplierForLevel(int level_)
{
	int result_ = round(100 * (pow((1 + ENEMIES_LEVELING_STATS_MULTIPLIER), (level_ - 1))));
	return result_;
}

int Game::GetBonusStatsMultiplierToBeOnParForLevel(int level_)
{
	int result_ = GetEnemiesStatsMultiplierForLevel(level_) - GetPlayerStatsMultiplierForLevel(level_);
	return result_;
}