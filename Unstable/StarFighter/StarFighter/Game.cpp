#include "Game.h"
#define stringify(x)  #x

const char* ColliderTypeValues[] =
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
				m_next_music_name = makePath("Music/03-TheSonofFlynn.ogg");
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

void Game::addToScene(GameObject *object, LayerType layer, ColliderType type)
{
	object->m_layer = layer;
	object->m_collider = type;

	//Window resolution adjustements
	//object->setScale(scale_factor.x, scale_factor.y);

	if (((int)layer >= 0 && (int)layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_GameObject))
	{
		AddGameObjectToVector(object, &m_sceneGameObjectsTyped[(int)type]);
		AddGameObjectToVector(object, &m_sceneGameObjectsLayered[(int)layer]);
		AddGameObjectToVector(object, &m_sceneGameObjects);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add GameObject '%s' to layer '%d'", object->GetTextureName().c_str(), (int)layer));
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
		AddSFTextToVector(text, &m_sceneFeedbackSFTexts);
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

void Game::changeObjectTypeAndLayer(GameObject *object, LayerType new_layer, ColliderType new_type)
{
	assert(((int)new_layer >= 0 && (int)new_layer < NBVAL_Layer) && (new_type >= 0 && new_type < NBVAL_GameObject));
	{
		m_sceneGameObjectsLayeredTemp[(int)new_layer].push_back(object);
		m_sceneGameObjectsTypedTemp[(int)new_type].push_back(object);
	}
}

void Game::UpdateScene(Time deltaTime)
{
	//TODO: Updating screen resolution
	m_scale_factor.x = 1.0f * m_screen_size.x / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * m_screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Checking colisions
	CheckCollisions();

	//Update GameObjects, interface and feedbacks
	UpdateObjects(deltaTime);

	//Update music transitions
	ManageMusicTransitions(deltaTime);

	//Update view
	m_mainScreen.setView(m_view);
}



void Game::UpdateObjects(Time deltaTime)
{
	//Update objects and delete "garbage" objects
	vector<GameObject*> temp_sceneGameObjects;
	for (GameObject* object : m_sceneGameObjects)
	{
		temp_sceneGameObjects.push_back(object);
	}
	m_sceneGameObjects.clear();
	for (GameObject* object : temp_sceneGameObjects)
	{
		if (object->m_garbageMe == true)
		{
			delete object;
		}
		else
		{
			object->Update(deltaTime);
			m_sceneGameObjects.push_back(object);
		}
	}

	size_t sceneGameObjectsSize = m_sceneGameObjects.size();
	for (size_t i = 0; i < sceneGameObjectsSize; i++)
	{
		if (m_sceneGameObjects[i] == NULL)
			continue;

		m_sceneGameObjects[i]->Update(deltaTime);
	}

	//SFTextPop (text feedbacks)
	size_t sceneTextPopFeedbacksSize = m_sceneFeedbackSFTexts.size();
	for (size_t i = 0; i < sceneTextPopFeedbacksSize; i++)
	{
		if (m_sceneFeedbackSFTexts[i] == NULL)
			continue;

		m_sceneFeedbackSFTexts[i]->Update(deltaTime);
	}
}

void Game::drawScene()
{
	m_mainScreen.clear();

	for (int i = 0; i < NBVAL_Layer; i++)
	{
		if (i == FeedbacksLayer)
		{
			for (std::list<RectangleShape*>::iterator it = m_sceneFeedbackBars.begin(); it != m_sceneFeedbackBars.end(); it++)
			{
				if (*it == NULL)
					continue;

				m_mainScreen.draw(*(*it));
			}
			for (std::list<Text*>::iterator it = m_sceneFeedbackTexts.begin(); it != m_sceneFeedbackTexts.end(); it++)
			{
				if (*it == NULL)
					continue;

				m_mainScreen.draw(*(*it));
			}
			for (std::vector<SFText*>::iterator it = m_sceneFeedbackSFTexts.begin(); it != m_sceneFeedbackSFTexts.end(); it++)
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
			for (std::list<SFPanel*>::iterator it = m_sceneFeedbackSFPanels.begin(); it != m_sceneFeedbackSFPanels.end(); it++)
			{
				(*(*it)).Draw(m_mainScreen);
			}
		}
		else
		{
			for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsLayered[i].begin(); it != m_sceneGameObjectsLayered[i].end(); it++)
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

void Game::CheckCollisions()
{
	sf::Clock dt;
	dt.restart();

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[ColliderType::PlayerShip].begin(); it1 != m_sceneGameObjectsTyped[ColliderType::PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Enemy bullets hitting the player
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[ColliderType::EnemyFire].begin(); it2 != m_sceneGameObjectsTyped[ColliderType::EnemyFire].end(); it2++)
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

void Game::TransferColliderTypedTempToSceneObjectsTyped(ColliderType collider)
{
	size_t current_index = 0;
	size_t current_object = 0;

	const size_t vectorSlaveSize = m_sceneGameObjectsTypedTemp[collider].size();
	for (size_t i = 0; i < vectorSlaveSize; i++)
	{
		if (m_sceneGameObjectsTypedTemp[collider][i] == NULL)
			continue;

		const size_t vectorMasterSize = m_sceneGameObjectsTyped[collider].size();
		for (size_t j = current_index; j < vectorMasterSize; j++)
		{
			if (m_sceneGameObjectsTyped[collider][j] == NULL)
			{
				//cut from old type
				ColliderType old_collider = m_sceneGameObjectsTypedTemp[collider][i]->m_collider;
				size_t previousVectorSize = m_sceneGameObjectsTyped[old_collider].size();
				for (size_t l = 0; l < previousVectorSize; l++)
				{
					if (m_sceneGameObjectsTyped[old_collider][l] == m_sceneGameObjectsTypedTemp[collider][i])
					{
						m_sceneGameObjectsTyped[old_collider][l] = NULL;
						continue;
					}
				}

				//then insert in new type
				m_sceneGameObjectsTypedTemp[collider][i]->m_collider = collider;
				m_sceneGameObjectsTyped[collider][j] = m_sceneGameObjectsTypedTemp[collider][i];
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
		ColliderType old_collider = m_sceneGameObjectsTypedTemp[collider][k]->m_collider;
		size_t previousVectorSize = m_sceneGameObjectsTyped[old_collider].size();
		for (size_t l = 0; l < previousVectorSize; l++)
		{
			if (m_sceneGameObjectsTyped[old_collider][l] == m_sceneGameObjectsTypedTemp[collider][k])
			{
				m_sceneGameObjectsTyped[old_collider][l] = NULL;
			}
		}

		//then insert in new type
		m_sceneGameObjectsTypedTemp[collider][k]->m_collider = collider;
		m_sceneGameObjectsTyped[collider].push_back(m_sceneGameObjectsTypedTemp[collider][k]);
	}
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
			return; // ay�, on a trouv� un free slot, ins�r�, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas o� on n'a pas trouv� de free slot => on rajoute � la fin
	vector->push_back(pSFText);
}

GameObject* Game::GetClosestObjectTyped(const GameObject* ref_obj, ColliderType type_of_closest_object, float dist_max, float angle_delta_max)
{
	const sf::Vector2f ref_position = ref_obj->getPosition();

	return GetClosestObjectTyped(ref_position, type_of_closest_object, dist_max, angle_delta_max);
}

GameObject* Game::GetClosestObjectTyped(const sf::Vector2f position, ColliderType type_of_closest_object, float dist_max, float angle_delta_max)
{
	float shortest_distance = -1;
	GameObject* returned_obj = NULL;
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[type_of_closest_object].begin(); it != m_sceneGameObjectsTyped[type_of_closest_object].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->m_visible == true)
		{
			const float a = position.x - (*it)->getPosition().x;
			const float b = position.y - (*it)->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				if (dist_max < 0 || distance_to_ref < dist_max)
				{
					float angle_delta = GetAngleDegToTargetPosition((*it)->getPosition(), (*it)->getRotation(), position);
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

std::vector<GameObject*> Game::GetSceneGameObjectsTyped(ColliderType type)
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

bool Game::AddStarSector(sf::Vector2i star_sector_index)
{
	for (sf::Vector2i index : m_star_sectors_known)
	{
		if (star_sector_index == index)
			return false;
	}

	m_star_sectors_known.push_back(star_sector_index);
	return true;
}