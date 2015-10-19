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

	scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	screen_size = sf::Vector2f(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	LoadSFX();

	view.setCenter(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	view.setSize(sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));

	//default value
	map_size = (sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));
}

sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}


int Game::LoadSFX()
{
	if (!soundBuffers[0].loadFromFile("Assets/Sounds/Bounce1.ogg"))
		return -1;
	if (!soundBuffers[1].loadFromFile("Assets/Sounds/Bounce2.ogg"))
		return -1;
	if (!soundBuffers[2].loadFromFile("Assets/Sounds/Bounce3.ogg"))
		return -1;
	if (!soundBuffers[3].loadFromFile("Assets/Sounds/Bounce4.ogg"))
		return -1;
	if (!soundBuffers[4].loadFromFile("Assets/Sounds/Bounce5.ogg"))
		return -1;
	if (!soundBuffers[5].loadFromFile("Assets/Sounds/Goal.ogg"))
		return -1;
	if (!soundBuffers[6].loadFromFile("Assets/Sounds/Tackle.ogg"))
		return -1;
	if (!soundBuffers[7].loadFromFile("Assets/Sounds/Throw.ogg"))
		return -1;
	if (!soundBuffers[8].loadFromFile("Assets/Sounds/Catch.ogg"))
		return -1;
	//if (!soundBuffers[9].loadFromFile("Assets/Sounds/Switch.ogg"))
	//return -1;

	soundsBounce[0].setBuffer(soundBuffers[0]);
	soundsBounce[1].setBuffer(soundBuffers[1]);
	soundsBounce[2].setBuffer(soundBuffers[2]);
	soundsBounce[3].setBuffer(soundBuffers[3]);
	soundsBounce[4].setBuffer(soundBuffers[4]);
	soundsGoal.setBuffer(soundBuffers[5]);
	soundsTackle.setBuffer(soundBuffers[6]);
	soundsThrow.setBuffer(soundBuffers[7]);
	soundsCatch.setBuffer(soundBuffers[8]);
	//soundsSwitch.setBuffer(soundBuffers[9]);

}

void Game::PlaySFX(SFX_Bank sfx_name)
{
	if (sfx_name == SFX_Bounce)
	{
		int i = RandomizeIntBetweenValues(0, 4);
		soundsBounce[i].play();
	}
	if (sfx_name == SFX_Goal)
	{
		soundsGoal.play();
	}
	if (sfx_name == SFX_Tackle)
	{
		soundsTackle.play();
	}
	if (sfx_name == SFX_Throw)
	{
		soundsThrow.play();
	}
	if (sfx_name == SFX_Catch)
	{
		soundsCatch.play();
	}
	if (sfx_name == SFX_Switch)
	{
		//soundsSwitch.play();
	}
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

	for (size_t i = 0; i < sceneGameObjectsSize; i++)
	{
		if (this->sceneGameObjects[i] == NULL)
			continue;

		this->sceneGameObjects[i]->update(deltaTime);
	}

	//Collect the dust
	collectGarbage();

	//Update view
	//view.setCenter(scale_factor.x*view.getCenter().x, scale_factor.y*view.getCenter().y);
	//window->setView(view);
	
	mainScreen.setView(view);
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
			for (std::vector<GameObject*>::iterator it = this->sceneGameObjectsLayered[i].begin(); it != this->sceneGameObjectsLayered[i].end(); it++)
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
	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[GameObjectType::PlayerShip].begin(); it1 != sceneGameObjectsTyped[GameObjectType::PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Player gets discoball
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::DiscoballObject].begin(); it2 != sceneGameObjectsTyped[GameObjectType::DiscoballObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 

				//TRON SPECIFIC
				float angle = GetAngleOfCollision(*it1, *it2);
				(*it1)->GetDiscoball(*it2, angle);
			}
		}

		//Players in contact
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::PlayerShip].begin(); it2 != sceneGameObjectsTyped[GameObjectType::PlayerShip].end(); it2++)
		{
			if (*it2 == NULL)
				continue;
			if (*it1 == *it2)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 

				//TRON SPECIFIC
				float angle = GetAngleOfCollision(*it1, *it2);
				(*it1)->PlayerContact(*it2, angle);
			}
		}
	}

	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[GameObjectType::DiscoballObject].begin(); it1 != sceneGameObjectsTyped[GameObjectType::DiscoballObject].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Discoball hits goal
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::GoalBlueObject].begin(); it2 != sceneGameObjectsTyped[GameObjectType::GoalBlueObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 

				//TRON SPECIFIC
				(*it2)->PlayHitFeedback();
				PlaySFX(SFX_Goal);
			}
		}

		//Discoball hits goal
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[GameObjectType::GoalRedObject].begin(); it2 != sceneGameObjectsTyped[GameObjectType::GoalRedObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 

				//TRON SPECIFIC
				(*it2)->PlayHitFeedback();
				PlaySFX(SFX_Goal);
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

	//Scene GameObjects
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

		//destructor function ??

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
			if (((**it).getPosition().x + ((**it).m_size.x) / 2 >= 0 && (**it).getPosition().x - ((**it).m_size.x) / 2 <= map_size.x) && ((**it).getPosition().y + ((**it).m_size.y) / 2 >= 0 && (**it).getPosition().y - ((**it).m_size.y) / 2 <= map_size.y))
			{
				(**it).isOnScene = true;
			}
		}

		//Content that went on scene and then exited have to be deleted
		if (!(**it).DontGarbageMe && (**it).isOnScene)
		{
			if ((**it).getPosition().x + ((**it).m_size.x) / 2 < 0 || (**it).getPosition().x - ((**it).m_size.x) / 2 > map_size.x
				|| (**it).getPosition().y + ((**it).m_size.y) / 2 < 0 || (**it).getPosition().y - ((**it).m_size.y) / 2 > map_size.y)
			{
				this->garbage.push_back(*it);
				continue;
			}
		}
	}

	//printf("| Collect: %d ",dt.getElapsedTime().asMilliseconds());
}

float Game::GetAngleOfCollision(const GameObject* ref_obj, const GameObject* aimed_obj)
{
	sf::Vector2f diff_position;

	const float a = ref_obj->getPosition().x - aimed_obj->getPosition().x;
	const float b = ref_obj->getPosition().y - aimed_obj->getPosition().y;

	float distance_to_obj = (a * a) + (b * b);
	distance_to_obj = sqrt(distance_to_obj);

	// TO DO
	float angle;
	angle = acos(a / distance_to_obj);

	if (b < 0)
	{
		angle = -angle;
	}

	angle += M_PI_2;

	return angle;
}
