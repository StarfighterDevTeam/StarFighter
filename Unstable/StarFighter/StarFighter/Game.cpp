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
	this->window = window;
	this->mainScreen.create(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y, false);
	this->mainScreen.setSmooth(true);
	this->hubScreen.create(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y, false);
	this->hubScreen.setSmooth(true);

	this->sceneChronometer.restart();
	scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	this->resetHazard();;//initalisation of the scoring system
	this->BeastScoreBonus = 0;
	this->direction = Directions::NO_DIRECTION;
	this->hyperspeedMultiplier = 1.0f;

	this->hud.Init(0, 0, 0, 1);
	this->m_interactionPanel = new InteractionPanel();
}

sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}

void Game::SetPlayerShip(Ship* m_playerShip)
{
	this->playerShip = m_playerShip;
}

void Game::addToScene(GameObject *object, LayerType m_layer, GameObjectType type)
{
	object->layer = m_layer;
	object->collider_type = type;

	//Window resolution adjustements
	//object->setScale(scale_factor.x, scale_factor.y);

	if (((int)m_layer >= 0 && (int)m_layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_GameObject))
	{
		AddGameObjectToVector(object, &this->sceneGameObjectsTyped[(int)type]);
		AddGameObjectToVector(object, &this->sceneGameObjectsLayered[(int)m_layer]);
		AddGameObjectToVector(object, &this->sceneGameObjects);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add GameObject '%s' to layer '%d'", object->getName().c_str(), (int)m_layer));
	}
}

void Game::addToFeedbacks(RectangleShape* feedback)
{
	sceneFeedbackBars.push_back(feedback);
}

void Game::addToFeedbacks(Text* text)
{
	sceneFeedbackTexts.push_back(text);
}

void Game::removeFromFeedbacks(RectangleShape* feedback)
{
	sceneFeedbackBars.remove(feedback);
}

void Game::removeFromFeedbacks(Text* text)
{
	sceneFeedbackTexts.remove(text);
}

void Game::updateScene(Time deltaTime)
{
	//printf("OnScene: %d / Collected: %d\n", this->sceneGameObjects.size(), this->garbage.size());

	//TODO: Updating screen resolution
	scale_factor.x = 1.0f * screen_size.x / REF_WINDOW_RESOLUTION_X;
	scale_factor.y = 1.0f * screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Clean garbage
	cleanGarbage();

	//Checking colisions
	colisionChecksV2();

	size_t sceneGameObjectsSize = this->sceneGameObjects.size();

	for (int i = 0; i < sceneGameObjectsSize; i++)
	{
		if (this->sceneGameObjects[i] == NULL)
			continue;

		this->sceneGameObjects[i]->update(deltaTime, hyperspeedMultiplier);
	}

	for (int i = 0; i < sceneGameObjectsSize; i++)
	{
		if (this->sceneGameObjects[i] == NULL)
			continue;

		this->sceneGameObjects[i]->updatePostCollision();
	}

	//Collect the dust
	collectGarbage();
}

void Game::updateHud(int m_armor, int m_armor_max, int m_shield, int m_shield_max, int m_money, int m_graze_count, int m_hazard_level, std::string scene_name, int level, int level_max, int xp, int xp_max, sf::Time deltaTime, bool hub,
	int focused_item_type, string f_name, int f_level, int f_xp, float f_max_speed, float f_hyperspeed, int f_armor, int f_shield, int f_shield_regen,
	int f_damage, bool f_bot, float f_ammo_speed, PatternType f_pattern,
	int f_multishot, int f_xspread, float f_rate_of_fire, ShotMode f_shot_mode, float f_dispersion, int f_rafale, float f_rafale_cooldown, TargetSeaking f_target_seaking)
{
	this->hud.Update(m_armor, m_armor_max, m_shield, m_shield_max, m_money, m_graze_count, m_hazard_level, scene_name, level, level_max, xp, xp_max, deltaTime, hub, focused_item_type, f_name, f_level, f_xp, f_max_speed, f_hyperspeed,
		f_armor, f_shield, f_shield_regen, f_damage, f_bot, f_ammo_speed, f_pattern, f_multishot, f_xspread, f_rate_of_fire, f_shot_mode, f_dispersion, f_rafale, f_rafale_cooldown, f_target_seaking);
}

