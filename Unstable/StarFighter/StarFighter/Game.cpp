#include "Game.h"
#define stringify(x)  #x

extern Game* CurrentGame;

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
	m_playerShip = NULL;
	m_pause = false;

	m_window = window;
	m_mainScreen.create(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y, false);
	m_mainScreen.setSmooth(true);

	m_scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	m_screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	m_view.setCenter(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	m_view.setSize(sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));
	//view.zoom(0.3f);

	//default value
	//GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	GameObject* background = new GameObject(sf::Vector2f(2240, 1536), sf::Vector2f(0, 0), "2D/city_old.jpg", sf::Vector2f(4480, 3072), sf::Vector2f(2240, 1536));
	addToScene(background, BackgroundLayer, BackgroundObject);
	m_map_size = background->m_size;
	//m_map_size = (sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));

	//fonts
	m_font[Font_Terminator] = new sf::Font();
	if (!m_font[Font_Terminator]->loadFromFile(makePath("Fonts/terminator_real_nfi.ttf")))
	{
		// error
		//TODO: font loader
	}

	m_font[Font_Arial] = new sf::Font();
	if (!m_font[Font_Arial]->loadFromFile(makePath("Fonts/arial.ttf")))
	{
		// error
		//TODO: font loader
	}

	//Sounds
	LoadSFX();
	m_SFX_Activated = true;

	//Music
	LOGGER_WRITE(Logger::DEBUG, "Loading Musics");
	m_Music_Activated = true;
	m_music_fader = 0;
	m_asking_music_fade_out = false;
	PlayMusic(Music_Main);

	//PICKPOCKETS SPECIFIC
	m_vision_cone_90 = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/vision_cone_90.png", sf::Vector2f(600, 600), sf::Vector2f(300, 300), 1, 1);

	//Pathfind
	const int tile_size = TILE_SIZE;
	const int nb_tile_rows = (int)m_map_size.x / tile_size;
	const int nb_tile_lines = (int)m_map_size.y / tile_size;
	TileType new_tile = NBVAL_TileTtype;
	m_tile_types.push_back(new_tile);
	for (size_t i = 0; i < nb_tile_lines * nb_tile_rows; i++)
	{
		new_tile = Tile_Street;
		m_tile_types.push_back(new_tile);
	}

	//Map
	//for (size_t i = 1; i < nb_tile_lines * nb_tile_rows + 1; i++)
	//{
	//	float posX = Game::GetTilePosX(i) * tile_size - tile_size / 2;
	//	float posY = Game::GetTilePosY(i) * tile_size - tile_size / 2;
	//	GameObject* new_tile = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/tile.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 1, 2);
	//	new_tile->setPosition(sf::Vector2f(posX, posY));
	//	new_tile->setAnimationLine(m_tile_types[i]);
	//	addToScene(new_tile, TileLayer, BackgroundObject);
	//}

	//m_tile_types[14] = Tile_Building;

	m_tile = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/tile.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 1, 2);
	addToScene(m_tile, TileLayer, BackgroundObject);
}

void Game::SetSFXVolume(bool activate_sfx)
{
	m_soundsLaser[0].setVolume(DEFAULT_SFX_VOLUME * activate_sfx);
}

int Game::LoadSFX()
{
	if (!m_soundBuffers[0].loadFromFile(makePath("Sounds/laser.ogg")))
		return -1;

	m_soundsLaser[0].setBuffer(m_soundBuffers[0]);

	m_soundsLaser[0].setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);
	//soundsSwitch.setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);

	return 0;
}

void Game::PlaySFX(SFX_Bank sfx_name)
{
	if (sfx_name == SFX_Laser)
	{
		m_soundsLaser[0].play();
	}
}

void Game::SetMusicVolume(bool activate_music)
{
	m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * activate_music);
}

void Game::PlayMusic(Music_Bank music, string specific_filename)
{
	if (music == m_curMusic_type)
	{
		return;
	}

	m_curMusic_type = music;

	if (!m_Music_Activated)
		return;

	//filename has been specified?
	if (!specific_filename.empty())
	{
		m_next_music_name = specific_filename;
	}
	//default musics
	else
	{
		//choose the right music file
		switch (music)
		{
			case Music_Main:
			{
				m_next_music_name = makePath("Music/Main.ogg");
				break;
			}
		}
	}

	m_asking_music_fade_out = true;
}

