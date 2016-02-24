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
	m_hubScreen.create(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y, false);
	m_hubScreen.setSmooth(true);

	m_sceneChronometer.restart();
	m_scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	m_screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	resetHazard();;//initalisation of the scoring system
	m_BeastScoreBonus = 0;
	m_direction = Directions::NO_DIRECTION;
	m_hyperspeedMultiplier = 1.0f;

	m_hud.Init(0, 0, 0, 1);
	m_interactionPanel = new InteractionPanel();
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

void Game::addToFeedbacks(Text* text)
{
	m_sceneFeedbackTexts.push_back(text);
}

void Game::removeFromFeedbacks(RectangleShape* feedback)
{
	m_sceneFeedbackBars.remove(feedback);
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

	//Checking colisions
	colisionChecksV2();

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

	//Collect the dust
	collectGarbage();
}

void Game::updateHud(int armor, int armor_max, int shield, int shield_max, int money, int graze_count, int hazard_level, std::string scene_name, int level, int level_max, int xp, int xp_max, sf::Time deltaTime, bool hub,
	int focused_item_type, string f_name, int f_level, int f_xp, float f_max_speed, float f_hyperspeed, int f_armor, int f_shield, int f_shield_regen,
	int f_damage, bool f_bot, float f_ammo_speed, PatternType f_pattern,
	int f_multishot, int f_xspread, float f_rate_of_fire, ShotMode f_shot_mode, float f_dispersion, int f_rafale, float f_rafale_cooldown, TargetSeaking f_target_seaking)
{
	m_hud.Update(armor, armor_max, shield, shield_max, money, graze_count, hazard_level, scene_name, level, level_max, xp, xp_max, deltaTime, hub, focused_item_type, f_name, f_level, f_xp, f_max_speed, f_hyperspeed,
		f_armor, f_shield, f_shield_regen, f_damage, f_bot, f_ammo_speed, f_pattern, f_multishot, f_xspread, f_rate_of_fire, f_shot_mode, f_dispersion, f_rafale, f_rafale_cooldown, f_target_seaking);
}

GameObject* Game::UpdateInteractionPanel(InteractionType interaction, int max_unlocked_hazard_level, sf::Time deltaTime)
{
	return this->m_interactionPanel->Update(interaction, max_unlocked_hazard_level, deltaTime);
	
}

void Game::SetShopMenu(ShopMenus menu)
{
	this->m_interactionPanel->m_currentShopMenu = menu;
}

ShopMenus Game::GetShopMenu()
{
	return this->m_interactionPanel->m_currentShopMenu;
}

void Game::SetSelectedIndex(int index_)
{
	this->m_interactionPanel->m_selected_index = index_;
}

void Game::SetSelectedDirection(Directions direction_)
{
	this->m_interactionPanel->m_direction = direction_;
}

void Game::SetSelectedDestination(string destination_)
{
	this->m_interactionPanel->m_textDestination.setString(destination_);
}

int Game::GetSelectedIndex()
{
	return this->m_interactionPanel->m_selected_index;
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
		else if (i == PanelLayer && m_direction == NO_DIRECTION)
		{
			this->m_interactionPanel->Draw(m_mainScreen);
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
	float black_stripe = (REF_WINDOW_RESOLUTION_X - (SCENE_SIZE_X * 4.0f / 3)) / 2;
	temp.setPosition(sf::Vector2f(m_scale_factor.x * black_stripe, 0));
	m_window->draw(temp);
}

void Game::drawHud()
{
	m_hud.Draw(m_hubScreen);
	m_hubScreen.display();
	sf::Sprite temp(m_hubScreen.getTexture());
	temp.scale(m_scale_factor.x, m_scale_factor.y);
	float black_stripe = (REF_WINDOW_RESOLUTION_X - (SCENE_SIZE_X * 4.0f / 3)) / 2;
	temp.setPosition(sf::Vector2f(m_scale_factor.x * (black_stripe + SCENE_SIZE_X), 0));
	m_window->draw(temp);

	//adding black stripes on the left and right
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
}

GameObject* Game::getHudFocusedItem()
{
	return m_hud.focused_item;
}

sf::Vector2i Game::getHudFocusedGridAndIndex()
{
	return m_hud.focused_grid_and_index;
}

sf::Vector2i Game::getHudFocusedIndexWithinGrid(HudGridsIndex grid_)
{
	switch (grid_)
	{
	case HudGrid_ShipGrid:
		return m_hud.fakeShipGrid.focus;

	case HudGrid_EquipmentGrid:
		return m_hud.fakeEquipmentGrid.focus;
	}

	return sf::Vector2i(-1, -1);
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

bool Game::InsertObjectInGrid(ObjectGrid& grid, GameObject& object, int index)
{
	bool result = grid.insertObject(object, index);

	return result;
}

bool Game::InsertObjectInShipGrid(GameObject& object, int index)
{
	bool result = m_hud.shipGrid.insertObject(object, index);

	return result;
}

bool Game::InsertObjectInEquipmentGrid(GameObject& object, int index)
{
	bool result = m_hud.equipmentGrid.insertObject(object, index);

	return result;
}


bool Game::SwapObjectBetweenGrids(ObjectGrid& grid, ObjectGrid& grid2, int index1, int index2)
{
	if (grid.getCellPointerFromIntIndex(index1) != NULL)
	{
		GameObject* tmpObj1 = grid.getCellPointerFromIntIndex(index1);
		//Equipement > Ship
		grid.setCellPointerForIntIndex(index1, grid2.getCellPointerFromIntIndex(index2));
		//Ship > equipement
		grid2.setCellPointerForIntIndex(index2, tmpObj1);
		tmpObj1 = NULL;
	}
	else
	{
		//Equipement > Ship
		grid.setCellPointerForIntIndex(index1, grid2.getCellPointerFromIntIndex(index2));
		//Equipment = NULL
		grid2.setCellPointerForIntIndex(index2, NULL);
	}

	return true;
}

bool Game::SwapEquipObjectInShipGrid(int index_ship, int index_equipment)
{
	if (m_hud.shipGrid.getCellPointerFromIntIndex(index_ship) != NULL)
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Swapping ship #'%d' to eq. # %d", index_ship + 1, index_equipment + 1));
		GameObject* tmpShip = m_hud.shipGrid.getCellPointerFromIntIndex(index_ship);
		//Equipement > Ship
		m_hud.shipGrid.setCellPointerForIntIndex(index_ship, m_hud.equipmentGrid.getCellPointerFromIntIndex(index_equipment));
		//Ship > equipement
		m_hud.equipmentGrid.setCellPointerForIntIndex(index_equipment, tmpShip);
		tmpShip = NULL;
	}
	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Equiping ship #'%d'", index_ship + 1));
		//Equipement > Ship
		m_hud.shipGrid.setCellPointerForIntIndex(index_ship, m_hud.equipmentGrid.getCellPointerFromIntIndex(index_equipment));
		//Equipment = NULL
		m_hud.equipmentGrid.setCellPointerForIntIndex(index_equipment, NULL);
	}

	return true;
}

void Game::GarbageObjectInGrid(int grid_id, int index)
{
	m_hud.GarbageObjectInGrid(grid_id, index);
}

void Game::setRemovingCursorAnimation(CursorFeedbackStates animation_index)
{
	m_hud.setRemovingCursorAnimation(animation_index);
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