void Game::UpdateInteractionPanel(InteractionType interaction, int max_unlocked_hazard_level, sf::Time deltaTime)
{
	this->m_interactionPanel->Update(interaction, max_unlocked_hazard_level, deltaTime);
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
	for (std::vector<GameObject*>::iterator it = this->sceneGameObjectsTyped[m_layer].begin(); it != this->sceneGameObjectsTyped[m_layer].end(); it++)
	{
		if ((*it) != NULL)
		{
			if ((*it)->isOnScene)
			{
				(*it)->Death();
			}
		}
	}
}

void Game::drawScene()
{
	this->mainScreen.clear();

	for (int i = 0; i < NBVAL_Layer; i++)
	{
		if (i == FeedbacksLayer)
		{
			for (std::list<RectangleShape*>::iterator it = this->sceneFeedbackBars.begin(); it != this->sceneFeedbackBars.end(); it++)
			{
				mainScreen.draw(*(*it));
			}
			for (std::list<Text*>::iterator it = this->sceneFeedbackTexts.begin(); it != this->sceneFeedbackTexts.end(); it++)
			{
				mainScreen.draw(*(*it));
			}
		}
		else if (i == PanelLayer && this->direction == NO_DIRECTION)
		{
			this->m_interactionPanel->Draw(mainScreen);
		}
		else
		{
			for (std::vector<GameObject*>::iterator it = this->sceneGameObjectsLayered[i].begin(); it != this->sceneGameObjectsLayered[i].end(); it++)
			{
				if (*it == NULL)
					continue;

				if ((*(*it)).visible)
				{
					if (!(*(*it)).transparent)
					{
						this->mainScreen.draw((*(*it)));
					}
				}
			}
		}
	}

	this->mainScreen.display();
	sf::Sprite temp(this->mainScreen.getTexture());
	temp.scale(scale_factor.x, scale_factor.y);
	float black_stripe = (REF_WINDOW_RESOLUTION_X - (SCENE_SIZE_X * 4.0f / 3)) / 2;
	temp.setPosition(sf::Vector2f(scale_factor.x * black_stripe, 0));
	this->window->draw(temp);
}

void Game::drawHud()
{
	hud.Draw(hubScreen);
	hubScreen.display();
	sf::Sprite temp(hubScreen.getTexture());
	temp.scale(scale_factor.x, scale_factor.y);
	float black_stripe = (REF_WINDOW_RESOLUTION_X - (SCENE_SIZE_X * 4.0f / 3)) / 2;
	temp.setPosition(sf::Vector2f(scale_factor.x * (black_stripe + SCENE_SIZE_X), 0));
	this->window->draw(temp);

	//adding black stripes on the left and right
	sf::RectangleShape blackStripeLeft, blackStripeRight;
	blackStripeLeft.setSize(sf::Vector2f(scale_factor.x * black_stripe, scale_factor.y * REF_WINDOW_RESOLUTION_Y));
	blackStripeRight.setSize(sf::Vector2f(scale_factor.x * black_stripe, scale_factor.y * REF_WINDOW_RESOLUTION_Y));
	blackStripeLeft.setFillColor(sf::Color(0, 0, 0, 255));
	blackStripeRight.setFillColor(sf::Color(0, 0, 0, 255));
	blackStripeLeft.setOrigin(0, 0);
	blackStripeRight.setOrigin(0, 0);
	blackStripeLeft.setPosition(0, 0);
	blackStripeRight.setPosition(sf::Vector2f(scale_factor.x * (REF_WINDOW_RESOLUTION_X - black_stripe), 0));
	this->window->draw(blackStripeLeft);
	this->window->draw(blackStripeRight);
}

GameObject* Game::getHudFocusedItem()
{
	return hud.focused_item;
}

