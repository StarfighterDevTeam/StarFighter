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

Game::Game(RenderWindow* window)
{
	m_playerShip = NULL;
	m_pause = false;
	m_window_has_focus = true;

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
	m_map_size = (sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));

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
	m_SFX_Activated = true;
	LoadSFX();

	//Music
	printf("Loading Musics");
	m_Music_Activated = true;
	m_music_fader = 0;
	PlayMusic(Music_Main);
}

void Game::SetSFXVolume(bool activate_sfx)
{
	m_sounds[0].setVolume(DEFAULT_SFX_VOLUME * activate_sfx);
}

int Game::LoadSFX()
{
	if (!m_soundBuffers[0].loadFromFile(makePath("Sounds/laser.ogg")))
		return -1;

	m_sounds[0].setBuffer(m_soundBuffers[0]);

	m_sounds[0].setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);
	//soundsSwitch.setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);

	return 0;
}

void Game::PlaySFX(SFX_Bank sfx_name)
{
	if (m_SFX_Activated)
		m_sounds[sfx_name].play();
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

void Game::changeObjectTypeAndLayer(GameObject *object, LayerType new_layer, GameObjectType new_type)
{
	assert(((int)new_layer >= 0 && (int)new_layer < NBVAL_Layer) && (new_type >= 0 && new_type < NBVAL_GameObject));
	{
		m_sceneGameObjectsLayeredTemp[(int)new_layer].push_back(object);
		m_sceneGameObjectsTypedTemp[(int)new_type].push_back(object);
	}
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

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[GameObjectType::PlayerShip].begin(); it1 != m_sceneGameObjectsTyped[GameObjectType::PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Enemy bullets hitting the player
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[GameObjectType::EnemyFire].begin(); it2 != m_sceneGameObjectsTyped[GameObjectType::EnemyFire].end(); it2++)
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

void Game::TransferGameObjectLayeredTempToSceneObjectsLayered(LayerType layer)
{
	size_t current_index = 0;
	size_t current_object = 0;

	const size_t vectorSlaveSize = m_sceneGameObjectsLayeredTemp[layer].size();
	for (size_t i = 0; i < vectorSlaveSize; i++)
	{
		if (m_sceneGameObjectsLayeredTemp[layer][i] == NULL)
			continue;

		const size_t vectorMasterSize = m_sceneGameObjectsLayered[layer].size();
		for (size_t j = current_index; j < vectorMasterSize; j++)
		{
			//found a free slot
			if (m_sceneGameObjectsLayered[layer][j] == NULL)
			{
				//cut from old layer
				LayerType old_layer = m_sceneGameObjectsLayeredTemp[layer][i]->m_layer;
				size_t previousVectorSize = m_sceneGameObjectsLayered[old_layer].size();
				for (size_t l = 0; l < previousVectorSize; l++)
				{
					if (m_sceneGameObjectsLayered[old_layer][l] == m_sceneGameObjectsLayeredTemp[layer][i])
					{
						m_sceneGameObjectsLayered[old_layer][l] = NULL;
					}
				}

				//then insert in new layer
				m_sceneGameObjectsLayeredTemp[layer][i]->m_layer = layer;
				m_sceneGameObjectsLayered[layer][j] = m_sceneGameObjectsLayeredTemp[layer][i];
				current_index = j + 1;
				current_object++;

				break;
			}
		}

		break;
	}

	for (size_t k = current_object; k < vectorSlaveSize; k++)
	{
		//cut from old layer
		LayerType old_layer = m_sceneGameObjectsLayeredTemp[layer][k]->m_layer;
		size_t previousVectorSize = m_sceneGameObjectsLayered[old_layer].size();
		for (size_t l = 0; l < previousVectorSize; l++)
		{
			if (m_sceneGameObjectsLayered[old_layer][l] == m_sceneGameObjectsLayeredTemp[layer][k])
			{
				m_sceneGameObjectsLayered[old_layer][l] = NULL;
				continue;
			}
		}

		//then insert in new layer
		m_sceneGameObjectsLayeredTemp[layer][k]->m_layer = layer;
		m_sceneGameObjectsLayered[layer].push_back(m_sceneGameObjectsLayeredTemp[layer][k]);
	}
}

void Game::TransferGameObjectTypedTempToSceneObjectsTyped(GameObjectType collider_type)
{
	size_t current_index = 0;
	size_t current_object = 0;

	const size_t vectorSlaveSize = m_sceneGameObjectsTypedTemp[collider_type].size();
	for (size_t i = 0; i < vectorSlaveSize; i++)
	{
		if (m_sceneGameObjectsTypedTemp[collider_type][i] == NULL)
			continue;

		const size_t vectorMasterSize = m_sceneGameObjectsTyped[collider_type].size();
		for (size_t j = current_index; j < vectorMasterSize; j++)
		{
			if (m_sceneGameObjectsTyped[collider_type][j] == NULL)
			{
				//cut from old type
				GameObjectType old_collider_type = m_sceneGameObjectsTypedTemp[collider_type][i]->m_collider_type;
				size_t previousVectorSize = m_sceneGameObjectsTyped[old_collider_type].size();
				for (size_t l = 0; l < previousVectorSize; l++)
				{
					if (m_sceneGameObjectsTyped[old_collider_type][l] == m_sceneGameObjectsTypedTemp[collider_type][i])
					{
						m_sceneGameObjectsTyped[old_collider_type][l] = NULL;
						continue;
					}
				}

				//then insert in new type
				m_sceneGameObjectsTypedTemp[collider_type][i]->m_collider_type = collider_type;
				m_sceneGameObjectsTyped[collider_type][j] = m_sceneGameObjectsTypedTemp[collider_type][i];
				current_index = j + 1;
				current_object++;

				break;
			}
		}

		break;
	}

	for (size_t k = current_object; k < vectorSlaveSize; k++)
	{
		//cut from old type
		GameObjectType old_collider_type = m_sceneGameObjectsTypedTemp[collider_type][k]->m_collider_type;
		size_t previousVectorSize = m_sceneGameObjectsTyped[old_collider_type].size();
		for (size_t l = 0; l < previousVectorSize; l++)
		{
			if (m_sceneGameObjectsTyped[old_collider_type][l] == m_sceneGameObjectsTypedTemp[collider_type][k])
			{
				m_sceneGameObjectsTyped[old_collider_type][l] = NULL;
			}
		}

		//then insert in new type
		m_sceneGameObjectsTypedTemp[collider_type][k]->m_collider_type = collider_type;
		m_sceneGameObjectsTyped[collider_type].push_back(m_sceneGameObjectsTypedTemp[collider_type][k]);
	}
}

void Game::cleanGarbage()
{
	sf::Clock dt;
	dt.restart();

	// On traite les demandes de changements de type
	size_t sceneGameObjectsLayeredTempSize[NBVAL_Layer];
	for (int layer = 0; layer < NBVAL_Layer; layer++)
	{
		TransferGameObjectLayeredTempToSceneObjectsLayered((LayerType)layer);
		m_sceneGameObjectsLayeredTemp[layer].clear();
	}

	size_t sceneGameObjectsTypedTempSize[NBVAL_GameObject];
	for (int type = 0; type < NBVAL_GameObject; type++)
	{
		TransferGameObjectTypedTempToSceneObjectsTyped((GameObjectType)type);
		m_sceneGameObjectsTypedTemp[type].clear();
	}

	// On mémorise les size, pour éviter d'appeler des fonctions à chaque itération
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

void Game::AddGameObjectVectorToVector(vector<GameObject*> vector_slave, vector<GameObject*>* vector_master)
{
	size_t current_index = 0;
	size_t current_object = 0;

	const size_t vectorSlaveSize = vector_slave.size();
	for (size_t i = 0; i < vectorSlaveSize; i++)
	{
		const size_t vectorMasterSize = vector_master->size();
		for (size_t j = current_index; j < vectorMasterSize; j++)
		{
			if ((*vector_master)[j] == NULL)
			{
				(*vector_master)[j] = vector_slave[i];
				current_index = j + 1;
				current_object++;

				break;
			}
		}

		break;
	}

	for (size_t k = current_object; k < vectorSlaveSize; k++)
	{
		vector_master->push_back(vector_slave[k]);
	}
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

GameObject* Game::GetClosestObjectTyped(const GameObject* ref_obj, GameObjectType type_of_closest_object, float dist_max, float angle_delta_max)
{
	const sf::Vector2f ref_position = ref_obj->getPosition();

	return GetClosestObjectTyped(ref_position, type_of_closest_object, dist_max, angle_delta_max);
}

GameObject* Game::GetClosestObjectTyped(const sf::Vector2f position, GameObjectType type_of_closest_object, float dist_max, float angle_delta_max)
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
				if (dist_max < 0 || distance_to_ref < dist_max)
				{
					float angle_delta = GameObject::GetAngleDegToTargetPosition((*it)->getPosition(), (*it)->getRotation(), position);
					if (angle_delta < 0)
						angle_delta = -angle_delta;

					if (angle_delta_max < 0 || angle_delta < angle_delta_max)
					{
						shortest_distance = distance_to_ref;
						returned_obj = (*it);
					}
				}

				shortest_distance = distance_to_ref;
				returned_obj = (*it);
			}
		}
	}

	return returned_obj;
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