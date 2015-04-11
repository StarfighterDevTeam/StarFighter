#include "Game.h"
#define stringify(x)  #x

const char* IndependantTypeValues[] =
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

	this->hud.Init(0, 0);
}

sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}

void Game::SetPlayerShip(Ship* m_playerShip)
{
	this->playerShip = m_playerShip;
}

void Game::addToScene(Independant *object, LayerType m_layer, IndependantType type)
{
	object->layer = m_layer;
	object->collider_type = type;

	//Window resolution adjustements
	//object->setScale(scale_factor.x, scale_factor.y);

	if ((int)m_layer >= 0 && (int)m_layer < NBVAL_Layer)
	{
		sceneIndependantsTyped[(int)type].push_back(object);
		sceneIndependantsLayered[(int)m_layer].push_back(object);
		sceneIndependants.push_back(object);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add Independant '%s' to layer '%d'", object->getName().c_str(), (int)m_layer));
	}
}

void Game::updateScene(Time deltaTime)
{
	//printf("OnScene: %d / Collected: %d\n", this->sceneIndependants.size(), this->garbage.size());

	//TODO: Updating screen resolution
	scale_factor.x = 1.0f * screen_size.x / REF_WINDOW_RESOLUTION_X;
	scale_factor.y = 1.0f * screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Collect & clean garbage
	collectGarbage();
	cleanGarbage();

	//Checking colisions
	colisionChecksV2();

	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{
		(*it)->update(deltaTime, hyperspeedMultiplier);
	}
	//isLastEnemyDead();
}


void Game::updateHud(int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_level, std::string scene_name, sf::Time deltaTime, bool hub,
	int focused_item_type, string f_name, float f_max_speed, float f_hyperspeed, int f_armor, int f_shield, int f_shield_regen, int f_damage, bool f_bot, float f_ammo_speed, PatternType f_pattern,
	int f_multishot, int f_xspread, float f_rate_of_fire, ShotMode f_shot_mode, float f_dispersion, int f_rafale, float f_rafale_cooldown, TargetSeaking f_target_seaking)
{
	this->hud.Update(m_armor, m_shield, m_money, m_graze_count, m_hazard_level, scene_name, deltaTime, hub, focused_item_type, f_name, f_max_speed, f_hyperspeed, 
		f_armor, f_shield, f_shield_regen, f_damage, f_bot, f_ammo_speed, f_pattern, f_multishot, f_xspread, f_rate_of_fire, f_shot_mode, f_dispersion, f_rafale, f_rafale_cooldown, f_target_seaking);
}