sf::Vector2i Game::getHudFocusedGridAndIndex()
{
	return hud.focused_grid_and_index;
}

sf::Vector2i Game::getHudFocusedIndexWithinGrid(HudGridsIndex grid_)
{
	switch (grid_)
	{
	case HudGrid_ShipGrid:
		return hud.fakeShipGrid.focus;

	case HudGrid_EquipmentGrid:
		return hud.fakeEquipmentGrid.focus;
	}

	return sf::Vector2i(-1, -1);
}

void Game::colisionChecksV2()
{
	sf::Clock dt;
	dt.restart();

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[GameObjectType::PlayerShip].begin(); it1 != sceneGameObjectsTyped[GameObjectType::PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Enemy bullets hitting the player
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::EnemyFire].begin(); it2 != sceneGameObjectsTyped[GameObjectType::EnemyFire].end(); it2++)
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
					if ((*it1)->getGameObjectArmor() <= 0)
					{
						(*it1)->visible = false;
						(*it1)->Death();
						//we all deserve another chance...
						//(*it1)->Respawn();
					}
				}
			}
		}

		//Enemy objects
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::EnemyObject].begin(); it2 != sceneGameObjectsTyped[GameObjectType::EnemyObject].end(); it2++)
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
				if ((*it1)->getGameObjectArmor() <= 0)
				{
					(*it1)->visible = false;
					(*it1)->Death();
					//we all deserve another chance...
					(*it1)->Respawn();
					hazard = 0;
				}

				//player may also deal contact damage to enemy ships
				(*it2)->damage_from(*(*it1));

				//TODO: display contact feedback (small explosion?)

				//death of enemy ship?
				if ((*it2)->getGameObjectArmor() <= 0)
				{
					(*it2)->Death();
					
				}
			}
		}

		//Loot
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::LootObject].begin(); it2 != sceneGameObjectsTyped[GameObjectType::LootObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something (like, take the loot)
				if ((*it1)->GetLoot((*(*it2))))
				{
					(*it2)->visible = false;
					(*it2)->isOnScene = false;
					(*it2)->GarbageMe = true;
				}
			}
		}

		//Portal
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::PortalObject].begin(); it2 != sceneGameObjectsTyped[GameObjectType::PortalObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				(*it1)->GetPortal((*it2));
			}
		}

		//Shop
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::ShopObject].begin(); it2 != sceneGameObjectsTyped[GameObjectType::ShopObject].end(); it2++)
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
	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[GameObjectType::EnemyObject].begin(); it1 != sceneGameObjectsTyped[GameObjectType::EnemyObject].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Player bullets on enemy
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::FriendlyFire].begin(); it2 != sceneGameObjectsTyped[GameObjectType::FriendlyFire].end(); it2++)
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
				if ((*it1)->getGameObjectArmor() <= 0)
				{
					(*it1)->Death();
				}
			}
		}
	}

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[GameObjectType::FakePlayerShip].begin(); it1 != sceneGameObjectsTyped[GameObjectType::FakePlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::LootObject].begin(); it2 != sceneGameObjectsTyped[GameObjectType::LootObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something (like, take the loot)
				if ((*it1)->GetLoot((*(*it2))))
				{
					(*it2)->visible = false;
					(*it2)->isOnScene = false;
					(*it2)->GarbageMe = true;
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

	// On "cache" les size, pour �viter d'appeler des fonctions � chaque it�ration
	const size_t garbageSize = this->garbage.size();
	const size_t sceneGameObjectsSize = this->sceneGameObjects.size();
	//Size layer
	size_t sceneGameObjectsLayeredSize[NBVAL_Layer];
	for (int layer = 0; layer < NBVAL_Layer; layer++)
	{
		sceneGameObjectsLayeredSize[layer] = this->sceneGameObjectsLayered[layer].size();
	}
	//Size ind type
	size_t sceneGameObjectsTypedSize[NBVAL_GameObject];
	for (int layer = 0; layer < NBVAL_GameObject; layer++)
	{
		sceneGameObjectsTypedSize[layer] = this->sceneGameObjectsTyped[layer].size();
	}

	//Scene objects
	for (size_t i = 0; i < garbageSize; i++)
	{
		GameObject*    pCurGameObject = this->garbage[i];

		// On remet � NULL lorsqu'on a trouv� un �l�ment � d�gager
		for (size_t j = 0; j < sceneGameObjectsSize; j++)
		{
			if (this->sceneGameObjects[j] == pCurGameObject)
			{
				this->sceneGameObjects[j] = NULL;
				break;
			}
		}

		// "layered"...
		const int layer = pCurGameObject->layer;
		for (size_t j = 0; j < sceneGameObjectsLayeredSize[layer]; j++)
		{
			if (this->sceneGameObjectsLayered[layer][j] == pCurGameObject)
			{
				this->sceneGameObjectsLayered[layer][j] = NULL;
				break;
			}
		}

		// "typed"
		const int type = pCurGameObject->collider_type;
		for (size_t j = 0; j < sceneGameObjectsTypedSize[type]; j++)
		{
			if (this->sceneGameObjectsTyped[type][j] == pCurGameObject)
			{
				this->sceneGameObjectsTyped[type][j] = NULL;
				break;
			}
		}

		pCurGameObject->Destroy();//destructor function

		// A la fin, on delete l'�l�ment
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
			return; // ay�, on a trouv� un free slot, ins�r�, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas o� on n'a pas trouv� de free slot => on rajoute � la fin
	vector->push_back(pGameObject);
}

void Game::collectGarbage()
{
	sf::Clock dt;
	dt.restart();

	this->garbage.clear();

	for (std::vector<GameObject*>::iterator it = (this->sceneGameObjects).begin(); it != (this->sceneGameObjects).end(); it++)
	{
		if (*it == NULL)
			continue;

		//Content flagged for deletion
		if ((**it).GarbageMe)
		{
			this->garbage.push_back(*it);
			continue;
		}

		if (!(**it).isOnScene)
		{
			//objects that are spawning out of screen are not deleted
			if (((**it).getPosition().x + ((**it).m_size.x) / 2 >= 0 && (**it).getPosition().x - ((**it).m_size.x) / 2 <= SCENE_SIZE_X) && ((**it).getPosition().y + ((**it).m_size.y) / 2 >= 0 && (**it).getPosition().y - ((**it).m_size.y) / 2 <= SCENE_SIZE_Y))
			{
				(**it).isOnScene = true;
			}
		}

		//Content that went on scene and then exited have to be deleted
		if (!(**it).DontGarbageMe && (**it).isOnScene)
		{
			if ((**it).getPosition().x + ((**it).m_size.x) / 2 < 0 || (**it).getPosition().x - ((**it).m_size.x) / 2 > SCENE_SIZE_X
				|| (**it).getPosition().y + ((**it).m_size.y) / 2 < 0 || (**it).getPosition().y - ((**it).m_size.y) / 2 > SCENE_SIZE_Y)
			{
				this->garbage.push_back(*it);
				continue;
			}
		}
	}

	//printf("| Collect: %d ",dt.getElapsedTime().asMilliseconds());

}

void Game::garbageLayer(LayerType m_layer, bool only_offscene)
{
	int clear_count = 0;
	for (std::vector<GameObject*>::iterator it = sceneGameObjectsLayered[m_layer].begin(); it != sceneGameObjectsLayered[m_layer].end(); it++)
	{
		if (*it == NULL)
			continue;

		if (only_offscene)
		{
			if (!(*it)->isOnScene)
			{
				(*it)->GarbageMe = true;
				clear_count++;
				//don't count them as "spawned" enemies if we cut them off this way
				if (m_layer == EnemyObjectLayer)
				{
					this->hazardSpawned -= (*it)->getMoney();
				}
			}
		}
		else
		{
			(*it)->visible = false;
			(*it)->isOnScene = false;
			(*it)->GarbageMe = true;
			//don't count them as "spawned" enemies if we cut them off this way
			if (m_layer == EnemyObjectLayer)
			{
				this->hazardSpawned -= (*it)->getMoney();
			}
		}
	}
}

void Game::SetLayerRotation(LayerType m_layer, float angle)
{
	for (std::vector<GameObject*>::iterator it = sceneGameObjectsLayered[m_layer].begin(); it != sceneGameObjectsLayered[m_layer].end(); it++)
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
	if (!isVectorEmpty(&this->sceneGameObjectsTyped[EnemyFire]))
		return false;
	else if (!isVectorEmpty(&this->sceneGameObjectsTyped[EnemyObject]))
		return false;
	
	return true;
}

int Game::getHazard()
{
	return hazard;
}

void Game::resetHazard(int hazard_overkill)
{
	hazard = hazard_overkill;
	hazardSpawned = 0;
}

void Game::GetBeastScoreBonus(float m_playerShipBeastScore, float m_sceneBeastScore)
{
	this->BeastScoreBonus = m_playerShipBeastScore + m_sceneBeastScore;
}

TargetScan Game::FoundNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range)
{
	sf::Vector2f pos;
	float shortest_distance = -1;
	for (std::vector<GameObject*>::iterator it = sceneGameObjectsTyped[type].begin(); it != sceneGameObjectsTyped[type].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->isOnScene && !(*it)->ghost)
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
	for (std::vector<GameObject*>::iterator it = sceneGameObjectsTyped[type].begin(); it != sceneGameObjectsTyped[type].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->isOnScene && !(*it)->ghost)
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
			sf::Vector2f diff_position = GameObject::getSize_for_Direction(this->direction, (sf::Vector2f((ref_position.y - pos.y), (ref_position.x - pos.x))));
			diff_position.x *= GameObject::getDirectionMultiplier(this->direction).x;
			angle = acos(diff_position.x / shortest_distance);
			angle = angle * 180 / M_PI;

			diff_position.y *= GameObject::getDirectionMultiplier(this->direction).y;

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
	for (std::vector<GameObject*>::iterator it = sceneGameObjectsTyped[GameObjectType::EnemyObject].begin(); it != sceneGameObjectsTyped[GameObjectType::EnemyObject].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->display_name == m_display_name)
		{
			(*it)->wake_up = true;
		}
	}
}

