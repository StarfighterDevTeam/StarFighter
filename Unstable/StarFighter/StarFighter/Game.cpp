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
	TextureLoader::getInstance ()->loadAll();

	for(int i =0; i< (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered));i++)
	{
		sceneIndependantsLayered[i] = new std::list<Independant*>;
	}
	for(int i =0; i< (sizeof(sceneIndependantsTyped)/sizeof(*sceneIndependantsTyped));i++)
	{
		sceneIndependantsTyped[i] = new std::list<Independant*>;
	}
	
	//ship_hud = new PlayerHud();
	//ship_hud->Init();
}


sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}

void Game::addToScene(Independant *object, int layer, IndependantType type)
{
	if(layer >= 0 && layer < (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered)))
	{
		(*(sceneIndependantsTyped[(int)type])).push_back(object);
		(*(sceneIndependantsLayered[layer])).push_back(object); 
		this->sceneIndependants.push_back(object);
	}
	else
	{
		//throw ?
		//printf("Erreur, mauvais layer, connard\n");
	}
}

void Game::updateScene(Time deltaTime)
{

	printf("OnScene: %d / Collected: %d\n", this->sceneIndependants.size(), this->garbage.size());

	//Collect & clean garbage
	collectGarbage();
	cleanGarbage();

	//Checking colisions
	colisionChecksV2();

	sf::Clock dt;
	dt.restart();

	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{
		(*(*it)).update(deltaTime);
	}
	//ship_hud->update(deltaTime, sceneIndependantsTyped[IndependantType::PlayerShip].begin().getIndependantArmor(), sceneIndependantsTyped[IndependantType::PlayerShip].begin().getIndependantShield());
	printf("| Updt: %d \n",dt.getElapsedTime().asMilliseconds());
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

void Game::colisionChecks()
{
	sf::Clock dt;
	dt.restart();
	int i= 0;

	for (std::list<Independant*>::iterator it1 = (this->sceneIndependants).begin(); it1 != std::prev((this->sceneIndependants).end()); it1++)
	{
		for (std::list<Independant*>::iterator it2 = std::next(it1); it2 != (this->sceneIndependants).end(); it2++)
		{
			i++;
			//Bullets are invisible after impact
			if((*(*it1)).collide_with((*(*it2))))
			{
				if((*(*it1)).collider_type == FriendlyFire || (*(*it1)).collider_type == EnemyFire)
				{
					(*(*it1)).setVisible(false);
					this->garbage.push_back(*it1);
				}

				if((*(*it2)).collider_type == FriendlyFire || (*(*it2)).collider_type == EnemyFire)
				{
					(*(*it2)).setVisible(false);
					this->garbage.push_back(*it2);
				}

				//printf("boom [%s vs %s]\n", IndependantTypeValues[(*(*it1)).collider_type], IndependantTypeValues[(*(*it2)).collider_type]);
			}
		}
	}

	printf("| Colisions: %d (x%d)",dt.getElapsedTime().asMilliseconds(),i);
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
			if((*(*it1)).collide_with((*(*it2))))
			{
				if((*it2)->collider_type == EnemyFire)
				{
					(*it2)->setVisible(false);
					this->garbage.push_back(*it2);

					//Do something (like, kill ship) -> OK
					(*it1)->damage_from(*(*it2));
					if ((*it1)->getIndependantArmor() <= 0)
					{
						(*it1)->setVisible(false);
						this->garbage.push_back(*it1);
					}

				}
			}
		}

		//Enemy objects
		for (std::list<Independant*>::iterator it2 = (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).begin(); it2 != (*this->sceneIndependantsTyped[IndependantType::EnemyObject]).end(); it2++)
		{
			i++;
			if((*(*it1)).collide_with((*(*it2))))
			{
				//Do something (like, kill ship)
				(*it1)->damage_from(*(*it2));
				//ship_hud->update((*it1)->getIndependantArmor(), (*it1)->getIndependantShield());
				if ((*it1)->getIndependantArmor() <= 0)
				{
					(*it1)->setVisible(false);
					this->garbage.push_back(*it1);
				}


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
			if((*(*it1)).collide_with((*(*it2))))
			{
				if((*it2)->collider_type == FriendlyFire)
				{
					(*it2)->setVisible(false);
					this->garbage.push_back(*it2);

					//Do something (like, kill the enemy ship ?)
					(*it1)->damage_from(*(*it2));
					if ((*it1)->getIndependantArmor() <= 0)
					{
						(*it1)->setVisible(false);
						this->garbage.push_back(*it1);
					}

				}
			}
		}
	}
	
	printf("| Colisions: %d (x%d)",dt.getElapsedTime().asMilliseconds(),i);
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

	printf("| Clean: %d ",dt.getElapsedTime().asMilliseconds());
}

void Game::collectGarbage()
{
	sf::Clock dt;
	dt.restart();

	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{
		//isOnScene -> true
		if(!(**it).isOnScene)
		{
			if(((**it).getPosition().x + ((**it).m_size.x)/2 >= 0 && (**it).getPosition().x - ((**it).m_size.x)/2 <= SCENE_SIZE_X) && ((**it).getPosition().y + ((**it).m_size.y)/2 >= 0 && (**it).getPosition().y - ((**it).m_size.y)/2 <= SCENE_SIZE_Y))
			{
				(**it).isOnScene = true;
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

	printf("Collect: %d ",dt.getElapsedTime().asMilliseconds());

}