void Game::ManageMusicTransitions(sf::Time deltaTime)
{
	if (!m_Music_Activated)
		return;

	//fade out previous music
	if (m_asking_music_fade_out)
	{
		float volume_change = 100.f / MUSIC_FADE_OUT_TIME * deltaTime.asSeconds();
		if (m_music_fader - volume_change > 0)
		{
			m_music_fader -= volume_change;
		}
		else
		{
			m_music_fader = 0;
			//load new music
			m_curMusic.openFromFile(m_next_music_name);
			m_curMusic.play();
			m_curMusic.setLoop(true);
			m_asking_music_fade_out = false;
		}
		m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * m_Music_Activated * m_music_fader / 100);
	}

	//fade in new music
	if (!m_asking_music_fade_out && m_music_fader < 100)
	{
		float volume_change = 100.f / MUSIC_FADE_IN_TIME * deltaTime.asSeconds();
		if (m_music_fader + volume_change < 100)
		{
			m_music_fader += volume_change;
		}
		else
		{
			m_music_fader = 100;
		}
		m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * m_Music_Activated * m_music_fader / 100);
	}
}

sf::RenderWindow* Game::getMainWindow()
{
	return m_window;
}

void Game::addToScene(GameObject *object, LayerType layer, GameObjectType type)
{
	object->m_layer = layer;
	object->m_collider_type = type;

	//Window resolution adjustements
	//object->setScale(scale_factor.x, scale_factor.y);

	if (((int)layer >= 0 && (int)layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_GameObject))
	{
		AddGameObjectToVector(object, &this->m_sceneGameObjectsTyped[(int)type]);
		AddGameObjectToVector(object, &this->m_sceneGameObjectsLayered[(int)layer]);
		AddGameObjectToVector(object, &this->m_sceneGameObjects);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add GameObject '%s' to layer '%d'", object->getName().c_str(), (int)layer));
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

void Game::addToFeedbacks(SFPanel* panel)
{
	m_sceneFeedbackSFPanels.push_back(panel);
}

void Game::addToFeedbacks(SFText* text)
{
	if (text)
	{
		AddSFTextToVector(text, &this->m_sceneFeedbackSFTexts);
	}
}

void Game::removeFromFeedbacks(RectangleShape* feedback)
{
	m_sceneFeedbackBars.remove(feedback);
}

void Game::removeFromFeedbacks(Text* text)
{
	m_sceneFeedbackTexts.remove(text);
}

void Game::removeFromFeedbacks(SFPanel* panel)
{
	m_sceneFeedbackSFPanels.remove(panel);
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


	//Update of game objects
	size_t sceneGameObjectsSize = this->m_sceneGameObjects.size();
	for (size_t i = 0; i < sceneGameObjectsSize; i++)
	{
		if (this->m_sceneGameObjects[i] == NULL)
			continue;

		this->m_sceneGameObjects[i]->update(deltaTime);
	}

	//SFTextPop (text feedbacks)
	size_t sceneTextPopFeedbacksSize = m_sceneFeedbackSFTexts.size();
	for (size_t i = 0; i < sceneTextPopFeedbacksSize; i++)
	{
		if (m_sceneFeedbackSFTexts[i] == NULL)
			continue;

		m_sceneFeedbackSFTexts[i]->update(deltaTime);
	}

	//Collect the dust
	collectGarbage();

	//Update music transitions
	ManageMusicTransitions(deltaTime);

	//Update view
	m_mainScreen.setView(m_view);
}

void Game::drawScene()
{
	const int tile_size = TILE_SIZE;
	const int nb_tile_rows = (int)m_map_size.x / tile_size;
	const int nb_tile_lines = (int)m_map_size.y / tile_size;

	m_mainScreen.clear();

	for (int i = 0; i < NBVAL_Layer; i++)
	{
		if (i == FeedbacksLayer)
		{
			for (std::list<RectangleShape*>::iterator it = this->m_sceneFeedbackBars.begin(); it != this->m_sceneFeedbackBars.end(); it++)
			{
				if (*it == NULL)
					continue;

				m_mainScreen.draw(*(*it));
			}
			for (std::list<Text*>::iterator it = this->m_sceneFeedbackTexts.begin(); it != this->m_sceneFeedbackTexts.end(); it++)
			{
				if (*it == NULL)
					continue;

				m_mainScreen.draw(*(*it));
			}
			for (std::vector<SFText*>::iterator it = this->m_sceneFeedbackSFTexts.begin(); it != this->m_sceneFeedbackSFTexts.end(); it++)
			{
				if (*it == NULL)
					continue;

				if ((*(*it)).m_visible)
				{
					m_mainScreen.draw(*(*it));
				}
			}
		}
		else if (i == PanelLayer)
		{
			for (std::list<SFPanel*>::iterator it = this->m_sceneFeedbackSFPanels.begin(); it != this->m_sceneFeedbackSFPanels.end(); it++)
			{
				(*(*it)).Draw(m_mainScreen);
			}
		}
		else if (i == TileLayer)
		{
			for (size_t i = 1; i < nb_tile_lines * nb_tile_rows + 1; i++)
			{
				float posX = Game::GetTilePosX(i) * tile_size - tile_size / 2;
				float posY = Game::GetTilePosY(i) * tile_size - tile_size / 2;
				m_tile->setPosition(sf::Vector2f(posX, posY));
				m_tile->setAnimationLine(m_tile_types[i]);
				m_mainScreen.draw(*m_tile);
			}
			//const int tile_size = TILE_SIZE;
			//size_t tilesVectorSize = m_tile_types.size();
			//for (size_t t = 1; t < tilesVectorSize; t++)
			//{
			//	float posX = Game::GetTilePosX(t) * tile_size - tile_size / 2;
			//	float posY = Game::GetTilePosY(t) * tile_size - tile_size / 2;
			//	m_tile->setPosition(sf::Vector2f(posX, posY));
			//	if (m_tile->m_animationNumber < 0)
			//	{
			//		printf("bug");
			//	}
			//	else
			//	{
			//		printf("tuile_");
			//	}
			//	m_tile->setAnimationLine(m_tile_types[t]);
			//	m_mainScreen.draw(*m_tile);
			//}
		}
		else
		{
			for (std::vector<GameObject*>::iterator it = this->m_sceneGameObjectsLayered[i].begin(); it != this->m_sceneGameObjectsLayered[i].end(); it++)
			{
				if (*it == NULL)
					continue;

				if ((*(*it)).m_visible)
				{
					m_mainScreen.draw((*(*it)));
				}
			}
		}
	}

	m_mainScreen.display();
	sf::Sprite temp(m_mainScreen.getTexture());
	temp.scale(m_scale_factor.x, m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	m_window->draw(temp);
}

void Game::colisionChecksV2()
{
	sf::Clock dt;
	dt.restart();

	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[PlayerShip].begin(); it1 != m_sceneGameObjectsTyped[PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[AgentObject].begin(); it2 != m_sceneGameObjectsTyped[AgentObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;
			
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something
				(*it1)->Collide(*it2);
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
	size_t sceneGameObjectsTypedSize[NBVAL_GameObject];
	for (int layer = 0; layer < NBVAL_GameObject; layer++)
	{
		sceneGameObjectsTypedSize[layer] = m_sceneGameObjectsTyped[layer].size();
	}

	//Scene GameObjects
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
		for (size_t j = 0; j < sceneGameObjectsTypedSize[type]; j++)
		{
			if (m_sceneGameObjectsTyped[type][j] == pCurGameObject)
			{
				m_sceneGameObjectsTyped[type][j] = NULL;
				break;
			}
		}

		//destructor function ??

		// A la fin, on delete l'élément
		delete pCurGameObject;
	}

	//Texts and feedbacks
	size_t garbageTextsSize = m_garbageTexts.size();
	for (size_t i = 0; i < garbageTextsSize; i++)
	{
		SFText*    pSFText = m_garbageTexts[i];

		size_t VectorTextsSize = m_sceneFeedbackSFTexts.size();
		for (size_t j = 0; j < VectorTextsSize; j++)
		{
			if (m_sceneFeedbackSFTexts[j] == pSFText)
			{
				m_sceneFeedbackSFTexts[j] = NULL;
				break;
			}
		}

		delete pSFText;
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

void Game::AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] == NULL)
		{
			(*vector)[i] = pSFText;
			return; // ayé, on a trouvé un free slot, inséré, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas où on n'a pas trouvé de free slot => on rajoute à la fin
	vector->push_back(pSFText);
}

void Game::collectGarbage()
{
	sf::Clock dt;
	dt.restart();

	m_garbage.clear();
	m_garbageTexts.clear();

	for (std::vector<GameObject*>::iterator it = m_sceneGameObjects.begin(); it != m_sceneGameObjects.end(); it++)
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
			if (((**it).getPosition().x + ((**it).m_size.x) / 2 >= 0 && (**it).getPosition().x - ((**it).m_size.x) / 2 <= m_map_size.x) && ((**it).getPosition().y + ((**it).m_size.y) / 2 >= 0 && (**it).getPosition().y - ((**it).m_size.y) / 2 <= m_map_size.y))
			{
				(**it).m_isOnScene = true;
			}
		}

		//Content that went on scene and then exited have to be deleted
		if (!(**it).m_DontGarbageMe && (**it).m_isOnScene)
		{
			if ((**it).getPosition().x + ((**it).m_size.x) / 2 < 0 || (**it).getPosition().x - ((**it).m_size.x) / 2 > m_map_size.x
				|| (**it).getPosition().y + ((**it).m_size.y) / 2 < 0 || (**it).getPosition().y - ((**it).m_size.y) / 2 > m_map_size.y)
			{
				m_garbage.push_back(*it);
				continue;
			}
		}
	}

	//Texts and feedbacks
	for (std::vector<SFText*>::iterator it = m_sceneFeedbackSFTexts.begin(); it != m_sceneFeedbackSFTexts.end(); it++)
	{
		if (*it == NULL)
			continue;

		//Content flagged for deletion
		if ((**it).m_GarbageMe)
		{
			m_garbageTexts.push_back(*it);
			continue;
		}
	}

	//printf("| Collect: %d ",dt.getElapsedTime().asMilliseconds());
}

GameObject* Game::GetClosestObject(const sf::Vector2f position, GameObjectType type_of_closest_object)
{
	float shortest_distance = -1;
	GameObject* returned_obj = NULL;
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[type_of_closest_object].begin(); it != m_sceneGameObjectsTyped[type_of_closest_object].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->m_isOnScene && !(*it)->m_ghost && (*it)->m_visible)
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
	return m_sceneGameObjectsTyped[type];
}

void Game::CreateSFTextPop(string text, FontsStyle font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* follow_target, float offset_positionY)
{
	SFText* text_feedback = new SFText(m_font[font], 16, color, sf::Vector2f(position.x, position.y), team);
	SFTextPop* pop_feedback = new SFTextPop(text_feedback, distance_not_faded, distance_faded, total_pop_time, follow_target, offset_positionY);
	pop_feedback->setString(text);
	pop_feedback->setPosition(sf::Vector2f(pop_feedback->getPosition().x - pop_feedback->getGlobalBounds().width / 2, pop_feedback->getPosition().y));
	delete text_feedback;
	addToFeedbacks(pop_feedback);
}

float Game::GetCurrentMaxAwareness(GameObject* agent)
{
	float max = 0;
	size_t playersVectorSize = m_sceneGameObjectsTyped[PlayerShip].size();
	for (size_t i = 0; i < playersVectorSize; i++)
	{
		float cur = m_sceneGameObjectsTyped[PlayerShip][i]->GetCurrentAwareness(agent);
		if (max < cur)
		{
			max = cur;
		}
	}

	return max;
}

int Game::GetTilePosX(size_t tile_index)
{
	const int tile_size = TILE_SIZE;
	const int nb_tile_rows = (int)(*CurrentGame).m_map_size.x / tile_size;
	int pos_x = (tile_index - 1) % nb_tile_rows + 1;
	return pos_x;
}

int Game::GetTilePosY(size_t tile_index)
{
	const int tile_size = TILE_SIZE;
	const int nb_tile_rows = (int)(*CurrentGame).m_map_size.x / tile_size;
	int pos_y = (tile_index - 1) / nb_tile_rows + 1;
	return pos_y;
}

size_t Game::GetTileIndex(int pos_x, int pos_y)
{
	const int tile_size = TILE_SIZE;
	const int nb_tile_rows = (int)(*CurrentGame).m_map_size.x / tile_size;
	size_t index = pos_x;
	index += (pos_y - 1) * nb_tile_rows;
	return index;
}