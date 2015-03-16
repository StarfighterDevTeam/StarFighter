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
	this->offscreen.create(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y, false);
	this->offscreen.setSmooth(true);

	this->sceneChronometer.restart();
	scale_factor.x = float(WINDOW_RESOLUTION_X) / float(REF_WINDOW_RESOLUTION_X);
	scale_factor.y = float(WINDOW_RESOLUTION_Y) / float(REF_WINDOW_RESOLUTION_Y);

	this->hazard = 0;//initalisation of the scoring system
	this->BeastScoreBonus = 0;
	this->direction = Directions::NO_DIRECTION;
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
		this->sceneIndependants.push_back(object);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add Independant '%s' to layer '%d'", object->getName().c_str(), (int)m_layer));
	}
}

void Game::updateScene(Time deltaTime)
{
	//printf("OnScene: %d / Collected: %d\n", this->sceneIndependants.size(), this->garbage.size());

	//Collect & clean garbage
	collectGarbage();
	cleanGarbage();

	//Checking colisions
	colisionChecksV2();

	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{
		(*it)->update(deltaTime);
	}
	isLastEnemyDead();
}

void Game::drawScene()
{
	this->offscreen.clear();

	for (int i = 0; i < NBVAL_Layer ; i++)
	{
		for (std::list<Independant*>::iterator it = this->sceneIndependantsLayered[i].begin(); it != this->sceneIndependantsLayered[i].end(); it++)
		{
			if ((*(*it)).visible)
			{
				if (!(*(*it)).transparent)
				{
					//this->window->draw((*(*it)));
					this->offscreen.draw((*(*it)));
				}
			}
		}
	}

	this->offscreen.display();
	sf::Sprite temp(this->offscreen.getTexture());
	temp.scale(WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y);
	//temp.setPosition((WINDOW_RESOLUTION_X - REF_WINDOW_RESOLUTION_X) / 2, (WINDOW_RESOLUTION_Y - REF_WINDOW_RESOLUTION_Y) / 2);
	this->window->draw(temp);
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
						(*it1)->setVisible(false);
						(*it1)->Death();
						//we all deserve another chance...
						(*it1)->Respawn();
						hazard = 0;
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
					(*it1)->setVisible(false);
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
				//(*it1)->get_money_from(*(*it2));
				(*it1)->GetLoot((*(*it2)));
				(*it2)->setVisible(false);
				(*it2)->isOnScene = false;
				(*it2)->GarbageMe = true;
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

	for (std::vector<Independant*>::iterator it = (this->garbage).begin(); it != (this->garbage).end(); it++)
	{
		Independant*	ptr	= *it;
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
			//ended FX and loot objets
			if ((**it).collider_type == IndependantType::LootObject || (**it).collider_type == IndependantType::Neutral)
			{
				this->garbage.push_back(*it);
				continue;
			}
			else
			{
				//objects that are spawning out of screen are not deleted
				if (((**it).getPosition().x + ((**it).m_size.x) / 2 >= 0 && (**it).getPosition().x - ((**it).m_size.x) / 2 <= SCENE_SIZE_X) && ((**it).getPosition().y + ((**it).m_size.y) / 2 >= 0 && (**it).getPosition().y - ((**it).m_size.y) / 2 <= SCENE_SIZE_Y))
				{
					(**it).isOnScene = true;
				}
			}
		}

		if ((**it).collider_type == BackgroundObject || !(**it).isOnScene)
		{
			continue;
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
	if (clear_count > 0)
	{
		printf("DEBUG: Clearing enemies that remained out of the scene (last screen = no spawn anymore) (number: %d).\n", clear_count);
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
			float distance_to_ref = (pow((ref_position.x - (*it)->getPosition().x), 2) + pow((ref_position.y - (*it)->getPosition().y), 2));
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
		if (range > 0 && pow(range, 2) <= shortest_distance)
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
			float distance_to_ref = (pow((ref_position.x - (*it)->getPosition().x), 2) + pow((ref_position.y - (*it)->getPosition().y), 2));
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
		if (range > 0 && pow(range, 2) < shortest_distance)//shortest_distance is already squared
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