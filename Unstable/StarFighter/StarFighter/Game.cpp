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

	scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);
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

	if (((int)m_layer >= 0 && (int)m_layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_Independant))
	{
		AddIndependantToVector(object, &this->sceneIndependantsTyped[(int)type]);
		AddIndependantToVector(object, &this->sceneIndependantsLayered[(int)m_layer]);
		AddIndependantToVector(object, &this->sceneIndependants);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add Independant '%s' to layer '%d'", object->getName().c_str(), (int)m_layer));
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
	//printf("OnScene: %d / Collected: %d\n", this->sceneIndependants.size(), this->garbage.size());

	//TODO: Updating screen resolution
	scale_factor.x = 1.0f * screen_size.x / REF_WINDOW_RESOLUTION_X;
	scale_factor.y = 1.0f * screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Clean garbage
	cleanGarbage();

	//Checking colisions
	colisionChecksV2();

	size_t sceneIndependantsSize = this->sceneIndependants.size();

	for (size_t i = 0; i < sceneIndependantsSize; i++)
	{
		if (this->sceneIndependants[i] == NULL)
			continue;

		this->sceneIndependants[i]->update(deltaTime);
	}

	//Collect the dust
	collectGarbage();
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
		else
		{
			for (std::vector<Independant*>::iterator it = this->sceneIndependantsLayered[i].begin(); it != this->sceneIndependantsLayered[i].end(); it++)
			{
				if (*it == NULL)
					continue;

				if ((*(*it)).visible)
				{
					this->mainScreen.draw((*(*it)));
				}
			}
		}
	}

	this->mainScreen.display();
	sf::Sprite temp(this->mainScreen.getTexture());
	temp.scale(scale_factor.x, scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	this->window->draw(temp);
}

void Game::colisionChecksV2()
{
	sf::Clock dt;
	dt.restart();

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<Independant*>::iterator it1 = sceneIndependantsTyped[IndependantType::PlayerShip].begin(); it1 != sceneIndependantsTyped[IndependantType::PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Enemy bullets hitting the player
		for (std::vector<Independant*>::iterator it2 = sceneIndependantsTyped[IndependantType::EnemyFire].begin(); it2 != sceneIndependantsTyped[IndependantType::EnemyFire].end(); it2++)
		{
			if (*it2 == NULL)
				continue;
			
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 
				
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
	const size_t garbageSize = this->garbage.size();
	const size_t sceneIndependantsSize = this->sceneIndependants.size();
	//Size layer
	size_t sceneIndependantsLayeredSize[NBVAL_Layer];
	for (int layer = 0; layer < NBVAL_Layer; layer++)
	{
		sceneIndependantsLayeredSize[layer] = this->sceneIndependantsLayered[layer].size();
	}
	//Size ind type
	size_t sceneIndependantsTypedSize[NBVAL_Independant];
	for (int layer = 0; layer < NBVAL_Independant; layer++)
	{
		sceneIndependantsTypedSize[layer] = this->sceneIndependantsTyped[layer].size();
	}

	//Scene independants
	for (size_t i = 0; i < garbageSize; i++)
	{
		Independant*    pCurIndependant = this->garbage[i];

		// On remet à NULL lorsqu'on a trouvé un élément à dégager
		for (size_t j = 0; j < sceneIndependantsSize; j++)
		{
			if (this->sceneIndependants[j] == pCurIndependant)
			{
				this->sceneIndependants[j] = NULL;
				break;
			}
		}

		// "layered"...
		const int layer = pCurIndependant->layer;
		for (size_t j = 0; j < sceneIndependantsLayeredSize[layer]; j++)
		{
			if (this->sceneIndependantsLayered[layer][j] == pCurIndependant)
			{
				this->sceneIndependantsLayered[layer][j] = NULL;
				break;
			}
		}

		// "typed"
		const int type = pCurIndependant->collider_type;
		for (size_t j = 0; j < sceneIndependantsTypedSize[type]; j++)
		{
			if (this->sceneIndependantsTyped[type][j] == pCurIndependant)
			{
				this->sceneIndependantsTyped[type][j] = NULL;
				break;
			}
		}

		//destructor function ??

		// A la fin, on delete l'élément
		delete pCurIndependant;
	}

	//printf("| Clean: %d ",dt.getElapsedTime().asMilliseconds());
}

void Game::AddIndependantToVector(Independant* pIndependant, vector<Independant*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] == NULL)
		{
			(*vector)[i] = pIndependant;
			return; // ayé, on a trouvé un free slot, inséré, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas où on n'a pas trouvé de free slot => on rajoute à la fin
	vector->push_back(pIndependant);
}

void Game::collectGarbage()
{
	sf::Clock dt;
	dt.restart();

	this->garbage.clear();

	for (std::vector<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
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