#include "Game.h"
#define stringify(x)  #x

const char* IndependantTypeValues[] =
{
	stringify(Background),
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

	for (int i = 0; i < (sizeof(sceneIndependantsLayered) / sizeof(*sceneIndependantsLayered)); i++)
	{
		sceneIndependantsLayered[i] = new std::list<Independant*>;
	}
	for (int i = 0; i < (sizeof(sceneIndependantsTyped) / sizeof(*sceneIndependantsTyped)); i++)
	{
		sceneIndependantsTyped[i] = new std::list<Independant*>;
	}

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

void Game::addToScene(Independant *object, int layer, IndependantType type)
{
	object->collider_type = type;

	//Window resolution adjustements
	//object->setScale(scale_factor.x, scale_factor.y);

	if (layer >= 0 && layer < (sizeof(sceneIndependantsLayered) / sizeof(*sceneIndependantsLayered)))
	{
		(*(sceneIndependantsTyped[(int)type])).push_back(object);
		(*(sceneIndependantsLayered[layer])).push_back(object);
		this->sceneIndependants.push_back(object);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add Independant '%s' to layer '%d'", object->getName().c_str(), layer));
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

	for (int i = 0; i < (sizeof(sceneIndependantsLayered) / sizeof(*sceneIndependantsLayered)); i++)
	{
		for (std::list<Independant*>::iterator it = (*(this->sceneIndependantsLayered[i])).begin(); it != (*(this->sceneIndependantsLayered[i])).end(); it++)
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
	for (std::list<Independant*>::iterator it1 = (*this->sceneIndependantsTyped[IndependantType::PlayerShip]).begin(); it1 != (*this->sceneIndependantsTyped[IndependantType::PlayerShip]).end(); it1++)
	{
		//Enemy bullets hitting the player
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::EnemyFire]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::EnemyFire]).end(); it2++)
		{
			i++;
			if (SimpleCollision::IsGrazing((*it1), (*it2)))
			{
				(*it1)->GetGrazing();

				if (SimpleCollision::AreColliding((*it1), (*it2)))
				{
					//Bullets are invisible after impact
					(*it2)->setVisible(false);
					(*it2)->isOnScene = false;
					(*it2)->GarbageMe = true;

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
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).end(); it2++)
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
					//TODO : 
					//(*it1)->CreateRandomLoot(BeastScale_HazardLevel + BeastScale_PlayerCombo);
					(*it2)->GenerateLoot();

					//sent to garbage collector
					(*it2)->setVisible(false);
					(*it2)->GarbageMe = true;
				}
			}
		}

		//Loot
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::LootObject]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::LootObject]).end(); it2++)
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
	}

	//Then, check if any allied bullet collide with any enemy
	for (std::list<Independant*>::iterator it1 = (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).begin(); it1 != (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).end(); it1++)
	{
		//Player bullets on enemy
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::FriendlyFire]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::FriendlyFire]).end(); it2++)
		{
			i++;
			//Bullets are invisible after impact
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				if ((*it2)->collider_type == FriendlyFire)
				{
					(*it2)->setVisible(false);
					(*it2)->GarbageMe = true;

					//Do something (like, kill the enemy ship ?)
					(*it1)->damage_from(*(*it2));
					//explosion
					(*it2)->Death();

					//death
					if ((*it1)->getIndependantArmor() <= 0)
					{
						(*it1)->Death();
						//Loot
						hazard += (*it1)->getMoney();
						(*it1)->CreateRandomLoot(this->BeastScoreBonus);
						//TODO : 
						//(*it1)->CreateRandomLoot(BeastScale_HazardLevel + BeastScale_PlayerCombo);
						(*it1)->GenerateLoot();

						//sent to garbage collector
						(*it1)->setVisible(false);
						(*it1)->GarbageMe = true;
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
		this->sceneIndependants.remove(*it);
		for (int i = 0; i < (sizeof(sceneIndependantsLayered) / sizeof(*sceneIndependantsLayered)); i++)
		{
			(*(this->sceneIndependantsLayered[i])).remove(*it);
		}
		for (int i = 0; i < (sizeof(sceneIndependantsTyped) / sizeof(*sceneIndependantsTyped)); i++)
		{
			(*(this->sceneIndependantsTyped[i])).remove(*it);
		}
		(*it)->~Independant();
		free(*it);
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

		if ((**it).collider_type == Background || !(**it).isOnScene)
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
	for (std::list<Independant*>::iterator it = (this->sceneIndependantsLayered[m_layer])->begin(); it != (this->sceneIndependantsLayered[m_layer])->end(); it++)
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
		printf("Clearing (%d) enemies that remained out of the scene (last screen = no spawn anymore).\n", clear_count);
	}
}

void Game::SetLayerRotation(LayerType m_layer, float angle)
{
	for (std::list<Independant*>::iterator it = (this->sceneIndependantsLayered[m_layer])->begin(); it != (this->sceneIndependantsLayered[m_layer])->end(); it++)
	{
		(*it)->setRotation(angle);
	}
}

bool Game::isLastEnemyDead()
{
	int n = 0;

	n += sceneIndependantsTyped[IndependantType::LootObject]->size();
	n += sceneIndependantsTyped[IndependantType::EnemyFire]->size();
	n += sceneIndependantsTyped[IndependantType::EnemyObject]->size();

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

float Game::GetAngleToNearestIndependant(IndependantType type, sf::Vector2f ref_position)
{
	float angle = 0.f;
	sf::Vector2f pos;
	float shortest_distance = -1.f;
	for (std::list<Independant*>::iterator it = (this->sceneIndependantsTyped[type])->begin(); it != (this->sceneIndependantsTyped[type])->end(); it++)
	{
		if ((*it)->isOnScene && !(*it)->ghost)
		{
			float distance_to_ref = (pow((ref_position.x - (*it)->getPosition().x), 2) + pow((ref_position.y - (*it)->getPosition().y), 2));
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				pos = (*it)->getPosition();
			}
		}
	}
	if (shortest_distance > 0)
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

	return angle;
}

void Game::WakeUpEnemiesWithName(string m_display_name)
{
	for (std::list<Independant*>::iterator it = (this->sceneIndependantsTyped[IndependantType::EnemyObject])->begin(); it != (this->sceneIndependantsTyped[IndependantType::EnemyObject])->end(); it++)
	{
		if ((*it)->display_name == m_display_name)
		{
			(*it)->wake_up = true;
		}
	}
}