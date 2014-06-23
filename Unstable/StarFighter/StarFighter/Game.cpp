#include "Game.h"
#define stringify(x)  #x

const char* IndependantTypeValues[] = 
{
	stringify( Background ),
	stringify( PlayerShip ),
	stringify( FriendlyFire ),
	stringify( EnemyFire ),
	stringify( EnemyObject )
};

void Game::init(RenderWindow* window)
{
	this->window = window;
	this->sceneChronometer.restart();

	for(int i =0; i< (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered));i++)
	{
		sceneIndependantsLayered[i] = new std::list<Independant*>;
	}
	for(int i =0; i< (sizeof(sceneIndependantsTyped)/sizeof(*sceneIndependantsTyped));i++)
	{
		sceneIndependantsTyped[i] = new std::list<Independant*>;
	}

	hazard = 0;//initalisation of the scoring system
}


sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}

void Game::addToScene(Independant *object, int layer, IndependantType type)
{
	object->collider_type = type;

	if(layer >= 0 && layer < (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered)))
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
}

void Game::drawScene()
{
	for(int i =0; i< (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered));i++)
	{
		for (std::list<Independant*>::iterator it = (*(this->sceneIndependantsLayered[i])).begin(); it != (*(this->sceneIndependantsLayered[i])).end(); it++)
		{
			if((*(*it)).visible)
				this->window->draw((*(*it)));
		}
	}
	//window->draw(ship_hud->armorBar);
	//window->draw(ship_hud->shieldBar);
}

void Game::colisionChecksV2()
{
	sf::Clock dt;
	dt.restart();
	int i= 0;

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::list<Independant*>::iterator it1 = (*this->sceneIndependantsTyped[IndependantType::PlayerShip]).begin(); it1 != (*this->sceneIndependantsTyped[IndependantType::PlayerShip]).end(); it1++)
	{
		//Enemy bullets hitting the player
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::EnemyFire]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::EnemyFire]).end(); it2++)
		{
			i++;
			//Bullets are invisible after impact
			if(SimpleCollision::AreColliding((*it1),(*it2)))
			{
				if((*it2)->collider_type == EnemyFire)
				{
					(*it2)->setVisible(false);
					this->garbage.push_back(*it2);

					//Do something (like, kill ship) -> OK
					(*it1)->damage_from(*(*it2));
					//explosion
					FX* explosion = new FX (sf::Vector2f((*it2)->getPosition().x, (*it2)->getPosition().y),sf::Vector2f(0,0), FX_EXPLOSION_FILENAME, sf::Vector2f(FX_EXPLOSION_WIDTH, FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(FX_MEDIUM_EXPLOSION_DURATION));
					this->addToScene(explosion, LayerType::ExplosionLayer, IndependantType::Neutral);
					//hide destroyed item
					if ((*it1)->getIndependantArmor() <= 0)
					{
						(*it1)->setVisible(false);
						FX* explosion = new FX (sf::Vector2f((*it1)->getPosition().x, (*it1)->getPosition().y),sf::Vector2f(0,0), FX_EXPLOSION_FILENAME, sf::Vector2f(FX_EXPLOSION_WIDTH, FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(FX_LARGE_EXPLOSION_DURATION));	
						this->addToScene(explosion, LayerType::ExplosionLayer, IndependantType::Neutral);
						//we all deserve another chance...
						(*it1)->Respawn();
						hazard = 0;
						//this->garbage.push_back(*it1);
					}
				}
			}
		}

		//Enemy objects
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).end(); it2++)
		{
			i++;
			if(SimpleCollision::AreColliding((*it1),(*it2)))
			{
				//Do something (like, kill ship)
				(*it1)->damage_from(*(*it2));
				//explosion
				FX* explosion = new FX (sf::Vector2f((*it1)->getPosition().x, (*it1)->getPosition().y),sf::Vector2f(0,0), FX_EXPLOSION_FILENAME, sf::Vector2f(FX_EXPLOSION_WIDTH, FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(FX_LITTLE_EXPLOSION_DURATION));	
				this->addToScene(explosion, LayerType::ExplosionLayer, IndependantType::Neutral);

				//death
				if ((*it1)->getIndependantArmor() <= 0)
				{
					(*it1)->setVisible(false);
					FX* explosion = new FX (sf::Vector2f((*it1)->getPosition().x, (*it1)->getPosition().y),sf::Vector2f(0,0), FX_EXPLOSION_FILENAME, sf::Vector2f(FX_EXPLOSION_WIDTH, FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(FX_LARGE_EXPLOSION_DURATION));	
					this->addToScene(explosion, LayerType::ExplosionLayer, IndependantType::Neutral);
					//we all deserve another chance...
					(*it1)->Respawn();
					hazard = 0;
					//this->garbage.push_back(*it1);
				}


			}
		}

		//Loot
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::LootObject]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::LootObject]).end(); it2++)
		{
			i++;
			if(SimpleCollision::AreColliding((*it1),(*it2)))
			{
				//Do something (like, take the loot)
				(*it1)->get_money_from(*(*it2));
				(*it2)->setVisible(false);
				(*it2)->isOnScene = false;
				this->garbage.push_back(*it2);
			}
		}
	}

	//Then, check if any allied bullet collide with any enemy
	for (std::list<Independant*>::iterator it1 = (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).begin(); it1 != (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).end(); it1++)
	{
		//Enemy objects
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::FriendlyFire]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::FriendlyFire]).end(); it2++)
		{
			i++;
			//Bullets are invisible after impact
			if(SimpleCollision::AreColliding((*it1),(*it2)))
			{
				if((*it2)->collider_type == FriendlyFire)
				{
					(*it2)->setVisible(false);
					this->garbage.push_back(*it2);

					//Do something (like, kill the enemy ship ?)
					(*it1)->damage_from(*(*it2));
					//explosion
					FX* explosion = new FX (sf::Vector2f((*it2)->getPosition().x, (*it2)->getPosition().y),sf::Vector2f(0,0), FX_EXPLOSION_FILENAME, sf::Vector2f(FX_EXPLOSION_WIDTH, FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(FX_MEDIUM_EXPLOSION_DURATION));
					this->addToScene(explosion, LayerType::ExplosionLayer, IndependantType::Neutral);


					//death
					if ((*it1)->getIndependantArmor() <= 0)
					{
						(*it1)->setVisible(false);
						FX* explosion = new FX (sf::Vector2f((*it1)->getPosition().x, (*it1)->getPosition().y),sf::Vector2f(0,0), FX_EXPLOSION_FILENAME, sf::Vector2f(FX_EXPLOSION_WIDTH, FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(FX_LARGE_EXPLOSION_DURATION));	
						this->addToScene(explosion, LayerType::ExplosionLayer, IndependantType::Neutral);
						//this->addToScene(((Enemy*)(*it1))->death(), LayerType::ExplosionLayer, IndependantType::Neutral);

						//Loot
						Loot* loot = new Loot (sf::Vector2f((*it1)->getPosition().x, (*it1)->getPosition().y),sf::Vector2f(0, LOOT_SPEED_Y), LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH));
						this->addToScene(loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
						hazard += (*it1)->getMoney();
						loot->get_money_from(*(*it1));
						//sent to garbage collector
						this->garbage.push_back(*it1);

					}

				}
			}
		}
	}

	//printf("| Colisions: %d (x%d)",dt.getElapsedTime().asMilliseconds(),i);
}

