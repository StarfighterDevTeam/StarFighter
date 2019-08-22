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

	//background
	m_background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Black, sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));
	m_background->setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
	addToScene(m_background, BackgroundLayer, BackgroundObject);

	//Star Hunter
	m_current_star_sector.m_index = sf::Vector2i(0, 0);
	m_current_star_sector.m_status = Sector_Current;
	AddToStarSectorsKnown(m_current_star_sector);
	UpdateSectorList(true);

	//DEBUG
	m_sector_debug_current = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Blue, sf::Vector2f(STAR_SECTOR_SIZE, STAR_SECTOR_SIZE), 3);
	m_sector_debug_onscreen = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Green, sf::Vector2f(STAR_SECTOR_SIZE, STAR_SECTOR_SIZE), 3);
	m_sector_debug_incoming = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Magenta, sf::Vector2f(STAR_SECTOR_SIZE, STAR_SECTOR_SIZE), 3);
	m_sector_debug_far = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Red, sf::Vector2f(STAR_SECTOR_SIZE, STAR_SECTOR_SIZE), 3);

}

Game::~Game()
{
	delete m_background;



	delete m_sector_debug_current;
	delete m_sector_debug_onscreen;
	delete m_sector_debug_incoming;
	delete m_sector_debug_far;
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

	if (((int)layer >= 0 && (int)layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_ColliderType))
	{
		m_sceneGameObjectsTyped[(int)type].push_back(object);
		m_sceneGameObjectsLayered[(int)layer].push_back(object);
		m_sceneGameObjects.push_back(object);
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

void Game::UpdateScene(Time deltaTime)
{
	//TODO: Updating screen resolution
	m_scale_factor.x = 1.0f * m_screen_size.x / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * m_screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Update quad-tree classification of objects
	UpdateSectorList();

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
	GameObject* player = (GameObject*)m_playerShip;

	//Update objects and delete "garbage" objects
	vector<GameObject*> temp_sceneGameObjects;
	for (GameObject* object : m_sceneGameObjects)
		temp_sceneGameObjects.push_back(object);
	m_sceneGameObjects.clear();
	for (int i = 0; i < NBVAL_Layer; i++)
		m_sceneGameObjectsLayered[i].clear();
	for (int i = 0; i < NBVAL_ColliderType; i++)
		m_sceneGameObjectsTyped[i].clear();

	for (GameObject* object : temp_sceneGameObjects)
	{
		if (object->m_garbageMe == true)
		{
			delete object;
		}
		else if (object->m_removeMe == false)//if true, it has been stored in m_sceneGameObjectsStored already, therefore there is no memory leak if we don't push it back
		{
			object->Update(deltaTime);

			if (object == player || object == m_background)
				object->setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
			else
				object->setPosition(sf::Vector2f(object->m_position.x - player->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, object->m_position.y - (- player->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));

			addToScene(object, object->m_layer, object->m_collider);
		}
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
		if (i == SectorLayer)
		{
			//DEBUG
			GameObject* player = (GameObject*)m_playerShip;
			
			for (StarSector sector : m_star_sectors_managed)
			{
				GameObject* sector_debug;
				switch (sector.m_status)
				{
				case Sector_Current:
					sector_debug = m_sector_debug_current;
					break;
				case Sector_OnScreen:
					sector_debug = m_sector_debug_onscreen;
					break;
				case Sector_Incoming:
					sector_debug = m_sector_debug_incoming;
					break;
				case Sector_Far:
					sector_debug = m_sector_debug_far;
					break;
				}

				sector_debug->SetStarSectorIndex(sector.m_index);
				sector_debug->setPosition(sf::Vector2f(sector_debug->m_position.x - player->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, - (sector_debug->m_position.y - player->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));

				m_mainScreen.draw(*sector_debug);
			}
		}
		else
		{
			for (GameObject* it : m_sceneGameObjectsLayered[i])
			//for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsLayered[i].begin(); it != m_sceneGameObjectsLayered[i].end(); it++)
			{
				if (it->m_visible == true)
				{
					m_mainScreen.draw(*it);
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
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[ColliderType::PlayerShipObject].begin(); it1 != m_sceneGameObjectsTyped[ColliderType::PlayerShipObject].end(); it1++)
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

bool Game::AddToStarSectorsKnown(StarSector sector)
{
	return AddToStarSectorsKnown(sector.m_index, sector.m_status);
}

bool Game::AddToStarSectorsKnown(sf::Vector2i star_sector_index, StarSectorStatus status)
{
	for (StarSector sector : m_star_sectors_known)
	{
		if (sector.m_index == star_sector_index)
		{
			sector.m_status = status;
			return false;
		}
	}

	m_star_sectors_known.push_back(StarSector(star_sector_index, status));

	return true;
}

bool Game::CreateNewSector(sf::Vector2i star_sector_index)
{
	//TODO: create sector procedural content
	//printf("Sector %d, %d created (nb of sectors known: %d).\n", star_sector_index.x, star_sector_index.y, m_star_sectors_known.size());
	return true;
}

void Game::UpdateSectorList(bool force_update)
{
	//update needed?
	GameObject* player = (GameObject*)m_playerShip;
	if (true)// force_update == true || m_current_star_sector.m_index != player->m_star_sector_index)
	{
		if (player != NULL)
			m_current_star_sector.m_index = player->m_star_sector_index;

		vector<StarSector> old_star_sectors_managed;
		for (StarSector sector : m_star_sectors_managed)
			old_star_sectors_managed.push_back(sector);
		m_star_sectors_managed.clear();
		
		//printf(">>> SECTORS UPDATE !!\n");
		int nb_sectors_x = (REF_WINDOW_RESOLUTION_X / STAR_SECTOR_SIZE) + 3;
		int nb_sectors_y = (REF_WINDOW_RESOLUTION_Y / STAR_SECTOR_SIZE) + 3;
		for (int i = 0; i < nb_sectors_x; i++)
		{
			for (int j = 0; j < nb_sectors_y; j++)
			{
				StarSector sector(sf::Vector2i(i + m_current_star_sector.m_index.x - (nb_sectors_x / 2), j + m_current_star_sector.m_index.y - (nb_sectors_y / 2)), (i == nb_sectors_x / 2 && j == nb_sectors_y / 2) ? Sector_Current : Sector_OnScreen);
				m_star_sectors_managed.push_back(sector);
				
				if (AddToStarSectorsKnown(sector) == true)
				{
					CreateNewSector(sector.m_index);//new sector discovered needs to be created
				}

				//printf("i: %d, j: %d", i + m_current_star_sector.m_index.x - (nb_sectors_x / 2), j + m_current_star_sector.m_index.y - (nb_sectors_y / 2));
				//if (i == nb_sectors_x / 2 && j == nb_sectors_y / 2)
				//	printf(" current");
				//printf("\n");
			}
		}

		//delta between old and new manageable sectors
		vector<sf::Vector2i> sector_index_to_add;
		vector<sf::Vector2i> sector_index_to_remove;
		for (StarSector old_sector : old_star_sectors_managed)
		{
			bool found = false;
			for (StarSector new_sector : m_star_sectors_known)
			{
				if (old_sector.m_index == new_sector.m_index)
				{
					found = true;
					break;
				}
			}

			if (found == false)
			{
				sector_index_to_remove.push_back(old_sector.m_index);
			}
		}

		for (StarSector new_sector : m_star_sectors_known)
		{
			bool found = false;
			for (StarSector old_sector : old_star_sectors_managed)
			{
				if (old_sector.m_index == new_sector.m_index)
				{
					found = true;
					break;
				}
			}

			if (found == false)
			{
				sector_index_to_add.push_back(new_sector.m_index);
			}
		}


		/*
		//Sectors to remove from scene
		for (sf::Vector2i index : sector_index_to_remove)
		{
			vector<GameObject*> vector_objects;
			for (GameObject* object : m_sceneGameObjects)
			{
				if (object->m_star_sector_index == index && object->m_garbageMe == false)
				{
					if (object->m_collider != EnemyFire && object->m_collider != PlayerFire)//temporary objects such as flying ammunition don't need to be stored, they can be deleted in the process
					{
						object->m_removeMe = true;
						vector_objects.push_back(object);
					}
					else
						object->m_garbageMe = true;
				}
			}

			//m_sceneGameObjectsStored.insert(pair<Vector2i, vector<GameObject*> >(index, vector_objects));
		}
		
		//Sectors to insert insert into scene
		for (sf::Vector2i index : sector_index_to_add)
		{
			//for (GameObject* object : m_sceneGameObjectsStored[index])
			//	addToScene(object, object->m_layer, object->m_collider);

			//m_sceneGameObjectsStored.erase(index);
		}
		*/
	}
}