bool Game::InsertObjectInShipGrid(GameObject& object, int index)
{
	bool result = hud.shipGrid.insertObject(object, index);

	return result;
}

bool Game::InsertObjectInEquipmentGrid(GameObject& object, int index)
{
	bool result = hud.equipmentGrid.insertObject(object, index);

	return result;
}

bool Game::SwapEquipObjectInShipGrid(int index_ship, int index_equipment)
{
	if (hud.shipGrid.getCellPointerFromIntIndex(index_ship) != NULL)
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Swapping ship #'%d' to eq. # %d", index_ship + 1, index_equipment + 1));
		GameObject* tmpShip = hud.shipGrid.getCellPointerFromIntIndex(index_ship);
		//Equipement > Ship
		hud.shipGrid.setCellPointerForIntIndex(index_ship, hud.equipmentGrid.getCellPointerFromIntIndex(index_equipment));
		//Ship > equipement
		hud.equipmentGrid.setCellPointerForIntIndex(index_equipment, tmpShip);
		tmpShip = NULL;
	}
	else
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Equiping ship #'%d'", index_ship + 1));
		//Equipement > Ship
		hud.shipGrid.setCellPointerForIntIndex(index_ship, hud.equipmentGrid.getCellPointerFromIntIndex(index_equipment));
		//Equipment = NULL
		hud.equipmentGrid.setCellPointerForIntIndex(index_equipment, NULL);
	}

	return true;
}

void Game::GarbageObjectInGrid(int grid_id, int index)
{
	hud.GarbageObjectInGrid(grid_id, index);
}

void Game::setRemovingCursorAnimation(CursorFeedbackStates animation_index)
{
	this->hud.setRemovingCursorAnimation(animation_index);
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