void Game::cleanGarbage()
{
	sf::Clock dt;
	dt.restart();

	for (std::vector<Independant*>::iterator it = (this->garbage).begin(); it != (this->garbage).end(); it++)
	{
		this->sceneIndependants.remove(*it);
		for(int i =0; i< (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered));i++)
		{
			(*(this->sceneIndependantsLayered[i])).remove(*it);
		}
		for(int i =0; i< (sizeof(sceneIndependantsTyped)/sizeof(*sceneIndependantsTyped));i++)
		{
			(*(this->sceneIndependantsTyped[i])).remove(*it);
		}
		(*it)->~Independant();
	}

	this->garbage.clear();

	//printf("| Clean: %d ",dt.getElapsedTime().asMilliseconds());
}

void Game::collectGarbage()
{
	sf::Clock dt;
	dt.restart();

	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{

		if(!(**it).isOnScene)
		{
			//ended FX and loot objets
			if ((**it).collider_type == IndependantType::LootObject || (**it).collider_type == IndependantType::Neutral)
			{
				this->garbage.push_back(*it);
			}

			//out of screen objets
			else
			{
				if(((**it).getPosition().x + ((**it).m_size.x)/2 >= 0 && (**it).getPosition().x - ((**it).m_size.x)/2 <= SCENE_SIZE_X) && ((**it).getPosition().y + ((**it).m_size.y)/2 >= 0 && (**it).getPosition().y - ((**it).m_size.y)/2 <= SCENE_SIZE_Y))
				{
					(**it).isOnScene = true;
				}
			}

		}

		if((**it).collider_type == Background || !(**it).isOnScene)
		{
			continue;
		}

		//Out of scene content
		if((**it).getPosition().x < 0 + ((**it).m_size.x)/2 < 0 ||  (**it).getPosition().x - ((**it).m_size.x)/2 > SCENE_SIZE_X ||(**it).getPosition().y + ((**it).m_size.y)/2 < 0 || (**it).getPosition().y - ((**it).m_size.y)/2 > SCENE_SIZE_Y)
		{
			this->garbage.push_back(*it);
			continue;
		}
	}

	//printf("Collect: %d ",dt.getElapsedTime().asMilliseconds());

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