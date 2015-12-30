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
	screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	view.setCenter(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	view.setSize(sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));
	//view.zoom(0.3f);

	//default value
	map_size = (sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));

	//fonts
	font = new sf::Font();
	if (!font->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
	{
		// error
		//TODO: font loader
	}

	font2 = new sf::Font();
	if (!font2->loadFromFile("Assets/Fonts/arial.ttf"))
	{
		// error
		//TODO: font loader
	}

	//TODO: save in local preferences
	m_Music_Activated = true;
	m_SFX_Activated = false;

	//Sounds
	LoadSFX();

	//Music
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading Musics");
	//if (!SpaceCowboys.openFromFile("Assets/Music/SpaceCowboys.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/CrimeaDigital.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/Rebecca.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/Daft Punk - Derezzed.ogg"))
	//if (m_curMusic.openFromFile("Assets/Music/Tron_End_Titles.ogg"))
	if (m_curMusic.openFromFile("Assets/Music/Tyrian_Remix/technoguyx_Sarah's_Song_(remix).ogg"))
	{
		m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * m_Music_Activated);
		m_curMusic.play();
		m_curMusic.setLoop(true);
	}
}

void Game::SetSFXVolume(bool activate_sfx)
{
	soundsLaser[0].setVolume(DEFAULT_SFX_VOLUME * activate_sfx);
}

int Game::LoadSFX()
{
	if (!soundBuffers[0].loadFromFile("Assets/Sounds/laser.ogg"))
		return -1;

	soundsLaser[0].setBuffer(soundBuffers[0]);

	soundsLaser[0].setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);

	return 0;
}

void Game::PlaySFX(SFX_Bank sfx_name)
{
	if (sfx_name == SFX_Laser)
	{
		soundsLaser[0].play();
	}
}

sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}

void Game::SetPlayerShip(Ship* m_playerShip)
{
	this->playerShip = m_playerShip;
}

void Game::addToScene(GameObject *object, LayerType layer, GameObjectType type)
{
	object->m_layer = layer;
	object->m_collider_type = type;

	//Window resolution adjustements
	//object->setScale(scale_factor.x, scale_factor.y);

	if (((int)layer >= 0 && (int)layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_GameObject))
	{
		AddGameObjectToVector(object, &this->sceneGameObjectsTyped[(int)type]);
		AddGameObjectToVector(object, &this->sceneGameObjectsLayered[(int)layer]);
		AddGameObjectToVector(object, &this->sceneGameObjects);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add GameObject '%s' to layer '%d'", object->getName().c_str(), (int)layer));
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
	ResolveConstructionBufferList();//must be done after collision checks

	size_t sceneGameObjectsSize = this->sceneGameObjects.size();

	for (size_t i = 0; i < sceneGameObjectsSize; i++)
	{
		if (this->sceneGameObjects[i] == NULL)
			continue;

		this->sceneGameObjects[i]->update(deltaTime);
	}

	//Collect the dust
	collectGarbage();

	mainScreen.setView(view);
}

void Game::ResolveConstructionBufferList()
{
	size_t scenePlayerShipsVectorSize = this->sceneGameObjectsTyped[PlayerShip].size();
	for (size_t i = 0; i < scenePlayerShipsVectorSize; i++)
	{
		if (this->sceneGameObjectsTyped[PlayerShip][i] == NULL)
			continue;
		
		this->sceneGameObjectsTyped[PlayerShip][i]->ResolveConstructionBufferList();
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
	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[PlayerShip].begin(); it1 != sceneGameObjectsTyped[PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Player eating Fluxor
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[FluxorObject].begin(); it2 != sceneGameObjectsTyped[FluxorObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;
			
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 
				(*it1)->GetFluxor(*it2);
			}
		}

		//Player interacting with Module
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[ModuleObject].begin(); it2 != sceneGameObjectsTyped[ModuleObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 
				(*it1)->GetModule(*it2);
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

		// On remet à NULL lorsqu'on a trouvé un élément à dégager
		for (size_t j = 0; j < sceneGameObjectsSize; j++)
		{
			if (this->sceneGameObjects[j] == pCurGameObject)
			{
				this->sceneGameObjects[j] = NULL;
				break;
			}
		}

		// "layered"...
		const int layer = pCurGameObject->m_layer;
		for (size_t j = 0; j < sceneGameObjectsLayeredSize[layer]; j++)
		{
			if (this->sceneGameObjectsLayered[layer][j] == pCurGameObject)
			{
				this->sceneGameObjectsLayered[layer][j] = NULL;
				break;
			}
		}

		// "typed"
		const int type = pCurGameObject->m_collider_type;
		for (size_t j = 0; j < sceneGameObjectsTypedSize[type]; j++)
		{
			if (this->sceneGameObjectsTyped[type][j] == pCurGameObject)
			{
				this->sceneGameObjectsTyped[type][j] = NULL;
				break;
			}
		}

		//destructor function ??

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

bool Game::isCellFree(sf::Vector2f position)
{
	sf::Vector2u grid_index = GetGridIndex(position);

	return isCellFree(grid_index);
}

bool Game::isCellFree(sf::Vector2u grid_index)
{
	//for (std::vector<GameObject*>::iterator it = sceneGameObjectsTyped[FluxorObject].begin(); it != sceneGameObjectsTyped[FluxorObject].end(); it++)
	//{
	//	if (*it == NULL)
	//		continue;
	//
	//	if ((*it)->visible)
	//	{
	//		sf::Vector2u fluxor_grid_index = GetGridIndex((*it)->getPosition());
	//		if (fluxor_grid_index == grid_index)
	//		{
	//			return false;
	//		}
	//	}
	//}

	for (std::vector<GameObject*>::iterator it = sceneGameObjectsTyped[ModuleObject].begin(); it != sceneGameObjectsTyped[ModuleObject].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->visible)
		{
			sf::Vector2u module_grid_index = GetGridIndex((*it)->getPosition());
			if (module_grid_index == grid_index)
			{
				return false;
			}
		}
	}

	return true;
}

sf::Vector2u Game::GetGridIndex(sf::Vector2f position)
{
	unsigned int grid_line = (unsigned int)(ceil(1.f * position.x / TILE_SIZE));
	unsigned int grid_row = (unsigned int)(ceil(1.f * position.y / TILE_SIZE));

	return sf::Vector2u(grid_line, grid_row);
}

GameObject* Game::GetClosestObject(const sf::Vector2f position, GameObjectType type_of_closest_object)
{
	float shortest_distance = -1;
	GameObject* returned_obj = NULL;
	for (std::vector<GameObject*>::iterator it = sceneGameObjectsTyped[type_of_closest_object].begin(); it != sceneGameObjectsTyped[type_of_closest_object].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->isOnScene && !(*it)->ghost && (*it)->visible)
		{
			const float a = position.x - (*it)->getPosition().x;
			const float b = position.y - (*it)->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				returned_obj = (*it);
			}
		}
	}

	return returned_obj;
}

GameObject* Game::GetClosestObject(const GameObject* ref_obj, GameObjectType type_of_closest_object)
{
	const sf::Vector2f ref_position = ref_obj->getPosition();
	
	return GetClosestObject(ref_position, type_of_closest_object);
}

std::vector<GameObject*> Game::GetSceneGameObjectsTyped(GameObjectType type)
{
	return sceneGameObjectsTyped[type];
}