void Game::drawScene()
{
	this->mainScreen.clear();

	for (int i = 0; i < NBVAL_Layer; i++)
	{
		for (std::list<Independant*>::iterator it = this->sceneIndependantsLayered[i].begin(); it != this->sceneIndependantsLayered[i].end(); it++)
		{
			if ((*(*it)).visible)
			{
				if (!(*(*it)).transparent)
				{
					this->mainScreen.draw((*(*it)));
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

Independant* Game::getHudFocusedItem()
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
		{
			return hud.fakeShipGrid.focus;
		}
		case HudGrid_EquipmentGrid:
		{
			return hud.fakeEquipmentGrid.focus;
		}
	}

	return sf::Vector2i(-1, -1);
}

void Game::colisionChecksV2()
{
	sf::Clock dt;
	dt.restart();
	int i = 0;

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::list<Independant*>::iterator it1 = sceneIndependantsTyped[IndependantType::PlayerShip].begin(); it1 != sceneIndependantsTyped[IndependantType::PlayerShip].end(); it1++)
	{
		//Enemy bullets hitting the player
		for (std::list<Independant*>::iterator it2 = sceneIndependantsTyped[IndependantType::EnemyFire].begin(); it2 != sceneIndependantsTyped[IndependantType::EnemyFire].end(); it2++)
		{
			i++;
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
					//this->addToScene(explosion, LayerType::ExplosionLayer, IndependantType::Neutral);
					//hide destroyed item
					if ((*it1)->getIndependantArmor() <= 0)
					{
						(*it1)->visible = false;
						(*it1)->Death();
						//we all deserve another chance...
						(*it1)->Respawn();
						//hazard = 0;
					}
				}
			}
		}

		//Enemy objects
		for (std::list<Independant*>::iterator it2 = sceneIndependantsTyped[IndependantType::EnemyObject].begin(); it2 != sceneIndependantsTyped[IndependantType::EnemyObject].end(); it2++)
		{
			i++;
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something (like, kill ship)
				(*it1)->damage_from(*(*it2));
				//explosion
				//TO DO : explosion impact enemy vs ship

				//death of player?
				if ((*it1)->getIndependantArmor() <= 0)
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
				if ((*it2)->getIndependantArmor() <= 0)
				{
					(*it2)->Death();
					//Loot
					hazard += (*it2)->getMoney();
					(*it2)->CreateRandomLoot(this->BeastScoreBonus);
					(*it2)->GenerateLoot();
				}
			}
		}

		//Loot
		for (std::list<Independant*>::iterator it2 = sceneIndependantsTyped[IndependantType::LootObject].begin(); it2 != sceneIndependantsTyped[IndependantType::LootObject].end(); it2++)
		{
			i++;
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
		for (std::list<Independant*>::iterator it2 = sceneIndependantsTyped[IndependantType::PortalObject].begin(); it2 != sceneIndependantsTyped[IndependantType::PortalObject].end(); it2++)
		{
			i++;
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				(*it1)->GetPortal((*it2));
			}
		}
	}

	//Then, check if any allied bullet collide with any enemy
	for (std::list<Independant*>::iterator it1 = sceneIndependantsTyped[IndependantType::EnemyObject].begin(); it1 != sceneIndependantsTyped[IndependantType::EnemyObject].end(); it1++)
	{
		//Player bullets on enemy
		for (std::list<Independant*>::iterator it2 = sceneIndependantsTyped[IndependantType::FriendlyFire].begin(); it2 != sceneIndependantsTyped[IndependantType::FriendlyFire].end(); it2++)
		{
			i++;
			//Bullets are invisible after impact
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				if ((*it2)->collider_type == FriendlyFire)
				{
					//Do something (like, kill the enemy ship ?)
					(*it1)->damage_from(*(*it2));
					//explosion
					(*it2)->Death();

					//death
					if ((*it1)->getIndependantArmor() <= 0)
					{
						//Loot
						hazard += (*it1)->getMoney();
						(*it1)->CreateRandomLoot(this->BeastScoreBonus);
						(*it1)->GenerateLoot();
						(*it1)->Death();
					}
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

	for (std::list<Independant*>::iterator it = (this->garbage).begin(); it != (this->garbage).end(); it++)
	{
		Independant*	ptr = *it;
		this->sceneIndependants.remove(ptr);
		this->sceneIndependantsLayered[ptr->layer].remove(*it);
		this->sceneIndependantsTyped[(int)(ptr->collider_type)].remove(*it);
		delete (*it);
	}

	//printf("| Clean: %d ",dt.getElapsedTime().asMilliseconds());
}

void Game::collectGarbage()
{
	sf::Clock dt;
	dt.restart();

	this->garbage.clear();

	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{
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

		//Out of scene content
		if (!(**it).DontGarbageMe)
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
	for (std::list<Independant*>::iterator it = sceneIndependantsLayered[m_layer].begin(); it != sceneIndependantsLayered[m_layer].end(); it++)
	{
		if (only_offscene)
		{
			if (!(*it)->isOnScene)
			{
				(*it)->GarbageMe = true;
				clear_count++;

			}
		}
		else
		{
			(*it)->visible = false;
			(*it)->isOnScene = false;
			(*it)->GarbageMe = true;
		}
	}
}

void Game::SetLayerRotation(LayerType m_layer, float angle)
{
	for (std::list<Independant*>::iterator it = sceneIndependantsLayered[m_layer].begin(); it != sceneIndependantsLayered[m_layer].end(); it++)
	{
		(*it)->setRotation(angle);
	}
}

bool Game::isLastEnemyDead()
{
	int n = 0;

	n += sceneIndependantsTyped[IndependantType::LootObject].size();
	n += sceneIndependantsTyped[IndependantType::EnemyFire].size();
	n += sceneIndependantsTyped[IndependantType::EnemyObject].size();

	if (n == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
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

TargetScan Game::FoundNearestIndependant(IndependantType type, sf::Vector2f ref_position, float range)
{
	sf::Vector2f pos;
	float shortest_distance = -1;
	for (std::list<Independant*>::iterator it = sceneIndependantsTyped[type].begin(); it != sceneIndependantsTyped[type].end(); it++)
	{
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

float Game::GetAngleToNearestIndependant(IndependantType type, sf::Vector2f ref_position, float range)
{
	float angle = 0.f;
	sf::Vector2f pos;
	float shortest_distance = -1;
	for (std::list<Independant*>::iterator it = sceneIndependantsTyped[type].begin(); it != sceneIndependantsTyped[type].end(); it++)
	{
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
			sf::Vector2f diff_position = Independant::getSize_for_Direction(this->direction, (sf::Vector2f((ref_position.y - pos.y), (ref_position.x - pos.x))));
			diff_position.x *= Independant::getDirectionMultiplier(this->direction).x;
			angle = acos(diff_position.x / shortest_distance);
			angle = angle * 180 / M_PI;

			diff_position.y *= Independant::getDirectionMultiplier(this->direction).y;

			//if (ref_position.x < pos.x)
			if (diff_position.y < 0)
			{
				angle = -angle;
			}
		}
	}

	return angle;
}

void Game::WakeUpEnemiesWithName(string m_display_name)
{
	for (std::list<Independant*>::iterator it = sceneIndependantsTyped[IndependantType::EnemyObject].begin(); it != sceneIndependantsTyped[IndependantType::EnemyObject].end(); it++)
	{
		if ((*it)->display_name == m_display_name)
		{
			(*it)->wake_up = true;
		}
	}
}

bool Game::InsertObjectInShipGrid(Independant& object, int index)
{
	bool result = hud.shipGrid.insertObject(object, index);

	return result;
}

bool Game::InsertObjectInEquipmentGrid(Independant& object)
{
	bool result = hud.equipmentGrid.insertObject(object);

	return result;
}

bool Game::SwapEquipObjectInShipGrid(int index_ship, int index_equipment, bool needs_swapping)
{
	
	if (needs_swapping)
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Swapping ship #'%d' to eq. # %d", index_ship + 1, index_equipment + 1));
		Independant* tmpShip = hud.shipGrid.getCellPointerFromIntIndex(index_ship);
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
	}

	return true;
}

void Game::GarbageObjectInGrid(int grid_id, int index)
{
	hud.GarbageObjectInGrid(grid_id, index);
}

sf::Time Game::updateHudActionHoldingTime(sf::Time deltaTime)
{
	return hud.updateHudActionHoldingTime(deltaTime);
}